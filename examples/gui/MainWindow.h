#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGridLayout>
#include <QMainWindow>
#include <QPushButton>

#include "fsnotifier.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    QGridLayout *layout;
    QPushButton *startButton, *stopButton, *request1Button, *request2Button;

    //    JB::LocalFileSystem *fs;
    //    QList<JB::WatchRequest> requests;
    FileSystemNotifier *fs;
};
#endif // MAINWINDOW_H
