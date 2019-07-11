//
//  PropVec3.qml
//
//  Created by Sam Gateau on 7/10/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.7

PropItem {
    Global { id: global }
    id: root

    // Vec3  Prop
    property bool integral: false
    property var numDigits: 2
    property var separator: "    "
    property var dimension: 3


    property var sourceValueVar: root.valueVarGetter()

    function valueToString() {
        var text = root.sourceValueVar.x.toFixed(numDigits)
             + separator + root.sourceValueVar.y.toFixed(numDigits)
        if (dimension >= 3) {
            text += separator + root.valueVarGetter().z.toFixed(numDigits);
        }
        return text
    }

    PropLabel {
        id: valueLabel

        anchors.left: root.splitter.right
        anchors.right: root.right
        anchors.verticalCenter: root.verticalCenter
        horizontalAlignment: global.valueTextAlign
        height: global.slimHeight
        
        text:  root.valueToString();

        background: Rectangle {
            color: global.color
            border.color: global.colorBorderLight
            border.width: global.valueBorderWidth
            radius: global.valueBorderRadius
        }
    }
}