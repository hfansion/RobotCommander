//
// Created by hamlet on 2022/3/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_commandpanel.h" resolved

#include "commandpanel.h"
#include "ui_commandpanel.h"
#include <QLabel>
#include <QLayoutItem>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include "../command/command.h"
#include "../command/anycommand.h"
#include "../command/pid1command.h"
#include "../command/pid2command.h"
#include "../command/positioncommand.h"
#include "../command/setgpscommand.h"
#include "../compositor.h"

CommandPanel::CommandPanel(QWidget *parent) :
        PanelBase(parent), ui(new Ui::CommandPanel) {
    this->installEventFilter(this);
    ui->setupUi(this);
    ui->commandEditor->setVisible(false);

    auto menu = new QMenu(this);
    menu->addAction(PositionCommand::NAME, [this]() {
        addCommand(std::make_shared<PositionCommand>(0, 0, 0.0f));
    });
    menu->addAction(AnyCommand::NAME, [this]() {
        addCommand(std::make_shared<AnyCommand>(""));
    });
    menu->addAction(PID1Command::NAME, [this]() {
        addCommand(std::make_shared<PID1Command>(0.0f, 0.0f, 0.0f));
    });
    menu->addAction(PID2Command::NAME, [this]() {
        addCommand(std::make_shared<PID2Command>(0.0f, 0.0f, 0.0f));
    });
    menu->addAction(SetGPSCommand::NAME, [this]() {
        addCommand(std::make_shared<SetGPSCommand>());
    });
    connect(ui->pushButton_add, &QPushButton::clicked, [this, menu]() {
        menu->exec(ui->pushButton_add->mapToGlobal(ui->pushButton_add->geometry().bottomLeft()));
    });

    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &CommandPanel::shot_showCommand);
    connect(ui->listWidget, &QListWidget::itemSelectionChanged, [this]() {
        if (ui->listWidget->selectedItems().empty()) ui->commandEditor->setVisible(false);
    });
    connect(ui->pushButton_remove, &QPushButton::clicked, this, &CommandPanel::slot_removeCommand);
    connect(ui->pushButton_sweep, &QPushButton::clicked, this, &CommandPanel::slot_clearCommand);
    connect(ui->pushButton_send, &QPushButton::clicked, [this]() {
        shot_showCommand(m_currentIndex);
        emit startSendAll();
    });
}

CommandPanel::~CommandPanel() {
    delete ui;
}

void CommandPanel::retranslateUi() {
    ui->retranslateUi(this);
}

void CommandPanel::shot_showCommand(int index) {
    if (m_currentIndex >= 0 && m_currentIndex < m_commands.size()) {
        m_commands[m_currentIndex] = ui->commandEditor->getCommandData();
        ui->listWidget->item(m_currentIndex)->setText(m_commands[m_currentIndex].command->toString());
    }
    if (m_currentIndex == index) return;
    if (index < 0 || index >= m_commands.size()) {
        ui->commandEditor->setVisible(false);
        m_currentIndex = -1;
    } else {
        ui->commandEditor->setVisible(true);
        m_currentIndex = index;
        ui->commandEditor->setCommandData(m_commands[m_currentIndex]);
    }
}

void CommandPanel::addCommand(const std::shared_ptr<Command>& command) {
    int index = static_cast<int>(m_commands.size());
    m_commands.emplace_back(command);
    auto item = new QListWidgetItem(ui->listWidget);
    item->setText(m_commands[index].command->toString());
    ui->listWidget->addItem(item);
    ui->listWidget->setCurrentRow(index);
}

void CommandPanel::slot_removeCommand() {
    if (ui->listWidget->selectedItems().size() != 1) {
        return;
    }
    int index = ui->listWidget->currentRow();
    if (index < 0 || index >= m_commands.size()) {
        return;
    }
    m_commands.erase(m_commands.begin() + index);
    ui->listWidget->takeItem(index);
    if (index >= m_commands.size()) {
        index = m_commands.size() - 1;
    }
    ui->listWidget->setCurrentRow(index);
}

void CommandPanel::slot_clearCommand() {
    m_commands.clear();
    ui->listWidget->clear();
    m_currentIndex = -1;
}

bool CommandPanel::eventFilter(QObject *object, QEvent *event) {
    if (object == this && event->type() == QEvent::Leave) {
        shot_showCommand(m_currentIndex);
    }
    return QWidget::eventFilter(object, event);
}

void CommandPanel::sendAll(Compositor *compositor) {
    for (auto& data : m_commands) {
        compositor->addCommand(std::unique_ptr<Command>(data.command->copy()));
    }
    compositor->send();
    slot_clearCommand();
}

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
    m_buttonEnabled->setVisible(false);
    m_buttonFixed->setVisible(false);
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
