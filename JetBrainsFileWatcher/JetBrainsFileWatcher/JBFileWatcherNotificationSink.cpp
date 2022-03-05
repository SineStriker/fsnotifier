#include "JBFileWatcherNotificationSink.h"
#include "JBPluggableFileWatcher.h"
#include "JBFileWatcherUtils.h"

JBFileWatcherNotificationSink::JBFileWatcherNotificationSink() {
}

JBFileWatcherNotificationSink::~JBFileWatcherNotificationSink() {
}

void JBFileWatcherNotificationSink::notifyManualWatchRoots(JBPluggableFileWatcher *watcher,
                                                           const QStringList &roots) {
}

void JBFileWatcherNotificationSink::notifyMapping(const QList<QPair<QString, QString>> &mapping) {
}

void JBFileWatcherNotificationSink::notifyDirtyPath(const QString &path) {
    jbDebug() << "Path Changed:" << path;
}

void JBFileWatcherNotificationSink::notifyPathCreatedOrDeleted(const QString &path) {
    jbDebug() << "Path Create Or Deleted:" << path;
}

void JBFileWatcherNotificationSink::notifyDirtyDirectory(const QString &path) {
    jbDebug() << "Dir Create Or Deleted:" << path;
}

void JBFileWatcherNotificationSink::notifyDirtyPathRecursive(const QString &path) {
    jbDebug() << "Dir Changed Rec:" << path;
}

void JBFileWatcherNotificationSink::notifyReset(const QString &path) {
    jbDebug() << "Path Reset:" << path;
}

void JBFileWatcherNotificationSink::notifyUserOnFailure(const QString &reason) {
    jbDebug() << "Failure occured:" << reason;
}
