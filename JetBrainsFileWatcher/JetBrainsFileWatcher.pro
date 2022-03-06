QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    FileSystemNotifier.cpp \
    FileSystemNotifier_p.cpp \
    JetBrainsFileWatcher/JBCanonicalPathMap.cpp \
    JetBrainsFileWatcher/JBFileWatchNamespace.cpp \
    JetBrainsFileWatcher/JBFileWatchRequest.cpp \
    JetBrainsFileWatcher/JBFileWatcher.cpp \
    JetBrainsFileWatcher/JBFileWatcherAdvancedNSink.cpp \
    JetBrainsFileWatcher/JBFileWatcherDirtyPaths.cpp \
    JetBrainsFileWatcher/JBFileWatcherNotificationSink.cpp \
    JetBrainsFileWatcher/JBFileWatcherUtils.cpp \
    JetBrainsFileWatcher/JBLocalFileSystem.cpp \
    JetBrainsFileWatcher/JBLocalFileSystemTimer.cpp \
    JetBrainsFileWatcher/JBNativeFileWatcher.cpp \
    JetBrainsFileWatcher/JBNavigableFileContainers.cpp \
    JetBrainsFileWatcher/JBNavigableMap.cpp \
    JetBrainsFileWatcher/JBNavigableSet.cpp \
    JetBrainsFileWatcher/JBPluggableFileWatcher.cpp \
    JetBrainsFileWatcher/JBWatchRootsManager.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    FileSystemNotifier.h \
    FileSystemNotifier_p.h \
    JetBrainsFileWatcher/JBCanonicalPathMap.h \
    JetBrainsFileWatcher/JBFileWatchNamespace.h \
    JetBrainsFileWatcher/JBFileWatchRequest.h \
    JetBrainsFileWatcher/JBFileWatcher.h \
    JetBrainsFileWatcher/JBFileWatcherAdvancedNSink.h \
    JetBrainsFileWatcher/JBFileWatcherDirtyPaths.h \
    JetBrainsFileWatcher/JBFileWatcherNotificationSink.h \
    JetBrainsFileWatcher/JBFileWatcherUtils.h \
    JetBrainsFileWatcher/JBLocalFileSystem.h \
    JetBrainsFileWatcher/JBLocalFileSystemTimer.h \
    JetBrainsFileWatcher/JBNativeFileWatcher.h \
    JetBrainsFileWatcher/JBNavigableFileContainers.h \
    JetBrainsFileWatcher/JBNavigableMap.h \
    JetBrainsFileWatcher/JBNavigableSet.h \
    JetBrainsFileWatcher/JBPluggableFileWatcher.h \
    JetBrainsFileWatcher/JBWatchRootsManager.h \
    MainWindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
