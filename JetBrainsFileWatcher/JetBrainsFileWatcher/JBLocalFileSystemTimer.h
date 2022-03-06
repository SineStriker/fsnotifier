#ifndef JBFILEWATCHDIRTYCHECKER_H
#define JBFILEWATCHDIRTYCHECKER_H

#include <QMutex>
#include <QObject>
#include <QThread>

class JBLocalFileSystem;
class JBFileWatcherDirtyPaths;

class JBLocalFileSystemTimer : public QObject {
    Q_OBJECT
public:
    explicit JBLocalFileSystemTimer(JBLocalFileSystem *fs);
    ~JBLocalFileSystemTimer();

public:
    void start();
    void stop();

    bool isRunning() const;

    bool storeRefreshStatusToFiles();

private:
    void handleThreadStart();
    void handleThreadFinish();

    void markPathsDirty(const QStringList &paths);
    void markFlatDirsDirty(const QStringList &paths);
    void markRecursiveDirsDirty(const QStringList &paths);

protected:
    JBLocalFileSystem *fs;

    int timerId;

    QStringList lastDirtyPaths;
    QStringList lastDirtyFlatDirs;
    QStringList lastDirtyRecursiveDirs;

    QThread *taskThread;
    QScopedPointer<QMutex> lock;

    void timerEvent(QTimerEvent *event) override;

signals:
    void afterMarkDirtyCallback(const QStringList &paths, const QStringList &flatDirs,
                                const QStringList &recursiveDirs);
};

#endif // JBFILEWATCHDIRTYCHECKER_H
