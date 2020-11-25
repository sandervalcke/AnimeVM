QT += core quick gui qml sql

TARGET   = avm
TEMPLATE = app
CONFIG   += c++17

INCLUDEPATH += inc

SOURCES += \
    src/Database.cpp \
    src/MainWindow.cpp \
    src/Data.cpp \
    src/SeriesSettings.cpp \
    src/Series.cpp \
    src/DirectoryList.cpp \
    src/CheckBoxDelegate.cpp
    src/main.cpp

RESOURCES += qml.qrc
