//
// Created by hamlet on 2022/3/10.
//

#include "settings.h"
#include <QDataStream>
#include <QSettings>

Q_DECLARE_METATYPE(Settings::Mark);

QDataStream &operator<<(QDataStream &out, const Settings::Mark &mark) {
    out << mark.pic_or_shape << mark.pic << mark.pic_size << static_cast<int>(mark.shape) << mark.shape_size
        << mark.color;
    return out;
}

QDataStream &operator>>(QDataStream &in, Settings::Mark &mark) {
    int shape;
    in >> mark.pic_or_shape >> mark.pic >> mark.pic_size >> shape >> mark.shape_size >> mark.color;
    mark.shape = static_cast<Settings::Mark::Shape>(shape);
    return in;
}

Q_DECLARE_METATYPE(Settings::Serial);

QDataStream &operator<<(QDataStream &out, const Settings::Serial &s) {
    out << s.name << s.baudRate << s.stringBaudRate << s.dataBits << s.stringDataBits << s.parity << s.stringParity
        << s.stopBits << s.stringStopBits << s.flowControl << s.stringFlowControl;
    return out;
}

QDataStream &operator>>(QDataStream &in, Settings::Serial &s) {
    in >> s.name >> s.baudRate >> s.stringBaudRate >> s.dataBits >> s.stringDataBits >> s.parity >> s.stringParity
       >> s.stopBits >> s.stringStopBits >> s.flowControl >> s.stringFlowControl;
    return in;
}

Settings::Settings() {
    qRegisterMetaType<Mark>();
    qRegisterMetaType<Serial>();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    qRegisterMetaTypeStreamOperators<Mark>();
    qRegisterMetaTypeStreamOperators<Serial>();
#endif
    m_settings = new QSettings;
    if (m_settings->contains("map_pic")) {
        map_pic = m_settings->value("map_pic").toString();
        mark_cur = m_settings->value("mark_cur").value<Mark>();
        mark_tar = m_settings->value("mark_tar").value<Mark>();
        serial = m_settings->value("serial").value<Serial>();
        language = static_cast<Language>(m_settings->value("language").toInt());
        channel = static_cast<ReleaseChannel>(m_settings->value("channel").toInt());
        auto_check_update = m_settings->value("auto_check_update").toBool();
        settingsDialog_size = m_settings->value("settingsDialog_size").toSize();
    } else {
        map_pic = ":/resource/map.jpg";
        mark_cur = {true, ":/icon/robot.png", 48, Mark::Square, 20, QColor(255, 255, 0)};
        mark_tar = {false, ":/icon/robot.png", 48, Mark::Circular, 16, QColor(255, 0, 0)};
    }
}

Settings::~Settings() {
    save();
    delete m_settings;
}

void Settings::save() {
    m_settings->setValue("map_pic", map_pic);
    m_settings->setValue("mark_cur", QVariant::fromValue(mark_cur));
    m_settings->setValue("mark_tar", QVariant::fromValue(mark_tar));
    m_settings->setValue("serial", QVariant::fromValue(serial));
    m_settings->setValue("language", static_cast<int>(language));
    m_settings->setValue("channel", static_cast<int>(channel));
    m_settings->setValue("auto_check_update", auto_check_update);
    m_settings->setValue("settingsDialog_size", settingsDialog_size);
}
