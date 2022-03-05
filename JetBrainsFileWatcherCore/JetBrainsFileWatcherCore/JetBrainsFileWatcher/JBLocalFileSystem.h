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

private:
    JBFileWatcher *myWatcher;
    JBWatchRootsManager *myWatchRootsManager;

    int myAfterMarkDirtyCallback;
    bool myDisposed;

    bool storeRefreshStatusToFiles();

private:
    void markPathsDirty(const QSet<QString> &dirtyPaths);
    void markFlatDirsDirty(const QSet<QString> &dirtyPaths);
    void markRecursiveDirsDirty(const QSet<QString> &dirtyPaths);

public:
    void markSuspiciousFilesDirty(const QSet<QString> &paths);

protected:
    void timerEvent(QTimerEvent *event) override;

    // Singleton
public:
    static JBLocalFileSystem *instance();

private:
    static JBLocalFileSystem *self;

signals:
    void pathsDirty(const QStringList &dirtyPaths);
    void flatDirsDirty(const QStringList &dirtyPaths);
    void recursiveDirsDirty(const QStringList &dirtyPaths);
};

#endif // JBLOCALFILESYSTEM_H
