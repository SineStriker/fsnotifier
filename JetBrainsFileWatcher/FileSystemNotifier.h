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
    void startWatch();
    void stopWatch();

    void addRecursivePaths(const QStringList &paths);
    void addFlatPaths(const QStringList &paths);

    void removeRecursivePaths(const QStringList &paths);
    void removeFlatPaths(const QStringList &paths);

    void removeAllRecursivePaths();
    void removeAllFlatPaths();
    void removeAllPaths();

    QStringList recursivePaths() const;
    QStringList flatPaths() const;

protected:
    bool event(QEvent *event) override;

private:
    QScopedPointer<FileSystemNotifierPrivate> d;

    // Static
public:
    static FileSystemNotifier *instance();

    static QString ExecutableFilePath();

private:
    static FileSystemNotifier *self;

    // Signals
signals:
    void changed(const QStringList &path);
    void renamed(const QStringList &path);
};

#endif // FILESYSTEMNOTIFIER_H
