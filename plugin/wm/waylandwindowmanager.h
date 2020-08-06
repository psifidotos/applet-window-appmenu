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

#ifndef WAYLANDWINDOWMANAGER_H
#define WAYLANDWINDOWMANAGER_H

//local
#include "abstractwindowmanager.h"

//Qt
#include <QObject>
#include <QPointer>
#include <QTimer>

// KDE
#include <KWayland/Client/plasmashell.h>
#include <KWayland/Client/plasmawindowmanagement.h>

// Plasma TaskManager
#include <taskmanager/tasksmodel.h>

namespace WM {

class WaylandWindowManager : public AbstractWindowManager
{
    Q_OBJECT

public:
    explicit WaylandWindowManager(QObject *parent = nullptr);
    ~WaylandWindowManager() override;

private slots:
    void onActiveWindowChanged();
    void onDelayedTimerTriggered();
    void onWinIdChanged();

private:
    void setupWaylandIntegration();
    void validateApplicationMenu(const QString &objectPath, const QString &serviceName);


    KWayland::Client::PlasmaWindow *windowFor(QVariant wid);

private:
    //! window that its menu initialization may be delayed
    QVariant m_delayedMenuWindowId{-1};
    QTimer m_delayedApplicationMenuTimer;

    KWayland::Client::PlasmaShell *m_waylandShell{nullptr};
    QPointer<KWayland::Client::PlasmaWindowManagement> m_windowManagement;

    TaskManager::TasksModel* m_tasksModel;
};

}

#endif
