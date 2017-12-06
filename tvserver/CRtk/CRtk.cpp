#include <CRtk.h>
#include "CRunOnce.h"
#include "CMsgQueue.h"
#include <CAttachThread.h>

void CRtk::Halt(void)
{
	for(int i = 0; i < MAX_THREADS; i++)
		if(m_threads[i] != NULL)
			m_threads[i]->Halt();
}

int CRtk::TaskCreate(char * name,int priority,int stacksize,FunctionInt entry,int param)
{
	int ret = 0;
	IRunnable *r = reinterpret_cast<IRunnable*>(entry);
	for(int i = 0; i < MAX_THREADS; i++)
	{
		if(!m_threads[i])
		{
			ret = i;
			m_threads[i] = new CRunOnce();
			//assert(m_threads[i]);
			m_threads[i]->Post(r,false,NULL,0);
			break;
		}
	}
	return ret;
}

int CRtk::MsgQCreate(int maxmsg,int maxmsglength)
{
	int ret = 0;
	for(int i = 0; i < MAX_QS; i++)
	{
		if(!m_qs[i])
		{
			ret = i;
			m_qs[i] = new CMsgQueue(maxmsglength,maxmsg);
			//assert(m_qs[i]);
			break;
		}
	}
	return ret;
}

Bool CRtk::MsgQSend(int msgq,Address buffer,int length,int timeout)
{
	Bool ret = 0;
	if((msgq < MAX_QS) && (m_qs[msgq]))
		ret = (Bool)m_qs[msgq]->Send(buffer);
	return ret;
}

int CRtk::MsgQReceive(int msgq,Address buffer,int maxlength,int timeout)
{
	int ret = 0;
	if((msgq < MAX_QS) && (m_qs[msgq]))
	{
		m_qs[msgq]->Receive(buffer);
		ret = 1;
	}
	return ret;
}

Semaphore CRtk::SemCreate( void ) 
{
	if(mtx_count < 100)
	{
		if (pthread_mutex_init(&m_mtxs[mtx_count], NULL) == 0)
		{
			return &m_mtxs[mtx_count++];
		}		
	}
	else
	{
		ASSERT(FALSE);	
	}
	return NULL;	
}

void CRtk::SemAcquire( Semaphore sem ) 
{
	pthread_mutex_lock((pthread_mutex_t *)sem);
}

void CRtk::SemRelease( Semaphore sem ) 
{
	pthread_mutex_unlock((pthread_mutex_t *)sem);
}

void CRtk::SemDelete( Semaphore sem ) 
{
	pthread_mutex_destroy ((pthread_mutex_t *)sem);
}

Nat32 CRtk::TimeGetCurrent(void)
{
	time_t ptime = NULL;
	time(&ptime);
	return ((Nat32)ptime);
}

void CRtk::TaskSleep( int timeout )
{
    int result = 0;
    struct timespec rqtp;
    struct timespec rmtp;

    if (timeout > 0)
    {
        rqtp.tv_sec = timeout / 1000;
        rqtp.tv_nsec = 1000000 * ( timeout % 1000 );        
        do
        {
            result = nanosleep( &rqtp, &rmtp );
            rqtp = rmtp;
        } while( result == -1 && errno == EINTR );        
    }        
} 

Nat32 CRtk::TimeAdd(Nat32 a,Nat32 b)
{
	return (a + b);
}

Nat32 CRtk::TimeSubtract(Nat32 a,Nat32 b)
{
	return (a - b);
}

CRtk::~CRtk()
{
	for(int i = 0; i < MAX_QS; i++)
		if(m_qs[i])
			delete m_qs[i];
	for(int i = 0; i < MAX_THREADS; i++)
		if(m_threads[i])
			delete m_threads[i];
	for (int i = 0; i < mtx_count; i++) {
		pthread_mutex_destroy (&m_mtxs[i]);
	}
}

static JavaVM* mVm = NULL;

extern "C" jint JNI_OnLoad(JavaVM* vm, void* p)
{
    JNIEnv* env = NULL;
    jint result = -1;
    ;

    mVm = vm;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ;
    }
    result = JNI_VERSION_1_4;
    return result;
}

void TvServerService_attachThread()
{
    JNIEnv *g_env;
	if (mVm != NULL)
	{
		// double check it's all ok
	     int getEnvStat = mVm->GetEnv((void **)&g_env, JNI_VERSION_1_4);
	     if (getEnvStat == JNI_EDETACHED) {
	         ;
	         if (mVm->AttachCurrentThread(&g_env, NULL) != 0) {
	             ;
	         }
	     } else if (getEnvStat == JNI_OK) {
	         //
	     } else if (getEnvStat == JNI_EVERSION) {
	     }
	}
}

void TvServerService_detachThread()
{
    JNIEnv *g_env;
	if (mVm != NULL)
	{
		// double check it's all ok
	     int getEnvStat = mVm->GetEnv((void **)&g_env, JNI_VERSION_1_4);
	     if (getEnvStat == JNI_EDETACHED) {
	        
	     } else if (getEnvStat == JNI_OK) {
	         ;
	         if (mVm->DetachCurrentThread() != 0) {
	             ;
	         }
	     } else if (getEnvStat == JNI_EVERSION) {
	     }
	}
}
