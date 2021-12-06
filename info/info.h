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

class Info {
public:
    static constexpr int DATA_LENGTH = 4;
    static int X_RANGE, Y_RANGE;
    static void SET_RANGE(int x_range, int y_range);

    static Info decode(const QByteArray &data);

    explicit Info(int x, int y);
    ~Info() = default;
    [[nodiscard]] QString toString() const;

    const int x;
    const int y;
};


#endif //ROBOTCOMMANDER_INFO_H
