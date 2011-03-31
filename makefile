
# host dev



ifeq "$(MAC_HS)" "true"
    OPTIONS += -DMAC_HS
endif

OPTIONS += -restrict

INCLUDE += -I$(WNBASE)/Host\ Dev/include
INCLUDE += -I$(WNBASE)/ue/src
INCLUDE += -I$(WNBASE)/src/mc_scheduler

all:
	$(CXX) -c $(INCLUDE) $(OPTIONS) -Wall client.cxx
	$(CXX) -c $(INCLUDE) $(OPTIONS) -Wall get_opt.cxx
	$(CXX) -c $(INCLUDE) $(OPTIONS) -Wall host_mem_mngr.cxx
	$(CXX) -c $(INCLUDE) $(OPTIONS) -Wall server.cxx
	$(CXX) -c $(INCLUDE) $(OPTIONS) -Wall main.cxx

ifeq "$(MAC_HS)" "true"
	$(CXX) client.o get_opt.o host_mem_mngr.o server.o main.o -L. -L$(LIBDIR) -lptypes -lenc_hsdpa_l1 -lwsock32  -lcommon -ltlsf -lencoder -lmac_hs -o host_with_mac
else
	$(CXX) client.o get_opt.o host_mem_mngr.o server.o main.o -L. -L$(LIBDIR) -lptypes -lenc_hsdpa -lencoder -lue -ltlsf -lcommon -o host_no_mac
endif


profile:
	$(CXX) -c $(OPTIONS) -D__PROFILE $(INCLUDE) profile.cxx
	$(CXX) -c $(OPTIONS) -D__PROFILE $(INCLUDE) client.cxx
	$(CXX) -c $(OPTIONS) -D__PROFILE $(INCLUDE) get_opt.cxx
	$(CXX) -c $(OPTIONS) -D__PROFILE $(INCLUDE) host_mem_mngr.cxx
	$(CXX) -c $(OPTIONS) -D__PROFILE $(INCLUDE) server.cxx
	$(CXX) -c $(OPTIONS) -D__PROFILE $(INCLUDE) main.cxx
	
	$(CXX) profile.o client.o get_opt.o host_mem_mngr.o main.o server.o -L. -L$(LIBDIR) -lcommon -ltlsf -lptypes -lenc_hsdpa_l1 -lencoder -lue -o host_profile_no_mac
	
clean:
	$(RM) host_with_mac
	$(RM) host_no_mac
	$(RM) host_profile_no_mac
	$(RM) turbo_encoders_test_main
	$(RM) *.o
	


