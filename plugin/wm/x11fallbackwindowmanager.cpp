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

#include "x11fallbackwindowmanager.h"

#include <config-appmenu.h>

#if HAVE_X11
#include <QX11Info>
#include <xcb/xcb.h>
#endif

#include <QGuiApplication>

static const QByteArray s_x11AppMenuServiceNamePropertyName = QByteArrayLiteral("_KDE_NET_WM_APPMENU_SERVICE_NAME");
static const QByteArray s_x11AppMenuObjectPathPropertyName = QByteArrayLiteral("_KDE_NET_WM_APPMENU_OBJECT_PATH");

#if HAVE_X11
static QHash<QByteArray, xcb_atom_t> s_atoms;
#endif

namespace WM {

X11FallbackWindowManager::X11FallbackWindowManager(QObject *parent)
    : AbstractWindowManager(parent)
{
    if (!KWindowSystem::isPlatformX11()) {
        return;
    }

    connect(this, &AbstractWindowManager::winIdChanged, this, [this] {
        onActiveWindowChanged(m_userWindowId.toUInt());
    });

    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged,
            this, &X11FallbackWindowManager::onActiveWindowChanged);

    connect(KWindowSystem::self()
            , static_cast<void (KWindowSystem::*)(WId)>(&KWindowSystem::windowChanged)
            , this
            , &X11FallbackWindowManager::onWindowChanged);

    connect(KWindowSystem::self()
            , static_cast<void (KWindowSystem::*)(WId)>(&KWindowSystem::windowRemoved)
            , this
            , &X11FallbackWindowManager::onWindowRemoved);

    connect(this, &AbstractWindowManager::screenGeometryChanged, this, [this] {
        onWindowChanged(m_currentWindowId.toUInt());
    });

    connect(this, &AbstractWindowManager::menuAvailableChanged, this, [this] {
        onWindowChanged(m_currentWindowId.toUInt());
    });

    onActiveWindowChanged(KWindowSystem::activeWindow());

}

X11FallbackWindowManager::~X11FallbackWindowManager()
{
}

void X11FallbackWindowManager::onWindowChanged(WId id)
{
    if (m_currentWindowId == id) {
        KWindowInfo info(id, NET::WMState | NET::WMGeometry);
        filterWindow(info);
    }
}

void X11FallbackWindowManager::onWindowRemoved(WId id)
{
    if (m_currentWindowId == id) {
        setMenuAvailable(false);
        setVisible(false);
    }
}

void X11FallbackWindowManager::filterWindow(KWindowInfo &info)
{
    if (m_currentWindowId == info.win()) {
        //! HACK: if the user has enabled screen scaling under X11 environment
        //! then the window and screen geometries can not be trusted for comparison
        //! before windows coordinates be adjusted properly.
        //! BUG: 404500
        QPoint windowCenter = info.geometry().center();
        if (KWindowSystem::isPlatformX11()) {
            windowCenter /= qApp->devicePixelRatio();
        }
        const bool contained = m_screenGeometry.isNull() || m_screenGeometry.contains(windowCenter);

        const bool isActive = m_filterByActive ? info.win() == KWindowSystem::activeWindow() : true;

        setVisible(isActive && !info.isMinimized() && contained);
    }
}

void X11FallbackWindowManager::onActiveWindowChanged(WId id)
{
    qApp->removeNativeEventFilter(this);

    if (hasUserWindowId()  && m_userWindowId!=id) {
        //! ignore any other window except the one preferred from plasmoid
        return;
    }

    if (!id) {
        setMenuAvailable(false);
        emit modelNeedsUpdate();
        return;
    }

#if HAVE_X11

    if (KWindowSystem::isPlatformX11()) {
        auto *c = QX11Info::connection();

        auto getWindowPropertyString = [c, this](WId id, const QByteArray & name) -> QByteArray {
            QByteArray value;

            if (!s_atoms.contains(name))
            {
                const xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom(c, false, name.length(), name.constData());
                QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atomReply(xcb_intern_atom_reply(c, atomCookie, nullptr));

                if (atomReply.isNull()) {
                    return value;
                }

                s_atoms[name] = atomReply->atom;

                if (s_atoms[name] == XCB_ATOM_NONE) {
                    return value;
                }
            }

            static const long MAX_PROP_SIZE = 10000;
            auto propertyCookie = xcb_get_property(c, false, id, s_atoms[name], XCB_ATOM_STRING, 0, MAX_PROP_SIZE);
            QScopedPointer<xcb_get_property_reply_t, QScopedPointerPodDeleter> propertyReply(xcb_get_property_reply(c, propertyCookie, nullptr));

            if (propertyReply.isNull())
            {
                return value;
            }

            if (propertyReply->type == XCB_ATOM_STRING && propertyReply->format == 8 && propertyReply->value_len > 0)
            {
                const char *data = (const char *) xcb_get_property_value(propertyReply.data());
                int len = propertyReply->value_len;

                if (data) {
                    value = QByteArray(data, data[len - 1] ? len : len - 1);
                }
            }

            return value;
        };

        auto updateMenuFromWindowIfHasMenu = [this, &getWindowPropertyString](WId id) {
            const QString serviceName = QString::fromUtf8(getWindowPropertyString(id, s_x11AppMenuServiceNamePropertyName));
            const QString menuObjectPath = QString::fromUtf8(getWindowPropertyString(id, s_x11AppMenuObjectPathPropertyName));

            if (!serviceName.isEmpty() && !menuObjectPath.isEmpty()) {
                emit applicationMenuChanged(serviceName, menuObjectPath);
                return true;
            }

            return false;
        };

        KWindowInfo info(id, NET::WMState | NET::WMWindowType | NET::WMGeometry, NET::WM2TransientFor);

        if (info.hasState(NET::SkipTaskbar) ||
                info.windowType(NET::UtilityMask) == NET::Utility ||
                info.windowType(NET::DesktopMask) == NET::Desktop) {

            //! hide when the windows or their transiet(s) do not have a menu
            if (filterByActive()) {

                KWindowInfo transientInfo = KWindowInfo(info.transientFor(), NET::WMState | NET::WMWindowType | NET::WMGeometry, NET::WM2TransientFor);

                while (transientInfo.win()) {
                    if (transientInfo.win() == m_currentWindowId) {
                        filterWindow(info);
                        return;
                    }

                    transientInfo = KWindowInfo(transientInfo.transientFor(), NET::WMState | NET::WMWindowType | NET::WMGeometry, NET::WM2TransientFor);
                }
            }

            if (filterByActive()) {
                setVisible(false);
            }

            return;
        }

        m_currentWindowId = id;

        if (!filterChildren()) {
            KWindowInfo transientInfo = KWindowInfo(info.transientFor(), NET::WMState | NET::WMWindowType | NET::WMGeometry, NET::WM2TransientFor);

            // look at transient windows first
            while (transientInfo.win()) {
                if (updateMenuFromWindowIfHasMenu(transientInfo.win())) {
                    filterWindow(info);
                    return;
                }

                transientInfo = KWindowInfo(transientInfo.transientFor(), NET::WMState | NET::WMWindowType | NET::WMGeometry, NET::WM2TransientFor);
            }
        }

        if (updateMenuFromWindowIfHasMenu(id)) {
            filterWindow(info);
            return;
        }

        // monitor whether an app menu becomes available later
        // this can happen when an app starts, shows its window, and only later announces global menu (e.g. Firefox)
        qApp->installNativeEventFilter(this);
        m_delayedMenuWindowId = id;

        //no menu found, set it to unavailable
        setMenuAvailable(false);
        emit modelNeedsUpdate();
    }

#endif

}

bool X11FallbackWindowManager::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);

    if (!KWindowSystem::isPlatformX11() || eventType != "xcb_generic_event_t") {
        return false;
    }

#if HAVE_X11
    auto e = static_cast<xcb_generic_event_t *>(message);
    const uint8_t type = e->response_type & ~0x80;

    if (type == XCB_PROPERTY_NOTIFY) {
        auto *event = reinterpret_cast<xcb_property_notify_event_t *>(e);

        if (event->window == m_delayedMenuWindowId) {

            auto serviceNameAtom = s_atoms.value(s_x11AppMenuServiceNamePropertyName);
            auto objectPathAtom = s_atoms.value(s_x11AppMenuObjectPathPropertyName);

            if (serviceNameAtom != XCB_ATOM_NONE && objectPathAtom != XCB_ATOM_NONE) { // shouldn't happen
                if (event->atom == serviceNameAtom || event->atom == objectPathAtom) {
                    // see if we now have a menu
                    onActiveWindowChanged(KWindowSystem::activeWindow());
                }
            }
        }
    }

#else
    Q_UNUSED(message);
#endif

    return false;
}

}
