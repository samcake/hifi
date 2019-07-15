//
//  Workload Views Inspector.qml
//
//  Created by Sam Gateau on 7/09/2019
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

    Prop.PropScalar {
        label: "Num Views"
        object: Workload.getConfig("setupViews")
        property: "numViews"
        readOnly: true
        integral: true
    } 

    Prop.PropVec234 {
        label: "Origin"
        object: Workload.getConfig("setupViews")
        property: "originQ"  
        numDigits: 1
    }
    Prop.PropVec234 {
        label: "Direction"
        object: Workload.getConfig("setupViews")
        property: "directionQ"  
        numDigits: 1
    }
    Prop.PropVec234 {
        label: "FOV"
        object: Workload.getConfig("setupViews")
        property: "fovQ" 
        dimension: 4 
        numDigits: 1
    }
    Prop.PropBool {
        label: "Freeze Views"
        object: Workload.getConfig("setupViews")
        property: "freezeViews"
    }
    Prop.PropBool {
        label: "Use Avatar View"
        object: Workload.getConfig("setupViews")
        property: "useAvatarView"
    }
    Prop.PropBool {
        label: "Force View Horizontal"
        object: Workload.getConfig("setupViews")
        property: "forceViewHorizontal"
    }
    Prop.PropBool {
        label: "Simulate Secondary"
        object: Workload.getConfig("setupViews")
        property: "simulateSecondaryCamera"
    }
}
