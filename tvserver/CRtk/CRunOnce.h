#ifndef C_RUN_ONCE_H
#define C_RUN_ONCE_H

#include <IRunnable.h>

#include <pthread.h>
#include <string.h>
#include <CAttachThread.h>

#define RUN_STATE_INIT 0
#define RUN_STATE_RUNNING 1
#define RUN_STATE_HALTED 2

class CRunOnce: public IThread
{
private:
    pthread_mutex_t m_mutex;
    int m_state;
    pthread_t m_thread;
    IParams *m_params;
    IRunnable *m_runnable;
    static void thread_exit(int sig)
    {
        TvServerService_detachThread();
        pthread_exit(0);
    }
    static void * looper_func(void *p)
    {
        CRunOnce *r = (CRunOnce*)p;
        
        //assert(r && r->m_runnable);

        struct sigaction actions;
        memset(&actions, 0, sizeof(actions)); 
        // sigemptyset(&actions.sa_mask);
        actions.sa_flags = SA_SIGINFO; 
        actions.sa_handler = CRunOnce::thread_exit;
        sigaction(SIGUSR2,&actions,NULL);

		TvServerService_attachThread();
        r->m_runnable->Run(r->m_params);
        TvServerService_detachThread();
        return NULL;
    }
public:
    virtual void Post(IRunnable *r, bool replace, IParams *p, unsigned int after )
    {
        if(!m_runnable && r)
        {
            m_runnable = r;
            m_params = p;
            pthread_mutex_lock(&m_mutex);
            if(m_state == RUN_STATE_INIT)
            {
                m_state = RUN_STATE_RUNNING;
                int ret = pthread_create(&m_thread, NULL,
                       CRunOnce::looper_func, this);
				// pthread_detach(m_thread);
            }
            //assert(!ret);
            pthread_mutex_unlock(&m_mutex);
            
        }
    }
    CRunOnce()
    {
        m_mutex = PTHREAD_MUTEX_INITIALIZER;
        m_state = RUN_STATE_INIT;
        m_runnable = NULL;  
        m_params = NULL;
    }
     
    virtual void Halt(void)
    {
        bool tohalt=false;
        pthread_mutex_lock(&m_mutex);
        if(m_state == RUN_STATE_RUNNING)
        {
            m_state = RUN_STATE_HALTED;
            tohalt = true;
        }
        pthread_mutex_unlock(&m_mutex);
        if(tohalt)
        {
            pthread_kill(m_thread, SIGUSR2);
            pthread_join(m_thread,NULL);
        }
    }

    virtual ~CRunOnce()
    {
        Halt();
        if(m_runnable)
        {
            if(m_params) m_params->Release();
            delete m_runnable;
        }
    }
};
#endif
