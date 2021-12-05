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
class Compositor;

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
    void sendCommand(const QPointF &ratio);
    void compositorRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    Ui::MainWindow *ui;
    SettingsDialog *m_settings;
    QSerialPort *m_serial;
    Compositor *m_compositor;

    void showStatusMessage(const QString &message);
};


#endif //ROBOTCOMMANDER_MAINWINDOW_H
