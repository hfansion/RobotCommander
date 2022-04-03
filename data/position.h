//
// Created by hamlet on 2021/12/7.
//

#ifndef ROBOTCOMMANDER_POSITION_H
#define ROBOTCOMMANDER_POSITION_H

#include <qglobal.h>

class QString;

class Position {
public:
    static int X_RANGE, Y_RANGE;
    [[maybe_unused]] static void SET_RANGE(int x_range, int y_range);
    static constexpr qsizetype DATA_LENGTH = 8;

    explicit Position(int x, int y, float r);
    ~Position() = default;
    [[nodiscard]] QString toString() const;

    const int x;
    const int y;
    const float r;
};


#endif //ROBOTCOMMANDER_POSITION_H
