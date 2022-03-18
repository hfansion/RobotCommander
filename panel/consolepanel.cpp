//
// Created by hamlet on 2022/3/14.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ConsolePanel.h" resolved

#include "consolepanel.h"
#include "ui_consolepanel.h"
#include "data/hexdisplayer.h"
#include "info/info.h"

ConsolePanel::ConsolePanel(QWidget *parent) :
        PanelBase(parent), ui(new Ui::ConsolePanel) {
    ui->setupUi(this);
    ui->label_CS_Alert->setVisible(false);

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
//    switch (info->getType()) {
//    }
    ui->textEdit_CS->append(tr(" [info] ").append(info->toString()).append('\n'));
}

void ConsolePanel::retranslateUi() {
    ui->retranslateUi(this);
}
