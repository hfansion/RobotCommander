//
// Created by hamlet on 2021/12/7.
//

#ifndef ROBOTCOMMANDER_ANYCOMMAND_H
#define ROBOTCOMMANDER_ANYCOMMAND_H

#include "command.h"

class AnyCommand : public Command {
public:
    explicit AnyCommand(QByteArray cmd);

    [[nodiscard]] QByteArray encode() const override;
    [[nodiscard]] QString toString() const override;

private:
    QByteArray m_code;
};


#endif //ROBOTCOMMANDER_ANYCOMMAND_H
