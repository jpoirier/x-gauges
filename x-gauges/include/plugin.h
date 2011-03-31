// Copyright (c) 2010 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#ifndef PLUGIN_H
#define PLUGIN_H

#define ELEMENT_CNT     (19)


// sim/cockpit2/clock_timer/elapsed_time_hours      int	n	hours	    Elapsed time on the timer, hours
// sim/cockpit2/clock_timer/elapsed_time_minutes    int	n	minutes	    Elapsed time on the timer, minutes
// sim/cockpit2/clock_timer/elapsed_time_seconds    int	n	seconds	    Elapsed time on the timer, seconds
// sim/cockpit2/clock_timer/zulu_time_hours         int	n	hours	    Zulu time, hours
// sim/cockpit2/clock_timer/zulu_time_minutes	       int	n	minutes	    Zulu time, minutes
// sim/cockpit2/clock_timer/zulu_time_seconds	       int	n	seconds	    Zulu time, seconds
// sim/cockpit2/clock_timer/local_time_hours	       int	n	hours	    Local time, hours
// sim/cockpit2/clock_timer/local_time_minutes      int	n	minutes 	Local time, minutes
// sim/cockpit2/clock_timer/local_time_seconds	   int	n	seconds 	Local time, seconds
// sim/cockpit2/clock_timer/hobbs_time_hours	        int	n	hours	    Hobbs meter time, hours
// sim/cockpit2/clock_timer/hobbs_time_minutes	    int	n	minutes 	Hobbs meter time, minutes
// sim/cockpit2/clock_timer/hobbs_time_seconds	    int	n	seconds 	Hobbs meter time, seconds

struct GaugeInfo {
    int sys_magic;
    int avionics_power_on;
//  0   compass_heading_deg_mag;
//  1   airspeed_kts;
//  2   altitude_ft;
//  3   vvi_fpm;
//  4   turn_rate_roll_deg;
//  5   turn_rate_heading_deg;
//  6   heading_AHARS_deg_mag;
//  7   heading_electric_deg_mag;
//  8   heading_vacuum_deg_mag;
//  9   pitch_AHARS_deg;
// 10   pitch_electric_deg;
// 11   pitch_vacuum_deg;
// 12   radio_altimeter_height_ft;
// 13   radio_altimeter_dh_lit;
// 14   roll_AHARS_deg;
// 15   roll_electric_deg;
// 16   roll_vacuum_deg;
// 17   wind_heading_deg_mag;
// 18   wind_speed_kts;
    float list[ELEMENT_CNT];
};

#ifdef __cplusplus
extern "C" {
#endif

    extern pt::string gP1_ip;
    extern pt::string gP1_port;
    extern pt::string gP1_enabled;
    extern pt::string gP2_ip;
    extern pt::string gP2_port;
    extern pt::string gP2_enabled;
    extern pt::string gCp1_ip;
    extern pt::string gCp1_port;
    extern pt::string gCp1_enabled;
    extern pt::string gCp2_ip;
    extern pt::string gCp2_port;
    extern pt::string gCp2_enabled;

#ifdef __cplusplus
}
#endif

#endif
