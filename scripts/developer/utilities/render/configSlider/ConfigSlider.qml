//
//  ConfigSlider.qml
//
//  Created by Zach Pomerantz on 2/8/2016
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.7
import QtQuick.Controls 1.4 as Original
import QtQuick.Controls.Styles 1.4

import "qrc:///qml/styles-uit"
import "qrc:///qml/controls-uit" as HifiControls


Item {
    HifiConstants { id: hifi }
    id: root

    anchors.left: parent.left
    anchors.right: parent.right    
    height: 24
    property bool integral: false
    property var config
    property string property
    property alias label: labelControl.text
    property alias min: sliderControl.minimumValue
    property alias max: sliderControl.maximumValue

    // Default number of digits displayed
    property var valueNumDigits: 0
    
    signal valueChanged(real value)

    Component.onCompleted: {
        // Binding favors qml value, so set it first
        sliderControl.value = root.config[root.property];
        bindingControl.when = true;
    }

    HifiControls.Label {
        id: labelControl
        text: root.label
        enabled: true
        anchors.left: root.left
        width: root.width * 2.0 / 5.0
        anchors.verticalCenter: root.verticalCenter
    }

    HifiControls.Slider {
        id: sliderControl
        stepSize: root.integral ? 1.0 : 0.0
        anchors.left: labelControl.right
        width: root.width * 2.0 / 5.0
        anchors.verticalCenter: root.verticalCenter

        onValueChanged: { root.valueChanged(value) }
    }

    HifiControls.Label {
        id: labelValue
        text: sliderControl.value.toFixed(root.integral ? 0 : root.valueNumDigits)
        anchors.left: sliderControl.right
        anchors.right: root.right
        anchors.rightMargin: 0
        anchors.verticalCenter: root.verticalCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Binding {
        id: bindingControl
        target: root.config
        property: root.property
        value: sliderControl.value
        when: false
    }
}
