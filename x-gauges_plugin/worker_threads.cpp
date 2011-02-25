// Copyright (c) 2011 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#if defined(__WINDOWS__)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include "ptypes.h"
#include "pinet.h"

#include "defs.h"
#include "nedmalloc.h"
#include "overloaded.h"
#include "plugin.h"
#include "worker_threads.h"


USING_PTYPES

const int MSG_MYJOB = MSG_USER + 1;
int volatile threads_run = false;

WorkerThread* p1 = 0;
WorkerThread* p2 = 0;
WorkerThread* cp1 = 0;
WorkerThread* cp2 = 0;

jobqueue    gP1_ijq;
jobqueue    gP2_ijq;
jobqueue    gCp1_ijq;
jobqueue    gCp2_ijq;

trigger     gP1Trigger(false, false);
trigger     gP2Trigger(false, false);
trigger     gCp1Trigger(false, false);
trigger     gCp2Trigger(false, false);


/**  obj(p_smt->peer_addr, p_smt->peer_port)
 *
 */
void WorkerThread::execute() {

    message* msg;
    GaugeInfo* s;

//    memset(buf, 0, OUT_BUF_CNT);

    while (threads_run) {
        state->wait();

        msg = ijq->getmessage(MSG_WAIT);
//XPLMSpeakString("received\n");
        s = ((myjob*) msg)->buf;

// TODO: check if avionics are on or off
// TODO: serialize the struct data and send via udp
//            try {
//                udp.send((const char*) udpSnd_buf, u8_snd_cnt);
//            } catch(estream* e) {
//                perr.putf("ClientThread jobqueue error: %s\n", pconst(e->get_message()));
//                delete e;
//            }

//end:
        free(s);
        delete msg;
    }

    DPRINTF_VA("X-Gauge Plugin: thread %d says goodbye\n", id);
}

bool WorkerThread::net_config(pt::ipaddress ip, int port) {
    bool err = false;

    if (!udp) {
        free(udp);
        udp = 0;
    }

    try {
        udp = (ipmessage*) new ipmessage(ip, port);
    } catch (estream* e) {
        DPRINTF_VA("NET CONFIG Error - id: %d, %s\n", id, (const char*)e->get_message());
        udp = 0;
        delete e;
        err = true;
    }

    return err;
}


