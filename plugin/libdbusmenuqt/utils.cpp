/* This file is part of the dbusmenu-qt library
   Copyright 2010 Canonical
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
#include "utils_p.h"

// Qt
#include <QString>

QString swapMnemonicChar(const QString &in, const char src, const char dst)
{
    QString out;
    bool mnemonicFound = false;

    for (int pos = 0; pos < in.length(); ) {
        QChar ch = in[pos];
        if (ch == src) {
            if (pos == in.length() - 1) {
                // 'src' at the end of string, skip it
                ++pos;
            } else {
                if (in[pos + 1] == src) {
                    // A real 'src'
                    out += src;
                    pos += 2;
                } else if (!mnemonicFound) {
                    // We found the mnemonic
                    mnemonicFound = true;
                    out += dst;
                    ++pos;
                } else {
                    // We already have a mnemonic, just skip the char
                    ++pos;
                }
            }
        } else if (ch == dst) {
            // Escape 'dst'
            out += dst;
            out += dst;
            ++pos;
        } else {
            out += ch;
            ++pos;
        }
    }

    return out;
}
