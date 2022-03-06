#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class Application : public QApplication {
    Q_OBJECT
public:
#ifdef Q_QDOC
    Application(int &argc, char **argv);
#else
    Application(int &argc, char **argv, int flag = ApplicationFlags);
#endif
    ~Application();
};

#endif // APPLICATION_H
