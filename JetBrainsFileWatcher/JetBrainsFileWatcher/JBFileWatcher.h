#ifndef JBFILEWATCHER_H
#define JBFILEWATCHER_H

#include "JBCanonicalPathMap.h"
#include "JBFileWatcherAdvancedNSink.h"

class JBFileWatcher : public QObject {
    Q_OBJECT
public:
    explicit JBFileWatcher(QObject *parent = nullptr);
    ~JBFileWatcher();

    friend class JBFileWatcherAdvancedNSink;

private:
    void init();

public:
    void start();
    void dispose();

public:
    bool isOperational() const;
    bool isSettingRoots() const;

    JBFileWatcherDirtyPaths getDirtyPaths() const;
    QStringList getManualWatchRoots() const;
    QStringList mapToAllSymlinks(const QString &reportedPath) const;

    bool belongsToWatchRoots(const QString &reportedPath, bool isFile) const;

    void setWatchRoots(const JBCanonicalPathMap &pathMap);

    void notifyOnFailure(const QString &reason);

private:
    JBCanonicalPathMap myPathMap;
    QMap<QObject *, QSet<QString>> myManualWatchRoots;

    QScopedPointer<JBFileWatcherAdvancedNSink> myNotificationSink;

    void clear();
};

#endif // JBFILEWATCHER_H
