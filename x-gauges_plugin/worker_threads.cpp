// Copyright (c) 2011 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#if defined(__WINDOWS__)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include "ptypes.h"
#include "pinet.h"

#include "nedmalloc.h"
#include "overloaded.h"
#include "defs.h"
#include "plugin.h"
#include "utils.h"
#include "worker_threads.h"


USING_PTYPES

const int MSG_MYJOB = MSG_USER + 1;
int volatile threads_run = false;

WorkerThread* p1 = 0;
WorkerThread* p2 = 0;
WorkerThread* cp1 = 0;
WorkerThread* cp2 = 0;

jobqueue gP1_ijq;
jobqueue gP2_ijq;
jobqueue gCp1_ijq;
jobqueue gCp2_ijq;

trigger gP1Trigger(false, false);
trigger gP2Trigger(false, false);
trigger gCp1Trigger(false, false);
trigger gCp2Trigger(false, false);


/**
 *
 */
void WorkerThread::execute() {

    message* msg;
    GaugeInfo* s;

    // floats packed into uint32_t
    uint8_t* buf = (uint8_t*) malloc(ELEMENT_CNT * sizeof(float));
    uint32_t* ptr;

    while (threads_run) {
        state->wait();

        msg = ijq->getmessage(MSG_WAIT);
//XPLMSpeakString("received\n");
        s = ((myjob*) msg)->buf;

        if (s->sys_magic != SYS_MAGIC_NUM)
            goto end;

// TODO: check if avionics are on or off
        ptr = (uint32_t*)(&buf[0]);
        for (int i = 0; i < ELEMENT_CNT; i++) {
            *ptr++ = (uint32_t) pack754_32(s->list[i]);
        }

        try {
            udp->send((const char*)buf, ELEMENT_CNT * sizeof(float));
        } catch (estream* e) {
            perr.putf("ClientThread jobqueue error: %s\n", pconst(e->get_message()));
            delete e;
        }

end:
        free(s);
        delete msg;
    }

    free(buf);

    DPRINTF_VA("X-Gauge Plugin: thread %d says goodbye\n", id);
}

bool WorkerThread::net_config(pt::string ip, pt::string port) {

    bool err = false;

    if (udp) {
        free(udp);
        udp = 0;
    }

    ipaddress iip = phostbyname(ip);

    if (iip == ipnone) {
        err = true;
    } else {
        try {
            udp = (ipmessage*) new ipmessage(iip, stringtoi(port));
        } catch (estream* e) {
            DPRINTF_VA("NET CONFIG Error - id: %d, %s\n", id, pconst(e->get_message()));
            udp = 0;
            delete e;
            err = true;
        }
    }

    return err;
}


