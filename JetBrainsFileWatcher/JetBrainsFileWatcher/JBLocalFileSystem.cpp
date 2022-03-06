#include "JBLocalFileSystem.h"
#include "JBLocalFileSystemTimer.h"
#include "JBNativeFileWatcher.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimerEvent>

JBLocalFileSystem::JBLocalFileSystem(QObject *parent)
    : QObject(parent), myTimer(new JBLocalFileSystemTimer(this)) {
    Q_ASSERT(!self);
    self = this;

    jbDebug() << "[Local FS] Local File System init";

    myDisposed = true;

    myWatcher = new JBFileWatcher(this);
    myWatchRootsManager = new JBWatchRootsManager(myWatcher, this);

    JBNativeFileWatcher::createWatchers(1);

    // Callback
    connect(myTimer.data(), &JBLocalFileSystemTimer::afterMarkDirtyCallback, this,
            &JBLocalFileSystem::handleAfterMarkDirtyCallback);

    // Stop timer before app quit
    connect(qApp, &QCoreApplication::aboutToQuit, this, &JBLocalFileSystem::dispose);
}

JBLocalFileSystem::~JBLocalFileSystem() {
    if (!myDisposed) {
        dispose();
    }
    JBNativeFileWatcher::destroyWatchers();

    jbDebug() << "[Local FS] Local File System quit";

    self = nullptr;
}

void JBLocalFileSystem::start() {
    if (myDisposed) {
        myWatchRootsManager->clear();
        myDisposed = false;

        myWatcher->start();
        myTimer->start();
    }
}

void JBLocalFileSystem::dispose() {
    if (!myDisposed) {
        myTimer->stop();

        myWatcher->dispose();
        myDisposed = true;
    }
}

bool JBLocalFileSystem::disposed() const {
    return myDisposed;
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
    return JBFileWatcherUtils::SetToList(myWatchRootsManager->replaceWatchedRoots(
        watchRequestsToRemove, recursiveRootsToAdd, flatRootsToAdd));
}

QList<JBLocalFileSystem::WatchRequest> JBLocalFileSystem::currentWatchedRoots() const {
    if (myDisposed) {
        return {};
    }
    return JBFileWatcherUtils::SetToList(myWatchRootsManager->currentWatchRequests());
}

void JBLocalFileSystem::markSuspiciousFilesDirty(const QStringList &paths) {
    Q_UNUSED(paths)
    storeRefreshStatusToFiles();
}

bool JBLocalFileSystem::storeRefreshStatusToFiles() {
    return myTimer->storeRefreshStatusToFiles();
}

void JBLocalFileSystem::handleAfterMarkDirtyCallback(const QStringList &paths,
                                                     const QStringList &flatDirs,
                                                     const QStringList &recursiveDirs) {
    paths.isEmpty() ? void() : emit pathsDirty(paths);
    flatDirs.isEmpty() ? void() : emit flatDirsDirty(flatDirs);
    recursiveDirs.isEmpty() ? void() : emit recursivePathsDirty(recursiveDirs);
}

JBLocalFileSystem *JBLocalFileSystem::self = nullptr;

JBLocalFileSystem *JBLocalFileSystem::instance() {
    if (!self) {
        jbDebug() << "[Local FS] Auto create JetBrains Local File System instance";
        new JBLocalFileSystem(qApp);
    }
    return self;
}
