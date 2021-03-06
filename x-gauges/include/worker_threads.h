// Copyright (c) 2010 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

#ifndef WORKER_THREADS_H
#define WORKER_THREADS_H


/**
 * @class WorkerThread
 *
 * @param
 * @param
 */
class WorkerThread : public pt::thread {
    protected:
        uint8_t         buf[ELEMENT_CNT * sizeof(float)];
        uint32_t*       ptr;
        GaugeInfo*      s;
        pt::jobqueue*   ijq;
        pt::trigger*    state;
        pt::ipmessage*  udp;
        pt::message*    msg;
        int             id;
        int             port;

        virtual void execute();
        virtual void cleanup() {}

    public:
        WorkerThread(int iid, pt::jobqueue* iiq, pt::trigger* itrigger)
                        : thread(true), id(iid), ijq(iiq), state(itrigger), udp(0) {}
        ~WorkerThread() {}

        bool net_config(pt::string ip, pt::string port);
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

    extern int volatile threads_run;

    extern pt::trigger gP1Trigger;
    extern pt::trigger gP2Trigger;
    extern pt::trigger gCp1Trigger;
    extern pt::trigger gCp2Trigger;

    extern pt::jobqueue gP1_ijq;
    extern pt::jobqueue gP2_ijq;
    extern pt::jobqueue gCp1_ijq;
    extern pt::jobqueue gCp2_ijq;

    extern WorkerThread* p1;
    extern WorkerThread* p2;
    extern WorkerThread* cp1;
    extern WorkerThread* cp2;

#ifdef __cplusplus
}
#endif

#endif
