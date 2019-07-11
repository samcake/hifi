//
//  Phase Inspector.qml
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

    Prop.PropVec3 {
        label: "Origin"
        object: Workload.getConfig("updatePhase")
        property: "loadingOriginQ"  
        numDigits: 1
    }
    
    Prop.PropScalar {
        label: "Ready Radius"
        object: Workload.getConfig("updatePhase")
        property: "readyRadius"
        readOnly: true
    }

    Prop.PropScalar {
        label: "Loading Radius"
        object: Workload.getConfig("updatePhase")
        property: "loadingRadius"
        readOnly: true
    }

    Prop.PropScalar {
        label: "Num Evaluated"
        object: Workload.getConfig("updatePhase")
        property: "numEvaluated"
        integral: true
        readOnly: true
    }
}

