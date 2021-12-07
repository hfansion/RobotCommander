//
// Created by hamlet on 2021/12/7.
//

#include "positioninfo.h"

PositionInfo PositionInfo::decode(const QByteArray &data) {
    if (data.length() != DATA_LENGTH) return std::move(PositionInfo{-1, -1});
    int x = (((int) ((char8_t) data.at(0))) << 8) + (int) ((char8_t) data.at(1));
    int y = (((int) ((char8_t) data.at(2))) << 8) + (int) ((char8_t) data.at(3));
    return std::move(PositionInfo{x, y});
}

ProtocolReceive PositionInfo::getInfoType() const {
    return ProtocolReceive::Position;
}

QString PositionInfo::toString() const {
    return Position::toString();
}
