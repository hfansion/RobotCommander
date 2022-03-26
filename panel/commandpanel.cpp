//
// Created by hamlet on 2022/3/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_commandpanel.h" resolved

#include "commandpanel.h"
#include "ui_commandpanel.h"
#include <QLabel>
#include <QLayoutItem>
#include <QPainter>
#include <QPushButton>
#include "../command/command.h"
#include "../command/anycommand.h"
#include "../command/positioncommand.h"

CommandPanel::CommandPanel(QWidget *parent) :
        PanelBase(parent), ui(new Ui::CommandPanel) {
    ui->setupUi(this);
    ui->commandEditor->setVisible(false);
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &CommandPanel::shot_showCommand);
    slot_addCommand({std::make_shared<PositionCommand>(1, 2)});
    slot_addCommand({std::make_shared<PositionCommand>(1, 2)});
    slot_addCommand({std::make_shared<PositionCommand>(1, 2)});
    slot_addCommand({std::make_shared<AnyCommand>(QByteArray{})});
}

CommandPanel::~CommandPanel() {
    delete ui;
}

void CommandPanel::retranslateUi() {
    ui->retranslateUi(this);
}

void CommandPanel::shot_showCommand(int index) {
    if (ui->listWidget->selectedItems().size() != 1) {
        ui->commandEditor->setVisible(false);
        return;
    }
    if (m_currentIndex >= 0 && m_currentIndex < m_commands.size()) {
        m_commands[m_currentIndex] = ui->commandEditor->getCommandData();
        ui->listWidget->item(m_currentIndex)->setText(m_commands[m_currentIndex].command->toString());
    }
    if (index < 0 || index >= m_commands.size()) {
        ui->commandEditor->setVisible(false);
        m_currentIndex = -1;
    } else {
        ui->commandEditor->setVisible(true);
        m_currentIndex = index;
        ui->commandEditor->setCommandData(m_commands[m_currentIndex]);
    }
}

void CommandPanel::slot_addCommand(const CommandData &commandData) {
    int index = static_cast<int>(m_commands.size());
    m_commands.emplace_back(commandData);
    auto item = new QListWidgetItem(ui->listWidget);
    item->setText(m_commands[index].command->toString());
    ui->listWidget->addItem(item);
    ui->listWidget->setCurrentRow(index);
}

//void CommandPanel::slot_removeCommand(int index) {
//
//}

CommandEditor::CommandEditor(QWidget *parent) :
        QWidget(parent), m_labelName(new QLabel(this)), m_buttonEnabled(new QPushButton(this)),
        m_buttonFixed(new QPushButton(this)), m_form(new QWidget(this)), m_layout(nullptr) {
    m_buttonEnabled->setText("Enabled");
    m_buttonFixed->setText("Fixed");
    auto h_layout = new QHBoxLayout();
    h_layout->addWidget(m_labelName);
    h_layout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    h_layout->addWidget(m_buttonEnabled);
    h_layout->addWidget(m_buttonFixed);

    auto v_layout = new QVBoxLayout(this);
    v_layout->addLayout(h_layout);
    v_layout->addWidget(m_form);
    m_layout = v_layout;

    m_buttonEnabled->setCheckable(true);
    m_buttonFixed->setCheckable(true);
}

void CommandEditor::setCommandData(const CommandData &commandData) {
    m_labelName->setText(commandData.command->getName());
    m_buttonEnabled->setChecked(commandData.is_enabled);
    m_buttonFixed->setChecked(commandData.is_fixed);
    auto form = commandData.command->createForm(this);
    auto item = m_layout->replaceWidget(m_form, form);
    delete item;
    m_form->deleteLater();
    m_form = form;
}

CommandData CommandEditor::getCommandData() {
    auto form = qobject_cast<Form *>(m_form);
    return {form->getCommand(), m_buttonEnabled->isChecked(), m_buttonEnabled->isChecked()};
}
