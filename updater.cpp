//
// Created by hamlet on 2022/3/11.
//

#include "updater.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "robotcommanderconfig.h"

Updater::Updater(Settings::ReleaseChannel channel, QObject *parent) :
        QObject(parent), m_manager(new QNetworkAccessManager(this)) {
    connect(m_manager, &QNetworkAccessManager::finished, this, &Updater::processData);
    QString url = tr("https://gitee.com/hfansion/RobotCommander/raw/%1/release.json");
    switch (channel) {
        case Settings::Main:
            url = url.arg("main");
            break;
        case Settings::Dev:
            url = url.arg("dev");
            break;
    }
    m_url = {url};
}

void Updater::check() {
    m_manager->get(QNetworkRequest{m_url});

}

void Updater::processData(QNetworkReply *reply) {
    auto json = QJsonDocument::fromJson(reply->readAll()).object();
    Result result = networkError;
    if (json.contains("rl_ver") && json["rl_ver"].toInt() >= 1) {
        m_version = qMove(json["version"].toString());
        m_link = {qMove(json["link"].toString())};
        result = m_version == ROBOTCOMMANDER_VERSION ? isLatest : isNotLatest;
    }
    emit checkFinished(result);
    reply->deleteLater();
}

const QString &Updater::getVersion() const {
    return m_version;
}

const QUrl &Updater::getLink() const {
    return m_link;
}
