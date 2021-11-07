//
// Created by hamlet on 2021/11/7.
//

#ifndef ROBOTCOMMANDER_MAINWINDOW_H
#define ROBOTCOMMANDER_MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

public slots:
    void resizeEvent(QResizeEvent *ev) override;

private:
    Ui::MainWindow *ui;

    QImage m_image;
};


#endif //ROBOTCOMMANDER_MAINWINDOW_H
