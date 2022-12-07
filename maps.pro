QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets  webenginewidgets network

CONFIG += c++1z
HEADERS += \
    mainwindow.h

SOURCES += main.cpp \
    mainwindow.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/webenginewidgets/maps
INSTALLS += target

!qtConfig(webengine-geolocation) {
    error('Qt WebEngine compiled without geolocation support, this example will not work.')
}

