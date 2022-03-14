//
// Created by hamlet on 2022/3/14.
//

#ifndef ROBOTCOMMANDER_SENDERPANEL_H
#define ROBOTCOMMANDER_SENDERPANEL_H

#include "panelbase.h"

class AnyCommand;
class Command;

QT_BEGIN_NAMESPACE
namespace Ui { class SenderPanel; }
QT_END_NAMESPACE

class SenderPanel : public PanelBase {
Q_OBJECT

public:
    explicit SenderPanel(QWidget *parent = nullptr);
    ~SenderPanel() override;
    QString PanelName() override { return qMove(tr("Sender")); }
    void retranslateUi() override;

signals:
    void sendCommand(Command *command);

private slots:
    void preview(const QString &text);

private:
    Ui::SenderPanel *ui;
    AnyCommand *m_LS_tmpCmd = nullptr;
};


#endif //ROBOTCOMMANDER_SENDERPANEL_H
