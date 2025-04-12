QT += sql
QT       += core gui
QT       += charts
QT       += winextras
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Utils/systemtrayutils.cpp \
    dbmanager.cpp \
    iconlabel.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    Utils/systemtrayutils.h \
    dbmanager.h \
    iconlabel.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin

LIBS += -lpsapi -luser32


!isEmpty(target.path): INSTALLS += target

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

RESOURCES += \
    res.qrc

DISTFILES += \
    Utils/test.txt
