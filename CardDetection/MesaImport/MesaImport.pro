TEMPLATE = lib

CONFIG += staticlib

SOURCES += loader.c \
    pci_id_driver_map.c


HEADERS += \
    include/pci_ids/i810_pci_ids.h \
    include/pci_ids/i915_pci_ids.h \
    include/pci_ids/i965_pci_ids.h \
    include/pci_ids/r200_pci_ids.h \
    include/pci_ids/r300_pci_ids.h \
    include/pci_ids/r600_pci_ids.h \
    include/pci_ids/radeon_pci_ids.h \
    include/pci_ids/radeonsi_pci_ids.h \
    include/pci_ids/vmwgfx_pci_ids.h \
    loader.h \
    pci_id_driver_map.h

INCLUDEPATH += include/

OTHER_FILES += 
 
CONFIG += link_pkgconfig
PKGCONFIG += libudev gbm libdrm
