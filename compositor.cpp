//
// Created by hamlet on 2021/12/5.
//

#include <QTime>
#include "compositor.h"
#include "info/positioninfo.h"
#include "info/anyinfo.h"
#include "data/datadisplayer.h"

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
    m_encodeMessage.insert(0,timeStr().append(tr("send: ")).append(DataDisplayer::toHex(code)).append('\n'));
    return std::move(code);
}

const QString &Compositor::getEncodeMessage() const {
    return m_encodeMessage;
}

void Compositor::decode(const QByteArray &data) {
    m_decodeMessage = timeStr().append(tr("receive: ")).append(DataDisplayer::toHex(data)).append('\n');
//    if (verifyAndPreProcess(data)) {
//        while (!m_queInfo.empty()) m_queInfo.pop();
    m_readBuffer.append(data);
    while (!m_readBuffer.isEmpty()) {
        if (m_readBuffer.startsWith("###")) {
            extract(m_readBuffer, 3);
        } else {
            Ptr<Info> info;
            DecodeStatus status = DecodeStatus::INCOMPLETE;
            char8_t typeData = extract(m_readBuffer, 1).at(0);
            if (typeData & static_cast<char8_t>(Protocol::Any)) {  // 第一位为1，即为Any类型
                int length = typeData & (~static_cast<char8_t>(Protocol::Any));
                if (m_readBuffer.length() >= length) {
                    info = std::make_unique<AnyInfo>(extract(m_readBuffer, length));
                    status = DecodeStatus::COMPLETE;
                } else {
                    m_readBuffer.insert(0, static_cast<char>(typeData));
                }
            } else {
                switch (static_cast<Protocol>(typeData)) {
                    case Protocol::Position: {
                        if (m_readBuffer.length() >= PositionInfo::DATA_LENGTH) {
                            info = PositionInfo::decode(extract(m_readBuffer, PositionInfo::DATA_LENGTH));
                            status = DecodeStatus::COMPLETE;
                        } else {
                            m_readBuffer.insert(0, static_cast<char>(typeData));
                        }
                        break;
                    }
                    default: {
                        m_decodeMessage.append(tr(" [ERROR] contains unknown information")).append('\n');
                        int j_left = -3, j_right;
                        bool found = false;
                        for (int i = 0; i < m_readBuffer.length(); ++i) {
                            if (m_readBuffer.at(i) == '#') {
                                j_right = i;
                                if ((j_right - j_left) > 2) {
                                    j_left = j_right;
                                } else if ((j_right - j_left) == 2) {
                                    if (m_readBuffer.at(j_right - 1) == '#') {
                                        found = true;
                                        break;
                                    }
                                }
                            }
                        }
                        if (found) {
                            extract(m_readBuffer, j_left);
                        } else {
                            m_readBuffer.clear();
                        }
                        status = DecodeStatus::BROKEN;
                    }
                }
            }
            switch (status) {
                case DecodeStatus::COMPLETE:
                    m_queInfo.push(std::move(info));
                    break;
                case DecodeStatus::INCOMPLETE:
                    goto OUT;
                case DecodeStatus::BROKEN:
                    break;
            }
        }
    }
    OUT:;
}

void Compositor::checkSumAndPostProcess(QByteArray &code) {
    code.push_front("###");
    code.push_back("###");
}

bool Compositor::verifyAndPreProcess(QByteArray &code) {
    if (code.startsWith("###") && code.endsWith("###")) {
        code.remove(0, 3);
        code.remove(code.size() - 3, 3);
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
