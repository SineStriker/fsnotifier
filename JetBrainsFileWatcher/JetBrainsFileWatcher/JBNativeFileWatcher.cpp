#include "JBNativeFileWatcher.h"
#include "JBFileWatcher.h"
#include "JBFileWatcherNotificationSink.h"
#include "JBLocalFileSystem.h"

#include <QApplication>
#include <QFileInfo>

typedef JBFileWatcherUtils::WatcherOp WatcherOp;

JBNativeFileWatcher::JBNativeFileWatcher(QObject *parent) : JBPluggableFileWatcher(parent) {
    Q_ASSERT(JBFS);
    myNotificationSink = nullptr;
    myLastChangedPaths.resize(2);
}

JBNativeFileWatcher::~JBNativeFileWatcher() {
    if (myProcess->state() == QProcess::Running) {
        dispose();
    }
}

void JBNativeFileWatcher::initialize(JBFileWatcherNotificationSink *sink) {
    Q_ASSERT(sink);

    myNotificationSink = sink;
    myExecutable = executable();

    myIsShuttingDown = false;
    myStartAttemptCount = 0;
    myIsShuttingDown = false;
    mySettingRoots = 0;
    myRecursiveWatchRoots.clear();
    myFlatWatchRoots.clear();
    myIgnoredRoots.clear();
    resetChangedPaths();
    myLastOp = WatcherOp::UNKNOWN;
    myLines.clear();

    QFileInfo info(myExecutable);
    if (!info.isFile()) {
        notifyOnFailure("watcher.exe.not.found");
    } else if (!info.isExecutable()) {
        notifyOnFailure("watcher.exe.not.exe");
    } else if (!startupProcess()) {
        notifyOnFailure("watcher.failed.to.start");
    }
}

void JBNativeFileWatcher::dispose() {
    myIsShuttingDown = true;
    shutdownProcess();
}

bool JBNativeFileWatcher::isSettingRoots() const {
    return isOperational() && mySettingRoots > 0;
}

void JBNativeFileWatcher::setWatchRoots(const QStringList &recursive, const QStringList &flat) {
    setWatchRootsCore(recursive, flat, false);
}

bool JBNativeFileWatcher::startupProcess(bool restart) {
    if (myIsShuttingDown) {
        return true;
    }

    if (myStartAttemptCount++ > MAX_PROCESS_LAUNCH_ATTEMPT_COUNT) {
        notifyOnFailure("watcher.bailed.out.10x");
        return false;
    }

    if (restart && !shutdownProcess()) {
        return false;
    }

    jbDebug() << "Starting file watcher:" << myExecutable;

    if (!startProcess(myExecutable)) {
        return false;
    }

    if (restart) {
        if (!myRecursiveWatchRoots.isEmpty() && !myFlatWatchRoots.isEmpty()) {
            setWatchRootsCore(myRecursiveWatchRoots, myFlatWatchRoots, true);
        }
    }

    return true;
}

bool JBNativeFileWatcher::shutdownProcess() {
    if (myProcess->state() == QProcess::Running) {
        if (!writeLine(EXIT_COMMAND)) {
            return false;
        }
        if (!myProcess->waitForFinished(500)) {
            jbDebug() << "File watcher is still alive, doing a force quit.";
            if (!killProcess()) {
                return false;
            }
        }
    }
    return true;
}

void JBNativeFileWatcher::setWatchRootsCore(QStringList recursive, QStringList flat, bool restart) {
    if (!restart && myRecursiveWatchRoots == recursive && myFlatWatchRoots == flat) {
        myNotificationSink->notifyManualWatchRoots(this, myIgnoredRoots);
        return;
    }

    mySettingRoots++;
    myRecursiveWatchRoots = recursive;
    myFlatWatchRoots = flat;

    QStringList ignored;

#ifdef Q_OS_WINDOWS
    recursive = screenUncRoots(recursive, ignored);
    flat = screenUncRoots(flat, ignored);
#endif

    myIgnoredRoots = ignored;
    myNotificationSink->notifyManualWatchRoots(this, ignored);

    bool flag = true;
    if (writeLine(ROOTS_COMMAND)) {
        for (auto it = recursive.begin(); it != recursive.end(); ++it) {
            const QString &path = *it;
            if (!writeLine(path)) {
                flag = false;
                break;
            }
        }
        if (flag) {
            for (auto it = flat.begin(); it != flat.end(); ++it) {
                const QString &path = *it;
                if (!writeLine('|' + path)) {
                    flag = false;
                    break;
                }
            }
        }
    }
    if (flag) {
        flag &= writeLine("#");
    }
    if (!flag) {
        jbDebug() << "Error setting roots.";
    }
}

QStringList JBNativeFileWatcher::screenUncRoots(const QStringList &roots, QStringList &ignored) {
    QStringList filtered;
    for (auto it = roots.begin(); it != roots.end(); ++it) {
        const QString &root = *it;
        if (JBFileWatcherUtils::OSAgnosticPathUtil::isUncPath(root)) {
            ignored.append(root);
        } else {
            filtered.append(root);
        }
    }
    return filtered;
}

QString JBNativeFileWatcher::executable() const {
    return FSNotifierExecutable();
}

QString JBNativeFileWatcher::FSNotifierExecutable() {
    QString name;
#ifdef Q_OS_WINDOWS
    name = "fsnotifier.exe";
#else
    name = "fsnotifier";
#endif
    return name;
}

void JBNativeFileWatcher::resetChangedPaths() {
    myLastChangedPathIndex = 0;
    myLastChangedPaths[0] = "";
    myLastChangedPaths[1] = "";
}

bool JBNativeFileWatcher::isRepetition(const QString &path) {
    const int length = myLastChangedPaths.size();
    for (int i = 0; i < length; ++i) {
        int last = myLastChangedPathIndex - i - 1;
        if (last < 0) {
            last += length;
        }
        const QString &lastChangedPath = myLastChangedPaths[last];
        if (!lastChangedPath.isEmpty() && !lastChangedPath.compare(path)) {
            return true;
        }
    }

    myLastChangedPaths[myLastChangedPathIndex++] = path;
    if (myLastChangedPathIndex == length) {
        myLastChangedPathIndex = 0;
    }

    return false;
}

void JBNativeFileWatcher::notifyProcessTerminated(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitStatus)

    jbDebug() << "Watcher terminated with exit code" << exitCode;

    if (!startupProcess(true)) {
        shutdownProcess();
        jbDebug()
            << "Watcher terminated and attempt to restart has failed. Exiting watching thread.";
    }
}

void JBNativeFileWatcher::notifyTextAvailable(const QString &line,
                                              QProcess::ProcessChannel channel) {
    if (channel == QProcess::StandardError) {
        jbDebug() << line;
        return;
    }

    if (myLastOp == WatcherOp::UNKNOWN) {
        WatcherOp watcherOp = JBFileWatcherUtils::StringToWatcherOp(line);
        if (watcherOp == WatcherOp::UNKNOWN) {
            jbDebug() << "Illegal watcher command: '" + line + "'";
            return;
        }
        if (watcherOp == WatcherOp::GIVEUP) {
            notifyOnFailure("watcher.gave.up");
            myIsShuttingDown = true;
        } else if (watcherOp == WatcherOp::RESET) {
            myNotificationSink->notifyReset("");
        } else {
            myLastOp = watcherOp;
        }
    } else if (myLastOp == WatcherOp::MESSAGE) {
        notifyOnFailure(JBFileWatcherUtils::MessageToFailureReasonString(line));
        myLastOp = WatcherOp::UNKNOWN;
    } else if (myLastOp == WatcherOp::REMAP || myLastOp == WatcherOp::UNWATCHEABLE) {
        if (line == '#') {
            if (myLastOp == WatcherOp::REMAP) {
                processRemap();
            } else {
                mySettingRoots--;
                processUnwatchable();
            }
            myLines.clear();
            myLastOp = WatcherOp::UNKNOWN;
        } else {
            myLines.append(line);
        }
    } else {
        QString path = line.simplified();
        processChange(path, myLastOp);
        myLastOp = WatcherOp::UNKNOWN;
    }
}

void JBNativeFileWatcher::notifyErrorOccured(QProcess::ProcessError error) {
    Q_UNUSED(error)
}

void JBNativeFileWatcher::notifyOnFailure(const QString &reason) {
    myNotificationSink->notifyUserOnFailure(reason);
}

void JBNativeFileWatcher::processRemap() {
    QList<QPair<QString, QString>> map;
    for (int i = 0; i < myLines.size() - 1; i += 2) {
        map.append({myLines.at(i), myLines.at(i + 1)});
    }
    myNotificationSink->notifyMapping(map);
}

void JBNativeFileWatcher::processUnwatchable() {
    myIgnoredRoots.append(myLines);
    myNotificationSink->notifyManualWatchRoots(this, myLines);
}

void JBNativeFileWatcher::processChange(const QString &path, WatcherOp op) {
#ifdef Q_OS_WINDOWS
    if (op == WatcherOp::RECDIRTY) {
        myNotificationSink->notifyReset(path);
        return;
    }
#endif

    if ((op == WatcherOp::CHANGE || op == WatcherOp::STATS) && isRepetition(path)) {
        return;
    }

#ifdef Q_OS_MAC
//    path = Normalizer.normalize(path, Normalizer.Form.NFC);
#endif

    switch (op) {
    case WatcherOp::STATS:
    case WatcherOp::CHANGE:
        myNotificationSink->notifyDirtyPath(path);
        break;

    case WatcherOp::CREATE:
    case WatcherOp::DELETE:
        myNotificationSink->notifyPathCreatedOrDeleted(path);
        break;

    case WatcherOp::DIRTY:
        myNotificationSink->notifyDirtyDirectory(path);
        break;

    case WatcherOp::RECDIRTY:
        myNotificationSink->notifyDirtyPathRecursive(path);
        break;

    default:
        jbDebug() << "Unexpected op:" << JBFileWatcherUtils::WatcherOpToString(op);
    }
}

QList<JBNativeFileWatcher *> JBNativeFileWatcher::s_watchers;

void JBNativeFileWatcher::createWatchers(int n) {
    destroyWatchers();
    for (int i = 0; i < n; ++i) {
        s_watchers.append(new JBNativeFileWatcher(JBFS));
    }
}

void JBNativeFileWatcher::destroyWatchers() {
    for (auto it = s_watchers.begin(); it != s_watchers.end(); ++it) {
        (*it)->deleteLater();
    }
    s_watchers.clear();
}

QList<JBNativeFileWatcher *> JBNativeFileWatcher::watchers() {
    return s_watchers;
}
