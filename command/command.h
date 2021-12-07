//
// Created by hamlet on 2021/12/5.
//

#ifndef ROBOTCOMMANDER_COMMAND_H
#define ROBOTCOMMANDER_COMMAND_H

#include <QByteArray>
#include "../protocol.h"

class Command {
public:
    virtual ~Command() = default;
    [[nodiscard]] virtual QByteArray encode() const = 0;
    [[nodiscard]] virtual QString toString() const = 0;
};


#endif //ROBOTCOMMANDER_COMMAND_H
