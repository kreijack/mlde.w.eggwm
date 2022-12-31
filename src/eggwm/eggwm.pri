SOURCES += src/eggwm/Main.cpp \
    src/eggwm/EggWM.cpp \
    src/eggwm/XcbEventFilter.cpp
HEADERS += src/eggwm/EggWM.h \
    src/eggwm/XcbEventFilter.h

include(util/util.pri)
include(events/handlers/handlers.pri)
include(events/factory/factory.pri)
include(xwindows/xwindows.pri)
include(config/config.pri)
include(standards/standards.pri)
include(atoms/atoms.pri)
