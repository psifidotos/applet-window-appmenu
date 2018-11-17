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

    Plasmoid.compactRepresentation: Rectangle {
        id: buttonGlobal
        readonly property int buttonIndex: 0

        Layout.preferredWidth: globalMenuIcon.width + 2 * units.smallSpacing
        Layout.fillWidth: root.vertical
        Layout.fillHeight: !root.vertical

        // fake highlighted
        color: menuOpened || globalButtonMouseArea.containsMouse ?
                   (enforceLattePalette ? latteBridge.palette.highlightColor : theme.highlightColor) :'transparent'

        radius: 2

        property bool menuOpened: plasmoid.nativeInterface.currentIndex === buttonIndex

        signal clicked;

        onClicked: {
            plasmoid.nativeInterface.trigger(this, buttonIndex);
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
                color: menuOpened || globalButtonMouseArea.containsMouse ?
                           latteBridge.palette.highlightedTextColor : latteBridge.palette.textColor
            }
        }

        MouseArea{
            id: globalButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onPressed: {
                buttonGlobal.clicked();
            }
        }

        //BEGIN Latte Dock Communicator for CompactRepresentation
        property QtObject latteBridge: null
        onLatteBridgeChanged: {
            if (latteBridge) {
                latteBridge.actions.setProperty(plasmoid.id, "disableLatteSideColoring", true);
            }
        }

        readonly property bool enforceLattePalette: latteBridge && latteBridge.applyPalette && latteBridge.palette
        //END  Latte Dock Communicator
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

            Rectangle {
                id: button
                readonly property int buttonIndex: index

                Layout.preferredWidth: buttonLbl.width + 2 * units.smallSpacing
                Layout.fillWidth: root.vertical
                Layout.fillHeight: !root.vertical

                visible: buttonLbl.text !== ""

                // fake highlighted
                color: menuOpened || buttonMouseArea.containsMouse ?
                           (enforceLattePalette ? latteBridge.palette.highlightColor : theme.highlightColor) :'transparent'

                radius: 2

                property bool menuOpened: plasmoid.nativeInterface.currentIndex === index

                signal clicked;

                onClicked: {
                    plasmoid.nativeInterface.trigger(this, index);
                }

                PlasmaComponents.Label{
                    id: buttonLbl
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter

                    text: {
                        var text = activeMenu;

                        var alt = keystateSource.data.Alt;
                        if (!alt || !alt.Pressed) {
                            // StyleHelpers.removeMnemonics
                            text = text.replace(/([^&]*)&(.)([^&]*)/g, function (match, p1, p2, p3) {
                                return p1.concat(p2, p3);
                            });
                        }

                        return text;
                    }

                    color: {
                        if (menuOpened || buttonMouseArea.containsMouse) {
                            if (enforceLattePalette) {
                                return latteBridge.palette.highlightedTextColor;
                            } else {
                                return theme.highlightedTextColor;
                            }
                        } else {
                            if (enforceLattePalette) {
                                return latteBridge.palette.textColor;
                            } else {
                                return theme.textColor;
                            }
                        }
                    }
                }

                // QMenu opens on press, so we'll replicate that here
                MouseArea {
                    id: buttonMouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onPressed: {
                        button.clicked();
                    }
                }
            }
        }
    }

    AppMenuPrivate.AppMenuModel {
        id: appMenuModel
        screenGeometry: plasmoid.configuration.filterByScreen ? plasmoid.screenGeometry : Qt.rect(-1, -1, 0, 0) //null geometry
        onRequestActivateIndex: plasmoid.nativeInterface.requestActivateIndex(index)
        Component.onCompleted: {
            plasmoid.nativeInterface.model = appMenuModel
        }
    }
}
