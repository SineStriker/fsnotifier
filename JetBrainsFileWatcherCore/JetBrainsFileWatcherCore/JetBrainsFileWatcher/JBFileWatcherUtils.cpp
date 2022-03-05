#include "JBFileWatcherUtils.h"

#include <QDebug>
#include <QDir>
#include <QSet>

const QStringList WatcherOpStrings = {
    "UNKNOWN", "GIVEUP", "RESET", "UNWATCHEABLE", "REMAP", "MESSAGE",
    "CREATE",  "DELETE", "STATS", "CHANGE",       "DIRTY", "RECDIRTY",
};

QString JBFileWatcherUtils::WatcherOpToString(JBFileWatcherUtils::WatcherOp op) {
    return WatcherOpStrings.at(op);
}

JBFileWatcherUtils::WatcherOp JBFileWatcherUtils::StringToWatcherOp(const QString &str) {
    int index = WatcherOpStrings.indexOf(str);
    if (str < 0) {
        return UNKNOWN;
    }
    return static_cast<WatcherOp>(index);
}

QString JBFileWatcherUtils::MessageToFailureReasonString(const QString &message) {
    return message;
}

bool JBFileWatcherUtils::SystemInfo::isFileSystemCaseSensitive() {
#ifdef Q_OS_WINDOWS
    return true;
#else
    return false;
#endif
}

Qt::CaseSensitivity JBFileWatcherUtils::SystemInfo::FileSystemCaseSensitivity() {
#ifdef Q_OS_WINDOWS
    return Qt::CaseInsensitive;
#else
    return Qt::CaseSensitive;
#endif
}

int JBFileWatcherUtils::StringUtil::compare(QChar c1, QChar c2, bool ignoreCase) {
    int d = int(c1.unicode()) - int(c2.unicode());
    if (d == 0 || !ignoreCase) {
        return d;
    }
    auto u1 = c1.toUpper();
    auto u2 = c2.toUpper();
    d = int(u1.unicode()) - int(u2.unicode());
    if (d != 0) {
        d = int(u1.toLower().unicode()) - int(u2.toLower().unicode());
    }
    return d;
}

int JBFileWatcherUtils::Integer::compare(int x, int y) {
    return (x < y) ? -1 : ((x == y) ? 0 : 1);
}

int JBFileWatcherUtils::PathUtil::getLastIndexOfPathSeparator(const QString &path, int end) {
    return qMax(path.lastIndexOf('/', end - 1), path.lastIndexOf('\\', end - 1));
}

QString JBFileWatcherUtils::PathUtil::getParentPath(const QString &path) {
    if (path.isEmpty()) {
        return "";
    }
    int end = qMax(path.lastIndexOf('/'), path.lastIndexOf('\\'));
    if (end == path.length() - 1) {
        end = getLastIndexOfPathSeparator(path, end);
    }
    if (end == -1 || end == 0) {
        return "";
    }
    if (isWindowsUNCRoot(path, end)) {
        return "";
    }
    // parent of '//host' is root
    QChar prev = path.at(end - 1);
    if (prev == '/' || prev == '\\') {
        end--;
    }
    return path.mid(0, end);
}

bool JBFileWatcherUtils::PathUtil::isWindowsUNCRoot(const QString &path,
                                                    int lastPathSeparatorPosition) {
#ifndef Q_OS_WINDOWS
    return false;
#else
    return (path.startsWith("//") || path.startsWith("\\\\")) &&
           getLastIndexOfPathSeparator(path, lastPathSeparatorPosition) == 1;
#endif
}

bool JBFileWatcherUtils::FileUtil::startsWith(const QString &path, const QString &prefix) {
    return path.startsWith(prefix, SystemInfo::FileSystemCaseSensitivity());
}

bool JBFileWatcherUtils::FileUtil::pathsEqual(const QString &path1, const QString &path2) {
    if (path1 == path2) {
        return true;
    }
    if (path1.isEmpty() || path2.isEmpty()) {
        return false;
    }
    QFileInfo info1(path1);
    QFileInfo info2(path2);
    return info1.canonicalFilePath() == info2.canonicalFilePath();
}

bool JBFileWatcherUtils::OSAgnosticPathUtil::isSlash(QChar c) {
    return c == '/' || c == '\\';
}

bool JBFileWatcherUtils::OSAgnosticPathUtil::isUncPath(const QString &path) {
    return path.length() > 1 && isSlash(path.at(0)) && path.at(1) == path.at(0);
}

bool JBFileWatcherUtils::OSAgnosticPathUtil::startsWith(const QString &path,
                                                        const QString &prefix) {
    int pathLength = path.size(), prefixLength = prefix.size();
    if (prefixLength == 0) {
        return true;
    }
    if (prefixLength > pathLength) {
        return false;
    }

    bool ignoreCase = !SystemInfo::isFileSystemCaseSensitive();

    for (int pos = 0; pos < pathLength && pos < prefixLength; pos++) {
        QChar ch1 = path.at(pos);
        QChar ch2 = prefix.at(pos);
        if (ch1 == ch2)
            continue;
        if (ch1 == '/') {
            if (ch2 == '\\')
                continue;
            return false;
        } else if (ch1 == '\\') {
            if (ch2 == '/')
                continue;
            return false;
        } else if (ch2 == '/' || ch2 == '\\') {
            return false;
        }
        if (StringUtil::compare(ch1, ch2, ignoreCase) != 0) {
            return false;
        }
    }
    if (pathLength == prefixLength) {
        return true;
    }
    QChar lastPrefixChar = prefix.at(prefixLength - 1);
    int slashOrSeparatorIdx = prefixLength;
    if (lastPrefixChar == '/' || lastPrefixChar == '\\') {
        slashOrSeparatorIdx = prefixLength - 1;
    }
    QChar next = path.at(slashOrSeparatorIdx);
    return next == '/' || next == '\\';
}

int JBFileWatcherUtils::OSAgnosticPathUtil::COMPARATOR_compare(const QString &path1,
                                                               const QString &path2) {
    if (path1 == path2) {
        return 0;
    }

    int length1 = path1.size();
    int length2 = path2.size();
    bool ignoreCase = !SystemInfo::isFileSystemCaseSensitive();

    for (int pos = 0; pos < length1 && pos < length2; pos++) {
        QChar ch1 = path1.at(pos);
        QChar ch2 = path2.at(pos);
        if (ch1 == ch2)
            continue;
        if (ch1 == '/') {
            if (ch2 == '\\')
                continue;
            return -1;
        } else if (ch1 == '\\') {
            if (ch2 == '/')
                continue;
            return -1;
        } else if (ch2 == '/' || ch2 == '\\') {
            return 1;
        }
        int diff = StringUtil::compare(ch1, ch2, ignoreCase);
        if (diff != 0) {
            return diff;
        }
    }
    return Integer::compare(length1, length2);
}

bool JBFileWatcherUtils::WatchRootsUtil::isCoveredRecursively(
    const JBNavigableFileSet &recursiveRoots, const QString &path) {
    auto it = recursiveRoots.floor(path);
    return it != recursiveRoots.end() && OSAgnosticPathUtil::startsWith(path, *it);
}

void JBFileWatcherUtils::WatchRootsUtil::insertRecursivePath(JBNavigableFileSet &recursiveRoots,
                                                             const QString &path) {
    if (!isCoveredRecursively(recursiveRoots, path)) {
        recursiveRoots.insert(path);

        // Remove any roots covered by newly added
        JBNavigableFileSet::iterator higherIt;
        while ((higherIt = recursiveRoots.higher(path)) != recursiveRoots.end() &&
               OSAgnosticPathUtil::startsWith(*higherIt, path)) {
            recursiveRoots.remove(*higherIt);
        }
    }
}

JBNavigableFileSet
    JBFileWatcherUtils::WatchRootsUtil::optimizeFlatRoots(const QSet<QString> &flatRoots,
                                                          const JBNavigableFileSet &recursiveRoots,
                                                          bool convertToForwardSlashes) {
    JBNavigableFileSet result;
    if (convertToForwardSlashes) {
        for (QString flatRoot : flatRoots) {
            flatRoot.replace('/', '\\');
            if (!isCoveredRecursively(recursiveRoots, flatRoot)) {
                result.insert(flatRoot);
            }
        }
    } else {
        for (const QString &flatRoot : flatRoots) {
            if (!isCoveredRecursively(recursiveRoots, flatRoot)) {
                result.insert(flatRoot);
            }
        }
    }
    return result;
}
