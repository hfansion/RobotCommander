//
// Created by hamlet on 2021/12/5.
//

#include "info.h"

int Info::X_RANGE{10000};
int Info::Y_RANGE{10000};

Info::Info(int x, int y) : x(x), y(y) {
}

QString Info::toString() const {
    return qMove(QString("Position(%1,%2)").arg(x).arg(y));
}

Info Info::decode(const QByteArray &data) {
    if (data.length() != DATA_LENGTH) return Info(-1, -1);
    int x = (((int) ((char8_t) data.at(0))) << 8) + (int) ((char8_t) data.at(1));
    int y = (((int) ((char8_t) data.at(2))) << 8) + (int) ((char8_t) data.at(3));
    return Info(x, y);
}

void Info::SET_RANGE(int x_range, int y_range) {
    X_RANGE = x_range;
    Y_RANGE = y_range;
}
