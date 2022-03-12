//
// Created by hamlet on 2022/3/12.
//

#ifndef ROBOTCOMMANDER_AUTOREFRESHCOMBOBOX_H
#define ROBOTCOMMANDER_AUTOREFRESHCOMBOBOX_H

#include <QComboBox>

class AutoRefreshComboBox : public QComboBox {
Q_OBJECT
public:
    using QComboBox::QComboBox;

signals:
    void needRefresh();

public slots:
    void mousePressEvent(QMouseEvent *e) override;
};


#endif //ROBOTCOMMANDER_AUTOREFRESHCOMBOBOX_H
