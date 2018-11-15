/* This file is part of the dbusmenu-qt library
   Copyright 2009 Canonical
   Author: Aurelien Gateau <aurelien.gateau@canonical.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License (LGPL) as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later
   version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef DBUSMENUIMPORTER_H
#define DBUSMENUIMPORTER_H

// Qt
#include <QObject>

class QAction;
class QDBusPendingCallWatcher;
class QDBusVariant;
class QIcon;
class QMenu;

class DBusMenuImporterPrivate;

/**
 * A DBusMenuImporter instance can recreate a menu serialized over DBus by
 * DBusMenuExporter
 */
class DBusMenuImporter : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a DBusMenuImporter listening over DBus on service, path
     */
    DBusMenuImporter(const QString &service, const QString &path, QObject *parent = nullptr);

    ~DBusMenuImporter() override;


    QAction *actionForId(int id) const;

    /**
     * The menu created from listening to the DBusMenuExporter over DBus
     */
    QMenu *menu() const;

public Q_SLOTS:
    /**
     * Load the menu
     *
     * Will emit menuUpdated() when complete.
     * This should be done before showing a menu
     */
    void updateMenu();

    void updateMenu(QMenu *menu);

Q_SIGNALS:
    /**
     * Emitted after a call to updateMenu().
     * @see updateMenu()
     */
    void menuUpdated(QMenu *);

    /**
     * Emitted when the exporter was asked to activate an action
     */
    void actionActivationRequested(QAction *);

protected:
    /**
     * Must create a menu, may be customized to fit host appearance.
     * Default implementation creates a simple QMenu.
     */
    virtual QMenu *createMenu(QWidget *parent);

    /**
     * Must convert a name into an icon.
     * Default implementation returns a null icon.
     */
    virtual QIcon iconForName(const QString &);

private Q_SLOTS:
    void sendClickedEvent(int);
    void slotMenuAboutToShow();
    void slotMenuAboutToHide();
    void slotAboutToShowDBusCallFinished(QDBusPendingCallWatcher *);
    void slotItemActivationRequested(int id, uint timestamp);
    void processPendingLayoutUpdates();
    void slotLayoutUpdated(uint revision, int parentId);
    void slotGetLayoutFinished(QDBusPendingCallWatcher *);

private:
    Q_DISABLE_COPY(DBusMenuImporter)
    DBusMenuImporterPrivate *const d;
    friend class DBusMenuImporterPrivate;

    // Use Q_PRIVATE_SLOT to avoid exposing DBusMenuItemList
    Q_PRIVATE_SLOT(d, void slotItemsPropertiesUpdated(const DBusMenuItemList &updatedList, const DBusMenuItemKeysList &removedList))
};

#endif /* DBUSMENUIMPORTER_H */
