//
//  Prop/style/PiSlider.qml
//
//  Created by Sam Gateau on 5/9/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.7
import QtQuick.Controls 2.2

Slider {
    Global { id: global }
    value: 0
    height: global.slimHeight
}