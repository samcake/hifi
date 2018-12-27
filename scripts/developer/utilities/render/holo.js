"use strict";
//
// holo.js
//
// Created by Sam Gateau on 2018-12-17
// Copyright 2018 High Fidelity, Inc.
//
// Distributed under the Apache License, Version 2.0
// See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

(function() {


    
    // Function Name: inFrontOf()
    //
    // Description:
    //   -Returns the position in front of the given "position" argument, where the forward vector is based off
    //    the "orientation" argument and the amount in front is based off the "distance" argument.
    function inFrontOf(distance, position, orientation) {
        return Vec3.sum(position || MyAvatar.position,
            Vec3.multiply(distance, Quat.getForward(orientation || MyAvatar.orientation)));
    }

    //*****************************************************
    // Holo 
    //*****************************************************
    const SECONDARY_CAMERA_RESOLUTION = 1024; // width/height multiplier, in pixels
 
    function Holo(config) {
        this.render = false;
        this.renderStereo = false;
        this.stereoImage = false;

        this.baseEntityProperties = {
                name: "Holo-base",
                //"collisionless": false,
                "color": {
                    "blue": 239,
                    "green": 180,
                    "red": 0
                },
                "dimensions": {
                    "x": 10,
                    "y": 0.1,
                    "z": 10,
                },
                "grab": {
                    "grabbable": false,
                },
               // "ignoreForCollisions": false,
                type: "Shape",
                shape: "Cylinder",
                shapeType:"box",
                "position": inFrontOf(8, Vec3.sum(MyAvatar.position, { x: 0, y: -1, z: 0 })),
                "rotation": MyAvatar.orientation,
                lifetime: config.lifetime,
            }
        this.baseEntity = Entities.addEntity( this.baseEntityProperties );
        this.baseEntityProperties = Entities.getEntityProperties(this.baseEntity);

        var DIM = {x: 6.0, y: 3.0, z: 0.0};

        this.screenEntityProperties = {
            name: "Holo-screen",
            "visible": false,
            "collisionless": true,
            "color": {
                "blue": 239,
                "red": 180,
                "green": 0
            },
            "dimensions": DIM,
            "grab": {
                "grabbable": false,
            },
            "ignoreForCollisions": true,
            type: "Shape",
            shape: "Box",
            parentID:  this.baseEntity,
            localPosition: { x: 0, y: DIM.y * 0.5, z: 0 },
            localRotation: { w: 1, x: 0, y: 0, z: 0 },
            lifetime: config.lifetime,
        }
        this.screenEntity = Entities.addEntity( this.screenEntityProperties );
        this.screenEntityProperties = Entities.getEntityProperties(this.screenEntity);

        this.screenOutEntityProperties = {
            name: "Holo-screen-out",
            "visible": false,
            "collisionless": true,
            "color": {
                "blue": 239,
                "red": 180,
                "green": 0
            },
            "dimensions": DIM,
            "grab": {
                "grabbable": false,
            },
            "ignoreForCollisions": true,
            type: "Shape",
            shape: "Box",
            parentID:  this.screenEntity,
            localPosition: { x: 10, y: 0, z: 0 },
            localRotation: { w: 0, x: 0, y: 1, z: 0 },
            lifetime: config.lifetime,
        }
        this.screenOutEntity = Entities.addEntity( this.screenOutEntityProperties );
        this.screenOutEntityProperties = Entities.getEntityProperties(this.screenOutEntity);

        
    
        var spectatorCameraConfig = Render.getConfig("SecondaryCamera");
        Render.getConfig("SecondaryCameraJob.ToneMapping").curve = 0;
        this.enableRender();
        this.enableRenderStereo();

        spectatorCameraConfig.stereoEyeInteraxial = 0.07;
        spectatorCameraConfig.portalProjection = true;
        spectatorCameraConfig.portalEntranceEntityId = this.screenOutEntity;
        spectatorCameraConfig.attachedEntityId = this.screenEntity;

        spectatorCameraConfig.resetSizeSpectatorCamera(DIM.x * SECONDARY_CAMERA_RESOLUTION,
            DIM.y * SECONDARY_CAMERA_RESOLUTION);

        
        this.screenProperties = {
            url: "resource://spectatorCameraFrame",
            emissive: true,
            parentID:  this.screenEntity,
            alpha: 1,
            localRotation: { w: 1, x: 0, y: 0, z: 0 },
            localPosition: { x: 0, y: 0.0, z: 0.0 },
            dimensions: {
                x: (DIM.y > DIM.x ? DIM.y : DIM.x),
                y: -(DIM.y > DIM.x ? DIM.y : DIM.x),
                z: 0
            },
            lifetime: config.lifetime,
        };
        this.screen = Overlays.addOverlay("image3d", this.screenProperties);
        this.enableStereoImage();
        //  this.screenProperties = Overlays.getProperties(this.screen);

    }

    Holo.prototype.setRender = function(enabled) {
        this.render = enabled       
        Render.getConfig("SecondaryCamera").enableSecondaryCameraRenderConfigs(enabled);
        print("Success: " + true + " setRender = " + enabled );
    }
    Holo.prototype.enableRender = function() {
        this.setRender(true);
    }
    Holo.prototype.disableRender = function() {
        this.setRender(false);
    }
    
    Holo.prototype.setStereoRender = function(enabled) {
        this.renderStereo = enabled       
        Render.getConfig("SecondaryCamera").stereo = enabled;
        print("Success: " + true + " setStereoRender = " + enabled );
    }
    Holo.prototype.enableRenderStereo = function() {
        this.setStereoRender(true);
    }
    Holo.prototype.disableRenderStereo = function() {
        this.setStereoRender(false);
    }

    Holo.prototype.setStereoImage = function(enabled) {
        this.stereoImage = enabled
        var success = Overlays.editOverlay(this.screen, {
            stereoImage: enabled
        });
        print("Success: " + success + " setStereoImage = " + enabled );
    }
    Holo.prototype.enableStereoImage = function() {
        this.setStereoImage(true);
    }
    Holo.prototype.disableStereoImage = function() {
        this.setStereoImage(false);
    }

    Holo.prototype.kill = function () {
        print("Kill Holo")
       var spectatorCameraConfig = Render.getConfig("SecondaryCamera");
       spectatorCameraConfig.enableSecondaryCameraRenderConfigs(false);
        
        if (this.baseEntity) {
            Entities.deleteEntity(this.baseEntity);
           // this.entity = null
        }
        if (this.screenEntity) {
            Entities.deleteEntity(this.screenEntity);
           // this.entity = null
        }
        if (this.screenOutEntity) {
            Entities.deleteEntity(this.screenOutEntity);
           // this.entity = null
        }
        if (this.screen) {
            Overlays.deleteOverlay(this.view);
        }
    };   


    
        var TABLET_BUTTON_NAME = "2ndCam";
        var QMLAPP_URL = Script.resolvePath("./secondCam.qml");
        var ICON_URL = Script.resolvePath("../../../system/assets/images/lod-i.svg");
        var ACTIVE_ICON_URL = Script.resolvePath("../../../system/assets/images/lod-a.svg");
    
        var onTablet = false; // set this to true to use the tablet, false use a floating window
    
        var onAppScreen = false;
    
        var tablet = Tablet.getTablet("com.highfidelity.interface.tablet.system");
        var button = tablet.addButton({
            text: TABLET_BUTTON_NAME,
            icon: ICON_URL,
            activeIcon: ACTIVE_ICON_URL
        });
    
        var hasEventBridge = false;
    
        var onScreen = false;
        var window;
    
        function onClicked() {
            if (onTablet) {
                if (onAppScreen) {
                    tablet.gotoHomeScreen();
                } else {
                    tablet.loadQMLSource(QMLAPP_URL);
                }
            } else {
                if (onScreen) {
                    killWindow()
                } else {
                    createWindow()    
                }
            }
        }
    
        function createWindow() {
            var qml = Script.resolvePath(QMLAPP_URL);
            window = Desktop.createWindow(Script.resolvePath(QMLAPP_URL), {
                title: TABLET_BUTTON_NAME,
                flags: Desktop.ALWAYS_ON_TOP,
                presentationMode: Desktop.PresentationMode.NATIVE,
                size: {x: 400, y: 600}
            });
            window.closed.connect(killWindow);
            window.fromQml.connect(fromQml);
            onScreen = true
            button.editProperties({isActive: true});
        }
    
        function killWindow() {
            if (window !==  undefined) { 
                window.closed.disconnect(killWindow);
                window.fromQml.disconnect(fromQml);
                window.close()
                window = undefined
            }
            if (holo) {
                holo.kill();
                holo = null;
            }
            onScreen = false
            button.editProperties({isActive: false})
        }
    
        function wireEventBridge(on) {
            if (!tablet) {
                print("Warning in wireEventBridge(): 'tablet' undefined!");
                return;
            }
            if (on) {
                if (!hasEventBridge) {
                    tablet.fromQml.connect(fromQml);
                    hasEventBridge = true;
                }
            } else {
                if (hasEventBridge) {
                    tablet.fromQml.disconnect(fromQml);
                    hasEventBridge = false;
                }
            }
        }
    
        function onScreenChanged(type, url) {
            if (onTablet) {
                onAppScreen = (url === QMLAPP_URL);
                
                button.editProperties({isActive: onAppScreen});
                wireEventBridge(onAppScreen);
            }
        }
            
        button.clicked.connect(onClicked);
        tablet.screenChanged.connect(onScreenChanged);
    
        function killApp() {
            killWindow()
            if (onAppScreen) {
                tablet.gotoHomeScreen();
            }
            button.clicked.disconnect(onClicked);
            tablet.screenChanged.disconnect(onScreenChanged);
            tablet.removeButton(button);
        };
    
     
        function sendToQml(message) {
            if (onTablet) {
                tablet.sendToQml(message);
            } else {
                if (window) {
                    window.sendToQml(message);
                }
            }
        }    

    //*****************************************************
    // Exe 
    //*****************************************************

    var holo = null;

    function fromQml(message) {
        switch (message.method) {
        case 'spawn2ndCam': {
                if (holo) {
                    holo.kill();
                    holo = null;
                }
                holo = new Holo({ lifetime: 60});
            }
            break;
        case 'kill2ndCam': {
                if (holo) {
                    holo.kill();
                    holo = null;
                }
            } break;

        case 'enableRender':
            if (holo) { holo.enableRender(); }
            break;
        case 'disableRender':
            if (holo) { holo.disableRender(); }
            break;  

        case 'enableRenderStereo':
            if (holo) { holo.enableRenderStereo(); }
            break;
        case 'disableRenderStereo':
            if (holo) { holo.disableRenderStereo(); }
            break;

        case 'enableShowStereoImage':
            if (holo) { holo.enableStereoImage(); }
            break;
        case 'disableShowStereoImage':
            if (holo) { holo.disableStereoImage(); }
            break;

        case 'setStereoInteraxial':
            if (holo) { holo.setStereoInteraxial( message.params[0] ); }
            break;
            
        }
    }

    function startup() {
    }

    function shutdown() {
        if (holo) {
            holo.kill();
            holo = null;
        }
        killApp();
    }


    startup();
    Script.scriptEnding.connect(shutdown);
}());