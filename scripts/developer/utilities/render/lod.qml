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
                    prop: "displayFPS",
                    label: "Display FPS",
                    color: "#FED959"
                },
                {
                    prop: "engineFPS",
                    label: "Engine FPS",
                    color: "#00B4EF"
                },
                {
                    prop: "lodDecreaseFPS",
                    label: "Min FPS",
                    color: "#FF6309"
                }
            ]
        }
        PlotPerf {
            title: "LOD"
            height: parent.evalEvenHeight()
            object: LODManager
            valueScale: 0.1
            valueUnit: ""
            plots: [
                {
                    prop: "lodLevel",
                    label: "LOD",
                    color: "#A2277C"
                }
            ]
        }

        Column {
            anchors.left: parent.left
            anchors.right: parent.right 
            spacing: 8
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
            ConfigSlider {
                label: "LOD -- Speed"
                integral: false
                config: LODManager
                property: "decreaseSpeed"
                max: 5
                min: 0

                anchors.left: parent.left
                anchors.right: parent.right 

                onValueChanged: { LODManager["decreaseSpeed"] = value; newStyle() }
            }
            ConfigSlider {
                label: "LOD ++ Speed"
                integral: false
                config: LODManager
                property: "increaseSpeed"
                max: 5
                min: 0

                anchors.left: parent.left
                anchors.right: parent.right 

                onValueChanged: { LODManager["increaseSpeed"] = value; newStyle() }
            }
        }
    }
}
