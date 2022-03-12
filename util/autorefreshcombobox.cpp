//
// Created by hamlet on 2022/3/12.
//

#include "autorefreshcombobox.h"

void AutoRefreshComboBox::mousePressEvent(QMouseEvent *e) {
    emit needRefresh();
    QComboBox::mousePressEvent(e);
}
