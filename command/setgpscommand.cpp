//
// Created by hamlet on 2022/4/3.
//

#include "setgpscommand.h"

const QString SetGPSCommand::NAME{"Set GPS on/off"};

QByteArray SetGPSCommand::encode() const {
    QByteArray code{};
    code += static_cast<char>(getType());
    return std::move(code);
}

QString SetGPSCommand::toString() const {
    return NAME;
}

Command *SetGPSCommand::copy() const {
    return new SetGPSCommand;
}

Form *SetGPSCommand::createForm(QWidget *parent) const {
    return new SetGPSForm(parent);
}

std::shared_ptr<Command> SetGPSForm::getCommand() const {
    return std::make_shared<SetGPSCommand>();
}
