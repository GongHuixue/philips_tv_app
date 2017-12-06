#ifndef __HSVSRTDB_H__
#define __HSVSRTDB_H__

#include "IHsvSortedTableConfigurationEx.h"
#include "hsvmempool.h"

#define BIN_SEARCH

using namespace std;

#define OperatorEqualMask ((int)0x1)
#define OperatorSmallerMask ((int)0x2)
#define OperatorGreaterMask ((int)0x4)

#define PrimaryKeyMask ((Nat8)0x1)
#define SecondaryKeyMask ((Nat8)0x2)
#define TeritiaryKeyMask ((Nat8)0x4)
#define QuaternaryKeyMask ((Nat8)0x8)

#define SmallerKey  (Nat8)0
#define EqualKey        (Nat8)0x1
#define BiggerKey   ((Nat8)0x2)

#define MUTEX_CREATE(__X__) pthread_mutex_init(&srtdb_mutex, NULL)
#define MUTEX_LOCK(__X__) pthread_mutex_lock(&srtdb_mutex)
#define MUTEX_UNLOCK(__X__) pthread_mutex_unlock(&srtdb_mutex) 
#define MUTEX_DELETE(__X__) pthread_mutex_destroy(&srtdb_mutex)

#define HSVCOMPAREKEY( TABLE, CONF, KEYTYPE, DATATYPE, FIELD )                                \
        Nat8 CONF##_Compare##KEYTYPE##Key( Nat32 key, Address data )                    \
        {                                                                               \
            DATATYPE * Section   = (DATATYPE *)data;                                    \
            Nat8            RetValue     = BiggerKey;                            \
                                                                                        \
            if( NULL != data )                                                          \
            {                                                                           \
                if ( key < (Nat32) Section->FIELD )                                           \
                {                                                                       \
                    RetValue = SmallerKey;                                       \
                }                                                                       \
                else                                                                    \
                {                                                                       \
                    if (key == (Nat32) Section->FIELD )                                       \
                    {                                                                   \
                        RetValue = EqualKey;                                        \
                    }                                                                   \
                }                                                                       \
            }                                                                           \
            return RetValue;                                                            \
        }\

#define INVALID_INDEX -1

class SortedTable {
private:
        IHsvSortedTableConfigurationEx *m_conf;
        
        int                 SortingCriteria,
                            SizeOfRecord,
                            MaxRecords,
                            TotNoOfRecords,
                            MemPool;

    Address *Data;
    MemoryPool *MemPoolptr;
    pthread_mutex_t srtdb_mutex;

    inline Address GetTableDataBuffer(int index);
public:
    SortedTable()
    {
        MemPoolptr = NULL;
        Data = NULL;
    }

    SortedTable( IHsvSortedTableConfigurationEx *conf)
    {
        m_conf = conf;
        SortingCriteria         = (int)conf->SortCriteria();
        MaxRecords              = conf->MaxElements();
        TotNoOfRecords = 0;
        SizeOfRecord            = conf->ElementSize();

        if( 0 != MaxRecords && \
            0 != SizeOfRecord )
        {
            MemPoolptr = new MemoryPool( MaxRecords, SizeOfRecord );
            Data = new Address[MaxRecords];
        }
        else
        {
            MemPoolptr = NULL;
            Data = NULL;
        }
        MUTEX_CREATE(0);
    }

    ~SortedTable()
    {
        //need to delete allocated ones... before that protect ..
        MUTEX_LOCK(0);
        if(MemPoolptr) delete MemPoolptr;
        if(Data) delete Data;
        MemPoolptr = NULL;
        Data = NULL;
        MUTEX_UNLOCK(0);
        MUTEX_DELETE(0);
    }

    void PrintTableDetails();

         int GetNoOfRecords( );
         int GetSortingCriteria( );
         int GetSizeOfRecord( );

    Bool InsertRecord(Nat32 PrimaryKey, Nat32 SecondaryKey, Nat32 TeritiaryKey,
            Nat32 QuaternaryKey, Address data);
    Bool DeleteRecord(Nat32 PrimaryKey, Nat32 SecondaryKey, Nat32 TeritiaryKey,
            Nat32 QuaternaryKey);
    Bool UpdateRecord(Nat32 PrimaryKey, Nat32 SecondaryKey, Nat32 TeritiaryKey,
            Nat32 QuaternaryKey, Address data);
    Bool ClearTable(void);
    Bool SearchOnPrimaryAndSecondaryKey(Nat32 PrimaryKey, Nat32 SecondaryKey,
            int* from, int* to);
    Bool SearchOnPrimarySecondaryAndTeritiaryKey(Nat32 PrimaryKey, Nat32 SecondaryKey,Nat32 TeritiaryKey,
            int* from, int* to);
    Bool SearchOnPrimaryKey(Nat32 PrimaryKey, int* from, int* to);
    Bool SearchOnKey(Nat32 PrimaryKey, Nat32 SecondaryKey, Nat32 TeritiaryKey,
            Nat32 QuaternaryKey, int* pindex);
    Bool SearchOnKey2(Nat32 PrimaryKey, Nat32 SecondaryKey, Nat32 TeritiaryKey,
            Nat32 QuaternaryKey, int Operator, int* pindex);

    Bool QueryOnIndex(int index, Address data);
    Bool QueryOnKey(Nat32 PrimaryKey, Nat32 SecondaryKey, Nat32 TeritiaryKey,
            Nat32 QuaternaryKey, Address data);

    int mBinInsert(Nat32 PrimaryKey, Nat32 SecondaryKey, Nat32 TeritiaryKey,
            Nat32 QuaternaryKey);

#ifdef BIN_SEARCH
    int mBinSearch( Nat8 Criteria, Nat32 PrimaryKey, Nat32 SecondaryKey, Nat32 TeritiaryKey, Nat32 QuaternaryKey);
#else
    int mSearch(Nat8 Criteria, Nat32 PrimaryKey, Nat32 SecondaryKey,
            Nat32 TeritiaryKey, Nat32 QuaternaryKey);
#endif /* BIN_SEARCH */
};

#endif /* __HSVSRTDB_H__ */
