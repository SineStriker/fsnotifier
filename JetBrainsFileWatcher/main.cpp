#include "Application.h"
#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    Application a(argc, argv);
    JB::LocalFileSystem fs;

    MainWindow w;
    w.show();

    return a.exec();
}
