//
// Created by hamlet on 2021/12/6.
//

#include "positioncommand.h"

QByteArray PositionCommand::encode() const {
    QByteArray code{};
    auto *p = (char8_t *) &(this->x);
    code.append((char) (p[1])).append((char) (p[0]));
    p = (char8_t *) &(this->y);
    code.append((char) (p[1])).append((char) (p[0]));
    return qMove(code);
}

QString PositionCommand::toString() const {
    return Info::toString();
}
