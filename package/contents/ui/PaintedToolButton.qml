/*
 * Copyright 2013  Heena Mahour <heena393@gmail.com>
 * Copyright 2013 Sebastian Kügler <sebas@kde.org>
 * Copyright 2016 Kai Uwe Broulik <kde@privat.broulik.de>
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

import org.kde.private.windowAppMenu 0.1 as AppMenuPrivate

import "../code/util.js" as Util

Item {
    id: buttonItem

    property int buttonIndex: -1
    property string text: ""
    property string icon: ""

    readonly property bool menuOpened: plasmoid.nativeInterface.currentIndex === buttonIndex
    readonly property int shadow: 3
    readonly property int implicitWidth: itemLoader.item.implicitWidth + 2*units.smallSpacing + 2*shadow
    readonly property int implicitHeight: itemLoader.item.implicitHeight + 2*units.smallSpacing + 2*shadow

    signal clicked;

    Rectangle {
        id: button
        anchors.fill: parent
        anchors.margins: 1

        radius: buttonItem.shadow

        // fake highlighted
        color: {
            if (menuOpened) {
                return enforceLattePalette ? latteBridge.palette.highlightColor : theme.highlightColor
            } else if (buttonMouseArea.containsMouse) {
                return enforceLattePalette ? latteBridge.palette.buttonBackgroundColor : theme.buttonBackgroundColor
            } else {
                return 'transparent';
            }
        }

        layer.enabled: menuOpened || buttonMouseArea.containsMouse
        layer.effect: DropShadow{
            radius: buttonItem.shadow
            samples: 2 * radius
            color: "#ff151515"
        }

        Loader{
            id: itemLoader
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            active: buttonItem.text !== "" || buttonItem.icon !== ""
            sourceComponent:  buttonItem.text !== "" ? labelComponent : iconComponent
        }
    }

    // QMenu opens on press, so we'll replicate that here
    MouseArea {
        id: buttonMouseArea
        anchors.fill: parent
        hoverEnabled: true

        onPressed: {
            buttonItem.clicked();
        }
    }

    //! Components

    Component{
        id: labelComponent
        PlasmaComponents.Label{
            id: buttonLbl

            textFormat: Text.StyledText
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
                    return enforceLattePalette ? latteBridge.palette.highlightedTextColor : theme.highlightedTextColor
                } else if (buttonMouseArea.containsMouse) {
                    return enforceLattePalette ? latteBridge.palette.buttonTextColor : theme.buttonTextColor
                } else {
                    return enforceLattePalette ? latteBridge.palette.textColor : theme.textColor;
                }
            }
        }
    }

    Component{
        id: iconComponent
        PlasmaCore.IconItem{
            Layout.fillWidth: root.vertical
            Layout.fillHeight: !root.vertical
            enabled:  menuAvailable
            source: buttonItem.icon

            layer.enabled: enforceLattePalette
            layer.effect: ColorOverlay{
                color: {
                    if (buttonItem.menuOpened) {
                        return enforceLattePalette ? latteBridge.palette.highlightedTextColor : theme.highlightedTextColor
                    } else if (buttonMouseArea.containsMouse) {
                        return enforceLattePalette ? latteBridge.palette.buttonTextColor : theme.buttonTextColor
                    } else {
                        return enforceLattePalette ? latteBridge.palette.textColor : theme.textColor;
                    }
                }
            }
        }
    }
}
