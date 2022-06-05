/* This file is part of the dbusmenu-qt library
    SPDX-FileCopyrightText: 2009 Canonical
    SPDX-FileContributor: Aurelien Gateau <aurelien.gateau@canonical.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "dbusmenushortcut_p.h"

// Qt
#include <QKeySequence>

static const int QT_COLUMN = 0;
static const int DM_COLUMN = 1;

static void processKeyTokens(QStringList *tokens, int srcCol, int dstCol)
{
    struct Row {
        const char *zero;
        const char *one;
        const char *operator[](int col) const
        {
            return col == 0 ? zero : one;
        }
    };
    static const Row table[] = {{"Meta", "Super"},
                                {"Ctrl", "Control"},
                                // Special cases for compatibility with libdbusmenu-glib which uses
                                // "plus" for "+" and "minus" for "-".
                                // cf https://bugs.launchpad.net/libdbusmenu-qt/+bug/712565
                                {"+", "plus"},
                                {"-", "minus"},
                                {nullptr, nullptr}};

    const Row *ptr = table;
    for (; ptr->zero != nullptr; ++ptr) {
        const char *from = (*ptr)[srcCol];
        const char *to = (*ptr)[dstCol];
        tokens->replaceInStrings(from, to);
    }
}

DBusMenuShortcut DBusMenuShortcut::fromKeySequence(const QKeySequence &sequence)
{
    QString string = sequence.toString();
    DBusMenuShortcut shortcut;
    QStringList tokens = string.split(QStringLiteral(", "));
    Q_FOREACH (QString token, tokens) {
        // Hack: Qt::CTRL | Qt::Key_Plus is turned into the string "Ctrl++",
        // but we don't want the call to token.split() to consider the
        // second '+' as a separator so we replace it with its final value.
        token.replace(QLatin1String("++"), QLatin1String("+plus"));
        QStringList keyTokens = token.split('+');
        processKeyTokens(&keyTokens, QT_COLUMN, DM_COLUMN);
        shortcut << keyTokens;
    }
    return shortcut;
}

QKeySequence DBusMenuShortcut::toKeySequence() const
{
    QStringList tmp;
    Q_FOREACH (const QStringList &keyTokens_, *this) {
        QStringList keyTokens = keyTokens_;
        processKeyTokens(&keyTokens, DM_COLUMN, QT_COLUMN);
        tmp << keyTokens.join(QLatin1String("+"));
    }
    QString string = tmp.join(QLatin1String(", "));
    return QKeySequence::fromString(string);
}
