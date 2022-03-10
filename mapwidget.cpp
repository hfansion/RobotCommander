//
// Created by hamlet on 2021/11/8.
//

#include <QPainter>
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
        m_imgRobotCur(QPixmap{":/icon/robot.png"}.scaled(48, 48)) {
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
    const auto &mc = m_settings->mark_cur;  // current
    if (mc.pic_or_shape) {
        int size = mc.pic_size / 2;
        painter.drawPixmap(m_curP - QPoint(size, size), m_imgRobotCur);
    } else {
        int size = mc.shape_size / 2;
        switch (mc.shape) {
            case Settings::Mark::Square: {
                painter.fillRect(m_curP.x() - size, m_curP.y() - size, mc.shape_size, mc.shape_size, mc.color);
                break;
            }
            case Settings::Mark::Triangle: {
                painter.setBrush(QBrush(mc.color, Qt::SolidPattern));
                QPainterPath path;
                QVector<QPointF> points{QPointF(m_curP.x(), m_curP.y() - size),
                                        QPointF(m_curP.x() - size, m_curP.y() + size),
                                        QPointF(m_curP.x() + size, m_curP.y() + size),
                                        QPointF(m_curP.x(), m_curP.y() - size)};
                path.addPolygon(QPolygonF(points));
                painter.drawPath(path);
                break;
            }
            case Settings::Mark::Circular: {
                painter.setBrush(QBrush(mc.color, Qt::SolidPattern));
                QPainterPath path;
                path.addEllipse(QRectF(m_curP.x() - size, m_curP.y() - size, mc.shape_size, mc.shape_size));
                painter.drawPath(path);
                break;
            }
        }
    }
    const auto &mt = m_settings->mark_tar;  // target
    if (mt.pic_or_shape) {
        int size = mt.pic_size / 2;
        painter.drawPixmap(m_tarP - QPoint(size, size), m_imgRobotTar);
    } else {
        int size = mt.shape_size / 2;
        switch (mt.shape) {
            case Settings::Mark::Square: {
                painter.fillRect(m_tarP.x() - size, m_tarP.y() - size, mt.shape_size, mt.shape_size, mt.color);
                break;
            }
            case Settings::Mark::Triangle: {
                painter.setBrush(QBrush(mt.color, Qt::SolidPattern));
                QPainterPath path;
                QVector<QPointF> points{QPointF(m_tarP.x(), m_tarP.y() - size),
                                        QPointF(m_tarP.x() - size, m_tarP.y() + size),
                                        QPointF(m_tarP.x() + size, m_tarP.y() + size),
                                        QPointF(m_tarP.x(), m_tarP.y() - size)};
                path.addPolygon(QPolygonF(points));
                painter.drawPath(path);
                break;
            }
            case Settings::Mark::Circular: {
                painter.setBrush(QBrush(mt.color, Qt::SolidPattern));
                QPainterPath path;
                path.addEllipse(QRectF(m_tarP.x() - size, m_tarP.y() - size, mt.shape_size, mt.shape_size));
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
        Command *command = new PositionCommand((int) (Position::X_RANGE * ratio.x()), (int) (Position::Y_RANGE * ratio.y()));
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
    m_imgMap = QPixmap(s.map_pic);
    if (!s.mark_cur.pic.isEmpty())
        m_imgRobotCur = QPixmap{s.mark_cur.pic}.scaled(s.mark_cur.pic_size, s.mark_cur.pic_size);
    if (!s.mark_tar.pic.isEmpty())
        m_imgRobotTar = QPixmap{s.mark_tar.pic}.scaled(s.mark_tar.pic_size, s.mark_tar.pic_size);
    resizeEvent(nullptr);
}
