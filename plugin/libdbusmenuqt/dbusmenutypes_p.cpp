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
#include "dbusmenutypes_p.h"

// Local
#include "dbusmenushortcut_p.h"

// Qt
#include <QDBusArgument>
#include <QDBusMetaType>

//// DBusMenuItem
QDBusArgument &operator<<(QDBusArgument &argument, const DBusMenuItem &obj)
{
    argument.beginStructure();
    argument << obj.id << obj.properties;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DBusMenuItem &obj)
{
    argument.beginStructure();
    argument >> obj.id >> obj.properties;
    argument.endStructure();
    return argument;
}

//// DBusMenuItemKeys
QDBusArgument &operator<<(QDBusArgument &argument, const DBusMenuItemKeys &obj)
{
    argument.beginStructure();
    argument << obj.id << obj.properties;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DBusMenuItemKeys &obj)
{
    argument.beginStructure();
    argument >> obj.id >> obj.properties;
    argument.endStructure();
    return argument;
}

//// DBusMenuLayoutItem
QDBusArgument &operator<<(QDBusArgument &argument, const DBusMenuLayoutItem &obj)
{
    argument.beginStructure();
    argument << obj.id << obj.properties;
    argument.beginArray(qMetaTypeId<QDBusVariant>());
    Q_FOREACH(const DBusMenuLayoutItem& child, obj.children) {
        argument << QDBusVariant(QVariant::fromValue<DBusMenuLayoutItem>(child));
    }
    argument.endArray();
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DBusMenuLayoutItem &obj)
{
    argument.beginStructure();
    argument >> obj.id >> obj.properties;
    argument.beginArray();
    while (!argument.atEnd()) {
        QDBusVariant dbusVariant;
        argument >> dbusVariant;
        QDBusArgument childArgument = dbusVariant.variant().value<QDBusArgument>();

        DBusMenuLayoutItem child;
        childArgument >> child;
        obj.children.append(child);
    }
    argument.endArray();
    argument.endStructure();
    return argument;
}

//// DBusMenuShortcut
QDBusArgument &operator<<(QDBusArgument &argument, const DBusMenuShortcut &obj)
{
    argument.beginArray(qMetaTypeId<QStringList>());
    typename QList<QStringList>::ConstIterator it = obj.constBegin();
    typename QList<QStringList>::ConstIterator end = obj.constEnd();
    for ( ; it != end; ++it)
        argument << *it;
    argument.endArray();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DBusMenuShortcut &obj)
{
    argument.beginArray();
    obj.clear();
    while (!argument.atEnd()) {
        QStringList item;
        argument >> item;
        obj.push_back(item);
    }
    argument.endArray();
    return argument;
}

void DBusMenuTypes_register()
{
    static bool registered = false;
    if (registered) {
        return;
    }
    qDBusRegisterMetaType<DBusMenuItem>();
    qDBusRegisterMetaType<DBusMenuItemList>();
    qDBusRegisterMetaType<DBusMenuItemKeys>();
    qDBusRegisterMetaType<DBusMenuItemKeysList>();
    qDBusRegisterMetaType<DBusMenuLayoutItem>();
    qDBusRegisterMetaType<DBusMenuLayoutItemList>();
    qDBusRegisterMetaType<DBusMenuShortcut>();
    registered = true;
}
