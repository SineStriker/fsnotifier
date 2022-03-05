#include "JBLocalFileSystem.h"
#include "JBNativeFileWatcher.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimerEvent>

JBLocalFileSystem::JBLocalFileSystem(QObject *parent) : QObject(parent) {
    Q_ASSERT(!self);
    self = this;

    myDisposed = true;
    myAfterMarkDirtyCallback = -1;

    myWatcher = new JBFileWatcher(this);
    myWatchRootsManager = new JBWatchRootsManager(myWatcher, this);

    JBNativeFileWatcher::createWatchers(1);
}

JBLocalFileSystem::~JBLocalFileSystem() {
    if (!myDisposed) {
        dispose();
    }
    JBNativeFileWatcher::destroyWatchers();
}

void JBLocalFileSystem::start() {
    myWatchRootsManager->clear();
    myDisposed = false;

    myWatcher->start();
    myAfterMarkDirtyCallback = startTimer(500);
}

void JBLocalFileSystem::dispose() {
    killTimer(myAfterMarkDirtyCallback);
    myAfterMarkDirtyCallback = -1;

    myWatcher->dispose();
    myDisposed = true;
}

JBFileWatcher *JBLocalFileSystem::fileWatcher() const {
    return myWatcher;
}

QList<JBLocalFileSystem::WatchRequest>
    JBLocalFileSystem::replaceWatchedRoots(const QList<WatchRequest> &watchRequestsToRemove,
                                           const QStringList &recursiveRootsToAdd,
                                           const QStringList &flatRootsToAdd) {
    if (myDisposed) {
        return {};
    }
    return myWatchRootsManager->replaceWatchedRoots(watchRequestsToRemove, recursiveRootsToAdd,
                                                    flatRootsToAdd);
}

bool JBLocalFileSystem::storeRefreshStatusToFiles() {
    if (myWatcher->isOperational()) {
        auto dirtyPaths = myWatcher->getDirtyPaths();
        markPathsDirty(dirtyPaths.dirtyPaths);
        markFlatDirsDirty(dirtyPaths.dirtyDirectories);
        markRecursiveDirsDirty(dirtyPaths.dirtyPathsRecursive);
        return !dirtyPaths.dirtyPaths.isEmpty() || !dirtyPaths.dirtyDirectories.isEmpty() ||
               !dirtyPaths.dirtyPathsRecursive.isEmpty();
    }
    return false;
}

void JBLocalFileSystem::markPathsDirty(const QSet<QString> &dirtyPaths) {
    if (!dirtyPaths.isEmpty()) {
        emit pathsDirty(JBFileWatcherUtils::SetToList(dirtyPaths));
    }
}

void JBLocalFileSystem::markFlatDirsDirty(const QSet<QString> &dirtyPaths) {
    if (!dirtyPaths.isEmpty()) {
        emit flatDirsDirty(JBFileWatcherUtils::SetToList(dirtyPaths));
    }
}

void JBLocalFileSystem::markRecursiveDirsDirty(const QSet<QString> &dirtyPaths) {
    if (!dirtyPaths.isEmpty()) {
        emit recursiveDirsDirty(JBFileWatcherUtils::SetToList(dirtyPaths));
    }
}

void JBLocalFileSystem::markSuspiciousFilesDirty(const QSet<QString> &paths) {
    // storeRefreshStatusToFiles();
}

void JBLocalFileSystem::timerEvent(QTimerEvent *event) {
    if (event->timerId() == myAfterMarkDirtyCallback) {
        storeRefreshStatusToFiles();
    }
}

JBLocalFileSystem *JBLocalFileSystem::self = nullptr;
JBLocalFileSystem *JBLocalFileSystem::instance() {
    if (!self) {
        qDebug() << "Auto create JetBrains Local File System instance.";
        self = new JBLocalFileSystem(qApp);
    }
    return self;
}
