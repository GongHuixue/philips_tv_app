#ifndef __WATCHDOG_MSG_H__
#define __WATCHDOG_MSG_H__

class CWatchDogMessage:public MessageHandler
{
private:
	IRunnable 		*m_run;
	pid_t           m_tid;

public:
	CWatchDogMessage(pid_t tid, IRunnable *r): m_run(r), m_tid(tid)
	{
	}
	virtual ~CWatchDogMessage()
	{
		/* Commented as before release is called process is killed */
		// m_params->Release();
	}

	virtual void handleMessage(const Message& message)
	{
		m_run->Run((IParams *)m_tid);
	}
};

#endif
