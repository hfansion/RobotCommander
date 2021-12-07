//
// Created by hamlet on 2021/12/5.
//

#include "compositor.h"
#include "command/command.h"
#include "info/info.h"

#ifdef QT_DEBUG

#include <QDebug>
#include <iostream>

#endif

Compositor::~Compositor() {
    for (const Command *c: m_queCommand)
        delete c;
    for (const Info *i: m_listInfo) {
        delete i;
    }
}

void Compositor::addCommand(Command *command) {
    m_queCommand.enqueue(command);
    // 暂时的操作：收到一条命令即发送
    emit needSendCommand();
}

#ifdef QT_DEBUG

inline char decToHex(char i) {
    return (char) (i < 10 ? ('0' + i) : ('A' + (i - 10)));
}

void printData(const QByteArray &data) {
    for (const auto &c: data) {
        switch (c) {
            case '#':
//            case '&':
//            case '@':
                std::cout << c;
                break;
            default: {
                char d1 = (char) ((char8_t) c >> 4), d2 = (char) ((char8_t) c - (d1 << 4));
                std::cout << decToHex(d1) << decToHex(d2);
            }
        }
    }
    std::cout << std::endl;
}

#endif

const QByteArray &Compositor::encode() {
    if (m_queCommand.isEmpty()) return m_code;
    m_code.clear();
    m_encodeMessage.clear();
    while (!m_queCommand.isEmpty()) {
        Command *command = m_queCommand.dequeue();
        const auto &c = command->encode();
        m_code.append(c);
        m_code.append('&');
        m_encodeMessage.append(tr(" [command] ")).append(command->toString()).append('\n');
        delete command;
    }
    m_code.replace(m_code.size() - 1, 1, "@");
    checkSumAndPostProcess();
#ifdef QT_DEBUG
    printData(m_code);
#endif
    return m_code;
}

const QString &Compositor::getEncodeMessage() const {
    return m_encodeMessage;
}

void Compositor::decode(const QByteArray &data) {
    m_data = data;
    m_decodeMessage.clear();
    if (verifyAndPreProcess()) {
        m_queInfo.clear();
        m_listInfo.clear();
        auto list = m_data.split('&');
        // 暂时的操作
        m_listInfo.push_back(Info::decode(list[0]));
        m_queInfo.enqueue(&m_listInfo[0]);
        m_decodeMessage.append(tr(" [info] ")).append(m_listInfo[0].toString()).append('\n');
    } else {
        m_data.clear();
    }
}

void Compositor::checkSumAndPostProcess() {
    // 这里省略计算校验和的方法
    m_code.append('\0');

    m_code.push_front("###");
    m_code.push_back("###");
}

bool Compositor::verifyAndPreProcess() {
    if (m_code.startsWith("###") && m_code.endsWith("###")) {
        m_code.remove(0, 3);
        m_code.remove(m_code.size() - 3, 3);

        // 这里省略计算校验和的方法
        m_code.remove(m_code.size() - 2, 2);
        return true;
    } else {
        return false;
    }
}

const QString &Compositor::getDecodeMessage() const {
    return m_decodeMessage;
}

const Info *Compositor::getInfo() {
    return m_queInfo.dequeue();
}
