//
// Created by hamlet on 2021/11/8.
//

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QPainterPath>
#include "mapwidget.h"
#include "compositor.h"
#include "command/command.h"
#include "command/positioncommand.h"
#include "info/info.h"
#include "settings.h"

MapWidget::MapWidget(QWidget *parent) :
        QWidget(parent),
        m_imgMap(QPixmap{":/resource/map.jpg"}),
        m_imgRobotCur(QPixmap{":/icon/robot.png"}.scaled(64, 64)) {
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
    if (m_settings->RCP_picThanShape) {
        int size = m_settings->RCP_picSize / 2;
        painter.drawPixmap(m_curP - QPoint(size, size), m_imgRobotCur);
    } else {
        int size = m_settings->RCP_shapeSize / 2;
        switch (m_settings->RCP_shape) {
            case Settings::Square: {
                painter.fillRect(m_curP.x() - size, m_curP.y() - size, m_settings->RCP_shapeSize,
                                 m_settings->RCP_shapeSize, m_settings->RCP_color);
                break;
            }
            case Settings::Triangle: {
                painter.setBrush(QBrush(m_settings->RCP_color, Qt::SolidPattern));
                QPainterPath path;
                QVector<QPointF> points{QPointF(m_curP.x(), m_curP.y() - size),
                                        QPointF(m_curP.x() - size, m_curP.y() + size),
                                        QPointF(m_curP.x() + size, m_curP.y() + size)};
                path.addPolygon(QPolygonF(points));
                painter.drawPath(path);
                break;
            }
            case Settings::Circular: {
                painter.setBrush(QBrush(m_settings->RCP_color, Qt::SolidPattern));
                QPainterPath path;
                path.addEllipse(QRectF(m_curP.x() - size, m_curP.y() - size, m_settings->RCP_shapeSize,
                                       m_settings->RCP_shapeSize));
                painter.drawPath(path);
                break;
            }
        }
    }
    if (m_settings->RTP_picThanShape) {
        int size = m_settings->RTP_picSize / 2;
        painter.drawPixmap(m_tarP - QPoint(size, size), m_imgRobotTar);
    } else {
        int size = m_settings->RTP_shapeSize / 2;
        switch (m_settings->RTP_shape) {
            case Settings::Square: {
                painter.fillRect(m_tarP.x() - size, m_tarP.y() - size, m_settings->RTP_shapeSize,
                                 m_settings->RTP_shapeSize, m_settings->RTP_color);
                break;
            }
            case Settings::Triangle: {
                painter.setBrush(QBrush(m_settings->RTP_color, Qt::SolidPattern));
                QPainterPath path;
                QVector<QPointF> points{QPointF(m_tarP.x(), m_tarP.y() - size),
                                        QPointF(m_tarP.x() - size, m_tarP.y() + size),
                                        QPointF(m_tarP.x() + size, m_tarP.y() + size)};
                path.addPolygon(QPolygonF(points));
                painter.drawPath(path);
                break;
            }
            case Settings::Circular: {
                painter.setBrush(QBrush(m_settings->RTP_color, Qt::SolidPattern));
                QPainterPath path;
                path.addEllipse(QRectF(m_tarP.x() - size, m_tarP.y() - size, m_settings->RTP_shapeSize,
                                       m_settings->RTP_shapeSize));
                painter.drawPath(path);
                break;
            }
        }
    }
}

void MapWidget::resizeEvent(QResizeEvent *event) {
    QPointF ratio1 = pointRatio(m_field, m_curP), ratio2 = pointRatio(m_field, m_tarP);

    m_tempMap = m_imgMap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_field.setX((this->width() - m_tempMap.width()) / 2);
    m_field.setSize(m_tempMap.size());

    m_curP = recoverRatio(m_field, ratio1);
    m_tarP = recoverRatio(m_field, ratio2);
}

void MapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isInside(m_field, event->pos())) {
        m_tarP = event->pos();
        repaint();
        QPointF ratio = pointRatio(m_field, m_tarP);
        Command *command = new PositionCommand((int) (Info::X_RANGE * ratio.x()), (int) (Info::Y_RANGE * ratio.y()));
        m_compositor->addCommand(command);
        event->accept();
    }
}

void MapWidget::infoCurPosition(const QPointF &pos) {
    m_curP = recoverRatio(m_field, pos);
    repaint();
}

void MapWidget::injectCompositor(Compositor *compositor) {
    m_compositor = compositor;
}

void MapWidget::injectSettings(const Settings *settings) {
    m_settings = settings;
}

void MapWidget::updateSettings() {
    const auto &s = *m_settings;
    m_imgMap = QPixmap(s.mapPic);
    if (!s.RCP_robotPic.isEmpty())
        m_imgRobotCur = QPixmap{s.RCP_robotPic}.scaled(s.RCP_picSize, s.RCP_picSize);
    if (!s.RTP_robotPic.isEmpty())
        m_imgRobotTar = QPixmap{s.RTP_robotPic}.scaled(s.RTP_picSize, s.RTP_picSize);
    resizeEvent(nullptr);
}
