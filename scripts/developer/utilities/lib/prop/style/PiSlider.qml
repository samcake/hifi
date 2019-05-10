//
//  Prop/style/PiSlider.qml
//
//  Created by Sam Gateau on 5/9/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.7
import QtQuick.Controls 2.2

Slider {
    id: slider
    Global { id: global }
    value: 0
    height: global.slimHeight

    property alias minimumValue: slider.from
    property alias maximumValue: slider.to
    property alias step: slider.stepSize


    /*background: Rectangle {
        implicitWidth: 200
        implicitHeight: 8
        color: "gray"
        radius: 8
    }*/

    handle: Rectangle {
        x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
        y: slider.topPadding + slider.availableHeight / 2 - height / 2
        implicitWidth: hifi.dimensions.sliderHandleSize
        implicitHeight: hifi.dimensions.sliderHandleSize
        radius: height / 2
        border.width: 1
        border.color: global.colorBorderLight

        Rectangle {
            height: parent.height - 2
            width: height
            radius: height / 2
            anchors.centerIn: parent
            //color: hifi.colors.transparent
            border.width: 1
            border.color: global.colorBorderLight
        }
    }

}