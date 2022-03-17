//
// Created by hamlet on 2021/12/7.
//

#ifndef ROBOTCOMMANDER_POSITIONINFO_H
#define ROBOTCOMMANDER_POSITIONINFO_H

#include "../data/position.h"
#include "info.h"

class PositionInfo : public Position, public Info {
public:
    static std::unique_ptr<PositionInfo> decode(const QByteArray &data);

    using Position::Position;
    [[nodiscard]] ProtocolReceive getInfoType() const override;
    [[nodiscard]] QString toString() const override;
};


#endif //ROBOTCOMMANDER_POSITIONINFO_H
