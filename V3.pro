QT       += core gui multimedia multimediawidgets

macx {
	QT += dbus
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

VERSION = 0.0.2.1

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
	CONFIG(debug, debug | release) {
		DESTDIR = $$PWD/bin_debug
	}
	else {
		DESTDIR = $$PWD/bin
	}
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    preferencesdialog.cpp

HEADERS += \
    mainwindow.h \
    preferencesdialog.h \
    structs.h

FORMS += \
    mainwindow.ui \
	preferencesdialog.ui

win32 {
	include($$PWD/AutoWindeployQt.pri)
}
