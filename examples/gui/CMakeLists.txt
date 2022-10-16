project(fsnotifier-gui)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME} main.cpp MainWindow.h MainWindow.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE fsnotifier-lib) 
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)