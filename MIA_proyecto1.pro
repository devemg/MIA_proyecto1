TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    handlerpartitions.cpp \
    handlerdisk.cpp \
    utils.cpp \
    virtual_blocks.cpp \
    parser.cpp \
    scanner.cpp \
    vars.cpp \
    handlerfilesystem.cpp \
    handler_reports.cpp

HEADERS += \
    structures.h \
    handlerpartitions.h \
    handlerdisk.h \
    enums.h \
    utils.h \
    virtual_blocks.h \
    parser.h \
    scanner.h \
    scanner.l \
    vars.h \
    handlerfilesystem.h \
    handler_reports.h

DISTFILES += \
    parser.y
