QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    booking.cpp \
    calendardialog.cpp \
    customer.cpp \
    employee.cpp \
    main.cpp \
    mainwindow.cpp \
    payment.cpp \
    thememanager.cpp \
    ucustomer.cpp

HEADERS += \
    booking.h \
    calendardialog.h \
    customer.h \
    employee.h \
    mainwindow.h \
    payment.h \
    thememanager.h \
    ucustomer.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
