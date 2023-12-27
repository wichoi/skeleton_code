QT       += core gui multimedia qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cli_task.cpp \
    json/json_manager.cpp \
    main.cpp \
    main_task.cpp \
    media_task.cpp \
    service/config_manager.cpp \
    service/config_service.cpp \
    service/event_service.cpp \
    service/log_service.cpp \
    service/timer_service.cpp \
    window/charging_window.cpp \
    window/idle_window.cpp \
    window/main_window.cpp

HEADERS += \
    cli_task.h \
    json/json_manager.h \
    main_interface.h \
    main_task.h \
    media_task.h \
    service/config_manager.h \
    service/config_service.h \
    service/event_service.h \
    service/log_service.h \
    service/timer_service.h \
    ui_interface.h \
    window/charging_window.h \
    window/idle_window.h \
    window/main_window.h


TRANSLATIONS += \
    ev1000_client_ko_KR.ts
CONFIG += lrelease
CONFIG += embed_translations

TEMPLATE = app
CONFIG += console
INCLUDEPATH += service/ json/ window/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    window/charging_window.ui \
    window/idle_window.ui
