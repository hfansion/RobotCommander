//
// Created by hamlet on 2021/12/7.
//

#include "position.h"
#include <QString>

int Position::X_RANGE{12000};
int Position::Y_RANGE{12000};

void Position::SET_RANGE(int x_range, int y_range) {
    X_RANGE = x_range;
    Y_RANGE = y_range;
}

Position::Position(int x, int y) : x(x), y(y) {
}

QString Position::toString() const {
    return std::move(QString("Position(%1,%2)").arg(x).arg(y));
}
