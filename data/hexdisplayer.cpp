//
// Created by hamlet on 2021/12/7.
//

#include "hexdisplayer.h"

inline char decToHex(char i) {
    return (char) (i < 10 ? ('0' + i) : ('A' + (i - 10)));
}

QString HexDisplayer::toString(const QByteArray &data) {
    QString output;
    for (const auto &c: data) {
        if (c == '#') {
            output.append(c);
        } else {
            char d1 = (char) ((char8_t) c >> 4), d2 = (char) ((char8_t) c - (d1 << 4));
            output.append(decToHex(d1)).append(decToHex(d2));
        }
    }
    return std::move(output);
}
