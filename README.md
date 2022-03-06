# Filesystem events processor

## Brief

Qt/C++ cross-platform file system watcher library using JetBrains fsnotifier.

If you're using Qt and QFileSystemWatcher can't meet your demands, try this one.

## JetBrains IntelliJ IDEA

IntelliJ IDEA uses a child process to notify the host of file system changes safely and efficiently.

The source code of IntelliJ IDEA community edition has been published on JetBrains GitHub homepage, and the source code of fsnotifier is in directory named "native".

Since fsnotifier is coded in C, there are three different codes for Windows, MAC, and Linux. You need to use CMake to compile different code on different platform to build the platform dependent executable, on Windows, it's usually called fsnotifier.exe.

#### IntelliJ IDEA community edition

https://github.com/JetBrains/intellij-community

#### WinFsNotifier

https://github.com/JetBrains/intellij-community/tree/master/native/WinFsNotifier

#### Linux And Mac fsNotifier

https://github.com/JetBrains/intellij-community/tree/master/native/fsNotifier

## C++ Process Manager

This is the Qt library that can manage JetBrains Filesystem events processor.

Most codes are C++ implementations of related Java code in IDEA.

## How to use

### Method 1
````
#include <QCoreApplication>

#include "FileSystemNotifier.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // Make sure to place the fsnotifier executable in the working directory
    FileSystemNotifier fs;
    fs.startWatch();

    // Print dirty paths
    connect(fs, &FileSystemNotifier::changed, [](const QStringList &paths) {
        qDebug() << "[Paths changed]";
        std::for_each(paths.begin(), paths.end(), [](const QString &path) {
            qDebug().noquote() << path;
        });
    });

    connect(fs, &FileSystemNotifier::renamed, [](const QStringList &paths) {
        qDebug() << "[Paths renamed]";
        std::for_each(paths.begin(), paths.end(), [](const QString &path) {
            qDebug().noquote() << path;
        });
    });

    // Watch "C:/foo" recursively and "D:/bar" non-recursively
    fs->addRecursivePaths({"C:/foo"});
    fs->addFlatPaths({"D:/bar"});

    return a.exec();
}
````

If a file or directroy is renamed(created or removed), signal "renamed" will be emitted.

If a file or directory is changed, signal "changed" will be emitted.

And for its parent directroy, signal "changed" will be emitted.


### Method 2

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
    });
    QObject::connect(&fs, &JB::LocalFileSystem::flatDirsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Non-recursive dirs dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(), [](const QString &path) {
            qDebug() << path;
        });
    });
    QObject::connect(&fs, &JB::LocalFileSystem::recursivePathsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Recursive paths dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(), [](const QString &path) {
            qDebug() << path;
        });
    });
    
    QList<JB::WatchRequest> requests;

    // Watch "C:/foo" recursively and "D:/bar" non-recursively
    requests = fs.replaceWatchedRoots({}, {"C:/foo"}, {"D:/bar"});

    return a.exec();
}

````

If a file or directroy is renamed(created or removed), signal "recursivePathsDirty" will be emitted.

If a file or directory is changed, signal "pathsDirty" or "flatDirsDirty" will be emitted.

And for its parent directroy, signal "pathsDirty" will be emitted.

## Message output

````
// #define JB_NO_DEBUG_OUTPUT
// #define JB_NO_WARNING_OUTPUT
````
Uncomment the lines in *JBFileWatcherUtils.h* to disable debug output and warning output.

## Suggestions

As long as JB::LocalFileSystem::start() is called, then JB::LocalFileSystem::dispose() is recommended to be called before QCoreApplication quits although it will call automatically after signal QCoreApplication::aboutToQuit emitted.