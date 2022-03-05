#include "MainWindow.h"
#include "JetBrainsFileWatcher/JBFileWatcherAdvancedNSink.h"

#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    fs = JBFS;
    //    fs = new JB::LocalFileSystem(this);
    //    fs = new JBLocalFileSystem(this);

    //    set.insert("C:/Windows");
    //    set.insert("E:/test1");

    //    qDebug() << set.size();
    //    qDebug() << JBFileWatcherUtils::StringUtil::compare('C', 'D', true);
    //    qDebug() << JBFileWatcherUtils::OSAgnosticPathUtil::COMPARATOR_compare("C:/Windows",
    //                                                                           "E:/test1");

    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);

    request1Button = new QPushButton("Req1", this);
    request2Button = new QPushButton("Req2", this);

    //    JBNavigableFileSet set;

    connect(startButton, &QPushButton::clicked, this, [&]() { fs->start(); });
    connect(stopButton, &QPushButton::clicked, this, [&]() { fs->dispose(); });

    connect(request1Button, &QPushButton::clicked, this, [&]() { //
        requests = fs->replaceWatchedRoots(requests, {}, {"C:/Windows/system32"});
        qDebug() << " ";
        qDebug() << requests;
        qDebug() << " ";
    });
    connect(request2Button, &QPushButton::clicked, this, [&]() { //
        requests = fs->replaceWatchedRoots(requests, {"C:/Windows"}, {});
        qDebug() << " ";
        qDebug() << requests;
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
