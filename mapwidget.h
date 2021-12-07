//
// Created by hamlet on 2021/11/8.
//

#ifndef ROBOTCOMMANDER_MAPWIDGET_H
#define ROBOTCOMMANDER_MAPWIDGET_H

#include <QWidget>

class Compositor;

class Settings;

class MapWidget : public QWidget {
Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);

    void injectCompositor(Compositor *compositor);

    void injectSettings(const Settings *settings);

    ~MapWidget() override;

public slots:

    void infoCurPosition(const QPointF &pos);

    void paintEvent(QPaintEvent *) override;

    void resizeEvent(QResizeEvent *) override;

    void mousePressEvent(QMouseEvent *) override;

    void updateSettings();

private:
    // inject
    Compositor *m_compositor = nullptr;
    const Settings *m_settings = nullptr;

    QPixmap m_imgMap;
    QPixmap m_tempMap;
    QPixmap m_imgRobotCur;
    QPixmap m_imgRobotTar;
    QPoint m_curP;
    QPoint m_tarP;
    QRect m_field;
};

#endif //ROBOTCOMMANDER_MAPWIDGET_H
