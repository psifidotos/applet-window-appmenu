/*
*  Copyright 2020 Michail Vourlakos <mvourlakos@gmail.com>
*
*  This file is part of applet-window-buttons
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

import QtQuick 2.7
import QtQuick.Controls 1.4

import org.kde.plasma.plasmoid 2.0

Label {
    id: customLbl
    width: implicitWidth + 2*3

    property int screenEdgeMargin: 0
    property int thicknessPadding: 1

    readonly property int edge: screenEdgeMargin + thicknessPadding

    states: [
        ///Top
        State {
            name: "top"
            when: (plasmoid.location === PlasmaCore.Types.TopEdge)
            AnchorChanges {
                target: customLbl
                anchors{top:parent.top; bottom:undefined; left:undefined; right:undefined}
            }
            PropertyChanges{
                target: customLbl
                anchors{leftMargin:0; rightMargin:0; topMargin:customLbl.edge; bottomMargin:0}
            }
        },
        ///Left
        State {
            name: "left"
            when: (plasmoid.location === PlasmaCore.Types.LeftEdge)
            AnchorChanges {
                target: customLbl
                anchors{top:undefined; bottom:undefined; left:parent.left; right:undefined}
            }
            PropertyChanges{
                target: customLbl
                anchors{leftMargin:buttonItem.edge; rightMargin:0; topMargin:0; bottomMargin:0}
            }
        },
        ///Right
        State {
            name: "right"
            when: (plasmoid.location === PlasmaCore.Types.RightEdge)
            AnchorChanges {
                target: customLbl
                anchors{top:undefined; bottom:undefined; left:undefined; right:parent.right}
            }
            PropertyChanges{
                target: customLbl
                anchors{leftMargin:0; rightMargin:buttonItem.edge; topMargin:0; bottomMargin:0}
            }
        },
        ///Default-Bottom
        State {
            name: "defaultbottom"
            AnchorChanges {
                target: customLbl
                anchors{top:undefined; bottom:parent.bottom; left:undefined; right:undefined}
            }
            PropertyChanges{
                target: customLbl
                anchors{leftMargin:0; rightMargin:0; topMargin:0; bottomMargin:buttonItem.edge}
            }
        }
    ]
}
