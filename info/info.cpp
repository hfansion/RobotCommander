//
// Created by hamlet on 2021/12/5.
//

#include "info.h"

Info::Info(int x, int y) : x(x), y(y) {
}

QString Info::toString() const {
    return qMove(QString("(%1,%2)").arg(x).arg(y));
}

Info Info::decode(const QByteArray &data) {
    if (data.length() != DATA_LENGTH) return Info(-1, -1);
    int x = (((int) ((char8_t) data.at(1))) << 8) + (int) ((char8_t) data.at(0));
    int y = (((int) ((char8_t) data.at(3))) << 8) + (int) ((char8_t) data.at(2));
    return Info(x, y);
}
