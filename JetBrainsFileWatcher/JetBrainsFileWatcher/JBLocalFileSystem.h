#ifndef JBLOCALFILESYSTEM_H
#define JBLOCALFILESYSTEM_H

#include "JBFileWatcher.h"
#include "JBWatchRootsManager.h"

#define JBFS JBLocalFileSystem::instance()

class JBLocalFileSystem : public QObject {
    Q_OBJECT
public:
    explicit JBLocalFileSystem(QObject *parent = nullptr);
    ~JBLocalFileSystem();

    typedef JBFileWatchRequest WatchRequest;

public:
    void start();
    void dispose();

    JBFileWatcher *fileWatcher() const;
    QList<WatchRequest> replaceWatchedRoots(const QList<WatchRequest> &watchRequestsToRemove,
                                            const QStringList &recursiveRootsToAdd,
                                            const QStringList &flatRootsToAdd);
    QList<WatchRequest> currentWatchedRoots() const;

private:
    JBFileWatcher *myWatcher;
    JBWatchRootsManager *myWatchRootsManager;

    int myAfterMarkDirtyCallback;
    bool myDisposed;

    bool storeRefreshStatusToFiles();

private:
    void markPathsDirty(const QStringList &dirtyPaths);
    void markFlatDirsDirty(const QStringList &dirtyPaths);
    void markRecursiveDirsDirty(const QStringList &dirtyPaths);

public:
    void markSuspiciousFilesDirty(const QStringList &paths);

protected:
    void timerEvent(QTimerEvent *event) override;

    // Singleton
public:
    static JBLocalFileSystem *instance();
    static JBLocalFileSystem *destruct();

private:
    static JBLocalFileSystem *self;

signals:
    void pathsDirty(const QStringList &dirtyPaths);
    void flatDirsDirty(const QStringList &dirtyPaths);
    void recursivePathsDirty(const QStringList &dirtyPaths);
};

#endif // JBLOCALFILESYSTEM_H
