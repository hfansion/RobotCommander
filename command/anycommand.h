//
// Created by hamlet on 2021/12/7.
//

#ifndef ROBOTCOMMANDER_ANYCOMMAND_H
#define ROBOTCOMMANDER_ANYCOMMAND_H

#include "command.h"

class QLabel;

class AnyCommand : public Command {
public:
    explicit AnyCommand(QByteArray cmd);
    static const QString NAME;

    [[nodiscard]] ProtocolSend getType() const override { return ProtocolSend::Any; }
    [[nodiscard]] QString getName() const override { return NAME; }
    [[nodiscard]] QByteArray encode() const override;
    [[nodiscard]] QString toString() const override;
    [[nodiscard("memory leak")]] Command *copy() const override;
    [[nodiscard("memory leak")]] Form *createForm(QWidget *parent) const override;

private:
    QByteArray m_code;
    AnyCommand() = default;
};

class AnyForm : public Form {
Q_OBJECT
public:
    explicit AnyForm(const AnyCommand *command, QWidget *parent = nullptr);
    [[nodiscard]] std::shared_ptr<Command> getCommand() const override;

private:
    QLabel *m_label;
    std::shared_ptr<AnyCommand> m_command;
};

#endif //ROBOTCOMMANDER_ANYCOMMAND_H
