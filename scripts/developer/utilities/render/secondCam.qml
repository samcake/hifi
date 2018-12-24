//
//  secondCam.qml
//  scripts/developer/utilities/render
//
//  Created by Sam Gateau on 2018.12.24
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//
import QtQuick 2.5
import QtQuick.Controls 1.4

import stylesUit 1.0
import controlsUit 1.0 as HifiControls

import "../lib/plotperf"
import "configSlider"

Item {
    id: secondCamRoot
    anchors.fill:parent

    Component.onCompleted: {
    }

    Component.onDestruction: {
        sendToScript({method: "kill2ndCam"})
    }

    property var secondaryCamTask: Render.getConfig("SecondaryCameraJob");
    property var secondaryCam: Render.getConfig("SecondaryCamera");

    Column {
        id: topHeader
        spacing: 8
        anchors.right: parent.right
        anchors.left: parent.left
        HifiControls.Button {
            text: "Spawn"
            // activeFocusOnPress: false
            onClicked: { sendToScript({method: "spawn2ndCam"}); }
        }

        HifiControls.CheckBox {
            boxSize: 20
            text: "2nd Cam Renders"
            checked: secondaryCamTask.enabled
            onCheckedChanged: { secondaryCamTask.enabled = checked }
        }

        HifiControls.CheckBox {
            boxSize: 20
            text: "Stereo"
            checked: secondaryCam.stereo
            onCheckedChanged: { secondaryCam.stereo = checked }
        }

        RichSlider {
            showLabel: true
            showValue: true
            label: "Stereo Interaxial"
            valueVar: secondaryCam["stereoEyeInteraxial"]
            valueVarSetter: (function (v) { secondaryCam["stereoEyeInteraxial"] = v })
            max: 1.0
            min: 0.0
            integral: false

            anchors.left: parent.left
            anchors.right: parent.right 
        }
        RichSlider {
            showLabel: true
            showValue: true
            label: "Near"
            valueVar: secondaryCam["nearClipPlaneDistance"]
            valueVarSetter: (function (v) { secondaryCam["nearClipPlaneDistance"] = v })
            max: 1000.0
            min: 0.01
            integral: false

            anchors.left: parent.left
            anchors.right: parent.right 
        }
        RichSlider {
            showLabel: true
            showValue: true
            label: "Near"
            valueVar: secondaryCam["farClipPlaneDistance"]
            valueVarSetter: (function (v) { secondaryCam["farClipPlaneDistance"] = v })
            max: 1000.0
            min: 0.01
            integral: false

            anchors.left: parent.left
            anchors.right: parent.right 
        }
        HifiControls.CheckBox {
            boxSize: 20
            text: "Mirror mode"
            checked: secondaryCam.mirrorProjection
            onCheckedChanged: { secondaryCam.mirrorProjection = checked }
        }
        HifiControls.CheckBox {
            boxSize: 20
            text: "Portal mode"
            checked: secondaryCam.portalProjection
            onCheckedChanged: { secondaryCam.portalProjection = checked }
        }
        HifiControls.Label {
            text: "Cam -> EntityID = " + secondaryCam.attachedEntityId 
        }
        HifiControls.Label {
            text: "Cam -> portalEntryEntityID = " + secondaryCam.portalEntranceEntityId 
        }
    }
}
