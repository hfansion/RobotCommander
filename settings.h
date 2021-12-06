//
// Created by hamlet on 2021/12/6.
//

#ifndef ROBOTCOMMANDER_SETTINGS_H
#define ROBOTCOMMANDER_SETTINGS_H

// 找机会看一下QSettings怎么用的

#include <QSerialPort>
#include <QColor>

struct Settings {
    enum Shape {
        Square = 0, Triangle = 1, Circular = 2
    };

    enum Language {
        Chinese = 0, English = 0
    };

    // Appearance
    QString mapPic;

    bool RCP_picThanShape;
    QString RCP_robotPic;
    int RCP_picSize;
    Shape RCP_shape;
    int RCP_shapeSize;
    QColor RCP_color;

    bool RTP_picThanShape;
    QString RTP_robotPic;
    int RTP_picSize;
    Shape RTP_shape;
    int RTP_shapeSize;
    QColor RTP_color;

    Language language;

    // Serial
    QString name;
    qint32 baudRate;
    QString stringBaudRate;
    QSerialPort::DataBits dataBits;
    QString stringDataBits;
    QSerialPort::Parity parity;
    QString stringParity;
    QSerialPort::StopBits stopBits;
    QString stringStopBits;
    QSerialPort::FlowControl flowControl;
    QString stringFlowControl;
};

#endif //ROBOTCOMMANDER_SETTINGS_H
