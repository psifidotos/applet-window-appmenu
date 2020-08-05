/******************************************************************
 * Copyright 2016 Kai Uwe Broulik <kde@privat.broulik.de>
 * Copyright 2016 Chinmoy Ranjan Pradhan <chinmoyrp65@gmail.com>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************/

#include "appmenumodel.h"
#include <config-appmenu.h>
#include <dbusmenuimporter.h>

// local
#include "wm/waylandwindowmanager.h"
#include "wm/x11fallbackwindowmanager.h"

// Qt
#include <QAction>
#include <QDebug>
#include <QMenu>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QGuiApplication>

// KDE
#include <KWindowSystem>


class KDBusMenuImporter : public DBusMenuImporter
{

public:
    KDBusMenuImporter(const QString &service, const QString &path, QObject *parent)
        : DBusMenuImporter(service, path, parent) {

    }

protected:
    QIcon iconForName(const QString &name) override {
        return QIcon::fromTheme(name);
    }

};

AppMenuModel::AppMenuModel(QObject *parent)
    : QAbstractListModel(parent),
      m_serviceWatcher(new QDBusServiceWatcher(this))
{
#if LibTaskManager_CURRENTMINOR_VERSION < 19
    // Disable for Plasma Desktop < 5.19
    if (KWindowSystem::isPlatformWayland()) {
        return;
    }
#endif

    initWM();

    connect(this, &AppMenuModel::modelNeedsUpdate, this, [this] {
        if (!m_updatePending)
        {
            m_updatePending = true;
            QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
        }
    });

    m_serviceWatcher->setConnection(QDBusConnection::sessionBus());
    //if our current DBus connection gets lost, close the menu
    //we'll select the new menu when the focus changes
    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, [this](const QString & serviceName) {
        if (serviceName == m_serviceName) {
            m_wm->setMenuAvailable(false);
            emit modelNeedsUpdate();
        }
    });
}

AppMenuModel::~AppMenuModel()
{
    for (const auto &var : m_wmconnections) {
        QObject::disconnect(var);
    }
}

bool AppMenuModel::filterByActive() const
{
    return m_wm && m_wm->filterByActive();
}

void AppMenuModel::setFilterByActive(bool active)
{
    if (m_wm) {
        m_wm->setFilterByActive(active);
    }
}

bool AppMenuModel::filterChildren() const
{
    return m_wm && m_wm->filterChildren();
}

void AppMenuModel::setFilterChildren(bool hideChildren)
{
    if (m_wm) {
        m_wm->setFilterChildren(hideChildren);
    }
}


bool AppMenuModel::menuAvailable() const
{
    return m_wm && m_wm->menuAvailable();
}

void AppMenuModel::setMenuAvailable(bool set)
{
    if (m_wm) {
        m_wm->setMenuAvailable(set);
    }
}

QRect AppMenuModel::screenGeometry() const
{
    return m_wm ? m_wm->screenGeometry() : QRect();
}

void AppMenuModel::setScreenGeometry(QRect geometry)
{
    if (m_wm) {
        m_wm->setScreenGeometry(geometry);
    }
}

bool AppMenuModel::visible() const
{
    return m_wm && m_wm->visible();
}

QVariant AppMenuModel::winId() const
{
    return m_wm ? m_wm->winId() : -1;
}

void AppMenuModel::setWinId(const QVariant &id)
{
    if (m_wm) {
        m_wm->setWinId(id);
    }
}

void AppMenuModel::initWM()
{

    if (KWindowSystem::isPlatformX11()) {
        m_wm = new WM::X11FallbackWindowManager(this);
    } else {
        m_wm = new WM::WaylandWindowManager(this);
    }

    m_wmconnections << connect(m_wm, &WM::AbstractWindowManager::modelNeedsUpdate, this, &AppMenuModel::modelNeedsUpdate);
    m_wmconnections << connect(m_wm, &WM::AbstractWindowManager::applicationMenuChanged, this, &AppMenuModel::updateApplicationMenu);

    m_wmconnections << connect(m_wm, &WM::AbstractWindowManager::menuAvailableChanged, this, &AppMenuModel::menuAvailableChanged );
    m_wmconnections << connect(m_wm, &WM::AbstractWindowManager::filterByActiveChanged, this, &AppMenuModel::filterByActiveChanged);
    m_wmconnections << connect(m_wm, &WM::AbstractWindowManager::filterChildrenChanged, this, &AppMenuModel::filterChildrenChanged);
    m_wmconnections << connect(m_wm, &WM::AbstractWindowManager::visibleChanged, this, &AppMenuModel::visibleChanged );
    m_wmconnections << connect(m_wm, &WM::AbstractWindowManager::screenGeometryChanged, this, &AppMenuModel::screenGeometryChanged);
    m_wmconnections << connect(m_wm, &WM::AbstractWindowManager::winIdChanged, this, &AppMenuModel::winIdChanged);
}


int AppMenuModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (!m_wm || !m_wm->menuAvailable() || !m_menu) {
        return 0;
    }

    return m_menu->actions().count();
}

void AppMenuModel::update()
{
    beginResetModel();
    endResetModel();
    m_updatePending = false;
}

QHash<int, QByteArray> AppMenuModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[MenuRole] = QByteArrayLiteral("activeMenu");
    roleNames[ActionRole] = QByteArrayLiteral("activeActions");
    return roleNames;
}

QVariant AppMenuModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    if (row < 0 || !m_wm || !m_wm->menuAvailable() || !m_menu) {
        return QVariant();
    }

    const auto actions = m_menu->actions();

    if (row >= actions.count()) {
        return QVariant();
    }

    if (role == MenuRole) { // TODO this should be Qt::DisplayRole
        return actions.at(row)->text();
    } else if (role == ActionRole) {
        return QVariant::fromValue((void *) actions.at(row));
    }

    return QVariant();
}

void AppMenuModel::updateApplicationMenu(const QString &serviceName, const QString &menuObjectPath)
{
    if (m_serviceName == serviceName && m_menuObjectPath == menuObjectPath) {
        if (m_importer) {
            QMetaObject::invokeMethod(m_importer, "updateMenu", Qt::QueuedConnection);
        }

        return;
    }

    m_serviceName = serviceName;
    m_serviceWatcher->setWatchedServices(QStringList({m_serviceName}));

    m_menuObjectPath = menuObjectPath;

    if (m_importer) {
        m_importer->deleteLater();
    }

    m_importer = new KDBusMenuImporter(serviceName, menuObjectPath, this);
    QMetaObject::invokeMethod(m_importer, "updateMenu", Qt::QueuedConnection);

    connect(m_importer.data(), &DBusMenuImporter::menuUpdated, this, [ = ](QMenu * menu) {
        m_menu = m_importer->menu();

        if (m_menu.isNull() || menu != m_menu) {
            return;
        }

        //cache first layer of sub menus, which we'll be popping up
        for (QAction *a : m_menu->actions()) {
            // signal dataChanged when the action changes
            connect(a, &QAction::changed, this, [this, a] {
                if (m_wm && m_wm->menuAvailable() && m_menu)
                {
                    const int actionIdx = m_menu->actions().indexOf(a);

                    if (actionIdx > -1) {
                        const QModelIndex modelIdx = index(actionIdx, 0);
                        emit dataChanged(modelIdx, modelIdx);
                    }
                }
            });

            connect(a, &QAction::destroyed, this, &AppMenuModel::modelNeedsUpdate);

            if (a->menu()) {
                m_importer->updateMenu(a->menu());
            }
        }

        m_wm->setMenuAvailable(true);
        emit modelNeedsUpdate();
    });

    connect(m_importer.data(), &DBusMenuImporter::actionActivationRequested, this, [this](QAction * action) {
        // TODO submenus
        if (!m_wm || !m_wm->menuAvailable() || !m_menu) {
            return;
        }

        const auto actions = m_menu->actions();
        auto it = std::find(actions.begin(), actions.end(), action);

        if (it != actions.end()) {
            requestActivateIndex(it - actions.begin());
        }
    });
}
