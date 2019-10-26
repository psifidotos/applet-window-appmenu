/*
*  Copyright 2018 Michail Vourlakos <mvourlakos@gmail.com>
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

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls 2.2 as Controls22
import QtQuick.Layouts 1.0

Controls22.ComboBox{
    id: combobox

    Connections{
        target: popup
        onClosed: root.forceActiveFocus();
    }

    delegate: MouseArea{
        width: combobox.width
        height: combobox.height
        hoverEnabled: true

        onClicked: {
            combobox.currentIndex = index;
            selectedScheme = model.file;
            combobox.popup.close();
        }

        Rectangle{
            id:delegateBackground
            anchors.fill: parent
            color: {
                if (containsMouse) {
                    return palette.highlight;
                }
                if (combobox.currentIndex === index) {
                    return selectedColor;
                }

                return "transparent";
            }

            readonly property color selectedColor: Qt.rgba(palette.highlight.r, palette.highlight.g, palette.highlight.b, 0.5);

            RowLayout{
                id: delegateRow
                height: parent.height

                Item{
                    Layout.leftMargin: 2
                    width: 1.25 * label.height
                    height: label.height

                    opacity: file==="_default_" || file==="_current_" ? 0 : 1

                    Rectangle{
                        width: height
                        height: 0.75 * label.height
                        color: backgroundColor
                        border.width: 1
                        border.color: containsMouse  || (combobox.currentIndex === index) ? palette.highlightedText : palette.text

                        Rectangle{
                            anchors.horizontalCenter: parent.right
                            anchors.verticalCenter: parent.bottom
                            width: parent.width
                            height: parent.height
                            color: textColor
                            border.width: parent.border.width
                            border.color: parent.border.color
                        }
                    }
                }

                Label{
                    id: label
                    text:display
                    color: containsMouse ? palette.highlightedText : palette.text
                }
            }
        }
    }
}
