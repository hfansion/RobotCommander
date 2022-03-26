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
#include "../protocol.h"

class Command;
class Compositor;

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


class CommandPanel : public PanelBase {
Q_OBJECT
public:
    explicit CommandPanel(QWidget *parent = nullptr);
    ~CommandPanel() override;
    QString PanelName() override { return std::move(tr("Command Queue")); }
    void retranslateUi() override;
    void addCommand(const std::shared_ptr<Command>& command);
    void sendAll(Compositor *compositor);
signals:
    void startSendAll();

public slots:
    void slot_removeCommand();
    void shot_showCommand(int index);
    void slot_clearCommand();

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    Ui::CommandPanel *ui;
    std::vector<CommandData> m_commands;
    int m_currentIndex = -1;
};


#endif //ROBOTCOMMANDER_COMMANDPANEL_H
