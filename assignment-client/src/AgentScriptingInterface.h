//
//  AgentScriptingInterface.h
//  assignment-client/src
//
//  Created by Thijs Wenker on 7/23/18.
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once

#ifndef hifi_AgentScriptingInterface_h
#define hifi_AgentScriptingInterface_h

#include <QObject>

#include "Agent.h"

/**jsdoc
 * The <code>Agent</code> API enables an assignment client to emulate an avatar. In particular, setting <code>isAvatar = 
 * true</code> connects the assignment client to the avatar and audio mixers and enables the {@link Avatar} API to be used.
 *
 * @namespace Agent
 *
 * @hifi-assignment-client
 *
 * @property {boolean} isAvatar - <code>true</code> if the assignment client script is emulating an avatar, otherwise 
 *     <code>false</code>.
 * @property {boolean} isPlayingAvatarSound - <code>true</code> if the script has a sound to play, otherwise <code>false</code>. 
 *     Sounds are played when <code>isAvatar</code> is <code>true</code>, from the position and with the orientation of the 
 *     scripted avatar's head.<em>Read-only.</em>
 * @property {boolean} isListeningToAudioStream - <code>true</code> if the agent is "listening" to the audio stream from the 
 *     domain, otherwise <code>false</code>.
 * @property {boolean} isNoiseGateEnabled - <code>true</code> if the noise gate is enabled, otherwise <code>false</code>. When 
 * enabled, the input audio stream is blocked (fully attenuated) if it falls below an adaptive threshold.
 * @property {number} lastReceivedAudioLoudness - The current loudness of the audio input, nominal range <code>0.0</code> (no 
 *     sound) &ndash; <code>1.0</code> (the onset of clipping). <em>Read-only.</em>
 * @property {Uuid} sessionUUID - The unique ID associated with the agent's current session in the domain. <em>Read-only.</em>
 */
class AgentScriptingInterface : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isAvatar READ isAvatar WRITE setIsAvatar)
    Q_PROPERTY(bool isPlayingAvatarSound READ isPlayingAvatarSound)
    Q_PROPERTY(bool isListeningToAudioStream READ isListeningToAudioStream WRITE setIsListeningToAudioStream)
    Q_PROPERTY(bool isNoiseGateEnabled READ isNoiseGateEnabled WRITE setIsNoiseGateEnabled)
    Q_PROPERTY(float lastReceivedAudioLoudness READ getLastReceivedAudioLoudness)
    Q_PROPERTY(QUuid sessionUUID READ getSessionUUID)

public:
    AgentScriptingInterface(Agent* agent);

    bool isPlayingAvatarSound() const { return _agent->isPlayingAvatarSound(); }

    bool isListeningToAudioStream() const { return _agent->isListeningToAudioStream(); }
    void setIsListeningToAudioStream(bool isListeningToAudioStream) const { _agent->setIsListeningToAudioStream(isListeningToAudioStream); }

    bool isNoiseGateEnabled() const { return _agent->isNoiseGateEnabled(); }
    void setIsNoiseGateEnabled(bool isNoiseGateEnabled) const { _agent->setIsNoiseGateEnabled(isNoiseGateEnabled); }

    float getLastReceivedAudioLoudness() const { return _agent->getLastReceivedAudioLoudness(); }
    QUuid getSessionUUID() const { return _agent->getSessionUUID(); }

public slots:
    /**jsdoc
     * Set whether or not the script should emulate an avatar.
     * @function Agent.setIsAvatar
     * @param {boolean} isAvatar - <code>true</code> if the script should act as if an avatar, otherwise <code>false</code>.
     */
    void setIsAvatar(bool isAvatar) const { _agent->setIsAvatar(isAvatar); }

    /**jsdoc
     * Check whether or not the script is emulating an avatar.
     * @function Agent.isAvatar
     * @returns {boolean} <code>true</code> if the script is acting as if an avatar, otherwise <code>false</code>.
     */
    bool isAvatar() const { return _agent->isAvatar(); }

    /**jsdoc
     * Play a sound from the position and with the orientation of the emulated avatar's head. No sound is played unless 
     * <code>isAvatar == true</code>.
     * @function Agent.playAvatarSound
     * @param {SoundObject} avatarSound - The sound to play.
     */
    void playAvatarSound(SharedSoundPointer avatarSound) const { _agent->playAvatarSound(avatarSound); }

private:
    Agent* _agent;

};


#endif // hifi_AgentScriptingInterface_h
