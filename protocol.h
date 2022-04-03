//
// Created by hamlet on 2021/12/7.
//

#ifndef ROBOTCOMMANDER_PROTOCOL_H
#define ROBOTCOMMANDER_PROTOCOL_H

enum class Protocol : char8_t {
    GET = '\x00', SET = '\x40',
    Position = '\x01',
    SET_GPS = '\x02',
    PID1 = '\x04',
    PID2 = '\x05',
    Any = u'\x80',
};

#endif //ROBOTCOMMANDER_PROTOCOL_H
