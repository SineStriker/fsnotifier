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
    jbDebug() << "[Notify] Path Changed:" << path;
}

void JBFileWatcherNotificationSink::notifyPathCreatedOrDeleted(const QString &path) {
    jbDebug() << "[Notify] Path Create Or Deleted:" << path;
}

void JBFileWatcherNotificationSink::notifyDirtyDirectory(const QString &path) {
    jbDebug() << "[Notify] Dir Create Or Deleted:" << path;
}

void JBFileWatcherNotificationSink::notifyDirtyPathRecursive(const QString &path) {
    jbDebug() << "[Notify] Dir Changed Rec:" << path;
}

void JBFileWatcherNotificationSink::notifyReset(const QString &path) {
    jbDebug() << "[Notify] Path Reset:" << path;
}

void JBFileWatcherNotificationSink::notifyUserOnFailure(const QString &reason) {
    jbDebug() << "[Notify] Failure occured:" << reason;
}
