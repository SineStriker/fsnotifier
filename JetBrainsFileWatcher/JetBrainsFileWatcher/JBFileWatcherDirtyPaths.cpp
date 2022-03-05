#include "JBFileWatcherDirtyPaths.h"

JBFileWatcherDirtyPaths::JBFileWatcherDirtyPaths() {
}

JBFileWatcherDirtyPaths::~JBFileWatcherDirtyPaths() {
}

bool JBFileWatcherDirtyPaths::isEmpty() {
    return dirtyPaths.isEmpty() && dirtyPathsRecursive.isEmpty() && dirtyDirectories.isEmpty();
}

void JBFileWatcherDirtyPaths::clear() {
    dirtyPaths.clear();
    dirtyPathsRecursive.clear();
    dirtyDirectories.clear();
}

void JBFileWatcherDirtyPaths::addDirtyPath(const QString &path) {
    if (!dirtyPathsRecursive.contains(path)) {
        dirtyPaths.insert(path);
    }
}

void JBFileWatcherDirtyPaths::addDirtyPathRecursive(const QString &path) {
    dirtyPaths.remove(path);
    dirtyPathsRecursive.insert(path);
}
