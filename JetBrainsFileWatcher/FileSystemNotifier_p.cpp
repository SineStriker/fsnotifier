#include "FileSystemNotifier_p.h"
#include "FileSystemNotifier.h"
#include "JetBrainsFileWatcher/JBFileWatchNamespace.h"

using namespace JBFileWatcherUtils;

#include <QCoreApplication>

FileSystemNotifierPrivate::FileSystemNotifierPrivate(FileSystemNotifier *q) : q(q), fs(nullptr) {
    hasChangeEvent = false;
    rootsNeedUpdate = false;
    separatorSystemDependent = true;
}

FileSystemNotifierPrivate::~FileSystemNotifierPrivate() {
}

void FileSystemNotifierPrivate::init() {
    fs = new JBLocalFileSystem(q);

    q->connect(fs, &JBLocalFileSystem::pathsDirty, q, [this](const QStringList &paths) { //
        emit q->changed((separatorSystemDependent ? listPathToNativeSeparators
                                                  : listPathFromNativeSeparators)(paths));
    });
    q->connect(fs, &JBLocalFileSystem::flatDirsDirty, q, [this](const QStringList &paths) { //
        emit q->changed((separatorSystemDependent ? listPathToNativeSeparators
                                                  : listPathFromNativeSeparators)(paths));
    });
    q->connect(fs, &JBLocalFileSystem::recursivePathsDirty, q, [this](const QStringList &paths) { //
        emit q->renamed((separatorSystemDependent ? listPathToNativeSeparators
                                                  : listPathFromNativeSeparators)(paths));
    });
    q->connect(fs, &JBLocalFileSystem::failureOccured, q, [this](const QString &reason) { //
        emit q->failed(reason);
    });
}

void FileSystemNotifierPrivate::postChange() {
    hasChangeEvent = true;
    QCoreApplication::postEvent(q, new QTimerEvent(-1));
}

void FileSystemNotifierPrivate::commitChange() {
    rootsNeedUpdate = false;
    if (!recursivePathsToRemove.isEmpty() || !flatPathsToRemove.isEmpty() ||
        !recursivePathsToAdd.isEmpty() || !flatPathsToAdd.isEmpty()) {
        QList<JB::WatchRequest> requestsToRemove;
        for (auto it = recursivePathsToRemove.begin(); it != recursivePathsToRemove.end(); ++it) {
            requestsToRemove.append(JB::WatchRequest(*it, true));
        }
        for (auto it = flatPathsToRemove.begin(); it != flatPathsToRemove.end(); ++it) {
            requestsToRemove.append(JB::WatchRequest(*it, false));
        }
        rootsNeedUpdate = fs->replaceWatchedRoots(requestsToRemove, SetToList(recursivePathsToAdd),
                                                  SetToList(flatPathsToAdd));
        clearCachedPaths();
    }
    hasChangeEvent = false;
}

bool FileSystemNotifierPrivate::waitForPathsSet(int msecs) {
    if (!hasChangeEvent) {
        return true;
    }
    commitChange(); // if roots need update, signal must be sent to watcher and d

    if (!rootsNeedUpdate) {
        return true;
    }

    QElapsedTimer stopWatch;
    stopWatch.start();

    // Wait for sending roots command to fsnotifier
    while (!fs->fileWatcher()->isSendingRoots()) {
        if (stopWatch.elapsed() > msecs) {
            return false;
        }
    }
    // Wait for fsnotifier to feedback
    while (fs->fileWatcher()->isSettingRoots()) {
        if (stopWatch.elapsed() > msecs) {
            return false;
        }
    }
    return true;
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

QStringList FileSystemNotifierPrivate::listPathFromNativeSeparators(const QStringList &paths) {
    QStringList res;
    for (auto it = paths.begin(); it != paths.end(); ++it) {
        res.append(QDir::fromNativeSeparators(*it));
    }
    return res;
}
