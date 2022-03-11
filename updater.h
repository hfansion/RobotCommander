//
// Created by hamlet on 2022/3/11.
//

#ifndef ROBOTCOMMANDER_UPDATER_H
#define ROBOTCOMMANDER_UPDATER_H

#include <QObject>
#include <QJsonObject>
#include "settings.h"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

class Updater : public QObject {
Q_OBJECT
public:
    explicit Updater(Settings::ReleaseChannel channel, QObject *parent = nullptr);
    void check();
    [[nodiscard]] const QString &getVersion() const;
    [[nodiscard]] const QUrl &getLink() const;

signals:
    void checkFinished(bool needUpdate);

private slots:
    void processData(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    QUrl m_url;
    QString m_version;
    QUrl m_link;
};

#endif //ROBOTCOMMANDER_UPDATER_H
