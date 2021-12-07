//
// Created by hamlet on 2021/12/7.
//

#include "anyinfo.h"

#include <utility>
#include "../data/hexdisplayer.h"

ProtocolReceive AnyInfo::getInfoType() const {
    return ProtocolReceive::Any;
}

QString AnyInfo::toString() const {
    return std::move(HexDisplayer::toString(DATA));
}

AnyInfo::AnyInfo(QByteArray data)
        : DATA_LENGTH(data.length()), DATA(std::move(data)) {
}
