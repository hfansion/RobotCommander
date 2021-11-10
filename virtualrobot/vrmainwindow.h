//
// Created by hamlet on 2021/11/10.
//

#ifndef ROBOTCOMMANDER_VRMAINWINDOW_H
#define ROBOTCOMMANDER_VRMAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class VRMainWindow; }
QT_END_NAMESPACE

class SettingsDialog;

class VRMainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit VRMainWindow(QWidget *parent = nullptr);

    ~VRMainWindow() override;


public slots:

    void openSerialPort();

    void closeSerialPort();

    void about();

    void sendPoint();

    void readPoint();

    void handleError(QSerialPort::SerialPortError error);

private:
    Ui::VRMainWindow *ui;
    SettingsDialog *m_settings;
    QSerialPort *m_serial;
    QTimer *m_timer;

    void showStatusMessage(const QString &message);
};


#endif //ROBOTCOMMANDER_VRMAINWINDOW_H
