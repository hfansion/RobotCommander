//
// Created by hamlet on 2021/12/5.
//

#include <QTime>
#include "compositor.h"
#include "info/positioninfo.h"
#include "info/anyinfo.h"
#include "data/hexdisplayer.h"

Compositor::~Compositor() = default;

void Compositor::addCommand(Ptr<Command> command) {
    m_queCommand.push(std::move(command));
}

QByteArray Compositor::previewEncode(const Command *command) {
    QByteArray code;
    code.append(command->encode());
    checkSumAndPostProcess(code);
    return std::move(code);
}

QByteArray Compositor::encode() {
    if (m_queCommand.empty()) return {};
    QByteArray code;
    m_encodeMessage.clear();
    while (!m_queCommand.empty()) {
        auto &command = m_queCommand.front();
        code.append(command->encode());
        m_encodeMessage.append(tr(" [command] ")).append(command->toString()).append('\n');
        m_queCommand.pop();
    }
    checkSumAndPostProcess(code);
    return std::move(code);
}

const QString &Compositor::getEncodeMessage() const {
    return m_encodeMessage;
}

void Compositor::decode(QByteArray data) {
    if (verifyAndPreProcess(data)) {
        m_decodeMessage = timeStr().append(tr("receive: ")).append(HexDisplayer::toString(data)).append('\n');
        while (!m_queInfo.empty()) m_queInfo.pop();
        while (!data.isEmpty()) {
            Ptr<Info> info;
            bool succeed = true;
            char8_t typeData = extract(data, 1).at(0);
            if (typeData & static_cast<char8_t>(Protocol::Any)) {  // 第一位为1，即为Any类型
                int length = (typeData << 1) >> 1;
                info = std::make_unique<AnyInfo>(extract(data, length));
            } else {
                switch (static_cast<Protocol>(typeData)) {
                    case Protocol::Position: {
                        info = PositionInfo::decode(extract(data, PositionInfo::DATA_LENGTH));
                        break;
                    }
                    default: {
                        m_decodeMessage.append(tr(" [ERROR] contains unknown information")).append('\n');
                        succeed = false;
                    }
                }
            }
            if (succeed) m_queInfo.push(std::move(info));
        }
    } else {
        m_decodeMessage.append(tr(" [ERROR] information is broken")).append('\n');
    }
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

Compositor::Ptr<Info> Compositor::getInfo() {
    return pop(m_queInfo);
}

QByteArray Compositor::extract(QByteArray &data, qsizetype length) {
    QByteArray out = data.left(length);
    data.remove(0, length);
    return std::move(out);
}

template<typename T>
Compositor::Ptr<T> Compositor::pop(Queue<Ptr<T>> &queue) {
    if (queue.empty()) return {};
    else {
        Ptr<T> p = std::move(queue.front());
        queue.pop();
        return p;
    }
}

QString Compositor::timeStr() {
    auto c_time = QTime::currentTime();
    return std::move(QString("[%1:%2:%3] ").arg(c_time.hour()).arg(c_time.minute()).arg(c_time.second()));
}
