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
#ifndef DBUSMENUTYPES_P_H
#define DBUSMENUTYPES_P_H

// Qt
#include <QList>
#include <QStringList>
#include <QVariant>

class QDBusArgument;

//// DBusMenuItem
/**
 * Internal struct used to communicate on DBus
 */
struct DBusMenuItem
{
    int id;
    QVariantMap properties;
};

Q_DECLARE_METATYPE(DBusMenuItem)

QDBusArgument &operator<<(QDBusArgument &argument, const DBusMenuItem &item);
const QDBusArgument &operator>>(const QDBusArgument &argument, DBusMenuItem &item);

typedef QList<DBusMenuItem> DBusMenuItemList;

Q_DECLARE_METATYPE(DBusMenuItemList)


//// DBusMenuItemKeys
/**
 * Represents a list of keys for a menu item
 */
struct DBusMenuItemKeys
{
    int id;
    QStringList properties;
};

Q_DECLARE_METATYPE(DBusMenuItemKeys)

QDBusArgument &operator<<(QDBusArgument &argument, const DBusMenuItemKeys &);
const QDBusArgument &operator>>(const QDBusArgument &argument, DBusMenuItemKeys &);

typedef QList<DBusMenuItemKeys> DBusMenuItemKeysList;

Q_DECLARE_METATYPE(DBusMenuItemKeysList)

//// DBusMenuLayoutItem
/**
 * Represents an item with its children. GetLayout() returns a
 * DBusMenuLayoutItemList.
 */
struct DBusMenuLayoutItem;
struct DBusMenuLayoutItem
{
    int id;
    QVariantMap properties;
    QList<DBusMenuLayoutItem> children;
};

Q_DECLARE_METATYPE(DBusMenuLayoutItem)

QDBusArgument &operator<<(QDBusArgument &argument, const DBusMenuLayoutItem &);
const QDBusArgument &operator>>(const QDBusArgument &argument, DBusMenuLayoutItem &);

typedef QList<DBusMenuLayoutItem> DBusMenuLayoutItemList;

Q_DECLARE_METATYPE(DBusMenuLayoutItemList)

//// DBusMenuShortcut

class DBusMenuShortcut;

QDBusArgument &operator<<(QDBusArgument &argument, const DBusMenuShortcut &);
const QDBusArgument &operator>>(const QDBusArgument &argument, DBusMenuShortcut &);

void DBusMenuTypes_register();
#endif /* DBUSMENUTYPES_P_H */
