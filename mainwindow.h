//
// Created by hamlet on 2021/11/7.
//

#ifndef ROBOTCOMMANDER_MAINWINDOW_H
#define ROBOTCOMMANDER_MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class SettingsDialog;
class Settings;
class Compositor;
class Command;
class QTranslator;

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

    // LittleSender
    void LS_preview(const QString &);
    void LS_preview_hex();
    void LS_preview_str();
    void LS_send();

private:
    Ui::MainWindow *ui;
    SettingsDialog *m_settingsDialog;
    const Settings *m_settings;
    QTranslator *m_translator;

    QSerialPort *m_serial;
    Compositor *m_compositor;

    void showStatusMessage(const QString &message);

    // LittleSender
    Command *m_tmpCmd = nullptr;
    bool m_isHexThanStr = true;
};


#endif //ROBOTCOMMANDER_MAINWINDOW_H
