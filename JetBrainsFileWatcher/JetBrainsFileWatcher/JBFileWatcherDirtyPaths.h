#ifndef JBFILEWATCHERDIRTYPATHS_H
#define JBFILEWATCHERDIRTYPATHS_H

#include <QSet>
#include <QString>

class JBFileWatcherDirtyPaths {
public:
    JBFileWatcherDirtyPaths();
    ~JBFileWatcherDirtyPaths();

public:
    bool isEmpty();
    void clear();

    void addDirtyPath(const QString &path);
    void addDirtyPathRecursive(const QString &path);

    QSet<QString> dirtyPaths;
    QSet<QString> dirtyPathsRecursive;
    QSet<QString> dirtyDirectories;
};

#endif // JBFILEWATCHERDIRTYPATHS_H
