#ifndef CRTK_H_
#define CRTK_H_

#include <provreq.h>
#include <InfraGlobals.h>
#include <ICeIsTpHostRtk2.h>
#include <IRealTimeKernel.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <sched.h>

class CRunOnce;
class CMsgQueue;
//move to vector / array later if reqd
#define MAX_QS 32
#define MAX_THREADS 32
#define MAX_MUTEXS 100

class CRtk 
{
private:
    CRunOnce* m_threads[MAX_THREADS];
    CMsgQueue* m_qs[MAX_QS]; 
    pthread_mutex_t m_mtxs[MAX_MUTEXS];
    int mtx_count;
    static CRtk* rtk;

public:
    static CRtk* RtkInstance()
    {
        for(int i = 0; i < MAX_QS; i++)
            m_qs[i] = NULL;
        for(int i = 0; i < MAX_THREADS; i++)
            m_threads[i] = NULL;

        mtx_count = 0;

        if(rtk == NULL) {
            rtk = new CRtk();
	    }
	    return rtk;
    }

    ~CRtk() {
        if(rtk != NULL) {
            delete rtk;
            rtk = NULL;
	    }
	}
};

#endif
