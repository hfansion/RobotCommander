//
// Created by hamlet on 2021/12/5.
//

#include "compositor.h"
#include "command/command.h"
#include "info/info.h"
#include "info/positioninfo.h"
#include "info/anyinfo.h"
#include "data/hexdisplayer.h"

template<typename T>
inline void deleteAllPointers(const QList<T> &anyList) {
    for (const T p: anyList)
        delete p;
}

Compositor::~Compositor() {
    deleteAllPointers(m_queCommand);
    deleteAllPointers(m_listInfo);
}

void Compositor::addCommand(Command *command) {
    m_queCommand.enqueue(command);
    // 暂时的操作：收到一条命令即发送
    emit needSendCommand();
}

QByteArray Compositor::previewEncode(const Command *command) {
    QByteArray code;
    code.append(command->encode());
    checkSumAndPostProcess(code);
    return std::move(code);
}

const QByteArray &Compositor::encode() {
    if (m_queCommand.isEmpty()) return m_sendCode;
    m_sendCode.clear();
    m_encodeMessage.clear();
    while (!m_queCommand.isEmpty()) {
        Command *command = m_queCommand.dequeue();
        const auto &c = command->encode();
        m_sendCode.append(c);
        m_encodeMessage.append(tr(" [command] ")).append(command->toString()).append('\n');
        delete command;
    }
    checkSumAndPostProcess(m_sendCode);
    return m_sendCode;
}

const QString &Compositor::getEncodeMessage() const {
    return m_encodeMessage;
}

void Compositor::decode(const QByteArray &data) {
    m_receiveCode = data;
    m_decodeMessage.clear();
    if (verifyAndPreProcess(m_receiveCode)) {
        m_queInfo.clear();
        deleteAllPointers(m_listInfo);
        m_listInfo.clear();
        while (!m_receiveCode.isEmpty()) {
            char8_t typeData = m_receiveCode.at(0);
            m_receiveCode.remove(0, 1);
            Info *info;
            if (typeData & '\x80') {  // 第一位为1，即为Any类型
                int data_length = (typeData << 1) >> 1;
                info = new AnyInfo(m_receiveCode.left(data_length));
                m_receiveCode.remove(0, data_length);
            } else {
                switch (static_cast<ProtocolReceive>(typeData)) {
                    case ProtocolReceive::Position: {
                        info = PositionInfo::decode(m_receiveCode.left(PositionInfo::DATA_LENGTH));
                        m_receiveCode.remove(0, PositionInfo::DATA_LENGTH);
                        break;
                    }
                    default: {
                        m_decodeMessage.append(tr(" [ERROR] cannot decode information")).append('\n');
                        goto OUT;
                    }
                }
            }
            m_listInfo.append(info);
            m_queInfo.enqueue(info);
            m_decodeMessage.append(tr(" [info] ")).append(info->toString()).append('\n');
        }
    } else {
        m_decodeMessage.append(tr(" [ERROR] information is broken")).append('\n');
        m_receiveCode.clear();
    }
    OUT:;
}

void Compositor::checkSumAndPostProcess(QByteArray &code) {
    // 这里省略计算校验和的方法
    code.append('\0');

    code.push_front("###");
    code.push_back("###");
}

bool Compositor::verifyAndPreProcess(QByteArray &code) {
    if (code.startsWith("###") && code.endsWith("###")) {
        code.remove(0, 3);
        code.remove(code.size() - 3, 3);

        // 这里省略计算校验和的方法
        code.remove(code.size() - 1, 1);
        return true;
    } else {
        return false;
    }
}

const QString &Compositor::getDecodeMessage() const {
    return m_decodeMessage;
}

const Info *Compositor::getInfo() {
    return m_queInfo.isEmpty() ? nullptr : m_queInfo.dequeue();
}
