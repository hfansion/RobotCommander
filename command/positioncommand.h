//
// Created by hamlet on 2021/12/6.
//

#ifndef ROBOTCOMMANDER_POSITIONCOMMAND_H
#define ROBOTCOMMANDER_POSITIONCOMMAND_H

#include "../data/position.h"
#include "command.h"

class PositionCommand : public Position, public Command {
public:
    using Position::Position;

    [[nodiscard]] QByteArray encode() const override;
    [[nodiscard]] QString toString() const override;
};

#endif //ROBOTCOMMANDER_POSITIONCOMMAND_H
