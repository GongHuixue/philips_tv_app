#ifndef	__MEMPOOL_H__
#define	__MEMPOOL_H__

#define POOL_TERMINAL    -1
class MemoryPool {
    int MaxElements;
    int ElementSize;
    int FreePtr;
    int *FreeList;
    unsigned char *Buffers;

public:

    void Free(Address addr);
    Address Malloc();
    void Reset();

    MemoryPool(int MaxRecords, int SizeOfElement)
    {
        MaxElements = MaxRecords;
        ElementSize = SizeOfElement;
        FreePtr = 0;
        FreeList = new int[MaxElements + 1];
        Buffers = new unsigned char[MaxElements * SizeOfElement];
        if (FreeList) {
            int j;

            for (j = 0; j < MaxElements; j++) {
                FreeList[j] = j + 1;
            }
            FreeList[j] = POOL_TERMINAL;
            FreePtr = 0;
        }
    }

    ~MemoryPool()
    {
        MaxElements = ElementSize = 0;
        FreePtr = 0;
        if (FreeList) {
            delete FreeList;
            FreeList = NULL;
        }
        if (Buffers) {
            delete Buffers;
            Buffers = NULL;
        }
    }
};
#endif
