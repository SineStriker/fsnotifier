#include "JBFileWatchRequest.h"

#include <QDebug>

JBSymlinkData::JBSymlinkData() {
}

JBSymlinkData::JBSymlinkData(int id, const QString &path, const QString &target)
    : m_id(id), m_path(path), m_target(QDir::toNativeSeparators(target)) {
}

int JBSymlinkData::id() const {
    return m_id;
}

QString JBSymlinkData::path() const {
    return m_path;
}

QString JBSymlinkData::target() const {
    return m_target;
}

bool JBSymlinkData::hasValidTarget() const {
    return !m_target.isEmpty();
}

bool JBSymlinkData::operator==(const JBSymlinkData &other) const {
    return m_id == other.m_id && m_path == other.m_path && m_target == other.m_target;
}

QDebug operator<<(QDebug debug, const JBSymlinkData &data) {
    debug.noquote().nospace() << data.toString();
    return debug;
}

JBFileWatchRequest::JBFileWatchRequest() : path(""), recursive(false), symlink(false) {
}

JBFileWatchRequest::JBFileWatchRequest(const QString &root, bool recursive)
    : path(root), recursive(recursive), symlink(false) {
}

JBFileWatchRequest::JBFileWatchRequest(const JBSymlinkData &data, bool recursive)
    : recursive(recursive), symlink(true), data(data) {
}

JBFileWatchRequest::~JBFileWatchRequest() {
}

QString JBFileWatchRequest::rootPath() const {
    return path;
}

bool JBFileWatchRequest::isSymlink() const {
    return symlink;
}

bool JBFileWatchRequest::isRecursive() const {
    return recursive;
}

bool JBFileWatchRequest::isRegistered() const {
    return symlink && registered;
}

bool JBFileWatchRequest::setRegistered(bool registered) {
    if (this->registered != registered) {
        this->registered = registered;
        return true;
    }
    return false;
}

JBSymlinkData JBFileWatchRequest::symlinkData() const {
    return data;
}

QString JBFileWatchRequest::orgPath() const {
    return data.path();
}

QDebug operator<<(QDebug debug, const JBFileWatchRequest &request) {
    debug.nospace() << "WatchRequest(" << request.path << ", "
                    << (request.recursive ? "Recursive" : "Flat") << ", "
                    << (request.symlink ? "Symlink" : "Normal") << ")";
    return debug;
}

#define Q_N(A) QString::number(A)

uint qHash(const JBFileWatchRequest &key, uint seed) {
    if (key.symlink) {
        return qHash(Q_N(key.recursive) + "|" + Q_N(key.data.id()) + "|" + key.data.path() + "|" +
                         key.data.target(),
                     seed);
    } else {
        return qHash(Q_N(key.recursive) + "|" + key.path);
    }
}

#undef Q_N
