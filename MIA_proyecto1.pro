TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    utils.cpp \
    virtual_blocks.cpp \
    parser.cpp \
    scanner.cpp \
    vars.cpp \
    handler_reports.cpp \
    handler_commands.cpp \
    handler_disk.cpp \
    handler_fs.cpp \
    handler_partitions.cpp

HEADERS += \
    structures.h \
    enums.h \
    utils.h \
    virtual_blocks.h \
    parser.h \
    scanner.h \
    scanner.l \
    vars.h \
    handler_reports.h \
    handler_commands.h \
    handler_disk.h \
    handler_fs.h \
    handler_partitions.h

DISTFILES += \
    parser.y
