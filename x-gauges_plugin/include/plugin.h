// Copyright (c) 2010 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#ifndef PLUGIN_H
#define PLUGIN_H

#define ELEMENT_CNT     (21)

typedef struct tagGaugeInfo {
    int sys_magic;
    float systems_avionics_on;
    float systems_avionics_off;
    float compass_heading_deg_mag;
    float airspeed_kts;
    float altitude_ft;
    float vvi_fpm;
    float turn_rate_roll_deg;
    float turn_rate_heading_deg;
    float heading_AHARS_deg_mag;
    float heading_electric_deg_mag;
    float heading_vacuum_deg_mag;
    float pitch_AHARS_deg;
    float pitch_electric_deg;
    float pitch_vacuum_deg;
    float radio_altimeter_height_ft;
    float radio_altimeter_dh_lit;
    float roll_AHARS_deg;
    float roll_electric_deg;
    float roll_vacuum_deg;
    float wind_heading_deg_mag;
    float wind_speed_kts;
    float list[ELEMENT_CNT];
} GaugeInfo;

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
