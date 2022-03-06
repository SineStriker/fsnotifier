#include "JBLocalFileSystemTimer.h"
#include "JBLocalFileSystem.h"

#include <QTimerEvent>

JBLocalFileSystemTimer::JBLocalFileSystemTimer(JBLocalFileSystem *fs)
    : QObject(nullptr), fs(fs), lock(new QMutex()) {
    timerId = -1;

    // New thread
    taskThread = new QThread(fs);
    moveToThread(taskThread);

    connect(taskThread, &QThread::started, this, &JBLocalFileSystemTimer::handleThreadStart);
    connect(taskThread, &QThread::finished, this, &JBLocalFileSystemTimer::handleThreadFinish);
}

JBLocalFileSystemTimer::~JBLocalFileSystemTimer() {
    stop();
}

void JBLocalFileSystemTimer::start() {
    // Run in host thread
    if (!taskThread->isRunning()) {
        taskThread->start();
    }
}

void JBLocalFileSystemTimer::stop() {
    // Run in host thread
    if (taskThread->isRunning()) {
        taskThread->quit();
        taskThread->wait();
    }
}

bool JBLocalFileSystemTimer::isRunning() const {
    return taskThread->isRunning();
}

bool JBLocalFileSystemTimer::storeRefreshStatusToFiles() {
    // Run in any thread
    QMutexLocker locker(lock.data());

    auto watcher = fs->fileWatcher();
    if (watcher->isOperational()) {
        auto dirtyPaths = watcher->getDirtyPaths();

        markPathsDirty(dirtyPaths.dirtyPaths());
        markFlatDirsDirty(dirtyPaths.dirtyDirectories());
        markRecursiveDirsDirty(dirtyPaths.dirtyPathsRecursive());

        return !dirtyPaths.isEmpty();
    }
    return false;
}

void JBLocalFileSystemTimer::handleThreadStart() {
    // Run in timer thread
    if (timerId < 0) {
        timerId = startTimer(500);
    }
}

void JBLocalFileSystemTimer::handleThreadFinish() {
    // Run in timer thread
    if (timerId >= 0) {
        killTimer(timerId);
        timerId = -1;
    }
}

void JBLocalFileSystemTimer::markPathsDirty(const QStringList &paths) {
    lastDirtyPaths = paths;
}

void JBLocalFileSystemTimer::markFlatDirsDirty(const QStringList &paths) {
    lastDirtyFlatDirs = paths;
}

void JBLocalFileSystemTimer::markRecursiveDirsDirty(const QStringList &paths) {
    lastDirtyRecursiveDirs = paths;
}

void JBLocalFileSystemTimer::timerEvent(QTimerEvent *event) {
    if (event->timerId() == timerId) {
        if (storeRefreshStatusToFiles()) {
            emit afterMarkDirtyCallback(lastDirtyPaths, lastDirtyFlatDirs, lastDirtyRecursiveDirs);
        }
    }
}
