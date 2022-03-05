#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    //    QApplication a(argc, argv);

    //    JBLocalFileSystem fs;

    //    MainWindow w;
    //    w.show();

    //    return a.exec();

    QApplication a(argc, argv);

    QList<JB::WatchRequest> requests;

    JB::LocalFileSystem fs;
    fs.start();

    // Watch "C:/foo" recursively and "D:/bar" non-recursively
    requests = fs.replaceWatchedRoots(requests, {"C:/Windows"}, {"E:/test1"});

    QObject::connect(&fs, &JB::LocalFileSystem::pathsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "Paths dirty:" << dirtyPaths;
    });
    QObject::connect(&fs, &JB::LocalFileSystem::flatDirsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "Non-recursive dirs dirty:" << dirtyPaths;
    });
    QObject::connect(&fs, &JB::LocalFileSystem::recursiveDirsDirty,
                     [](const QStringList &dirtyPaths) { //
                         qDebug() << "Recursive dirs dirty:" << dirtyPaths;
                     });

    MainWindow w;
    w.show();

    return a.exec();
}
