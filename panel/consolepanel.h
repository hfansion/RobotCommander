//
// Created by hamlet on 2022/3/14.
//

#ifndef ROBOTCOMMANDER_CONSOLEPANEL_H
#define ROBOTCOMMANDER_CONSOLEPANEL_H

#include "panelbase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ConsolePanel; }
QT_END_NAMESPACE

class Info;
class Command;

class ConsolePanel : public PanelBase {
Q_OBJECT

public:
    explicit ConsolePanel(QWidget *parent = nullptr);
    ~ConsolePanel() override;
    QString PanelName() override { return std::move(tr("Console")); }
    void retranslateUi() override;

    void appendMessage(const QString &content);
    void appendInfo(const Info *info);

private slots:
    void CS_clear();

private:
    Ui::ConsolePanel *ui;
    int m_CS_recordLines = 0;
    int m_CS_maxRecordLines = 500;
    void CS_checkRecordFulls();
};


#endif //ROBOTCOMMANDER_CONSOLEPANEL_H
