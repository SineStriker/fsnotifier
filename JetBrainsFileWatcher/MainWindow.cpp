#include "MainWindow.h"
#include "JetBrainsFileWatcher/JBFileWatcherAdvancedNSink.h"

#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    fs = JBFS;

    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);

    request1Button = new QPushButton("Req1", this);
    request2Button = new QPushButton("Req2", this);

    connect(startButton, &QPushButton::clicked, this, [&]() { fs->start(); });
    connect(stopButton, &QPushButton::clicked, this, [&]() { fs->dispose(); });

    Q_UNUSED(requests)

    connect(request1Button, &QPushButton::clicked, this, [&]() { //
        fs->replaceWatchedRoots({}, {"E:/test1/123"}, {});
    });
    connect(request2Button, &QPushButton::clicked, this, [&]() { //
        fs->replaceWatchedRoots(fs->currentWatchedRoots(), {}, {});
    });

    connect(fs, &JB::LocalFileSystem::pathsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Paths dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(),
                      [](const QString &path) { qDebug() << path; });
    });
    connect(fs, &JB::LocalFileSystem::flatDirsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Non-recursive dirs dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(),
                      [](const QString &path) { qDebug() << path; });
    });
    connect(fs, &JB::LocalFileSystem::recursivePathsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Recursive paths dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(),
                      [](const QString &path) { qDebug() << path; });
    });

    QWidget *w = new QWidget();
    setCentralWidget(w);

    layout = new QGridLayout();
    w->setLayout(layout);

    layout->addWidget(startButton, 0, 0);
    layout->addWidget(stopButton, 0, 1);
    layout->addWidget(request1Button, 1, 0);
    layout->addWidget(request2Button, 1, 1);
}

MainWindow::~MainWindow() {
}
