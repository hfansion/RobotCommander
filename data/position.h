//
// Created by hamlet on 2021/12/7.
//

#ifndef ROBOTCOMMANDER_POSITION_H
#define ROBOTCOMMANDER_POSITION_H

class QString;

class Position {
public:
    static int X_RANGE, Y_RANGE;
    static void SET_RANGE(int x_range, int y_range);

    explicit Position(int x, int y);
    ~Position() = default;
    [[nodiscard]] QString toString() const;

    const int x;
    const int y;
};


#endif //ROBOTCOMMANDER_POSITION_H