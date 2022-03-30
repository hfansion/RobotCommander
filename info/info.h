//
// Created by hamlet on 2021/12/5.
//
// Info类，用于表示从单片机传回电脑的数据
// 目前仅包含机器人当前位置的数据
// 数据为4个字节，
// 前2个字节表示x座标，可能的值1~65536
// 后2个字节表示y座标，可能的值1~65536
// 数字都采用大端法表示
//
//

#ifndef ROBOTCOMMANDER_INFO_H
#define ROBOTCOMMANDER_INFO_H

#include <QString>
#include "../data/position.h"
#include "../protocol.h"

class Info {
public:
    virtual ~Info() = default;
    [[nodiscard]] virtual Protocol getType() const = 0;
    [[nodiscard]] virtual QString toString() const = 0;
};

#endif //ROBOTCOMMANDER_INFO_H
