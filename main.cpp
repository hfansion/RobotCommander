#include <QApplication>
#include <QCommandLineParser>
#include "mainwindow.h"
#include "robotcommanderconfig.h"

int main(int argc, char *argv[]) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication app(argc, argv);
    QApplication::setOrganizationName("XJTU_Robocon");
    QApplication::setApplicationName("RobotCommander");
    QApplication::setApplicationVersion(ROBOTCOMMANDER_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(
            QApplication::tr("Robot Controlling and Debugging Tool version %1").arg(ROBOTCOMMANDER_VERSION));
    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(app);

    MainWindow mw;
    mw.show();
    return QApplication::exec();
}
