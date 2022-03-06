#include "JBFileWatcher.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

JBFileWatcher::JBFileWatcher(QObject *parent) : QObject(parent) {
    init();
}

JBFileWatcher::~JBFileWatcher() {
}

void JBFileWatcher::init() {
    myNotificationSink.reset(new JBFileWatcherAdvancedNSink(this));
}

void JBFileWatcher::start() {
    clear();
    const auto &watchers = JBNativeFileWatcher::watchers();
    for (auto it = watchers.begin(); it != watchers.end(); ++it) {
        auto watcher = *it;
        watcher->initialize(myNotificationSink.data());
    }
}

void JBFileWatcher::dispose() {
    const auto &watchers = JBNativeFileWatcher::watchers();
    for (auto it = watchers.begin(); it != watchers.end(); ++it) {
        auto watcher = *it;
        watcher->dispose();
    }
}

bool JBFileWatcher::isOperational() const {
    const auto &watchers = JBNativeFileWatcher::watchers();
    for (auto it = watchers.begin(); it != watchers.end(); ++it) {
        auto watcher = *it;
        if (watcher->isOperational()) {
            return true;
        }
    }
    return false;
}

bool JBFileWatcher::isSettingRoots() const {
    const auto &watchers = JBNativeFileWatcher::watchers();
    for (auto it = watchers.begin(); it != watchers.end(); ++it) {
        auto watcher = *it;
        if (watcher->isSettingRoots()) {
            return true;
        }
    }
    return false;
}

JBFileWatcherDirtyPaths JBFileWatcher::getDirtyPaths() const {
    return myNotificationSink->getDirtyPaths();
}

QStringList JBFileWatcher::getManualWatchRoots() const {
    QSet<QString> result;
    for (auto it = myManualWatchRoots.begin(); it != myManualWatchRoots.end(); ++it) {
        result.unite(it.value());
    }
    return result.values();
}

QStringList JBFileWatcher::mapToAllSymlinks(const QString &reportedPath) const {
    auto result = myPathMap.mapToOriginalWatchRoots(reportedPath, true);
    if (!result.isEmpty()) {
        result.remove(reportedPath);
    }
    return result.values();
}

bool JBFileWatcher::belongsToWatchRoots(const QString &reportedPath, bool isFile) const {
    return myPathMap.belongsToWatchRoots(reportedPath, isFile);
}

void JBFileWatcher::setWatchRoots(const JBCanonicalPathMap &pathMap) {
    myPathMap = pathMap;
    myManualWatchRoots.clear();

    auto roots = myPathMap.getCanonicalWatchRoots();
    const auto &watchers = JBNativeFileWatcher::watchers();
    for (auto it = watchers.begin(); it != watchers.end(); ++it) {
        auto watcher = *it;
        if (watcher->isOperational()) {
            watcher->setWatchRoots(roots.first, roots.second);
        }
    }
}

void JBFileWatcher::notifyOnFailure(const QString &reason) {
    if (reason == "watcher.exe.not.found") {
        jbWarning() << "[Watcher] Fsnotifier not found.";
    } else if (reason == "watcher.exe.not.exe") {
        jbWarning() << "[Watcher] Fsnotifier is not executable.";
    } else if (reason == "watcher.failed.to.start") {
        jbWarning() << "[Watcher] Fsnotifier failed to start.";
    } else if (reason == "watcher.gave.up") {
        jbWarning() << "[Watcher] Fsnotifier gave up.";
    } else if (reason == "watcher.bailed.out.10x") {
        jbWarning() << "[Watcher] Fsnotifier failed for more than 10 times.";
    }
}

void JBFileWatcher::clear() {
    myPathMap = JBCanonicalPathMap();
    myManualWatchRoots.clear();
}
