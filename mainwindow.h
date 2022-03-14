//
// Created by hamlet on 2021/11/7.
//

#ifndef ROBOTCOMMANDER_MAINWINDOW_H
#define ROBOTCOMMANDER_MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QQueue>
#include "updater.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Command;
class Compositor;
class ConsolePanel;
class PanelBase;
class QTranslator;
class SenderPanel;
class Settings;
class SettingsDialog;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void openSerialPort();
    void closeSerialPort();
    void about();

    void compositorSend();
    void compositorRead();
    void handleError(QSerialPort::SerialPortError error);

    void updateSettings();

    void startCheckForUpdate();
    void showUpdateDialog(Updater::Result result);

private:
    Ui::MainWindow *ui;
    SettingsDialog *m_settingsDialog;
    const Settings *m_settings;
    QTranslator *m_translator;
    Updater *m_updater;

    QSerialPort *m_serial;
    Compositor *m_compositor;

    void showStatusMessage(const QString &message);

    struct PanelRelation {
        QAction *action;
        PanelBase* panel;
        QDockWidget *dock;
    };
    QVector<PanelRelation> m_panelRelations;
    void registerPanel(PanelBase *panel, const QString &objectName);
    SenderPanel *m_senderPanel;
    ConsolePanel *m_consolePanel;
};

/** 自动更新：
 * 1. 如何写入配置文件
 * 2. 安装包
 * 3. 从网上下载内容(json)并解析（发现更新）
 * 4. 从网上下载文件到tmp（下载安装包）
 * 5. 以管理员权限运行安装包
 * 端口转发工具及Virtual Robot开发
 **/

#endif //ROBOTCOMMANDER_MAINWINDOW_H
