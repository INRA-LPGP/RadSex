TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -L$$PWD/include/bwa/ -lbwa -pthread -static-libstdc++ -lz
INCLUDEPATH += $$PWD/include/bwa $$PWD/include
DEPENDPATH += $$PWD/include/bwa

PRE_TARGETDEPS += $$PWD/include/bwa/libbwa.a

HEADERS += \
    src/analysis.h \
    src/arg_parser.h \
    src/depth.h \
    src/distrib.h \
    src/freq.h \
    src/map.h \
    src/marker.h \
    src/markers_table.h \
    src/parameters.h \
    src/popmap.h \
    src/process.h \
    src/signif.h \
    src/stats.h \
    src/subset.h \
    src/utils.h

SOURCES += \
    src/analysis.cpp \
    src/depth.cpp \
    src/distrib.cpp \
    src/freq.cpp \
    src/main.cpp \
    src/map.cpp \
    src/marker.cpp \
    src/markers_table.cpp \
    src/popmap.cpp \
    src/process.cpp \
    src/signif.cpp \
    src/stats.cpp \
    include/kfun/kfun.cpp \
    src/subset.cpp

