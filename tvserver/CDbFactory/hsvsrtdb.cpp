// hsvsrtdb.cpp : Defines the entry point for the console application.
//

//#include <iostream.h>
// #include <vector>
//#include "common.h"
#include "InfraGlobals.h"
#include "hsvsrtdb.h"

/* namespace */
using namespace std;

/* Dummy for compiling .. REMOVE */

extern void PrintSdtDetails(Address data);

 int SortedTable::GetSizeOfRecord( void )
{
    return SizeOfRecord;
}
 int SortedTable::GetNoOfRecords( void )
{
    return TotNoOfRecords;
}

 int SortedTable::GetSortingCriteria( void )
{
    return SortingCriteria;
}

inline Address SortedTable::GetTableDataBuffer(int index)
{
    Address     addr = NULL;

    if (index < GetNoOfRecords()) {
        addr = Data[index];
    }

    return addr;
}

int SortedTable::mBinInsert(Nat32 PrimaryKey, Nat32 SecondaryKey,
        Nat32 TeritiaryKey, Nat32 QuaternaryKey)
{
    int ret = INVALID_INDEX;

    if (GetSortingCriteria() & PrimaryKeyMask ) {
        int low = 0;
        int hi = GetNoOfRecords() - 1;
        int mid;

        /* handle extrane case*/
        if (GetNoOfRecords() == 0) {
            ret = 0;
        }

        while ((low <= hi) && (INVALID_INDEX == ret)) {
            int resultmid = -1;
            int resultlow = -1;
            int resulthi = -1;

            mid = (low + hi) >> 1;

            /* Analyze the low index */
            if ((resultlow = m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(low)))== EqualKey) 
            {
                if (GetSortingCriteria() & SecondaryKeyMask)
                {
                    if ((resultlow = m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer(low))) == EqualKey)
                    {
                        if (GetSortingCriteria() & TeritiaryKeyMask)
                        {
                            if ((resultlow = m_conf->CompareTeritiaryKey(TeritiaryKey, GetTableDataBuffer(low))) == EqualKey)
                            {
                                if (GetSortingCriteria() & QuaternaryKeyMask)
                                {
                                    if ((resultlow = m_conf->CompareQuaternaryKey(QuaternaryKey, GetTableDataBuffer(low))) == EqualKey)
                                    {
                                        /* fixme */
                                        ret = INVALID_INDEX;
                                        break;
                                    }
                                }
                                else {
                                    /* quaternay was not a search criteria,
                                     matching record found*/
                                    ret = INVALID_INDEX;
                                    break;
                                }
                            }
                        }
                        else {
                            /* teritiary was not a search criteria,
                             matching record found*/
                            ret = INVALID_INDEX;
                            break;
                        }

                    }
                }
                else {
                    /* secondary was not a search criteria,
                     matching record found*/
                    ret = INVALID_INDEX;
                    break;
                }
            }
            /* return if  extreme*/
            if (BiggerKey == resultlow) {
                ret = low;
            }

            /* Analyze the hi index */
            if ((resulthi = m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(hi)))== EqualKey) 
            {
                if (GetSortingCriteria() & SecondaryKeyMask)
                {
                    if ((resulthi = m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer(hi))) == EqualKey)
                    {
                        if (GetSortingCriteria() & TeritiaryKeyMask)
                        {
                            if ((resulthi = m_conf->CompareTeritiaryKey(TeritiaryKey, GetTableDataBuffer(hi))) == EqualKey)
                            {
                                 if (GetSortingCriteria() & QuaternaryKeyMask)
                                 {
                                    if ((resulthi = m_conf->CompareQuaternaryKey(QuaternaryKey, GetTableDataBuffer(hi))) == EqualKey)
                                    {
                                        /* fixme */
                                        ret = INVALID_INDEX;
                                        break;
                                    }
                                }
                                else {
                                    /* quaternary was not a search criteria,
                                     matching record found*/
                                    ret = INVALID_INDEX;
                                    break;
                                }
                            }
                        }
                        else {
                            /* teritiary was not a search criteria,
                             matching record found*/
                            ret = INVALID_INDEX;
                            break;
                        }

                    }
                }
                else {
                    /* secondary was not a search criteria,
                     matching record found*/
                    ret = INVALID_INDEX;
                    break;
                }
            }
            /* return if extreme */
            if (SmallerKey == resulthi) {
                ret = hi + 1;
            }

            /* Analyze the mid index */
            if ((resultmid = m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(mid)))== EqualKey) 
            {
                if (GetSortingCriteria() & SecondaryKeyMask)
                {
                    if ((resultmid = m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer(mid))) == EqualKey)
                    {
                        if (GetSortingCriteria() & TeritiaryKeyMask)
                        {
                            if ((resultmid = m_conf->CompareTeritiaryKey(TeritiaryKey, GetTableDataBuffer(mid))) == EqualKey)
                            {
                                if (GetSortingCriteria() & QuaternaryKeyMask)
                                {
                                    if ((resultmid = m_conf->CompareQuaternaryKey(QuaternaryKey, GetTableDataBuffer(mid))) == EqualKey)
                                    {
                                        /* fixme */
                                        ret = INVALID_INDEX;
                                        break;
                                    }
                                }
                                else {
                                    /* quaternary was not a search criteria,
                                     matching record found*/
                                    ret = INVALID_INDEX;
                                    break;
                                }
                            }
                        }
                        else {
                            /* teritiary was not a search criteria,
                             matching record found*/
                            ret = INVALID_INDEX;
                            break;
                        }

                    }
                }
                else {
                    /* secondary was not a search criteria,
                     matching record found*/
                    ret = INVALID_INDEX;
                    break;
                }
            }
            if (SmallerKey == resultmid) {
                low = mid + 1;
            }
            else if (BiggerKey == resultmid) {
                hi = mid - 1;
            }
            else {
                if (EqualKey != resultmid) {
                    break;
                }
            }
        }
    }
    return ret;
}

/*************************************************************
 Search functions
 **************************************************************/
/***********************************************
 - Binary search implementation
 **********************************************/

#ifdef BIN_SEARCH
int SortedTable :: mBinSearch( Nat8 Criteria, Nat32 PrimaryKey, Nat32 SecondaryKey, Nat32 TeritiaryKey, Nat32 QuaternaryKey)
{
    int ret = INVALID_INDEX;
    if (Criteria & PrimaryKeyMask)
    {
        int low = 0;
        int hi = GetNoOfRecords() -1;
        int mid;
        while ( (low <= hi) && (INVALID_INDEX == ret) )
        {
            int result=-1;

            mid = (low + hi)>>1;
            if ((result = m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(mid)))== EqualKey) 
            {
                if (Criteria & SecondaryKeyMask)
                {
                    if ((result = m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer(mid))) == EqualKey)
                    {
                        if (Criteria & TeritiaryKeyMask)
                        {
                            if ((result = m_conf->CompareTeritiaryKey(TeritiaryKey, GetTableDataBuffer(mid))) == EqualKey)
                            {
                                if (Criteria & QuaternaryKeyMask)
                                {
                                    if ((result = m_conf->CompareQuaternaryKey(QuaternaryKey, GetTableDataBuffer(mid))) == EqualKey)
                                    {
                                        ret = mid;
                                    }
                                }
                                else
                                {
                                    /* quaternary was not a search criteria, hence we have found the record*/
                                    ret = mid;
                                }
                            }
                        }
                        else
                        {
                            /* teritiary was not a search criteria, hence we have found the record*/
                            ret = mid;
                        }
                    }
                }
                else
                {
                    /* secondary was not a search criteria, hence we have found the record*/
                    ret = mid;
                }
            }
            if ( BiggerKey == result )
            {
                hi = mid - 1;
            }
            else if ( SmallerKey == result )
            {
                low = mid + 1;
            }
            else
            {
                if( EqualKey != result )
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#else
/****************
 Sequential search
 *****************/
int SortedTable::mSearch(Nat8 Criteria, Nat32 PrimaryKey, Nat32 SecondaryKey,
        Nat32 TeritiaryKey, Nat32 QuaternaryKey)
{
    int retval = INVALID_INDEX;
    int i;

    if (Criteria & PrimaryKeyMask)
    {
        for (i = 0; i < GetNoOfRecords(); i++)
        {
            if (EqualKey ==m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(i)))
            {
                if (Criteria & SecondaryKeyMask)
                {
                    for (; i < GetNoOfRecords(); i++)
                    {
                        if (EqualKey ==m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer(i)))
                        {
                            if (Criteria & TeritiaryKeyMask)
                            {
                                for (; i < GetNoOfRecords(); i++)
                                {
                                    if (EqualKey ==m_conf->CompareTeritiaryKey(TeritiaryKey, GetTableDataBuffer(i)))
                                    {
                                        if (Criteria & QuaternaryKeyMask)
                                        {
											for (; i < GetNoOfRecords(); i++)
											{
												if (EqualKey ==m_conf->CompareQuaternaryKey(QuaternaryKey, GetTableDataBuffer(i)))
												{
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            }
        }
        if (i != GetNoOfRecords()) {
            retval = i;
        }
    }
    return retval;
}
#endif

Bool SortedTable::InsertRecord(Nat32 PrimaryKey, Nat32 SecondaryKey,
        Nat32 TeritiaryKey, Nat32 QuaternaryKey, Address data)
{
    Bool ret = FALSE;

    MUTEX_LOCK(0);

    if (GetNoOfRecords() >= MaxRecords) {
        MUTEX_UNLOCK(0); /*1*/
        return ret;
    }

    if (GetSizeOfRecord() > 0) {
        int index;
        /* insert is atomic, no other operations are allowed on the table
         during the function call*/
        {
#ifdef BIN_SEARCH
            index = mBinInsert( PrimaryKey, SecondaryKey, TeritiaryKey, QuaternaryKey);
#else
		    if (GetSortingCriteria( ) & PrimaryKeyMask)
		    {
		        for (index = 0; index < GetNoOfRecords(); index++)
		        {
		            if (SmallerKey !=m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer( index )))
		            {
                        break;
                    }
                }
		        if (GetSortingCriteria( ) & SecondaryKeyMask)
		        {
		            for (; index < GetNoOfRecords(); index++)
		            {                
		                if (SmallerKey !=m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer( index )))
		                {
                            break;
                        }
		                if (EqualKey !=m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer( index )))
		                {
                            break;
                        }
                    }
		            if (GetSortingCriteria( ) & TeritiaryKeyMask)
		            {
		                for (; index < GetNoOfRecords(); index++)
		                {                
		                    if (SmallerKey !=m_conf->CompareTeritiaryKey(TeritiaryKey, GetTableDataBuffer( index )))
		                    {
                                break;
                            }
		                    if (EqualKey !=m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer( index )))
		                    {
                                break;
                            }
		                    if ( (EqualKey !=m_conf->ComparePrimaryKey(PrimaryKey  , GetTableDataBuffer( index ))))
		                    {
                                break;
                            }
                        }
		                if (GetSortingCriteria( ) & QuaternaryKeyMask)
		                {
		                    for (; index < GetNoOfRecords(); index++)
		                    {                
		                        if (SmallerKey !=m_conf->CompareQuaternaryKey(QuaternaryKey, GetTableDataBuffer( index )))
		                        {
                                    break;
                                }
		                        if (EqualKey !=m_conf->CompareTeritiaryKey(TeritiaryKey, GetTableDataBuffer( index )))
		                        {
                                    break;
                                }
		                        if (EqualKey !=m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer( index )))
		                        {
                                    break;
                                }
		                        if ( (EqualKey !=m_conf->ComparePrimaryKey(PrimaryKey  , GetTableDataBuffer( index ))))
		                        {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
#endif
			if ( INVALID_INDEX != index )
		    {
		        if (data)
				{
                    int i = GetNoOfRecords();
                    Address ptr = MemPoolptr->Malloc();

                    if (NULL != ptr) {
                        for (; i != index && i < MaxRecords; i--)
                        {
                            Data[i] = Data[i - 1]; /* if only pointers where understood by all programmers */
                        }

                        Data[index] = ptr;
                        memcpy(Data[index], data, GetSizeOfRecord());
                        TotNoOfRecords = GetNoOfRecords() + 1;
                        ret = TRUE;
                    }
                }
            }
        }
    }
    
    MUTEX_UNLOCK(0); /*1*/
    return ret;
}

Bool SortedTable::UpdateRecord(Nat32 PrimaryKey, Nat32 SecondaryKey,
        Nat32 TeritiaryKey, Nat32 QuaternaryKey, Address data)
{
    Bool ret = FALSE;

    if (GetSizeOfRecord() > 0) {
        int index;
        MUTEX_LOCK(0); /*1*/
        {
            if (GetNoOfRecords() > 0) {
#ifdef BIN_SEARCH
                index = mBinSearch(GetSortingCriteria(), PrimaryKey, SecondaryKey, TeritiaryKey, QuaternaryKey);
#else
                index = mSearch(GetSortingCriteria(), PrimaryKey, SecondaryKey, TeritiaryKey, QuaternaryKey);
#endif

                if (INVALID_INDEX != index) {
                    /* 
                    if (data) {
                        memcpy(Data[index], data, GetSizeOfRecord());
                        ret = TRUE;
                    }
                    */
                    MUTEX_UNLOCK(0);
                    ret = DeleteRecord (PrimaryKey, SecondaryKey, TeritiaryKey, QuaternaryKey);
                    ret = InsertRecord (PrimaryKey, SecondaryKey,TeritiaryKey, QuaternaryKey, data);
                    MUTEX_LOCK(0);
                }
                else {
                    /* ("Record not found. Update failed.")*/
                    /* do nothing*/
                }
            }
            else {
                /* err : nothiing is inserted yet */
            }
        }
        MUTEX_UNLOCK(0);
    }

    return ret;
}

Bool SortedTable::DeleteRecord(Nat32 PrimaryKey, Nat32 SecondaryKey,
        Nat32 TeritiaryKey, Nat32 QuaternaryKey)
{
    Bool ret = FALSE;

    if (GetSizeOfRecord() > 0) {
        int index;
        MUTEX_LOCK(0); /*1*/
        {
            if (GetNoOfRecords() > 0) {
#ifdef BIN_SEARCH
                index = mBinSearch(GetSortingCriteria(), PrimaryKey, SecondaryKey, TeritiaryKey, QuaternaryKey);
#else
                index = mSearch(GetSortingCriteria(), PrimaryKey, SecondaryKey,
                        TeritiaryKey, QuaternaryKey);
#endif
                if (INVALID_INDEX != index) {
                    MemPoolptr->Free(Data[index]);
                    TotNoOfRecords = GetNoOfRecords() - 1;

                    for (; index < GetNoOfRecords(); index++) {
                        Data[index] = Data[index + 1];
                    }
                    /* Reset the last index to NULL */
                    if (index < MaxRecords) {
                        Data[index] = NULL;
                    }
                    ret = TRUE;
                }
                else {
                    /* ("Record not found. Delete failed.")*/
                    /* do nothing*/
                }
            }
            else {
                /* err : nothiing is inserted yet */
            }
        }
        MUTEX_UNLOCK(0); /*1*/
    }
    return ret;
}

Bool SortedTable::QueryOnKey(Nat32 PrimaryKey, Nat32 SecondaryKey,
        Nat32 TeritiaryKey, Nat32 QuaternaryKey, Address data)
{
    Bool ret = FALSE;

    if (GetSizeOfRecord() > 0) {
        int index;
        MUTEX_LOCK(0); /*1*/
        {
#ifdef BIN_SEARCH
            index = mBinSearch(GetSortingCriteria(),PrimaryKey, SecondaryKey, TeritiaryKey, QuaternaryKey);
#else
            index = mSearch(GetSortingCriteria(), PrimaryKey, SecondaryKey,
                    TeritiaryKey, QuaternaryKey);
#endif    

            if (INVALID_INDEX != index) {
                Address tmpdata = GetTableDataBuffer(index);

                if (data && tmpdata) {
                    memcpy((char*) data, (char*) tmpdata, GetSizeOfRecord());
                    ret = TRUE;
                }
            }
            else {
                /* ("Record not found.")*/
                /* do nothing */
            }
        }
        MUTEX_UNLOCK(0);
    }
    return ret;
}

Bool SortedTable::QueryOnIndex(int index, Address data)
{
    Bool ret = FALSE;

    MUTEX_LOCK(0);

    if (GetSizeOfRecord() > 0) {
        if (index < GetNoOfRecords()) {
            Address tmpdata = GetTableDataBuffer(index);
            if (data && tmpdata) {
                memcpy((char*) data, (char*) tmpdata, GetSizeOfRecord());
                ret = TRUE;
            }
        }
    }

    MUTEX_UNLOCK(0);
    return ret;
}

Bool SortedTable::SearchOnKey2(Nat32 PrimaryKey, Nat32 SecondaryKey,
        Nat32 TeritiaryKey, Nat32 QuaternaryKey, int Operator, int* pindex)
{
    Bool retval = FALSE;
    int index = 0;

    ;

    if (Operator == OperatorEqualMask ) {
        retval = SearchOnKey(PrimaryKey, SecondaryKey, TeritiaryKey,
                QuaternaryKey, pindex);
    }
    else {
        MUTEX_LOCK(0); /*1*/
        {
            /* find the position to insert */
#ifdef BIN_SEARCH
            index = mBinInsert( PrimaryKey, SecondaryKey, TeritiaryKey, QuaternaryKey);
#else

                if (GetSortingCriteria() & PrimaryKeyMask)
                {
                    for (index = 0; index < GetNoOfRecords(); index++)
                    {
                        if (SmallerKey !=m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(index)))
                        {
                        break;
                    }
                }
                    if (GetSortingCriteria() & SecondaryKeyMask)
                    {
                        for (; index < GetNoOfRecords(); index++)
                        {                
                            if (SmallerKey !=m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer(index)))
                            {
                            break;
                        }
                            if (EqualKey !=m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(index)))
                            {
                            break;
                        }
                    }

                }
            }
#endif
            /* evaluate the expression  and make index point to the record of interest*/
            if (INVALID_INDEX == index) {
#ifdef BIN_SEARCH
                index = mBinSearch(GetSortingCriteria(),PrimaryKey, SecondaryKey, TeritiaryKey, QuaternaryKey);
#else
                index = mSearch(GetSortingCriteria(), PrimaryKey, SecondaryKey,
                        TeritiaryKey, QuaternaryKey);
#endif
                if (INVALID_INDEX == index) {
                    if (Operator == OperatorGreaterMask ) {
                        index--;

                    }
                    else if (Operator == OperatorSmallerMask ) {
                        index++;
                    }
                }
            }
            else {
                if (Operator == OperatorGreaterMask ) {
                    index--;
                }
                else if (Operator == OperatorSmallerMask ) {
                    /* already poniting to smaller */
                }
            }
            /* if index in valid range*/
            retval = FALSE;
            if ((index >= 0) && (index < GetNoOfRecords())) {
                *pindex = index;
                retval = TRUE;
            }
        }
        MUTEX_UNLOCK(0); /*1*/
    }
    return retval;
}

Bool SortedTable::SearchOnKey(Nat32 PrimaryKey, Nat32 SecondaryKey,
        Nat32 TeritiaryKey, Nat32 QuaternaryKey, int* index)
{
    Bool ret = FALSE;

    if (GetSizeOfRecord() > 0) {
        int id;
        MUTEX_LOCK(0); /*1*/
        {
#ifdef BIN_SEARCH
            id = mBinSearch(GetSortingCriteria(), PrimaryKey, SecondaryKey, TeritiaryKey, QuaternaryKey);
#else
            id = mSearch(GetSortingCriteria(), PrimaryKey, SecondaryKey,
                    TeritiaryKey, QuaternaryKey);
#endif
            /* return the value before releasing sem*/
            if (INVALID_INDEX != id) {
                if (index) {
                    *index = id;
                    ret = TRUE;
                }
            }
        }
        MUTEX_UNLOCK(0);
    }
    return ret;
}

Bool SortedTable::SearchOnPrimaryKey(Nat32 PrimaryKey, int* from, int* to)
{
    Bool ret = FALSE;

    if (GetSizeOfRecord() > 0) {
        int index;

        MUTEX_LOCK(0);
        {
            if ((NULL != from) && (NULL != to)) {
#ifdef BIN_SEARCH
                index = mBinSearch((Nat8)(GetSortingCriteria() & PrimaryKeyMask), PrimaryKey, 0, 0, 0);
                if ( INVALID_INDEX != index )
                {
                    int i;
                    i = index -1;
                    /* walk up the array to get matching records*/
                    for (; i >= 0; i--)
                    {
                        if (EqualKey !=m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(i)))
                        break;
                    }
                    *from = i + 1;
                    ret = TRUE;
                }
#else
                index = mSearch((Nat8)(GetSortingCriteria() & PrimaryKeyMask ),
                        PrimaryKey, 0, 0, 0);
                if (INVALID_INDEX != index) {
                    *from = index;
                    ret = TRUE;
                }
#endif

                /* index is the first element to be added now*/
                if (INVALID_INDEX != index) {
                    /* proceed next*/
                    index++;
                    /*search for all other matches if any*/
                    for (; (index < GetNoOfRecords()); index++)
                    {
                        if (EqualKey !=m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(index)))
                            break;
                    }
                    *to = index - 1;
                    ret = TRUE;
                }
                else {
                    /* ("Record matching Primary key not found.")*/
                    /*do nothing*/
                }
            }
            else {
                /* err : from and to values passed are invalid @#@#! */
            }
        }
        MUTEX_UNLOCK(0);
    }
    return ret;
}

Bool SortedTable::SearchOnPrimaryAndSecondaryKey(Nat32 PrimaryKey,
        Nat32 SecondaryKey, int* from, int* to)
{
    Bool ret = FALSE;

    if (GetSizeOfRecord() > 0) {
        int index;

        MUTEX_LOCK(0);
        {
            if ((NULL != from) && (NULL != to)) {
#ifdef BIN_SEARCH
                index = mBinSearch((Nat8)(GetSortingCriteria( ) & ( PrimaryKeyMask | SecondaryKeyMask )), PrimaryKey, SecondaryKey, 0, 0);
                if ( INVALID_INDEX != index )
                {
                    int i;
                    i = index -1;
                    /* walk up the array to get matching records*/
                    for (; i >= 0; i--)
                    {
                        if ( (EqualKey !=m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(i))) ||
                             (EqualKey !=m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer(i))) )
                        break;
                    }
                    *from = i + 1;
                    ret = TRUE;
                }
#else
                index = mSearch((Nat8)(GetSortingCriteria() & (PrimaryKeyMask | SecondaryKeyMask) ), PrimaryKey, SecondaryKey, 0, 0);
                if ( INVALID_INDEX != index )
                {
                    *from = index;
                    ret = TRUE;
                }
#endif

                /* index is the first element to be added now*/
                if (INVALID_INDEX != index) {
                    /* proceed next*/
                    index++;
                    /*search for all other matches if any*/
                    for (; (index < GetNoOfRecords()); index++)
                    {
                        if ( (EqualKey !=m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(index))) || 
                             (EqualKey !=m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer(index))) )
                            break;
                    }
                    *to = index - 1;
                    ret = TRUE;
                }
                else {
                    /* ("Record matching Primary key not found.")*/
                    /*do nothing*/
                }
            }
            else {
                /* err : from and to values passed are invalid @#@#! */
            }
        }
        MUTEX_UNLOCK(0);
    }
    return ret;
}

Bool SortedTable::SearchOnPrimarySecondaryAndTeritiaryKey(Nat32 PrimaryKey,
        Nat32 SecondaryKey, Nat32 TeritiaryKey, int* from, int* to)
{
    Bool ret = FALSE;

    if (GetSizeOfRecord() > 0) {
        int index;

        MUTEX_LOCK(0);
        {
            if ((NULL != from) && (NULL != to)) {
#ifdef BIN_SEARCH
                index = mBinSearch((Nat8)(GetSortingCriteria( ) & ( PrimaryKeyMask | SecondaryKeyMask | TeritiaryKeyMask)), PrimaryKey, SecondaryKey, TeritiaryKey, 0);
                if ( INVALID_INDEX != index )
                {
                    int i;
                    i = index -1;
                    /* walk up the array to get matching records*/
                    for (; i >= 0; i--)
                    {
                        if ( (EqualKey !=m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(i))) ||
                             (EqualKey !=m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer(i))) ||
                             (EqualKey !=m_conf->CompareTeritiaryKey(TeritiaryKey, GetTableDataBuffer(i))) )
                        break;
                    }
                    *from = i + 1;
                    ret = TRUE;
                }
#else
                index = mSearch((Nat8)(GetSortingCriteria() & (PrimaryKeyMask | SecondaryKeyMask | TeritiaryKeyMask) ), PrimaryKey, SecondaryKey, TeritiaryKey, 0);
                if ( INVALID_INDEX != index )
                {
                    *from = index;
                    ret = TRUE;
                }
#endif

                /* index is the first element to be added now*/
                if (INVALID_INDEX != index) {
                    /* proceed next*/
                    index++;
                    /*search for all other matches if any*/
                    for (; (index < GetNoOfRecords()); index++)
                    {
                        if ( (EqualKey !=m_conf->ComparePrimaryKey(PrimaryKey, GetTableDataBuffer(index))) || 
                             (EqualKey !=m_conf->CompareSecondaryKey(SecondaryKey, GetTableDataBuffer(index))) ||
                             (EqualKey !=m_conf->CompareTeritiaryKey(TeritiaryKey, GetTableDataBuffer(index))) )
                            break;
                    }
                    *to = index - 1;
                    ret = TRUE;
                }
                else {
                    /* ("Record matching Primary key not found.")*/
                    /*do nothing*/
                }
            }
            else {
                /* err : from and to values passed are invalid @#@#! */
            }
        }
        MUTEX_UNLOCK(0);
    }
    return ret;
}


Bool SortedTable::ClearTable(void)
{
    int     index;
    /* Delete and reallocate to clear data base */
    MUTEX_LOCK(0);

    TotNoOfRecords = 0;
    MemPoolptr->Reset();
    for (index = 0; index < MaxRecords; index++) {
        Data[index] = NULL;
    }

    MUTEX_UNLOCK(0);

    return TRUE;
}

#if 0 /* AA0269 */
void SortedTable::PrintTableDetails()
{
    cout << "--------------------------------------------" << endl;
    cout << hex << uppercase << " Sort Crit : " << GetSortingCriteria() << endl;
    cout << hex << uppercase << " No Of Rcd : " << GetNoOfRecords() << endl;
    cout << hex << uppercase << " Sz Of Rcd : " << GetSizeOfRecord() << endl;

    int recd = 0, TtRcd = GetNoOfRecords();

    while (recd < TtRcd) {
        PrintSdtDetails(GetTableDataBuffer(recd));
        ++recd;
    }

    cout << "--------------------------------------------" << endl;
}

void SortedTable :: PrintConfigDetails( )
{
    cout<<"--------------------------------------------"<<endl;
    cout<<hex<<uppercase<<" Table Id  : "<<GetTableId()<<endl;
    cout<<hex<<uppercase<<" Sort Crit : "<<GetSortingCriteria()<<endl;
    cout<<hex<<uppercase<<" No Of Rcd : "<<GetNoOfRecords()<<endl;
    cout<<hex<<uppercase<<" Sz Of Rcd : "<<GetSizeOfRecord()<<endl;

    int recd = 0,TtRcd = GetNoOfRecords();

    while( recd < TtRcd )
    {
        PrintSdtDetails(GetTableDataBuffer( recd ));
        ++recd;
    }

    cout<<"--------------------------------------------"<<endl;
}

void SortedTable :: PrintSpecificTableDetails( int table_id )
{
    vector<SortedTable>::iterator it = SortedTableData.begin();

    it[table_id].PrintConfigDetails( );
}

void SortedTable :: PrintTableDetails( )
{
    int t_ix =0, tot_tbl = GetNoOfTablesAvailable();

    for(; t_ix < tot_tbl; ++t_ix )
    {
        PrintSpecificTableDetails( t_ix );
    }
}

#endif /* #if 0 */
