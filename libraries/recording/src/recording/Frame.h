//
//  Created by Bradley Austin Davis 2015/11/04
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#pragma once
#ifndef hifi_Recording_Frame_h
#define hifi_Recording_Frame_h

#include "Forward.h"

#include <functional>

#include <QtCore/QObject>

namespace recording {

struct Frame {
public:
    using Pointer = std::shared_ptr<Frame>;
    using Handler = std::function<void(Frame::Pointer frame)>;

    static const FrameType TYPE_INVALID = 0xFFFF;
    static const FrameType TYPE_HEADER = 0x0;
    FrameType type { TYPE_INVALID };
    float timeOffset { 0 };
    QByteArray data;

    Frame() {}
    Frame(FrameType type, float timeOffset, const QByteArray& data) 
        : type(type), timeOffset(timeOffset), data(data) {}

    static FrameType registerFrameType(const QString& frameTypeName);
    static QMap<QString, FrameType> getFrameTypes();
    static QMap<FrameType, QString> getFrameTypeNames();
    static Handler registerFrameHandler(FrameType type, Handler handler);
};

}

#endif
