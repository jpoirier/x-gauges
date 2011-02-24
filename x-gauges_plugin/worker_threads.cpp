// Copyright (c) 2011 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#include <cstring>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "pport.h"
#include "ptypes.h"
#include "pstreams.h"

#include "XPLMUtilities.h"

#include "defs.h"
#include "utils.h"
#include "client_thread.h"
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
trigger     gP1Trigger(true, false);
trigger     gP2Trigger(true, false);
trigger     gCp1Trigger(false, false);
trigger     gCp2Trigger(false, false);


/**
 *
 */
void ToPanelThread::execute() {

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

end:
        free(s);
        delete msg;
    }

    DPRINTF("X-Gauge Plugin: thread %s says goodbye\n", id);
}


