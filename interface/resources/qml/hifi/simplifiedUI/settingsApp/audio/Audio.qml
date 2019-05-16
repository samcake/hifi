//
//  Audio.qml
//
//  Created by Zach Fox on 2019-05-06
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.10
import QtQuick.Controls 2.3
import "../../simplifiedConstants" as SimplifiedConstants
import "../../simplifiedControls" as SimplifiedControls
import stylesUit 1.0 as HifiStylesUit
import QtQuick.Layouts 1.3

Flickable {
    id: root
    contentWidth: parent.width
    contentHeight: audioColumnLayout.height
    topMargin: 16
    bottomMargin: 16
    clip: true

    function changePeakValuesEnabled(enabled) {
        if (!enabled) {
            AudioScriptingInterface.devices.input.peakValuesEnabled = true;
        }
    }

    onVisibleChanged: {
        AudioScriptingInterface.devices.input.peakValuesEnabled = visible;
        if (visible) {
            root.contentX = 0;
            root.contentY = -root.topMargin;
            AudioScriptingInterface.devices.input.peakValuesEnabledChanged.connect(changePeakValuesEnabled);
        } else {
            AudioScriptingInterface.devices.input.peakValuesEnabledChanged.disconnect(changePeakValuesEnabled);
        }
    }


    SimplifiedConstants.SimplifiedConstants {
        id: simplifiedUI
    }

    ColumnLayout {
        id: audioColumnLayout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: simplifiedUI.margins.settings.spacingBetweenSettings
        
        ColumnLayout {
            id: volumeControlsContainer
            Layout.preferredWidth: parent.width
            spacing: 0

            HifiStylesUit.GraphikRegular {
                id: volumeControlsTitle
                text: "Volume Controls"
                Layout.maximumWidth: parent.width
                height: paintedHeight
                size: 22
                color: simplifiedUI.colors.text.white
            }

            SimplifiedControls.Slider {
                id: peopleVolume
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.topMargin: simplifiedUI.margins.settings.settingsGroupTopMargin
                height: 30
                labelText: "People Volume"
                from: -60
                to: 10
                defaultValue: 0.0
                value: AudioScriptingInterface.getAvatarGain()
                live: true
                onValueChanged: {
                    if (AudioScriptingInterface.getAvatarGain() != peopleVolume.value) {
                        AudioScriptingInterface.setAvatarGain(peopleVolume.value);
                    }
                }
            }

            SimplifiedControls.Slider {
                id: environmentVolume
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.topMargin: 2
                height: 30
                labelText: "Environment Volume"
                from: -60
                to: 10
                defaultValue: 0.0
                value: AudioScriptingInterface.getInjectorGain()
                live: true
                onValueChanged: {
                    if (AudioScriptingInterface.getInjectorGain() != environmentVolume.value) {
                        AudioScriptingInterface.setInjectorGain(environmentVolume.value);
                    }
                }
            }

            SimplifiedControls.Slider {
                id: systemSoundVolume
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.topMargin: 2
                height: 30
                labelText: "System Sound Volume"
                from: -60
                to: 10
                defaultValue: 0.0
                value: AudioScriptingInterface.getSystemInjectorGain()
                live: true
                onValueChanged: {
                    if (AudioScriptingInterface.getSystemInjectorGain() != systemSoundVolume.value) {
                        AudioScriptingInterface.setSystemInjectorGain(systemSoundVolume.value);
                    }
                }
            }
        }
        
        ColumnLayout {
            id: micControlsContainer
            Layout.preferredWidth: parent.width
            spacing: 0

            HifiStylesUit.GraphikRegular {
                id: micControlsTitle
                text: "Default Mute Controls"
                Layout.maximumWidth: parent.width
                height: paintedHeight
                size: 22
                color: simplifiedUI.colors.text.white
            }

            ColumnLayout {
                id: micControlsSwitchGroup
                Layout.topMargin: simplifiedUI.margins.settings.settingsGroupTopMargin

                SimplifiedControls.Switch {
                    id: muteMicrophoneSwitch
                    width: parent.width
                    height: 18
                    labelTextOn: "Mute Microphone"
                    checked: AudioScriptingInterface.mutedDesktop
                    onClicked: {
                        AudioScriptingInterface.mutedDesktop = !AudioScriptingInterface.mutedDesktop;
                    }
                }

                SimplifiedControls.Switch {
                    id: pushToTalkSwitch
                    width: parent.width
                    height: 18
                    labelTextOn: "Push to Talk - Press and Hold \"T\" to Talk"
                    checked: AudioScriptingInterface.pushToTalkDesktop
                    onClicked: {
                        AudioScriptingInterface.pushToTalkDesktop = !AudioScriptingInterface.pushToTalkDesktop;
                    }
                }
            }
        }

        ColumnLayout {
            id: inputDeviceContainer
            Layout.preferredWidth: parent.width
            spacing: 0

            HifiStylesUit.GraphikRegular {
                id: inputDeviceTitle
                text: "Which input device?"
                Layout.maximumWidth: parent.width
                height: paintedHeight
                size: 22
                color: simplifiedUI.colors.text.white
            }

            ButtonGroup { id: inputDeviceButtonGroup }

            ListView {
                id: inputDeviceListView
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.topMargin: simplifiedUI.margins.settings.settingsGroupTopMargin
                interactive: false
                height: contentItem.height
                spacing: 4
                clip: true
                model: AudioScriptingInterface.devices.input
                delegate: Item {
                    width: parent.width
                    height: inputDeviceCheckbox.height

                    SimplifiedControls.RadioButton {
                        id: inputDeviceCheckbox
                        anchors.left: parent.left
                        width: parent.width - inputLevel.width
                        checked: selectedDesktop
                        text: model.devicename
                        ButtonGroup.group: inputDeviceButtonGroup
                        onClicked: {
                            AudioScriptingInterface.setStereoInput(false); // the next selected audio device might not support stereo
                            AudioScriptingInterface.setInputDevice(model.info, false); // `false` argument for Desktop mode setting
                        }
                    }

                    SimplifiedControls.InputPeak {
                        id: inputLevel
                        showMuted: AudioScriptingInterface.mutedDesktop
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        peak: model.peak
                        visible: AudioScriptingInterface.devices.input.peakValuesAvailable
                    }
                }
            }

            SimplifiedControls.Button {
                property bool audioLoopedBack: AudioScriptingInterface.getLocalEcho()
                
                function startAudioLoopback() {
                    if (!audioLoopedBack) {
                        audioLoopedBack = true;
                        AudioScriptingInterface.setLocalEcho(true);
                    }
                }
                function stopAudioLoopback() {
                    if (audioLoopedBack) {
                        audioLoopedBack = false;
                        AudioScriptingInterface.setLocalEcho(false);
                    }
                }

                Timer {
                    id: loopbackTimer
                    interval: 8000
                    running: false
                    repeat: false
                    onTriggered: {
                        stopAudioLoopback();
                    }
                }

                id: testYourMicButton
                enabled: !HMD.active
                anchors.left: parent.left
                Layout.topMargin: simplifiedUI.margins.settings.settingsGroupTopMargin
                width: 160
                height: 32
                text: audioLoopedBack ? "STOP TESTING" : "TEST YOUR MIC"
                onClicked: {
                    if (audioLoopedBack) {
                        loopbackTimer.stop();
                        stopAudioLoopback();
                    } else {
                        loopbackTimer.restart();
                        startAudioLoopback();
                    }
                }
            }
        }

        ColumnLayout {
            id: outputDeviceContainer
            Layout.preferredWidth: parent.width
            spacing: 0

            HifiStylesUit.GraphikRegular {
                id: outputDeviceTitle
                text: "Which output device?"
                Layout.maximumWidth: parent.width
                height: paintedHeight
                size: 22
                color: simplifiedUI.colors.text.white
            }

            ButtonGroup { id: outputDeviceButtonGroup }

            ListView {
                id: outputDeviceListView
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.topMargin: simplifiedUI.margins.settings.settingsGroupTopMargin
                interactive: false
                height: contentItem.height
                spacing: 4
                clip: true
                model: AudioScriptingInterface.devices.output
                delegate: Item {
                    width: parent.width
                    height: outputDeviceCheckbox.height

                    SimplifiedControls.RadioButton {
                        id: outputDeviceCheckbox
                        anchors.left: parent.left
                        width: parent.width
                        checked: selectedDesktop
                        text: model.devicename
                        ButtonGroup.group: outputDeviceButtonGroup
                        onClicked: {
                            AudioScriptingInterface.setOutputDevice(model.info, false); // `false` argument for Desktop mode setting
                        }
                    }
                }
            }

            SimplifiedControls.Button {
                property var sound: null
                property var sample: null
                property bool isPlaying: false
                function createSampleSound() {
                    sound = ApplicationInterface.getSampleSound();
                    sample = null;
                }
                function playSound() {
                    // FIXME: MyAvatar is not properly exposed to QML; MyAvatar.qmlPosition is a stopgap
                    // FIXME: AudioScriptingInterface.playSystemSound should not require position
                    if (sample === null && !isPlaying) {
                        sample = AudioScriptingInterface.playSystemSound(sound, MyAvatar.qmlPosition);
                        isPlaying = true;
                        sample.finished.connect(reset);
                    }
                }
                function stopSound() {
                    if (sample && isPlaying) {
                        sample.stop();
                    }
                }

                function reset() {
                    sample.finished.disconnect(reset);
                    isPlaying = false;
                    sample = null;
                }

                Component.onCompleted: createSampleSound();
                Component.onDestruction: stopSound();

                onVisibleChanged: {
                    if (!visible) {
                        stopSound();
                    }
                }

                id: testYourSoundButton
                enabled: !HMD.active
                anchors.left: parent.left
                Layout.topMargin: simplifiedUI.margins.settings.settingsGroupTopMargin
                width: 160
                height: 32
                text: isPlaying ? "STOP TESTING" : "TEST YOUR SOUND"
                onClicked: {
                    isPlaying ? stopSound() : playSound();
                }
            }
        }
    }
}
