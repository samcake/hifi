//
//  PropItem.qml
//
//  Created by Sam Gateau on 3/2/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.7

PropItem {
    Global { id: global }
    id: root

    // Scalar Prop
    property bool integral: false
    property var numDigits: 2


    property alias valueVar : sliderControl.value
    property alias min: sliderControl.minimumValue
    property alias max: sliderControl.maximumValue



    property bool showValue: true  

       
    signal valueChanged(real value)

    Component.onCompleted: {
        valueVar = root.valueVarGetter();
    }

    PropLabel {
        id: valueLabel
        enabled: root.showValue

        anchors.left: root.splitter.right
        anchors.verticalCenter: root.verticalCenter
        width: root.width * global.valueAreaWidthScale
        horizontalAlignment: global.valueTextAlign
        height: global.slimHeight
        
        text: sliderControl.value.toFixed(root.integral ? 0 : root.numDigits)

        background: Rectangle {
            color: global.color
            border.color: global.colorBorderLight
            border.width: global.valueBorderWidth
            radius: global.valueBorderRadius
        }
    }

    PropSlider {
        id: sliderControl
        stepSize: root.integral ? 1.0 : 0.0
        anchors.left: valueLabel.right
        anchors.right: root.right
        anchors.verticalCenter: root.verticalCenter

        onValueChanged: { root.valueVarSetter(value) }
    }

    
}
