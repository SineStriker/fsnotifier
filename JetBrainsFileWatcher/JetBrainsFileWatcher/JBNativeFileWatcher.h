#ifndef JBNATIVEFILEWATCHER_H
#define JBNATIVEFILEWATCHER_H

#include <QMutex>
#include <QObject>
#include <QPointer>
#include <QProcess>

#include "JBFileWatcherUtils.h"
#include "JBPluggableFileWatcher.h"

/* FSNotifier process handler */

class JBNativeFileWatcher : public JBPluggableFileWatcher {
    Q_OBJECT
public:
    explicit JBNativeFileWatcher(QObject *parent = nullptr);
    ~JBNativeFileWatcher();

private:
    JBFileWatcherNotificationSink *myNotificationSink;

    QString myExecutable;

    QAtomicInt myStartAttemptCount;
    bool myIsShuttingDown;
    QAtomicInt mySettingRoots;

    QStringList myRecursiveWatchRoots;
    QStringList myFlatWatchRoots;
    QStringList myIgnoredRoots;

    QVector<QString> myLastChangedPaths;
    int myLastChangedPathIndex;

    JBFileWatcherUtils::WatcherOp myLastOp;
    QStringList myLines;

    const QString ROOTS_COMMAND = "ROOTS";
    const QString EXIT_COMMAND = "EXIT";
    const int MAX_PROCESS_LAUNCH_ATTEMPT_COUNT = 10;

    QScopedPointer<QMutex> myLastChangedPathsLock;

public:
    void initialize(JBFileWatcherNotificationSink *sink);
    void dispose();

    bool isSettingRoots() const;
    void setWatchRoots(const QStringList &recursive, const QStringList &flat);

protected:
    virtual QString executable() const;

public:
    static QString FSNotifierExecutable();

    virtual void resetChangedPaths();

private:
    bool startupProcess(bool restart = false);
    bool shutdownProcess();

    void setWatchRootsCore(QStringList recursive, QStringList flat, bool restart);

    static QStringList screenUncRoots(const QStringList &roots, QStringList &ignored);

protected:
    bool isRepetition(const QString &path);

    void notifyProcessTerminated(int exitCode, QProcess::ExitStatus exitStatus) override;
    void notifyTextAvailable(const QString &line, QProcess::ProcessChannel channel) override;
    void notifyErrorOccured(QProcess::ProcessError error) override;
    void notifyOnFailure(const QString &reason) override;

private:
    void processRemap();
    void processUnwatchable();
    void processChange(const QString &path, JBFileWatcherUtils::WatcherOp op);

public:
    static void createWatchers(int n);
    static void destroyWatchers();

    static QList<JBNativeFileWatcher *> watchers();

private:
    static QList<JBNativeFileWatcher *> s_watchers;
};

#endif // JBNATIVEFILEWATCHER_H
