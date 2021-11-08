//
// Created by hamlet on 2021/11/8.
//

#ifndef ROBOTCOMMANDER_MAPLABEL_H
#define ROBOTCOMMANDER_MAPLABEL_H

#include <QLabel>

class MapLabel : public QLabel {
Q_OBJECT
public:
    explicit MapLabel(QWidget *parent = nullptr);
    ~MapLabel() override;

public slots:
    void paintEvent(QPaintEvent *event) override;

    void resizePic(const QSize &pic_size);

private:
    QPixmap m_imgMap;
    QPixmap m_imgRobot;
    QPoint m_curP;
    QPoint m_gotoP;
};


#endif //ROBOTCOMMANDER_MAPLABEL_H
