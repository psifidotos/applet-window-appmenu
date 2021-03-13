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
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: lbl

    property int screenEdgeMargin: 0
    property int thicknessPadding: 1

    property string text: ""

    readonly property int implicitWidth: customLbl.implicitWidth + 2*3
    readonly property int implicitHeight: customLbl.thickness
    readonly property int edge: screenEdgeMargin + thicknessPadding

    Label {
        id: customLbl
        width: plasmoid.formFactor === PlasmaCore.Types.Horizontal ? length : thickness
        height: plasmoid.formFactor === PlasmaCore.Types.Horizontal ? thickness : length

        color: enforceLattePalette ? root.latteBridge.palette.textColor : theme.textColor;
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        text: parent.text

        readonly property int length: (plasmoid.formFactor === PlasmaCore.Types.Horizontal) ? parent.width : parent.height
        readonly property int thickness: (plasmoid.formFactor === PlasmaCore.Types.Horizontal ? parent.height : parent.width) - screenEdgeMargin - 2*thicknessPadding

        states: [
            ///Top
            State {
                name: "top"
                when: (plasmoid.location === PlasmaCore.Types.TopEdge)
                AnchorChanges {
                    target: customLbl
                    anchors{top:parent.top; bottom:undefined; left:parent.left; right:undefined}
                }
                PropertyChanges{
                    target: customLbl
                    anchors{leftMargin:0; rightMargin:0; topMargin:lbl.edge; bottomMargin:0}
                }
            },
            ///Left
            State {
                name: "left"
                when: (plasmoid.location === PlasmaCore.Types.LeftEdge)
                AnchorChanges {
                    target: customLbl
                    anchors{top:parent.top; bottom:undefined; left:parent.left; right:undefined}
                }
                PropertyChanges{
                    target: customLbl
                    anchors{leftMargin:lbl.edge; rightMargin:0; topMargin:0; bottomMargin:0}
                }
            },
            ///Right
            State {
                name: "right"
                when: (plasmoid.location === PlasmaCore.Types.RightEdge)
                AnchorChanges {
                    target: customLbl
                    anchors{top:parent.top; bottom:undefined; left:undefined; right:parent.right}
                }
                PropertyChanges{
                    target: customLbl
                    anchors{leftMargin:0; rightMargin:lbl.edge; topMargin:0; bottomMargin:0}
                }
            },
            ///Default-Bottom
            State {
                name: "defaultbottom"
                when: (plasmoid.location !== PlasmaCore.Types.TopEdge)
                      && (plasmoid.location !== PlasmaCore.Types.LeftEdge)
                      && (plasmoid.location !== PlasmaCore.Types.RightEdge)
                AnchorChanges {
                    target: customLbl
                    anchors{top:undefined; bottom:parent.bottom; left:parent.left; right:undefined}
                }
                PropertyChanges{
                    target: customLbl
                    anchors{leftMargin:0; rightMargin:0; topMargin:0; bottomMargin:lbl.edge}
                }
            }
        ]
    }


}
