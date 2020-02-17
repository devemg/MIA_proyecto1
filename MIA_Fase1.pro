TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    automata.cpp \
    main.cpp \
    handlerpartitions.cpp \
    handlerdisk.cpp \
    utils.cpp \
    virtual_blocks.cpp

HEADERS += \
    automata.h \
    structures.h \
    handlerpartitions.h \
    handlerdisk.h \
    enums.h \
    utils.h \
    virtual_blocks.h
