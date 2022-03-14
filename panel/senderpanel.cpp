//
// Created by hamlet on 2022/3/14.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SenderPanel.h" resolved

#include "senderpanel.h"
#include "ui_senderpanel.h"
#include "command/anycommand.h"
#include "compositor.h"
#include "data/hexdisplayer.h"

SenderPanel::SenderPanel(QWidget *parent) :
        PanelBase(parent), ui(new Ui::SenderPanel) {
    ui->setupUi(this);
    connect(ui->lineEdit_LS, &QLineEdit::textChanged, this, &SenderPanel::preview);
    auto f_preview = [this]() { preview(ui->lineEdit_LS->text()); };
    connect(ui->radioButton_LS_hex, &QRadioButton::pressed, f_preview);
    connect(ui->radioButton_LS_str, &QRadioButton::pressed, f_preview);
    connect(ui->pushButton_LS_Send, &QPushButton::clicked, [this]() {
        emit sendCommand(m_LS_tmpCmd->copy());
    });
    f_preview();
}

SenderPanel::~SenderPanel() {
    delete m_LS_tmpCmd;
    delete ui;
}

inline char hexToDec(char i) {
    return (char) (('0' <= i && i <= '9') ? (i - '0') : (i - 'A' + 10));
}

void SenderPanel::preview(const QString &text) {
    delete m_LS_tmpCmd;
    if (ui->radioButton_LS_hex->isChecked()) {
        QByteArray code{};
        bool isOdd = text.length() % 2 == 1;
        if (isOdd)
            code.append(hexToDec(text.at(0).toLatin1()));
        for (int i = isOdd ? 1 : 0; i < text.length(); i += 2)
            code.append((char) ((hexToDec(text.at(i).toLatin1()) << 4) + hexToDec(text.at(i + 1).toLatin1())));
        m_LS_tmpCmd = new AnyCommand(code);
    } else {
        m_LS_tmpCmd = new AnyCommand(text.toLocal8Bit());
    }
    ui->label_LS_result->setText(HexDisplayer::toString(Compositor::previewEncode(m_LS_tmpCmd)));
}

void SenderPanel::retranslateUi() {
    ui->retranslateUi(this);
}
