#include <QCoreApplication>
#include <QDebug>

#include "fsnotifier.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // Make sure to place the fsnotifier executable in the working directory
    FileSystemNotifier fs;
    fs.startWatch();

    // Print dirty paths
    QObject::connect(&fs, &FileSystemNotifier::changed, [](const QStringList &paths) {
        qDebug() << "[Paths changed]";
        std::for_each(paths.begin(), paths.end(), [](const QString &path) {
            qDebug().noquote() << path;
        });
    });

    QObject::connect(&fs, &FileSystemNotifier::renamed, [](const QStringList &paths) {
        qDebug() << "[Paths renamed]";
        std::for_each(paths.begin(), paths.end(),[](const QString &path) {
            qDebug().noquote() << path;
        });
    });

    // Watch "C:/foo" recursively and "D:/bar" non-recursively
    fs.addRecursivePaths({"E:/test1"});

    return a.exec();
}
