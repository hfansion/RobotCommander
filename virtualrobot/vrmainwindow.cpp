//
// Created by hamlet on 2021/11/10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_VRMainWindow.h" resolved

#include "vrmainwindow.h"
#include "ui_vrmainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QTimer>
#include "util.h"

struct Point curP{50, 50}, tarP{50, 50};

VRMainWindow::VRMainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::VRMainWindow), m_timer(new QTimer(this)),
        m_settings(new SettingsDialog), m_serial(new QSerialPort(this)) {
    ui->setupUi(this);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    connect(ui->actionConnect, &QAction::triggered, this, &VRMainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &VRMainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &VRMainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(ui->actionAbout, &QAction::triggered, this, &VRMainWindow::about);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);

    connect(m_serial, &QSerialPort::errorOccurred, this, &VRMainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &VRMainWindow::readPoint);
    connect(m_timer, &QTimer::timeout, this, &VRMainWindow::sendPoint);

    m_timer->start(100);
}

VRMainWindow::~VRMainWindow() {
    delete m_settings;
    delete ui;
}

void VRMainWindow::openSerialPort() {
    const SettingsDialog::Settings p = m_settings->settings();
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

void VRMainWindow::closeSerialPort() {
    if (m_serial->isOpen())
        m_serial->close();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}

void VRMainWindow::showStatusMessage(const QString &message) {
    ui->statusbar->showMessage(message);
}

void VRMainWindow::about() {
    QMessageBox::about(this, tr("About Virtual Robot"),
                       tr(""));
}

void VRMainWindow::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

void VRMainWindow::sendPoint() {
    if (m_serial->isOpen()) {
        moveForward(&curP, &tarP);
        QString command = QString::fromLocal8Bit(point2Str(&curP));
        showStatusMessage(QString("send: ").append(command));
        ui->label_current->setText(command);
        m_serial->write(command.toStdString().c_str());
        m_timer->start(100);
    }
}

void VRMainWindow::readPoint() {
    QString data = m_serial->readAll();
    showStatusMessage(QString("receive: ").append(data));
    ui->label_target->setText(data);
    tarP = str2Point(data.toStdString().c_str());
}

