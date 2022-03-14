//
// Created by hamlet on 2021/11/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QDockWidget>
#include <QMessageBox>
#include <QTranslator>
#include <QSettings>
#include "command/anycommand.h"
#include "compositor.h"
#include "data/hexdisplayer.h"
#include "info/info.h"
#include "info/positioninfo.h"
#include "panel/consolepanel.h"
#include "panel/senderpanel.h"
#include "robotcommanderconfig.h"
#include "settings.h"
#include "settingsdialog.h"
#include "updater.h"

template<typename T>
inline void deleteAllPointers(const QList<T> &anyList) {
    for (const T &p: anyList)
        delete p;
}

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow), m_translator(new QTranslator()),
        m_settingsDialog(new SettingsDialog(this)), m_serial(new QSerialPort(this)),
        m_compositor(new Compositor), m_senderPanel(new SenderPanel(this)),
        m_consolePanel(new ConsolePanel(this)) {
    ui->setupUi(this);
    m_settings = m_settingsDialog->settings();
    updateSettings();
    registerPanel(m_senderPanel, "senderPanel");
    registerPanel(m_consolePanel, "consolePanel");
    QSettings settings;
    restoreGeometry(settings.value("mainWindow_geometry").toByteArray());
    restoreState(settings.value("mainWindow_state").toByteArray());

    ui->imageWidget->injectCompositor(m_compositor);
    ui->imageWidget->injectSettings(m_settings);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionPreferences, &QAction::triggered,
            [this]() { m_settingsDialog->showPage(SettingsDialog::Page::General); });
    connect(ui->actionConfigure, &QAction::triggered,
            [this]() { m_settingsDialog->showPage(SettingsDialog::Page::SerialPort); });
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::compositorRead);
    connect(m_compositor, &Compositor::needSendCommand, this, &MainWindow::compositorSend);
    connect(m_settingsDialog, &SettingsDialog::needUpdateSettings, ui->imageWidget, &MapWidget::updateSettings);
    connect(m_settingsDialog, &SettingsDialog::needUpdateSettings, this, &MainWindow::updateSettings);
    connect(m_settingsDialog, &SettingsDialog::needCheckForUpdate, this, &MainWindow::startCheckForUpdate);

    // Sender Panel
    connect(m_senderPanel, &SenderPanel::sendCommand, [this](Command *command) {
        m_compositor->addCommand(command);
    });

    // last init
    if (m_settings->auto_check_update) startCheckForUpdate();
}

MainWindow::~MainWindow() {
    QSettings settings;
    settings.setValue("mainWindow_geometry", saveGeometry());
    settings.setValue("mainWindow_state", saveState());
    closeSerialPort();
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
        m_compositor->isConnected = true;
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
    m_compositor->isConnected = false;
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Robot Commander"), tr(R"(
<html><head/><body><p><span style=" font-weight:600;">Robot Commander</span></p>
<p>Version %1</p><p>Robot Commander is a tool for controlling and debugging the movements of robots in Robocon.</p>
<p><a href="https://github.com/hfansion/RobotCommander/blob/main/LICENSE/">
<span style=" text-decoration: underline; color:#1d99f3;">GPL-3.0 License</span>
</a>: This is a <a href="http://www.gnu.org/"><span style=" text-decoration: underline; color:#1d99f3;">free software</span></a>.</p>
<p><a href="https://github.com/hfansion/RobotCommander"><span style=" text-decoration: underline; color:#1d99f3;">
https://github.com/hfansion/RobotCommander</span></a></p></body></html>)").arg(ROBOTCOMMANDER_VERSION));
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
        showStatusMessage(tr("receive: ").append(m_compositor->getDecodeMessage()));
        switch (info->getInfoType()) {
            case ProtocolReceive::Position: {
                auto *p = (PositionInfo *) info;
                ui->label_position->setText(QString("  x: %1   y: %2  ").arg(p->x).arg(p->y));
                ui->imageWidget->infoCurPosition(
                        QPointF((qreal) p->x / Position::X_RANGE, (qreal) p->y / Position::Y_RANGE));
                break;
            }
            case ProtocolReceive::Any:
                break;
        }
        info = m_compositor->getInfo();
    }
    m_consolePanel->appendMessage(data, m_compositor->getDecodeMessage());
}

void MainWindow::compositorSend() {
    if (m_serial->isOpen()) {
        m_serial->write(m_compositor->encode());
        showStatusMessage(tr("send: ").append(m_compositor->getEncodeMessage()));
        m_consolePanel->appendMessage(m_compositor->encode(), m_compositor->getEncodeMessage());
    }
}

void MainWindow::updateSettings() {
    ui->retranslateUi(this);
    for (const auto &r: m_panelRelations) {
        QString panel_name = r.panel->PanelName();
        r.action->setText(panel_name);
        r.panel->retranslateUi();
        r.dock->setWindowTitle(panel_name);
    }
}

void MainWindow::showUpdateDialog(Updater::Result result) {
    if (result == Updater::isNotLatest) {
        QMessageBox msgBox;
        msgBox.setText(tr("发现新版本v%1，是否要更新？").arg(m_updater->getVersion()));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        if (msgBox.exec() == QMessageBox::Yes) {
            QDesktopServices::openUrl(m_updater->getLink());
        }
    }
    m_settingsDialog->checkUpdateResult(result);
    m_updater->deleteLater();
}

void MainWindow::startCheckForUpdate() {
    m_updater = new Updater(m_settings->channel, this);
    connect(m_updater, &Updater::checkFinished, this, &MainWindow::showUpdateDialog);
    m_updater->check();
}

void MainWindow::registerPanel(PanelBase *panel, const QString &objectName) {
    QString panel_name = panel->PanelName();
    auto dock = new QDockWidget(this);
    dock->setWidget(panel);
    dock->setObjectName(objectName);
    dock->setWindowTitle(panel_name);
    this->addDockWidget(Qt::LeftDockWidgetArea, dock);
    auto action = dock->toggleViewAction();
    action->setText(panel_name);
    ui->menuView->addAction(action);
    connect(action, &QAction::triggered, [dock](bool checked) {
        if (checked) dock->show(); else dock->hide();
    });
    m_panelRelations.append(PanelRelation{action, panel, dock});
}
