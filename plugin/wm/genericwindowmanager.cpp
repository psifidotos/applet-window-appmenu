/*
*  Copyright 20120 Michail Vourlakos <mvourlakos@gmail.com>
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

#include "genericwindowmanager.h"

//Qt
#include <QDebug>
#include <QModelIndex>

#include <taskmanager/abstracttasksmodel.h>

namespace WM {

GenericWindowManager::GenericWindowManager(QObject *parent)
    : AbstractWindowManager(parent),
      m_tasksModel(new TaskManager::TasksModel(this))
{
    m_tasksModel->setFilterByScreen(true);
    connect(m_tasksModel, &TaskManager::TasksModel::activeTaskChanged, this, &GenericWindowManager::onActiveWindowChanged);
    connect(m_tasksModel, &TaskManager::TasksModel::activityChanged, this, &GenericWindowManager::onActiveWindowChanged);
    connect(m_tasksModel, &TaskManager::TasksModel::virtualDesktopChanged, this, &GenericWindowManager::onActiveWindowChanged);
    connect(m_tasksModel, &TaskManager::TasksModel::countChanged, this, &GenericWindowManager::onActiveWindowChanged);

    connect(this, &AbstractWindowManager::screenGeometryChanged, this, [this] {
        m_tasksModel->setScreenGeometry(m_screenGeometry);
    });
}

GenericWindowManager::~GenericWindowManager()
{
}

void GenericWindowManager::onActiveWindowChanged()
{
    const QModelIndex activeTaskIndex = m_tasksModel->activeTask();
    const QString objectPath = m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::ApplicationMenuObjectPath).toString();
    const QString serviceName = m_tasksModel->data(activeTaskIndex, TaskManager::AbstractTasksModel::ApplicationMenuServiceName).toString();

    if (!objectPath.isEmpty() && !serviceName.isEmpty()) {
        setMenuAvailable(true);
        emit applicationMenuChanged(serviceName, objectPath);
        setVisible(true);
        emit modelNeedsUpdate();
    } else {
        setMenuAvailable(false);
        setVisible(false);
    }
}

}
