// Copyright (c) 2011 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#if defined(__WINDOWS__)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include "ptypes.h"
#include "pinet.h"

#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"

#include "nedmalloc.h"
#include "overloaded.h"
#include "defs.h"
#include "plugin.h"
#include "worker_threads.h"
#include "config.h"

float FlightLoopCallback(float inElapsedSinceLastCall,
                         float inElapsedTimeSinceLastFlightLoop,
                         int inCounter,
                         void* inRefcon);

USING_PTYPES

string gP1_ip       = "";
string gP1_port     = "";
string gP1_enabled  = '0';
string gP2_ip       = "";
string gP2_port     = "";
string gP2_enabled  = '0';
string gCp1_ip      = "";
string gCp1_port    = "";
string gCp1_enabled = '0';
string gCp2_ip      = "";
string gCp2_port    = "";
string gCp2_enabled = '0';

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
XPLMDataRef systems_avionics_on_ref;
XPLMDataRef systems_avionics_off_ref;
XPLMDataRef compass_heading_deg_mag_ref;
XPLMDataRef airspeed_kts_pilot_ref;
XPLMDataRef airspeed_kts_copilot_ref;
XPLMDataRef altitude_ft_pilot_ref;
XPLMDataRef altitude_ft_copilot_ref;
XPLMDataRef vvi_fpm_pilot_ref;
XPLMDataRef vvi_fpm_copilot_ref;
XPLMDataRef turn_rate_roll_deg_pilot_ref;
XPLMDataRef turn_rate_roll_deg_copilot_ref;
XPLMDataRef turn_rate_heading_deg_pilot_ref;
XPLMDataRef turn_rate_heading_deg_copilot_ref;
XPLMDataRef heading_AHARS_deg_mag_pilot_ref;
XPLMDataRef heading_AHARS_deg_mag_copilot_ref;
XPLMDataRef heading_electric_deg_mag_pilot_ref;
XPLMDataRef heading_electric_deg_mag_copilot_ref;
XPLMDataRef heading_vacuum_deg_mag_pilot_ref;
XPLMDataRef heading_vacuum_deg_mag_copilot_ref;
XPLMDataRef pitch_AHARS_deg_pilot_ref;
XPLMDataRef pitch_AHARS_deg_copilot_ref;
XPLMDataRef pitch_electric_deg_pilot_ref;
XPLMDataRef pitch_electric_deg_copilot_ref;
XPLMDataRef pitch_vacuum_deg_pilot_ref;
XPLMDataRef pitch_vacuum_deg_copilot_ref;
XPLMDataRef radio_altimeter_height_ft_pilot_ref;
XPLMDataRef radio_altimeter_height_ft_copilot_ref;
XPLMDataRef radio_altimeter_dh_lit_pilot_ref;
XPLMDataRef radio_altimeter_dh_lit_copilot_ref;
XPLMDataRef roll_AHARS_deg_pilot_ref;
XPLMDataRef roll_AHARS_deg_copilot_ref;
XPLMDataRef roll_electric_deg_pilot_ref;
XPLMDataRef roll_electric_deg_copilot_ref;
XPLMDataRef roll_vacuum_deg_pilot_ref;
XPLMDataRef roll_vacuum_deg_copilot_ref;
XPLMDataRef wind_heading_deg_mag_ref;
XPLMDataRef wind_speed_kts_ref;

/*
 *
 */
PLUGIN_API int
XPluginStart(char* outName, char* outSig, char* outDesc) {

    DPRINTF("X-Gauges Plugin: XPluginStart\n");

    strcpy(outName, "X-Gauges");
    strcpy(outSig , "jdp.x-gauges");
    strcpy(outDesc, "X-Gauges  Plugin.");

    read_config();

    systems_avionics_on_ref                 = XPLMFindDataRef("sim/systems/avionics_on");
    systems_avionics_off_ref                = XPLMFindDataRef("sim/systems/avionics_off");
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

    DPRINTF("X-Gauges Plugin: data refs initialized\n");

    pexchange((int*)&threads_run, true);

    p1  = new WorkerThread(P1_THREAD_ID, &gP1_ijq, &gP1Trigger);
    p2  = new WorkerThread(P2_THREAD_ID, &gP2_ijq, &gP2Trigger);
    cp1 = new WorkerThread(CP1_THREAD_ID, &gCp1_ijq, &gCp1Trigger);
    cp2 = new WorkerThread(CP2_THREAD_ID, &gCp2_ijq, &gCp2Trigger);

    if (!isempty(gP1_ip) && !isempty(gP1_port))
        p1->net_config(gP1_ip, gP1_port);

    if (!isempty(gP2_ip) && !isempty(gP2_port))
        p2->net_config(gP2_ip, gP2_port);

    if (!isempty(gCp1_ip) && !isempty(gCp1_port))
        cp1->net_config(gCp1_ip, gCp1_port);

    if (!isempty(gCp2_ip) && !isempty(gCp2_port))
        cp2->net_config(gCp2_ip, gCp2_port);

    p1->start();
    p2->start();
    cp1->start();
    cp2->start();

    DPRINTF("X-Gauges Plugin: worker threads running\n");

    if (gP1_enabled == '1')
        gP1Trigger.post();

    if (gP2_enabled == '1')
        gP2Trigger.post();

    if (gCp1_enabled == '1')
        gCp1Trigger.post();

    if (gCp2_enabled == '1')
        gCp2Trigger.post();

    XPLMRegisterFlightLoopCallback(FlightLoopCallback, FL_CB_INTERVAL, NULL);

    DPRINTF("X-Gauges  Plugin: startup completed\n");

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

// todo: looks like the named items can be disgarded and an array used

    if ((gFlCbCnt % NETWORK_SEND_INTERVAL) == 0) {
        GaugeInfo* p1 = 0;
        GaugeInfo* cp1 = 0;

        // Pilot gauge info
        if (gP1_enabled == '1' || gP2_enabled == '1') {
            p1 = (GaugeInfo*) malloc(sizeof(GaugeInfo));
            p1->sys_magic                   = SYS_MAGIC_NUM;
            p1->systems_avionics_on         = XPLMGetDataf(systems_avionics_on_ref);
            p1->systems_avionics_off        = XPLMGetDataf(systems_avionics_off_ref);
            p1->compass_heading_deg_mag     = XPLMGetDataf(compass_heading_deg_mag_ref);
            p1->airspeed_kts                = XPLMGetDataf(airspeed_kts_pilot_ref);
            p1->altitude_ft                 = XPLMGetDataf(altitude_ft_pilot_ref);
            p1->vvi_fpm                     = XPLMGetDataf(vvi_fpm_pilot_ref);
            p1->turn_rate_roll_deg          = XPLMGetDataf(turn_rate_roll_deg_pilot_ref);
            p1->turn_rate_heading_deg       = XPLMGetDataf(turn_rate_heading_deg_pilot_ref);
            p1->heading_AHARS_deg_mag       = XPLMGetDataf(heading_AHARS_deg_mag_pilot_ref);
            p1->heading_electric_deg_mag    = XPLMGetDataf(heading_electric_deg_mag_pilot_ref);
            p1->heading_vacuum_deg_mag      = XPLMGetDataf(heading_vacuum_deg_mag_pilot_ref);
            p1->pitch_AHARS_deg             = XPLMGetDataf(pitch_AHARS_deg_pilot_ref);
            p1->pitch_electric_deg          = XPLMGetDataf(pitch_electric_deg_pilot_ref);
            p1->pitch_vacuum_deg            = XPLMGetDataf(pitch_vacuum_deg_pilot_ref);
            p1->radio_altimeter_height_ft   = XPLMGetDataf(radio_altimeter_height_ft_pilot_ref);
            p1->radio_altimeter_dh_lit      = XPLMGetDataf(radio_altimeter_dh_lit_pilot_ref);
            p1->roll_AHARS_deg              = XPLMGetDataf(roll_AHARS_deg_pilot_ref);
            p1->roll_electric_deg           = XPLMGetDataf(roll_electric_deg_pilot_ref);
            p1->roll_vacuum_deg             = XPLMGetDataf(roll_vacuum_deg_pilot_ref);
            p1->wind_heading_deg_mag        = XPLMGetDataf(wind_heading_deg_mag_ref);
            p1->wind_speed_kts              = XPLMGetDataf(wind_speed_kts_ref);

            // replace the names; not needed (?), above with an array
            p1->list[0]     = p1->systems_avionics_on;
            p1->list[1]     = p1->systems_avionics_off;
            p1->list[2]     = p1->compass_heading_deg_mag;
            p1->list[3]     = p1->airspeed_kts;
            p1->list[4]     = p1->altitude_ft;
            p1->list[5]     = p1->vvi_fpm;
            p1->list[6]     = p1->turn_rate_roll_deg;
            p1->list[7]     = p1->turn_rate_heading_deg;
            p1->list[8]     = p1->heading_AHARS_deg_mag;
            p1->list[9]     = p1->heading_electric_deg_mag;
            p1->list[10]    = p1->heading_vacuum_deg_mag;
            p1->list[11]    = p1->pitch_AHARS_deg;
            p1->list[12]    = p1->pitch_electric_deg;
            p1->list[13]    = p1->pitch_vacuum_deg;
            p1->list[14]    = p1->radio_altimeter_height_ft;
            p1->list[15]    = p1->radio_altimeter_dh_lit;
            p1->list[16]    = p1->roll_AHARS_deg;
            p1->list[17]    = p1->roll_electric_deg;
            p1->list[18]    = p1->roll_vacuum_deg;
            p1->list[19]    = p1->wind_heading_deg_mag;
            p1->list[20]    = p1->wind_speed_kts;
        }

        // CoPilot gauge info
        if (gCp1_enabled == '1' || gCp2_enabled == '1') {
            cp1 = (GaugeInfo*) malloc(sizeof(GaugeInfo));
            cp1->sys_magic                  = SYS_MAGIC_NUM;
            cp1->systems_avionics_on        = XPLMGetDataf(systems_avionics_on_ref);
            cp1->systems_avionics_off       = XPLMGetDataf(systems_avionics_off_ref);
            cp1->compass_heading_deg_mag    = XPLMGetDataf(compass_heading_deg_mag_ref);
            cp1->airspeed_kts               = XPLMGetDataf(airspeed_kts_copilot_ref);
            cp1->altitude_ft                = XPLMGetDataf(altitude_ft_copilot_ref);
            cp1->vvi_fpm                    = XPLMGetDataf(vvi_fpm_copilot_ref);
            cp1->turn_rate_roll_deg         = XPLMGetDataf(turn_rate_roll_deg_copilot_ref);
            cp1->turn_rate_heading_deg      = XPLMGetDataf(turn_rate_heading_deg_copilot_ref);
            cp1->heading_AHARS_deg_mag      = XPLMGetDataf(heading_AHARS_deg_mag_copilot_ref);
            cp1->heading_electric_deg_mag   = XPLMGetDataf(heading_electric_deg_mag_copilot_ref);
            cp1->heading_vacuum_deg_mag     = XPLMGetDataf(heading_vacuum_deg_mag_copilot_ref);
            cp1->pitch_AHARS_deg            = XPLMGetDataf(pitch_AHARS_deg_copilot_ref);
            cp1->pitch_electric_deg         = XPLMGetDataf(pitch_electric_deg_copilot_ref);
            cp1->pitch_vacuum_deg           = XPLMGetDataf(pitch_vacuum_deg_copilot_ref);
            cp1->radio_altimeter_height_ft  = XPLMGetDataf(radio_altimeter_height_ft_copilot_ref);
            cp1->radio_altimeter_dh_lit     = XPLMGetDataf(radio_altimeter_dh_lit_copilot_ref);
            cp1->roll_AHARS_deg             = XPLMGetDataf(roll_AHARS_deg_copilot_ref);
            cp1->roll_electric_deg          = XPLMGetDataf(roll_electric_deg_copilot_ref);
            cp1->roll_vacuum_deg            = XPLMGetDataf(roll_vacuum_deg_copilot_ref);
            cp1->wind_heading_deg_mag       = XPLMGetDataf(wind_heading_deg_mag_ref);
            cp1->wind_speed_kts             = XPLMGetDataf(wind_speed_kts_ref);

            cp1->list[0]    = cp1->systems_avionics_on;
            cp1->list[1]    = cp1->systems_avionics_off;
            cp1->list[2]    = cp1->compass_heading_deg_mag;
            cp1->list[3]    = cp1->airspeed_kts;
            cp1->list[4]    = cp1->altitude_ft;
            cp1->list[5]    = cp1->vvi_fpm;
            cp1->list[6]    = cp1->turn_rate_roll_deg;
            cp1->list[7]    = cp1->turn_rate_heading_deg;
            cp1->list[8]    = cp1->heading_AHARS_deg_mag;
            cp1->list[9]    = cp1->heading_electric_deg_mag;
            cp1->list[10]   = cp1->heading_vacuum_deg_mag;
            cp1->list[11]   = cp1->pitch_AHARS_deg;
            cp1->list[12]   = cp1->pitch_electric_deg;
            cp1->list[13]   = cp1->pitch_vacuum_deg;
            cp1->list[14]   = cp1->radio_altimeter_height_ft;
            cp1->list[15]   = cp1->radio_altimeter_dh_lit;
            cp1->list[16]   = cp1->roll_AHARS_deg;
            cp1->list[17]   = cp1->roll_electric_deg;
            cp1->list[18]   = cp1->roll_vacuum_deg;
            cp1->list[19]   = cp1->wind_heading_deg_mag;
            cp1->list[20]   = cp1->wind_speed_kts;
        }

        if (gP1_enabled == '1') {
            if (gP2_enabled == '1') {
                GaugeInfo* p2 = (GaugeInfo*) malloc(sizeof(GaugeInfo));
                *p2 = *p1;
                gP2_ijq.post(new myjob(p2));
            }

            gP1_ijq.post(new myjob(p1));
        } else {
            gP2_ijq.post(new myjob(p1));
        }

        if (gCp1_enabled == '1') {
            if (gCp2_enabled == '1') {
                GaugeInfo* cp2 = (GaugeInfo*) malloc(sizeof(GaugeInfo));
                *cp2 = *cp1;
                gCp2_ijq.post(new myjob(cp2));
            }

            gCp1_ijq.post(new myjob(cp1));
        } else {
            gCp2_ijq.post(new myjob(cp1));
        }
    }

    gFlCbCnt++;

    return 1.0;
}

/*
 *
 */
PLUGIN_API void
XPluginStop(void) {

    GaugeInfo* s;

    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
    s->sys_magic = false;
    gP1_ijq.post(new myjob(s));

    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
    s->sys_magic = false;
    gP2_ijq.post(new myjob(s));

    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
    s->sys_magic = false;
    gCp1_ijq.post(new myjob(s));

    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
    s->sys_magic = false;
    gCp2_ijq.post(new myjob(s));

    pexchange((int*)&threads_run, false);

    gP1Trigger.post();
    gP2Trigger.post();
    gCp1Trigger.post();
    gCp2Trigger.post();

    psleep(500);
    XPLMUnregisterFlightLoopCallback(FlightLoopCallback, NULL);
}

/*
 *
 */
PLUGIN_API void
XPluginDisable(void) {

    gEnabled = false;
    gP1Trigger.reset();
    gP2Trigger.reset();
    gCp1Trigger.reset();
    gCp2Trigger.reset();

    GaugeInfo* s;

    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
    s->sys_magic = false;
    gP1_ijq.post(new myjob(s));

    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
    s->sys_magic = false;
    gP2_ijq.post(new myjob(s));

    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
    s->sys_magic = false;
    gCp1_ijq.post(new myjob(s));

    s = (GaugeInfo*) malloc(sizeof(GaugeInfo));
    s->sys_magic = false;
    gCp2_ijq.post(new myjob(s));
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

    gP1Trigger.post();
    gP2Trigger.post();
    gCp1Trigger.post();
    gCp2Trigger.post();

    return 1;
}

PLUGIN_API void
XPluginReceiveMessage(XPLMPluginID inFrom, long inMsg, void* inParam) {
    // nothing
}
