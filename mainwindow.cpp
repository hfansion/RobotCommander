//
// Created by hamlet on 2021/11/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorSpace>
#include <QImageReader>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    QImageReader reader{"/home/hamlet/Codebase/RobotCommander/resource/map.jpg"};
    reader.setAutoTransform(true);
    m_image = reader.read();
    if (m_image.colorSpace().isValid())
        m_image.convertToColorSpace(QColorSpace::SRgb);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *ev) {
    ui->imageLabel->setPixmap(QPixmap::fromImage(
            m_image.scaled(ui->centralwidget->size() - QSize(15, 15), Qt::KeepAspectRatio, Qt::SmoothTransformation)
            ));
}

