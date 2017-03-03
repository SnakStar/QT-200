#-------------------------------------------------
#
# Project created by QtCreator 2016-04-14T23:15:41
#
#-------------------------------------------------

QT       += core gui
QT +=sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = qt4
TEMPLATE = app

TRANSLATIONS = QT200_ch_CN.ts QT200_en_CN.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    frminput.cpp \
    testwindow.cpp \
    querywindow.cpp \
    qcwindow.cpp \
    setwindow.cpp \
    settimer.cpp \
    cutilsettings.cpp \
    renfvalue.cpp \
    cqtprodb.cpp \
    cchecktablewidget.cpp \
    ccheckheaderview.cpp \
    qcvalue.cpp \
    qcustomplot.cpp \
    qextserial/qextserialport.cpp \
    resultcalc.cpp \
    upgrading.cpp \
    udiskshow.cpp \
    exportdata.cpp \
    QRoundProgressBar.cpp \
    hardgrading.cpp \
    userinterface.cpp \
    csimplehl7.cpp \
    resultmodify.cpp
win32{
    SOURCES += qextserial/qextserialport_win.cpp
}
unix{
    SOURCES += canbus.cpp
    SOURCES += qextserial/qextserialport_unix.cpp
    HEADERS += canbus.h
}

HEADERS  += mainwindow.h \
    frminput.h \
    testwindow.h \
    querywindow.h \
    qcwindow.h \
    setwindow.h \
    settimer.h \
    cutilsettings.h \
    renfvalue.h \
    cqtprodb.h \
    cchecktablewidget.h \
    ccheckheaderview.h \
    qcvalue.h \
    qcustomplot.h \
    CanCmd.h \
    qextserial/qextserialport_p.h \
    qextserial/qextserialport_global.h \
    qextserial/qextserialport.h \
    resultcalc.h \
    CalcDef.h \
    upgrading.h \
    udiskshow.h \
    exportdata.h \
    QRoundProgressBar.h \
    hardgrading.h \
    userinterface.h \
    csimplehl7.h \
    resultmodify.h


FORMS    += mainwindow.ui \
    frminput.ui \
    testwindow.ui \
    querywindow.ui \
    qcwindow.ui \
    setwindow.ui \
    settimer.ui \
    renfvalue.ui \
    qcvalue.ui \
    udiskshow.ui \
    userinterface.ui \
    resultmodify.ui

RESOURCES += \
    qt200.qrc

OTHER_FILES += \
    ../qt4-build-desktop-Qt_4_8_6__install____/style.qss
