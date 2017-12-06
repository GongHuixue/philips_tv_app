#ifndef CLOOPER_RUNNABLER_H
#define CLOOPER_RUNNABLER_H

#include <IRunnable.h>
#include <IPumpEngine.h>
#include <pthread.h>
#include <utils/Looper.h>
#include <list>
#include <CAttachThread.h>

#include <signal.h>

#include "PumpLogger.h"

/* Need to check and optimize these poll interval */
#define LOOPER_SEND_ALIVE_INTERVAL  (5000)
#define MAX_DBG_MSG                  (300)

using namespace android;

#include "CWatchDogMessage.h"
#include "IWatchDog.h"

class CriticalSection
{
private:
    pthread_mutex_t *m_mtx;
public:
    CriticalSection(pthread_mutex_t *m) { m_mtx = m; pthread_mutex_lock(m);}
    ~CriticalSection() { pthread_mutex_unlock(m_mtx); }
};

class CLooperParams:public IParams
{
private:
	int m_p1;
	unsigned int m_p2;
public:
	CLooperParams(int p1, unsigned int p2) { m_p1 = p1; m_p2 = p2;}
	int  P1(void)  { return m_p1; }
	unsigned int  P2(void)  { return m_p2; }
    virtual ~CLooperParams(){}
    virtual void Release(void)
    {
        delete this;
    }
    virtual const char * getInfo(int *p1, int *p2) {
        *p1 = 0; *p2 = 0;
        static const char *wdparamtag = "wdParams";
        return wdparamtag;
    }
};

class CMessageHandler:public MessageHandler
{
private:
    IRunnable *m_run;
    pthread_mutex_t *m_mutex;
    IParams *m_params;
    char    info[200];
public:
    CMessageHandler(IRunnable *r, pthread_mutex_t *m, IParams *p):m_run(r),
                                                        m_mutex(m),
                                                        m_params(p)
    {
        **m_run = ((void*)(this));
    }
    virtual ~CMessageHandler()
    {
        m_params->Release();
    }

    virtual void handleMessage(const Message& message)
    {
        int p1,p2;
        const char *pumph = m_params->getInfo(&p1, &p2);
        snprintf(info, sizeof(info), "S >> %s-> p1[%d] p2[%d]", pumph, p1, p2);
        m_run->RunDebug(info);
        m_run->Run(m_params);
        snprintf(info, sizeof(info), "E >> %s-> p1[%d] p2[%d]", pumph, p1, p2);
        m_run->RunDebug(info);
        {
            CriticalSection cs(m_mutex);
            if(**m_run == ((void*)(this)))
                **m_run = NULL;
        }
    }
};

class CLooperRunnabler:public IThread, public IPumpEngine, public IRunnable
{
private:
    sp<Looper> m_looper;
    pthread_t m_thread;
    pthread_mutex_t m_mutex;
    bool m_alive;
	static IWatchDog *m_watchDog; /* Single instance per process...*/
    sp<CWatchDogMessage>    m_wdmsg;
    pid_t                   m_threadid;
    const char *            m_name;
    PumpLogger              *m_logger;
    char                    m_dbgMsg[MAX_DBG_MSG];

    static void * looper_func(void *p)
    {
        CLooperRunnabler *r = (CLooperRunnabler*)p;
        TvServerService_attachThread();
        //Looper::setForThread(r->m_looper); - not required
        int ret = 0;
		int uid = 0;

		struct sigaction act;

        r->m_threadid = gettid();

        if (NULL != m_watchDog) {
            r->m_wdmsg = r->m_watchDog->createWdMessage(r->m_threadid);

            memset (&act, 0, sizeof(act));
            act.sa_sigaction = &siga_handler;
            act.sa_flags = SA_SIGINFO;

            // ret = sigaction (SIGUSR2, &act, NULL);

            r->Run(NULL);
        }

        while(r->m_alive /*&& (ret != ALOOPER_POLL_ERROR )*/)
        {
            /*  TODO: Need to check and optimize the poll intervals */
            ret = r->m_looper->pollOnce(35, NULL, NULL, NULL);
        }

        if (m_watchDog) {
            /* Remove thread from monitor .. */
            m_watchDog->Halt(r->m_wdmsg, reinterpret_cast<int>(r->m_threadid));
        }
        TvServerService_detachThread(); 

        //assert(ret != ALOOPER_POLL_ERROR );
        return NULL;
    }

	static void siga_handler (int sig, siginfo_t *siginfo, void *context)
	{
		// CallStack	bt("PumpWatchDog");
		// bt.update();
		// bt.dump("PumpWatchDog");
		// exit(0);
        abort();
		return;
	}

public:
    CLooperRunnabler(IWatchDog *mwd, const char * name, PumpLogger *ptr):m_alive(true), m_logger(ptr)
    {
        m_mutex = PTHREAD_MUTEX_INITIALIZER;
        m_looper = new Looper(true);
        m_name = name;
        m_watchDog = mwd;
        pthread_create(&m_thread, NULL,
                   CLooperRunnabler::looper_func, this);
        pthread_setname_np(m_thread, name);
		// pthread_detach(m_thread);
    }

    virtual void Halt(void)
    {
        //better mechanisms can be used : for now this would suffice
        if(m_alive)
        {
            if (m_watchDog) {
                /* Remove thread from monitor .. */
                m_watchDog->Halt(m_wdmsg, reinterpret_cast<int>(m_threadid));
            }
            m_alive = false;
            m_looper->wake();
            pthread_join(m_thread,NULL);
        }
    }

    virtual ~CLooperRunnabler()
    {

        Halt();
        pthread_mutex_destroy(&m_mutex);
    }

    virtual void Post(IRunnable *r, bool replace, IParams *p, unsigned int after )
    {
        //CParams* ret = new CParams(r,p);
        nsecs_t nafter = ((nsecs_t)after) * 1000000LL;
        CriticalSection cs(&m_mutex);
        if(replace)
        {
            sp<CMessageHandler> rhdlr = (CMessageHandler*)(**r);
            if(rhdlr.get())
            {
                m_looper->removeMessages(rhdlr );
            }
        }
        m_looper->sendMessageDelayed(nafter,new CMessageHandler(r,&m_mutex,p),0);
    }

    virtual IThread* PumpEngine(void)
    {
        return static_cast<IThread*>(this);
    }

	virtual void Run(IParams *p)
	{
        if (NULL != m_watchDog) {
            m_watchDog->SendAlive(m_wdmsg, reinterpret_cast<int>(m_threadid));
            logupdate("SendAlive");
        }

		Post(static_cast<IRunnable*>(this), false,
				static_cast<IParams *>(new CLooperParams(0xdeafdace,0xdeafdace)),
				LOOPER_SEND_ALIVE_INTERVAL);
	}

    int logupdate (const char *msg)
    {
        struct timespec     ts; 
        if (m_logger != NULL) {
            clock_gettime(CLOCK_REALTIME , &ts);

            time_t when = time(NULL);
            struct tm* ptm = localtime(&when);
            char timeBuf[32];
            strftime(timeBuf, sizeof(timeBuf), "%m-%d %H:%M:%S", ptm);

            snprintf(m_dbgMsg, sizeof(m_dbgMsg), "%s.%03ld %5d %5d %s: %s\n", timeBuf, ts.tv_nsec / 1000000, getpid(), gettid(), m_name, msg);
            m_logger->writeMsg(m_dbgMsg);
        }
        return 0;
    }

    void RunDebug (const char *info) {
        // logupdate(info);
    }

    virtual void Log(const char *msg) {
        logupdate(msg);
    }

};

IWatchDog * CLooperRunnabler::m_watchDog = NULL;


#endif
