#include "MainWindow.h"

#include <QApplication>

#include "FileSystemNotifier.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    FileSystemNotifier n;

    MainWindow w;
    w.show();

    return a.exec();
}
