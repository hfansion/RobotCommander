//
// Created by hamlet on 2021/12/5.
//

#ifndef ROBOTCOMMANDER_COMPOSITOR_H
#define ROBOTCOMMANDER_COMPOSITOR_H

#include <queue>
#include <QObject>
#include "command/command.h"
#include "info/info.h"
#include "protocol.h"

class Compositor : public QObject {
Q_OBJECT
public:
    Compositor() = default;
    ~Compositor() override;

    template<typename T>
    using Queue = std::queue<T>;
    template<typename T>
    using Ptr = std::unique_ptr<T>;

    void addCommand(Ptr<Command> command);                   // 追加一条命令
    void send() { emit needSendCommand(); }                  // 发送全部命令
    QByteArray encode();                                     // 编码操作
    static QByteArray previewEncode(const Command *command); // 预览编码
    [[nodiscard]] const QString &getEncodeMessage() const;   // 获取当前编码的显示内容

    void decode(const QByteArray& data);                            // 解码信息
    Ptr<Info> getInfo();                                     // 获取解码结果
    [[nodiscard]] const QString &getDecodeMessage() const;   // 获取当前解码的显示内容

signals:
    void needSendCommand();

private:
    Queue<Ptr<Command>> m_queCommand;
    Queue<Ptr<Info>> m_queInfo;
    QString m_encodeMessage;
    QString m_decodeMessage;

    template<typename T>
    inline static Ptr<T> pop(Queue<Ptr<T>> &queue);
    inline static QByteArray extract(QByteArray &data, qsizetype length);
    static void checkSumAndPostProcess(QByteArray &code);
    static bool verifyAndPreProcess(QByteArray &code);
    static inline QString timeStr();

    QByteArray m_readBuffer;
    enum class DecodeStatus {
        COMPLETE, INCOMPLETE, BROKEN
    };
};


#endif //ROBOTCOMMANDER_COMPOSITOR_H
