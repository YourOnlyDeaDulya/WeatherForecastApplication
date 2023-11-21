QT       += core gui
QT += network
QT += charts
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    additionalnfowindow.cpp \
    apiqueryconstructor.cpp \
    apirequesthandler.cpp \
    contentwidget.cpp \
    currentwindow.cpp \
    errormessage.cpp \
    forecastwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    resultwindow.cpp \
    sqlcachecleaner.cpp \
    sqlcachingservice.cpp \
    temperaturerecordswidget.cpp \
    weatherdatacollector.cpp

HEADERS += \
    additionalnfowindow.h \
    apiqueryconstructor.h \
    apirequesthandler.h \
    contentwidget.h \
    currentwindow.h \
    errormessage.h \
    forecastwindow.h \
    mainwindow.h \
    request_info_types.h \
    resultwindow.h \
    sqlcachecleaner.h \
    sqlcachingservice.h \
    temperaturerecordswidget.h \
    weatherdatacollector.h

FORMS += \
    additionalnfowindow.ui \
    currentwindow.ui \
    forecastwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
