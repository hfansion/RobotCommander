//
// Created by hamlet on 2022/4/3.
//

#ifndef ROBOTCOMMANDER_PID2COMMAND_H
#define ROBOTCOMMANDER_PID2COMMAND_H

#include "../data/pid.h"
#include "command.h"

class QDoubleSpinBox;

class PID2Command : public Command, public PID {
public:
    using PID::PID;
    static const QString NAME;

    [[nodiscard]] Protocol getType() const override { return Protocol::PID2; }
    [[nodiscard]] QString getName() const override { return NAME; }
    [[nodiscard]] QByteArray encode() const override;
    [[nodiscard]] QString toString() const override;
    [[nodiscard("memory leak")]] Command *copy() const override;
    [[nodiscard("memory leak")]] Form *createForm(QWidget *parent) const override;
};

class PID2Form : public Form {
Q_OBJECT
public:
    explicit PID2Form(const PID2Command *command, QWidget *parent = nullptr);
    [[nodiscard]] std::shared_ptr<Command> getCommand() const override;
private:
    QDoubleSpinBox *spinBox_p;
    QDoubleSpinBox *spinBox_i;
    QDoubleSpinBox *spinBox_d;
};

#endif //ROBOTCOMMANDER_PID2COMMAND_H
