//
// Created by hamlet on 2021/12/7.
//

#include "anycommand.h"
#include <utility>
#include <QHBoxLayout>
#include <QLabel>
#include "../data/datadisplayer.h"

const QString AnyCommand::NAME{"Any"};

AnyCommand::AnyCommand(QByteArray cmd) : m_code(std::move(cmd)) {
    m_code.insert(0, (char) ((char8_t) m_code.length() | '\x80'));
}

QByteArray AnyCommand::encode() const {
    return m_code;
}

QString AnyCommand::toString() const {
    QString text = QString("Any(%1)").arg(DataDisplayer::toASCII(m_code.right(m_code.length() - 1)));
    return std::move(text);
}

Command *AnyCommand::copy() const {
    auto command = new AnyCommand();
    command->m_code = m_code;
    return command;
}

Form *AnyCommand::createForm(QWidget *parent) const {
    return new AnyForm(this, parent);
}

AnyForm::AnyForm(const AnyCommand *command, QWidget *parent) :
        Form(parent), m_label(new QLabel("暂不支持编辑Any，请期待后续版本", this)) {
    auto layout = new QHBoxLayout(this);
    layout->addWidget(m_label);
    m_command = std::shared_ptr<AnyCommand>(dynamic_cast<AnyCommand *>(command->copy()));
}

std::shared_ptr<Command> AnyForm::getCommand() const {
    return m_command;
}
