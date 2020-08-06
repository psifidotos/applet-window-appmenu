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

#include "abstractwindowmanager.h"

namespace WM {

AbstractWindowManager::AbstractWindowManager(QObject *parent)
    : QObject (parent)
{
    connect(this, &AbstractWindowManager::dataChanged, this, &AbstractWindowManager::menuAvailableChanged);
    connect(this, &AbstractWindowManager::dataChanged, this, &AbstractWindowManager::filterByActiveChanged);
    connect(this, &AbstractWindowManager::dataChanged, this, &AbstractWindowManager::filterChildrenChanged);
    connect(this, &AbstractWindowManager::dataChanged, this, &AbstractWindowManager::visibleChanged);
    connect(this, &AbstractWindowManager::dataChanged, this, &AbstractWindowManager::screenGeometryChanged);
    connect(this, &AbstractWindowManager::dataChanged, this, &AbstractWindowManager::winIdChanged);
}

AbstractWindowManager::~AbstractWindowManager()
{
}

bool AbstractWindowManager::filterByActive() const
{
    return m_filterByActive;
}

void AbstractWindowManager::setFilterByActive(bool active)
{
    if (m_filterByActive == active) {
        return;
    }

    m_filterByActive = active;
    emit filterByActiveChanged();
}

bool AbstractWindowManager::filterChildren() const
{
    return m_filterChildren;
}

void AbstractWindowManager::setFilterChildren(bool hideChildren)
{
    if (m_filterChildren == hideChildren) {
        return;
    }

    m_filterChildren = hideChildren;
    emit filterChildrenChanged();
}


bool AbstractWindowManager::menuAvailable() const
{
    return m_menuAvailable;
}

void AbstractWindowManager::setMenuAvailable(bool set)
{
    if (m_menuAvailable != set) {
        m_menuAvailable = set;
        emit menuAvailableChanged();
    }
}

QRect AbstractWindowManager::screenGeometry() const
{
    return m_screenGeometry;
}

void AbstractWindowManager::setScreenGeometry(QRect geometry)
{
    if (m_screenGeometry == geometry) {
        return;
    }

    m_screenGeometry = geometry;
    emit screenGeometryChanged();
}

bool AbstractWindowManager::visible() const
{
    return m_visible;
}

void AbstractWindowManager::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        emit visibleChanged();
    }
}

QVariant AbstractWindowManager::winId() const
{
    return m_userWindowId;
}

void AbstractWindowManager::setWinId(const QVariant &id)
{
    if (m_userWindowId == id) {
        return;
    }

    m_userWindowId = id;
    emit winIdChanged();
}

bool AbstractWindowManager::hasUserWindowId() const
{
    return (m_userWindowId != -1);
}

WMData AbstractWindowManager::data() const
{
    WMData d;

    d.filterByActive = m_filterByActive;
    d.filterChildren = m_filterChildren;
    d.menuAvailable = m_menuAvailable;
    d.visible = m_visible;
    d.screenGeometry = m_screenGeometry;
    d.userWindowId = m_userWindowId;

    return d;
}

void AbstractWindowManager::setData(const WMData &data)
{
    m_filterByActive = data.filterByActive;
    m_filterChildren = data.filterChildren;
    m_menuAvailable = data.menuAvailable;
    m_visible = data.visible;
    m_screenGeometry = data.screenGeometry;
    m_userWindowId = data.userWindowId;

    emit dataChanged();
}


}
