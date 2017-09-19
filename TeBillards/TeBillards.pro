TEMPLATE = app
CONFIG += c++1z
CONFIG -= app_bundle
CONFIG -= qt

DEFINES -= DEBUG

LIBS += -lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio

SOURCES += main.cpp \
    ball.cpp \
    game.cpp \
    utils.cpp \
    pocket.cpp \
    random.cpp

HEADERS += \
    ball.h \
    constants.h \
    utils.h \
    game.h \
    pocket.h \
    random.h
