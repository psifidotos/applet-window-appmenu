/*
 * Copyright 2018  Michail Vourlakos <mvourlakos@gmail.com>
 *
 * This file is part of Latte-Dock
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

#include "commontools.h"

// Qt
#include <QFileInfo>
#include <QStandardPaths>

namespace AppletDecoration {

QString standardPath(QString subPath, bool localfirst)
{
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

    if (localfirst) {
        for (const auto &pt : paths) {
            QString ptF = pt + "/" +subPath;
            if (QFileInfo(ptF).exists()) {
                return ptF;
            }
        }
    } else {
        for (int i=paths.count()-1; i>=0; i--) {
            QString ptF = paths[i] + "/" +subPath;
            if (QFileInfo(ptF).exists()) {
                return ptF;
            }
        }
    }

    //! in any case that above fails
    if (QFileInfo("/usr/share/"+subPath).exists()) {
        return "/usr/share/"+subPath;
    }

    return "";
}

QStringList standardPaths(bool localfirst)
{
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

    if (localfirst) {
        return paths;
    } else {
        QStringList reversed;

        for (int i=paths.count()-1; i>=0; i--) {
            reversed << paths[i];
        }

        return reversed;
    }
}

QStringList standardPathsFor(QString subPath, bool localfirst)
{
    QStringList paths = standardPaths(localfirst);

    QString separator = subPath.startsWith("/") ? "" : "/";

    for (int i=0; i<paths.count(); ++i) {
        paths[i] = paths[i] + separator + subPath;
    }

    return paths;
}

}
