#include "Application.h"

#include "JetBrainsFileWatcher/JBFileWatchNamespace.h"

#ifdef Q_QDOC
Application::Application(int m_argc, char **argv) : QApplication(argc, argv) {
}
#else
Application::Application(int &argc, char **argv, int flag) : QApplication(argc, argv, flag) {
}
#endif

Application::~Application() {
}
