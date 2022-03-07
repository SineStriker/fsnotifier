#include "MainWindow.h"
#include "JetBrainsFileWatcher/JBFileWatcherAdvancedNSink.h"

#include <QApplication>
#include <QDebug>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);

    request1Button = new QPushButton("Add", this);
    request2Button = new QPushButton("Remove", this);

    fs = FSN;

    connect(startButton, &QPushButton::clicked, this, [&]() { fs->startWatch(); });
    connect(stopButton, &QPushButton::clicked, this, [&]() { fs->stopWatch(); });

    connect(request1Button, &QPushButton::clicked, this, [&]() { //
        fs->addRecursivePaths({"C:/"});
    });

    connect(request2Button, &QPushButton::clicked, this, [&]() { //
        fs->removeAllPaths();
    });

    connect(fs, &FileSystemNotifier::changed, [](const QStringList &paths) {
        qDebug() << "[Paths changed]";
        std::for_each(paths.begin(), paths.end(), [](const QString &path) { //
            qDebug().noquote() << path;
        });
    });

    connect(fs, &FileSystemNotifier::renamed, [](const QStringList &paths) {
        qDebug() << "[Paths renamed]";
        std::for_each(paths.begin(), paths.end(), [](const QString &path) { //
            qDebug().noquote() << path;
        });
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
