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
#include "../command/positioncommand.h"

static int current_row = 0;
static QRect current_rect;

CommandPanel::CommandPanel(QWidget *parent) :
        PanelBase(parent), ui(new Ui::CommandPanel), m_commandModel(new CommandModel(this)) {
    ui->setupUi(this);
    ui->listView->setModel(m_commandModel);
    ui->listView->setItemDelegate(new CommandDelegate(this));
//    connect(ui->listView, &QListView::doubleClicked, [this]() {
//        ui->listView->adjustSize();
//    });
}

CommandPanel::~CommandPanel() {
    delete ui;
}

void CommandPanel::retranslateUi() {
    ui->retranslateUi(this);
}

CommandModel::CommandModel(QObject *parent) : QAbstractListModel(parent) {
    qRegisterMetaType<CommandData>();
    for (int i = 0; i < 400; i += 10) {
        m_commands.emplace_back(std::make_shared<PositionCommand>(i, i));
    }
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
        current_row = index.row();
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

void CommandDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const {
    current_rect = option.rect;
    current_rect.setHeight(130);
    editor->setGeometry(current_rect);
}

QSize CommandDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
//    if (index.row() == current_row) {
//        return current_size;
//    } else {
    return QStyledItemDelegate::sizeHint(option, index);
//    }
}

void CommandDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(current_rect, option.palette.highlight());
    }
    QStyledItemDelegate::paint(painter, option, index);
}

CommandEditor::CommandEditor(QWidget *parent) :
        QWidget(parent), m_labelName(new QLabel(this)), m_buttonEnabled(new QPushButton(this)),
        m_buttonFixed(new QPushButton(this)), m_form(new QWidget(this)), m_layout(nullptr) {
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
    CommandData commandData{form->getCommand(), m_buttonEnabled->isChecked(), m_buttonEnabled->isChecked()};
    return commandData;
}
