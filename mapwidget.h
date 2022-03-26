//
// Created by hamlet on 2021/11/8.
//

#ifndef ROBOTCOMMANDER_MAPWIDGET_H
#define ROBOTCOMMANDER_MAPWIDGET_H

#include <QWidget>
#include "settings.h"

class QMenu;
class Command;

class MapWidget : public QWidget {
Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget() override;
    void infoCurPosition(const QPointF &pos);
    void updateSettings(const Settings *settings);

    enum ViewForm {
        NormalView, SuitableView, FilledView
    };
    void zoomIn(QPoint center={});
    void zoomOut(QPoint center={});
    void setViewForm(ViewForm viewForm);

signals:
    void sendCommand(std::shared_ptr<Command> command);
    void updateViewForm(ViewForm viewForm);

protected slots:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    const Settings *m_settings = nullptr;

    QPixmap m_tmpMap;
    QPixmap m_imgMap;
    QPoint m_baseP;
    ViewForm m_viewForm = SuitableView;

    QPixmap m_imgRobotCur;
    QPixmap m_imgRobotTar;
    QPointF m_curP;  // 位置由0到1之间的double类型表示，下同
    QPointF m_tarP;
    QPointF m_tarPTmp;
    inline bool isInside(const QPoint &p);
    inline QPointF generateRatio(const QPoint &p);
    inline QPoint recoverRatio(const QPointF &ratio);

    QPoint m_mouseP;
    QPoint m_startP;
    bool m_fill_width_or_height=true;  // FilledView模式下，铺满width为true，铺满height为false

    bool mod_Ctrl=false;

    inline void resizePaint();

    QMenu *m_menu;
    QAction *m_action_send;
    std::shared_ptr<Command> m_command;
};

#endif //ROBOTCOMMANDER_MAPWIDGET_H
