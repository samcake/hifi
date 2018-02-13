//
//  lod.qml
//  scripts/developer/utilities/render
//
//  Created by Andrew Meadows on 2018.01.10
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//
import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import "qrc:///qml/styles-uit"
import "qrc:///qml/controls-uit" as HifiControls
import "../lib/plotperf"
import  "configSlider"

Item {
    id: lodIU
    anchors.fill:parent
     HifiConstants { id: hifi;}

    property var sceneOctree: Render.getConfig("RenderMainView.DrawSceneOctree");
    property var itemSelection: Render.getConfig("RenderMainView.DrawItemSelection");

    Component.onCompleted: {
        sceneOctree.enabled = true;
        sceneOctree.showVisibleCells = false;
        sceneOctree.showEmptyCells = false;                
    }
    Component.onDestruction: {
        sceneOctree.enabled = false; 
    }

    Column {
        id: stats
        spacing: 8
        anchors.fill:parent

        function evalEvenHeight() {
            // Why do we have to do that manually ? cannot seem to find a qml / anchor / layout mode that does that ?
            return (height - spacing * (children.length - 1)) / children.length
        }

        PlotPerf {
            title: "Duration"
            height: parent.evalEvenHeight()
            object: LODManager
            valueScale: 1
            valueUnit: "ms"
            plots: [
                {
                    prop: "presentTime",
                    label: "present",
                    color: "#FED959"
                },
                {
                    prop: "renderTime",
                    label: "render",
                    color: "#00B4EF"
                },
                {
                    prop: "batchTime",
                    label: "batch",
                    color: "#1AC567"
                },
                {
                    prop: "gpuTime",
                    label: "gpu",
                    color: "#E2334D"
                }
            ]
        }
        PlotPerf {
            title: "FPS"
            height: parent.evalEvenHeight()
            object: LODManager
            valueScale: 1
            valueUnit: "Hz"
            plots: [
                {
                    prop: "lodDecreaseFPS",
                    label: "Min FPS",
                    color: "#FF6309"
                },
                {
                    prop: "displayTargetFPS",
                    label: "Target FPS",
                    color: "#1AC567"
                },
                {
                    prop: "displayFPS",
                    label: "Display FPS",
                    color: "#FED959"
                },
                {
                    prop: "engineFPS",
                    label: "Engine FPS",
                    color: "#00B4EF"
                }
            ]
        }
        Column {
            anchors.left: parent.left
            anchors.right: parent.right 
            spacing: 8
            Row {
                spacing: 8
                HifiControls.CheckBox {
                        boxSize: 20
                        text: "Automatic"
                        checked: LODManager.lodAutomatic
                        onCheckedChanged: { LODManager.lodAutomatic = checked }
                    }
                
                Repeater {
                    model: [
                        "LOD Reticle:RenderMainView.DrawSceneOctree:enabled"
                    ]
                    HifiControls.CheckBox {
                        boxSize: 20
                        text: modelData.split(":")[0]
                        checked: Render.getConfig(modelData.split(":")[1])[modelData.split(":")[2]]
                        onCheckedChanged: { Render.getConfig(modelData.split(":")[1])[modelData.split(":")[2]] = checked }
                    }
                }
            }
            
            ConfigSlider {
                label: "Frequency"
                integral: false
                config: LODManager
                property: "numLoops"
                max: 8
                min: 0
                anchors.left: parent.left
                anchors.right: parent.right 
            }
            ConfigSlider {
                label: "LOD"
                integral: false
                config: LODManager
                property: "lodNormalized"
                max: 1.5
                min: 0.02

                anchors.left: parent.left
                anchors.right: parent.right 
            }
            ConfigSlider {
                label: "PID Kp"
                integral: false
                config: LODManager
                property: "pidControlKp"
                max: 0.1
                min: 0
                valueNumDigits: 3               
                anchors.left: parent.left
                anchors.right: parent.right 
           }
           ConfigSlider {
                label: "PID Ki"
                integral: false
                config: LODManager
                property: "pidControlKi"
                max: 0.01
                min: 0               
                valueNumDigits: 4
                anchors.left: parent.left
                anchors.right: parent.right       
           }
           ConfigSlider {
                label: "PID Kd"
                integral: false
                config: LODManager
                property: "pidControlKd"
                max: 0.1
                min: -0.1
                valueNumDigits: 4               
                anchors.left: parent.left
                anchors.right: parent.right 
           }
        }
        PlotPerf {
            title: "PID"
            height: parent.evalEvenHeight()
            object: LODManager
            valueScale: 1
            valueUnit: ""
            plots: [
                {
                    prop: "pidError",
                    label: "Error",
                    color: "#FF6309"
                }
            ]
        }
        PlotPerf {
            title: "PID"
            height:  parent.evalEvenHeight()
            object: LODManager
            valueScale: 1
            valueNumDigits: 3
            valueUnit: ""
            plots: [
                {
                    prop: "pidFeedbackP",
                    label: "P",
                    color: "#1AC567"
                },
                {
                    prop: "pidFeedbackI",
                    label: "I",
                    color: "#FED959"
                },
                {
                    prop: "pidFeedbackD",
                    label: "D",
                    color: "#00B4EF"
                },
                {
                    prop: "lodNormalized",
                    label: "LOD",
                    color: "#FFFFFF"
                }
            ]
        }
    }
}
