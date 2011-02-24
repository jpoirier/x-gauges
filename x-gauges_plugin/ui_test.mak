
CC=gcc
CXX=g++
CP=cp

#DIR=$(shell cd)
DIR=$(shell pwd)
SDK=$(DIR)/../SDK/CHeaders


#OPTIONS+=-ggdb -arch i386 -D__XPTESTING__ -DDEBUG
OPTIONS+=-Os -arch i386 -DNO_PANEL_CHECK
#DEFS+=-DNO_NED_NAMESPACE -DREPLACE_SYSTEM_ALLOCATOR -DXPLM200 -DAPL=1
DEFS+=-DXPLM200 -DAPL=1

INCLUDE=-I$(DIR)/../include
INCLUDE+=-I$(DIR)/../include/ptypes
INCLUDE+=-I$(SDK)/XPLM
INCLUDE+=-I$(SDK)/Wrappers
INCLUDE+=-I$(SDK)/Widgets
INCLUDE+=-I$(DIR)/include

LIBS=-framework IOKit -framework CoreFoundation
LIBS+=-lptypes32

LNFLAGS+=-dynamiclib -flat_namespace -undefined warning

all:
	$(CXX) -c $(INCLUDE) $(DEFS) $(OPTIONS) -Wall config.cpp
	$(CXX) -c $(INCLUDE) $(DEFS) $(OPTIONS) -Wall ui.cpp
	$(CXX) -c $(INCLUDE) $(DEFS) $(OPTIONS) -Wall ui_test.cpp
	$(CXX) $(OPTIONS) -L. $(LIBS) $(LNFLAGS) config.o ui.o ui_test.o -o ui_test.xpl

clean:
	$(RM) *.o *.xpl
	


