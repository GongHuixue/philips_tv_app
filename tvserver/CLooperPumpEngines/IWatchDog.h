#ifndef __IWATCH_DOG_H__
#define __IWATCH_DOG_H__

class IWatchDog {
public:
	virtual ~IWatchDog() {}
	virtual void SendAlive (sp<CWatchDogMessage> &msg, int what) = 0;
	virtual int Halt (sp<CWatchDogMessage> &msg, int what) = 0;
	virtual sp<CWatchDogMessage> createWdMessage(pthread_t tid) = 0;
};

#endif
