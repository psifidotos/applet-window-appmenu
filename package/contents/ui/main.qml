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
    id: root

    readonly property bool vertical: plasmoid.formFactor === PlasmaCore.Types.Vertical
    readonly property bool view: plasmoid.configuration.compactView
    readonly property bool menuAvailable: appMenuModel.menuAvailable

    readonly property bool kcmAuthorized: KCMShell.authorize(["style.desktop"]).length > 0

    onViewChanged: {
        plasmoid.nativeInterface.view = view
    }

    Plasmoid.preferredRepresentation: (plasmoid.configuration.compactView || vertical) ? Plasmoid.compactRepresentation : Plasmoid.fullRepresentation

    Plasmoid.compactRepresentation: Item {
        id: globalButtonItem
        Layout.preferredWidth: globalMenuIcon.width + 2 * units.smallSpacing + 2*shadow
        Layout.fillWidth: root.vertical
        Layout.fillHeight: !root.vertical

        readonly property int buttonIndex: 0
        readonly property int shadow: 3

        signal clicked;

        onClicked: {
            plasmoid.nativeInterface.trigger(this, buttonIndex);
        }

        //BEGIN Latte Dock Communicator for CompactRepresentation
        property QtObject latteBridge: null
        onLatteBridgeChanged: {
            if (latteBridge) {
                latteBridge.actions.setProperty(plasmoid.id, "disableLatteSideColoring", true);
                buttonGrid.latteBridge = latteBridge;
            }
        }

        readonly property bool enforceLattePalette: latteBridge && latteBridge.applyPalette && latteBridge.palette
        //END  Latte Dock Communicator

        Rectangle {
            id: buttonGlobal
            anchors.fill: parent
            anchors.margins: 1

            // fake highlighted
            color: {
                if (menuOpened) {
                    return enforceLattePalette ? latteBridge.palette.highlightColor : theme.highlightColor
                } else if (globalButtonMouseArea.containsMouse) {
                    return enforceLattePalette ? latteBridge.palette.buttonBackgroundColor : theme.buttonBackgroundColor
                } else {
                    return 'transparent';
                }
            }

            radius: 2

            property bool menuOpened: plasmoid.nativeInterface.currentIndex === buttonIndex



            layer.enabled: menuOpened || globalButtonMouseArea.containsMouse
            layer.effect: DropShadow{
                radius: globalButtonItem.shadow
                samples: 2 * radius
                color: "#ff151515"
            }

            PlasmaCore.IconItem{
                id: globalMenuIcon
                Layout.fillWidth: root.vertical
                Layout.fillHeight: !root.vertical
                anchors.centerIn: parent
                enabled:  menuAvailable
                source: "application-menu"

                layer.enabled: enforceLattePalette
                layer.effect: ColorOverlay{
                    color: {
                        if (buttonGlobal.menuOpened) {
                            return enforceLattePalette ? latteBridge.palette.highlightedTextColor : theme.highlightedTextColor
                        } else if (globalButtonMouseArea.containsMouse) {
                            return enforceLattePalette ? latteBridge.palette.buttonTextColor : theme.buttonTextColor
                        } else {
                            return enforceLattePalette ? latteBridge.palette.textColor : theme.textColor;
                        }
                    }
                }
            }
        }

        MouseArea{
            id: globalButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onPressed: {
                globalButtonItem.clicked();
            }
        }
    }

    Plasmoid.fullRepresentation: GridLayout {
        id: buttonGrid

        Plasmoid.status: {
            if (menuAvailable && plasmoid.nativeInterface.currentIndex > -1 && buttonRepeater.count > 0) {
                return PlasmaCore.Types.NeedsAttentionStatus;
            } else if (menuAvailable){
                //when we're not enabled set to active to show the configure button
                return buttonRepeater.count > 0 ? PlasmaCore.Types.ActiveStatus : PlasmaCore.Types.HiddenStatus;
            } else {
                return PlasmaCore.Types.PassiveStatus;
            }
        }

        Layout.minimumWidth: implicitWidth
        Layout.minimumHeight: implicitHeight

        flow: root.vertical ? GridLayout.TopToBottom : GridLayout.LeftToRight
        rowSpacing: units.smallSpacing
        columnSpacing: units.smallSpacing

        //BEGIN Latte Dock Communicator for FullRepresentation
        property QtObject latteBridge: null
        onLatteBridgeChanged: {
            if (latteBridge) {
                latteBridge.actions.setProperty(plasmoid.id, "disableLatteSideColoring", true);
                globalButtonItem.latteBridge = latteBridge;
            }
        }

        readonly property bool enforceLattePalette: latteBridge && latteBridge.applyPalette && latteBridge.palette
        //END  Latte Dock Communicator

        Component.onCompleted: {
            plasmoid.nativeInterface.buttonGrid = buttonGrid

            // using a Connections {} doesn't work for some reason in Qt >= 5.8
            plasmoid.nativeInterface.requestActivateIndex.connect(function (index) {
                var idx = Math.max(0, Math.min(buttonRepeater.count - 1, index))
                var button = buttonRepeater.itemAt(index)

                if (button) {
                    button.clicked()
                }
            });

            plasmoid.activated.connect(function () {
                var button = buttonRepeater.itemAt(0);
                if (button) {
                    button.clicked();
                }
            });
        }

        // So we can show mnemonic underlines only while Alt is pressed
        PlasmaCore.DataSource {
            id: keystateSource
            engine: "keystate"
            connectedSources: ["Alt"]
        }

        Repeater {
            id: buttonRepeater
            model: appMenuModel.visible ? appMenuModel : null

            Item{
                id: buttonItem
                Layout.preferredWidth: buttonLbl.width + 2 * units.smallSpacing + 2 * shadow
                Layout.fillWidth: root.vertical
                Layout.fillHeight: !root.vertical

                visible: buttonLbl.text !== ""

                readonly property int buttonIndex: index
                readonly property int shadow: 3

                signal clicked;

                onClicked: {
                    plasmoid.nativeInterface.trigger(this, index);
                }

                Rectangle {
                    id: button
                    anchors.fill: parent
                    anchors.margins: 1

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

                    radius: 3

                    property bool menuOpened: plasmoid.nativeInterface.currentIndex === index

                    layer.enabled: menuOpened || buttonMouseArea.containsMouse
                    layer.effect: DropShadow{
                        radius: buttonItem.shadow
                        samples: 2 * radius
                        color: "#ff151515"
                    }

                    PlasmaComponents.Label{
                        id: buttonLbl
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter

                        textFormat: Text.StyledText
                        text: {
                            var text = activeMenu;

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
                            if (button.menuOpened) {
                                return enforceLattePalette ? latteBridge.palette.highlightedTextColor : theme.highlightedTextColor
                            } else if (buttonMouseArea.containsMouse) {
                                return enforceLattePalette ? latteBridge.palette.buttonTextColor : theme.buttonTextColor
                            } else {
                                return enforceLattePalette ? latteBridge.palette.textColor : theme.textColor;
                            }
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
                }
            }
        }
    }

    AppMenuPrivate.AppMenuModel {
        id: appMenuModel

        filterByActive: plasmoid.configuration.filterByActive
        filterChildren: plasmoid.configuration.filterChildrenWindows
        screenGeometry: plasmoid.configuration.filterByScreen ? plasmoid.screenGeometry : Qt.rect(-1, -1, 0, 0) //null geometry
        onRequestActivateIndex: plasmoid.nativeInterface.requestActivateIndex(index)
        Component.onCompleted: {
            plasmoid.nativeInterface.model = appMenuModel
        }
    }
}
