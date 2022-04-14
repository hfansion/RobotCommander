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
    QString url = tr("https://gitee.com/jadulose/RobotCommander/raw/%1/release.json");
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
        m_version = std::move(json["version"].toString());
        m_link = {std::move(json["link"].toString())};
        auto versions = m_version.split('.');
        result = isLatest;
        if (int major = versions[0].toInt(); major > ROBOTCOMMANDER_VERSION_MAJOR) {
            result = isNotLatest;
        } else if (major == ROBOTCOMMANDER_VERSION_MAJOR) {
            if (int minor = versions[1].toInt(); minor > ROBOTCOMMANDER_VERSION_MINOR) {
                result = isNotLatest;
            } else if (minor == ROBOTCOMMANDER_VERSION_MINOR) {
                if (int patch = versions[2].toInt(); patch > ROBOTCOMMANDER_VERSION_PATCH) {
                    result = isNotLatest;
                }
            }
        }
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
