//
// Created by hamlet on 2021/12/7.
//

#include "datadisplayer.h"

inline char decToHex(char i) {
    return (char) (i < 10 ? ('0' + i) : ('A' + (i - 10)));
}

QString DataDisplayer::toASCII(const QByteArray &data) {
    QString result;
    for (char c: data) {
        if (c >= 0x20 && c <= 0x7E) {
            result += c;
        } else {
            result += ".";
        }
    }
    return std::move(result);
}

[[maybe_unused]] QString DataDisplayer::toHex(const QByteArray &data) {
    QString result;
    for (char c: data) {
        if (c == '#') {
            result += c;
        } else {
            char d1 = (char) ((char8_t) c >> 4), d2 = (char) ((char8_t) c - (d1 << 4));
            result += decToHex(d1);
            result += decToHex(d2);
        }
    }
    return std::move(result);
}
