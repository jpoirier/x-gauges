// Copyright (c) 2010 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#ifndef THREADS_H
#define THREADS_H

#include "pasync.h"
#include "defs.h"
#include "plugin.h"


/**
 * @class WorkerThread
 *
 * @param
 * @param
 */
class WorkerThread : public pt::thread {
    protected:
        int             id;
        pt::jobqueue*   ijq;
        pt::trigger*    state;

//        unsigned char   buf[IN_BUF_CNT];

        virtual void execute();
        virtual void cleanup() {}

    public:
        WorkerThread(int iid, pt::jobqueue* iiq, pt::trigger* itrigger)
                        : thread(true), id(iid) ijq(iiq), state(itrigger) {}
        ~WorkerThread() {}
};


/**
 * @class myjob
 * Inherits from PTypes message class. A queue item consists
 * of a pointer to a dynamically allocated structure.
 *
 * @param GaugeInfo* pbuf
 */
class myjob : public pt::message {
    public:
        GaugeInfo* buf;

        myjob(GaugeInfo* pbuf) : message(pt::MSG_USER + 1), buf(pbuf) {}
        ~myjob()  {}
};

#ifdef __cplusplus
extern "C" {
#endif

    extern pt::trigger gP1Trigger;
    extern pt::trigger gP2Trigger;
    extern pt::trigger gCp1Trigger;
    extern pt::trigger gCp2Trigger;

    extern pt::jobqueue gP1_ijq;
    extern pt::jobqueue gP2_ijq;
    extern pt::jobqueue gCp1_ijq;
    extern pt::jobqueue gCp2_ijq;

    extern int volatile threads_run;

#ifdef __cplusplus
}
#endif

#endif
