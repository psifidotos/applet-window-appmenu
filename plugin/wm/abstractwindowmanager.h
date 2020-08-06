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

#ifndef ABSTRACTWINDOWMANAGER_H
#define ABSTRACTWINDOWMANAGER_H

//Qt
#include <QObject>
#include <QRect>
#include <QVariant>

namespace WM {

struct WMData{
    bool filterByActive{false};
    bool filterChildren{false};
    bool menuAvailable{false};
    bool visible{true};
    QRect screenGeometry;
    QVariant userWindowId{-1};
};

class AbstractWindowManager : public QObject
{
    Q_OBJECT

public:
    explicit AbstractWindowManager(QObject *parent = nullptr);
    ~AbstractWindowManager() override;

    bool filterByActive() const;
    void setFilterByActive(bool active);

    bool filterChildren() const;
    void setFilterChildren(bool hideChildren);

    bool menuAvailable() const;
    void setMenuAvailable(bool set);

    bool visible() const;

    QRect screenGeometry() const;
    void setScreenGeometry(QRect geometry);

    QVariant winId() const;
    void setWinId(const QVariant &id);

    WMData data() const;
    void setData(const WMData &data);

signals:
    void applicationMenuChanged(const QString &serviceName, const QString &menuObjectPath);
    void modelNeedsUpdate();

    void dataChanged();
    void menuAvailableChanged();
    void filterByActiveChanged();
    void filterChildrenChanged();
    void visibleChanged();
    void screenGeometryChanged();
    void winIdChanged();

protected:
    void setVisible(bool visible);

    bool hasUserWindowId() const;

protected:
    bool m_filterByActive{false};
    bool m_filterChildren{false};
    bool m_menuAvailable{false};
    bool m_visible{true};
    QRect m_screenGeometry;
    QVariant m_userWindowId{-1};

    //! current active window used
    QVariant m_currentWindowId{-1};
};

}

#endif
