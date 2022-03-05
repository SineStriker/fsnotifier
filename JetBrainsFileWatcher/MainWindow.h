#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGridLayout>
#include <QMainWindow>
#include <QPushButton>

#include "JetBrainsFileWatcher/JBFileWatchNamespace.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    QGridLayout *layout;
    QPushButton *startButton, *stopButton, *request1Button, *request2Button;

    JB::LocalFileSystem *fs;
    QList<JB::WatchRequest> requests;
};
#endif // MAINWINDOW_H
