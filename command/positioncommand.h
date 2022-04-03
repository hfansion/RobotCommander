//
// Created by hamlet on 2021/12/6.
//

#ifndef ROBOTCOMMANDER_POSITIONCOMMAND_H
#define ROBOTCOMMANDER_POSITIONCOMMAND_H

#include "../data/position.h"
#include "command.h"

class QSpinBox;
class QDoubleSpinBox;

class PositionCommand : public Position, public Command {
public:
    using Position::Position;
    static const QString NAME;

    [[nodiscard]] Protocol getType() const override { return Protocol::Position; }
    [[nodiscard]] QString getName() const override { return NAME; }
    [[nodiscard]] QByteArray encode() const override;
    [[nodiscard]] QString toString() const override;
    [[nodiscard("memory leak")]] Command *copy() const override;
    [[nodiscard("memory leak")]] Form *createForm(QWidget *parent) const override;
};

class PositionForm : public Form {
Q_OBJECT
public:
    explicit PositionForm(const PositionCommand *command, QWidget *parent = nullptr);
    [[nodiscard]] std::shared_ptr<Command> getCommand() const override;
private:
    QSpinBox *spinBox_X;
    QSpinBox *spinBox_Y;
    QDoubleSpinBox *spinBox_R;
};

#endif //ROBOTCOMMANDER_POSITIONCOMMAND_H
