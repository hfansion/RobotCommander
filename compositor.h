//
// Created by hamlet on 2021/12/5.
//

#ifndef ROBOTCOMMANDER_COMPOSITOR_H
#define ROBOTCOMMANDER_COMPOSITOR_H

#include <QObject>
#include <QStack>

class Info;

class Command;

class Compositor {
Q_OBJECT
public:
    Compositor();

    ~Compositor();

    void addCommand(Command *command);        // 追加一条指令
    const QByteArray &encode();               // 编码操作
    const QString &getMessage();              // 获取当前编码的显示内容

    void *decode(const QByteArray &data);     // 解码信息
    Info *getInfo();                          // 获取解码结果

private:
    // 所有Command和Info的生命周期由本类的这两个stack管理
    QStack<Command *> m_stkCommand;
    QStack<Info *> m_stkInfo;
    QByteArray m_code;
    QString m_message;
};


#endif //ROBOTCOMMANDER_COMPOSITOR_H
