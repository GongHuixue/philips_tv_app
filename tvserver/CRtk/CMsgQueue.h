#ifndef CMSG_Q_H
#define CMSG_Q_H

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <utils/Looper.h>
#include <vector>
#include <android/log.h>

using namespace android;

struct AllocExtra
{
    unsigned int magic;
    struct AllocExtra *next;
};

#define ALIGN_8(x) (((x+7)/8)*8)

#define ALLOC_MAGIC 0xdeadbeef


//no thread safety in this class
class CFixedAlloc
{
private:
    int m_sz;
    int m_nr;
    char *m_p;
    struct AllocExtra *m_free;
public:
    CFixedAlloc(int sz, int nr)
    {
        m_sz = ALIGN_8(sz)+ALIGN_8(sizeof(struct AllocExtra));
        m_nr = nr;
        m_p = (char*)malloc(m_sz * m_nr);
        if(m_p)
        {
            struct AllocExtra *p = reinterpret_cast<AllocExtra *>(m_p);
            for(int i = 0; i < m_nr; i++)
            {
                p->magic = ALLOC_MAGIC;
                p->next = reinterpret_cast<AllocExtra *>(reinterpret_cast<char *>(p)+m_sz);
                if(i+1 == m_nr)
                    p->next = NULL;
                else
                    p = p->next;
            }
        }
        m_free = reinterpret_cast<AllocExtra *>(m_p);
    }
    ~CFixedAlloc()
    {
        if(m_p)
            free(m_p);
    }
    void *Alloc(void)
    {
        char *ret = NULL;
        if(m_free)
        {
            ret = reinterpret_cast<char *>(m_free)+ALIGN_8(sizeof(struct AllocExtra));
            m_free = m_free->next;
        }
        return (void*)ret;
    }
    void Free(void *p)
    {
        AllocExtra *a = reinterpret_cast<AllocExtra *>(reinterpret_cast<char *>(p)-ALIGN_8(sizeof(struct AllocExtra)));
        //assert(a->magic == ALLOC_MAGIC);
        if (m_free) {
            a->next = m_free;
        }
        else {
            a->next = NULL;
        }
        m_free = a;
    }
};

using std::vector;
class CMsgQueue
{
private:
    vector<void *> m_q;
    CFixedAlloc m_fa;
    pthread_mutex_t m_mutex;
    sp<Looper> m_looper;
    int m_sz;
public:
    CMsgQueue(int sz, int nr):m_fa(sz,nr),m_sz(sz)
    {
        m_mutex = PTHREAD_MUTEX_INITIALIZER;
        m_looper = new Looper(false);
        //assert(m_looper);
    }
    void Receive(void *msg)//infinite wait for now
    {
        while(m_q.empty())
            m_looper->pollOnce(-1, NULL, NULL, NULL);
        pthread_mutex_lock(&m_mutex);
        memcpy(msg,m_q[0],m_sz);
        m_fa.Free(m_q[0]);
        m_q.erase (m_q.begin());
        pthread_mutex_unlock(&m_mutex);
    }
    int Receive(void *msg, int timeout)
    {
        int ret = 0;
        if(m_q.empty())
        {
            ret = m_looper->pollOnce(timeout, NULL, NULL, NULL);
        }
        pthread_mutex_lock(&m_mutex);
        if(!m_q.empty())
        {
            ret = 1;    
            memcpy(msg,m_q[0],m_sz);
            m_fa.Free(m_q[0]);
            m_q.erase (m_q.begin());
        }
        if (ret < 0)
            ret = 0;
        pthread_mutex_unlock(&m_mutex);       
        return ret;
    }    
    int Send(void *msg)
    {
        pthread_mutex_lock(&m_mutex);
        void *p = m_fa.Alloc();
        if(p)
        {
            memcpy(p,msg,m_sz);
            m_q.push_back(p);
        }
        pthread_mutex_unlock(&m_mutex);
        if(p) {
            m_looper->wake();
        }

        return (int)p;
    }
    ~CMsgQueue()
    {
    }
};

#endif
