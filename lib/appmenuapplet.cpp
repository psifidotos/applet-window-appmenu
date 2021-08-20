/*
 * Copyright 2016 Kai Uwe Broulik <kde@privat.broulik.de>
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
 */

#include "appmenuapplet.h"
#include <config-appmenu.h>

#include "decorationpalette.h"
#include "../plugin/appmenumodel.h"

#include <QAction>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QQuickItem>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QTimer>
#include <QQuickWindow>
#include <QScreen>

#include <KWindowSystem>

int AppMenuApplet::s_refs = 0;
namespace {
QString viewService() { return QStringLiteral("org.kde.kappmenuview"); }
}

AppMenuApplet::AppMenuApplet(QObject *parent, const QVariantList &data)
    : Plasma::Applet(parent, data)
{
#if LibTaskManager_CURRENTMINOR_VERSION < 19 /*5.19*/
    // Disable for Plasma Desktop < 5.19
    if (KWindowSystem::isPlatformWayland()) {
        return;
    }
#endif

    ++s_refs;

    //if we're the first, regster the service
    if (s_refs == 1) {
        QDBusConnection::sessionBus().interface()->registerService(viewService(),
                                                                   QDBusConnectionInterface::QueueService,
                                                                   QDBusConnectionInterface::DontAllowReplacement);
    }

    /*it registers or unregisters the service when the destroyed value of the applet change,
      and not in the dtor, because:
      when we "delete" an applet, it just hides it for about a minute setting its status
      to destroyed, in order to be able to do a clean undo: if we undo, there will be
      another destroyedchanged and destroyed will be false.
      When this happens, if we are the only appmenu applet existing, the dbus interface
      will have to be registered again*/
    connect(this, &Applet::destroyedChanged, this, [this](bool destroyed) {
        if (destroyed) {
            //if we were the last, unregister
            if (--s_refs == 0) {
                QDBusConnection::sessionBus().interface()->unregisterService(viewService());
            }
        } else {
            //if we're the first, regster the service
            if (++s_refs == 1) {
                QDBusConnection::sessionBus().interface()->registerService(viewService(),
                                                                           QDBusConnectionInterface::QueueService,
                                                                           QDBusConnectionInterface::DontAllowReplacement);
            }
        }
    });
}

AppMenuApplet::~AppMenuApplet() = default;

void AppMenuApplet::init()
{
}

QObject *AppMenuApplet::model() const
{
    return m_model;
}

void AppMenuApplet::setModel(QObject *model)
{
    QAbstractListModel *appmodel = qobject_cast<QAbstractListModel *>(model);

    if (appmodel && m_model != appmodel) {
        m_model = appmodel;
        emit modelChanged();
    }
}

int AppMenuApplet::view() const
{
    return m_viewType;
}

void AppMenuApplet::setView(int type)
{
    if (m_viewType != type) {
        m_viewType = type;
        emit viewChanged();
    }
}

int AppMenuApplet::currentIndex() const
{
    return m_currentIndex;
}

void AppMenuApplet::setCurrentIndex(int currentIndex)
{
    if (m_currentIndex != currentIndex) {
        m_currentIndex = currentIndex;
        emit currentIndexChanged();
    }
}

QString AppMenuApplet::menuColorScheme() const
{
    return m_menuColorScheme;
}

void AppMenuApplet::setMenuColorScheme(const QString &scheme)
{
    if (m_menuColorScheme == scheme) {
        return;
    }

    m_menuColorScheme = scheme;

    if (!m_menuColorScheme.isEmpty()) {
        m_decorationPalette->deleteLater();
        m_decorationPalette = new DecorationPalette(scheme);\
    } else {
        m_decorationPalette->deleteLater();
    }

    emit menuColorSchemeChanged();
}

QQuickItem *AppMenuApplet::buttonGrid() const
{
    return m_buttonGrid;
}

void AppMenuApplet::setButtonGrid(QQuickItem *buttonGrid)
{
    if (m_buttonGrid != buttonGrid) {
        m_buttonGrid = buttonGrid;
        emit buttonGridChanged();
    }
}

QMenu *AppMenuApplet::createMenu(int idx) const
{
    QMenu *menu = nullptr;
    QAction *action = nullptr;

    if (view() == CompactView) {
        menu = new QMenu();

        for (int i = 0; i < m_model->rowCount(); i++) {
            const QModelIndex index = m_model->index(i, 0);
            const QVariant data = m_model->data(index, AppMenuModel::ActionRole);
            action = (QAction *)data.value<void *>();
            menu->addAction(action);
        }

        menu->setAttribute(Qt::WA_DeleteOnClose);
    } else if (view() == FullView) {
        const QModelIndex index = m_model->index(idx, 0);
        const QVariant data = m_model->data(index, AppMenuModel::ActionRole);
        action = (QAction *)data.value<void *>();

        if (action) {
            menu = action->menu();
        }
    }

    if (menu && m_decorationPalette) {
        menu->setPalette(m_decorationPalette->palette());
    }

    return menu;
}

void AppMenuApplet::onMenuAboutToHide()
{
    m_menuVisible = false;
    setCurrentIndex(-1);

    if (!m_currentMenu) {
        return;
    }

    //! Workaround: Send a fake QEvent::Leave to inform grid buttons for mouse leaving the view.
    //! This is needed because after triggering menus through hovering over them when the user clicks
    //! far from menus, the menus deactivate but for some reason an Enter event is sent and for that
    //! reason a button think that containsMouse. Without that workaround the experience
    //! is a bit broken because there case that buttons appear hovered without really be hovered.
    QHoverEvent e(QEvent::Leave, QPoint(-5,-5),  QPoint(2, 2));
    QCoreApplication::instance()->sendEvent(m_currentMenu->windowHandle()->transientParent(), &e);
}

void AppMenuApplet::repositionMenu()
{
    if (!m_currentMenu) {
        return;
    }

    QPoint pos = proposedPos(m_currentMenu, m_currentParentGeometry);
    m_currentMenu->move(pos);
}

bool AppMenuApplet::menuIsShown() const
{
    return m_currentMenu && m_menuVisible;
}

QPoint AppMenuApplet::proposedPos(QMenu *menu, QRect parentGeometry)
{
    if (!menu) {
        return QPoint();
    }

    QPoint result = parentGeometry.topLeft();
    const auto &geo =  menu->windowHandle()->transientParent()->screen()->geometry();

    if (location() == Plasma::Types::TopEdge) {
        result.setY(result.y() + parentGeometry.height());
    } else if (location() == Plasma::Types::BottomEdge) {
        result.setY(result.y() - menu->windowHandle()->height());
    } else if (location() == Plasma::Types::LeftEdge) {
        result.setX(result.x() + parentGeometry.width());
    } else {
        //Right Edge
        result.setX(result.x() - menu->windowHandle()->width());
    }

    result = QPoint(qBound(geo.x(), result.x(), geo.x() + geo.width() - menu->windowHandle()->width()),
                    qBound(geo.y(), result.y(), geo.y() + geo.height() - menu->windowHandle()->height()));

    return result;
}

bool AppMenuApplet::inPanel() const
{
    return (location() == Plasma::Types::BottomEdge)
            || (location() == Plasma::Types::TopEdge)
            || (location() == Plasma::Types::LeftEdge)
            || (location() == Plasma::Types::RightEdge);
}

void AppMenuApplet::trigger(QQuickItem *ctx, int idx)
{
    if (m_currentIndex == idx) {
        return;
    }

    if (!ctx || !ctx->window() || !ctx->window()->screen()) {
        return;
    }

    QMenu *actionMenu = createMenu(idx);

    if (actionMenu) {
        //this is a workaround where Qt will fail to realize a mouse has been released
        // this happens if a window which does not accept focus spawns a new window that takes focus and X grab
        // whilst the mouse is depressed
        // https://bugreports.qt.io/browse/QTBUG-59044
        // this causes the next click to go missing

        //by releasing manually we avoid that situation

        if (KWindowSystem::isPlatformX11()) {
            //! Under wayland is not needed
            auto ungrabMouseHack = [ctx]() {
                if (ctx && ctx->window() && ctx->window()->mouseGrabberItem()) {
                    // FIXME event forge thing enters press and hold move mode :/
                    ctx->window()->mouseGrabberItem()->ungrabMouse();
                }
            };

            QTimer::singleShot(0, ctx, ungrabMouseHack);
        }
        //end workaround

        // hide the old menu only after showing the new one to avoid brief flickering
        // in other windows as they briefly re-gain focus
        QMenu *oldMenu = m_currentMenu;

        if (oldMenu && oldMenu != actionMenu) {
            //don't initialize the currentIndex when another menu is already shown
            disconnect(oldMenu, &QMenu::aboutToHide, this, &AppMenuApplet::onMenuAboutToHide);
            disconnect(oldMenu->windowHandle(), &QWindow::widthChanged, this, &AppMenuApplet::repositionMenu);
            disconnect(oldMenu->windowHandle(), &QWindow::heightChanged, this, &AppMenuApplet::repositionMenu);
            disconnect(oldMenu, &QObject::destroyed, this, &AppMenuApplet::menuIsShownChanged);
            oldMenu->hide();
        }

        QPoint pos = ctx->window()->mapToGlobal(ctx->mapToScene(QPointF()).toPoint());
        m_currentParentGeometry = QRect(pos, QSize(ctx->width(), ctx->height()));

        bool firstmenuinitialization{m_currentMenu != actionMenu};
        m_currentMenu = actionMenu;

        //! Hiding the old menu before showing the new one is needed by wayland.
        //! Without that code reordering half of menus in wayland are not shown
        //! at all and wayland is complaining
        actionMenu->winId();//create window handle
        actionMenu->windowHandle()->setTransientParent(ctx->window());
        pos = proposedPos(actionMenu, m_currentParentGeometry);

        if (view() == FullView) {
            actionMenu->installEventFilter(this);
        }

        setCurrentIndex(idx);
        m_menuVisible = true;

        if (firstmenuinitialization) {
            connect(actionMenu, &QMenu::aboutToHide, this, &AppMenuApplet::onMenuAboutToHide, Qt::UniqueConnection);
            //! update menu positioning if menu width/height changed
            connect(actionMenu->windowHandle(), &QWindow::heightChanged, this, &AppMenuApplet::repositionMenu);
            connect(actionMenu->windowHandle(), &QWindow::widthChanged, this, &AppMenuApplet::repositionMenu);
        }

        actionMenu->popup(pos);

        emit menuIsShownChanged();
    } else { // is it just an action without a menu?
        const QVariant data = m_model->index(idx, 0).data(AppMenuModel::ActionRole);
        QAction *action = static_cast<QAction *>(data.value<void *>());

        if (action) {
            Q_ASSERT(!action->menu());
            action->trigger();
        }
    }
}

// FIXME TODO doesn't work on submenu
bool AppMenuApplet::eventFilter(QObject *watched, QEvent *event)
{
    auto *menu = qobject_cast<QMenu *>(watched);

    if (!menu) {
        return false;
    }

    if (event->type() == QEvent::KeyPress) {
        auto *e = static_cast<QKeyEvent *>(event);

        // TODO right to left languages
        if (e->key() == Qt::Key_Left) {
            int desiredIndex = m_currentIndex - 1;
            emit requestActivateIndex(desiredIndex);
            return true;
        } else if (e->key() == Qt::Key_Right) {
            if (menu->activeAction() && menu->activeAction()->menu()) {
                return false;
            }

            int desiredIndex = m_currentIndex + 1;
            emit requestActivateIndex(desiredIndex);
            return true;
        }

    } else if (event->type() == QEvent::MouseMove) {
        if (KWindowSystem::isPlatformX11()) {
            auto *e = static_cast<QMouseEvent *>(event);

            if (!m_buttonGrid || !m_buttonGrid->window()) {
                return false;
            }

            // Support Fitt's Law and take care the panel margins
            const QPointF &windowLocalPos = m_buttonGrid->window()->mapFromGlobal(e->globalPos());
            QPointF buttonGridLocalPos = m_buttonGrid->mapFromScene(windowLocalPos);

            // In Latte panel >= v0.10 we can access applets visual geometry
            QVariant appletsVisualGeomVariant = m_buttonGrid->window() ? m_buttonGrid->window()->property("_applets_layout_geometry") : QVariant();
            QRect windowVisualGeom = m_buttonGrid->window()->geometry();

            if (appletsVisualGeomVariant.isValid()) {
                QRect appletsVisualGeom = appletsVisualGeomVariant.toRect();
                appletsVisualGeom.moveTopLeft(windowVisualGeom.topLeft());
                windowVisualGeom = appletsVisualGeom;
            }

            if (inPanel() && windowVisualGeom.contains(e->globalPos()) && !m_buttonGrid->contains(buttonGridLocalPos)) {
                if (formFactor() == Plasma::Types::Horizontal) {
                    buttonGridLocalPos.setY(1);
                } else {
                    buttonGridLocalPos.setX(1);
                }
            }

            auto *item = m_buttonGrid->childAt(buttonGridLocalPos.x(), buttonGridLocalPos.y());

            if (!item) {
                return false;
            }

            bool ok;
            const int buttonIndex = item->property("buttonIndex").toInt(&ok);

            if (!ok) {
                return false;
            }

            emit requestActivateIndex(buttonIndex);
        }
    } else if (event->type() == QEvent::Leave) {
        if (!m_buttonGrid || !m_buttonGrid->window()) {
            return false;
        }

        m_buttonGrid->setProperty("currentIndex", -1);
    }

    return false;
}

K_EXPORT_PLASMA_APPLET_WITH_JSON(appmenu, AppMenuApplet, "metadata.json")

#include "appmenuapplet.moc"
