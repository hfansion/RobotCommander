//
// Created by hamlet on 2022/3/14.
//

#ifndef ROBOTCOMMANDER_PANELBASE_H
#define ROBOTCOMMANDER_PANELBASE_H

#include <QWidget>

class PanelBase : public QWidget {
Q_OBJECT

public:
    using QWidget::QWidget;
    virtual QString PanelName() = 0;
    virtual void retranslateUi() = 0;
};

#endif //ROBOTCOMMANDER_PANELBASE_H
