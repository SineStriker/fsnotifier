#include "MainWindow.h"

#include <QApplication>
#include <QDir>

#include "FileSystemNotifier.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

//    QDir dir(a.applicationDirPath());
//    dir.cdUp();
//    FileSystemNotifier::setExecutableFilePath(dir.path() + "/fsnotifier.exe");

//    printf("%s\n", FileSystemNotifier::ExecutableFilePath().toStdString().c_str());
//    fflush(stdout);

    FileSystemNotifier n;

    MainWindow w;
    w.show();

    return a.exec();
}
