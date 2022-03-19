//
// Created by hamlet on 2022/3/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_commandpanel.h" resolved

#include "commandpanel.h"
#include "ui_commandpanel.h"
#include <QLabel>
#include <QLayoutItem>
#include <QPushButton>
#include "../command/command.h"
#include "../command/positioncommand.h"

CommandPanel::CommandPanel(QWidget *parent) :
        PanelBase(parent), ui(new Ui::CommandPanel), m_commandModel(new CommandModel(this)) {
    ui->setupUi(this);
    ui->listView->setModel(m_commandModel);
    ui->listView->setItemDelegate(new CommandDelegate);
}

CommandPanel::~CommandPanel() {
    delete ui;
}

void CommandPanel::retranslateUi() {
    ui->retranslateUi(this);
}

CommandModel::CommandModel(QObject *parent) : QAbstractListModel(parent) {
    qRegisterMetaType<CommandData>();
    m_commands.emplace_back(std::make_shared<PositionCommand>(10, 10));
}

int CommandModel::rowCount(const QModelIndex &parent) const {
    return static_cast<int>(m_commands.size());
}

QVariant CommandModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        return m_commands[index.row()].command->toString();
    } else if (role == Qt::EditRole) {
        return QVariant::fromValue(m_commands[index.row()]);
    }
    return {};
}

bool CommandModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        if (index.row() >= m_commands.size() || !value.canConvert<CommandData>())
            return false;
        m_commands[index.row()] = value.value<CommandData>();
        return true;
    }
    return false;
}

Qt::ItemFlags CommandModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractListModel::flags(index);
}

QWidget *CommandDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const {
    if (index.data(Qt::EditRole).canConvert<CommandData>()) {
        return new CommandEditor(parent);
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void CommandDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    if (index.data(Qt::EditRole).canConvert<CommandData>()) {
        auto data = index.data(Qt::EditRole).value<CommandData>();
        auto commandEditor = qobject_cast<CommandEditor *>(editor);
        commandEditor->setCommandData(data);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void CommandDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    if (index.data(Qt::EditRole).canConvert<CommandData>()) {
        auto commandEditor = qobject_cast<CommandEditor *>(editor);
        model->setData(index, QVariant::fromValue(commandEditor->getCommandData()));
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

CommandEditor::CommandEditor(QWidget *parent) :
        QWidget(parent), m_labelName(new QLabel(this)), m_buttonEnabled(new QPushButton(this)),
        m_buttonFixed(new QPushButton(this)), m_form(new QWidget(this)), m_layout(nullptr) {
    auto h_layout = new QHBoxLayout();
    h_layout->addWidget(m_labelName);
    h_layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
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
    auto form = commandData.command->createForm(this);
    auto item = m_layout->replaceWidget(m_form, form);
    if (item == nullptr) {
        qDebug() << "有大问题";
    }
    delete item;
    m_form->deleteLater();
    m_form = form;
}

CommandData CommandEditor::getCommandData() {
    auto form = qobject_cast<Form *>(m_form);
    return {form->getCommand(), m_buttonEnabled->isChecked(), m_buttonEnabled->isChecked()};
}
