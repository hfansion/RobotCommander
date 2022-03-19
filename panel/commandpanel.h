//
// Created by hamlet on 2022/3/18.
//

#ifndef ROBOTCOMMANDER_COMMANDPANEL_H
#define ROBOTCOMMANDER_COMMANDPANEL_H

#include <utility>
#include <utility>
#include <utility>
#include <vector>
#include <QAbstractListModel>
#include <QStyledItemDelegate>
#include "panelbase.h"

class Command;

QT_BEGIN_NAMESPACE
namespace Ui { class CommandPanel; }
class QLabel;
class QPushButton;
QT_END_NAMESPACE


struct CommandData {
    std::shared_ptr<Command> command;
    bool is_enabled = true;
    bool is_fixed = false;

    CommandData() = default;
    CommandData(std::shared_ptr<Command> command, bool enabled = true, bool fixed = false) :
            command(std::move(command)), is_enabled(enabled), is_fixed(fixed) {}
    ~CommandData() = default;
    CommandData(const CommandData &) = default;
    CommandData &operator=(const CommandData &) = default;
};
Q_DECLARE_METATYPE(CommandData);


class CommandModel : public QAbstractListModel {
Q_OBJECT
public:
    explicit CommandModel(QObject *parent = nullptr);
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
//    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    std::vector<CommandData> m_commands;
};


class CommandEditor : public QWidget {
Q_OBJECT
public:
    explicit CommandEditor(QWidget *parent = nullptr);
    void setCommandData(const CommandData &commandData);
    CommandData getCommandData();

private:
    QLabel      *m_labelName;
    QPushButton *m_buttonEnabled;
    QPushButton *m_buttonFixed;
    QWidget     *m_form;
    QLayout     *m_layout;
};


class CommandDelegate : public QStyledItemDelegate {
Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

//    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
//    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};


class CommandPanel : public PanelBase {
Q_OBJECT
public:
    explicit CommandPanel(QWidget *parent = nullptr);
    ~CommandPanel() override;
    QString PanelName() override { return std::move(tr("Command Queue")); }
    void retranslateUi() override;

private:
    Ui::CommandPanel *ui;
    CommandModel *m_commandModel;
};


#endif //ROBOTCOMMANDER_COMMANDPANEL_H
