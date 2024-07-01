QT       += core gui sql network charts \# widget
    quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    api.cpp \
    bill.cpp \
    billwindow.cpp \
    database.cpp \
    filterdialog.cpp \
    financialgoalswindow.cpp \
    graphwindow.cpp \
    ingoalswindow.cpp \
    main.cpp \
    mainwindow.cpp \
    outgoalswindow.cpp \
    saving.cpp \
    transaction.cpp \
    transactionwindow.cpp

HEADERS += \
    account.h \
    api.h \
    bill.h \
    billwindow.h \
    database.h \
    filterdialog.h \
    financialgoalswindow.h \
    graphwindow.h \
    ingoalswindow.h \
    mainwindow.h \
    outgoalswindow.h \
    saving.h \
    transaction.h \
    transactionwindow.h

FORMS += \
    billwindow.ui \
    financialgoalswindow.ui \
    graphwindow.ui \
    ingoalswindow.ui \
    mainwindow.ui \
    outgoalswindow.ui \
    transactionwindow.ui

TRANSLATIONS += \
    financia_manager_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    picture.qrc
