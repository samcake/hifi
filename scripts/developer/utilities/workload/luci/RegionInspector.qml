//
//  Workload Region Inspector.qml
//
//  Created by Sam Gateau on 7/11/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "../../lib/prop" as Prop

Column {
    anchors.left: parent.left 
    anchors.right: parent.right 
    Prop.PropBool {
        label: "Show Regions"
        object: Workload.getConfig("SpaceToRender")
        property: "showViews"
    }
    Prop.PropVec234 {
        label: "R1 t/b/f"
        object: Workload.getConfig("controlViews")
        property: "r1Q"
    }
    Prop.PropVec234 {
        label: "R2 t/b/f"
        object: Workload.getConfig("controlViews")
        property: "r2Q"
    }
    Prop.PropVec234 {
        label: "R3 t/b/f"
        object: Workload.getConfig("controlViews")
        property: "r3Q"
    }
    Prop.PropScalar {
        label: "Region 1"
        object: Workload.getConfig("regionState")
        property: "numR1"
        readOnly: true
        integral: true
    }
    Prop.PropScalar {
        label: "Region 2"
        object: Workload.getConfig("regionState")
        property: "numR2"
        readOnly: true
        integral: true
    }
    Prop.PropScalar {
        label: "Region 3"
        object: Workload.getConfig("regionState")
        property: "numR3"
        readOnly: true
        integral: true
    }
}
