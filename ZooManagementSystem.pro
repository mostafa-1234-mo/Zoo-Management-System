QT += core gui widgets sql charts printsupport
QT += core gui widgets sql charts printsupport
QT += core gui sql printsupport


CONFIG += c++17

SOURCES += \
    addanimaldialog.cpp \
    admindialog.cpp \
    animalwidget.cpp \
    dashboardwidget.cpp \
    databasemanager.cpp \
    feedingwidget.cpp \
    logger.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    animal.cpp \
    registerdialog.cpp \
    reportwidget.cpp \
    ticket.cpp \
    employee.cpp \
    animalmanager.cpp \
    ticketmanager.cpp \
    ticketwidget.cpp \
    usermanager.cpp \
    validator.cpp \
    welcomedialog.cpp

HEADERS += \
    addanimaldialog.h \
    admindialog.h \
    animalwidget.h \
    dashboardwidget.h \
    databasemanager.h \
    feedingwidget.h \
    logger.h \
    logindialog.h \
    mainwindow.h \
    animal.h \
    registerdialog.h \
    reportwidget.h \
    ticket.h \
    employee.h \
    animalmanager.h \
    ticketmanager.h \
    ticketwidget.h \
    usermanager.h \
    validator.h \
    welcomedialog.h

FORMS += \
    addanimaldialog.ui \
    admindialog.ui \
    animalwidget.ui \
    dashboardwidget.ui \
    feedingwidget.ui \
    logindialog.ui \
    mainwindow.ui \
    registerdialog.ui \
    reportwidget.ui \
    ticketwidget.ui \
    welcomedialog.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    style.qss