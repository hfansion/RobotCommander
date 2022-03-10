//
// Created by hamlet on 2021/11/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include "compositor.h"
#include "command/anycommand.h"
#include "info/info.h"
#include "info/positioninfo.h"
#include "data/hexdisplayer.h"
#include "settings.h"
#include <QTranslator>
#include <QTime>
#include "robotcommanderconfig.h"

#ifdef QT_DEBUG


#endif

template<typename T>
inline void deleteAllPointers(const QList<T> &anyList) {
    for (const T p: anyList)
        delete p;
}

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow), m_translator(new QTranslator()),
        m_settingsDialog(new SettingsDialog(this)), m_serial(new QSerialPort(this)),
        m_compositor(new Compositor) {
    ui->setupUi(this);
    m_settings = m_settingsDialog->settings();
    updateSettings();

    ui->imageWidget->injectCompositor(m_compositor);
    ui->imageWidget->injectSettings(m_settings);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);
    ui->actionLittle_Sender->setChecked(true);
    ui->actionConsole->setChecked(true);
    ui->label_CS_Alert->setVisible(false);

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::showPreferences);
    connect(ui->actionConfigure, &QAction::triggered, this, &MainWindow::showConfigure);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionLittle_Sender, &QAction::triggered, this, &MainWindow::showLittleSender);
    connect(ui->actionConsole, &QAction::triggered, this, &MainWindow::showConsole);
    connect(ui->menuView, &QMenu::aboutToShow, this, &MainWindow::checkToolWindowVisible);

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::compositorRead);
    connect(m_compositor, &Compositor::needSendCommand, this, &MainWindow::compositorSend);
    connect(m_settingsDialog, &SettingsDialog::needUpdateSettings, ui->imageWidget, &MapWidget::updateSettings);
    connect(m_settingsDialog, &SettingsDialog::needUpdateSettings, this, &MainWindow::updateSettings);

    // LittleSender
    connect(ui->lineEdit_LS, &QLineEdit::textChanged, this, &MainWindow::LS_preview);
    connect(ui->radioButton_LS_hex, &QRadioButton::pressed, this, &MainWindow::LS_preview_hex);
    connect(ui->radioButton_LS_str, &QRadioButton::pressed, this, &MainWindow::LS_preview_str);
    connect(ui->pushButton_LS_Send, &QPushButton::clicked, this, &MainWindow::LS_send);

    // Console
    connect(ui->pushButton_CS_top, &QPushButton::clicked, this, &MainWindow::CS_moveToTop);
    connect(ui->pushButton_CS_bottom, &QPushButton::clicked, this, &MainWindow::CS_moveToBottom);
    connect(ui->pushButton_CS_clear, &QPushButton::clicked, this, &MainWindow::CS_clear);
    connect(ui->comboBox_CS_max, &QComboBox::currentTextChanged, this, &MainWindow::CS_changeMRL);
    connect(ui->checkBox_CS_send, &QCheckBox::toggled, this, &MainWindow::CS_changeIfRecordS);
    connect(ui->checkBox_CS_receive, &QCheckBox::toggled, this, &MainWindow::CS_changeIfRecordR);
    connect(ui->checkBox_CS_position, &QCheckBox::toggled, this, &MainWindow::CS_changeIfRecordPosition);
    connect(ui->checkBox_CS_any, &QCheckBox::toggled, this, &MainWindow::CS_changeIfRecordAny);

    // last init
    LS_preview_hex();
}

MainWindow::~MainWindow() {
    closeSerialPort();
    deleteAllPointers(m_CS_content);
    delete m_LS_tmpCmd;
    delete m_settingsDialog;
    delete m_compositor;
    delete m_translator;
    delete ui;
}

void MainWindow::openSerialPort() {
    const auto &p = m_settings->serial;
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                  .arg(p.name, p.stringBaudRate, p.stringDataBits, p.stringParity, p.stringStopBits,
                                       p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::showStatusMessage(const QString &message) {
    ui->statusbar->showMessage(message);
}

void MainWindow::closeSerialPort() {
    if (m_serial->isOpen())
        m_serial->close();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Robot Commander"),
                       tr("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                          "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                          "p, li { white-space: pre-wrap; }\n"
                          "</style></head><body style=\" font-family:'Noto Sans'; font-shape_size:10pt; font-weight:400; font-style:normal;\">\n"
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-shape_size:14pt; font-weight:600;\">Robot Commander</span></p>\n"
                          "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-shape_size:12pt; font-weight:600;\"><br /></p>\n"
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-shape_size:12pt;\">Version %1.%2.%3%4</span></p>\n"
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-shape_size:12pt;\">Robot Commander is a tool for controlling and debugging the movements of robots in Robocon.</span></p>\n"
                          "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-shape_size:12pt;\"><br /></p>\n"
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"https://github.com/hfansion/RobotCommander/blob/main/LICENSE\"><span style=\" font-shape_size:12pt; text-decoration: underline; color:#1d99f3;\">GPL-3.0 License</span></a><span style=\" font-shape_size:12pt;\">: This is a </span><a href=\"http://www.gnu.org/\"><span style=\" font-shape_size:12pt; text-decoration: underline; color:#1d99f3;\">free software</span></a>.</p>\n"
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"https://github.com/hfansion/RobotCommander\"><span style=\" text-decoration: underline; color:#1d99f3;\">https://github.com/hfansion/RobotCommander</span></a></p></body></html>").arg(
                               ROBOTCOMMANDER_VERSION_MAJOR).arg(ROBOTCOMMANDER_VERSION_MINOR).arg(ROBOTCOMMANDER_VERSION_PATCH).arg(
                               ROBOTCOMMANDER_VERSION_TWEAK ? QString("-beta.%1").arg(ROBOTCOMMANDER_VERSION_TWEAK) : ""));
}

void MainWindow::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::compositorRead() {
    QByteArray data = m_serial->readAll();
    m_compositor->decode(data);
    bool containsPos = false, containsAny = false;  // 暂时操作
    const Info *info = m_compositor->getInfo();
    while (info != nullptr) {
        showStatusMessage(tr("receive: ").append(m_compositor->getDecodeMessage()));
        switch (info->getInfoType()) {
            case ProtocolReceive::Position: {
                auto *p = (PositionInfo *) info;
                containsPos = true;
                ui->label_position->setText(QString("  x: %1   y: %2  ").arg(p->x).arg(p->y));
                ui->imageWidget->infoCurPosition(
                        QPointF((qreal) p->x / Position::X_RANGE, (qreal) p->y / Position::Y_RANGE));
                break;
            }
            case ProtocolReceive::Any:
                containsAny = true;
                break;
        }
        info = m_compositor->getInfo();
    }
    if (m_CS_recordReceive && (m_CS_recordPosition || !containsPos) && (m_CS_recordAny || !containsAny)) {
        auto *message = new QString();
        auto cur_time = QTime::currentTime();
        message->append(QString("[%1:%2:%3] ").arg(cur_time.hour()).arg(cur_time.minute()).arg(cur_time.second()));
        message->append(tr("receive: ")).append(HexDisplayer::toString(data)).append('\n').append(
                m_compositor->getDecodeMessage());
        m_CS_content.enqueue(message);
        ui->textEdit_CS->append(*message);
        CS_checkRecordFulls();
    }
}

void MainWindow::compositorSend() {
    if (m_serial->isOpen()) {
        m_serial->write(m_compositor->encode());
        showStatusMessage(tr("send: ").append(m_compositor->getEncodeMessage()));
        if (m_CS_recordSend) {
            auto *message = new QString();
            auto cur_time = QTime::currentTime();
            message->append(QString("[%1:%2:%3] ").arg(cur_time.hour()).arg(cur_time.minute()).arg(cur_time.second()));
            message->append(tr("send: ")).append(HexDisplayer::toString(m_compositor->encode())).append('\n').append(
                    m_compositor->getEncodeMessage());
            m_CS_content.enqueue(message);
            ui->textEdit_CS->append(*message);
            CS_checkRecordFulls();
        }
    }
}

void MainWindow::showPreferences() {
    m_settingsDialog->setPage(0);
    m_settingsDialog->show();
}

void MainWindow::showConfigure() {
    m_settingsDialog->setPage(1);
    m_settingsDialog->show();
}

void MainWindow::updateSettings() {
    switch (m_settings->language) {
        case Settings::Chinese: {
            bool rs = m_translator->load(":/translation/RobotCommander_zh.qm");
            if (rs) qApp->installTranslator(m_translator);
            else showStatusMessage(tr("Cannot open translation file."));
            break;
        }
        case Settings::English: {
            bool rs = m_translator->load(":/translation/RobotCommander_en.qm");
            if (rs) qApp->installTranslator(m_translator);
            else showStatusMessage(tr("Cannot open translation file."));
            break;
        }
    }
    ui->retranslateUi(this);
}

void MainWindow::showLittleSender(bool checked) {
    if (checked)
        ui->dockWidget_LittleSender->show();
    else
        ui->dockWidget_LittleSender->hide();
}

void MainWindow::showConsole(bool checked) {
    if (checked)
        ui->dockWidget_Console->show();
    else
        ui->dockWidget_Console->hide();
}

void MainWindow::checkToolWindowVisible() {
    ui->actionLittle_Sender->setChecked(ui->dockWidget_LittleSender->isVisible());
    ui->actionConsole->setChecked(ui->dockWidget_Console->isVisible());
}

inline char hexToDec(char i) {
    return (char) (('0' <= i && i <= '9') ? (i - '0') : (i - 'A' + 10));
}

void MainWindow::LS_preview(const QString &data) {
    delete m_LS_tmpCmd;
    if (m_LS_isHexThanStr) {
        QByteArray code{};
        bool isOdd = data.length() % 2 == 1;
        if (isOdd)
            code.append(hexToDec(data.at(0).toLatin1()));
        for (int i = isOdd ? 1 : 0; i < data.length(); i += 2)
            code.append((char) ((hexToDec(data.at(i).toLatin1()) << 4) + hexToDec(data.at(i + 1).toLatin1())));
        m_LS_tmpCmd = new AnyCommand(code);
    } else {
        m_LS_tmpCmd = new AnyCommand(data.toLocal8Bit());
    }
    ui->label_LS_result->setText(HexDisplayer::toString(Compositor::previewEncode(m_LS_tmpCmd)));
}

void MainWindow::LS_send() {
    m_compositor->addCommand(((AnyCommand *) m_LS_tmpCmd)->copy());
}

void MainWindow::LS_preview_hex() {
    m_LS_isHexThanStr = true;
    LS_preview(ui->lineEdit_LS->text());
}

void MainWindow::LS_preview_str() {
    m_LS_isHexThanStr = false;
    LS_preview(ui->lineEdit_LS->text());
}

void MainWindow::CS_moveToTop() {
    ui->textEdit_CS->moveCursor(QTextCursor::Start);
}

void MainWindow::CS_moveToBottom() {
    ui->textEdit_CS->moveCursor(QTextCursor::End);
}

void MainWindow::CS_clear() {
    ui->label_CS_Alert->setVisible(false);
    ui->textEdit_CS->clear();
    deleteAllPointers(m_CS_content);
    m_CS_content.clear();
}

void MainWindow::CS_changeMRL(const QString &text) {
    m_CS_maxRecordLines = text.toInt();
    CS_checkRecordFulls();
}

void MainWindow::CS_checkRecordFulls() {  // 非常差的操作，不过暂时想不到别的方法
    if (m_CS_content.length() > m_CS_maxRecordLines) {
        ui->label_CS_Alert->setVisible(true);
        if (m_CS_content.length() == 10000) {  // 条数大于10000自动清除
            CS_clear();
        }
    } else {
        ui->label_CS_Alert->setVisible(false);
    }
}

void MainWindow::CS_changeIfRecordS(bool checked) {
    m_CS_recordSend = checked;
}

void MainWindow::CS_changeIfRecordR(bool checked) {
    m_CS_recordReceive = checked;
}

void MainWindow::CS_changeIfRecordPosition(bool checked) {
    m_CS_recordPosition = checked;
}

void MainWindow::CS_changeIfRecordAny(bool checked) {
    m_CS_recordAny = checked;
}
