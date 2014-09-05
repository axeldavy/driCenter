TEMPLATE = lib

CONFIG += staticlib

SOURCES += display_system.c \
    drm_info.c \
    egl_info.c \
    gl_info.c \
    glx_info.c \
    gpu_info.c \
    system_info.c \
    wayland_info.c \
    x_info.c \


HEADERS += \
    include/display_system.h \
    include/dricenter-lib.h \
    include/drm_info.h \
    include/egl_info.h \
    include/gl_info.h \
    include/glx_info.h \
    include/gpu_info.h \
    include/misc.h \
    include/system.h \
    include/wayland_info.h \
    include/x_info.h \

INCLUDEPATH += include/

OTHER_FILES += 

CONFIG += link_pkgconfig
PKGCONFIG += libudev dri egl gbm gl libdrm wayland-client x11
