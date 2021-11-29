#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QCommandLineParser>
#include "MainWindow.h"

#define  FIRST_LOGIN_SCREEN_FILE_PATH       QStandardPaths::standardLocations(QStandardPaths::CacheLocation)[0]
#define  FIRST_LOGIN_SCREEN_FILE_NAME       FIRST_LOGIN_SCREEN_FILE_PATH + "/info"
#define  VERSION                            "0.0.4"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("px-first-login-welcome-screen");
    QApplication::setApplicationVersion(VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription("PantherX First Login Welcome Screen");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption forceOption(QStringList() << "f" << "force",
                                    "Force running even if '~/.cache/px-first-login-welcome-screen/info' was exist");
    parser.addOption(forceOption);
    parser.process(app);

    QFile file(FIRST_LOGIN_SCREEN_FILE_NAME);
    if(!parser.isSet(forceOption) && file.exists()) {
        return -1;
    } else {
        QDir path(FIRST_LOGIN_SCREEN_FILE_PATH);
        if(!path.exists()){
            path.mkpath(FIRST_LOGIN_SCREEN_FILE_PATH);
        }
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream << "time:" << QDateTime::currentMSecsSinceEpoch() << Qt::endl;
            stream << "version:" << VERSION << Qt::endl;
            file.close();
        }
    }
    MainWindow window;
    return app.exec(); 
}
