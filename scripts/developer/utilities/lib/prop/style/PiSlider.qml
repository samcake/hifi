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
    id: control
    Global { id: global }
    value: 0
    height: global.slimHeight

    property alias minimumValue: control.from
    property alias maximumValue: control.to
    property alias step: control.stepSize

    background: Rectangle {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: global.sliderWidth
        implicitHeight: global.sliderGrooveHeight
        width: control.availableWidth
        height: global.sliderGrooveHeight
        radius: 2
        color: global.colorBorderLighter

        Rectangle {
            width: control.visualPosition * parent.width
            height: parent.height
            color: global.colorBorderHighight
            radius: 2
        }
    }

    handle: Rectangle {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: global.sliderHandleWidth
        implicitHeight: global.sliderHandleHeight
        radius: global.slimHeight / 2
        color: global.colorBorderHighight
        border.color: control.pressed ? global.colorBorderLighter : global.colorBorderLight
    }
}