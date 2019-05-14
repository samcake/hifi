//
//  EntityPropertyFlags.h
//  libraries/entities/src
//
//  Created by Brad Hefta-Gaub on 12/4/13.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_EntityPropertyFlags_h
#define hifi_EntityPropertyFlags_h

#include <PropertyFlags.h>

enum EntityPropertyList {
    PROP_PAGED_PROPERTY,
    PROP_CUSTOM_PROPERTIES_INCLUDED,

    // Core properties
    PROP_SIMULATION_OWNER,
    PROP_PARENT_ID,
    PROP_PARENT_JOINT_INDEX,
    PROP_VISIBLE,
    PROP_NAME,
    PROP_LOCKED,
    PROP_USER_DATA,
    PROP_PRIVATE_USER_DATA,
    PROP_HREF,
    PROP_DESCRIPTION,
    PROP_POSITION,
    PROP_DIMENSIONS,
    PROP_ROTATION,
    PROP_REGISTRATION_POINT,
    PROP_CREATED,
    PROP_LAST_EDITED_BY,
    PROP_ENTITY_HOST_TYPE,            // not sent over the wire
    PROP_OWNING_AVATAR_ID,            // not sent over the wire
    PROP_QUERY_AA_CUBE,
    PROP_CAN_CAST_SHADOW,
    PROP_VISIBLE_IN_SECONDARY_CAMERA, // not sent over the wire
    PROP_RENDER_LAYER,
    PROP_PRIMITIVE_MODE,
    PROP_IGNORE_PICK_INTERSECTION,
    // Grab
    PROP_GRAB_GRABBABLE,
    PROP_GRAB_KINEMATIC,
    PROP_GRAB_FOLLOWS_CONTROLLER,
    PROP_GRAB_TRIGGERABLE,
    PROP_GRAB_EQUIPPABLE,
    PROP_GRAB_DELEGATE_TO_PARENT,
    PROP_GRAB_LEFT_EQUIPPABLE_POSITION_OFFSET,
    PROP_GRAB_LEFT_EQUIPPABLE_ROTATION_OFFSET,
    PROP_GRAB_RIGHT_EQUIPPABLE_POSITION_OFFSET,
    PROP_GRAB_RIGHT_EQUIPPABLE_ROTATION_OFFSET,
    PROP_GRAB_EQUIPPABLE_INDICATOR_URL,
    PROP_GRAB_EQUIPPABLE_INDICATOR_SCALE,
    PROP_GRAB_EQUIPPABLE_INDICATOR_OFFSET,

    // Physics
    PROP_DENSITY,
    PROP_VELOCITY,
    PROP_ANGULAR_VELOCITY,
    PROP_GRAVITY,
    PROP_ACCELERATION,
    PROP_DAMPING,
    PROP_ANGULAR_DAMPING,
    PROP_RESTITUTION,
    PROP_FRICTION,
    PROP_LIFETIME,
    PROP_COLLISIONLESS,
    PROP_COLLISION_MASK,
    PROP_DYNAMIC,
    PROP_COLLISION_SOUND_URL,
    PROP_ACTION_DATA,

    // Cloning
    PROP_CLONEABLE,
    PROP_CLONE_LIFETIME,
    PROP_CLONE_LIMIT,
    PROP_CLONE_DYNAMIC,
    PROP_CLONE_AVATAR_ENTITY,
    PROP_CLONE_ORIGIN_ID,

    // Scripts
    PROP_SCRIPT,
    PROP_SCRIPT_TIMESTAMP,
    PROP_SERVER_SCRIPTS,

    // Certifiable Properties
    PROP_ITEM_NAME,
    PROP_ITEM_DESCRIPTION,
    PROP_ITEM_CATEGORIES,
    PROP_ITEM_ARTIST,
    PROP_ITEM_LICENSE,
    PROP_LIMITED_RUN,
    PROP_MARKETPLACE_ID,
    PROP_EDITION_NUMBER,
    PROP_ENTITY_INSTANCE_NUMBER,
    PROP_CERTIFICATE_ID,
    PROP_CERTIFICATE_TYPE,
    PROP_STATIC_CERTIFICATE_VERSION,

    // Used to convert values to and from scripts
    PROP_LOCAL_POSITION,
    PROP_LOCAL_ROTATION,
    PROP_LOCAL_VELOCITY,
    PROP_LOCAL_ANGULAR_VELOCITY,
    PROP_LOCAL_DIMENSIONS,

    // These properties are used by multiple subtypes but aren't in the base EntityItem
    PROP_SHAPE_TYPE,
    PROP_COMPOUND_SHAPE_URL,
    PROP_COLOR,
    PROP_ALPHA,
    PROP_PULSE_MIN,
    PROP_PULSE_MAX,
    PROP_PULSE_PERIOD,
    PROP_PULSE_COLOR_MODE,
    PROP_PULSE_ALPHA_MODE,
    PROP_TEXTURES,
    PROP_BILLBOARD_MODE,

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // ATTENTION: add new shared EntityItem properties to the list ABOVE this line
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // We need as many of these as the number of unique properties of a derived EntityItem class
    PROP_DERIVED_0,
    PROP_DERIVED_1,
    PROP_DERIVED_2,
    PROP_DERIVED_3,
    PROP_DERIVED_4,
    PROP_DERIVED_5,
    PROP_DERIVED_6,
    PROP_DERIVED_7,
    PROP_DERIVED_8,
    PROP_DERIVED_9,
    PROP_DERIVED_10,
    PROP_DERIVED_11,
    PROP_DERIVED_12,
    PROP_DERIVED_13,
    PROP_DERIVED_14,
    PROP_DERIVED_15,
    PROP_DERIVED_16,
    PROP_DERIVED_17,
    PROP_DERIVED_18,
    PROP_DERIVED_19,
    PROP_DERIVED_20,
    PROP_DERIVED_21,
    PROP_DERIVED_22,
    PROP_DERIVED_23,
    PROP_DERIVED_24,
    PROP_DERIVED_25,
    PROP_DERIVED_26,
    PROP_DERIVED_27,
    PROP_DERIVED_28,
    PROP_DERIVED_29,
    PROP_DERIVED_30,
    PROP_DERIVED_31,

    PROP_AFTER_LAST_ITEM,

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // WARNING! Do not add props here unless you intentionally mean to reuse PROP_DERIVED_X indexes
    //
    // These properties intentionally reuse the enum values for other properties which will never overlap with each other. We do this so that we don't have to expand
    // the size of the properties bitflags mask
    //
    // Only add properties here that are only used by one subclass.  Otherwise, they should go above to prevent collisions

    // Particles
    PROP_MAX_PARTICLES = PROP_DERIVED_0,
    PROP_LIFESPAN = PROP_DERIVED_1,
    PROP_EMITTING_PARTICLES = PROP_DERIVED_2,
    PROP_EMIT_RATE = PROP_DERIVED_3,
    PROP_EMIT_SPEED = PROP_DERIVED_4,
    PROP_SPEED_SPREAD = PROP_DERIVED_5,
    PROP_EMIT_ORIENTATION = PROP_DERIVED_6,
    PROP_EMIT_DIMENSIONS = PROP_DERIVED_7,
    PROP_ACCELERATION_SPREAD = PROP_DERIVED_8,
    PROP_POLAR_START = PROP_DERIVED_9,
    PROP_POLAR_FINISH = PROP_DERIVED_10,
    PROP_AZIMUTH_START = PROP_DERIVED_11,
    PROP_AZIMUTH_FINISH = PROP_DERIVED_12,
    PROP_EMIT_RADIUS_START = PROP_DERIVED_13,
    PROP_EMIT_ACCELERATION = PROP_DERIVED_14,
    PROP_PARTICLE_RADIUS = PROP_DERIVED_15,
    PROP_RADIUS_SPREAD = PROP_DERIVED_16,
    PROP_RADIUS_START = PROP_DERIVED_17,
    PROP_RADIUS_FINISH = PROP_DERIVED_18,
    PROP_COLOR_SPREAD = PROP_DERIVED_19,
    PROP_COLOR_START = PROP_DERIVED_20,
    PROP_COLOR_FINISH = PROP_DERIVED_21,
    PROP_ALPHA_SPREAD = PROP_DERIVED_22,
    PROP_ALPHA_START = PROP_DERIVED_23,
    PROP_ALPHA_FINISH = PROP_DERIVED_24,
    PROP_EMITTER_SHOULD_TRAIL = PROP_DERIVED_25,
    PROP_PARTICLE_SPIN = PROP_DERIVED_26,
    PROP_SPIN_START = PROP_DERIVED_27,
    PROP_SPIN_FINISH = PROP_DERIVED_28,
    PROP_SPIN_SPREAD = PROP_DERIVED_29,
    PROP_PARTICLE_ROTATE_WITH_ENTITY = PROP_DERIVED_30,

    // Model
    PROP_MODEL_URL = PROP_DERIVED_0,
    PROP_MODEL_SCALE = PROP_DERIVED_1,
    PROP_JOINT_ROTATIONS_SET = PROP_DERIVED_2,
    PROP_JOINT_ROTATIONS = PROP_DERIVED_3,
    PROP_JOINT_TRANSLATIONS_SET = PROP_DERIVED_4,
    PROP_JOINT_TRANSLATIONS = PROP_DERIVED_5,
    PROP_RELAY_PARENT_JOINTS = PROP_DERIVED_6,
    PROP_GROUP_CULLED = PROP_DERIVED_7,
    // Animation
    PROP_ANIMATION_URL = PROP_DERIVED_8,
    PROP_ANIMATION_ALLOW_TRANSLATION = PROP_DERIVED_9,
    PROP_ANIMATION_FPS = PROP_DERIVED_10,
    PROP_ANIMATION_FRAME_INDEX = PROP_DERIVED_11,
    PROP_ANIMATION_PLAYING = PROP_DERIVED_12,
    PROP_ANIMATION_LOOP = PROP_DERIVED_13,
    PROP_ANIMATION_FIRST_FRAME = PROP_DERIVED_14,
    PROP_ANIMATION_LAST_FRAME = PROP_DERIVED_15,
    PROP_ANIMATION_HOLD = PROP_DERIVED_16,

    // Light
    PROP_IS_SPOTLIGHT = PROP_DERIVED_0,
    PROP_INTENSITY = PROP_DERIVED_1,
    PROP_EXPONENT = PROP_DERIVED_2,
    PROP_CUTOFF = PROP_DERIVED_3,
    PROP_FALLOFF_RADIUS = PROP_DERIVED_4,

    // Text
    PROP_TEXT = PROP_DERIVED_0,
    PROP_LINE_HEIGHT = PROP_DERIVED_1,
    PROP_TEXT_COLOR = PROP_DERIVED_2,
    PROP_TEXT_ALPHA = PROP_DERIVED_3,
    PROP_BACKGROUND_COLOR = PROP_DERIVED_4,
    PROP_BACKGROUND_ALPHA = PROP_DERIVED_5,
    PROP_LEFT_MARGIN = PROP_DERIVED_6,
    PROP_RIGHT_MARGIN = PROP_DERIVED_7,
    PROP_TOP_MARGIN = PROP_DERIVED_8,
    PROP_BOTTOM_MARGIN = PROP_DERIVED_9,

    // Zone
    // Keylight
    PROP_KEYLIGHT_COLOR = PROP_DERIVED_0,
    PROP_KEYLIGHT_INTENSITY = PROP_DERIVED_1,
    PROP_KEYLIGHT_DIRECTION = PROP_DERIVED_2,
    PROP_KEYLIGHT_CAST_SHADOW = PROP_DERIVED_3,
    // Ambient light
    PROP_AMBIENT_LIGHT_INTENSITY = PROP_DERIVED_4,
    PROP_AMBIENT_LIGHT_URL = PROP_DERIVED_5,
    // Skybox
    PROP_SKYBOX_COLOR = PROP_DERIVED_6,
    PROP_SKYBOX_URL = PROP_DERIVED_7,
    // Haze
    PROP_HAZE_RANGE = PROP_DERIVED_8,
    PROP_HAZE_COLOR = PROP_DERIVED_9,
    PROP_HAZE_GLARE_COLOR = PROP_DERIVED_10,
    PROP_HAZE_ENABLE_GLARE = PROP_DERIVED_11,
    PROP_HAZE_GLARE_ANGLE = PROP_DERIVED_12,
    PROP_HAZE_ALTITUDE_EFFECT = PROP_DERIVED_13,
    PROP_HAZE_CEILING = PROP_DERIVED_14,
    PROP_HAZE_BASE_REF = PROP_DERIVED_15,
    PROP_HAZE_BACKGROUND_BLEND = PROP_DERIVED_16,
    PROP_HAZE_ATTENUATE_KEYLIGHT = PROP_DERIVED_17,
    PROP_HAZE_KEYLIGHT_RANGE = PROP_DERIVED_18,
    PROP_HAZE_KEYLIGHT_ALTITUDE = PROP_DERIVED_19,
    // Bloom
    PROP_BLOOM_INTENSITY = PROP_DERIVED_20,
    PROP_BLOOM_THRESHOLD = PROP_DERIVED_21,
    PROP_BLOOM_SIZE = PROP_DERIVED_22,
    PROP_FLYING_ALLOWED = PROP_DERIVED_23,
    PROP_GHOSTING_ALLOWED = PROP_DERIVED_24,
    PROP_FILTER_URL = PROP_DERIVED_25,
    PROP_KEY_LIGHT_MODE = PROP_DERIVED_26,
    PROP_AMBIENT_LIGHT_MODE = PROP_DERIVED_27,
    PROP_SKYBOX_MODE = PROP_DERIVED_28,
    PROP_HAZE_MODE = PROP_DERIVED_29,
    PROP_BLOOM_MODE = PROP_DERIVED_30,
    // Avatar priority
    PROP_AVATAR_PRIORITY = PROP_DERIVED_31,

    // Polyvox
    PROP_VOXEL_VOLUME_SIZE = PROP_DERIVED_0,
    PROP_VOXEL_DATA = PROP_DERIVED_1,
    PROP_VOXEL_SURFACE_STYLE = PROP_DERIVED_2,
    PROP_X_TEXTURE_URL = PROP_DERIVED_3,
    PROP_Y_TEXTURE_URL = PROP_DERIVED_4,
    PROP_Z_TEXTURE_URL = PROP_DERIVED_5,
    PROP_X_N_NEIGHBOR_ID = PROP_DERIVED_6,
    PROP_Y_N_NEIGHBOR_ID = PROP_DERIVED_7,
    PROP_Z_N_NEIGHBOR_ID = PROP_DERIVED_8,
    PROP_X_P_NEIGHBOR_ID = PROP_DERIVED_9,
    PROP_Y_P_NEIGHBOR_ID = PROP_DERIVED_10,
    PROP_Z_P_NEIGHBOR_ID = PROP_DERIVED_11,

    // Web
    PROP_SOURCE_URL = PROP_DERIVED_0,
    PROP_DPI = PROP_DERIVED_1,
    PROP_SCRIPT_URL = PROP_DERIVED_2,
    PROP_MAX_FPS = PROP_DERIVED_3,
    PROP_INPUT_MODE = PROP_DERIVED_4,
    PROP_SHOW_KEYBOARD_FOCUS_HIGHLIGHT = PROP_DERIVED_5,

    // Polyline
    PROP_LINE_POINTS = PROP_DERIVED_0,
    PROP_STROKE_WIDTHS = PROP_DERIVED_1,
    PROP_STROKE_NORMALS = PROP_DERIVED_2,
    PROP_STROKE_COLORS = PROP_DERIVED_3,
    PROP_IS_UV_MODE_STRETCH = PROP_DERIVED_4,
    PROP_LINE_GLOW = PROP_DERIVED_5,
    PROP_LINE_FACE_CAMERA = PROP_DERIVED_6,

    // Shape
    PROP_SHAPE = PROP_DERIVED_0,

    // Material
    PROP_MATERIAL_URL = PROP_DERIVED_0,
    PROP_MATERIAL_MAPPING_MODE = PROP_DERIVED_1,
    PROP_MATERIAL_PRIORITY = PROP_DERIVED_2,
    PROP_PARENT_MATERIAL_NAME = PROP_DERIVED_3,
    PROP_MATERIAL_MAPPING_POS = PROP_DERIVED_4,
    PROP_MATERIAL_MAPPING_SCALE = PROP_DERIVED_5,
    PROP_MATERIAL_MAPPING_ROT = PROP_DERIVED_6,
    PROP_MATERIAL_DATA = PROP_DERIVED_7,
    PROP_MATERIAL_REPEAT = PROP_DERIVED_8,

    // Image
    PROP_IMAGE_URL = PROP_DERIVED_0,
    PROP_EMISSIVE = PROP_DERIVED_1,
    PROP_KEEP_ASPECT_RATIO = PROP_DERIVED_2,
    PROP_SUB_IMAGE = PROP_DERIVED_3,

    // Grid
    PROP_GRID_FOLLOW_CAMERA = PROP_DERIVED_0,
    PROP_MAJOR_GRID_EVERY = PROP_DERIVED_1,
    PROP_MINOR_GRID_EVERY = PROP_DERIVED_2,

    // Gizmo
    PROP_GIZMO_TYPE = PROP_DERIVED_0,
    // Ring
    PROP_START_ANGLE = PROP_DERIVED_1,
    PROP_END_ANGLE = PROP_DERIVED_2,
    PROP_INNER_RADIUS = PROP_DERIVED_3,
    PROP_INNER_START_COLOR = PROP_DERIVED_4,
    PROP_INNER_END_COLOR = PROP_DERIVED_5,
    PROP_OUTER_START_COLOR = PROP_DERIVED_6,
    PROP_OUTER_END_COLOR = PROP_DERIVED_7,
    PROP_INNER_START_ALPHA = PROP_DERIVED_8,
    PROP_INNER_END_ALPHA = PROP_DERIVED_9,
    PROP_OUTER_START_ALPHA = PROP_DERIVED_10,
    PROP_OUTER_END_ALPHA = PROP_DERIVED_11,
    PROP_HAS_TICK_MARKS = PROP_DERIVED_12,
    PROP_MAJOR_TICK_MARKS_ANGLE = PROP_DERIVED_13,
    PROP_MINOR_TICK_MARKS_ANGLE = PROP_DERIVED_14,
    PROP_MAJOR_TICK_MARKS_LENGTH = PROP_DERIVED_15,
    PROP_MINOR_TICK_MARKS_LENGTH = PROP_DERIVED_16,
    PROP_MAJOR_TICK_MARKS_COLOR = PROP_DERIVED_17,
    PROP_MINOR_TICK_MARKS_COLOR = PROP_DERIVED_18,

    // WARNING!!! DO NOT ADD PROPS_xxx here unless you really really meant to.... Add them UP above
};

typedef PropertyFlags<EntityPropertyList> EntityPropertyFlags;

// this is set at the top of EntityItemProperties.cpp to PROP_AFTER_LAST_ITEM - 1.  PROP_AFTER_LAST_ITEM is always
// one greater than the last item property due to the enum's auto-incrementing.
extern EntityPropertyList PROP_LAST_ITEM;

#endif // hifi_EntityPropertyFlags_h
