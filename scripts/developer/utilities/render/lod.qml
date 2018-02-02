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
import QtQuick 2.5
import QtQuick.Controls 1.4
import "../lib/plotperf"

Item {
    id: lodIU
    anchors.fill:parent

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
                    color: "#E2334D"
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
                    color: "#00FFFF"
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
                    color: "#E2334D"
                },
                {
                    prop: "engineFPS",
                    label: "Engine FPS",
                    color: "#00B4EF"
                },
                {
                    prop: "lodDecreaseFPS",
                    label: "LOD--",
                    color: "#FF6666"
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
                    color: "#9999FF"
                }
            ]
        }
    }
}
