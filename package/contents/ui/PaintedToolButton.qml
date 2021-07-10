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
import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

import "../code/util.js" as Util

Item {
    id: buttonItem

    property int buttonIndex: -1
    property string text: ""
    property string icon: ""

    //! HACK: in order to identify containsMouse correctly at all cases,
    //! best way to reproduce the issue is in Unity mode that many buttons
    //! thought that they containMouse even though they did not
    property bool containsMouse: {
        if (inFullView) {
            if (plasmoid.nativeInterface.currentIndex > -1) {
                return plasmoid.nativeInterface.currentIndex === buttonIndex;
            }

            return (buttonIndex > -1) && (buttonGrid.currentIndex === buttonIndex);
        }

        return buttonMouseArea.containsMouse;
    }

    readonly property bool menuOpened: plasmoid.nativeInterface.currentIndex === buttonIndex
    readonly property int shadow: 3

    readonly property int implicitWidth: {
        if (itemLoader.item) {
            if (buttonItem.text !== "") {
                return itemLoader.item.implicitWidth + plasmoid.configuration.spacing * 2 + 2*shadow;
            } else {
                return itemLoader.item.implicitWidth + units.smallspacing * 2 + 2*shadow;
            }
        }

        return 0;
    }

    readonly property int implicitHeight: {
        if (itemLoader.item) {
            if (buttonItem.text !== "") {
                return itemLoader.item.implicitHeight + units.smallspacing * 2 + 2*shadow;
            } else {
                return itemLoader.item.implicitHeight + units.smallspacing * 2 + 2*shadow;
            }
        }

        return 0;
    }

    property int screenEdgeMargin: 0
    property int thicknessPadding: 1

    readonly property int edge: screenEdgeMargin + thicknessPadding

    signal clicked;
    signal scrolledUp(int step);
    signal scrolledDown(int step);

    onMenuOpenedChanged: {
        if (menuOpened) {
            buttonGrid.currentIndex = buttonIndex
        }
    }

    Item {
        id: edgeRelevantLocatedItem
        width: plasmoid.formFactor === PlasmaCore.Types.Horizontal ? length : thickness
        height: plasmoid.formFactor === PlasmaCore.Types.Horizontal ? thickness : length

        readonly property int length: plasmoid.formFactor === PlasmaCore.Types.Horizontal ? parent.width : parent.height
        readonly property int thickness: (plasmoid.formFactor === PlasmaCore.Types.Horizontal ? parent.height : parent.width) - screenEdgeMargin - 2*thicknessPadding

        Rectangle {
            id: button
            anchors.fill: parent
            anchors.topMargin: 1
            anchors.bottomMargin: 1
            anchors.leftMargin: buttonItem.shadow
            anchors.rightMargin: buttonItem.shadow

            radius: buttonItem.shadow

            // fake highlighted
            color: {
                if (menuOpened) {
                    return enforceLattePalette ? root.latteBridge.palette.highlightColor : theme.highlightColor
                } else if (buttonItem.containsMouse) {
                    return enforceLattePalette ? root.latteBridge.palette.buttonBackgroundColor : theme.buttonBackgroundColor
                } else {
                    return 'transparent';
                }
            }

            layer.enabled: menuOpened || buttonItem.containsMouse
            layer.effect: DropShadow{
                radius: buttonItem.shadow
                samples: 2 * radius
                color: "#ff151515"
            }
        }

        Loader{
            id: itemLoader
            anchors.fill: parent

            active: buttonItem.text !== "" || buttonItem.icon !== ""
            sourceComponent:  buttonItem.text !== "" ? labelComponent : iconComponent
        }

        states: [
            ///Top
            State {
                name: "top"
                when: (plasmoid.location === PlasmaCore.Types.TopEdge)
                AnchorChanges {
                    target: edgeRelevantLocatedItem
                    anchors{top:parent.top; bottom:undefined; left:parent.left; right:undefined;}
                }
                PropertyChanges{
                    target: edgeRelevantLocatedItem
                    anchors{leftMargin:0; rightMargin:0; topMargin:buttonItem.edge; bottomMargin:0}
                }
            },
            ///Left
            State {
                name: "left"
                when: (plasmoid.location === PlasmaCore.Types.LeftEdge)
                AnchorChanges {
                    target: edgeRelevantLocatedItem
                    anchors{top:parent.top; bottom:undefined; left:parent.left; right:undefined}
                }
                PropertyChanges{
                    target: edgeRelevantLocatedItem
                    anchors{leftMargin:buttonItem.edge; rightMargin:0; topMargin:0; bottomMargin:0}
                }
            },
            ///Right
            State {
                name: "right"
                when: (plasmoid.location === PlasmaCore.Types.RightEdge)
                AnchorChanges {
                    target: edgeRelevantLocatedItem
                    anchors{top:parent.top; bottom:undefined; left:undefined; right:parent.right}
                }
                PropertyChanges{
                    target: edgeRelevantLocatedItem
                    anchors{leftMargin:0; rightMargin:buttonItem.edge; topMargin:0; bottomMargin:0}
                }
            },
            ///Default-Bottom
            State {
                name: "defaultbottom"
                when: (plasmoid.location !== PlasmaCore.Types.TopEdge)
                      && (plasmoid.location !== PlasmaCore.Types.LeftEdge)
                      && (plasmoid.location !== PlasmaCore.Types.RightEdge)
                AnchorChanges {
                    target: edgeRelevantLocatedItem
                    anchors{top:undefined; bottom:parent.bottom; left:parent.left; right:undefined}
                }
                PropertyChanges{
                    target: edgeRelevantLocatedItem
                    anchors{leftMargin:0; rightMargin:0; topMargin:0; bottomMargin:buttonItem.edge}
                }
            }
        ]
    }

    // QMenu opens on press, so we'll replicate that here
    MouseArea {
        id: buttonMouseArea
        anchors.fill: parent
        hoverEnabled: true

        onPressed: {
            buttonItem.clicked();
        }

        onWheel: {
            var delta = 0;

            if (wheel.angleDelta.y>=0 && wheel.angleDelta.x>=0) {
                delta = Math.max(wheel.angleDelta.y, wheel.angleDelta.x);
            } else {
                delta = Math.min(wheel.angleDelta.y, wheel.angleDelta.x);
            }

            var angle = delta / 8;

            if (angle>10) {
                buttonItem.scrolledUp(buttonItem.implicitWidth);
            } else if (angle<-10) {
                buttonItem.scrolledDown(buttonItem.implicitWidth);
            }
        }

        //! HACK: in order to identify containsMouse correctly at all cases,
        //! best way to reproduce the issue is in Unity mode that many buttons
        //! thought that they containMouse even though they did not
        onEntered: {
            if (inFullView) {
                buttonGrid.currentIndex = buttonIndex;
            }
        }

        //! HACK: in order to identify containsMouse correctly at all cases,
        //! best way to reproduce the issue is in Unity mode that many buttons
        //! thought that they containMouse even though they did not
        onExited: {
            if (inFullView && buttonGrid.currentIndex === buttonIndex) {
                buttonGrid.currentIndex = -1;
            }
        }
    }

    //! Components

    Component{
        id: labelComponent
        PlasmaComponents.Label{
            id: buttonLbl
            anchors.fill: parent

            textFormat: Text.StyledText
            horizontalAlignment: Text.AlignHCenter

            text: {
                var text = buttonItem.text;

                var alt = keystateSource.data.Alt;
                if (!alt || !alt.Pressed) {

                    // StyleHelpers.removeMnemonics
                    text = text.replace(/([^&]*)&(.)([^&]*)/g, function (match, p1, p2, p3) {
                        return p1.concat(p2, p3);
                    });
                }

                return Util.stylizeEscapedMnemonics(Util.toHtmlEscaped(text));
            }

            color: {
                if (buttonItem.menuOpened) {
                    return enforceLattePalette ? root.latteBridge.palette.highlightedTextColor : theme.highlightedTextColor
                } else if (buttonItem.containsMouse) {
                    return enforceLattePalette ? root.latteBridge.palette.buttonTextColor : theme.buttonTextColor
                } else {
                    return enforceLattePalette ? root.latteBridge.palette.textColor : theme.textColor;
                }
            }
        }
    }

    Component{
        id: iconComponent
        PlasmaCore.IconItem{
            anchors.centerIn: parent
            width: edgeRelevantLocatedItem.thickness
            height: edgeRelevantLocatedItem.thickness
            enabled:  buttonItem.enabled
            source: buttonItem.icon

            layer.enabled: enforceLattePalette
            layer.effect: ColorOverlay{
                color: {
                    if (buttonItem.menuOpened) {
                        return enforceLattePalette ? root.latteBridge.palette.highlightedTextColor : theme.highlightedTextColor
                    } else if (buttonItem.containsMouse) {
                        return enforceLattePalette ? root.latteBridge.palette.buttonTextColor : theme.buttonTextColor
                    } else {
                        return enforceLattePalette ? root.latteBridge.palette.textColor : theme.textColor;
                    }
                }
            }
        }
    }
}
