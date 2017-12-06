#ifndef __PUMP_WATCH_DOG_H__
#define __PUMP_WATCH_DOG_H__

#include "IWatchDog.h"
#include "CWatchDogMessage.h"

#include <android/log.h>
#include <utils/CallStack.h>

#define TRACESYMBOL(x, T) static const char *x = T;
#define TraceNotice(m,...) __android_log_print(ANDROID_LOG_DEBUG, m, __VA_ARGS__)

#define BITE_INTERVAL_MS (50000)
#define WATCHDOG_POLL_INTERVAL	(2000)

TRACESYMBOL(m, "PumpWatchDog");

class CAutoLock
{
private:
    pthread_mutex_t *m_mtx;
public:
    CAutoLock(pthread_mutex_t *m) { m_mtx = m; pthread_mutex_lock(m);}
    ~CAutoLock() { pthread_mutex_unlock(m_mtx); }
};

class PumpWatchDog;

class Final {
private:
	Final() {}
	friend class PumpWatchDog;
};

/* Singleton class. Only one watch dog thread per process */
class PumpWatchDog : public IRunnable, public IWatchDog, virtual Final
{
private:
	sp<Looper> 		wdLooper;
    pthread_t 		wdThread;
    pthread_mutex_t wdMutex;
    PumpLogger      *m_logger;

	static PumpWatchDog	*thiz;

	static void * looper_task(void *p)
    {
        PumpWatchDog *r = (PumpWatchDog*)p;

        int ret = 0;
        while(1)
        {
            ret = r->wdLooper->pollOnce(WATCHDOG_POLL_INTERVAL, NULL, NULL, NULL);
        }

        return NULL;
    }

	PumpWatchDog()
	{
		//TraceNotice(m,"PumpWatchDog constructor");
        wdMutex = PTHREAD_MUTEX_INITIALIZER;
		wdLooper = new Looper(true);
		pthread_create(&wdThread, NULL, PumpWatchDog::looper_task, this);
		pthread_setname_np(wdThread, "PumpWatchDog");
	}


public:
	static PumpWatchDog * getWatchDogInstance () {
		/* Assuming pumps are created in sequence of INIT so for analysis and inital phase
		* this is there with a critical section. Will be visited again . 		*/
		if (NULL == thiz) {
			thiz = new PumpWatchDog();
		}
		return thiz;
	}

	void bark (sp<CWatchDogMessage> &msg, int what, unsigned int milliSec)
	{
        nsecs_t nafter = ((nsecs_t)milliSec) * 1000000LL;

        //TraceNotice(m,"bark:Enter");
        CAutoLock cs(&wdMutex);
        
        Message 	wht(what);

        wdLooper->removeMessages (msg, what);

        wdLooper->sendMessageDelayed(nafter, msg, wht);
        //TraceNotice(m,"bark:Exit");
	}

	virtual void Run(IParams *p)
	{
		TraceNotice(m,"Run");
		 dogBite((pid_t)p);
	}

	void dogBite(pthread_t param1)
	{
		TraceNotice(m,"BOG BITE: tid %lu", param1);
        // raise(SIGUSR2);
        LogPumpData();
		CallStack	bt;
        bt.update(1, param1);
        bt.log(m, ANDROID_LOG_ERROR, 0);
		abort();
	}

	virtual void SendAlive (sp<CWatchDogMessage> &msg, int what) {
		bark(msg, what, BITE_INTERVAL_MS); /* Half a minute */
	}

	virtual int Halt (sp<CWatchDogMessage> &msg, int what)	{
		CAutoLock cs(&wdMutex);
		wdLooper->removeMessages (msg, what);
		return 0;
	}

	virtual sp<CWatchDogMessage> createWdMessage(pthread_t tid) {
		return new CWatchDogMessage(tid,static_cast<IRunnable*>(this));
	}

    void setPumpLogger (PumpLogger *m_ptr) {
        m_logger = m_ptr;
    }

    void LogPumpData () {
        if (m_logger == NULL) {
            return;
        }
        FILE *fp = fopen ("/tmp/CircularBuffer.txt", "w");
        if (fp!= NULL) {
            m_logger->writeToFile(fp);
            fclose (fp);
        }
    }

};

PumpWatchDog * PumpWatchDog::thiz = NULL;

#endif
