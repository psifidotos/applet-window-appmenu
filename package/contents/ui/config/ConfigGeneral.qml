/********************************************************************
 * Copyright 2016 Chinmoy Ranjan Pradhan <chinmoyrp65@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

import QtQuick 2.0
import QtQuick.Controls 1.0 as Controls
import QtQuick.Layouts 1.1

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore

import org.kde.kirigami 2.4 as Kirigami

import org.kde.private.windowAppMenu 0.1 as AppMenuPrivate

Item {
    id: configGeneral

    property alias cfg_compactView: compactViewRadioButton.checked
    property alias cfg_fillWidth: fillWidthChk.checked
    property alias cfg_filterByActive: activeChk.checked
    property alias cfg_filterByMaximized: maximizedChk.checked
    property alias cfg_filterChildrenWindows: childrenChk.checked
    property alias cfg_filterByScreen: screenAwareChk.checked
    property alias cfg_selectedScheme: configGeneral.selectedScheme
    property alias cfg_spacing: spacingSlider.value
    property alias cfg_showWindowTitleOnMouseExit: showWindowTitleChk.checked
    property alias cfg_toggleMaximizedOnDoubleClick: toggleMaximizedChk.checked
    property alias cfg_toggleMaximizedOnMouseWheel: toggleMouseWheelMaximizedChk.checked

    property bool disableSetting: plasmoid.formFactor === PlasmaCore.Types.Vertical

    // used as bridge to communicate properly between configuration and ui
    property string selectedScheme

    // used from the ui
    readonly property real centerFactor: 0.3
    readonly property int minimumWidth: 220

    AppMenuPrivate.SchemesModel {
        id: schemesModel

        currentOptionIsShown: plasmoid.configuration.supportsActiveWindowSchemes
    }

    ColumnLayout {
        id:mainColumn
        spacing: units.largeSpacing
        width:parent.width - anchors.leftMargin * 2
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 2

        GridLayout{
            columns: 2

            Controls.ExclusiveGroup {
                id: viewOptionGroup
            }

            Controls.Label{
                Layout.minimumWidth: Math.max(centerFactor * root.width, minimumWidth)
                text: i18n("Buttons:")
                horizontalAlignment: Text.AlignRight
            }

            Controls.RadioButton {
                id: fullViewRadioButton
                //this checked binding is just for the initial load in case
                //compactViewCheckBox is not checked. Then exclusive group manages it
                enabled: !disableSetting
                checked: !compactViewRadioButton.checked
                text: i18n("Show full application menu")
                exclusiveGroup: viewOptionGroup
            }

            Controls.Label{
            }

            Controls.RadioButton {
                id: compactViewRadioButton
                enabled: !disableSetting
                text: i18n("Use single button for application menu")
                exclusiveGroup: viewOptionGroup
            }
        }

        GridLayout{
            columns: 2

            Controls.Label {
                Layout.minimumWidth: Math.max(centerFactor * root.width, minimumWidth)
                text: i18n("Menu Colors:")
                horizontalAlignment: Text.AlignRight
            }

            ColorsComboBox{
                id:colorsCmbBox
                Layout.minimumWidth: 250
                Layout.preferredWidth: 0.3 * root.width
                Layout.maximumWidth: 380

                model: schemesModel
                textRole: "display"

                Component.onCompleted: {
                    currentIndex = schemesModel.indexOf(plasmoid.configuration.selectedScheme);
                }
            }
        }

        GridLayout{
            columns: 2
            enabled: !compactViewRadioButton.checked

            Controls.Label{
                Layout.minimumWidth: Math.max(centerFactor * root.width, minimumWidth)
                text: i18n("Spacing:")
                horizontalAlignment: Text.AlignRight
            }

            RowLayout{
                Controls.Slider {
                    id: spacingSlider
                    minimumValue: 0
                    maximumValue: 36
                    stepSize: 1
                }
                Controls.Label {
                    text: spacingSlider.value + " " + i18n("px.")
                }
            }
        }

        GridLayout{
            columns: 2
            enabled: !compactViewRadioButton.checked
            //! Plasma panels do not support fillWidth(s) easily any more.
            //! This statement needs investigation with use cases in order to be valid
            //visible: plasmoid.configuration.containmentType === 2 /*Latte Containment*/

            Controls.Label{
                Layout.minimumWidth: Math.max(centerFactor * root.width, minimumWidth)
                text: i18n("Behavior:")
                horizontalAlignment: Text.AlignRight
            }

            Controls.CheckBox {
                id: fillWidthChk
                text: i18n("Always use maximum available width")
            }

            Controls.Label{
                text: ""
                visible: showWindowTitleChk.visible
            }

            Controls.CheckBox {
                id: showWindowTitleChk
                text: i18n("Show Window Title applet on mouse exit")
                visible: plasmoid.configuration.containmentType === 2 /*Latte Containment*/
                enabled: plasmoid.configuration.windowTitleIsPresent
            }

            Controls.Label {
                text: ""
                visible: toggleMaximizedChk.visible
            }

            Controls.CheckBox {
                id: toggleMaximizedChk
                text: i18n("Maximize/restore active window on double click")
                visible: plasmoid.configuration.containmentType !== 2 /*non-Latte Containment*/
                enabled: fillWidthChk.checked
            }

            Controls.Label {
                text: ""
                visible: toggleMouseWheelMaximizedChk.visible
            }

            Controls.CheckBox {
                id: toggleMouseWheelMaximizedChk
                text: i18n("Maximize/restore active window on mouse wheel up/down")
                visible: plasmoid.configuration.containmentType !== 2 /*non-Latte Containment*/
                enabled: fillWidthChk.checked
            }
        }

        Kirigami.InlineMessage {
            id: inlineMessage
            Layout.fillWidth: true
            Layout.bottomMargin: 5

            type: Kirigami.MessageType.Warning
            text: cfg_showWindowTitleOnMouseExit ?
                      i18n("Would you like <b>also to activate</b> that behavior to surrounding Window Title?") :
                      i18n("Would you like <b>also to deactivate</b> that behavior to surrounding Window Title?")

            actions: [
                Kirigami.Action {
                    icon.name: "dialog-yes"
                    text: i18n("Yes")
                    onTriggered: {
                        plasmoid.configuration.sendActivateWindowTitleCooperationFromEditMode = cfg_showWindowTitleOnMouseExit;
                        inlineMessage.visible = false;
                    }
                },
                Kirigami.Action {
                    icon.name: "dialog-no"
                    text: "No"
                    onTriggered: {
                        inlineMessage.visible = false;
                    }
                }
            ]

            readonly property bool showWindowTitleTouched: showWindowTitleChk.checked !== plasmoid.configuration.showWindowTitleOnMouseExit;

            onShowWindowTitleTouchedChanged: {
                if (plasmoid.configuration.containmentType !== 2 /*Latte Containment*/) {
                    visible = false;
                    return;
                }

                if (showWindowTitleTouched){
                    inlineMessage.visible = true;
                } else {
                    inlineMessage.visible = false;
                }
            }
        }

        GridLayout{
            columns: 2

            Controls.Label{
                Layout.minimumWidth: Math.max(centerFactor * root.width, minimumWidth)
                text: i18n("Filters:")
                horizontalAlignment: Text.AlignRight
            }

            Controls.CheckBox {
                id: screenAwareChk
                text: i18n("Show only menus from current screen")
            }

            Controls.Label{}

            Controls.CheckBox {
                id: activeChk
                text: i18n("Show only menus from active applications")
            }

            Controls.Label{}

            Controls.CheckBox {
                id: childrenChk
                text: i18n("Show only menus from main window")
            }

            Controls.Label{}

            Controls.CheckBox {
                id: maximizedChk
                text: i18n("Show only menus from maximized windows")
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
