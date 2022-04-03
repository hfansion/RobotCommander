//
// Created by hamlet on 2021/12/6.
//

#include "positioncommand.h"
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>

const QString PositionCommand::NAME{"Position"};

inline void appendFloat(QByteArray &code, float value) { // 在code末尾添加一个4位float值
    code.append(reinterpret_cast<const char *>(&value), sizeof(value));
}

QByteArray PositionCommand::encode() const {
    QByteArray code{};
    code += static_cast<char>(getType());
    auto *p = (char8_t *) &(this->x);
    code.append((char) (p[1])).append((char) (p[0]));
    p = (char8_t *) &(this->y);
    code.append((char) (p[1])).append((char) (p[0]));
    appendFloat(code, r);
    return std::move(code);
}

QString PositionCommand::toString() const {
    return Position::toString();
}

Form *PositionCommand::createForm(QWidget *parent) const {
    return new PositionForm(this, parent);
}

Command *PositionCommand::copy() const {
    return new PositionCommand(this->x, this->y, this->r);
}

PositionForm::PositionForm(const PositionCommand *command, QWidget *parent) :
        Form(parent), spinBox_X(new QSpinBox(this)), spinBox_Y(new QSpinBox(this)),
        spinBox_R(new QDoubleSpinBox(this)) {
    auto label_X = new QLabel(this), label_Y = new QLabel(this), label_R = new QLabel(this);
    label_X->setText(tr("X:")), label_Y->setText(tr("Y:")), label_R->setText(tr("R:"));
    spinBox_X->setMinimum(0), spinBox_X->setMaximum(Position::X_RANGE);
    spinBox_X->setValue(command->x);
    spinBox_Y->setMinimum(0), spinBox_Y->setMaximum(Position::Y_RANGE);
    spinBox_Y->setValue(command->y);
    spinBox_R->setDecimals(3);
    spinBox_R->setMinimum(-1.000), spinBox_R->setMaximum(1.000);
    spinBox_R->setSingleStep(0.050);
    spinBox_R->setSuffix("\317\200");
    auto layout = new QHBoxLayout(this);
    layout->addWidget(label_X), layout->addWidget(spinBox_X);
    layout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout->addWidget(label_Y), layout->addWidget(spinBox_Y);
    layout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout->addWidget(label_R), layout->addWidget(spinBox_R);
    layout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout->setStretch(2, 1);
    layout->setStretch(5, 1);
    layout->setStretch(8, 2);
}

std::shared_ptr<Command> PositionForm::getCommand() const {
    return std::make_shared<PositionCommand>(spinBox_X->value(), spinBox_Y->value(), spinBox_R->value());
}
