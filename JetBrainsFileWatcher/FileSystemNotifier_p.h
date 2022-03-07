#ifndef FILESYSTEMNOTIFIER_P_H
#define FILESYSTEMNOTIFIER_P_H

#include <QEvent>
#include <QScopedPointer>
#include <QSet>
#include <QString>
#include <QThread>

class JBLocalFileSystem;
class FileSystemNotifier;

class FileSystemNotifierPrivate {
public:
    FileSystemNotifierPrivate(FileSystemNotifier *q);
    ~FileSystemNotifierPrivate();

    void init();

    void postChange();
    void commitChange();

    void clearCachedPaths();

    QSet<QString> recursivePaths() const;
    QSet<QString> flatPaths() const;
    QPair<QSet<QString>, QSet<QString>> paths() const;

    FileSystemNotifier *const q; // q_ptr

    QSet<QString> recursivePathsToAdd;
    QSet<QString> flatPathsToAdd;
    QSet<QString> recursivePathsToRemove;
    QSet<QString> flatPathsToRemove;

    JBLocalFileSystem *fs;

    int maxChangeEventId;

    QStringList listPathToNativeSeparators(const QStringList &paths);
};

#endif // FILESYSTEMNOTIFIER_P_H
