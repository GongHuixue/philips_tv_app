#include <CLooperPumpEngines.h>
#include "CLooperRunnabler.h"
#include "PumpWatchDog.h"

#define ENABLE_PUMP_WATCHDOG

PumpLogger * PumpLogger::thiz = NULL;

CLooperPumpEngines::CLooperPumpEngines()
{
	// FILE *fptr = NULL;

#ifdef ENABLE_PUMP_WATCHDOG
	m_watchDog = PumpWatchDog::getWatchDogInstance();
    m_pumpLogger = PumpLogger::getPumpLoggerInstance();
    // m_pumpLogger = NULL;
#else
    m_watchDog = NULL;
    m_pumpLogger = NULL;
#endif
#if 0
	{
	    int len = 0;
	    char dir[50] = {0};
	    char logFile[250] = {0};

	    snprintf(dir, sizeof(dir), "/proc/%d/cmdline", getpid());

	    FILE *fp = fopen (dir, "r");
	    int length = sizeof(dir);
	    while (length-- && !feof(fp)) {
	        dir[len++] = fgetc(fp);
	    }
	    fclose(fp);
	    snprintf(logFile, sizeof(logFile), "/data/data/%s/%d_alive.log", dir,getpid());
	    fptr = fopen(logFile, "w");
	}
#endif
    m_watchDog->setPumpLogger(m_pumpLogger);
	m_sundry = new CLooperRunnabler(static_cast<IWatchDog *>(m_watchDog), "Pen-Sundry", m_pumpLogger);
	m_priority = new CLooperRunnabler(static_cast<IWatchDog *>(m_watchDog), "Pen-Priority", m_pumpLogger);
	sundry = m_sundry;
	priority = m_priority;


    // m_sundry->setWatchDog(static_cast<IWatchDog *>(m_watchDog));
	// m_priority->setWatchDog(static_cast<IWatchDog *>(m_watchDog));
}

void CLooperPumpEngines::Halt(void)
{
	m_sundry->Halt();
	m_priority->Halt();
}

CLooperPumpEngines::~CLooperPumpEngines()
{
	if(m_sundry)
		delete m_sundry;
	if(m_priority)
		delete m_priority;
}
