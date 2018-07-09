TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt


SOURCES += main.cpp \
    test.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libworld/release/ -lworld
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libworld/debug/ -lworld
else:unix: LIBS += -L$$PWD/libworld/ -lworld

INCLUDEPATH += $$PWD/libworld
DEPENDPATH += $$PWD/libworld

HEADERS += \
    test.h

