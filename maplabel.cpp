//
// Created by hamlet on 2021/11/8.
//

#include <QPainter>
#include <QDebug>
#include "maplabel.h"

MapLabel::MapLabel(QWidget *parent) :
        QLabel(parent),
        m_imgMap(QPixmap{"/home/hamlet/Codebase/RobotCommander/resource/map.jpg"}),
        m_imgRobot(QPixmap{"/home/hamlet/Codebase/RobotCommander/icon/robot.png"}.scaled(64, 64)) {
    m_curP.setX(0), m_curP.setY(0);
    this->setPixmap(m_imgRobot);
    this->setText("阿坝阿坝把");
}

MapLabel::~MapLabel() = default;

void MapLabel::paintEvent(QPaintEvent *event) {
    QPainter painter{this};
    painter.drawPixmap(m_curP, m_imgRobot);
}

void MapLabel::resizePic(const QSize &pic_size) {
//    qDebug() << pic_size;
//    this->setPixmap(
//            m_imgMap.scaled(pic_size, Qt::KeepAspectRatio, Qt::SmoothTransformation)
//    );
}
