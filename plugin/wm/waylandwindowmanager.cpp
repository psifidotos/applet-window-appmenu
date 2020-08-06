/*
*  Copyright 2020 Michail Vourlakos <mvourlakos@gmail.com>
*
*  This file is part of Latte-Dock
*
*  Latte-Dock is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of
*  the License, or (at your option) any later version.
*
*  Latte-Dock is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "waylandwindowmanager.h"
#include <config-appmenu.h>

//Qt
#include <QDebug>
#include <QModelIndex>

// KDE
#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/registry.h>
#include <KWindowSystem>

// Plasma
#include <taskmanager/abstracttasksmodel.h>

#define DELAYEDMENUTIMER 2000

namespace WM {

WaylandWindowManager::WaylandWindowManager(QObject *parent)
    : AbstractWindowManager(parent)
{
#if KF5_CURRENTMINOR_VERSION < 69
    // Disable for KF5::Wayland < 5.69
    if (KWindowSystem::isPlatformWayland()) {
        return;
    }
#endif

    m_tasksModel = new TaskManager::TasksModel(this);

    setupWaylandIntegration();

    m_delayedApplicationMenuTimer.setSingleShot(true);
    m_delayedApplicationMenuTimer.setInterval(DELAYEDMENUTIMER);
    connect(&m_delayedApplicationMenuTimer, &QTimer::timeout, this, &WaylandWindowManager::onDelayedTimerTriggered);

    m_tasksModel->setFilterByScreen(true);
    connect(m_tasksModel, &TaskManager::TasksModel::activeTaskChanged, this, &WaylandWindowManager::onActiveWindowChanged);
    connect(m_tasksModel, &TaskManager::TasksModel::activityChanged, this, &WaylandWindowManager::onActiveWindowChanged);
    connect(m_tasksModel, &TaskManager::TasksModel::virtualDesktopChanged, this, &WaylandWindowManager::onActiveWindowChanged);
    connect(m_tasksModel, &TaskManager::TasksModel::countChanged, this, &WaylandWindowManager::onActiveWindowChanged);

    connect(this, &AbstractWindowManager::screenGeometryChanged, this, [this] {
        m_tasksModel->setScreenGeometry(m_screenGeometry);
    });

    connect(this, &AbstractWindowManager::winIdChanged, this, &WaylandWindowManager::onWinIdChanged);
}

WaylandWindowManager::~WaylandWindowManager()
{
}

void WaylandWindowManager::setupWaylandIntegration()
{
    if (!KWindowSystem::isPlatformWayland()) {
        return;
    }

    using namespace KWayland::Client;
    auto connection = ConnectionThread::fromApplication(this);

    if (!connection) {
        return;
    }

    Registry *registry{new Registry(this)};
    registry->create(connection);

    connect(registry, &Registry::plasmaShellAnnounced, this
            , [this, registry](quint32 name, quint32 version) {
        m_waylandShell = registry->createPlasmaShell(name, version, this);
    });

    QObject::connect(registry, &KWayland::Client::Registry::plasmaWindowManagementAnnounced,
                     [this, registry](quint32 name, quint32 version) {
        m_windowManagement = registry->createPlasmaWindowManagement(name, version, this);
    });

    registry->setup();
    connection->roundtrip();
}

KWayland::Client::PlasmaWindow *WaylandWindowManager::windowFor(QVariant wid)
{
    auto it = std::find_if(m_windowManagement->windows().constBegin(), m_windowManagement->windows().constEnd(), [&wid](KWayland::Client::PlasmaWindow * w) noexcept {
            return w->isValid() && w->internalId() == wid.toUInt();
    });

    if (it == m_windowManagement->windows().constEnd()) {
        return nullptr;
    }

    return *it;
}

void WaylandWindowManager::onActiveWindowChanged()
{
    if (hasUserWindowId()) {
        return;
    }

#if LibTaskManager_CURRENTMINOR_VERSION >= 19
    const QModelIndex activeTaskIndex = m_tasksModel->activeTask();
    const QString objectPath = m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::ApplicationMenuObjectPath).toString();
    const QString serviceName = m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::ApplicationMenuServiceName).toString();

    validateApplicationMenu(objectPath, serviceName);
#endif
}

void WaylandWindowManager::onWinIdChanged()
{
    if (!m_windowManagement || !hasUserWindowId()) {
        return;
    }

#if KF5_CURRENTMINOR_VERSION >= 69
    auto window = windowFor(m_userWindowId);

    if (window) {
        const QString objectPath = window->applicationMenuObjectPath();
        const QString serviceName = window->applicationMenuServiceName();

        validateApplicationMenu(objectPath, serviceName);
    }
#endif
}

void WaylandWindowManager::validateApplicationMenu(const QString &objectPath, const QString &serviceName)
{
    if (!objectPath.isEmpty() && !serviceName.isEmpty()) {
        setMenuAvailable(true);
        emit applicationMenuChanged(serviceName, objectPath);
        setVisible(true);
        emit modelNeedsUpdate();
    } else {
        if (m_delayedMenuWindowId.toInt()<0) {
            m_delayedMenuWindowId = hasUserWindowId() ? m_userWindowId : 1/*flag case*/;
            m_delayedApplicationMenuTimer.start();
        }

        setMenuAvailable(false);
        setVisible(false);
    }
}

void WaylandWindowManager::onDelayedTimerTriggered()
{
    if (hasUserWindowId()) {
        onWinIdChanged();
    } else {
        onActiveWindowChanged();
    }

    m_delayedMenuWindowId = -1;
}

}
