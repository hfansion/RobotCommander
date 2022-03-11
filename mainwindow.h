//
// Created by hamlet on 2021/11/7.
//

#ifndef ROBOTCOMMANDER_MAINWINDOW_H
#define ROBOTCOMMANDER_MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QQueue>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Command;
class Compositor;
class QTranslator;
class Settings;
class SettingsDialog;
class Updater;

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

    void showPreferences();
    void showConfigure();
    void showLittleSender(bool checked);
    void showConsole(bool checked);
    void checkToolWindowVisible();

    void updateSettings();

    void showUpdateDialog(bool needUpdate);

    // LittleSender
    void LS_preview(const QString &);
    void LS_preview_hex();
    void LS_preview_str();
    void LS_send();

    // Console
    void CS_moveToTop();
    void CS_moveToBottom();
    void CS_clear();
    void CS_changeMRL(const QString &);
    void CS_changeIfRecordS(bool);
    void CS_changeIfRecordR(bool);
    void CS_changeIfRecordPosition(bool);
    void CS_changeIfRecordAny(bool);

private:
    Ui::MainWindow *ui;
    SettingsDialog *m_settingsDialog;
    const Settings *m_settings;
    QTranslator *m_translator;
    Updater *m_updater;

    QSerialPort *m_serial;
    Compositor *m_compositor;

    void showStatusMessage(const QString &message);

    // LittleSender
    Command *m_LS_tmpCmd = nullptr;
    bool m_LS_isHexThanStr = true;
    // Console
    QQueue<QString *> m_CS_content;
    int m_CS_maxRecordLines = 500;
    bool m_CS_recordSend = true;
    bool m_CS_recordReceive = true;
    bool m_CS_recordPosition = true;
    bool m_CS_recordAny = true;
    void CS_checkRecordFulls();
};

/** 未来任务：
 * 继续学习Qt基础数据类型，找到最合适的（比如QQueue真的好吗）
 * 优化、重构整体代码，架构，变量名字
 * 写文档和注释
 * 少量设置外观上的功能
 * 其他：
 * 1. 如何写入配置文件
 * 2. 安装包
 * 3. 从网上下载内容(json)并解析（发现更新）
 * 4. 从网上下载文件到tmp（下载安装包）
 * 5. 以管理员权限运行安装包
 * 端口转发工具及Virtual Robot开发
 **/

#endif //ROBOTCOMMANDER_MAINWINDOW_H
