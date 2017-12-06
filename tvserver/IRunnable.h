#ifndef IRUNNABLE_H
#define IRUNNABLE_H

class IParams
{
public:
    virtual ~IParams(){}
	virtual void Release(void) = 0;
	virtual const char * getInfo(int *p1, int *p2) = 0;
};

class IRunnable
{
private:
	void *m_key;
public:
	IRunnable():m_key(0){}
    virtual ~IRunnable(){}
	virtual void Run(IParams *p) = 0;
	inline  void*&      operator* ()  { return m_key; }
	virtual void RunDebug (const char *info) {;}
	
};

class IThread
{
public:
    virtual ~IThread(){}
	virtual void Post(IRunnable *r, bool replace, IParams *p, unsigned int after ) = 0;
	virtual void Halt(void) = 0;
	virtual void Log(const char *msg) {;}
};

#endif
