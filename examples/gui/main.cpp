#include "MainWindow.h"

#include <QApplication>
#include <QDir>

#include "fsnotifier.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // QDir dir(a.applicationDirPath());
    // dir.cdUp();
    // FileSystemNotifier::setExecutableFilePath(dir.path() + "/fsnotifier.exe");

    FileSystemNotifier n;

    MainWindow w;
    w.show();

    return a.exec();
}
