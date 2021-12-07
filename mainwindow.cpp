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

#ifdef QT_DEBUG

#include <QDebug>

#endif

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
}

MainWindow::~MainWindow() {
    delete m_tmpCmd;
    delete m_settingsDialog;
    delete m_compositor;
    delete m_translator;
    delete ui;
}

void MainWindow::openSerialPort() {
    const auto &p = *m_settings;
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
                          "</style></head><body style=\" font-family:'Noto Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt; font-weight:600;\">Robot Commander</span></p>\n"
                          "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt; font-weight:600;\"><br /></p>\n"
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">Version 0.0.1</span></p>\n"
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">Robot Commander is a tool for controlling and debugging the movements of robots in Robocon.</span></p>\n"
                          "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n"
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"https://github.com/hfansion/RobotCommander/blob/main/LICENSE\"><span style=\" font-size:12pt; text-decoration: underline; color:#1d99f3;\">GPL-3.0 License</span></a><span style=\" font-size:12pt;\">: This is a </span><a href=\"http://www.gnu.org/\"><span style=\" font-size:12pt; text-decoration: underline; color:#1d99f3;\">free software</span></a>.</p>\n"
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"https://github.com/hfansion/RobotCommander\"><span style=\" text-decoration: underline; color:#1d99f3;\">https://github.com/hfansion/RobotCommander</span></a></p></body></html>"));
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
    const Info *info = m_compositor->getInfo();
    while (info != nullptr) {
        showStatusMessage(QString(tr("receive: ")).append(info->toString()));
        switch (info->getInfoType()) {
            case ProtocolReceive::Position: {
                auto *p = (PositionInfo *) info;
                ui->imageWidget->infoCurPosition(
                        QPointF((qreal) p->x / Position::X_RANGE, (qreal) p->y / Position::Y_RANGE));
                break;
            }
            case ProtocolReceive::Any:
                break;
        }
        info = m_compositor->getInfo();
    }
}

void MainWindow::compositorSend() {
    if (m_serial->isOpen()) {
        m_serial->write(m_compositor->encode());
    }
    m_compositor->encode();
    showStatusMessage(QString(tr("send: ")).append(m_compositor->getEncodeMessage()));
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
    delete m_tmpCmd;
    if (m_isHexThanStr) {
        QByteArray code{};
        bool isOdd = data.length() % 2 == 1;
        if (isOdd)
            code.append(hexToDec(data.at(0).toLatin1()));
        for (int i = isOdd ? 1 : 0; i < data.length(); i += 2)
            code.append((char) ((hexToDec(data.at(i).toLatin1()) << 4) + hexToDec(data.at(i + 1).toLatin1())));
        m_tmpCmd = new AnyCommand(code);
    } else {
        m_tmpCmd = new AnyCommand(data.toLocal8Bit());
    }
    ui->label_LS_result->setText(HexDisplayer::toString(Compositor::previewEncode(m_tmpCmd)));
}

void MainWindow::LS_send() {
    m_compositor->addCommand(m_tmpCmd);
    m_tmpCmd = nullptr;
}

void MainWindow::LS_preview_hex() {
    m_isHexThanStr = true;
    LS_preview(ui->lineEdit_LS->text());
}

void MainWindow::LS_preview_str() {
    m_isHexThanStr = false;
    LS_preview(ui->lineEdit_LS->text());
}
