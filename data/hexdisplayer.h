//
// Created by hamlet on 2021/12/7.
//

#ifndef ROBOTCOMMANDER_HEXDISPLAYER_H
#define ROBOTCOMMANDER_HEXDISPLAYER_H

#include <QByteArray>
#include <QString>

class HexDisplayer {
public:
    static QString toString(const QByteArray &data);
};


#endif //ROBOTCOMMANDER_HEXDISPLAYER_H
