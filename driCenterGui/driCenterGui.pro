TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    cardinfo.cpp \
    drimodel.cpp \
    drixml.cpp \
    appcardchooser.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    cardinfo.h \
    drimodel.h \
    drixml.h \
    appcardchooser.h

OTHER_FILES += 

LIBS += -L$$OUT_PWD/../CardDetection/MesaImport/ -lMesaImport

INCLUDEPATH += $$PWD/../CardDetection/MesaImport
DEPENDPATH += $$PWD/../CardDetection/MesaImport

PRE_TARGETDEPS += $$OUT_PWD/../CardDetection/MesaImport/libMesaImport.a

LIBS += -L$$OUT_PWD/../CardDetection/driCenterDete/ -ldriCenterDete

INCLUDEPATH += $$PWD/../CardDetection/driCenterDete/include/
DEPENDPATH += $$PWD/../CardDetection/driCenterDete

PRE_TARGETDEPS += $$OUT_PWD/../CardDetection/driCenterDete/libdriCenterDete.a

LIBS += -ldl

CONFIG += link_pkgconfig
PKGCONFIG += libudev dri egl gbm gl libdrm wayland-client x11
