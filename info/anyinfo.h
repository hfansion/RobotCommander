//
// Created by hamlet on 2021/12/7.
//

#ifndef ROBOTCOMMANDER_ANYINFO_H
#define ROBOTCOMMANDER_ANYINFO_H

#include "info.h"

class AnyInfo : public Info {
public:
    explicit AnyInfo(QByteArray data);

    [[nodiscard]] ProtocolReceive getInfoType() const override;
    [[nodiscard]] QString toString() const override;

    const QByteArray DATA;
};


#endif //ROBOTCOMMANDER_ANYINFO_H
