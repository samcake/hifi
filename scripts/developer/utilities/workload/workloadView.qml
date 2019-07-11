//
//  workloadView.qml
//
//  Created by Sam Gateau on 7/9/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//
import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

import controlsUit 1.0 as HifiControls

import "../lib/prop" as Prop
import "./luci"

Rectangle {
    anchors.fill: parent 
    id: platform;   
    
    Prop.Global { id: global;}
    color: global.colorBack

    ScrollView {
        id: scrollView
        anchors.fill: parent 
        contentWidth: parent.width
        clip: true

        Column { 
            anchors.left: parent.left 
            anchors.right: parent.right 

            Prop.PropFolderPanel {
                label: "Space Inspector"
                isUnfold: true
                panelFrameData: Component {
                    SpaceInspector {
                    }
                }
            }
            Prop.PropFolderPanel {
                label: "Views Inspector"
                isUnfold: true
                panelFrameData: Component {
                    ViewsInspector {
                    }
                }
            }

            Prop.PropFolderPanel {
                label: "Phase Inspector"
                isUnfold: true
                panelFrameData: Component {
                    PhaseInspector {
                    }
                }
            }
        }
    }
}

