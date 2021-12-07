//
// Created by hamlet on 2021/12/7.
//

#include "anycommand.h"
#include <utility>
#include "../data/hexdisplayer.h"

AnyCommand::AnyCommand(QByteArray cmd) : m_code(std::move(cmd)) {
    m_code.insert(0, (char) ((char8_t) m_code.length() | '\x80'));
}

QByteArray AnyCommand::encode() const {
    return m_code;
}

QString AnyCommand::toString() const {
    return std::move(HexDisplayer::toString(m_code));
}
