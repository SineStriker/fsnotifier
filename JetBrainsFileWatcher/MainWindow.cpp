#include "MainWindow.h"
#include "JetBrainsFileWatcher/JBFileWatcherAdvancedNSink.h"

#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    fs = new JB::LocalFileSystem(this);

    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);

    request1Button = new QPushButton("Req1", this);
    request2Button = new QPushButton("Req2", this);

    connect(startButton, &QPushButton::clicked, this, [&]() { fs->start(); });
    connect(stopButton, &QPushButton::clicked, this, [&]() { fs->dispose(); });

    Q_UNUSED(requests)

    connect(request1Button, &QPushButton::clicked, this, [&]() { //
        fs->replaceWatchedRoots({}, {}, {"E:/test1"});
        qDebug() << " ";
        qDebug() << fs->currentWatchedRoots();
        qDebug() << " ";
    });
    connect(request2Button, &QPushButton::clicked, this, [&]() { //
        fs->replaceWatchedRoots({}, {"C:/Windows"}, {});
        qDebug() << " ";
        qDebug() << fs->currentWatchedRoots();
        qDebug() << " ";
    });

    connect(fs, &JB::LocalFileSystem::pathsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Paths dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(),
                      [](const QString &path) { qDebug() << path; });
        qDebug() << " ";
    });
    connect(fs, &JB::LocalFileSystem::flatDirsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Non-recursive dirs dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(),
                      [](const QString &path) { qDebug() << path; });
        qDebug() << " ";
    });
    connect(fs, &JB::LocalFileSystem::recursiveDirsDirty, [](const QStringList &dirtyPaths) {
        qDebug() << "[Recursive dirs dirty]";
        std::for_each(dirtyPaths.begin(), dirtyPaths.end(),
                      [](const QString &path) { qDebug() << path; });
        qDebug() << " ";
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
