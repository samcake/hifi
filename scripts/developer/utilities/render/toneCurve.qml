//
//  ToneMapping.qml
//
//  Created by Sam Gateau on 4/17/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.7
import QtQuick.Controls 1.4
import "../lib/prop" as Prop
import "../lib/plotperf"


Column {
    id: render2;   
    
    //color: hifi.colors.baseGray;

    //anchors.fill: parent
    //anchors.left: parent.right
    //anchors.right: parent.right  

    anchors.margins: 10
    
    Row {
        anchors.left: parent.left
        width: parent.width

        spacing: 5

        function itemIndex(item) {
            if (item.parent == null)
                return -1
            var siblings = item.parent.children
            for (var i = 0; i < siblings.length; i++)
                if (siblings[i] == item)
                    return i
            return -1 //will never happen
        }

        function previousItem(item) {
            if (item.parent == null)
                return null
            var index = itemIndex(item)
            return (index > 0)? item.parent.children[itemIndex(item) - 1]: null
        }

        Repeater {
            
            model: [
                "Red:#FF001A",
                "Green:#009036",
                "Blue:#009EE0"
            ]

            Item {
                width: (parent.parent.width - 5) / 3
                height: 400 

                Column {
                    width: parent.width
                    spacing: 5
                    
                    function evalEvenHeight() {
                        // Why do we have to do that manually ? cannot seem to find a qml / anchor / layout mode that does that ?
                        return (height - spacing * (children.length - 1)) / children.length
                    }
    
                    PlotCurve {
                        title: "Tone Curve " + modelData.split(":")[0]
                        width: parent.width - 5
                        object: parent.drawOpaqueConfig
                        valueScale: 1
                        valueNumDigits: 3
                        height: 160
                        color: modelData.split(":")[1]
                        lineWidth: 2
                        points: 
                        [
                            {
                                object: Render.getConfig("RenderMainView.ToneMapping"),
                                prop: "toeLength"
                            },
                            {
                                object: Render.getConfig("RenderMainView.ToneMapping"),
                                prop: "toeStrength"
                            },
                            {
                                object: Render.getConfig("RenderMainView.ToneMapping"),
                                prop: "shoulderLength"
                            }
                        ]
                    }

                    Prop.PropScalar {
                        label: "Exposure"
                        object: Render.getConfig("RenderMainView.ToneMapping")
                        property: "exposure"
                        min: -4
                        max: 4
                        anchors.left: parent.left
                        anchors.right: parent.right 
                    }
            
                    Repeater {
                        model: [
                            "Toe Strength:toeStrength",
                            "Toe Length:toeLength",
                            "Shoulder Strength:shoulderStrength",
                            "Shoulder Length:shoulderLength",
                            "Shoulder Angle:shoulderAngle"
                        ]

                        Prop.PropScalar {
                            label: modelData.split(":")[0]
                            object: Render.getConfig("RenderMainView.ToneMapping")
                            property: modelData.split(":")[1]
                            min: 0
                            max: 1
                            anchors.left: parent.left
                            anchors.right: parent.right 
                        }
                    }

                    Prop.PropScalar {
                        label: "Gamma"
                        object: Render.getConfig("RenderMainView.ToneMapping")
                        property: "gamma"
                        numDigits: 1
                        min: 1
                        max: 3
                        anchors.left: parent.left
                        anchors.right: parent.right 
                    }
                }
            }
        }
    }
}
