//
//  PlotPerf.qml
//  examples/utilities/render/plotperf
//
//  Created by Sam Gateau on 3//2016
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//
import QtQuick 2.5
import QtQuick.Controls 1.4

Item {
    id: root
    width: parent.width
    height: 100

    // The title of the graph
    property string title

    // The object used as the default source object for the prop plots
    property var object

    property var backgroundOpacity: 0.6

    property var points

    // Default value scale used to define the max value of the chart
    property var valueScale: 1

    // Default value unit appended to the value displayed
    property var valueUnit: ""

    // Default number of digits displayed
    property var valueNumDigits: 0
    

    property var valueMax : 1
    property var valueMin : 0

    property var displayMinAt0 : true
    property var _displayMaxValue : 1
    property var _displayMinValue : 0.1

    property var _values
    property var tick : 0

    function createValues() {
        pullFreshValues();
    }

    Component.onCompleted: {
        createValues();   
    }

    function pullFreshValues() {
        // Wait until values are created to begin pulling
        if (!_values) { return; }

        var VALUE_HISTORY_SIZE = 100;
        tick++;

        mycanvas.requestPaint()
    }

    Timer {
        interval: 100; running: true; repeat: true
        onTriggered: pullFreshValues()
    }

    Canvas {
        id: mycanvas
        anchors.fill:parent
        
        onPaint: {
            var lineHeight = 12;

            function displayValue(val, unit) {
                 return (val / root.valueScale).toFixed(root.valueNumDigits) + " " + unit
            }

            function pixelFromVal(val, valScale) {
                return lineHeight + (height - lineHeight) * (1 - (0.99) * (val - _displayMinValue) / (_displayMaxValue - _displayMinValue));
            }
            function valueFromPixel(pixY) {
                return _displayMinValue + (((pixY - lineHeight) / (height - lineHeight) - 1) * (_displayMaxValue - _displayMinValue) / (-0.99));
            }
            function plotValueHistory(ctx, valHistory, color) {
                var widthStep= width / (valHistory.length - 1);

                ctx.beginPath();
                ctx.strokeStyle= color; // Green path
                ctx.lineWidth="2";
                ctx.moveTo(0, pixelFromVal(valHistory[0])); 
                   
                for (var i = 1; i < valHistory.length; i++) { 
                    ctx.lineTo(i * widthStep, pixelFromVal(valHistory[i])); 
                }

                ctx.stroke();
            }

            function plot(ctx) {
                ctx.beginPath();
                ctx.strokeStyle= Qt.rgba(1, 1, 1, 1);
                ctx.lineWidth="1";
                ctx.moveTo(0, height); 
                
                //ctx.bezierCurveTo(0, height, 20, height - 10, 50, 50); 

                var point = points[0];
                var value = point["prop"];

                var val = point.object[value];

                ctx.bezierCurveTo(0, height, 20, height - 10, 50, 60 + val); 
                
                ctx.stroke();
            }
            
            function displayTitle(ctx, text, maxVal) {
                ctx.fillStyle = "grey";
                ctx.textAlign = "right";
               
                ctx.fillStyle = "white";
                ctx.textAlign = "left";
                ctx.fillText(text, 0, lineHeight);
            }

            function displayBackground(ctx) {
                ctx.fillStyle = Qt.rgba(0, 0, 0, root.backgroundOpacity);
                ctx.fillRect(0, 0, width, height);
            }
            
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            ctx.font="12px Verdana";

            displayBackground(ctx);
                
            plot(ctx);

            displayTitle(ctx, title, _displayMaxValue)
        }
    }

    MouseArea {
        id: hitbox
        anchors.fill: mycanvas

        onClicked: {
        }
    }
}
