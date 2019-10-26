/*
 * Copyright 2019  Michail Vourlakos <mvourlakos@gmail.com>
 *
 * This file is part of the libappletdecoration library
 *
 * Latte-Dock is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Latte-Dock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QString>
#include <QStringList>

namespace AppletDecoration {

//! returns the standard path found that contains the subPath
//! local paths have higher priority by default
QString standardPath(QString subPath, bool localFirst = true);

QStringList standardPaths(bool localfirst = true);
QStringList standardPathsFor(QString subPath, bool localfirst = true);
}
