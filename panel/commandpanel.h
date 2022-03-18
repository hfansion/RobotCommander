//
// Created by hamlet on 2022/3/18.
//

#ifndef ROBOTCOMMANDER_COMMANDPANEL_H
#define ROBOTCOMMANDER_COMMANDPANEL_H

#include <vector>
#include <QAbstractListModel>
#include <QStyledItemDelegate>
#include "panelbase.h"

class Command;

QT_BEGIN_NAMESPACE
namespace Ui { class CommandPanel; }
QT_END_NAMESPACE


class CommandModel : public QAbstractListModel {
Q_OBJECT
public:
    CommandModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    std::vector<std::unique_ptr<Command>> m_commands;
};


class CommandEditor : public QWidget {
Q_OBJECT
public:
    explicit CommandEditor(QWidget *parent = nullptr);
    ~CommandEditor() override;
    void setCommand(const Command *command);
    void getCommand(std::unique_ptr<Command> command);
};


class CommandDelegate : public QStyledItemDelegate {
Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
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
