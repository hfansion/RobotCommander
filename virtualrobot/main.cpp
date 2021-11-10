#include <QApplication>
#include "vrmainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    VRMainWindow vrmw;
    vrmw.show();
    return QApplication::exec();
}

