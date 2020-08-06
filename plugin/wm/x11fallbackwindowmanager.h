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
#ifndef X11FALLBACKWINDOWMANAGER_H
#define X11FALLBACKWINDOWMANAGER_H

//local
#include "abstractwindowmanager.h"

//Qt
#include <QAbstractNativeEventFilter>
#include <QObject>
#include <KWindowSystem>

namespace WM {

class X11FallbackWindowManager : public AbstractWindowManager, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit X11FallbackWindowManager(QObject *parent = nullptr);
    ~X11FallbackWindowManager() override;

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long int *result) override;

private Q_SLOTS:
    void onActiveWindowChanged(WId id);
    void onWindowChanged(WId id);
    //! there are apps that are not releasing their menu properly after closing
    //! and as such their menu is still shown even though the app does not exist
    //! any more. Such apps are Java based e.g. smartgit
    void onWindowRemoved(WId id);
    void filterWindow(KWindowInfo &info);

private:
    //! window that its menu initialization may be delayed
    QVariant m_delayedMenuWindowId{-1};

};

}

#endif
