//
// Created by hamlet on 2022/4/3.
//

#include "pid.h"
#include <QString>

PID::PID(float p, float i, float d) : p(p), i(i), d(d) {
}

QString PID::toString() const {
    return std::move(QString("(%1,%2,%3)").arg(p).arg(i).arg(d));
}
