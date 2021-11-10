//
// Created by hamlet on 2021/11/10.
//

#ifndef ROBOTCOMMANDER_UTIL_H
#define ROBOTCOMMANDER_UTIL_H

struct Point {
    int x;
    int y;
};

void moveForward(struct Point *curP, const struct Point *tarP) {
    if (curP->x < tarP->x) ++curP->x;
    else if (curP->x > tarP->x)--curP->x;
    if (curP->y < tarP->y) ++curP->y;
    else if (curP->y > tarP->y)--curP->y;
}

struct Point str2Point(const char *str) {
    int x = 0, y = 0, i, mid_i = 0;
    for (i = 0; str[i] != ')'; ++i) {
        if (str[i] == ',') {
            int times = 1;
            for (int j = i - 1; j > 0; --j) {
                x += (str[j] - '0') * times;
                times *= 10;
            }
            mid_i = i;
        }
    }
    int times = 1;
    for (int j = i - 1; j > mid_i; ++j) {
        y += (str[j] - '0') * times;
        times *= 10;
    }
    struct Point p{x, y};
    return p;
}

static char s[10];

char *point2Str(const struct Point *point) {
    s[0] = '(';
    int i = 0, x = point->x, y = point->y;
    if (x != 100) {
        if (x < 10)
            s[++i] = (char) ('0' + x);
        else {
            s[++i] = (char) ('0' + x / 10);
            s[++i] = (char) ('0' + x % 10);
        }
    } else {
        s[++i] = '1';
        s[++i] = '0';
        s[++i] = '0';
    }
    s[++i] = ',';
    if (y != 100) {
        if (y < 10)
            s[++i] = (char) ('0' + y);
        else {
            s[++i] = (char) ('0' + y / 10);
            s[++i] = (char) ('0' + y % 10);
        }
    } else {
        s[++i] = '1';
        s[++i] = '0';
        s[++i] = '0';
    }
    s[++i] = ')';
    s[++i] = '\0';
    return s;
}

#endif //ROBOTCOMMANDER_UTIL_H
