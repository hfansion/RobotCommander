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

MapWidget::MapWidget(QWidget *parent) : QWidget(parent) {
    m_field.setY(0);
    m_curP = m_field.topLeft();
}

MapWidget::~MapWidget() = default;

bool MapWidget::isInside(const QPoint &p) {
    return p.x() >= m_baseP.x() && p.x() <= (m_baseP.x() + m_imgMap.width())
           && p.y() >= m_baseP.y() && p.y() <= (m_baseP.y() + m_imgMap.height());
}

QPointF MapWidget::generateRatio(const QPoint &p) {
    return {static_cast<qreal>(p.x() - m_baseP.x()) / m_imgMap.width(),
            static_cast<qreal>(p.y() - m_baseP.y()) / m_imgMap.height()};
}

QPoint MapWidget::recoverRatio(const QPointF &ratio) {
    return {m_baseP.x() + static_cast<int>(m_imgMap.width() * ratio.x()),
            m_baseP.y() + static_cast<int>(m_imgMap.height() * ratio.y())};
}

void paintMark(QPainter &painter, const QPoint &p, const Settings::Mark &mk, const QPixmap &tmp_pic) {
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
    painter.drawPixmap(m_baseP, m_imgMap);
    paintMark(painter, recoverRatio(m_curP), m_settings->mark_cur, m_imgRobotCur);
    paintMark(painter, recoverRatio(m_tarP), m_settings->mark_tar, m_imgRobotTar);
}

void MapWidget::resizeEvent(QResizeEvent *event) {
    switch (m_viewForm) {
        case NormalView: {
            if (m_imgMap.width() > this->width()) {
                if ((this->width() - m_baseP.x()) > m_imgMap.width())
                    m_baseP.setX(this->width() - m_imgMap.width());
            } else {
                m_baseP.setX((this->width() - m_imgMap.width()) / 2);
            }
            if (m_imgMap.height() > this->height()) {
                if ((this->height() - m_baseP.y()) > m_imgMap.height())
                    m_baseP.setY(this->height() - m_imgMap.height());
            } else {
                m_baseP.setY((this->height() - m_imgMap.height()) / 2);
            }
            break;
        }
        case SuitableView: {
            m_imgMap = QPixmap{m_settings->map_pic}.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_baseP.setX((this->width() - m_imgMap.width()) / 2);
            m_baseP.setY((this->height() - m_imgMap.height()) / 2);
            break;
        }
        case FilledView: {
            // TODO
            break;
        }
    }
}

void MapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isInside(event->pos())) {
        m_tarP = generateRatio(event->pos());
        repaint();
        emit sendCommand(new PositionCommand(static_cast<int>(Position::X_RANGE * m_tarP.x()),
                                             static_cast<int>(Position::Y_RANGE * m_tarP.y())));
        event->accept();
    }
}

void MapWidget::infoCurPosition(const QPointF &pos) {
    m_curP = pos;
    repaint();
}

void MapWidget::updateSettings(const Settings *s) {
    m_settings = s;
    m_imgMap = QPixmap{s->map_pic};
    if (!s->mark_cur.pic.isEmpty())
        m_imgRobotCur = QPixmap{s->mark_cur.pic}.scaled(s->mark_cur.pic_size, s->mark_cur.pic_size);
    if (!s->mark_tar.pic.isEmpty())
        m_imgRobotTar = QPixmap{s->mark_tar.pic}.scaled(s->mark_tar.pic_size, s->mark_tar.pic_size);
    resizeEvent(nullptr);
}
