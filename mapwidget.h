//
// Created by hamlet on 2021/11/8.
//

#ifndef ROBOTCOMMANDER_MAPWIDGET_H
#define ROBOTCOMMANDER_MAPWIDGET_H

#include <QWidget>
#include "settings.h"

class Command;

class MapWidget : public QWidget {
Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget() override;

signals:
    void sendCommand(Command *command);

public slots:
    void infoCurPosition(const QPointF &pos);
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void mousePressEvent(QMouseEvent *) override;

    void updateSettings(const Settings *settings);

private:
    const Settings *m_settings = nullptr;

    enum ViewForm {
        NormalView, SuitableView, FilledView
    };
    QPixmap m_imgMap;
    QPoint m_baseP;
    ViewForm m_viewForm = SuitableView;

    QPixmap m_imgRobotCur;
    QPixmap m_imgRobotTar;
    QPointF m_curP;  // 位置由0到1之间的double类型表示，下同
    QPointF m_tarP;
    inline bool isInside(const QPoint &p);
    inline QPointF generateRatio(const QPoint &p);
    inline QPoint recoverRatio(const QPointF &ratio);
};

#endif //ROBOTCOMMANDER_MAPWIDGET_H
