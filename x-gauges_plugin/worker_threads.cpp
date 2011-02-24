// Copyright (c) 2011 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#include <cstring>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "ptypes.h"
#include "pport.h"
#include "pstreams.h"

#include "XPLMUtilities.h"

#include "defs.h"
//#include "utils.h"
#include "worker_threads.h"
#include "nedmalloc.h"
#include "overloaded.h"
#include "plugin.h"


USING_PTYPES

const int MSG_MYJOB         = MSG_USER + 1;
int volatile threads_run    = false;

jobqueue    gP1_ijq;
jobqueue    gP2_ijq;
jobqueue    gCp1_ijq;
jobqueue    gCp2_ijq;

trigger     gP1Trigger(false, false);
trigger     gP2Trigger(false, false);
trigger     gCp1Trigger(false, false);
trigger     gCp2Trigger(false, false);


/**
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

//end:
        free(s);
        delete msg;
    }

    DPRINTF_VA("X-Gauge Plugin: thread %d says goodbye\n", id);
}


