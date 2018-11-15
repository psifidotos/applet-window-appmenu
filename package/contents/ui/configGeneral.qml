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
import QtQuick.Layouts 1.1 as Layouts

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore

Layouts.ColumnLayout {
    id: configGeneral

    property alias cfg_compactView: compactViewRadioButton.checked

    property bool disableSetting: plasmoid.formFactor === PlasmaCore.Types.Vertical

    Controls.ExclusiveGroup {
        id: viewOptionGroup
    }

    Controls.RadioButton {
        id: compactViewRadioButton
        enabled: !disableSetting
        text: i18n("Use single button for application menu")
        exclusiveGroup: viewOptionGroup
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

    Item {
        Layouts.Layout.fillHeight: true
    }
}
