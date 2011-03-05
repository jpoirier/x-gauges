CONFIG += warn_on \
          qt \
          thread \
          exceptions \
          stl \
          rtti \
          staticlib \
          debug_and_release
QT += xml svg core gui

HEADERS += ../../Instruments/src/ParseHelper.h \
           ../../Instruments/src/Parameter.h \
           ../../Instruments/src/ParameterAirspeed.h \
           ../../Instruments/src/ParameterContainer.h \
           ../../Instruments/src/XMLTags.h \
           ../../Instruments/src/Frame.h \
           ../../Instruments/src/Scale.h \
           ../../Instruments/src/ScaleRound.h \
           ../../Instruments/src/AbstractGauge.h \
           ../../Instruments/src/XMLGaugeRound.h \
           ../../Instruments/src/XMLGaugeRoundAltitude.h \
           ../../Instruments/src/Needle.h \
           ../../Instruments/src/TurnBankGaugeRound.h \
           ../../Instruments/src/AttitudeGaugeRound.h \
           ../../Instruments/src/EngineMiniGaugeRect.h \
           ../../Instruments/src/FuelCompGaugeRect.h

SOURCES += ../../Instruments/src/ParseHelper.cpp \
           ../../Instruments/src/Parameter.cpp \
           ../../Instruments/src/ParameterAirspeed.cpp \
           ../../Instruments/src/ParameterContainer.cpp \
           ../../Instruments/src/Frame.cpp \
           ../../Instruments/src/Scale.cpp \
           ../../Instruments/src/ScaleRound.cpp \
           ../../Instruments/src/AbstractGauge.cpp \
           ../../Instruments/src/XMLGaugeRound.cpp \
           ../../Instruments/src/XMLGaugeRoundAltitude.cpp \
           ../../Instruments/src/Needle.cpp \
           ../../Instruments/src/TurnBankGaugeRound.cpp \
           ../../Instruments/src/AttitudeGaugeRound.cpp \
           ../../Instruments/src/EngineMiniGaugeRect.cpp \
           ../../Instruments/src/FuelCompGaugeRect.cpp

TEMPLATE = lib

INCLUDEPATH += ../src \
               ../../Common


DEFINES += _KANARDIA_NESIS \
           QT_NO_DEBUG



