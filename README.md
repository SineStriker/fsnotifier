# Filesystem events processor

## Brief

Qt/C++ cross-platform file system watcher library using JetBrains fsnotifier.

If you're using Qt and QFileSystemWatcher can't meet your demands, try this one.

## JetBrains IntelliJ IDEA

IntelliJ IDEA uses a child process to notify the host of file system changes safely and efficiently.

The source code of IntelliJ IDEA community edition has been published on JetBrains GitHub homepage, and the source code of fsnotifier is in directory named "native".

Since fsnotifier is coded in C, there are three different codes for Windows, MAC, and Linux. You need to use CMake to compile different code on different platform to build the platform dependent executable, on Windows, it's usually called fsnotifier.exe.

## C++ Process Manager

This is the Qt library that can manage JetBrains Filesystem events processor.

Most codes are C++ implementations of related Java code in IDEA.

## How to use

````
#include <QCoreApplication>

#include "JBFileWatchNamespace.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // Make sure to place the fsnotifier executable in the working directory
    JB::LocalFileSystem fs;
    fs.start();

    // Print dirty paths
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
    QObject::connect(&fs, &JB::LocalFileSystem::recursivePathsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Recursive paths dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(), [](const QString &path) {
            qDebug() << path;
        });
        qDebug() << " ";
    });
    
    QList<JB::WatchRequest> requests;

    // Watch "C:/foo" recursively and "D:/bar" non-recursively
    requests = fs.replaceWatchedRoots({}, {"C:/foo"}, {"D:/bar"});

    return a.exec();
}

````

If a file or directroy is renamed, signal "recursivePathsDirty" will be emitted.

If a file or directory is changed(created or removed), signal "recursivePathsDirty" will be emitted. And for its parent directroy, signal "pathsDirty" will be emitted.

## Message output

````
// #define JB_NO_DEBUG_OUTPUT
// #define JB_NO_WARNING_OUTPUT
````
Uncomment the lines in *JBFileWatcherUtils.h* to disable debug output and warning output.

## Suggestions

As long as JB::LocalFileSystem::start() is called, then JB::LocalFileSystem::dispose() is recommended to be called before QCoreApplication quits.