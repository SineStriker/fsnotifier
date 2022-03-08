#ifndef FILESYSTEMNOTIFIER_H
#define FILESYSTEMNOTIFIER_H

#include <QObject>
#include <QSet>

class FileSystemNotifierPrivate;

#define FSN FileSystemNotifier::instance()

class FileSystemNotifier : public QObject {
    Q_OBJECT
public:
    explicit FileSystemNotifier(QObject *parent = nullptr);
    ~FileSystemNotifier();

protected:
    FileSystemNotifier(FileSystemNotifierPrivate &d, QObject *parent = nullptr);

public:
    /* Note: it's recommended to call stopWatch() at end */
    void startWatch();
    void stopWatch();

    /* Note: continuously added requests will be combined and sent to fsnotifier later */
    void addRecursivePaths(const QStringList &paths);
    void addFlatPaths(const QStringList &paths);

    void removeRecursivePaths(const QStringList &paths);
    void removeFlatPaths(const QStringList &paths);

    void removeAllRecursivePaths();
    void removeAllFlatPaths();
    void removeAllPaths();

    /* Paths currently watched */
    QStringList recursivePaths() const;
    QStringList flatPaths() const;

protected:
    bool event(QEvent *event) override;

private:
    QScopedPointer<FileSystemNotifierPrivate> d; // d_ptr

    // Static
public:
    static FileSystemNotifier *instance();

    static QString ExecutableFilePath();
    static void setExecutableFilePath(const QString &path);

private:
    static FileSystemNotifier *self;

    // Signals
signals:
    void changed(const QStringList &path);
    void renamed(const QStringList &path);
    void failed(const QString &reason);
};

#endif // FILESYSTEMNOTIFIER_H
