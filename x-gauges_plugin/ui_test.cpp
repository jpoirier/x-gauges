// Copyright (c) 2011 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#if defined(__WINDOWS__)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pport.h"
#include "ptypes.h"
#include "pasync.h"
#include "ptime.h"
#include "pstreams.h"
#include "pstringx.h"

#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMNavigation.h"
#include "XPLMDisplay.h"
#include "XPLMMenus.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"

#include "defs.h"
#include "plugin.h"
#include "ui.h"
#include "config.h"


float plugin_callback(float inElapsedSinceLastCall,
                      float inElapsedTimeSinceLastFlightLoop,
                      int inCounter,
                      void* inRefcon);


USING_PTYPES


bool gEnabled = false;
static const float FL_CB_INTERVAL = -1.0;

ipaddress   gP1_ip = " ";
string      gP1_port = " ";
string      gP1_enabled = '0';
ipaddress   gP2_ip = " ";
string      gP2_port = " ";
string      gP2_enabled = '0';
ipaddress   gCp1_ip = " ";
string      gCp1_port = " ";
string      gCp1_enabled = '0';
ipaddress   gCp2_ip = " ";
string      gCp2_port = " ";
string      gCp2_enabled = '0';


PLUGIN_API int
XPluginStart(char* outName, char* outSig, char* outDesc) {

    DPRINTF("X-Gauges Plugin: starting XPluginStart\n");

    strcpy(outName, "X-Gauges");
    strcpy(outSig , "jdp.x-gauges");
    strcpy(outDesc, "X-Gauges Plugin.");

    DPRINTF("X-Gauges Plugin: calling read_config\n");
    read_config();

    DPRINTF("X-Gauges Plugin: calling ui_create\n");
    ui_create();

    DPRINTF("X-Gauges Plugin: ending XPluginStart\n");
    return true;
}


PLUGIN_API int
XPluginEnable(void) {

    gEnabled = true;
    DPRINTF("X-Gauges: XPluginEnable\n");

    // register flight loop callbacks
//  XPLMRegisterFlightLoopCallback(
//                          sendSimCallback,
//                          nav_data_delay,
//                          NULL);

//    XPLMRegisterFlightLoopCallback(plugin_callback, FL_CB_INTERVAL, NULL);

    return true;
}

float plugin_callback(float   inElapsedSinceLastCall,
                      float   inElapsedTimeSinceLastFlightLoop,
                      int     inCounter,
                      void*   inRefcon) {
    return 1.0;
}


PLUGIN_API void
XPluginReceiveMessage(XPLMPluginID  inFromWho,
                      long          inMessage,
                      void*         inParam) {
    // nop
}


PLUGIN_API void
XPluginDisable(void) {

    DPRINTF("X-Gauges: XPluginDisable\n");

    gEnabled = false;
    XPLMUnregisterFlightLoopCallback(plugin_callback, NULL);
}


PLUGIN_API void
XPluginStop(void) {

    write_config();
    ui_destroy();
}


