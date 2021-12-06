//
// Created by hamlet on 2021/12/5.
//

#ifndef ROBOTCOMMANDER_COMPOSITOR_H
#define ROBOTCOMMANDER_COMPOSITOR_H

#include <QObject>
#include <QQueue>
#include <vector>

class Info;

class Command;

class Compositor : QObject {
Q_OBJECT
public:
    Compositor() = default;

    ~Compositor() override;

    void addCommand(Command *command);                      // 追加一条指令
    const QByteArray &encode();                             // 编码操作
    [[nodiscard]] const QString &getEncodeMessage() const;  // 获取当前编码的显示内容

    void decode(const QByteArray &data);                    // 解码信息
    const Info *getInfo();                                  // 获取解码结果
    [[nodiscard]] const QString &getDecodeMessage() const;  // 获取当前解码的显示内容

private:
    // 所有Command和Info的生命周期由本类的这两个queue管理
    QQueue<Command *> m_queCommand;
    QQueue<const Info *> m_queInfo;
    std::vector<Info> m_listInfo;
    QByteArray m_code, m_data;
    QString m_encodeMessage;
    QString m_decodeMessage;

    void checkSumAndPostProcess();

    bool verifyAndPreProcess();
};


#endif //ROBOTCOMMANDER_COMPOSITOR_H
