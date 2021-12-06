//
// Created by hamlet on 2021/11/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include "compositor.h"
#include "info/info.h"
#include "settings.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow),
        m_settingsDialog(new SettingsDialog), m_serial(new QSerialPort(this)),
        m_compositor(new Compositor) {
    ui->setupUi(this);
    m_settings = m_settingsDialog->settings();

    ui->imageWidget->injectCompositor(m_compositor);
    ui->imageWidget->injectSettings(m_settings);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::showPreferences);
    connect(ui->actionConfigure, &QAction::triggered, this, &MainWindow::showConfigure);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::compositorRead);
    connect(m_compositor, &Compositor::needSendCommand, this, &MainWindow::compositorSend);
    connect(m_settingsDialog, &SettingsDialog::needUpdateSettings, ui->imageWidget, &MapWidget::updateSettings);
}

MainWindow::~MainWindow() {
    delete m_settingsDialog;
    delete m_compositor;
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
                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"https://github.com/hfansion/RobotCommander/blob/main/LICENSE\"><span style=\" font-size:12pt; text-decoration: underline; color:#1d99f3;\">GPL-3.0 License</span></a><span style=\" font-size:12pt;\">: This is a free software.</span></p>\n"
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
    if (info == nullptr) return;
    showStatusMessage(QString(tr("receive: ")).append(m_compositor->getDecodeMessage()));
    ui->imageWidget->infoCurPosition(QPointF((qreal) info->x / Info::X_RANGE, (qreal) info->y / Info::Y_RANGE));
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
