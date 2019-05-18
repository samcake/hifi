//
//  Simplified Nametag
//  nameTagListManager.js
//  Created by Milad Nazeri on 2019-03-09
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
//  Helps manage the list of avatars added to the nametag list
//

var ON = 'ON';
var OFF = 'OFF';
var DEBUG_ON = true;
var DEBUG_OFF = false;
var log = Script.require(
    'https://hifi-content.s3.amazonaws.com/milad/ROLC/d/ROLC_High-Fidelity/02_Organize/O_Projects/Repos/hifi-content/developerTools/sharedLibraries/easyLog/easyLog.js?'
    + Date.now())(DEBUG_OFF, 'nameTagListManager.js');


var EntityMaker = Script.require('./entityMaker.js?' + Date.now());
var entityProps = Script.require('./defaultLocalEntityProps.js?' + Date.now());
var textHelper = new (Script.require('./textHelper.js?' + Date.now()));
var X = 0;
var Y = 1;
var Z = 2;
var HALF = 0.5;
var CLEAR_ENTITY_EDIT_PROPS = true;
var MILISECONDS_IN_SECOND = 1000;

// *************************************
// START UTILTY
// *************************************
// #region UTILTY


// Properties to give new avatars added to the list
function NewAvatarProps() {
    return {
        avatarInfo: null,
        previousDistance: null,
        currentDistance: null,
        initialDistance: null,
        initialDimensions: null,
        previousName: null,
        timeoutStarted: false
    };
}


// Makes sure clear interval exists before changing
function maybeClearInterval() {
    if (_this.redrawTimeout) {
        Script.clearInterval(_this.redrawTimeout);
        _this.redrawTimeout = null;
    }
}


// Calculate our initial properties for the nametag
var Z_SIZE = 0.01;
var LINE_HEIGHT_SCALER = 0.99;
var DISTANCE_SCALER_ON = 0.35;
var DISTANCE_SCALER_ALWAYS_ON = 0.45;
var distanceScaler = DISTANCE_SCALER_ON;
var userScaler = 1.0;
var DEFAULT_LINE_HEIGHT = entityProps.lineHeight;
function calculateInitialProperties(uuid) {
    var avatar = _this.avatars[uuid];
    var avatarInfo = avatar.avatarInfo;

    var adjustedScaler = null;
    var distance = null;
    var dimensions = null;
    var lineHeight = null;
    var scaledDimensions = null;
    var name = null;

    // Handle if we are asking for the main or sub properties
    name = avatarInfo.displayName;

    // Use the text helper to calculate what our dimensions for the text box should be
    textHelper
        .setText(name)
        .setLineHeight(DEFAULT_LINE_HEIGHT);

    // Calculate the distance from the camera to the target avatar
    distance = getDistance(uuid);

    // Adjust the distance by the distance scaler
    distanceScaler = avatarNametagMode === "on" ? DISTANCE_SCALER_ON : DISTANCE_SCALER_ALWAYS_ON;
    adjustedScaler = distance * distanceScaler;
    // Get the new dimensions from the text helper
    dimensions = [textHelper.getTotalTextLength(), DEFAULT_LINE_HEIGHT, Z_SIZE];
    // Adjust the dimensions by the modified distance scaler
    scaledDimensions = Vec3.multiply(dimensions, adjustedScaler);

    // Adjust the lineheight to be the new scaled dimensions Y 
    lineHeight = scaledDimensions[Y] * LINE_HEIGHT_SCALER;

    return {
        distance: distance,
        scaledDimensions: scaledDimensions,
        lineHeight: lineHeight
    };
}


// Used in alwaysOn mode to show or hide if they reached the max radius
function showHide(uuid, type) {
    var avatar = _this.avatars[uuid];
    var nametag = avatar.nametag;

    if (type === "show") {
        nametag.show();
    } else {
        nametag.hide();
    }
}


// Go through the selected avatar list and see if any of the avatars need a redraw
function checkAllSelectedForRedraw() {
    for (var avatar in _this.selectedAvatars) {
        maybeRedraw(avatar);
    }
}


// Remake the nametags if the display name changes
function updateName(uuid) {
    var avatar = _this.avatars[uuid];
    avatar.nametag.destroy();

    avatar.nametag = new EntityMaker('local').add(entityProps);

    makeNameTag(uuid);
}


// Get the current data for an avatar.
function getAvatarData(uuid) {
    var avatar = _this.avatars[uuid];
    var avatarInfo = avatar.avatarInfo;

    var newAvatarInfo = AvatarManager.getAvatar(uuid);
    // Save the username so it doesn't get overwritten when grabbing new avatarData
    var combinedAvatarInfo = Object.assign({}, newAvatarInfo, {
        username: avatarInfo === null ? null : avatarInfo.username
    });

    // Now combine that avatar data with the main avatar object
    _this.avatars[uuid] = Object.assign({}, avatar, { avatarInfo: combinedAvatarInfo });

    return _this;
}


// Calculate the distance between the camera and the target avatar
function getDistance(uuid, checkAvatar, shouldSave) {
    checkAvatar = checkAvatar || false;
    shouldSave = shouldSave || true;
    var eye = checkAvatar ? MyAvatar.position : Camera.position;
    var avatar = _this.avatars[uuid];
    var avatarInfo = avatar.avatarInfo;

    var target = avatarInfo.position;

    var currentDistance = Vec3.distance(target, eye);

    if (!checkAvatar && shouldSave) {
        avatar.previousDistance = avatar.currentDistance;
        avatar.currentDistance = currentDistance;
    }

    return currentDistance;
}


// Check to see if we need to toggle our interval check because we went to 0 avatars
// or if we got our first avatar in the select list
function shouldToggleInterval() {
    var currentNumberOfAvatarsSelected = Object.keys(_this.selectedAvatars).length;

    if (currentNumberOfAvatarsSelected === 0 && _this.redrawTimeout) {
        toggleInterval();
        return;
    }

    if (currentNumberOfAvatarsSelected > 0 && !_this.redrawTimeout) {
        toggleInterval();
        return;
    }
}


// Turn off and on the redraw check
var INTERVAL_CHECK_MS = 30;
function toggleInterval() {
    if (_this.redrawTimeout) {
        maybeClearInterval();
    } else {
        _this.redrawTimeout =
            Script.setInterval(checkAllSelectedForRedraw, INTERVAL_CHECK_MS);
    }
}


// handle turning the peristenet mode on
function handleAlwaysOnMode(shouldTurnOnAlwaysOnMode) {
    _this.reset();
    if (shouldTurnOnAlwaysOnMode) {
        AvatarManager
            .getAvatarIdentifiers()
            .forEach(function (avatar) {
                if (avatar) {
                    add(avatar);
                }
            });
    }
}


// #endregion
// *************************************
// END UTILTY
// *************************************

// *************************************
// START Nametag
// *************************************
// #region Nametag


var _this = null;
function nameTagListManager() {
    _this = this;

    _this.avatars = {};
    _this.selectedAvatars = {};
    _this.redrawTimeout = null;
}


// Create or make visible either the sub or the main tag.
var REDRAW_TIMEOUT_AMOUNT_MS = 500;
var LEFT_MARGIN_SCALER = 0.15;
var RIGHT_MARGIN_SCALER = 0.10;
var TOP_MARGIN_SCALER = 0.07;
var BOTTOM_MARGIN_SCALER = 0.03;
var ABOVE_HEAD_OFFSET = 0.30;
var DISTANCE_SCALER_INTERPOLATION_OFFSET_ALWAYSON = 15;
var DISTANCE_SCALER_INTERPOLATION_OFFSET_ON = 10;
var maxDistance = MAX_RADIUS_IGNORE_METERS;
var onModeScalar = 0.55;
var alwaysOnModeScalar = -0.75;
function makeNameTag(uuid) {
    var avatar = _this.avatars[uuid];
    var avatarInfo = avatar.avatarInfo;
    var nametag = avatar.nametag;

    // Make sure an anonymous name is covered before sending to calculate

    avatarInfo.displayName = avatarInfo.displayName === "" ? "anonymous" : avatarInfo.displayName.trim();
    avatar.previousName = avatarInfo.displayName;

    // Returns back the properties we need based on what we are looking for and the distance from the avatar
    var calculatedProps = calculateInitialProperties(uuid);
    var distance = calculatedProps.distance;
    var scaledDimensions = calculatedProps.scaledDimensions;
    var lineHeight = calculatedProps.lineHeight;

    // Capture the inital dimensions, distance, and displayName in case we need to redraw
    avatar.previousDisplayName = avatarInfo.displayName;
    avatar.initialDimensions = scaledDimensions;
    avatar.initialDistance = distance;
    var name = avatarInfo.displayName;
    var parentID = uuid;

    nametag.add("text", name);

    // Multiply the new dimensions and line height with the user selected scaler
    scaledDimensions = Vec3.multiply(scaledDimensions, userScaler);

    maxDistance = avatarNametagMode === "on"
        ? MAX_ON_MODE_DISTANCE + DISTANCE_SCALER_INTERPOLATION_OFFSET_ON
        : MAX_RADIUS_IGNORE_METERS + DISTANCE_SCALER_INTERPOLATION_OFFSET_ALWAYSON;
    var finalScaler = (distance - maxDistance) / (MIN_DISTANCE - maxDistance);

    var remainder = 1 - finalScaler;
    var multipliedRemainderOn = remainder * onModeScalar;
    var multipliedRemainderAlwaysOn = remainder * alwaysOnModeScalar;
    finalScaler = avatarNametagMode === "on" ? finalScaler + multipliedRemainderOn : finalScaler + multipliedRemainderAlwaysOn;

    scaledDimensions = Vec3.multiply(scaledDimensions, finalScaler);

    lineHeight = scaledDimensions[Y] * LINE_HEIGHT_SCALER;
    // Add some room for the margin by using lineHeight as a reference
    scaledDimensions[X] += (lineHeight * LEFT_MARGIN_SCALER) + (lineHeight * RIGHT_MARGIN_SCALER);
    scaledDimensions[Y] += (lineHeight * TOP_MARGIN_SCALER) + (lineHeight * BOTTOM_MARGIN_SCALER);

    var scaledDimenionsYHalf = scaledDimensions[Y] * HALF;
    var AvatarData = AvatarManager.getAvatar(uuid);
    var headJointIndex = AvatarData.getJointIndex("Head");
    var jointInObjectFrame = AvatarData.getAbsoluteJointTranslationInObjectFrame(headJointIndex);
    var nameTagPosition = jointInObjectFrame.y + scaledDimenionsYHalf + ABOVE_HEAD_OFFSET;
    var localPosition = [0, nameTagPosition, 0];

    var visible = true;
    if (avatarNametagMode === "alwaysOn") {
        var currentDistance = getDistance(uuid, CHECK_AVATAR, false);
        visible = currentDistance > MAX_RADIUS_IGNORE_METERS ? false : true;
    }

    nametag
        .add("leftMargin", lineHeight * LEFT_MARGIN_SCALER)
        .add("rightMargin", lineHeight * RIGHT_MARGIN_SCALER)
        .add("topMargin", lineHeight * TOP_MARGIN_SCALER)
        .add("bottomMargin", lineHeight * BOTTOM_MARGIN_SCALER)
        .add("lineHeight", lineHeight)
        .add("dimensions", scaledDimensions)
        .add("parentID", parentID)
        .add("localPosition", localPosition)
        .add("visible", visible)
        .create(CLEAR_ENTITY_EDIT_PROPS);

    Script.setTimeout(function () {
        nametag.edit("visible", true);
    }, REDRAW_TIMEOUT_AMOUNT_MS);
}


// Check to see if the display named changed or if the distance is big enough to need a redraw.
var MAX_RADIUS_IGNORE_METERS = 22;
var MAX_ON_MODE_DISTANCE = 35;
var CHECK_AVATAR = true;
var MIN_DISTANCE = 0.2;
function maybeRedraw(uuid) {
    var avatar = _this.avatars[uuid];
    var avatarInfo = avatar.avatarInfo;
    getAvatarData(uuid);

    getDistance(uuid);
    var avatarDistance = getDistance(uuid, CHECK_AVATAR, false);
    if (avatarNametagMode === "alwaysOn" && avatarDistance > MAX_RADIUS_IGNORE_METERS) {
        showHide(uuid, "hide");
    }

    if (avatarNametagMode === "alwaysOn" && avatarDistance < MAX_RADIUS_IGNORE_METERS) {
        showHide(uuid, "show");
    }

    avatarInfo.displayName = avatarInfo.displayName === "" ? "anonymous" : avatarInfo.displayName.trim();

    if (avatar.previousName !== avatarInfo.displayName) {
        updateName(uuid, avatarInfo.displayName);
    } else {
        redraw(uuid);
    }

}


// Handle redrawing if needed
function redraw(uuid) {
    var avatar = _this.avatars[uuid];

    var nametag = avatar.nametag;
    var initialDimensions = null;
    var initialDistance = null;
    var currentDistance = null;
    var newDimensions = null;
    var lineHeight = null;

    initialDistance = avatar.initialDistance;
    currentDistance = avatar.currentDistance;

    initialDimensions = avatar.initialDimensions;

    // Find our new dimensions from the new distance 
    newDimensions = [
        (initialDimensions[X] / initialDistance) * currentDistance,
        (initialDimensions[Y] / initialDistance) * currentDistance,
        (initialDimensions[Z] / initialDistance) * currentDistance
    ];

    // Multiply the new dimensions and line height with the user selected scaler
    newDimensions = Vec3.multiply(newDimensions, userScaler);

    var distance = getDistance(uuid, false, false);

    maxDistance = avatarNametagMode === "on"
        ? MAX_ON_MODE_DISTANCE + DISTANCE_SCALER_INTERPOLATION_OFFSET_ON
        : MAX_RADIUS_IGNORE_METERS + DISTANCE_SCALER_INTERPOLATION_OFFSET_ALWAYSON;
    var finalScaler = (distance - maxDistance) / (MIN_DISTANCE - maxDistance);
    var remainder = 1 - finalScaler;
    var multipliedRemainderOn = remainder * onModeScalar;
    var multipliedRemainderAlwaysOn = remainder * alwaysOnModeScalar;
    finalScaler = avatarNametagMode === "on" ? finalScaler + multipliedRemainderOn : finalScaler + multipliedRemainderAlwaysOn;

    newDimensions = Vec3.multiply(newDimensions, finalScaler);

    lineHeight = newDimensions[Y] * LINE_HEIGHT_SCALER;

    // Add some room for the margin by using lineHeight as a reference
    newDimensions[X] += (lineHeight * LEFT_MARGIN_SCALER) + (lineHeight * RIGHT_MARGIN_SCALER);
    newDimensions[Y] += (lineHeight * TOP_MARGIN_SCALER) + (lineHeight * BOTTOM_MARGIN_SCALER);

    // We can generalize some of the processes that are similar in makeNameTag() and redraw() if we wanted to reduce some code
    var newDimenionsYHalf = newDimensions[Y] * HALF;
    var AvatarData = AvatarManager.getAvatar(uuid);
    var headJointIndex = AvatarData.getJointIndex("Head");
    var jointInObjectFrame = AvatarData.getAbsoluteJointTranslationInObjectFrame(headJointIndex);
    var nameTagPosition = jointInObjectFrame.y + newDimenionsYHalf + ABOVE_HEAD_OFFSET;
    var localPosition = [0, nameTagPosition, 0];

    nametag
        .add("leftMargin", lineHeight * LEFT_MARGIN_SCALER)
        .add("rightMargin", lineHeight * RIGHT_MARGIN_SCALER)
        .add("topMargin", lineHeight * TOP_MARGIN_SCALER)
        .add("bottomMargin", lineHeight * BOTTOM_MARGIN_SCALER)
        .add("lineHeight", lineHeight)
        .add("dimensions", newDimensions)
        .add("localPosition", localPosition)
        .sync();
}

// Add a user to the list.
var DEFAULT_LIFETIME = entityProps.lifetime;


// add a user to our current selections
function add(uuid) {
    // User Doesn't exist so give them new props and save in the cache, and get their current avatar info.
    if (!_this.avatars[uuid]) {
        _this.avatars[uuid] = new NewAvatarProps();
        getAvatarData(uuid);
    }

    var avatar = _this.avatars[uuid];

    _this.selectedAvatars[uuid] = true;
    if (avatarNametagMode === "alwaysOn") {
        entityProps.lifetime = -1;
    } else {
        entityProps.lifetime = DEFAULT_LIFETIME;
    }

    avatar.nametag = new EntityMaker('local').add(entityProps);

    // When the user clicks someone, we create their nametag
    makeNameTag(uuid);
    var deleteEnttyInMiliseconds = entityProps.lifetime * MILISECONDS_IN_SECOND;

    // Remove from list after lifetime is over
    if (avatarNametagMode === "on") {
        avatar.timeoutStarted = Script.setTimeout(function () {
            removeNametag(uuid);
        }, deleteEnttyInMiliseconds);
    }

    // Check to see if anyone is in the selected list now to see if we need to start the interval checking
    shouldToggleInterval();

    return _this;
}


// Remove the avatar from the list.
function remove(uuid) {
    if (_this.selectedAvatars[uuid]) {
        delete _this.selectedAvatars[uuid];
    }

    removeNametag(uuid);

    shouldToggleInterval();
    delete _this.avatars[uuid];

    return _this;
}


// Remove all the current nametags.
function removeAllNametags() {
    for (var uuid in _this.selectedAvatars) {
        removeNametag(uuid);
    }

    return _this;
}


// Remove a single Nametag.
function removeNametag(uuid) {
    var avatar = _this.avatars[uuid];

    if (avatar) {
        avatar.nametag.destroy();
        delete _this.selectedAvatars[uuid];
        return _this;
    }

}


// #endregion
// *************************************
// END Nametag
// *************************************

// *************************************
// START API
// *************************************
// #region API


// Create the manager and hook up username signal
function create() {

    return _this;
}


// Destory the manager and disconnect from username signal
function destroy() {
    _this.reset();
    return _this;
}


// Check to see if we need to delete any close by nametags
var MAX_DELETE_RANGE = 4;
function checkIfAnyAreClose(target) {
    var targetPosition = AvatarManager.getAvatar(target).position;
    for (var uuid in _this.selectedAvatars) {
        var position = AvatarManager.getAvatar(uuid).position;
        var distance = Vec3.distance(position, targetPosition);
        if (distance <= MAX_DELETE_RANGE) {
            var timeoutStarted = _this.avatars[uuid].timeoutStarted;
            if (timeoutStarted) {
                Script.clearTimeout(timeoutStarted);
                timeoutStarted = null;
            }
            removeNametag(uuid);
        }
    }
}

// Handles what happens when an avatar gets triggered on
function handleSelect(uuid) {
    if (avatarNametagMode === "off" || avatarNametagMode === "alwaysOn") {
        return;
    }

    var inSelected = uuid in _this.selectedAvatars;

    if (inSelected) {
        if (avatarNametagMode === "on") {
            var timeoutStarted = _this.avatars[uuid].timeoutStarted;
            if (timeoutStarted) {
                Script.clearTimeout(timeoutStarted);
                timeoutStarted = null;
            }
        }

        removeNametag(uuid);

    } else {
        checkIfAnyAreClose(uuid);
        add(uuid);
    }
}


// Check to see if we need to clear timeouts for avatars
function maybeClearAllTimeouts() {
    for (var uuid in _this.selectedAvatars) {
        var timeoutStarted = _this.avatars[uuid].timeoutStarted;
        if (timeoutStarted) {
            Script.clearTimeout(timeoutStarted);
            timeoutStarted = null;
        }
    }
}


// Check to see if the uuid is in the avatars list before removing
function maybeRemove(uuid) {
    if (uuid in _this.avatars) {
        remove(uuid);
    }
}


// Check to see if we need to add this user to our list
function maybeAdd(uuid) {
    if (uuid && avatarNametagMode === "alwaysOn" && !(uuid in _this.avatars)) {
        add(uuid);
    }
}


// Register the beggining scaler in case it was saved from a previous session
function registerInitialScaler(initalScaler) {
    userScaler = initalScaler;
}


// Handle the user updating scale
function updateUserScaler(newUSerScaler) {
    userScaler = newUSerScaler;
    for (var avatar in _this.selectedAvatars) {
        redraw(avatar);
    }
}


// Reset the avatar list
function reset() {
    maybeClearAllTimeouts();
    removeAllNametags();
    _this.avatars = {};
    shouldToggleInterval();

    return _this;
}


// Update the nametag display mode
var avatarNametagMode = "on";
function handleAvatarNametagMode(newAvatarNametagMode) {
    if (avatarNametagMode === "alwaysOn") {
        handleAlwaysOnMode(false);
    }

    avatarNametagMode = newAvatarNametagMode;
    if (avatarNametagMode === "alwaysOn") {
        handleAlwaysOnMode(true);
    }

    if (avatarNametagMode === "off" || avatarNametagMode === "on") {
        reset();
    }
}


// #endregion
// *************************************
// END API
// *************************************


nameTagListManager.prototype = {
    create: create,
    destroy: destroy,
    handleSelect: handleSelect,
    maybeRemove: maybeRemove,
    maybeAdd: maybeAdd,
    registerInitialScaler: registerInitialScaler,
    updateUserScaler: updateUserScaler,
    handleAvatarNametagMode: handleAvatarNametagMode,
    reset: reset
};


module.exports = nameTagListManager;