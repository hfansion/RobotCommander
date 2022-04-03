//
// Created by hamlet on 2022/4/3.
//

#ifndef ROBOTCOMMANDER_SETGPSCOMMAND_H
#define ROBOTCOMMANDER_SETGPSCOMMAND_H

#include "command.h"

class SetGPSCommand : public Command {
public:
    static const QString NAME;
    SetGPSCommand() = default;
    [[nodiscard]] Protocol getType() const override { return Protocol::SET_GPS; }
    [[nodiscard]] QString getName() const override { return NAME; }
    [[nodiscard]] QByteArray encode() const override;
    [[nodiscard]] QString toString() const override;
    [[nodiscard("memory leak")]] Command *copy() const override;
    [[nodiscard("memory leak")]] Form *createForm(QWidget *parent) const override;
};

class SetGPSForm : public Form {
Q_OBJECT
public:
    using Form::Form;
    [[nodiscard]] std::shared_ptr<Command> getCommand() const override;
};

#endif //ROBOTCOMMANDER_SETGPSCOMMAND_H
