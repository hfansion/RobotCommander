//
// Created by hamlet on 2021/12/7.
//

#include "anyinfo.h"
#include "../data/datadisplayer.h"

QString AnyInfo::toString() const {
    QString text = QString("Any(%1)").arg(DataDisplayer::toASCII(DATA));
    return std::move(text);
}

AnyInfo::AnyInfo(QByteArray data)
        : DATA(std::move(data)) {
}
