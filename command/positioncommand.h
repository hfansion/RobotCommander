//
// Created by hamlet on 2021/12/6.
//

#ifndef ROBOTCOMMANDER_POSITIONCOMMAND_H
#define ROBOTCOMMANDER_POSITIONCOMMAND_H

#include "../info/info.h"
#include "command.h"

class PositionCommand : public Info, public Command {
public:
    using Info::Info;
    using Info::toString;

    [[nodiscard]] const QByteArray &encode() const override;
};

#endif //ROBOTCOMMANDER_POSITIONCOMMAND_H
