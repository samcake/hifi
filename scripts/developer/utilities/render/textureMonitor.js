//
//  textureMonitor.js
//  examples/utilities/tools/render
//
//  Sam Gateau, created on 3/22/2016.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

var window = Desktop.createWindow(Script.resolvePath('./luci/TextureMemory.qml'), {
    title: "Texture Monitor",
    presentationMode: Desktop.PresentationMode.NATIVE,
    size: {x: 350, y: 300}
});
