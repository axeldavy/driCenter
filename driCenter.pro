TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    systemanalysis.cpp \
    cardinfo.cpp \
    drimodel.cpp \
    drixml.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    systemanalysis.h \
    cardinfo.h \
    drimodel.h \
    drixml.h

LIBS += -ludev -lEGL -lGL -lX11

OTHER_FILES +=
