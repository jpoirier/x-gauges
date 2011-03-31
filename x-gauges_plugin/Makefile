
CC=gcc
CXX=g++
CP=cp

#DIR=$(shell cd)
DIR=$(shell pwd)
SDK=$(DIR)/SDK/CHeaders


#OPTIONS+=-ggdb -arch i386 -D__XPTESTING__ -DDEBUG
OPTIONS+=-Os -arch i386

DEFS+=-DNO_NED_NAMESPACE -DREPLACE_SYSTEM_ALLOCATOR -DXPLM200 -DAPL=1
#DEFS+=-DXPLM200 -DAPL=1

INCLUDE=-I$(DIR)/../include
INCLUDE+=-I$(DIR)/../include/ptypes
INCLUDE+=-I$(SDK)/XPLM
INCLUDE+=-I$(SDK)/Wrappers
INCLUDE+=-I$(SDK)/Widgets
INCLUDE+=-I$(DIR)/include

LIBS=-framework IOKit -framework CoreFoundation
LIBS+=-lptypes32

# -shared -rdynamic -nodefaultlibs -m32 export ARCHFLAGS ="-arch i386"
LNFLAGS+=-dynamiclib  -flat_namespace -undefined warning

all:
	$(CXX) -c $(INCLUDE) $(DEFS) $(OPTIONS) -Wall utils.c
	$(CC)  -c $(INCLUDE) $(DEFS) $(OPTIONS) nedmalloc.c
	$(CXX) -c $(INCLUDE) $(DEFS) $(OPTIONS) -Wall overloaded.cpp
	$(CXX) -c $(INCLUDE) $(DEFS) $(OPTIONS) -Wall config.cpp
	$(CXX) -c $(INCLUDE) $(DEFS) $(OPTIONS) -Wall ui.cpp
	$(CXX) -c $(INCLUDE) $(DEFS) $(OPTIONS) -Wall worker_threads.cpp
	$(CXX) -c $(INCLUDE) $(DEFS) $(OPTIONS) -Wall plugin.cpp

	$(CXX) $(OPTIONS) -L. $(LIBS) $(LNFLAGS) nedmalloc.o overloaded.o worker_threads.o utils.o \
							config.o ui.o plugin.o -o x-gauges.xpl

clean:
	$(RM) *.o *.xpl
	


