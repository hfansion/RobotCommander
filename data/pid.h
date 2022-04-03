//
// Created by hamlet on 2022/4/3.
//

#ifndef ROBOTCOMMANDER_PID_H
#define ROBOTCOMMANDER_PID_H

#include <qglobal.h>
class QString;

class PID {
public:
    static constexpr qsizetype DATA_LENGTH = 12;
    explicit PID(float p, float i, float d);
    ~PID() = default;
    [[nodiscard]] QString toString() const;

    const float p;
    const float i;
    const float d;
};


#endif //ROBOTCOMMANDER_PID_H
