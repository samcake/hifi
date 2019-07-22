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

    Prop.PropVec234 {
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
        numDigits: 0
        readOnly: true
    }

    Prop.PropScalar {
        label: "Expansion Speed"
        object: Workload.getConfig("updatePhase")
        property: "expansionSpeed"
        numDigits: 1
        min: 0.0
        max: 20.0
    }

    Prop.PropVec234 {
        label: "Num OnHold/Begin/Loading/Done"
        object: Workload.getConfig("updatePhase")
        property: "numEvaluatedPerPhaseQ"  
        numDigits: 0
        dimension: 4
    }
    Prop.PropVec234 {
        label: "OnHold Range"
        object: Workload.getConfig("updatePhase")
        property: "onHoldRangeQ"  
        numDigits: 0
        dimension: 2
    }
    Prop.PropVec234 {
        label: "Loading Range"
        object: Workload.getConfig("updatePhase")
        property: "loadingRangeQ"  
        numDigits: 0
        dimension: 2
    }
}

