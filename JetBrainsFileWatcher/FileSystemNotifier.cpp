#include "FileSystemNotifier.h"
#include "FileSystemNotifier_p.h"
#include "JetBrainsFileWatcher/JBFileWatchNamespace.h"

using namespace JB::Utils;

#include <QCoreApplication>

FileSystemNotifier::FileSystemNotifier(QObject *parent)
    : FileSystemNotifier(*new FileSystemNotifierPrivate(this), parent) {
}

FileSystemNotifier::~FileSystemNotifier() {
}

FileSystemNotifier::FileSystemNotifier(FileSystemNotifierPrivate &d, QObject *parent)
    : QObject(parent), d(&d) {

    Q_ASSERT(!self);
    self = this;

    d.init();
}

void FileSystemNotifier::startWatch() {
    d->fs->start();
}

void FileSystemNotifier::stopWatch() {
    d->fs->dispose();
}

bool FileSystemNotifier::isWatching() const {
    return !d->fs->disposed();
}

void FileSystemNotifier::addRecursivePaths(const QStringList &paths) {
    auto set = ListToSet(paths);
    d->recursivePathsToAdd.unite(set);
    d->recursivePathsToRemove.subtract(set);
    d->postChange();
}

void FileSystemNotifier::addFlatPaths(const QStringList &paths) {
    auto set = ListToSet(paths);
    d->flatPathsToAdd.unite(set);
    d->flatPathsToRemove.subtract(set);
    d->postChange();
}

void FileSystemNotifier::removeRecursivePaths(const QStringList &paths) {
    auto set = ListToSet(paths);
    d->recursivePathsToAdd.subtract(set);
    d->recursivePathsToRemove.unite(set);
    d->postChange();
}

void FileSystemNotifier::removeFlatPaths(const QStringList &paths) {
    auto set = ListToSet(paths);
    d->flatPathsToAdd.subtract(set);
    d->flatPathsToRemove.unite(set);
    d->postChange();
}

void FileSystemNotifier::removeAllRecursivePaths() {
    auto paths = d->recursivePaths();
    d->recursivePathsToAdd.clear();
    d->recursivePathsToRemove.unite(paths);
    d->postChange();
}

void FileSystemNotifier::removeAllFlatPaths() {
    auto paths = d->flatPaths();
    d->flatPathsToAdd.clear();
    d->flatPathsToRemove.unite(paths);
    d->postChange();
}

void FileSystemNotifier::removeAllPaths() {
    auto pathsPair = d->paths();
    d->recursivePathsToAdd.clear();
    d->flatPathsToAdd.clear();
    d->recursivePathsToRemove.unite(pathsPair.first);
    d->flatPathsToRemove.unite(pathsPair.second);
    d->postChange();
}

QStringList FileSystemNotifier::recursivePaths() const {
    auto paths = d->recursivePaths();
    return SetToList(paths.unite(d->recursivePathsToAdd).subtract(d->recursivePathsToRemove));
}

QStringList FileSystemNotifier::flatPaths() const {
    auto paths = d->flatPaths();
    return SetToList(paths.unite(d->flatPathsToAdd).subtract(d->flatPathsToRemove));
}

bool FileSystemNotifier::waitForPathsSet(int msecs) {
    return d->waitForPathsSet(msecs);
}

bool FileSystemNotifier::event(QEvent *event) {
    if (event->type() == QEvent::Timer) {
        QTimerEvent *e = static_cast<QTimerEvent *>(event);
        int id = -e->timerId();
        if (id > 0) {
            if (id > d->maxChangeEventId) {
                d->maxChangeEventId = id;
                d->commitChange();
                return true;
            }
            return false;
        }
    }
    return QObject::event(event);
}

FileSystemNotifier *FileSystemNotifier::self = nullptr;

FileSystemNotifier *FileSystemNotifier::instance() {
    if (!self) {
        new FileSystemNotifier(qApp);
    }
    return self;
}

QString FileSystemNotifier::ExecutableFilePath() {
    return JBNativeFileWatcher::FSNotifierExecutable();
}

void FileSystemNotifier::setExecutableFilePath(const QString &path) {
    JBNativeFileWatcher::setFsNotifierExecutablePath(path);
}
