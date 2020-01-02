/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         dm_hash.c
  Description: operations for the hash table.
  History:
*******************************************************************************/

#include "dm_internal.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 


/**
* the below numbers is took from
* http://planetmath.org/encyclopedia/GoodHashTablePrimes.html
 The following is such a list. It has the properties that:

   1. each number in the list is prime
   2. each number is slightly less than twice the size of the previous
   3. each number is as far as possible from the nearest two powers of two

Using primes for hash tables is a good idea because it minimizes clustering in the hashed table.
Item (2) is nice because it is convenient for growing a hash table in the face of expanding data.
Item (3) has, allegedly, been shown to yield especially good results in practice.

And here is the list:
lwr 	        upr 	       % err 	        prime
$2^{5}$ 	$2^{6}$ 	10.416667 	53
$2^{6}$ 	$2^{7}$ 	1.041667 	97
$2^{7}$ 	$2^{8}$ 	0.520833 	193
$2^{8}$ 	$2^{9}$ 	1.302083 	389
$2^{9}$ 	$2^{10}$ 	0.130208 	769
$2^{10}$ 	$2^{11}$ 	0.455729 	1543
$2^{11}$ 	$2^{12}$ 	0.227865 	3079
$2^{12}$ 	$2^{13}$ 	0.113932 	6151
$2^{13}$ 	$2^{14}$ 	0.008138 	12289
$2^{14}$ 	$2^{15}$ 	0.069173 	24593
$2^{15}$ 	$2^{16}$ 	0.010173 	49157
$2^{16}$ 	$2^{17}$ 	0.013224 	98317
$2^{17}$ 	$2^{18}$ 	0.002543 	196613
$2^{18}$ 	$2^{19}$ 	0.006358 	393241
$2^{19}$ 	$2^{20}$ 	0.000127 	786433
$2^{20}$ 	$2^{21}$ 	0.000318 	1572869
$2^{21}$ 	$2^{22}$ 	0.000350 	3145739
$2^{22}$ 	$2^{23}$ 	0.000207 	6291469
$2^{23}$ 	$2^{24}$ 	0.000040 	12582917
$2^{24}$ 	$2^{25}$ 	0.000075 	25165843
$2^{25}$ 	$2^{26}$ 	0.000010 	50331653
$2^{26}$ 	$2^{27}$ 	0.000023 	100663319
$2^{27}$ 	$2^{28}$ 	0.000009 	201326611
$2^{28}$ 	$2^{29}$ 	0.000001 	402653189
$2^{29}$ 	$2^{30}$ 	0.000011 	805306457
$2^{30}$ 	$2^{31}$ 	0.000000 	1610612741

The columns are, in order, the lower bounding power of two,
the upper bounding power of two,
the relative deviation (in percent) of the prime number from the optimal middle of the first two, and finally the prime itself.
*
*/
/* DM_HASH_INIT_SIZE was defined as 53 */
static const UINT32 dm_primes[] = {
53, 97, 193, 389,
769, 1543, 3079, 6151,
12289, 24593, 49157, 98317,
196613, 393241, 786433, 1572869,
3145739, 6291469, 12582917, 25165843,
50331653, 100663319, 201326611, 402653189,
805306457, 1610612741
};

static const UINT32 dm_prime_table_length = sizeof(dm_primes)/sizeof(dm_primes[0]);
static const float dm_max_load_factor = 0.65;

static INT32 osa_hash_add(DM_HASH_T *pHash, char* key,void* value);


static INT8* osa_hash_keydup(INT8 *o)
{
	INT32 l=strlen(o)+1;
	INT8 *ns=(INT8*)DM_MALLOC(l*sizeof(INT8));
	if(ns==NULL)
	{
		return NULL;
	}
	else
	{
        memset(ns,0,l*sizeof(INT8));
		strcpy(ns,o);
		return ns;
	}
}

static UINT32 osa_hash_calculate(DM_HASH_T *pHash, INT8 *s)
{
    UINT32 h=0;

    for(;*s;s++)
    {
        h=*s+h*31;
    }

    return h%(UINT32)(pHash->maxSize);
}

static INT32 osa_hash_expand(DM_HASH_T *pHash)
{
    UINT32 i;
    DM_HASH_NODE_T *np,*t;
    DM_HASH_NODE_T **tmpHashTab = NULL;
    INT32 tableSize = pHash->maxSize;

    tmpHashTab = pHash->hashTable;

    /* set the right hashsize */
    for(i=0; i< dm_prime_table_length; i++)
    {
        if(dm_primes[i] == pHash->maxSize)
        {
            if(i < dm_prime_table_length -1)
            {
                pHash->maxSize = dm_primes[i+1];
            }
            else
            {
                pHash->maxSize = dm_primes[i]*2;
            }
            break;
        }
    }

    pHash->hashTable = (DM_HASH_NODE_T**)DM_MALLOC(sizeof(DM_HASH_NODE_T*)*pHash->maxSize);

    if(NULL == pHash->hashTable)
    {
        pHash->maxSize = tableSize;
        pHash->hashTable = tmpHashTab;
        return OSA_FAILURE;
    }

    osa_mem_set(pHash->hashTable, 0, sizeof(DM_HASH_NODE_T*)*pHash->maxSize);

    for(i = 0; i < pHash->maxSize; i++)
    {
        pHash->hashTable[i] = NULL;
    }

    pHash->usedSize = 0;

    for(i=0; i<(unsigned int)tableSize; i++)
    {
        if((tmpHashTab[i]) != NULL)
        {
          np=(tmpHashTab[i]);
          while(np!=NULL)
          {
		    t=np->next;
	        osa_hash_add(pHash, (INT8*)(np->key), np->value);
		    DM_FREE(np->key);
		    DM_FREE(np);
            np=t;
          }
        }
    }
    DM_FREE(tmpHashTab);

    return OSA_SUCCESS;
}

static DM_HASH_NODE_T* osa_hash_lookup(DM_HASH_T *pHash, INT8 *n)
{
    UINT32 hi=osa_hash_calculate(pHash, n);
    DM_HASH_NODE_T* np=pHash->hashTable[hi];

    for(;np!=NULL;np=np->next)
    {
        if(!strcmp(np->key,n))
        {
            return np;
        }
    }

    return NULL;
}

static INT32 osa_hash_add(DM_HASH_T *pHash, char* key,void* value)
{
    UINT32 hi;
    DM_HASH_NODE_T *np;

    if(pHash->usedSize >= pHash->maxSize-1)
    {
        if (osa_hash_expand(pHash) == OSA_FAILURE)
        {
            return OSA_FAILURE;
        }
    }

    np=osa_hash_lookup(pHash, key);

    if(np==NULL)
    {
        hi=osa_hash_calculate(pHash, key);
        np=(DM_HASH_NODE_T*)DM_MALLOC(sizeof(DM_HASH_NODE_T));

        if(np==NULL)
        {
            return OSA_FAILURE;
        }

        osa_mem_set(np,0,sizeof(DM_HASH_NODE_T));

        np->key=osa_hash_keydup(key);
        if(np->key==NULL)
        {
            DM_FREE(np);
            return OSA_FAILURE;
        }

        np->next=pHash->hashTable[hi];
        pHash->hashTable[hi]=np;
        pHash->usedSize ++;
    }
    else
    {
        //DM_FREE(np->value);
    }

    np->value= value;

    return OSA_SUCCESS;
}

static void osa_hash_init_enumerator(DM_HASH_T *pHash, DM_HASH_ENUM_T *pEnum)
{
    INT32 i = 0;

    if (NULL == pHash || NULL == pEnum)
    {
        return;
    }
    
	pEnum->pHash = pHash;
	pEnum->lNextEnumIndex = 0;
	pEnum->pNextEnumNode  = NULL;

	for(i = 0; i < pHash->maxSize; i++)
	{
		if(pHash->hashTable[i] != NULL)
		{
			pEnum->lNextEnumIndex = i;
			pEnum->pNextEnumNode  = pHash->hashTable[i];
			break;
		}
	}
	
	return;
}

static void* osa_hash_enum_next(DM_HASH_ENUM_T *pEnum)
{
    INT32 i = 0;
	void*  pCurEnumValue = NULL;

    if (NULL == pEnum || NULL == pEnum->pHash)
    {
        return NULL;
    }

	if(pEnum->pNextEnumNode != NULL)
    {
        /* get current value */
		pCurEnumValue = pEnum->pNextEnumNode->value;

        /* Move next pointer to next element */
		if (pEnum->pNextEnumNode->next != NULL)
    	{
    	    pEnum->pNextEnumNode = pEnum->pNextEnumNode->next;
    	}
    	else
    	{
			pEnum->pNextEnumNode  = NULL;
			pEnum->lNextEnumIndex ++ ;

            for(i = pEnum->lNextEnumIndex; i< pEnum->pHash->maxSize; i++)
        	{
        	    if(pEnum->pHash->hashTable[i] != NULL)
    		   	{
    				pEnum->lNextEnumIndex = i;
    		   	    pEnum->pNextEnumNode  = pEnum->pHash->hashTable[i];
    				break;
    		   	}
        	}
    	}
    }

    return pCurEnumValue;
}

INT32 DmHashCreate(DM_HASH_T *pHash, INT32 hashSize)
{
    INT32 i = 0;

    if (hashSize <= 0)
    {
        hashSize = DM_HASH_DEFAULT_MAX_SIZE;
    }

    if (osa_thread_mutex_create(&pHash->mutex) != OSA_SUCCESS)
    {
        return OSA_FAILURE;
    }

    pHash->hashTable = (DM_HASH_NODE_T**)DM_MALLOC(sizeof(DM_HASH_NODE_T*)*hashSize);

    if(NULL == pHash->hashTable)
    {
        return OSA_FAILURE;
    }

    osa_mem_set(pHash->hashTable, 0, sizeof(DM_HASH_NODE_T*)*hashSize);

    for(i = 0; i < pHash->maxSize; i++)
    {
        pHash->hashTable[i] = NULL;
    }

    pHash->maxSize = hashSize;
    pHash->usedSize = 0;

    return OSA_SUCCESS;
}

void* DmHashGetValue(DM_HASH_T *pHash, INT8* key)
{
    void *p = NULL;

    if (pHash->usedSize<=0 || pHash->hashTable==NULL)
    {
        return p;
    }

    osa_thread_mutex_lock(&pHash->mutex);

    DM_HASH_NODE_T* n=osa_hash_lookup(pHash, key);

    if(n==NULL)
    {
        p = NULL;
    }
    else
    {
        p = n->value;
    }

    osa_thread_mutex_unlock(&pHash->mutex);

    return p;
}

void *DmHashEnumerator(DM_HASH_T *pHash, DM_HASH_ENUM_T *pEnum)
{
    if (NULL == pHash || NULL == pEnum)
    {
        return NULL;
    }
    
    osa_thread_mutex_lock(&pHash->mutex);
    
    if (pHash != pEnum->pHash)    /* At first time, reset next element pointer */
    {
        osa_hash_init_enumerator(pHash, pEnum);
	}

	void* pCurEnumValue = osa_hash_enum_next(pEnum);

    osa_thread_mutex_unlock(&pHash->mutex);

	return pCurEnumValue;
}

void* DmHashDel(DM_HASH_T *pHash, INT8* key)
{

    INT32 i;
    DM_HASH_NODE_T *np,*t;
    UINT32 hi=0;
    void *pRet = NULL;

    if (pHash->usedSize<=0 || pHash->hashTable==NULL)
    {
        return pRet;
    }

    osa_thread_mutex_lock(&pHash->mutex);

    hi=osa_hash_calculate(pHash, key);

    i = hi;

    if(pHash->hashTable[i]!=NULL)
    {
        np=pHash->hashTable[i];
        t = np;

        while(np!=NULL)
        {
            if(strcmp(key, np->key) == 0)
            {
                if(t == np)
                {
                    /* the first node */
                    pHash->hashTable[i] = np->next;
                    pRet = np->value;
                    DM_FREE(np->key);
                    DM_FREE(np);
                }
                else
                {
                    pRet = np->value;
                    t->next = np->next;
                    DM_FREE(np->key);
                    DM_FREE(np);
                }

                pHash->usedSize --;
                break;
            }

            t=np;
            np=np->next;
        }
    }

    osa_thread_mutex_unlock(&pHash->mutex);

    return pRet;
}

void* DmHashGetKey(DM_HASH_T *pHash, void * value)
{
    INT32 i;
    DM_HASH_NODE_T *np,*t;
    INT8 *key = NULL;

    if (pHash->usedSize<=0 || pHash->hashTable==NULL)
    {
        return key;
    }

    osa_thread_mutex_lock(&pHash->mutex);

    for(i=0;i<pHash->maxSize;i++)
    {
        if(pHash->hashTable[i]!=NULL)
        {
            np=pHash->hashTable[i];

            while(np!=NULL)
            {
                if(value == np->value)
                {
                    key = np->key;
                    break;
                }

                t=np->next;
                np=t;
            }
        }
    }

    osa_thread_mutex_unlock(&pHash->mutex);

    return key;
}

INT32 DmHashGetUsedsize(DM_HASH_T *pHash)
{
    return pHash->usedSize;
}

INT32 DmHashAdd(DM_HASH_T *pHash, char* key,void* value)
{
    INT32 lRet = OSA_FAILURE;

    if (pHash==NULL || pHash->hashTable == NULL)
    {
        return lRet;
    }

    osa_thread_mutex_lock(&pHash->mutex);

    lRet = osa_hash_add(pHash, key, value);

    osa_thread_mutex_unlock(&pHash->mutex);

    return lRet;
}

INT32 DmHashDestroy(DM_HASH_T *pHash, BOOL8 bDelValue)
{
    INT32 i;
    DM_HASH_NODE_T *np,*t;

    osa_thread_mutex_lock(&pHash->mutex);

    if(pHash->usedSize <= 0)
    {
        DM_FREE(pHash->hashTable);
    }
    else
  	{
        for(i=0;i<pHash->maxSize;i++)
        {
            if(pHash->hashTable[i]!=NULL)
            {
                np=pHash->hashTable[i];

                while(np!=NULL)
                {
                    t=np->next;

                    if(NULL != np->key)
                    {
                        DM_FREE(np->key);
                    }

                    if(NULL != np->value && OSA_TRUE == bDelValue)
                    {
                        DM_FREE(np->value);
                    }

                    DM_FREE(np);
                    np=t;
                }
            }
        }

        DM_FREE(pHash->hashTable);
  	}

    pHash->usedSize = 0;

    osa_thread_mutex_unlock(&pHash->mutex);
    osa_thread_mutex_delete(&pHash->mutex);

    return OSA_SUCCESS;
}

void* DmHashFindValue(DM_HASH_T *pHash, BOOL8 (*compare)(void * current_value, void *param), void* param)
{
    void *p = NULL;
	void *cur = NULL;

	if(NULL == compare)
	{
		return NULL;
	}
	
	DM_HASH_ENUM_T hashEnum;
	memset(&hashEnum, 0, sizeof(DM_HASH_ENUM_T));

	osa_thread_mutex_lock(&pHash->mutex);

    osa_hash_init_enumerator(pHash, &hashEnum);
	while (NULL != (cur = osa_hash_enum_next(&hashEnum)))
	{
		if (compare(cur, param) == DM_TRUE)
		{
			p = cur;
			break;
		}
	}

    osa_thread_mutex_unlock(&pHash->mutex);

    return p;
}

#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
