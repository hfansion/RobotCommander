//
// Created by hamlet on 2021/11/8.
//

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QFileDialog>
#include "mapwidget.h"
#include "compositor.h"
#include "command/command.h"
#include "command/positioncommand.h"
#include "info/info.h"
#include "settings.h"

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
    int size = m_settings->RCP_picSize / 2;
    painter.drawPixmap(m_curP - QPoint(size, size), m_imgRobot);
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
        QPointF ratio = pointRatio(m_field, m_gotoP);
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
    // TODO 整体画图设计已经落后，需要重构
    const auto &s = *m_settings;
    m_imgMap = QPixmap(s.mapPic);
    m_imgRobot = QPixmap{s.RCP_robotPic}.scaled(s.RCP_picSize, s.RCP_picSize);
    ANCHOR_COLOR = s.RTP_color;
    resizeEvent(nullptr);
}
