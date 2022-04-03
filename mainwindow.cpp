//
// Created by hamlet on 2021/11/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QDockWidget>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTranslator>
#include <QSettings>
#include "command/anycommand.h"
#include "compositor.h"
#include "data/datadisplayer.h"
#include "info/info.h"
#include "panel/commandpanel.h"
#include "panel/consolepanel.h"
#include "panel/senderpanel.h"
#include "robotcommanderconfig.h"
#include "settings.h"
#include "settingsdialog.h"
#include "updater.h"

void MainWindow::setViewForm(MapWidget::ViewForm viewForm) {
    ui->centralwidget->setViewForm(viewForm);
    ui->actionViewNormal->setChecked(false);
    ui->actionViewSuitable->setChecked(false);
    ui->actionViewFilled->setChecked(false);
    switch (viewForm) {
        case MapWidget::NormalView:
            ui->actionViewNormal->setChecked(true);
            break;
        case MapWidget::SuitableView:
            ui->actionViewSuitable->setChecked(true);
            break;
        case MapWidget::FilledView:
            ui->actionViewFilled->setChecked(true);
            break;
    }
}

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow), m_translator(new QTranslator()),
        m_settingsDialog(new SettingsDialog(this)), m_serial(new QSerialPort(this)),
        m_compositor(new Compositor), m_senderPanel(new SenderPanel(this)),
        m_consolePanel(new ConsolePanel(this)), m_updater(nullptr),
        m_commandPanel(new CommandPanel(this)) {
    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/icon");
    ui->setupUi(this);
    updateSettings(m_settingsDialog->settings());
    registerPanel(m_senderPanel, "senderPanel");
    registerPanel(m_consolePanel, "consolePanel");
    registerPanel(m_commandPanel, "commandPanel");
    auto toolBar = new QToolBar(this);
    toolBar->setObjectName("mapToolBar");
    this->addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->addAction(ui->actionZoomIn);
    toolBar->addAction(ui->actionZoomOut);
    toolBar->addAction(ui->actionViewSuitable);
    ui->actionViewSuitable->setChecked(true);
    toolBar->addAction(ui->actionViewFilled);
    QSettings settings;
    restoreGeometry(settings.value("mainWindow_geometry").toByteArray());
    restoreState(settings.value("mainWindow_state").toByteArray());

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigureSerialPort->setEnabled(true);

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered,
            [this]() { m_settingsDialog->showPage(SettingsDialog::Page::General); });
    connect(ui->actionConfigureSerialPort, &QAction::triggered,
            [this]() { m_settingsDialog->showPage(SettingsDialog::Page::SerialPort); });
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionZoomIn, &QAction::triggered, [this]() {
        setViewForm(MapWidget::NormalView);
        ui->centralwidget->zoomIn();
    });
    connect(ui->actionZoomOut, &QAction::triggered, [this]() {
        setViewForm(MapWidget::NormalView);
        ui->centralwidget->zoomOut();
    });
    connect(ui->actionViewNormal, &QAction::triggered, [this](bool checked) {
        setViewForm(checked ? MapWidget::NormalView : MapWidget::SuitableView);
    });
    connect(ui->actionViewSuitable, &QAction::triggered, [this](bool checked) {
        setViewForm(checked ? MapWidget::SuitableView : MapWidget::NormalView);
    });
    connect(ui->actionViewFilled, &QAction::triggered, [this](bool checked) {
        setViewForm(checked ? MapWidget::FilledView : MapWidget::NormalView);
    });
    connect(ui->centralwidget, &MapWidget::updateViewForm, this, &MainWindow::setViewForm);

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::compositorRead);
    connect(m_compositor, &Compositor::needSendCommand, this, &MainWindow::compositorSend);
    connect(m_settingsDialog, &SettingsDialog::needUpdateSettings, this, &MainWindow::updateSettings);
    connect(m_settingsDialog, &SettingsDialog::needCheckForUpdate, this, &MainWindow::startCheckForUpdate);

    auto send_command = [this](const std::shared_ptr<Command>& command) { m_commandPanel->addCommand(command); };
    connect(ui->centralwidget, &MapWidget::sendCommand, send_command);
    connect(m_senderPanel, &SenderPanel::sendCommand, send_command);
    connect(m_commandPanel, &CommandPanel::startSendAll, [this]() { m_commandPanel->sendAll(m_compositor); });

    // last init
    if (m_settingsDialog->settings()->auto_check_update) startCheckForUpdate();
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
    const auto &p = m_settingsDialog->settings()->serial;
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigureSerialPort->setEnabled(false);
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
    ui->actionConfigureSerialPort->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Robot Commander"),
                       tr(R"(<html><head/><body><p><span style=" font-weight:600;">Robot Commander</span></p>
<p>Version %1</p><p>Robot Commander is a tool for controlling and debugging the movements of robots in Robocon.</p>
<p><a href="https://github.com/jadulose/RobotCommander/blob/main/LICENSE/">
<span style=" text-decoration: underline; color:#1d99f3;">GPLv3 License</span>
</a>: This is a <a href="https://www.gnu.org/"><span style=" text-decoration: underline; color:#1d99f3;">free software</span></a>.</p>
<p><a href="https://github.com/jadulose/RobotCommander"><span style=" text-decoration: underline; color:#1d99f3;">
https://github.com/jadulose/RobotCommander</span></a></p></body></html>)").arg(ROBOTCOMMANDER_VERSION));
}

void MainWindow::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::compositorRead() {
    m_readBuffer.append(m_serial->readAll());
    if (!m_readBuffer.endsWith("###")) return;
    m_compositor->decode(m_readBuffer);
    m_consolePanel->appendMessage(m_compositor->getDecodeMessage());
    auto info = m_compositor->getInfo();
    while (info != nullptr) {  // ?
        switch (info->getType()) {
            case Protocol::Position: {
                auto p = dynamic_cast<Position *>(info.get());
                ui->centralwidget->infoCurPosition(
                        QPointF((qreal) p->x / Position::X_RANGE, (qreal) p->y / Position::Y_RANGE));
                break;
            }
            default:
                break;
        }
        m_consolePanel->appendInfo(info.get());
        info = m_compositor->getInfo();
    }
    m_readBuffer.clear();
}

void MainWindow::compositorSend() {
    if (m_serial->isOpen()) {
        m_serial->write(m_compositor->encode());
        showStatusMessage(tr("send: ").append(m_compositor->getEncodeMessage()));
        m_consolePanel->appendMessage(m_compositor->getEncodeMessage());
    }
}

void MainWindow::updateSettings(const Settings *settings) {
    ui->retranslateUi(this);
    for (const auto &r: m_panelRelations) {
        QString panel_name = r.panel->PanelName();
        r.action->setText(panel_name);
        r.panel->retranslateUi();
        r.dock->setWindowTitle(panel_name);
    }
    ui->centralwidget->updateSettings(settings);
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
    m_updater = new Updater(m_settingsDialog->settings()->channel, this);
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
    ui->menuPanel->addAction(action);
    connect(action, &QAction::triggered, [dock](bool checked) {
        if (checked) dock->show(); else dock->hide();
    });
    m_panelRelations.append(PanelRelation{action, panel, dock});
}
