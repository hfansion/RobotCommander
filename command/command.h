//
// Created by hamlet on 2021/12/5.
//

#ifndef ROBOTCOMMANDER_COMMAND_H
#define ROBOTCOMMANDER_COMMAND_H

#include <QByteArray>
#include <QWidget>
#include "../protocol.h"

class Form;

class Command {
public:
    virtual ~Command() = default;
    [[nodiscard]] virtual ProtocolSend getType() const = 0;
    [[nodiscard]] virtual QString getName() const = 0;
    [[nodiscard]] virtual QByteArray encode() const = 0;
    [[nodiscard]] virtual QString toString() const = 0;
    [[nodiscard("memory leak")]] virtual Command *copy() const = 0;
    [[nodiscard("memory leak")]] virtual Form *createForm(QWidget *parent) const = 0;
};

class Form : public QWidget {
Q_OBJECT
public:
    using QWidget::QWidget;
    [[nodiscard]] virtual std::shared_ptr<Command> getCommand() const = 0;
};

#endif //ROBOTCOMMANDER_COMMAND_H
