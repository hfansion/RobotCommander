//
// Created by hamlet on 2022/3/14.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ConsolePanel.h" resolved

#include "consolepanel.h"
#include "ui_consolepanel.h"
#include "data/datadisplayer.h"
#include "info/info.h"
#include "command/positioncommand.h"
#include "command/anycommand.h"
#include "command/pid1command.h"
#include "command/pid2command.h"
#include "command/setgpscommand.h"

QString check_mark = "✓";

ConsolePanel::ConsolePanel(QWidget *parent) :
        PanelBase(parent), ui(new Ui::ConsolePanel) {
    ui->setupUi(this);
    ui->label_CS_Alert->setVisible(false);

    auto add_item = [this](const QString &name, Protocol p) {
        auto item = new QListWidgetItem(check_mark + name);
        item->setToolTip(QString::number(static_cast<int>(p)));
        ui->listWidget_receive->addItem(item);
        m_visibleMap[p] = true;
    };

    add_item(PositionCommand::NAME, Protocol::Position);
    add_item(AnyCommand::NAME, Protocol::Any);
    add_item(PID1Command::NAME, Protocol::PID1);
    add_item(PID2Command::NAME, Protocol::PID2);
    add_item(SetGPSCommand::NAME, Protocol::SET_GPS);

    connect(ui->listWidget_receive, &QListWidget::itemDoubleClicked, [this](QListWidgetItem *item) {
        auto proto = static_cast<Protocol>(item->toolTip().toInt());
        auto txt = item->text();
        if (txt.startsWith(check_mark)) {
            item->setText(txt.right(txt.size() - check_mark.size()));
            m_visibleMap[proto] = false;
        } else {
            item->setText(check_mark + txt);
            m_visibleMap[proto] = true;
        }
    });

    connect(ui->pushButton_CS_top, &QPushButton::clicked,
            [this]() { ui->textEdit_CS->moveCursor(QTextCursor::Start); });
    connect(ui->pushButton_CS_bottom, &QPushButton::clicked,
            [this]() { ui->textEdit_CS->moveCursor(QTextCursor::End); });
    connect(ui->pushButton_CS_clear, &QPushButton::clicked, this, &ConsolePanel::CS_clear);
    connect(ui->comboBox_CS_max, &QComboBox::currentTextChanged, [this](const QString &text) {
        m_CS_maxRecordLines = text.toInt();
        CS_checkRecordFulls();
    });
}

ConsolePanel::~ConsolePanel() {
    delete ui;
}

void ConsolePanel::CS_clear() {
    m_CS_recordLines = 0;
    ui->label_CS_Alert->setVisible(false);
    ui->textEdit_CS->clear();
}

void ConsolePanel::CS_checkRecordFulls() {  // 非常差的操作，不过暂时想不到别的方法
    if (m_CS_recordLines == m_CS_maxRecordLines) {
        ui->label_CS_Alert->setVisible(true);
    } else if (m_CS_recordLines > 10000) {  // 条数大于10000自动清除
        CS_clear();
    }
}

void ConsolePanel::appendMessage(const QString &content) {
    ui->textEdit_CS->append(content);
    ++m_CS_recordLines;
    CS_checkRecordFulls();
}

void ConsolePanel::appendInfo(const Info *info) {
    if (!m_visibleMap[info->getType()]) return;
    m_tmpInfo.append(tr(" [info] ").append(info->toString()).append('\n'));
    ++m_infoNum;
}

void ConsolePanel::retranslateUi() {
    ui->retranslateUi(this);
}

void ConsolePanel::commitInfo() {
    if (m_infoNum != 0)
        ui->textEdit_CS->append(m_tmpInfo);
    m_tmpInfo.clear();
    m_infoNum = 0;
}

void ConsolePanel::beginInfo(const QString &content) {
    m_tmpInfo.append(content);
}
