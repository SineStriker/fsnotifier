#include <QCoreApplication>

#include "JBFileWatchNamespace.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // Make sure to place the fsnotifier executable in the working directory
    JB::LocalFileSystem fs;
    fs.start();

    // Current requests
    QList<JB::WatchRequest> requests;

    QObject::connect(&fs, &JB::LocalFileSystem::pathsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Paths dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(), [](const QString &path) {
            qDebug() << path;
        });
        qDebug() << " ";
    });
    QObject::connect(&fs, &JB::LocalFileSystem::flatDirsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Non-recursive dirs dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(), [](const QString &path) {
            qDebug() << path;
        });
        qDebug() << " ";
    });
    QObject::connect(&fs, &JB::LocalFileSystem::recursiveDirsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Recursive dirs dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(), [](const QString &path) {
            qDebug() << path;
        });
        qDebug() << " ";
    });

    // Watch "C:/foo" recursively and "D:/bar" non-recursively
    requests = fs.replaceWatchedRoots(requests, {"C:/Windows"}, {"E:/test1"});

    return a.exec();
}
