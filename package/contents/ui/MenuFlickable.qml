/*
 * Copyright 2018 Michail Vourlakos <mvourlakos@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

Flickable{
    id: flickable
    anchors.left: parent.left

    clip: true
    interactive: true
    flickableDirection: Flickable.HorizontalFlick

    readonly property bool contentsExceed: contentWidth > width
    readonly property int contentsExtraSpace: contentsExceed ? contentWidth - width : 0

    function increaseX(step) {
        contentX = Math.min(contentsExtraSpace, contentX + step);
    }

    function decreaseX(step) {
        contentX = Math.max(0, contentX - step);
    }

    onContentsExceedChanged: {
        if (!contentsExceed) {
            contentX = 0;
        }
    }

    Behavior on contentX {
        NumberAnimation {
            duration: 300;
            easing.type: Easing.OutQuad
        }
    }
}
