//
// Created by hamlet on 2021/11/8.
//

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QFileDialog>
#include "mapwidget.h"

MapWidget::MapWidget(QWidget *parent) :
        QWidget(parent),
        m_imgMap(QPixmap{":/resource/map.jpg"}),
        m_imgRobot(QPixmap{":/icon/robot.png"}.scaled(64, 64)) {
    m_field.setY(0);
    m_curP = m_field.topLeft();
}

MapWidget::~MapWidget() = default;

inline bool isInside(const QRect &rect, const QPoint &p) {
    return p.x() >= rect.x() && p.x() <= (rect.x() + rect.width())
           && p.y() >= rect.y() && p.y() <= (rect.y() + rect.height());
}

inline QPointF pointRatio(const QRect &rect, const QPoint &p) {
    return {(qreal) (p.x() - rect.x()) / rect.width(),
            (qreal) (p.y() - rect.y()) / rect.height()};
}

inline QPoint recoverRatio(const QRect &rect, const QPointF &ratio) {
    return rect.topLeft() + QPoint((int) (rect.width() * ratio.x()), (int) (rect.height() * ratio.y()));
}

void MapWidget::paintEvent(QPaintEvent *event) {
    QPainter painter{this};
    painter.drawPixmap(m_field.topLeft(), m_tempMap);
    painter.drawPixmap(m_curP - ROBOT_SIZE, m_imgRobot);
    painter.fillRect(m_gotoP.x() - 10, m_gotoP.y() - 10, 20, 20, ANCHOR_COLOR);
}

void MapWidget::resizeEvent(QResizeEvent *event) {
    QPointF ratio1 = pointRatio(m_field, m_curP), ratio2 = pointRatio(m_field, m_gotoP);

    m_tempMap = m_imgMap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_field.setX((this->width() - m_tempMap.width()) / 2);
    m_field.setSize(m_tempMap.size());

    m_curP = recoverRatio(m_field, ratio1);
    m_gotoP = recoverRatio(m_field, ratio2);
}

void MapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isInside(m_field, event->pos())) {
        m_gotoP = event->pos();
        repaint();
        emit commandGotoPosition(pointRatio(m_field, m_gotoP));
        event->accept();
    }
}

void MapWidget::infoCurPosition(const QPointF &pos) {
    m_curP = recoverRatio(m_field, pos);
    repaint();
}

void MapWidget::openMapFile() {
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Map"), ".",
                                                     QString("%1 (*.png *.jpg *.jpeg);;%2 (*.*)").arg(tr("Image"),
                                                                                                      tr("All files")));
    if (!file_name.isEmpty()) {
        m_imgMap = QPixmap(file_name);
        resizeEvent(nullptr);
    }
}
