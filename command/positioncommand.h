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

    [[nodiscard]] QByteArray encode() const override;
    [[nodiscard]] QString toString() const override;
};

#endif //ROBOTCOMMANDER_POSITIONCOMMAND_H
