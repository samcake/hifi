//
//  General.qml
//
//  Created by Zach Fox on 2019-05-06
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.10
import "../../simplifiedConstants" as SimplifiedConstants
import "../../simplifiedControls" as SimplifiedControls
import stylesUit 1.0 as HifiStylesUit
import QtQuick.Layouts 1.3
import PerformanceEnums 1.0

Flickable {
    property string avatarNametagMode: Settings.getValue("simplifiedNametag/avatarNametagMode", "on")
    id: root
    contentWidth: parent.width
    contentHeight: generalColumnLayout.height
    topMargin: 24
    bottomMargin: 24
    clip: true

    onAvatarNametagModeChanged: {
        sendNameTagInfo({method: 'handleAvatarNametagMode', avatarNametagMode: root.avatarNametagMode, source: "SettingsApp.qml"});
    }

    onVisibleChanged: {
        if (visible) {
            root.contentX = 0;
            root.contentY = -root.topMargin;
        }
    }

    SimplifiedConstants.SimplifiedConstants {
        id: simplifiedUI
    }

    ColumnLayout {
        id: generalColumnLayout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: simplifiedUI.margins.settings.spacingBetweenSettings

        ColumnLayout {
            id: avatarNameTagsContainer
            Layout.preferredWidth: parent.width
            spacing: 0

            HifiStylesUit.GraphikRegular {
                id: avatarNameTagsTitle
                text: "Avatar Name Tags"
                Layout.maximumWidth: parent.width
                height: paintedHeight
                size: 22
                color: simplifiedUI.colors.text.white
            }

            ColumnLayout {
                id: avatarNameTagsRadioButtonGroup
                Layout.topMargin: simplifiedUI.margins.settings.settingsGroupTopMargin
                spacing: simplifiedUI.margins.settings.spacingBetweenRadiobuttons

                SimplifiedControls.RadioButton {
                    id: avatarNameTagsOff
                    text: "Off"
                    checked: root.avatarNametagMode === "off"
                    onClicked: {
                        root.avatarNametagMode = "off"
                    }
                }

                SimplifiedControls.RadioButton {
                    id: avatarNameTagsAlwaysOn
                    text: "Always On"
                    checked: root.avatarNametagMode === "alwaysOn"
                    onClicked: {
                        root.avatarNametagMode = "alwaysOn"
                    }
                }

                SimplifiedControls.RadioButton {
                    id: avatarNameTagsClickToView
                    text: "Click to View"
                    checked: root.avatarNametagMode === "on"
                    onClicked: {
                        root.avatarNametagMode = "on"
                    }
                }
            }
        }

        ColumnLayout {
            id: performanceContainer
            Layout.preferredWidth: parent.width
            spacing: 0

            HifiStylesUit.GraphikRegular {
                id: performanceTitle
                text: "Performance"
                Layout.maximumWidth: parent.width
                height: paintedHeight
                size: 22
                color: simplifiedUI.colors.text.white
            }

            ColumnLayout {
                id: performanceRadioButtonGroup
                Layout.topMargin: simplifiedUI.margins.settings.settingsGroupTopMargin
                spacing: simplifiedUI.margins.settings.spacingBetweenRadiobuttons

                SimplifiedControls.RadioButton {
                    id: performanceLow
                    text: "Eco"
                    checked: Performance.getRefreshRateProfile() === RefreshRate.ECO
                    onClicked: {
                        Performance.setRefreshRateProfile(RefreshRate.ECO);
                    }
                }

                SimplifiedControls.RadioButton {
                    id: performanceMedium
                    text: "Interactive"
                    checked: Performance.getRefreshRateProfile() === RefreshRate.INTERACTIVE
                    onClicked: {
                        Performance.setRefreshRateProfile(RefreshRate.INTERACTIVE);
                    }
                }

                SimplifiedControls.RadioButton {
                    id: performanceHigh
                    text: "Realtime"
                    checked: Performance.getRefreshRateProfile() === RefreshRate.REALTIME
                    onClicked: {
                        Performance.setRefreshRateProfile(RefreshRate.REALTIME);
                    }
                }
            }
        }

        ColumnLayout {
            id: cameraContainer
            Layout.preferredWidth: parent.width
            spacing: 0

            HifiStylesUit.GraphikRegular {
                id: cameraTitle
                text: "Camera View"
                Layout.maximumWidth: parent.width
                height: paintedHeight
                size: 22
                color: simplifiedUI.colors.text.white
            }

            ColumnLayout {
                id: cameraRadioButtonGroup
                Layout.topMargin: simplifiedUI.margins.settings.settingsGroupTopMargin
                spacing: simplifiedUI.margins.settings.spacingBetweenRadiobuttons

                SimplifiedControls.RadioButton {
                    id: firstPerson
                    text: "First Person View"
                    checked: Camera.mode === "first person"
                    onClicked: {
                        Camera.mode = "first person"
                    }
                }

                SimplifiedControls.RadioButton {
                    id: thirdPerson
                    text: "Third Person View"
                    checked: Camera.mode === "third person"
                    onClicked: {
                        Camera.mode = "third person"
                    }
                }
                
              Connections {
                    target: Camera

                    onModeUpdated: {
                        if (Camera.mode === "first person") {
                            firstPerson.checked = true
                        } else if (Camera.mode === "third person") {
                            thirdPerson.checked = true
                        }
                    }
              }
            }
        }

        HifiStylesUit.GraphikRegular {
            id: logoutText
            text: (AccountServices.username === "Unknown user" ? "Log In" : "Logout " + AccountServices.username)
            wrapMode: Text.Wrap
            width: paintedWidth
            height: paintedHeight
            size: 22
            color: simplifiedUI.colors.text.lightBlue

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color = simplifiedUI.colors.text.lightBlueHover;
                }
                onExited: {
                    parent.color = simplifiedUI.colors.text.lightBlue;
                }
                onClicked: {
                    if (Account.loggedIn) {
                        AccountServices.logOut();
                    } else {
                        DialogsManager.showLoginDialog();
                    }
                }
            }
        }
    }

    signal sendNameTagInfo(var message);
}
