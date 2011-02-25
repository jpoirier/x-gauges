// Copyright (c) 2011 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#if defined(__WINDOWS__)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include "ptypes.h"
#include "pstringx.h"
#include "pinet.h"

#include "XPLMUtilities.h"

#include "nedmalloc.h"
#include "overloaded.h"
#include "defs.h"
#include "plugin.h"
#include "worker_threads.h"
#include "config.h"

USING_PTYPES

const int KEY_CNT = 12;
const string CFG_FILE = "./Resources/plugins/x-gauges.cfg";

string config_keys[KEY_CNT] = {"p1_ip", "p1_port", "p1_enabled", "p2_ip",
                               "p2_port", "p2_enabled", "cp1_ip", "cp1_port",
                               "cp1_enabled", "cp2_ip", "cp2_port", "cp2_enabled"};

void read_config() {

    infile f(CFG_FILE);

    try {
        f.open();
    } catch (estream* e) {
        DPRINTF_VA("Open Config Read File Error: %s\n", (const char*)e->get_message());
        delete e;
        return;
    }

    int i;
    string key;
    string val;

    try {
        while (!f.get_eof()) {
            string l = f.line();

            if (isempty(l))
                continue;

            if ((i = pos('=', l)) == -1) {
                continue;
            }

            key = trim(copy(l, 0, i));
            val = trim(copy(l, i+1, length(l)-1));

            if (isempty(key) || isempty(val)) {
                continue;
            }

            for (i = 0; i < KEY_CNT; ++i) {
                if (key == config_keys[i]) {
                    switch (i) {
                        case 0:
                            gP1_ip = val;
                            break;
                        case 1:
                            gP1_port = val;
                            break;
                        case 2:
                            gP1_enabled = val;
                            break;
                        case 3:
                            gP2_ip = val;
                            break;
                        case 4:
                            gP2_port = val;
                            break;
                        case 5:
                            gP2_enabled = val;
                            break;
                        case 6:
                            gCp1_ip = val;
                            break;
                        case 7:
                            gCp1_port = val;
                            break;
                        case 8:
                            gCp1_enabled = val;
                            break;
                        case 9:
                            gCp2_ip = val;
                            break;
                        case 10:
                            gCp2_port = val;
                            break;
                        case 11:
                            gCp2_enabled = val;
                            break;
                    }

                    break;
                } // if
            } // for
        } // while
    } catch (estream* e) {
        DPRINTF_VA("Read Config File Error: %s\n", (const char*)e->get_message());
        delete e;
    }

    f.close();
}

void write_config() {

    outfile f(CFG_FILE);

    try {
        f.open();
    } catch (estream* e) {
        DPRINTF_VA("Open Config Write File Error: %s\n", (const char*)e->get_message());
        delete e;
        return;
    }

    try {
        f.putline(config_keys[0] + "=" + gP1_ip);
        f.putline(config_keys[1] + "=" + gP1_port);
        f.putline(config_keys[2] + "=" + gP1_enabled);
        f.putline(config_keys[3] + "=" + gP2_ip);
        f.putline(config_keys[4] + "=" + gP2_port);
        f.putline(config_keys[5] + "=" + gP2_enabled);
        f.putline(config_keys[6] + "=" + gCp1_ip);
        f.putline(config_keys[7] + "=" + gCp1_port);
        f.putline(config_keys[8] + "=" + gCp1_enabled);
        f.putline(config_keys[9] + "=" + gCp2_ip);
        f.putline(config_keys[10] + "=" + gCp2_port);
        f.putline(config_keys[11] + "=" + gCp2_enabled);
    } catch (estream* e) {
        DPRINTF_VA("Write Config File Error: %s\n", (const char*)e->get_message());
        delete e;
    }

    f.close();
}
