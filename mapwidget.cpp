//
// Created by hamlet on 2021/11/8.
//

#include "mapwidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include "info/info.h"
#include "command/command.h"
#include "command/positioncommand.h"
#include "compositor.h"
#include "settings.h"

MapWidget::MapWidget(QWidget *parent) : QWidget(parent) {
    m_field.setY(0);
    m_curP = m_field.topLeft();
}

MapWidget::~MapWidget() = default;

inline bool isInside(const QRect &rect, const QPoint &p) {
    return p.x() >= rect.x() && p.x() <= (rect.x() + rect.width())
           && p.y() >= rect.y() && p.y() <= (rect.y() + rect.height());
}

inline QPointF generateRatio(const QRect &rect, const QPoint &p) {
    return {(qreal) (p.x() - rect.x()) / rect.width(),
            (qreal) (p.y() - rect.y()) / rect.height()};
}

inline QPoint recoverRatio(const QRect &rect, const QPointF &ratio) {
    return rect.topLeft() + QPoint((int) (rect.width() * ratio.x()), (int) (rect.height() * ratio.y()));
}

inline void paintMark(QPainter &painter, const QPoint &p, const Settings::Mark &mk, const QPixmap &tmp_pic) {
    if (mk.pic_or_shape) {
        int size = mk.pic_size / 2;
        painter.drawPixmap(p - QPoint(size, size), tmp_pic);
    } else {
        int size = mk.shape_size / 2;
        switch (mk.shape) {
            case Settings::Mark::Square: {
                painter.fillRect(p.x() - size, p.y() - size, mk.shape_size, mk.shape_size, mk.color);
                break;
            }
            case Settings::Mark::Triangle: {
                painter.setBrush(QBrush(mk.color, Qt::SolidPattern));
                QPainterPath path;
                QVector<QPointF> points{QPointF(p.x(), p.y() - size), QPointF(p.x() - size, p.y() + size),
                                        QPointF(p.x() + size, p.y() + size), QPointF(p.x(), p.y() - size)};
                path.addPolygon(QPolygonF(points));
                painter.drawPath(path);
                break;
            }
            case Settings::Mark::Circular: {
                painter.setBrush(QBrush(mk.color, Qt::SolidPattern));
                QPainterPath path;
                path.addEllipse(QRectF(p.x() - size, p.y() - size, mk.shape_size, mk.shape_size));
                painter.drawPath(path);
                break;
            }
        }
    }
}

void MapWidget::paintEvent(QPaintEvent *event) {
    QPainter painter{this};
    painter.drawPixmap(m_field.topLeft(), m_tempMap);
    paintMark(painter, recoverRatio(m_field, m_curP), m_settings->mark_cur, m_imgRobotCur);
    paintMark(painter, recoverRatio(m_field, m_tarP), m_settings->mark_tar, m_imgRobotTar);
}

void MapWidget::resizeEvent(QResizeEvent *event) {
    m_tempMap = m_imgMap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_field.setX((this->width() - m_tempMap.width()) / 2);
    m_field.setSize(m_tempMap.size());
}

void MapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isInside(m_field, event->pos())) {
        m_tarP = generateRatio(m_field, event->pos());
        repaint();
        if (m_compositor->isConnected) {
            Command *command = new PositionCommand((int) (Position::X_RANGE * m_tarP.x()),
                                                   (int) (Position::Y_RANGE * m_tarP.y()));
            m_compositor->addCommand(command);
        }
        event->accept();
    }
}

void MapWidget::infoCurPosition(const QPointF &pos) {
    m_curP = pos;
    repaint();
}

void MapWidget::injectCompositor(Compositor *compositor) {
    m_compositor = compositor;
}

void MapWidget::injectSettings(const Settings *settings) {
    m_settings = settings;
    updateSettings();
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
