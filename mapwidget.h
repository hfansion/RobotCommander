//
// Created by hamlet on 2021/11/8.
//

#ifndef ROBOTCOMMANDER_MAPWIDGET_H
#define ROBOTCOMMANDER_MAPWIDGET_H

#include <QWidget>

class Compositor;

class MapWidget : public QWidget {
Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);
    void injectCompositor(Compositor *compositor);

    ~MapWidget() override;

public slots:
    void infoCurPosition(const QPointF &pos);
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void openMapFile();

private:
    Compositor *m_compositor = nullptr;

    QPixmap m_imgMap;
    QPixmap m_tempMap;
    QPixmap m_imgRobot;
    QPoint m_curP;
    QPoint m_gotoP;
    QRect m_field;

    constexpr static const QPoint ROBOT_SIZE{32, 32};
    constexpr static const QColor ANCHOR_COLOR{255, 0, 0};
};

#endif //ROBOTCOMMANDER_MAPWIDGET_H
