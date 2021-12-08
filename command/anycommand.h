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
    [[nodiscard("you should maintain the life of this pointer")]]
    Command *copy() const;

private:
    QByteArray m_code;
    AnyCommand();
};


#endif //ROBOTCOMMANDER_ANYCOMMAND_H
