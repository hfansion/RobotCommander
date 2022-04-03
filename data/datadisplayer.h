//
// Created by hamlet on 2021/12/7.
//

#ifndef ROBOTCOMMANDER_DATADISPLAYER_H
#define ROBOTCOMMANDER_DATADISPLAYER_H

#include <QByteArray>
#include <QString>

class DataDisplayer {
public:
    static QString toASCII(const QByteArray &data);
    static QString toHex(const QByteArray &data);
};


#endif //ROBOTCOMMANDER_DATADISPLAYER_H
