/*
 *  Copyright(C) 2009 Koninklijke Philips Electronics N.V.,
 *  All Rights Reserved.
 *  This  source code and any compilation or derivative thereof is the
 *  proprietary information of Koninklijke Philips Electronics N.V.
 *  and is confidential in nature.
 *  Under no circumstances is this software to be exposed to or placed
 *  under an Open Source License of any type without the expressed
 *  written permission of Koninklijke Philips Electronics N.V.
 *
 *          %name: hsvmempool_m.c %
 *       %version: TVG_1 %
 * %date_modified: Mon Apr 06 16:38:21 2009 %
 *    %derived_by: aneeshc %
 */

//#include <iostream.h>
#include "InfraGlobals.h"
#include "hsvmempool.h"

#include "errno.h"
#include <assert.h>

using namespace std;

/* Semaphores are replaced with mutex */
/******************************************************************************/
void MemoryPool::Free(Address addr)
{
    int index;
    if (FreeList) {
        if (NULL != addr) {
            index = ((Nat32) addr - (Nat32) Buffers) / ElementSize;

            /* defensive check for boundary conditions*/
            if ((index >= 0) && (index < MaxElements)) {
                if (FreeList[index] == index) {
                    FreeList[index] = FreePtr;
                    FreePtr = index;
                }
            }
        }
    }
}

Address MemoryPool::Malloc()
{
    Address ret = NULL;
    int index = POOL_TERMINAL;

    if (FreeList) {
        if (POOL_TERMINAL != FreePtr) {
            index = FreePtr;
            FreePtr = FreeList[FreePtr];
            FreeList[index] = index;
            ret = (void*) (Buffers + (index * ElementSize));
        }
    }

    return ret;
}

void MemoryPool::Reset()
{
    if (FreeList) {
        int j;

        for (j = 0; j < MaxElements; j++) {
            FreeList[j] = j + 1;
        }
        FreeList[j] = POOL_TERMINAL;
        FreePtr = 0;
    }
}
