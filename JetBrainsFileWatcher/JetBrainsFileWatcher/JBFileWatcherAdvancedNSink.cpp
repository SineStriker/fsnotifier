#include "JBFileWatcherAdvancedNSink.h"
#include "JBFileWatcher.h"
#include "JBPluggableFileWatcher.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

JBFileWatcherAdvancedNSink::JBFileWatcherAdvancedNSink(JBFileWatcher *watcher)
    : myWatcher(watcher) {
    Q_ASSERT(watcher);
}

JBFileWatcherAdvancedNSink::~JBFileWatcherAdvancedNSink() {
}

JBFileWatcherAdvancedNSink::DirtyPaths JBFileWatcherAdvancedNSink::getDirtyPaths() {
    DirtyPaths dirtyPaths;
    if (!myDirtyPaths.isEmpty()) {
        dirtyPaths = myDirtyPaths;
        myDirtyPaths.clear();
    }

    const auto &watchers = JBNativeFileWatcher::watchers();
    for (auto it = watchers.begin(); it != watchers.end(); ++it) {
        auto watcher = *it;
        watcher->resetChangedPaths();
    }

    return dirtyPaths;
}

void JBFileWatcherAdvancedNSink::notifyManualWatchRoots(JBPluggableFileWatcher *watcher,
                                                        const QStringList &roots) {
    registerManualWatchRoots(watcher, QSet<QString>(roots.begin(), roots.end()));
}

void JBFileWatcherAdvancedNSink::notifyMapping(const QList<QPair<QString, QString>> &mapping) {
    if (!mapping.isEmpty()) {
        myWatcher->myPathMap.addMapping(mapping);
    }
    notifyOnEvent(OTHER);
}

void JBFileWatcherAdvancedNSink::notifyDirtyPath(const QString &path) {
    auto paths = myWatcher->myPathMap.mapToOriginalWatchRoots(path, true);
    if (!paths.isEmpty()) {
        for (auto it = paths.begin(); it != paths.end(); ++it) {
            myDirtyPaths.addDirtyPath(*it);
        }
    }
    notifyOnEvent(path);
}

void JBFileWatcherAdvancedNSink::notifyPathCreatedOrDeleted(const QString &path) {
    auto paths = myWatcher->myPathMap.mapToOriginalWatchRoots(path, true);
    if (!paths.isEmpty()) {
        for (auto it = paths.begin(); it != paths.end(); ++it) {
            const QString &p = *it;
            myDirtyPaths.addDirtyPathRecursive(p);
            QString parentPath = QFileInfo(p).absolutePath();
            if (!parentPath.isEmpty()) {
                myDirtyPaths.addDirtyPath(parentPath);
            }
        }
    }
    notifyOnEvent(path);
}

void JBFileWatcherAdvancedNSink::notifyDirtyDirectory(const QString &path) {
    auto paths = myWatcher->myPathMap.mapToOriginalWatchRoots(path, false);
    if (!paths.isEmpty()) {
        myDirtyPaths.addDirtyDirs(paths);
    }
    notifyOnEvent(path);
}

void JBFileWatcherAdvancedNSink::notifyDirtyPathRecursive(const QString &path) {
    auto paths = myWatcher->myPathMap.mapToOriginalWatchRoots(path, false);
    if (!paths.isEmpty()) {
        for (auto it = paths.begin(); it != paths.end(); ++it) {
            myDirtyPaths.addDirtyPathRecursive(*it);
        }
    }
    notifyOnEvent(path);
}

void JBFileWatcherAdvancedNSink::notifyReset(const QString &path) {
    if (!path.isEmpty()) {
        myDirtyPaths.addDirtyPathRecursive(path);
    } else {
        QFileInfoList roots = QDir::drives();
        for (auto it = roots.begin(); it != roots.end(); ++it) {
            auto root = *it;
            myDirtyPaths.addDirtyPathRecursive(root.absoluteFilePath());
        }
    }
    notifyOnEvent(RESET);
}

void JBFileWatcherAdvancedNSink::notifyUserOnFailure(const QString &reason) {
    myWatcher->notifyOnFailure(reason);
}

void JBFileWatcherAdvancedNSink::registerManualWatchRoots(QObject *key,
                                                          const QSet<QString> &roots) {
    auto it = myManualWatchRoots.find(key);
    if (it == myManualWatchRoots.end()) {
        it = myManualWatchRoots.insert(key, {});
    }
    auto &rootSet = it.value();
    rootSet.unite(roots);
    notifyOnEvent(OTHER);
}

void JBFileWatcherAdvancedNSink::notifyOnEvent(const QString &path) {
    Q_UNUSED(path)
}
