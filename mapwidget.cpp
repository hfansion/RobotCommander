//
// Created by hamlet on 2021/11/8.
//

#include "mapwidget.h"
#include <QAction>
#include <QCursor>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include "info/info.h"
#include "command/command.h"
#include "command/positioncommand.h"

MapWidget::MapWidget(QWidget *parent) : QWidget(parent), m_action_send(new QAction(this)), m_menu(new QMenu(this)) {
    this->setFocusPolicy(Qt::WheelFocus);
    m_menu->addAction(m_action_send);
    connect(m_action_send, &QAction::triggered, [this]() {
        emit sendCommand(m_command);
        m_tarP = m_tarPTmp;
    });
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
            m_imgMap = m_tmpMap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_baseP.setX((this->width() - m_imgMap.width()) / 2);
            m_baseP.setY((this->height() - m_imgMap.height()) / 2);
            break;
        }
        case FilledView: {
            if (m_imgMap.height() * this->width() >= m_imgMap.width() * this->height()) {
                m_imgMap = m_tmpMap.scaledToWidth(this->width(), Qt::SmoothTransformation);
                m_baseP.setX(0);
                m_fill_width_or_height = true;
                if (m_baseP.y() > 0) m_baseP.setY(0);
                else if (int h = this->height() - m_imgMap.height(); m_baseP.y() < h)
                    m_baseP.setY(h);
            } else {
                m_imgMap = m_tmpMap.scaledToHeight(this->height(), Qt::SmoothTransformation);
                m_baseP.setY(0);
                m_fill_width_or_height = false;
                if (m_baseP.x() > 0) m_baseP.setX(0);
                else if (int w = this->width() - m_imgMap.width(); m_baseP.x() < w)
                    m_baseP.setX(w);
            }
            break;
        }
    }
}

void MapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (m_viewForm == NormalView || m_viewForm == FilledView) {
            m_mouseP = event->pos();
            m_startP = m_baseP;
            this->setCursor(QCursor{Qt::ClosedHandCursor});
        }
    } else if (event->button() == Qt::RightButton && isInside(event->pos())) {
        m_tarPTmp = generateRatio(event->pos());
        repaint();
        int x = static_cast<int>(Position::X_RANGE * m_tarPTmp.x());
        int y = static_cast<int>(Position::Y_RANGE * m_tarPTmp.y());
        m_command = std::make_shared<PositionCommand>(x, y, 0.0f);
        m_action_send->setText(tr("Send(%1, %2)").arg(x).arg(y));
        m_menu->exec(event->globalPosition().toPoint());
    }
}

void MapWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!m_mouseP.isNull()) {
        m_baseP = m_startP + event->pos() - m_mouseP;
        if (m_viewForm == FilledView) {
            if (m_fill_width_or_height)
                m_baseP.setX(0);
            else
                m_baseP.setY(0);
        }
        if (m_viewForm == NormalView || (m_viewForm == FilledView && !m_fill_width_or_height)) {
            if (m_imgMap.width() >= this->width()) {
                if (int dx = m_baseP.x(); dx > 0) {
                    m_mouseP.rx() -= dx;
                    m_startP.rx() -= dx;
                    m_baseP.setX(0);
                } else if (dx = this->width() - m_baseP.x() - m_imgMap.width(); dx > 0) {
                    m_mouseP.rx() += dx;
                    m_startP.rx() += dx;
                    m_baseP.setX(this->width() - m_imgMap.width());
                }
            } else {
                m_baseP.setX((this->width() - m_imgMap.width()) / 2);
            }
        }
        if (m_viewForm == NormalView || (m_viewForm == FilledView && m_fill_width_or_height)) {
            if (m_imgMap.height() >= this->height()) {
                if (int dy = m_baseP.y(); dy > 0) {
                    m_mouseP.ry() -= dy;
                    m_startP.ry() -= dy;
                    m_baseP.setY(0);
                } else if (dy = this->height() - m_baseP.y() - m_imgMap.height(); dy > 0) {
                    m_mouseP.ry() += dy;
                    m_startP.ry() += dy;
                    m_baseP.setY(this->height() - m_imgMap.height());
                }
            } else {
                m_baseP.setY((this->height() - m_imgMap.height()) / 2);
            }
        }
        repaint();
    }
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (m_viewForm == NormalView || m_viewForm == FilledView) {
            m_mouseP = {};
            this->setCursor(QCursor{Qt::ArrowCursor});
        }
    }
}

void MapWidget::wheelEvent(QWheelEvent *event) {
    if (mod_Ctrl) {
        emit updateViewForm(NormalView);
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        repaint();
    } else {
        bool need_paint = false;
        if (m_viewForm == NormalView || (m_viewForm == FilledView && m_fill_width_or_height)) {
            if (m_imgMap.height() > this->height()) {
                int dy = event->angleDelta().y();
                if (dy > 0) {
                    m_baseP.ry() += 20;
                    if (m_baseP.y() > 0) m_baseP.setY(0);
                } else if (dy < 0) {
                    m_baseP.ry() -= 20;
                    if (int h = this->height() - m_imgMap.height(); m_baseP.y() < h)
                        m_baseP.setY(h);
                }
                need_paint = true;
            }
        }
        if (m_viewForm == NormalView || (m_viewForm == FilledView && !m_fill_width_or_height)) {
            if (m_imgMap.width() > this->width()) {
                int dx = event->angleDelta().x();
                if (dx > 0) {
                    m_baseP.rx() -= 20;
                    if (int w = this->width() - m_imgMap.width(); m_baseP.x() < w)
                        m_baseP.setX(w);
                } else if (dx < 0) {
                    m_baseP.rx() += 20;
                    if (m_baseP.x() > 0) m_baseP.setX(0);
                }
                need_paint = true;
            }
        }
        if (need_paint) repaint();
    }
}

void MapWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Control && event->modifiers() == Qt::ControlModifier) {
        mod_Ctrl = true;
    }
}

void MapWidget::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Control) {
        mod_Ctrl = false;
    }
}

void MapWidget::infoCurPosition(const QPointF &pos) {
    m_curP = pos;
    repaint();
}

void MapWidget::updateSettings(const Settings *s) {
    m_settings = s;
    m_tmpMap = QPixmap{s->map_pic};
    if (!s->mark_cur.pic.isEmpty())
        m_imgRobotCur = QPixmap{s->mark_cur.pic}.scaled(s->mark_cur.pic_size, s->mark_cur.pic_size);
    if (!s->mark_tar.pic.isEmpty())
        m_imgRobotTar = QPixmap{s->mark_tar.pic}.scaled(s->mark_tar.pic_size, s->mark_tar.pic_size);
    emit updateViewForm(m_viewForm);
    resizeEvent(nullptr);
}

void MapWidget::zoomIn(QPoint center) {
    if (center.isNull()) center = {this->width() / 2, this->height() / 2};
    m_baseP = {m_baseP.x() + (m_baseP.x() - center.x()) / 10,
               m_baseP.y() + (m_baseP.y() - center.y()) / 10};
    m_imgMap = m_tmpMap.scaled(m_imgMap.size() * 1.1, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    resizePaint();
}

void MapWidget::zoomOut(QPoint center) {
    if (center.isNull()) center = {this->width() / 2, this->height() / 2};
    m_baseP = {m_baseP.x() - (m_baseP.x() - center.x()) / 10,
               m_baseP.y() - (m_baseP.y() - center.y()) / 10};
    m_imgMap = m_tmpMap.scaled(m_imgMap.size() * 0.9, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    resizePaint();
}

void MapWidget::setViewForm(MapWidget::ViewForm viewForm) {
    m_viewForm = viewForm;
    if (viewForm != NormalView)
        resizePaint();
}

void MapWidget::resizePaint() {
    resizeEvent(nullptr);
    repaint();
}
