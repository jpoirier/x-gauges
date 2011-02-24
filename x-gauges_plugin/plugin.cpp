// Copyright (c) 2011 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#if defined(__WINDOWS__)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include "pport.h"
#include "ptypes.h"
#include "pasync.h"
#include "ptime.h"


#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"

#include "nedmalloc.h"
#include "defs.h"
#include "overloaded.h"
#include "client_thread.h"
#include "plugin.h"

float FlightLoopCallback(float inElapsedSinceLastCall,
                         float inElapsedTimeSinceLastFlightLoop,
                         int inCounter,
                         void* inRefcon);


/*


sim/flightmodel2/controls/flap_handle_deploy_ratio
sim/aircraft/specialcontrols/acf_ail1flaps
*/

USING_PTYPES

bool gPowerUp = true;
bool gEnabled = false;
unsigned int gFlCbCnt = 0;

static const float FL_CB_INTERVAL = -1.0;


/*
sim/cockpit2/gauges/indicators/compass_heading_deg_mag		    float	n	degrees_magnetic	Indicated heading of the wet compass, in degrees.
sim/cockpit2/gauges/indicators/airspeed_kts_pilot		            float	n	knots			    Indicated airspeed in knots, pilot.
sim/cockpit2/gauges/indicators/airspeed_kts_copilot		        float	n	knots			    Indicated airspeed in knots, copilot.
sim/cockpit2/gauges/indicators/altitude_ft_pilot		            float	n	feet			    Indicated height, MSL, in feet, primary system, based on pilots barometric pressure input.
sim/cockpit2/gauges/indicators/altitude_ft_copilot		        float	n	feet			    Indicated height, MSL, in feet, primary system, based on co-pilots barometric pressure input.
sim/cockpit2/gauges/indicators/vvi_fpm_pilot			            float	n	feet/minute		    Indicated vertical speed in feet per minute, pilot system.
sim/cockpit2/gauges/indicators/vvi_fpm_copilot			        float	n	feet/minute		    Indicated vertical speed in feet per minute, copilot system.
sim/cockpit2/gauges/indicators/turn_rate_roll_deg_pilot		    float	n	degrees			    Indicated rate-of-turn, in degrees deflection, for newer roll-augmented turn-indicators.  Pilot side.
sim/cockpit2/gauges/indicators/turn_rate_roll_deg_copilot	    float	n	degrees			    Indicated rate-of-turn, in degrees deflection, for newer roll-augmented turn-indicators.  Copilot side.
sim/cockpit2/gauges/indicators/turn_rate_heading_deg_pilot	    float	n	degrees			    Indicated rate-of-turn, in degrees deflection, for old-style turn-indicators.  Pilot side.
sim/cockpit2/gauges/indicators/turn_rate_heading_deg_copilot	    float	n	degrees			    Indicated rate-of-turn, in degrees deflection, for old-style turn-indicators.  Copiot side.
sim/cockpit2/gauges/indicators/heading_AHARS_deg_mag_pilot	    float	n	degrees_magnetic	Indicated magetic heading, in degrees.  Source: AHARS.  Side: Pilot
sim/cockpit2/gauges/indicators/heading_AHARS_deg_mag_copilot	    float	n	degrees_magnetic	Indicated magetic heading, in degrees.  Source: AHARS.  Side: Copilot
sim/cockpit2/gauges/indicators/heading_electric_deg_mag_pilot	float	n	degrees_magnetic	Indicated magetic heading, in degrees.  Source: electric gyro.  Side: Pilot
sim/cockpit2/gauges/indicators/heading_electric_deg_mag_copilot	float	n	degrees_magnetic	Indicated magetic heading, in degrees.  Source: electric gyro.  Side: Copilot
sim/cockpit2/gauges/indicators/heading_vacuum_deg_mag_pilot	    float	n	degrees_magnetic	Indicated magetic heading, in degrees.  Source: vacuum gyro.  Side: Pilot
sim/cockpit2/gauges/indicators/heading_vacuum_deg_mag_copilot	float	n	degrees_magnetic	Indicated magetic heading, in degrees.  Source: vacuum gyro.  Side: Copilot
sim/cockpit2/gauges/indicators/pitch_AHARS_deg_pilot		        float	n	degrees			    Indicated pitch, in degrees, psoitive up.  Source: AHARS.  Side: Pilot
sim/cockpit2/gauges/indicators/pitch_AHARS_deg_copilot		    float	n	degrees			    Indicated pitch, in degrees, psoitive up.  Source: AHARS.  Side: Copilot
sim/cockpit2/gauges/indicators/pitch_electric_deg_pilot		    float	n	degrees			    Indicated pitch, in degrees, psoitive up.  Source: electric gyro.  Side: Pilot
sim/cockpit2/gauges/indicators/pitch_electric_deg_copilot	    float	n	degrees			    Indicated pitch, in degrees, psoitive up.  Source: electric gyro.  Side: Copilot
sim/cockpit2/gauges/indicators/pitch_vacuum_deg_pilot		    float	n	degrees			    Indicated pitch, in degrees, psoitive up.  Source: vacuum gyro.  Side: Pilot
sim/cockpit2/gauges/indicators/pitch_vacuum_deg_copilot		    float	n	degrees			    Indicated pitch, in degrees, psoitive up.  Source: vacuum gyro.  Side: Copilot
sim/cockpit2/gauges/indicators/radio_altimeter_height_ft_pilot	float	n	feet			    Radio-altimeter indicated height in feet, pilot-side
sim/cockpit2/gauges/indicators/radio_altimeter_height_ft_copilot float	n	feet			    Radio-altimeter indicated height in feet, pilot-side
sim/cockpit2/gauges/indicators/radio_altimeter_dh_lit_pilot	    int	    n	boolean			    Radio-altimeter indicated height in feet, pilot-side
sim/cockpit2/gauges/indicators/radio_altimeter_dh_lit_copilot	int	    n	boolean			    Radio-altimeter indicated height in feet, pilot-side
sim/cockpit2/gauges/indicators/roll_AHARS_deg_pilot		        float	n	degrees			    Indicated roll, in degrees, positive right.  Source: AHARS.  Side: Pilot
sim/cockpit2/gauges/indicators/roll_AHARS_deg_copilot		    float	n	degrees			    Indicated roll, in degrees, positive right.  Source: AHARS.  Side: Copilot
sim/cockpit2/gauges/indicators/roll_electric_deg_pilot		    float	n	degrees			    Indicated roll, in degrees, positive right.  Source: electric gyro.  Side: Pilot
sim/cockpit2/gauges/indicators/roll_electric_deg_copilot	        float	n	degrees			    Indicated roll, in degrees, positive right.  Source: electric gyro.  Side: Copilot
sim/cockpit2/gauges/indicators/roll_vacuum_deg_pilot		        float	n	degrees			    Indicated roll, in degrees, positive right.  Source: vacuum gyro.  Side: Pilot
sim/cockpit2/gauges/indicators/roll_vacuum_deg_copilot		    float	n	degrees			    Indicated roll, in degrees, positive right.	 Source: vacuum gyro.  Side: Copilot
sim/cockpit2/gauges/indicators/wind_heading_deg_mag		        float	n	degrees_magnetic	Wind direction currently acting on the plane, in degrees magnetic.
sim/cockpit2/gauges/indicators/wind_speed_kts			            float	n	knots			    Wind speed currently acting on the plane, in knots true.

sim/cockpit2/engine/indicators/engine_speed_rpm	                float[8]	n	revolutions/minute	Engine speed, radians per second
sim/cockpit2/engine/indicators/prop_speed_rpm	                    float[8]	n	revolutions/minute	Prop speed, radians per second
*/
XPLMCommandRef systems_avionics_on_ref;
XPLMCommandRef systems_avionics_off_ref;
XPLMCommandRef compass_heading_deg_mag_ref;
XPLMCommandRef airspeed_kts_pilot_ref;
XPLMCommandRef airspeed_kts_copilot_ref;
XPLMCommandRef altitude_ft_pilot_ref;
XPLMCommandRef altitude_ft_copilot_ref;
XPLMCommandRef vvi_fpm_pilot_ref;
XPLMCommandRef vvi_fpm_copilot_ref;
XPLMCommandRef turn_rate_roll_deg_pilot_ref;
XPLMCommandRef turn_rate_roll_deg_copilot_ref;
XPLMCommandRef turn_rate_heading_deg_pilot_ref;
XPLMCommandRef turn_rate_heading_deg_copilot_ref;
XPLMCommandRef heading_AHARS_deg_mag_pilot_ref;
XPLMCommandRef heading_AHARS_deg_mag_copilot_ref;
XPLMCommandRef heading_electric_deg_mag_pilot_ref;
XPLMCommandRef heading_electric_deg_mag_copilot_ref;
XPLMCommandRef heading_vacuum_deg_mag_pilot_ref;
XPLMCommandRef heading_vacuum_deg_mag_copilot_ref;
XPLMCommandRef pitch_AHARS_deg_pilot_ref;
XPLMCommandRef pitch_AHARS_deg_copilot_ref;
XPLMCommandRef pitch_electric_deg_pilot_ref;
XPLMCommandRef pitch_electric_deg_copilot_ref;
XPLMCommandRef pitch_vacuum_deg_pilot_ref;
XPLMCommandRef pitch_vacuum_deg_copilot_ref;
XPLMCommandRef radio_altimeter_height_ft_pilot_ref;
XPLMCommandRef radio_altimeter_height_ft_copilot_ref;
XPLMCommandRef radio_altimeter_dh_lit_pilot_ref;
XPLMCommandRef radio_altimeter_dh_lit_copilot_ref;
XPLMCommandRef roll_AHARS_deg_pilot_ref;
XPLMCommandRef roll_AHARS_deg_copilot_ref;
XPLMCommandRef roll_electric_deg_pilot_ref;
XPLMCommandRef roll_electric_deg_copilot_ref;
XPLMCommandRef roll_vacuum_deg_pilot_ref;
XPLMCommandRef roll_vacuum_deg_copilot_ref;
XPLMCommandRef wind_heading_deg_mag_ref;
XPLMCommandRef wind_speed_kts_ref;

string gP1_ip = " ";
string gP1_port = " ";
string gP1_enabled = '0';
string gP2_ip = " ";
string gP2_port = " ";
string gP2_enabled = '0';
string gCp1_ip = " ";
string gCp1_port = " ";
string gCp1_enabled = '0';
string gCp2_ip = " ";
string gCp2_port = " ";
string gCp2_enabled = '0';



/*
 * - register the plugin
 * - check for hid connected pro panels
 * - register callbacks
 * - start threads
 *
 */
PLUGIN_API int
XPluginStart(char* outName, char* outSig, char* outDesc) {
    int tmp;

    DPRINTF("X-Gauges Plugin: XPluginStart\n");

    strcpy(outName, "X-Gauges");
    strcpy(outSig , "jdp.x-gauges");
    strcpy(outDesc, "X-Gauges  Plugin.");

    systems_avionics_on_ref                 = XPLMFindDataRef("sim/systems/avionics_on","Avionics on");
    systems_avionics_off_ref                = XPLMFindDataRef("sim/systems/avionics_off","Avionics off");
    compass_heading_deg_mag_ref             = XPLMFindDataRef("sim/cockpit2/gauges/indicators/compass_heading_deg_mag");
    airspeed_kts_pilot_ref                  = XPLMFindDataRef("sim/cockpit2/gauges/indicators/airspeed_kts_pilot");
    airspeed_kts_copilot_ref                = XPLMFindDataRef("sim/cockpit2/gauges/indicators/airspeed_kts_copilot");
    altitude_ft_pilot_ref                   = XPLMFindDataRef("sim/cockpit2/gauges/indicators/altitude_ft_pilot");
    altitude_ft_copilot_ref                 = XPLMFindDataRef("sim/cockpit2/gauges/indicators/altitude_ft_copilot");
    vvi_fpm_pilot_ref                       = XPLMFindDataRef("sim/cockpit2/gauges/indicators/vvi_fpm_pilot");
    vvi_fpm_copilot_ref                     = XPLMFindDataRef("sim/cockpit2/gauges/indicators/vvi_fpm_copilot");
    turn_rate_roll_deg_pilot_ref            = XPLMFindDataRef("sim/cockpit2/gauges/indicators/turn_rate_roll_deg_pilot");
    turn_rate_roll_deg_copilot_ref          = XPLMFindDataRef("sim/cockpit2/gauges/indicators/turn_rate_roll_deg_copilot");
    turn_rate_heading_deg_pilot_ref         = XPLMFindDataRef("sim/cockpit2/gauges/indicators/turn_rate_heading_deg_pilot");
    turn_rate_heading_deg_copilot_ref       = XPLMFindDataRef("sim/cockpit2/gauges/indicators/turn_rate_heading_deg_copilot	");
    heading_AHARS_deg_mag_pilot_ref         = XPLMFindDataRef("sim/cockpit2/gauges/indicators/heading_AHARS_deg_mag_pilot");
    heading_AHARS_deg_mag_copilot_ref       = XPLMFindDataRef("sim/cockpit2/gauges/indicators/heading_AHARS_deg_mag_copilot	");
    heading_electric_deg_mag_pilot_ref      = XPLMFindDataRef("sim/cockpit2/gauges/indicators/heading_electric_deg_mag_pilot");
    heading_electric_deg_mag_copilot_ref    = XPLMFindDataRef("sim/cockpit2/gauges/indicators/heading_electric_deg_mag_copilot");
    heading_vacuum_deg_mag_pilot_ref        = XPLMFindDataRef("sim/cockpit2/gauges/indicators/heading_vacuum_deg_mag_pilot");
    heading_vacuum_deg_mag_copilot_ref      = XPLMFindDataRef("sim/cockpit2/gauges/indicators/heading_vacuum_deg_mag_copilot");
    pitch_AHARS_deg_pilot_ref               = XPLMFindDataRef("sim/cockpit2/gauges/indicators/pitch_AHARS_deg_pilot	");
    pitch_AHARS_deg_copilot_ref             = XPLMFindDataRef("sim/cockpit2/gauges/indicators/pitch_AHARS_deg_copilot");
    pitch_electric_deg_pilot_ref            = XPLMFindDataRef("sim/cockpit2/gauges/indicators/pitch_electric_deg_pilot");
    pitch_electric_deg_copilot_ref          = XPLMFindDataRef("sim/cockpit2/gauges/indicators/pitch_electric_deg_copilot");
    pitch_vacuum_deg_pilot_ref              = XPLMFindDataRef("sim/cockpit2/gauges/indicators/pitch_vacuum_deg_pilot");
    pitch_vacuum_deg_copilot_ref            = XPLMFindDataRef("sim/cockpit2/gauges/indicators/pitch_vacuum_deg_copilot");
    radio_altimeter_height_ft_pilot_ref     = XPLMFindDataRef("sim/cockpit2/gauges/indicators/radio_altimeter_height_ft_pilot");
    radio_altimeter_height_ft_copilot_ref   = XPLMFindDataRef("sim/cockpit2/gauges/indicators/radio_altimeter_height_ft_copilot");
    radio_altimeter_dh_lit_pilot_ref        = XPLMFindDataRef("sim/cockpit2/gauges/indicators/radio_altimeter_dh_lit_pilot");
    radio_altimeter_dh_lit_copilot_ref      = XPLMFindDataRef("sim/cockpit2/gauges/indicators/radio_altimeter_dh_lit_copilot");
    roll_AHARS_deg_pilot_ref                = XPLMFindDataRef("sim/cockpit2/gauges/indicators/roll_AHARS_deg_pilot");
    roll_AHARS_deg_copilot_ref              = XPLMFindDataRef("sim/cockpit2/gauges/indicators/roll_AHARS_deg_copilot");
    roll_electric_deg_pilot_ref             = XPLMFindDataRef("sim/cockpit2/gauges/indicators/roll_electric_deg_pilot");
    roll_electric_deg_copilot_ref           = XPLMFindDataRef("sim/cockpit2/gauges/indicators/roll_electric_deg_copilot");
    roll_vacuum_deg_pilot_ref               = XPLMFindDataRef("sim/cockpit2/gauges/indicators/roll_vacuum_deg_pilot");
    roll_vacuum_deg_copilot_ref             = XPLMFindDataRef("sim/cockpit2/gauges/indicators/roll_vacuum_deg_copilot");
    wind_heading_deg_mag_ref                = XPLMFindDataRef("sim/cockpit2/gauges/indicators/wind_heading_deg_mag");
    wind_speed_kts_ref                      = XPLMFindDataRef("sim/cockpit2/gauges/indicators/wind_speed_kts	");



    DPRINTF("X-Gauges Plugin: commands initialized\n");

    init_hid(&gRpHandle, RP_PROD_ID);
    init_hid(&gMpHandle, MP_PROD_ID);
    init_hid(&gSpHandle, SP_PROD_ID);

    mp_init(gMpHandle, XPLMGetDatai(gApStateRef));

    pexchange((int*)&threads_run, true);

    ToPanelThread*     tp;
    FromPanelThread*   fp;

    // radio panel
    tp = new ToPanelThread(gRpHandle, &gRp_ijq, &gRpTrigger, RP_PROD_ID);
    fp = new FromPanelThread(gRpHandle, &gRp_ijq, &gRp_ojq, &gRpTrigger, RP_PROD_ID, &rpProcOutData);

    tp->start();
    fp->start();

    // multi panel
    tp = new ToPanelThread(gMpHandle, &gMp_ijq, &gMpTrigger, MP_PROD_ID);
    fp = new FromPanelThread(gMpHandle, &gMp_ijq, &gMp_ojq, &gMpTrigger, MP_PROD_ID, &mpProcOutData);

    tp->start();
    fp->start();

    // switch panel
    tp = new ToPanelThread(gSpHandle, &gSp_ijq, &gSpTrigger, SP_PROD_ID);
    fp = new FromPanelThread(gSpHandle, &gSp_ijq, &gSp_ojq, &gSpTrigger, SP_PROD_ID, &spProcOutData);

    tp->start();
    fp->start();

    PanelsCheckThread* pc = new PanelsCheckThread();
    pc->start();

    if (gRpHandle) { DPRINTF("Saitek ProPanels Plugin: gRpHandle\n"); gRpTrigger.post(); }
    if (gMpHandle) { DPRINTF("Saitek ProPanels Plugin: gMpHandle\n"); gMpTrigger.post(); }
    if (gSpHandle) { DPRINTF("Saitek ProPanels Plugin: gSpHandle\n"); gSpTrigger.post(); }

    DPRINTF("Saitek ProPanels Plugin: Panel threads running\n");

    XPLMRegisterFlightLoopCallback(FlightLoopCallback, FL_CB_INTERVAL, NULL);

    DPRINTF("Saitek ProPanels Plugin: startup completed\n");

    return 1;
}

/*
 *
 *
 */
float FlightLoopCallback(float  inElapsedSinceLastCall,
                         float  inElapsedTimeSinceLastFlightLoop,
                         int    inCounter,
                         void*  inRefcon) {

    if ((gFlCbCnt % PANEL_CHECK_INTERVAL) == 0) {
        if (gEnabled) {
            gPcTrigger.post();
        }
    }

    // Pilot gauge info
    GaugeInfo* p                    = (GaugeInfo*) malloc(sizeof(GaugeInfo));
    p->systems_avionics_on          = XPLMGetDataf(systems_avionics_on_ref);
    p->systems_avionics_off         = XPLMGetDataf(systems_avionics_off_ref);
    p->compass_heading_deg_mag      = XPLMGetDataf(compass_heading_deg_mag_ref);
    p->airspeed_kts                 = XPLMGetDataf(airspeed_kts_pilot_ref);
    p->altitude_ft                  = XPLMGetDataf(altitude_ft_pilot_ref);
    p->vvi_fpm                      = XPLMGetDataf(vvi_fpm_pilot_ref);
    p->turn_rate_roll_deg           = XPLMGetDataf(turn_rate_roll_deg_pilot_ref);
    p->turn_rate_heading_deg        = XPLMGetDataf(turn_rate_heading_deg_pilot_ref);
    p->heading_AHARS_deg_mag        = XPLMGetDataf(heading_AHARS_deg_mag_pilot_ref);
    p->heading_electric_deg_mag     = XPLMGetDataf(heading_electric_deg_mag_pilot_ref);
    p->heading_vacuum_deg_mag       = XPLMGetDataf(heading_vacuum_deg_mag_pilot_ref);
    p->pitch_AHARS_deg              = XPLMGetDataf(pitch_AHARS_deg_pilot_ref);
    p->pitch_electric_deg           = XPLMGetDataf(pitch_electric_deg_pilot_ref);
    p->pitch_vacuum_deg             = XPLMGetDataf(pitch_vacuum_deg_pilot_ref);
    p->radio_altimeter_height_ft    = XPLMGetDataf(radio_altimeter_height_ft_pilot_ref);
    p->radio_altimeter_dh_lit       = XPLMGetDataf(radio_altimeter_dh_lit_pilot_ref);
    p->roll_AHARS_deg               = XPLMGetDataf(roll_AHARS_deg_pilot_ref);
    p->roll_electric_deg            = XPLMGetDataf(roll_electric_deg_pilot_ref);
    p->roll_vacuum_deg              = XPLMGetDataf(roll_vacuum_deg_pilot_ref);
    p->wind_heading_deg_mag         = XPLMGetDataf(wind_heading_deg_mag_ref);
    p->wind_speed_kts               = XPLMGetDataf(wind_speed_kts_ref);

    // CoPilot gauge info
    GaugeInfo* cp                   = (GaugeInfo*) malloc(sizeof(GaugeInfo));
    cp->systems_avionics_on         = XPLMGetDataf(systems_avionics_on_ref);
    cp->systems_avionics_off        = XPLMGetDataf(systems_avionics_off_ref);
    cp->compass_heading_deg_mag     = XPLMGetDataf(compass_heading_deg_mag_ref);
    cp->airspeed_kts                = XPLMGetDataf(airspeed_kts_copilot_ref);
    cp->altitude_ft                 = XPLMGetDataf(altitude_ft_copilot_ref);
    cp->vvi_fpm                     = XPLMGetDataf(vvi_fpm_copilot_ref);
    cp->turn_rate_roll_deg          = XPLMGetDataf(turn_rate_roll_deg_copilot_ref);
    cp->turn_rate_heading_deg       = XPLMGetDataf(turn_rate_heading_deg_copilot_ref);
    cp->heading_AHARS_deg_mag       = XPLMGetDataf(heading_AHARS_deg_mag_copilot_ref);
    cp->heading_electric_deg_mag    = XPLMGetDataf(heading_electric_deg_mag_copilot_ref);
    cp->heading_vacuum_deg_mag      = XPLMGetDataf(heading_vacuum_deg_mag_copilot_ref);
    cp->pitch_AHARS_deg             = XPLMGetDataf(pitch_AHARS_deg_copilot_ref);
    cp->pitch_electric_deg          = XPLMGetDataf(pitch_electric_deg_copilot_ref);
    cp->pitch_vacuum_deg            = XPLMGetDataf(pitch_vacuum_deg_copilot_ref);
    cp->radio_altimeter_height_ft   = XPLMGetDataf(radio_altimeter_height_ft_copilot_ref);
    cp->radio_altimeter_dh_lit      = XPLMGetDataf(radio_altimeter_dh_lit_copilot_ref);
    cp->roll_AHARS_deg              = XPLMGetDataf(roll_AHARS_deg_copilot_ref);
    cp->roll_electric_deg           = XPLMGetDataf(roll_electric_deg_copilot_ref);
    cp->roll_vacuum_deg             = XPLMGetDataf(roll_vacuum_deg_copilot_ref);
    cp->wind_heading_deg_mag        = XPLMGetDataf(wind_heading_deg_mag_ref);
    cp->wind_speed_kts              = XPLMGetDataf(wind_speed_kts_ref);

    gFlCbCnt++;

    return 1.0;
}

/*
 *
 */
PLUGIN_API void
XPluginStop(void) {
    unsigned char* x;
// TODO: do the message and protocol
    x = (unsigned char*) malloc(sizeof(unsigned char));
    *x = 0xff;
    gRp_ijq.post(new myjob(x));

    x = (unsigned char*) malloc(sizeof(unsigned char));
    *x = 0xff;
    gMp_ijq.post(new myjob(x));

    x = (unsigned char*) malloc(sizeof(unsigned char));
    *x = 0xff;
    gSp_ijq.post(new myjob(x));

    pexchange((int*)&pc_run, false);
    pexchange((int*)&threads_run, false);

    gPcTrigger.post();
    gRpTrigger.post();
    gMpTrigger.post();
    gSpTrigger.post();

    hid_close((hid_device*)gRpHandle);
    hid_close((hid_device*)gMpHandle);
    hid_close((hid_device*)gSpHandle);

    psleep(500);
    XPLMUnregisterFlightLoopCallback(FlightLoopCallback, NULL);
}

/*
 *
 */
PLUGIN_API void
XPluginDisable(void) {
    gEnabled = false;
    gRpTrigger.reset();
    gMpTrigger.reset();
    gSpTrigger.reset();
}

/*
 *
 */
PLUGIN_API int
XPluginEnable(void) {
    gEnabled = true;

    if (gPowerUp) {
        gPowerUp = false;
        return 1;
    }

    gRpTrigger.post();
    gMpTrigger.post();
    gSpTrigger.post();

    return 1;
}

PLUGIN_API void
XPluginReceiveMessage(XPLMPluginID inFrom, long inMsg, void* inParam) {
    //
}
