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

#ifndef GENERICWINDOWMANAGER_H
#define GENERICWINDOWMANAGER_H

//local
#include "abstractwindowmanager.h"

//Qt
#include <QObject>

// Plasma TaskManager
#include <taskmanager/tasksmodel.h>

namespace WM {

class GenericWindowManager : public AbstractWindowManager
{
    Q_OBJECT

public:
    explicit GenericWindowManager(QObject *parent = nullptr);
    ~GenericWindowManager() override;

private slots:
    void onActiveWindowChanged();

private:
    TaskManager::TasksModel* m_tasksModel;

};

}

#endif
