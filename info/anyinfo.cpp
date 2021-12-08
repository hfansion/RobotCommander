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
    QString text = QString("Any(%1)").arg(HexDisplayer::toString(DATA));
    return std::move(text);
}

AnyInfo::AnyInfo(QByteArray data)
        : DATA_LENGTH(data.length()), DATA(std::move(data)) {
}
