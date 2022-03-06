#include "FileSystemNotifier_p.h"
#include "FileSystemNotifier.h"
#include "JetBrainsFileWatcher/JBFileWatchNamespace.h"

using namespace JBFileWatcherUtils;

#include <QCoreApplication>

FileSystemNotifierPrivate::FileSystemNotifierPrivate(FileSystemNotifier *q) : q(q), fs(nullptr) {
    maxChangeEventId = 0;
}

FileSystemNotifierPrivate::~FileSystemNotifierPrivate() {
}

void FileSystemNotifierPrivate::init() {
    fs = new JBLocalFileSystem(q);

    q->connect(fs, &JBLocalFileSystem::pathsDirty, q, [&](const QStringList &paths) { //
        emit q->changed(listPathToNativeSeparators(paths));
    });
    q->connect(fs, &JBLocalFileSystem::flatDirsDirty, q, [&](const QStringList &paths) { //
        emit q->changed(listPathToNativeSeparators(paths));
    });
    q->connect(fs, &JBLocalFileSystem::recursivePathsDirty, q, [&](const QStringList &paths) { //
        emit q->renamed(listPathToNativeSeparators(paths));
    });
}

void FileSystemNotifierPrivate::postChange() {
    QCoreApplication::postEvent(q, new QTimerEvent(-(maxChangeEventId + 1)));
}

void FileSystemNotifierPrivate::commitChange() {
    QList<JB::WatchRequest> requestsToRemove;
    for (auto it = recursivePathsToRemove.begin(); it != recursivePathsToRemove.end(); ++it) {
        requestsToRemove.append(JB::WatchRequest(*it, true));
    }
    for (auto it = flatPathsToRemove.begin(); it != flatPathsToRemove.end(); ++it) {
        requestsToRemove.append(JB::WatchRequest(*it, false));
    }
    fs->replaceWatchedRoots(requestsToRemove, SetToList(recursivePathsToAdd),
                            SetToList(flatPathsToAdd));
    clearCachedPaths();
}

void FileSystemNotifierPrivate::clearCachedPaths() {
    recursivePathsToAdd.clear();
    recursivePathsToRemove.clear();
    flatPathsToAdd.clear();
    flatPathsToRemove.clear();
}

QSet<QString> FileSystemNotifierPrivate::recursivePaths() const {
    auto requests = fs->currentWatchedRoots();
    QSet<QString> res;
    for (auto it = requests.begin(); it != requests.end(); ++it) {
        if (!it->isSymlink() && it->isRecursive()) {
            res.insert(it->rootPath());
        }
    }
    return res;
}

QSet<QString> FileSystemNotifierPrivate::flatPaths() const {
    auto requests = fs->currentWatchedRoots();
    QSet<QString> res;
    for (auto it = requests.begin(); it != requests.end(); ++it) {
        if (!it->isSymlink() && !it->isRecursive()) {
            res.insert(it->rootPath());
        }
    }
    return res;
}

QPair<QSet<QString>, QSet<QString>> FileSystemNotifierPrivate::paths() const {
    auto requests = fs->currentWatchedRoots();
    QPair<QSet<QString>, QSet<QString>> res;
    for (auto it = requests.begin(); it != requests.end(); ++it) {
        if (!it->isSymlink()) {
            (it->isRecursive() ? res.first : res.second).insert(it->rootPath());
        }
    }
    return res;
}

QStringList FileSystemNotifierPrivate::listPathToNativeSeparators(const QStringList &paths) {
    QStringList res;
    for (auto it = paths.begin(); it != paths.end(); ++it) {
        res.append(QDir::toNativeSeparators(*it));
    }
    return res;
}
