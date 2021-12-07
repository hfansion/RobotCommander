//
// Created by hamlet on 2021/12/7.
//

#ifndef ROBOTCOMMANDER_PROTOCOL_H
#define ROBOTCOMMANDER_PROTOCOL_H

enum class ProtocolSend : char8_t {
    Position = '\x01',
    Any
};

enum class ProtocolReceive : char8_t {
    Position = '\x01',
    Any
};

#endif //ROBOTCOMMANDER_PROTOCOL_H
