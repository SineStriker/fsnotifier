#ifndef JBFILEWATCHREQUEST_H
#define JBFILEWATCHREQUEST_H

#include <QDir>
#include <QScopedPointer>
#include <QString>

class JBSymlinkData {
public:
    JBSymlinkData();
    JBSymlinkData(int id, const QString &path, const QString &target);

public:
    int id() const;
    QString path() const;
    QString target() const;

    bool hasValidTarget() const;

    inline QString toString() const {
        return "SymlinkData{" + QString::number(m_id) + ", " + m_path + " -> " + m_target + '}';
    }

private:
    int m_id;
    QString m_path;
    QString m_target;

public:
    friend QDebug operator<<(QDebug debug, const JBSymlinkData &data);
};

class JBFileWatchRequest {
public:
    JBFileWatchRequest();
    JBFileWatchRequest(const QString &root, bool recursive, bool symlink = false);
    JBFileWatchRequest(const JBSymlinkData &data, bool recursive);
    ~JBFileWatchRequest();

    QString rootPath() const;

    bool isSymlink() const;
    bool isRecursive() const;

    bool isRegistered() const;
    bool setRegistered(bool registered);

    JBSymlinkData symlinkData() const;
    void setSymlinkData(const JBSymlinkData &data);

    QString orgPath() const;

    inline bool operator==(const JBFileWatchRequest &other) const {
        return (path == other.path) && (recursive == other.recursive) &&
               (symlink == other.symlink) &&
               ((symlink && registered == other.registered) || !symlink) &&
               (data.id() == other.data.id());
    }

protected:
    QString path;

    bool recursive;
    bool symlink;
    bool registered;

    JBSymlinkData data;

public:
    friend QDebug operator<<(QDebug debug, const JBFileWatchRequest &request);
    friend uint qHash(const JBFileWatchRequest &key, uint seed);
};

#endif // JBFILEWATCHREQUEST_H
