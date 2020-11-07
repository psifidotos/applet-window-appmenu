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
import org.kde.taskmanager 0.1 as TaskManager

import org.kde.private.windowAppMenu 0.1 as AppMenuPrivate

Item {
    id: root

    readonly property int containmentType: plasmoid.configuration.containmentType

    readonly property bool vertical: plasmoid.formFactor === PlasmaCore.Types.Vertical
    readonly property bool view: plasmoid.configuration.compactView
    readonly property bool inEditMode: plasmoid.userConfiguring || latteInEditMode
    readonly property bool menuAvailable: appMenuModel.menuAvailable
    readonly property bool kcmAuthorized: KCMShell.authorize(["style.desktop"]).length > 0

    readonly property bool inFullView: !plasmoid.configuration.compactView && plasmoid.formFactor === PlasmaCore.Types.Horizontal
    readonly property bool inCompactView: !inFullView

    readonly property string currentScheme: plasmoid.configuration.selectedScheme

    Plasmoid.preferredRepresentation: plasmoid.fullRepresentation
    Plasmoid.status: inFullView ? fullLayout.status : compactLayout.status

    //BEGIN Layout properties
    Layout.fillWidth: inFullView && plasmoid.configuration.fillWidth ? true : root.vertical
    Layout.fillHeight: inFullView ? true : !root.vertical
    Layout.minimumWidth: {
        if (inFullView) {
            if (plasmoid.configuration.fillWidth && !inEditMode) {
                return -1;
            }

            return inEditMode ? buttonGrid.width : 0
        } else {
            return -1;
        }
    }

    Layout.preferredWidth: {
        if (inFullView) {
            if (plasmoid.configuration.fillWidth && !inEditMode) {
                return -1;
            }

            return buttonGrid.width;
        } else {
            return Math.max(compactLayout.implicitWidth, root.height);
        }
    }

    Layout.maximumWidth: {
        if (inFullView) {
            return plasmoid.configuration.fillWidth && !inEditMode ? Infinity : buttonGrid.width;
        } else {
            return -1;
        }
    }
    //END Layout properties

    //BEGIN Latte Dock Communicator
    property QtObject latteBridge: null

    property bool latteSupportsActiveWindowSchemes: plasmoid.configuration.supportsActiveWindowSchemes
    onLatteBridgeChanged: {
        if (latteBridge) {
            latteBridge.actions.setProperty(plasmoid.id, "latteSideColoringEnabled", false);
            latteBridge.actions.setProperty(plasmoid.id, "activeIndicatorEnabled", false);
            latteBridge.actions.setProperty(plasmoid.id, "windowsTrackingEnabled", true);
            latteBridge.actions.setProperty(plasmoid.id, "screenEdgeMarginSupported", true);

            if (latteBridge.version >= latteBridge.actions.version(0,9,4)) {
                plasmoid.configuration.supportsActiveWindowSchemes = true;
            }
        }
    }

    readonly property bool latteInEditMode: latteBridge && latteBridge.inEditMode
    readonly property bool enforceLattePalette: latteBridge && latteBridge.applyPalette && latteBridge.palette

    readonly property int screenEdgeMargin: latteBridge && latteBridge.hasOwnProperty("screenEdgeMargin") ? latteBridge.screenEdgeMargin : 0

    Broadcaster {
        id: broadcaster
    }
    //END  Latte Dock Communicator

    //! make sure that on startup it will always be shown
    readonly property bool existsWindowActive: (windowInfoLoader.item && windowInfoLoader.item.existsWindowActive)
                                               || containmentIdentifierTimer.running
    readonly property bool existsWindowShown: (windowInfoLoader.item && windowInfoLoader.item.existsWindowShown)
                                              || containmentIdentifierTimer.running

    readonly property bool isLastActiveWindowPinned: lastActiveTaskItem && existsWindowShown && lastActiveTaskItem.isOnAllDesktops
    readonly property bool isLastActiveWindowMaximized: lastActiveTaskItem && existsWindowShown && lastActiveTaskItem.isMaximized
    readonly property bool isLastActiveWindowKeepAbove: lastActiveTaskItem && existsWindowShown && lastActiveTaskItem.isKeepAbove

    readonly property bool isLastActiveWindowClosable: lastActiveTaskItem && existsWindowShown && lastActiveTaskItem.isClosable
    readonly property bool isLastActiveWindowMaximizable: lastActiveTaskItem && existsWindowShown && lastActiveTaskItem.isMaximizable
    readonly property bool isLastActiveWindowMinimizable: lastActiveTaskItem && existsWindowShown && lastActiveTaskItem.isMinimizable
    readonly property bool isLastActiveWindowVirtualDesktopsChangeable: lastActiveTaskItem && existsWindowShown && lastActiveTaskItem.isVirtualDesktopsChangeable

    readonly property Item lastActiveTaskItem: windowInfoLoader.item.lastActiveTaskItem

    Loader {
        id: windowInfoLoader
        sourceComponent: latteBridge
                         && latteBridge.windowsTracker
                         && latteBridge.windowsTracker.currentScreen.lastActiveWindow
                         && latteBridge.windowsTracker.allScreens.lastActiveWindow ? latteTrackerComponent : plasmaTasksModel

        Component{
            id: latteTrackerComponent
            LatteWindowsTracker{
                filterByScreen: plasmoid.configuration.filterByScreen
            }
        }

        Component{
            id: plasmaTasksModel
            PlasmaTasksModel{
                filterByScreen: plasmoid.configuration.filterByScreen
            }
        }
    }
    // END Window properties

    onViewChanged: {
        plasmoid.nativeInterface.view = view;
    }

    Component.onCompleted: {
        plasmoid.configuration.supportsActiveWindowSchemes = false;
        plasmoid.configuration.windowTitleIsPresent = false;
        plasmoid.nativeInterface.buttonGrid = buttonGrid;

        containmentIdentifierTimer.start();

        // using a Connections {} doesn't work for some reason in Qt >= 5.8
        plasmoid.nativeInterface.requestActivateIndex.connect(function (index) {
            if(inFullView) {
                var idx = Math.max(0, Math.min(buttonRepeater.count - 1, index))
                var button = buttonRepeater.itemAt(index)

                if (button) {
                    button.clicked()
                }
            } else {
                compactLayout.clicked();
            }
        });

        /*
          // commented in order to avoid to be activated from Latte neutral areas activation

            plasmoid.activated.connect(function () {
            if (inFullView) {
                var button = buttonRepeater.itemAt(0);
                if (button) {
                    button.clicked();
                }
            } else {
                compactLayout.clicked();
            }
        });*/
    }

    Binding {
        target: plasmoid.nativeInterface
        property: "menuColorScheme"
        when: plasmoid.nativeInterface && appMenuModel

        value: {
            if (root.currentScheme === "_default_") {
                return "";
            }

            if (root.currentScheme === "_current_") {
                if (latteSupportsActiveWindowSchemes) {
                    /* colorScheme value was added after Latte v0.9.4*/
                    return appMenuModel.selectedTracker.lastActiveWindow.colorScheme
                } else {
                    return "";
                }
            }

            return plasmoid.configuration.selectedScheme
        }
    }

    PaintedToolButton {
        id: compactLayout
        anchors.fill: parent
        enabled: menuAvailable
        visible: inCompactView
        screenEdgeMargin: root.screenEdgeMargin

        buttonIndex: 0
        icon: "application-menu"

        onClicked: {
            if (visible) {
                plasmoid.nativeInterface.trigger(this, buttonIndex);
            }
        }

        readonly property int status: {
            if (menuAvailable && plasmoid.nativeInterface.currentIndex === 0) {
                return PlasmaCore.Types.NeedsAttentionStatus;
            } else if (menuAvailable && appMenuModel.visible){
                return PlasmaCore.Types.ActiveStatus
            } else if (!inEditMode) {
                return PlasmaCore.Types.HiddenStatus;
            }

            return PlasmaCore.Types.PassiveStatus;
        }
    }

    Item {
        id: fullLayout
        anchors.fill: parent
        visible: inFullView

        readonly property int status: {
            if (broadcaster.cooperationEstablished && broadcaster.hiddenFromBroadcast && !inEditMode) {
                return PlasmaCore.Types.HiddenStatus;
            }

            if (menuAvailable){
                if (plasmoid.nativeInterface.currentIndex > -1 && buttonRepeater.count > 0) {
                    return PlasmaCore.Types.NeedsAttentionStatus;
                } else if (buttonRepeater.count > 0) {
                    return PlasmaCore.Types.ActiveStatus
                } else if (!plasmoid.configuration.fillWidth) {
                    return PlasmaCore.Types.HiddenStatus;
                }
            } else if (!menuAvailable && !inEditMode && !plasmoid.configuration.fillWidth){
                return PlasmaCore.Types.HiddenStatus;
            }

            return PlasmaCore.Types.PassiveStatus;
        }

        // So we can show mnemonic underlines only while Alt is pressed
        PlasmaCore.DataSource {
            id: keystateSource
            engine: "keystate"
            connectedSources: ["Alt"]

            readonly property bool modifierIsPressed: data.Alt && data.Alt.Pressed
        }

        MouseArea {
            id: fullViewBackMousearea
            anchors.left: gridFlickable.right
            width: visible ? parent.width - gridFlickable.width : 0 //! zero helps to release containsmouse on first showing
            height: parent.height - 1
            visible: broadcaster.cooperationEstablished && root.inFullView && (fullLayout.status !== PlasmaCore.Types.HiddenStatus)
                     && plasmoid.configuration.fillWidth && buttonRepeater.count > 0
            hoverEnabled: true
            propagateComposedEvents: true

            onPressed: {
                mouse.accepted = false;
            }

            onReleased: {
                mouse.accepted = false;
            }
        }

        MenuFlickable{
            id: gridFlickable
            width: parent.width < contentWidth && !inEditMode ? parent.width : contentWidth
            height: parent.height
            contentWidth: buttonGrid.width
            contentHeight: buttonGrid.height

            GridLayout{
                id: buttonGrid

                flow: GridLayout.LeftToRight
                rowSpacing: 0
                columnSpacing: 0

                property int currentIndex: -1

                onCurrentIndexChanged: {
                    if (currentIndex >= 0 && plasmoid.nativeInterface.menuIsShown) {
                        //! as it appears this codepath when triggered from buttons entering under x11
                        //! does not work because the shown menu gets the grabber. So under
                        //! x11 the applet event filter is responsible for buttons hovering
                        //! to trigger menus showing but for wayland the qml painted buttons
                        //! take up the task
                        plasmoid.nativeInterface.requestActivateIndex(currentIndex);
                    }
                }

                readonly property bool containsMouse: {
                    if (currentIndex>=0 || fullViewBackMousearea.containsMouse) {
                        return true;
                    }

                    for (var i=0; i<buttonGrid.children.length; ++i) {
                        if (buttonGrid.children[i] && buttonGrid.children[i] !== buttonRepeater && buttonGrid.children[i].containsMouse) {
                            return true;
                        }
                    }

                    return false;
                }

                Repeater {
                    id: buttonRepeater
                    model: {
                        if (appMenuModel.visible
                                   && appMenuModel.menuAvailable
                                   && !appMenuModel.ignoreWindow
                                   && !broadcaster.hiddenFromBroadcast
                                   && !(inEditMode && !appMenuModel.selectedTracker)) {
                            return appMenuModel;
                        } else if (inEditMode) {
                            return editModeModel;
                        }

                        return null;
                    }

                    PaintedToolButton{
                        id:menuItem

                        Layout.minimumWidth: broadcaster.hiddenFromBroadcast && !inEditMode ? 0 : implicitWidth
                        Layout.preferredWidth: Layout.minimumWidth

                        Layout.minimumHeight: fullLayout.height
                        Layout.preferredHeight: Layout.minimumHeight

                        visible: activeMenu !== ""

                        buttonIndex: index
                        screenEdgeMargin: root.screenEdgeMargin
                        text: activeMenu                        

                        onClicked: {
                            plasmoid.nativeInterface.trigger(this, index);
                        }

                        onScrolledUp: {
                            if (gridFlickable.contentsExceed) {
                                gridFlickable.decreaseX(step);
                            }
                        }

                        onScrolledDown: {
                            if (gridFlickable.contentsExceed) {
                                gridFlickable.increaseX(step);
                            }
                        }
                    }
                }
            }
        } //end of flickable

        FlickableIndicators{
            anchors.fill: parent

            leftIndicatorOpacity: gridFlickable.contentX / gridFlickable.contentsExtraSpace;
            rightIndicatorOpacity: (gridFlickable.contentsExtraSpace - gridFlickable.contentX) / gridFlickable.contentsExtraSpace
        }

        //This Loader is to support maximize/restore active window for plasma panels. Latte panels are not and should not be influenced by this implementation
        Loader {
            active: plasmoid.configuration.fillWidth && (plasmoid.configuration.toggleMaximizedOnDoubleClick || plasmoid.configuration.toggleMaximizedOnMouseWheel) && containmentType !== 2
            anchors.fill: parent
            sourceComponent: Component {
                MouseArea {
                    anchors.right: parent.right
                    width: parent.width - gridFlickable.contentWidth
                    height: parent.height

                    TaskManager.TasksModel {
                        id: tasksModel
                        filterByScreen: plasmoid.configuration.filterByScreen
                        screenGeometry: plasmoid.screenGeometry        
                    }

                    onDoubleClicked: {
                        if(plasmoid.configuration.toggleMaximizedOnDoubleClick){
                            tasksModel.requestToggleMaximized(tasksModel.activeTask)
                        }         
                    }

                    onWheel: {
                        if(plasmoid.configuration.toggleMaximizedOnMouseWheel){
                            var isMaximized = tasksModel.data(tasksModel.activeTask, TaskManager.AbstractTasksModel.IsMaximized)
                            if (wheel.angleDelta.y > 0 && !isMaximized) {
                                tasksModel.requestToggleMaximized(tasksModel.activeTask)           
                            } else if(wheel.angleDelta.y < 0 && isMaximized){
                                tasksModel.requestToggleMaximized(tasksModel.activeTask)
                            }
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
        screenGeometry: plasmoid.configuration.filterByScreen && !latteBridge ? plasmoid.screenGeometry : Qt.rect(-1, -1, 0, 0) //null geometry
        onRequestActivateIndex: plasmoid.nativeInterface.requestActivateIndex(index)
        Component.onCompleted: {
            plasmoid.nativeInterface.model = appMenuModel
        }

        winId: latteBridge && existsWindowShown ? lastActiveTaskItem.winId : -1

        readonly property bool ignoreWindow: {
            var activeFilter = plasmoid.configuration.filterByActive ? !existsWindowActive || !existsWindowShown : false;
            var maximizedFilter = plasmoid.configuration.filterByMaximized ?  !isLastActiveWindowMaximized : false;

            return (activeFilter || maximizedFilter);
        }
    }


    //! Example model in order to be used in edit mode when there is no
    //! other menu available
    ListModel {
        id: editModeModel

        ListElement {
            activeMenu: "File"
        }
        ListElement {
            activeMenu: "Edit"
        }
        ListElement {
            activeMenu: "Help"
        }
    }

    //! this timer is used in order to identify in which containment the applet is in
    //! it should be called only the first time an applet is created and loaded because
    //! afterwards the applet has no way to move between different processes such
    //! as Plasma and Latte
    Timer{
        id: containmentIdentifierTimer
        interval: 5000
        onTriggered: {
            if (latteBridge) {
                plasmoid.configuration.containmentType = 2; /*Latte containment with new API*/
                latteBridge.actions.broadcastToApplet("org.kde.windowtitle", "isPresent", true);
                latteBridge.actions.broadcastToApplet("org.kde.windowtitle", "menuIsPresent", broadcaster.menuIsPresent);
            } else {
                plasmoid.configuration.containmentType = 1; /*Plasma containment or Latte with old API*/
            }
        }
    }
}
