/*
* ==============================================================================
*   Copyright (C) 2010, Percello Ltd, Inc.
* ==============================================================================
*/
/*******************************************************************************
* File name         : dm_api.c
* System name       : femtocell DM
* Module            : Femto DM
* description       :
*
* ------------------------------------------------------------------------------
* Author            : Yuqiang Chen
* Department        :
* Date created      : 2010-11-26
* Update            :
*******************************************************************************/

/*
* ==============================================================================
* Revision:
* ==============================================================================
* date&time         | modify description                           |Author
* ------------------------------------------------------------------------------
*
*
* ------------------------------------------------------------------------------
*/

#include <libgen.h>
#include <dlfcn.h>

#include "dm_internal.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

/* Minimum and maximum values a `signed long long int' can hold.  */
#ifndef LLONG_MAX
#define LLONG_MAX	9223372036854775807LL
#endif

/* Maximum value an `unsigned long long int' can hold.  (Minimum is 0.)  */
#ifndef ULLONG_MAX
#define ULLONG_MAX	18446744073709551615ULL
#endif


extern INT8 *DmIntToStr(INT32);
extern INT8 *getObjName(INT8 *);

extern INT32 DbGetObjectNum(DM_HANDLER_T *pDmHandler, INT8 *pucPath);

extern BOOL8 DmCheckRight(UINT32 lTransactionID, DM_HANDLER_T *pDmHandler, INT32 lRightType, UINT32 ulParaId);
extern INT32 DmGetIDFromPath(DM_HASH_T *pTable, INT8 *pucPath);



extern BOOL8 DbNodeExistCheck(DM_HANDLER_T *pDmHandler,INT8 *pcNodeName);
extern BOOL8 isObject(DM_HANDLER_T *pHandler, UINT32 lId);
extern BOOL8 ConvertPathToXPath(INT8 *pucPath, INT8 *pucXPath);
extern INT8 * DmGetLastNodeName(INT8 *pucObjectPath);
extern BOOL8 DbGetObjectIndexContOld(DM_HANDLER_T *pDmHandler, INT8 *pucPath, UINT32 **pValue);
extern BOOL8 DbAddCurHandlerWithInstance(DM_HANDLER_PARA_T *pAddPara);




typedef struct namevalue_t
{
    struct namevalue_t *pNext;
	DM_HANDLER_PARA_T *pValue;
}NAMEVALUE_T;

typedef struct dm_para_name_t
{
    INT8       *pucName;       /**< Parameter name */
    BOOL8        bWritable;     /**< whether the parameter is writable, only used for getParameterName */
    struct dm_para_name_t *pNext;
}DM_PARA_NAME_T;

typedef struct
{
	DM_HANDLER_T *pDmHandler;
	INT8* pucParamName;
	UINT32 TransactionId;
	INT32 lId;
	DM_PARA_NAME_T *pParaNameFirst;
	DM_PARA_NAME_T *pParaNameLast;
}DM_PARA_INFO_T;

UINT32 g_DbSyncCounter = 0;

/*******************************************************************************
  Module   : Femto DM
  Function : change the name a.b.{i}.c. to a.b.c.
               the returned value should be freeed by caller.
  Input    : UINT8 *pathName
  Output   :
  NOTE     :
  Return   : UINT8 * name: the obj org name
  History  :
      1.     yadong zhang   2011.07.19  first version
 *******************************************************************************/
INT8 *DmChangeOrgNameToFullname(INT8 *pucOrgName)
{
    INT8 *pucRet = NULL;

	UINT32 lLen   = 0;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmChangeOrgNameToFullname %s\n", pucOrgName);
	if(NULL == pucOrgName)
 	{
 	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ call DmChangeOrgNameToFullname error\n");
 	    return pucRet;
 	}
	lLen = strlen(pucOrgName)+2;
	pucRet = calloc(lLen, sizeof(INT8));
	if(NULL == pucRet)
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmChangeOrgNameToFullname memory malloc error\n");
 	    return pucRet; 
	}
	INT8 *pucOrgPath = NULL;
	INT8 *pucTmpPath = NULL;
	INT8 *pucToken = NULL;
	pucOrgPath = strdup(pucOrgName);
	
	pucToken = strtok_r(pucOrgPath, ".", &pucTmpPath);
    while(pucToken != NULL)
    {
        if(isdigit(pucToken[0]))
        {
            ;
        }
		else if(strstr(pucToken, "{i}") != 0)
		{
		    ;
		}
        else
        {
            strcat(pucRet, pucToken);
			strcat(pucRet, ".");
            
        }
        pucToken = strtok_r(NULL, ".", &pucTmpPath);
    }
	DM_FREE(pucOrgPath);
	/*if the path not end with ".", remove it from the converted path*/
	if(pucOrgName[strlen(pucOrgName)-1] != '.')
	{
	    pucRet[strlen(pucRet)-1] = '\0';
	}

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmChangeOrgNameToFullname %s ", pucRet);
	return pucRet;
}


/*******************************************************************************
  Module   : Femto DM
  Function : Get the instance sequece from parameter name.
             For example, "a.1.b.2.c.3." -> "[1,2,3]"
             the returned value should be freeed by caller.
             If there's no instance number in para name, returned value will be NULL
  Input    : UINT8 *pucParaName
  Output   :
  NOTE     :
  Return   : UINT8 *, instance number sequence with brackets. Note that the 
  		     returned value should be freed by caller if it's not NULL.
  History  :
      1.     yuqiang chen   2011.07.25  first version
 *******************************************************************************/
INT8* DmGetInstanceNumSequence(const INT8 *pucParaName)
{
	INT8 *pucRet = NULL;
	UINT32 lLen   = 0;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmChangeOrgNameToFullname %s\n", pucParaName);
	if(NULL == pucParaName)
 	{
 	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ call DmChangeOrgNameToFullname error\n");
 	    return pucRet;
 	}
	lLen = strlen(pucParaName)+2;
	pucRet = calloc(lLen, sizeof(UINT8));
	
	if(NULL == pucRet)
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmChangeOrgNameToFullname memory malloc error\n");
 	    return pucRet; 
	}
	memset(pucRet,0,sizeof(INT8)*lLen);
	strcat(pucRet,"[");
	
	INT8 *pucOrgPath = NULL;
	INT8 *pucTmpPath = NULL;
	INT8 *pucToken = NULL;
	pucOrgPath = strdup(pucParaName);
	
	pucToken = strtok_r(pucOrgPath, ".", &pucTmpPath);
    while(pucToken != NULL)
    {
		if(pucToken[0] == 'i')
		{
			if(strlen(pucToken)>1)
			{
				if(isdigit(pucToken[1]))
		        {
		            strcat(pucRet,pucToken+1);
					strcat(pucRet,",");
		        }				
			}
		}
		else if(isdigit(pucToken[0]))
        {
            strcat(pucRet,pucToken);
			strcat(pucRet,",");
        }
        pucToken = strtok_r(NULL, ".", &pucTmpPath);
    }
	if(strlen(pucRet)>1)
	{
		pucRet[strlen(pucRet)-1] = ']';
	}
	else
	{
		DM_FREE(pucRet);
		pucRet = NULL;
	}
	DM_FREE(pucOrgPath);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmChangeOrgNameToFullname %s ", pucRet);

	return pucRet;
}

/*******************************************************************************
  Module   : Femto DM
  Function : Get the default value as per the instance number sequence.
             For example, 
             pDefaultValue = "[1,1]=\"GSM 900\"[1,2]=\"GSM 900\"[1,3]=\"GSM 900\"";
             pucInsNumSeq = "[1,3]";
             "GSM 900" will be returned.
             If pucInsNumSeq is NULL, a duplicated pDefaultValue will be returned if
             it's not NULL, otherwise "" will returned.
  Input    : UINT8 *pucInsNumSeq, UINT8 *pDefaultValue
  Output   :
  NOTE     :
  Return   : UINT8 *, default value for the instance number sequence. Note that the
  		     returned value should be freed by caller if it's not NULL.
  History  :
      1.     yuqiang chen   2011.07.25  first version
 *******************************************************************************/
INT8* DmGetDefaultValue(const INT8 *pucInsNumSeq, const INT8 *pDefaultValue)
{
	const INT8 *pucTmp = NULL;
	INT8 *pucDefValue = NULL;
	UINT32 i = 0;


	if(NULL == pucInsNumSeq)
	{
		if(NULL != pDefaultValue)
		{
			return strdup(pDefaultValue);
		}
		else
		{
			return strdup("");
		}
	}
	if((NULL != pDefaultValue)&& (pDefaultValue[0] == '\0'))
	{
		return strdup("");
	}

	if (NULL == pDefaultValue)
	{
		return strdup("");
	}

	pucTmp = strstr(pDefaultValue,pucInsNumSeq);

	if(NULL == pucTmp)
	{
		pucTmp = strstr(pDefaultValue,"=");
		if(NULL == pucTmp)
		{
			return strdup(pDefaultValue);
		}
		else
		{
			if(pucTmp == pDefaultValue) /* the first is '=' */
			{
				return strdup(pDefaultValue);
			}
			else if(']' == *(pucTmp-1))
			{
				pucTmp++;					 	/**< jump "=" */
			}
			else
			{
				return strdup(pDefaultValue);  /* example: "C=CN, ST=LiaoNing" */
			}
		}
	}
	else
	{
		pucTmp += strlen(pucInsNumSeq)+1;  	/**< jump "[1,3]=" */
	}


	if(*pucTmp == '\"')
	{
		pucTmp++;
	}

	while((pucTmp[i] != '[')&&(pucTmp[i] != '\0')&&(pucTmp[i] != '\"'))
	{
		i++;
	}

	pucDefValue = (INT8 *)malloc(sizeof(UINT8)*(i+1));
    if(NULL == pucDefValue)
    {
        return NULL;
    }
	memset(pucDefValue,0,sizeof(UINT8)*(i+1));
	memcpy(pucDefValue,pucTmp,sizeof(UINT8)*i);	

	return pucDefValue;
}

/*******************************************************************************
  Module   : Femto DM
  Function : getObjName
  Input    : UINT8 *pathName
  Output   : 
  NOTE     : 
  Return   : UINT8 * name: the obj name
  History  :
      1.     yadong zhang   2010.12.20  first version
 *******************************************************************************/
INT8* getObjName(INT8 *pathName)
{    
    UINT8 *pucFullName = NULL;    
    INT8 *name        = NULL;    
    UINT8 *token       = NULL;    
    UINT8 *secndName   = NULL;    
    INT32 lStrLen      = strlen(pathName);    
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter getObjName %s", pathName);
    pucFullName        = (UINT8*)strdup(pathName);    
    if(pucFullName[lStrLen-1] == (UINT8)'.')   
    {        
        pucFullName[lStrLen-1] = (UINT8)'\0';    
    }
	
	secndName = pucFullName;   /* if there no dot in path name, path name will be object name */
	
    token = (UINT8*)strtok_r((char*)pucFullName, ".", (char**)(&name));
    while(NULL != token)
    {    	
        if((strcmp((char*)token, "{i}")) != 0) 		
        {    	
            secndName = token;   
        } 
        token = (UINT8*)strtok_r(NULL, ".", (char**)(&name));
    }    
    name = strdup((char*)secndName);    
    DM_FREE(pucFullName);    
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave getObjName %s", name);
    return name;
}

/*******************************************************************************
  Module   : Femto DM
  Function : Convert the integer to string
  Input    : UINT32   ulNum,
  Output   : null
  NOTE     : the caller must free the returned pointer
  Return   : UINT8 * : the string of i
  History  :
      1.     yadong zhang   2010.11.26  first version
 *******************************************************************************/
INT8* DmUintToStr(UINT32 ulNum)
{
    INT8 *pucStr = (INT8 *)malloc(sizeof(INT8)*16);
    if(NULL == pucStr)
    {
        return NULL;
    }
	memset(pucStr,0,sizeof(INT8)*16);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmUintToStr %u", ulNum);
    snprintf(pucStr, sizeof(INT8)*16, "%u", ulNum);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmUintToStr %s", pucStr);
    return pucStr;
}

void DmStrlwr(INT8 *pStr)
{
    while(*pStr != '\0')
    {
        *pStr = tolower(*pStr);
        pStr++;
    }

    return;
}

/*******************************************************************************
  Module   : Femto DM
  Function : Convert the integer to string as hex format
  Input    : UINT32   ulNum,
  Output   : null
  NOTE     : the caller must free the returned pointer
  Return   : UINT8 * : the string of i
  History  :
      1.     yadong zhang   2010.11.26  first version
 *******************************************************************************/
INT8* DmUintHexToStr(UINT32 ulNum)
{
    INT8 *pucStr = (INT8 *)malloc(sizeof(INT8)*16);
    if(NULL == pucStr)
    {
        return NULL;
    }
	memset(pucStr,0,sizeof(INT8)*16);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmUintHexToStr %u", ulNum);
    snprintf(pucStr, sizeof(INT8)*16, "0x%x", ulNum);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmUintHexToStr %s", pucStr);
    return pucStr;
}

/*******************************************************************************
  Module   : Femto DM
  Function : Convert the integer to string
  Input    : INT32   lNum,
  Output   : null
  NOTE     : the caller must free the returned pointer
  Return   : UINT8 * : the string of i
  History  :
      1.     Yuqiang Chen   2011.10.28  first version
 *******************************************************************************/
INT8* DmIntToStr(INT32 lNum)
{
    INT8 *pcStr = (INT8 *)malloc(sizeof(INT8)*16);
    if(NULL == pcStr)
    {
        return NULL;
    }
	memset(pcStr,0,sizeof(INT8)*16);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmIntToStr %d", lNum);
    snprintf(pcStr, sizeof(INT8)*16, "%d", lNum);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmIntToStr %s", pcStr);
    return pcStr;
}


/*******************************************************************************
  Module   : Femto DM
  Function : Convert the integer to string
  Input    : UINT64   ulNum,
  Output   : null
  NOTE     : the caller must free the returned pointer
  Return   : UINT8 * : the string of i
  History  :
      1.     Yuqiang Chen   2011.10.28  first version
 *******************************************************************************/
INT8* DmUint64ToStr(UINT64 ullNum)
{
    INT8 *pucStr = (INT8 *)malloc(sizeof(INT8)*32);
    if(NULL == pucStr)
    {
        return NULL;
    }
	memset(pucStr,0,sizeof(INT8)*32);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmUint64ToStr %llu", ullNum);
    snprintf(pucStr, sizeof(INT8)*32, "%llu", ullNum);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmUint64ToStr %s", pucStr);
    return pucStr;
}


/*******************************************************************************
  Module   : Femto DM
  Function : Convert the integer to string
  Input    : INT64   ulNum,
  Output   : null
  NOTE     : the caller must free the returned pointer
  Return   : UINT8 * : the string of i
  History  :
      1.     Yuqiang Chen   2015.09.01  first version
 *******************************************************************************/
INT8* DmInt64ToStr(INT64 llNum)
{
    INT8 *pcStr = (INT8 *)malloc(sizeof(INT8)*32);
    if(NULL == pcStr)
    {
        return NULL;
    }
	memset(pcStr,0,sizeof(INT8)*32);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmInt64ToStr %d", llNum);
    snprintf(pcStr, sizeof(INT8)*32, "%lld", llNum);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmInt64ToStr %s", pcStr);
    return pcStr;
}


/*******************************************************************************
  Module   : Femto DM
  Function : To check if the str in the str
  Input    : UINT8 *dst: the string to be searching in
             UINT8 *src: the string to be searched in dst
  Output   : null
  NOTE     : null
  Return   : TRUE: found in dst, FALSE: not found in dst
  History  :
      1.     yadong zhang   2010.11.30  first version
 *******************************************************************************/
BOOL8 IsStrStr(UINT8 * dst, UINT8 * src)
{
    BOOL8 rtn = DM_FALSE;
	UINT8 *substr = (UINT8*)strstr((char*)dst, (char*)src);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter IsStrStr");
	/* not found */
	if(substr == NULL)
	{
	    rtn = DM_FALSE;
	}
	/* at the begin */
	else if(substr == dst)
	{
	    if(',' == dst[strlen((char*)src)] || ('\0' == dst[strlen((char*)src)]))
		{
		    rtn = DM_TRUE;
		}
	}
	/* at the middle */
	else if( *(substr-1) == ',' && *(substr+strlen((char*)src)) == ',' )
	{
	    rtn = DM_TRUE;
	}
	/* at the end*/
	else if( *(substr-1) == ',' && *(substr+strlen((char*)src)) == '\0' )
	{
	    rtn = DM_TRUE;
	}
	/* exception */
	else
	{
	    rtn = DM_FALSE;
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave IsStrStr");
	return rtn;
}

BOOL8 DmConvertStringToBool(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    BOOL8 rtn = DM_TRUE;
    BOOL8 *pbValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToBool");
	if (strcasecmp("true", pucStringValue)==0)
    {
        *pbValue = DM_TRUE;
    }
	else if(strcasecmp("1", pucStringValue)==0)
	{
        *pbValue = DM_TRUE;
    }
    else
    {
        *pbValue = DM_FALSE;
    }
	*pulStructCnt = 1;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToBool");
    return rtn;
}

BOOL8 DmConvertStringToInt(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
	INT8 *pucStrValue = pucStringValue;
	INT32 *pIntValue = (INT32 *)pStructValue;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToInt");
	if(NULL  == pStructValue)
	{
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToInt parameter error");
		return bRet;
	}
    *pIntValue = atol(pucStrValue);
    *pulStructCnt = 1;
	bRet = DM_TRUE;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToInt");	
    return bRet;
}

BOOL8 DmConvertStringToUint8(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
	INT8 *pucStrValue = pucStringValue;
	UINT8 *pIntValue = (UINT8 *)pStructValue;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToUint8");
	if(NULL  == pStructValue)
	{
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint8 parameter error");
		return bRet;
	}
    *pIntValue = (UINT8)atol(pucStrValue);
    *pulStructCnt = 1;
	bRet = DM_TRUE;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint8");	
    return bRet;
}

BOOL8 DmConvertStringToUint16(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
	INT8 *pucStrValue = pucStringValue;
	UINT16 *pIntValue = (UINT16 *)pStructValue;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToUint16");
	if(NULL  == pStructValue)
	{
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint16 parameter error");
		return bRet;
	}
    *pIntValue = (UINT16)atol(pucStrValue);
    *pulStructCnt = 1;
	bRet = DM_TRUE;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint16");	
    return bRet;
}

BOOL8 DmConvertStringToUint32(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
	INT8 *pucStrValue = pucStringValue;
	UINT32 *pIntValue = (UINT32 *)pStructValue;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToUint32");
	if(NULL  == pStructValue)
	{
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint32 parameter error");
		return bRet;
	}
    *pIntValue = (UINT32)strtoul(pucStrValue,NULL,10);
    *pulStructCnt = 1;
	bRet = DM_TRUE;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint32");	
    return bRet;
}

BOOL8 DmConvertStringToUint32Hex(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
	INT8 *pucStrValue = strdup(pucStringValue);
	UINT32 *pIntValue = (UINT32 *)pStructValue;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToUint32Hex");
	if(NULL  == pStructValue)
	{
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint32Hex parameter error");
        DM_FREE(pucStrValue);
		return bRet;
	}

    DmStrlwr(pucStrValue);

    if (strstr(pucStrValue,"0x")!=NULL)
    {
        sscanf(pucStrValue,"0x%x",pIntValue);
    }
    else
    {
        sscanf(pucStrValue,"%x",pIntValue);
    }
    *pulStructCnt = 1;
    DM_FREE(pucStrValue);
	bRet = DM_TRUE;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint32Hex");	
    return bRet;
}

BOOL8 DmConvertStringToUint64(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
	INT8 *pucStrValue = pucStringValue;
	UINT64 *pIntValue = (UINT64 *)pStructValue;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToUint64");
	if(NULL  == pStructValue)
	{
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint64 parameter error");
		return bRet;
	}
    *pIntValue = (UINT64)strtoull(pucStrValue,NULL,10);
    *pulStructCnt = 1;
	bRet = DM_TRUE;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint64");	
    return bRet;
}

BOOL8 DmConvertStringToInt64(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
	INT8 *pucStrValue = pucStringValue;
	INT64 *pIntValue = (INT64 *)pStructValue;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToInt64");
	if(NULL  == pStructValue)
	{
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToInt64 parameter error");
		return bRet;
	}
    *pIntValue = (INT64)strtoll(pucStrValue,NULL,10);
    *pulStructCnt = 1;
	bRet = DM_TRUE;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToInt64");
    return bRet;
}


BOOL8 DmConvertStringToUint8Array(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue)
{
	INT8 *pcStr = NULL;
	UINT8 *token = (UINT8*)strtok_r((char*)(pucStringValue), ",", &pcStr);
	UINT8 *pValue = (UINT8 *)pStructValue;
	UINT8 ucLow = 0;
	UINT8 ucHigh = 0;
	UINT32 i = 0;
	UINT32 j = 0;
	UINT8 *pDotDot = NULL;

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToUint8Array");

	if (lStringCnt < 0)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmConvertStringToUint8Array : lStringCnt < 0");	
		return DM_FALSE;
	}

	while(token != NULL)
	{
		pDotDot = (UINT8*)strstr((char*)token, "..");
		if (NULL != pDotDot)
		{
			ucLow = atoi((char*)token);
			ucHigh = atoi((char*)(pDotDot+2));

			for (j = ucLow; j <= ucHigh; j++)
			{
				pValue[i] = j;
				i++;
			}
		}
		else
		{
			pValue[i] = atoi((char*)token);
			i++;
		}
		token = (UINT8*)strtok_r(NULL, ",", &pcStr);
	}
	*pulStructCnt = i;
    
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint8Array");
	return DM_TRUE;
}

BOOL8 DmConvertStringToUint16Array(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
	INT8 *pcStr = NULL;
    UINT8 *token = (UINT8*)strtok_r((char*)(pucStringValue), ",", &pcStr);
	UINT16 *pValue = (UINT16 *)pStructValue;
    UINT16 usLow = 0;
    UINT16 usHigh = 0;
	UINT32 i = 0;
	UINT32 j = 0;
    UINT8 *pDotDot = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToUint16Array");

	if (lStringCnt < 0)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmConvertStringToUint16Array : lStringCnt < 0");	
		return DM_FALSE;
	}

	while(token != NULL)
	{
        pDotDot = (UINT8*)strstr((char*)token, "..");
        if (NULL != pDotDot)
        {
            usLow = atoi((char*)token);
            usHigh = atoi((char*)(pDotDot+2));

            for (j = usLow; j <= usHigh; j++)
            {
                pValue[i] = j;
                i++;
            }
        }
        else
        {
            pValue[i] = atoi((char*)token);
            i++;
        }
		token = (UINT8*)strtok_r(NULL, ",", &pcStr);
    }
    *pulStructCnt = i;
     
 	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint16Array");  
    return DM_TRUE;
}

BOOL8 DmConvertStringToUint32Array(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
	INT8 *pcStr = NULL;
    UINT8 *token = (UINT8*)strtok_r((char*)(pucStringValue), ",", &pcStr);
	UINT32 *pValue = (UINT32 *)pStructValue;
    UINT32 ulLow = 0;
    UINT32 ulHigh = 0;
	UINT32 i = 0;
	UINT32 j = 0;
    UINT8 *pDotDot = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToUint32Array");
    
	if (lStringCnt < 0)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmConvertStringToUint32Array : lStringCnt < 0");	
		return DM_FALSE;
	}

	while(token != NULL)
	{
        pDotDot = (UINT8*)strstr((char*)token, "..");
        if (NULL != pDotDot)
        {
            ulLow = atoi((char*)token);
            ulHigh = atoi((char*)(pDotDot+2));

            for (j = ulLow; j <= ulHigh; j++)
            {
                pValue[i] = j;
                i++;
            }
        }
        else
        {
            pValue[i] = atoi((char*)token);
            i++;
        }
		token = (UINT8*)strtok_r(NULL, ",", &pcStr);
    }
    *pulStructCnt = i;
     
 	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToUint32Array");  
    return DM_TRUE;
}

BOOL8 DmConvertStringToCharArray(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
	INT8 *pValue = (INT8 *)pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToCharArray");
    strcpy(pValue, (char*)pucStringValue);
	*pulStructCnt = strlen((char*)pucStringValue);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToCharArray");
	return DM_TRUE;
}

BOOL8 DmConvertStringToInt8Array(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    INT8 *pcStr = NULL;
    /* call the convert func */
    UINT8 *token = (UINT8*)strtok_r((char*)(pucStringValue), ",", &pcStr);
    INT8  *pValue = (INT8 *)pStructValue;
    INT8  cLow = 0;
    INT8  cHigh = 0;

    INT32 i = 0;
    INT32 j = 0;
    UINT8 *pDotDot = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToInt8Array");  
    while(token != NULL)
    {
        pDotDot = (UINT8*)strstr((char*)token, "..");
        if (NULL != pDotDot)
        {
            cLow = atoi((char*)token);
            cHigh = atoi((char*)(pDotDot+2));

            for (j = cLow; j <= cHigh; j++)
            {
                pValue[i] = j;
                i++;
            }
        }
        else
        {
            pValue[i] = atoi((char*)token);
            i++;
        }
        token = (UINT8*)strtok_r(NULL, ",", &pcStr);
    }

    *pulStructCnt = (UINT32)i;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToInt8Array");  
    return DM_TRUE;
}

BOOL8 DmConvertStringToInt16Array(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
	INT8 *pcStr = NULL;
	/* call the convert func */
	UINT8 *token = (UINT8*)strtok_r((char*)(pucStringValue), ",", &pcStr);
	INT16 *pValue = (INT16 *)pStructValue;
	INT16 sLow = 0;
	INT16 sHigh = 0;

	INT32 i = 0;
	INT32 j = 0;
	UINT8 *pDotDot = NULL;

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToInt16Array");	
	while(token != NULL)
	{
		pDotDot = (UINT8*)strstr((char*)token, "..");
		if (NULL != pDotDot)
		{
			sLow = atoi((char*)token);
			sHigh = atoi((char*)(pDotDot+2));

			for (j = sLow; j <= sHigh; j++)
			{
				pValue[i] = j;
				i++;
			}
		}
		else
		{
			pValue[i] = atoi((char*)token);
			i++;
		}
		token = (UINT8*)strtok_r(NULL, ",", &pcStr);
	}

	*pulStructCnt = (UINT32)i;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToInt16Array");  
	return DM_TRUE;
}

BOOL8 DmConvertStringToEnum(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
    INT8 *pValueList = NULL;
    INT8 *pToken = NULL;
    INT8 *pTmp = NULL;
    INT8   *pEnumString = (INT8 *)pucStringValue;
    UINT32 *pEnumValue = (UINT32 *)pStructValue;
    UINT32 cnt = 0;

    if (pcValueRange==NULL)
    {
        return bRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToEnum pucStringValue=%s range=%s",pucStringValue,pcValueRange);

    pValueList = strdup(pcValueRange);

    if (pValueList==NULL)
    {
        return bRet;
    }

    *pulStructCnt = 0;
    *pEnumValue = 0;

    pToken = strtok_r(pValueList,",",&pTmp);

    while(pToken)
    {
        if (strcmp(pEnumString, pToken) == 0)
        {
            *pulStructCnt = 1;
            *pEnumValue = cnt;
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ %s matched, enum-value=0x%x pEnumValue=0x%x",pEnumString,*pEnumValue,pEnumValue);
            bRet = DM_TRUE;
            break;
        }
        else if (cnt >= DM_UINT32_ENUM_MAX_VALUE)
        {
            break;
        }

        cnt++;
        pToken = strtok_r(NULL,",",&pTmp);
    }

    DM_FREE(pValueList);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToEnum");
    return bRet;
}

BOOL8 DmConvertStringToBitmap(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
    INT8 *pValueList = NULL;
    INT8 *pToken = NULL;
    INT8 *pTmp = NULL;
    INT8   *pBitmapString = (INT8 *)pucStringValue;
    INT8 *pBitmap = NULL;
    INT8 *pTmpBitmap = NULL;
    UINT32 *pBitmapValue = (UINT32 *)pStructValue;
    UINT32 cnt = 0;

    if (pcValueRange==NULL)
    {
        return bRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertStringToBitmap pucStringValue=%s range=%s",pucStringValue,pcValueRange);

    pValueList = strdup(pcValueRange);
    pBitmapString = strdup(pucStringValue);

    if (pValueList==NULL || pBitmapString==NULL)
    {
        DM_FREE(pValueList);
        DM_FREE(pBitmapString);
        return bRet;
    }

    *pulStructCnt = 0;
    *pBitmapValue = 0;

    pBitmap = strtok_r(pBitmapString,",",&pTmpBitmap);

    
    while(pBitmap)
    {
        pToken = strtok_r(pValueList,",",&pTmp);

        cnt = 1;
        while(pToken)
        {
            if (strcmp(pBitmap, pToken) == 0)
            {
                (*pulStructCnt)++;
                *pBitmapValue |= cnt;
                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ %s matched, bitmap-value=0x%x pBitmapValue=0x%x",pBitmap,*pBitmapValue,pBitmapValue);
            }
            else if (cnt >= DM_UINT32_BITMAP_MAX_VALUE)
            {
                break;
            }

            cnt = cnt<<1;
            pToken = strtok_r(NULL,",",&pTmp);
        }

        memset(pValueList,0,strlen(pValueList));
        strcpy(pValueList,pcValueRange);

        pBitmap = strtok_r(NULL,",",&pTmpBitmap);
    }

    DM_FREE(pValueList);
    DM_FREE(pBitmapString);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertStringToBitmap");
    bRet = DM_TRUE;
    return bRet;
}

BOOL8 DmConvertBoolToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = pucStringValue;
	BOOL8 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertBoolToString");	
    if(DM_TRUE == 	*pValue)
	{
	    strcpy(pcStr,"1");
	}
	else
	{
	    strcpy(pcStr, "0");
	}
    *plStringCnt = strlen(pcStr);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertBoolToString");	
    return DM_TRUE;
}

BOOL8 DmConvertUint8ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = NULL;
	UINT8 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertUint8ToString");

	pcStr = DmUintToStr(*pValue);
    strcpy(pucStringValue, pcStr);
	DM_FREE(pcStr);
    *plStringCnt = strlen(pucStringValue);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertUint8ToString");
    return DM_TRUE;
}

BOOL8 DmConvertUint16ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = NULL;
	UINT16 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertUint16ToString");

	pcStr = DmUintToStr(*pValue);
    strcpy(pucStringValue, pcStr);
	DM_FREE(pcStr);
    *plStringCnt = strlen(pucStringValue);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertUint16ToString");	
    return DM_TRUE;
}

BOOL8 DmConvertUint32ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = NULL;
	UINT32 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertUint32ToString");

	pcStr = DmUintToStr(*pValue);

	if(NULL != pcStr)
	{
        strcpy(pucStringValue, pcStr);
		DM_FREE(pcStr);
		*plStringCnt = strlen(pucStringValue);
	}
	else
	{
		*plStringCnt = 0;
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertUint32ToString");	
    return DM_TRUE;
}

BOOL8 DmConvertUint32HexToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = NULL;
	UINT32 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertUint32ToString");

	pcStr = DmUintHexToStr(*pValue);

	if(NULL != pcStr)
	{
        strcpy(pucStringValue, pcStr);
		DM_FREE(pcStr);
		*plStringCnt = strlen(pucStringValue);
	}
	else
	{
		*plStringCnt = 0;
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertUint32ToString");	
    return DM_TRUE;
}


BOOL8 DmConvertUint64ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = NULL;
	UINT64 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertUint64ToString");

	pcStr = DmUint64ToStr(*pValue);
    strcpy((INT8 *)pucStringValue, pcStr);
	DM_FREE(pcStr);
    *plStringCnt = strlen((INT8 *)pucStringValue);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertUint64ToString");	
    return DM_TRUE;
}

BOOL8 DmConvertInt64ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = NULL;
	INT64 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertInt64ToString");

	pcStr = DmInt64ToStr(*pValue);
    strcpy((INT8 *)pucStringValue, pcStr);
	DM_FREE(pcStr);
    *plStringCnt = strlen((INT8 *)pucStringValue);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertInt64ToString");
    return DM_TRUE;
}

BOOL8 DmConvertInt8ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = NULL;
	INT8 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertInt8ToString");

    /* set the value from the db */
	pcStr = DmIntToStr(*pValue);
    strcpy(pucStringValue, pcStr);
    DM_FREE(pcStr);
    *plStringCnt = strlen(pucStringValue);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertInt8ToString");	
    return DM_TRUE;
}

BOOL8 DmConvertInt16ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = NULL;
	INT16 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertInt16ToString");

    /* set the value from the db */
	pcStr = DmIntToStr(*pValue);
    strcpy(pucStringValue, pcStr);
    DM_FREE(pcStr);
    *plStringCnt = strlen(pucStringValue);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertInt16ToString");	
    return DM_TRUE;
}

BOOL8 DmConvertInt32ToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = NULL;
	INT32 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertInt32ToString");

    /* set the value from the db */
	pcStr = DmIntToStr(*pValue);
    strcpy(pucStringValue, pcStr);
    DM_FREE(pcStr);
    *plStringCnt = strlen(pucStringValue);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertInt32ToString");	
    return DM_TRUE;
}

BOOL8 DmConvertUint8ArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = pucStringValue;
	UINT8  *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertUint8ArrayToString");

    if(0 != lStructCnt)
    {
    	INT8* tmpValue = NULL;
    	INT32 i = 0;

    	for(i=0; i<lStructCnt; i++)
    	{
    	    tmpValue = DmUintToStr(pValue[i]);
    		strcat((char*)(pcStr), tmpValue);
    	    if(i != lStructCnt-1)
    		{
    		    strcat((char*)(pcStr),",");
    		}
    		DM_FREE(tmpValue);
    	}
    }
    *plStringCnt = strlen((char*)pcStr);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertUint8ArrayToString");
    return DM_TRUE;
}

BOOL8 DmConvertUint16ArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = pucStringValue;
	UINT16 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertUint16ArrayToString");

    if(0 != lStructCnt)
    {
    	INT8* tmpValue = NULL;
    	INT32 i = 0;

    	for(i=0; i<lStructCnt; i++)
    	{
    	    tmpValue = DmUintToStr(pValue[i]);
    		strcat(pcStr, tmpValue);
    	    if(i != lStructCnt-1)
    		{
    		    strcat(pcStr,",");
    		}
    		DM_FREE(tmpValue);
    	}
    }
    *plStringCnt = strlen(pcStr);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertUint16ArrayToString");	
    return DM_TRUE;
}

BOOL8 DmConvertUint32ArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = pucStringValue;
	UINT32 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertUint32ArrayToString");

    if(0 != lStructCnt)
    {
    	INT8* tmpValue = NULL;
    	INT32 i = 0;

    	for(i=0; i<lStructCnt; i++)
    	{
    	    tmpValue = DmUintToStr(pValue[i]);
    		strcat(pcStr, tmpValue);
    	    if(i != lStructCnt-1)
    		{
    		    strcat(pcStr,",");
    		}
    		DM_FREE(tmpValue);
    	}
    }
    *plStringCnt = strlen(pcStr);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertUint32ArrayToString");	
    return DM_TRUE;
}

BOOL8 DmConvertCharArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
	INT8 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertCharArrayToString");

    if((0 != lStructCnt)||(NULL != pStructValue))
    {
		strncpy((char *)pucStringValue,pValue,MAX_SET_STRING_VALUE_LEN-1);
		*plStringCnt = strlen((char *)pucStringValue);
    }
	else
	{
		*plStringCnt = 0;
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertCharArrayToString");
    return DM_TRUE;
}

BOOL8 DmConvertInt8ArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = pucStringValue;
	INT8 *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertInt8ArrayToString");

    if(0 != lStructCnt)
    {

    	INT8* tmpValue = NULL;
    	INT32          i = 0;

    	for(i=0; i<lStructCnt; i++)
    	{
    	    tmpValue = DmIntToStr(pValue[i]);
    		strcat(pcStr, tmpValue);
    	    if(i != lStructCnt-1)
    		{
    		    strcat(pcStr,",");
    		}
    		DM_FREE(tmpValue);
    	}

    }
    *plStringCnt = strlen(pcStr);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertInt8ArrayToString");	
    return DM_TRUE;
}

BOOL8 DmConvertInt16ArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pcStr = pucStringValue;
	INT16  *pValue = pStructValue;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertInt16ArrayToString");

    if(0 != lStructCnt)
    {

    	INT8* tmpValue = NULL;
    	INT32          i = 0;

    	for(i=0; i<lStructCnt; i++)
    	{
    	    tmpValue = DmIntToStr(pValue[i]);
    		strcat(pcStr, tmpValue);
    	    if(i != lStructCnt-1)
    		{
    		    strcat(pcStr,",");
    		}
    		DM_FREE(tmpValue);
    	}

    }
    *plStringCnt = strlen(pcStr);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertInt16ArrayToString");	
    return DM_TRUE;
}

BOOL8 DmConvertEnumToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
    INT8 *pValueList = NULL;
    INT8 *pToken = NULL;
    INT8 *pTmp = NULL;
    UINT32 *pEnum = (UINT32 *)pStructValue;
    UINT32 cnt = 0;

    if (pcValueRange==NULL)
    {
        return bRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertEnumToString enum=%u range=%s",*pEnum,pcValueRange);

    pValueList = strdup(pcValueRange);

    if (pValueList==NULL)
    {
        return bRet;
    }

    pToken = strtok_r(pValueList,",",&pTmp);

    while(pToken)
    {
        if (*pEnum==cnt)
        {
            strcpy(pucStringValue, pToken);
            bRet = DM_TRUE;
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ find enum 0x%x -> %s",*pEnum,pucStringValue);
            break;
        }
        else if (cnt >= DM_UINT32_ENUM_MAX_VALUE)
        {
            break;
        }

        cnt++;
        pToken = strtok_r(NULL,",",&pTmp);
    }

    DM_FREE(pValueList);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertEnumToString");
    return bRet;
}

BOOL8 DmConvertBitmapToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    BOOL8 bRet = DM_FALSE;
    INT8  *pValueList = NULL;
    INT8  *pToken = NULL;
    INT8  *pTmp = NULL;
    UINT32 *pBitmap = (UINT32 *)pStructValue;
    INT8  *pDest = (INT8 *)pucStringValue;
    INT8  *cDelimiters = ",";
    UINT32 cnt = 1;

    if (pcValueRange==NULL)
    {
        return bRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmConvertBitmapToString pBitmap=%u range=%s",*pBitmap,pcValueRange);

    pValueList = strdup(pcValueRange);

    if (pValueList==NULL)
    {
        return bRet;
    }

    pToken = strtok_r(pValueList,",",&pTmp);

    bRet = DM_TRUE;
    while(pToken)
    {
        if ((*pBitmap)&cnt)
        {
            if (strcmp(pDest,"") != 0)
            {
                strcat(pDest, cDelimiters);
            }
            strcat(pDest, pToken);
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ update bitmap 0x%x (%s)",*pBitmap,pDest);
        }
        else if (cnt >= DM_UINT32_BITMAP_MAX_VALUE)
        {
            bRet = DM_FALSE;
            break;
        }

        cnt = cnt<<1;
        pToken = strtok_r(NULL,",",&pTmp);
    }

    DM_FREE(pValueList);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmConvertBitmapToString");
    return bRet;
}

INT8* GetRootNameFromPath(INT8 *pucName)
{
//	INT8 aucRootName[MAX_NAME_LEN]= {0};
	
	INT32 lLen = 0;
	INT8* pucNameIndex = NULL;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter GetRootNameFromPath %s\n", pucName);	
	if(NULL == pucName)
	{
	    return NULL;
	}
	pucNameIndex = index(pucName, '.');
	if(NULL == pucNameIndex)
	{
	    lLen = strlen(pucName);
	}
	else
	{
 
	    lLen = abs(pucName - pucNameIndex);
	}

	INT8 *aucRootName = NULL;
	aucRootName = (INT8 *)malloc(sizeof(INT8)*MAX_NAME_LEN);

	if (aucRootName == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave GetRootNameFromPath NULL\n");	

		return aucRootName;
	}

	memset(aucRootName,0,sizeof(INT8)*MAX_NAME_LEN);

	strncpy(aucRootName, pucName, lLen);
	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave GetRootNameFromPath %s\n", aucRootName);	
	return aucRootName;
}

DM_HANDLER_T *GetDmHandlerByName(INT8 *pucName)
{
    DM_HANDLER_T *pResult = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter GetDmHandlerByName %s", pucName);	

	INT8        *pucRootName = GetRootNameFromPath(pucName);

	if(NULL == pucRootName)
	{
        
	    return NULL;
	}

	pResult = (DM_HANDLER_T*)DmHashGetFromPath(&g_dmHandlerHash, pucRootName);
	DM_FREE(pucRootName);
		
    if(NULL == pResult)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ (%s) GetDmHandlerByName null",pucName);
    }
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave GetDmHandlerByName");
    }
    
	return pResult;

}

static BOOL8 CompareLibHandlerOfDmHandler(void* p, void* pLibHandler)
{
	DM_HANDLER_T *pDmHandler = (void *)p;
	
	return  pLibHandler == pDmHandler->pDlHandler;
}

DM_HANDLER_T *GetDmHandlerByLibHandler(void* pLibHandler)
{
    DM_HANDLER_T *pFoundDmHandler = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter GetDmHandlerByLibHandler");

	pFoundDmHandler = DmHashFindValue(&g_dmHandlerHash, CompareLibHandlerOfDmHandler ,pLibHandler);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave GetDmHandlerByLibHandler");

    return pFoundDmHandler;
}

static BOOL8 CompareTransactionIdOfDmHandler(void* p, void* param)
{
	DM_HANDLER_T *pDmHandler = (void *)p;
	INT8 *pTransId = (INT8 *)param;
	
	return  (NULL != DmHashGetValue(&pDmHandler->hashTransactions, pTransId));
}

DM_HANDLER_T *GetDmHandlerByTransId(UINT32 transactionId)
{
    DM_HANDLER_T *pDmHandler = NULL;

    INT8 *pTransId = DmUintToStr(transactionId);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter GetDmHandlerByTransId");

	pDmHandler = DmHashFindValue(&g_dmHandlerHash, CompareTransactionIdOfDmHandler, pTransId);

    DM_FREE(pTransId);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave GetDmHandlerByTransId");

    return pDmHandler;
}


INT8 DmReadFileToBuf(INT8 * filename, INT8 * buf, UINT32 len)
{
	FILE * fp = NULL;

	if( (filename ==NULL)||(buf==NULL)||(len==0) )
		return DM_FAILURE;

	if((fp=fopen(filename,"rb"))==NULL)
	{
		return DM_FAILURE;
	}
	if(fread(buf, sizeof(char), len, fp) != len)
	{
		fclose(fp);
		return DM_FAILURE;
	}
	fclose(fp);
	
	return DM_SUCCESS;
}

INT8 DmWriteBufToFile(INT8 * filename, INT8 * buf,UINT32 len)
{
	FILE * fp = NULL;
	UINT32 realwritelen = 0;

	if( (filename ==NULL)||(buf==NULL) )
		return DM_FAILURE;

	if((fp=fopen(filename,"w+"))==NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH,"DM Trace ------ Cannot open file :%s",filename);
		return DM_FAILURE;
	}

	if((realwritelen=fwrite(buf,sizeof(char),len,fp))!=len)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH,"DM Trace ------ write file error:%s,realwrite-len:%d,reqwrite-len:%d",filename,realwritelen,len);
		fclose(fp);
		return DM_FAILURE;
	}
	fflush(fp);

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE,"DM Trace ------ DmWriteBufToFile, before fsync");
	fsync(fileno(fp));
	DM_TRACE(DM_TRACE_LEVEL_MIDDLE,"DM Trace ------ DmWriteBufToFile, after fsync");

	DM_TRACE(DM_TRACE_LEVEL_HIGH,"DM Trace ------ save %s to flash counter = %d",filename,++g_DbSyncCounter);

	fclose(fp);

	return DM_SUCCESS;
}


static INT32 DmExecSyncDBFile(INT8 * from,INT8 * to1,INT8 * to2)
{
	INT8 tmppath[128] = {0};
    INT32 SameTo1 = 0;
    INT32 SameTo2 = 0;

	struct stat filestat;
	INT8 * buf = NULL;

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE,"DM Trace ------ enter DmExecSyncDBFile, from = %s, to1 = %s, to2 = %s", from, to1, to2);

	if((from ==NULL)||(to1==NULL)||(to2==NULL))
	{
		return DM_FAILURE;
	}

    SameTo1 = strcmp(from, to1);
    SameTo2 = strcmp(from, to2);


    /* "To1-file" or "To2-file" is different from "from-file" */
    if (SameTo1!=0 || SameTo2!=0)
    {
        memset(&filestat, 0, sizeof(struct stat));
    	stat(from,&filestat);
    	if((buf=malloc(filestat.st_size))==NULL)
    	{
    		DM_TRACE(DM_TRACE_LEVEL_HIGH,"DM Trace ------ Malloc error in DmExecSyncDBFile");
    		return DM_FAILURE;
    	}

    	if(DmReadFileToBuf(from,buf,filestat.st_size)!=DM_SUCCESS)
    	{
    		DM_FREE(buf);
    		return DM_FAILURE;
    	}

        /* backup to "To1-file" if it is different from "from-file" */
        if (SameTo1!=0)
        {
        	snprintf(tmppath,sizeof(tmppath),"%s-bak",to1);
        	if(DmWriteBufToFile(tmppath,buf,filestat.st_size)!=DM_SUCCESS)
        	{
        		DM_FREE(buf);
        		return DM_FAILURE;
        	}

            remove(to1);

        	if(rename(tmppath,to1)!=DM_SUCCESS)
        	{
        		DM_FREE(buf);
        		DM_WARNING("DM Trace ------ rename to1 error in DmExecSyncDBFile");
        		return DM_FAILURE;
        	}
        }

        /* backup to "To2-file" if it is different from "from-file" */
        if (SameTo2!=0)
        {
        	snprintf(tmppath,sizeof(tmppath),"%s-bak",to2);
        	if(DmWriteBufToFile(tmppath,buf,filestat.st_size)!=DM_SUCCESS)
        	{
        		DM_FREE(buf);
        		return DM_FAILURE;
        	}

            remove(to2);

        	if(rename(tmppath,to2)!=DM_SUCCESS)
        	{
        		DM_FREE(buf);
        		DM_WARNING("DM Trace ------ rename to2 error in DmExecSyncDBFile");
        		return DM_FAILURE;
        	}
        }

    	DM_FREE(buf);
    }

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE,"DM Trace ------ leave DmExecSyncDBFile");

	return DM_SUCCESS;
}	


INT32 DmSyncDBFileReq(DM_HANDLER_T* pDmHandler)
{
    INT32 lRet = DM_FAILURE;
	INT8 tmppath[128] = {0};
    INT8 *fromTmpFile = pDmHandler->pucFilePath;

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE,"DM Trace ------ enter DmSyncDBFileReq, DbName = %s ", pDmHandler->pucDbName);

	/* save xml data from memory to file. */
    xmlSaveFormatFileEnc(pDmHandler->pucFilePath, pDmHandler->pXmlDocument, (const char *)"UTF-8", 1);	

    /* Encrypt data to a temp file */
    if (g_DmCtrl.encryptFunc)
    {
        snprintf(tmppath,sizeof(tmppath),"/tmp/%s-encrypt", basename(pDmHandler->pucFilePath));
        if (g_DmCtrl.encryptFunc(DM_TRUE,pDmHandler->pucFilePath,tmppath)!=DM_SUCCESS)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH,"DM Trace ------ encryption failed, from = %s, to = %s", pDmHandler->pucFilePath, tmppath);
            remove(tmppath);
			remove(pDmHandler->pucFilePath);  /* remove unused XML file */
            return DM_FAILURE;
        }

        /* change from file to encrypted file */
        fromTmpFile = tmppath;
		remove(pDmHandler->pucFilePath);  /* remove unused XML file */
    }

	 /* sync db file to flash */
    lRet = DmExecSyncDBFile(fromTmpFile, pDmHandler->pLoadPath,pDmHandler->pBackupPath);
    if (lRet != DM_SUCCESS)
    {
	    DM_TRACE(DM_TRACE_LEVEL_HIGH,"DM Trace ------ failed to do DmExecSyncDBFile.");
    }

    remove(fromTmpFile);  /* remove unused file */

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE,"DM Trace ------ leave DmSyncDBFileReq");

	return lRet;
}

BOOL8 DmRegisterPatternReq(DM_REG_PATTERN_REQ_T *pPara)
{
	DM_HANDLER_T *pDmHandler = NULL;
	DM_NODE_INFO_T *pPattern = NULL;
	INT32          *plId = NULL;
	BOOL8           bRet = DM_FALSE;
	BOOL8           bCreateNewXmlFile = DM_TRUE;
	INT8          *pucKey = NULL;

    INT8 dbLoadingCmd[256] = {0};

	INT32         i          = 0;

    BOOL8         bLoadPathUsed = DM_FALSE;

    INT32         bootResetCnt = 0;
    INT32         *bootResetArray = NULL;
    INT8          dbXmlReplace[128] = {0};
    INT8          dbXmlCorrupted[128] = {0};

    BOOL8         bDecryptedFailure = DM_FALSE;

	if(NULL == pPara || NULL == pPara->pucRootName || NULL == pPara->pPatternFirstNode || pPara->lPatternSize <= 0)
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmRegisterPatternReq parameter error");
	    return DM_FALSE;
	}

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmRegisterPatternReq %s\n", pPara->pucRootName);	

	/*get dm handler*/
	pDmHandler = GetDmHandlerByName(pPara->pucRootName);
	if(NULL == pDmHandler)
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ failed to get dm-handler in DmRegisterPatternReq");
	    return DM_FALSE;
	}

	/*add to dm handler*/
	pDmHandler->pPattern = pPara->pPatternFirstNode;
	pDmHandler->lPatternLength = pPara->lPatternSize;

    pDmHandler->pAttrPattern = pPara->pAttrPatternFirstNode;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s db c-type size = %u bytes", pPara->pucRootName, pPara->pPatternFirstNode[0].ulCTypeSize);

	/*initialize the  crud*abc */
	pPattern = pPara->pPatternFirstNode;
	for(i = 0; i < pDmHandler->lPatternLength ; i++)
	{
	    pPattern[i].pGetHandler    = NULL;
	    pPattern[i].pNotifyHandler = NULL;	

		pPattern[i].pNodeHandler   = (DM_NODE_HANDLER_T*)DM_MALLOC(sizeof(DM_NODE_HANDLER_T));
		if(NULL == pPattern[i].pNodeHandler)
		{
		    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmRegisterPatternReq malloc memory failure");
			return DM_FALSE;
		}
		/*register handler*/
		pPattern[i].pNodeHandler->pAddAftHandler = (HANDLER_FUNC)DefaultAddAftHandler;
		pPattern[i].pNodeHandler->pAddBefHandler = NULL;
		pPattern[i].pNodeHandler->pAddCurHandler = (HANDLER_FUNC)DbAddCurHandler;

		pPattern[i].pNodeHandler->pDelAftHandler = (HANDLER_FUNC)DefaultDelAftHandler;
		pPattern[i].pNodeHandler->pDelBefHandler = NULL;
		pPattern[i].pNodeHandler->pDelCurHandler = (HANDLER_FUNC)DbDelCurHandler;

		pPattern[i].pNodeHandler->pGetAftHandler = NULL;
		pPattern[i].pNodeHandler->pGetBefHandler = NULL;
		pPattern[i].pNodeHandler->pGetCurHandler = (HANDLER_FUNC)DbGetCurHandler;

		pPattern[i].pNodeHandler->pSetAftHandler = (HANDLER_FUNC)DefaultSetAftHandler;
		pPattern[i].pNodeHandler->pSetBefHandler = NULL;
		pPattern[i].pNodeHandler->pSetCurHandler = (HANDLER_FUNC)DbSetCurHandler;

		pPattern[i].pNodeHandler->pGetAttrAftHandler = NULL;
		pPattern[i].pNodeHandler->pGetAttrBefHandler = NULL;
		pPattern[i].pNodeHandler->pGetAttrCurHandler = (HANDLER_FUNC)DbGetAttrCurHandler;

		pPattern[i].pNodeHandler->pSetAttrAftHandler = (HANDLER_FUNC)DefaultSetAttrAftHandler;
		pPattern[i].pNodeHandler->pSetAttrBefHandler = NULL;
		pPattern[i].pNodeHandler->pSetAttrCurHandler = (HANDLER_FUNC)DbSetAttrCurHandler;

		/*initialize the name and id relation*/
		plId = (INT32*)DM_MALLOC(sizeof(INT32));
		if(NULL == plId)
		{
		    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmRegisterPatternReq malloc memory failure");
			return DM_FALSE;
		}
		*plId = i;

	    if(DM_TRUE != pDmHandler->pDbIsObjectFunc(pPattern[i].ucParaType))
    	{
    	    UINT32 uStrLen = strlen(pPattern[pPattern[i].ulParentIdx].pucNodeName)+strlen(pPattern[i].pucNodeName);
    	    INT8 *pucTmpName = calloc(uStrLen+3, sizeof(UINT8));
			if(NULL == pucTmpName)
			{
			    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmRegisterPatternReq malloc memory failure");
				if (plId != NULL)
				{
					DM_FREE(plId);
				}

				return DM_FALSE;
			}
			strcat(pucTmpName, pPattern[pPattern[i].ulParentIdx].pucNodeName);
			strcat(pucTmpName, pPattern[i].pucNodeName);
			pucKey = DmChangeOrgNameToFullname(pucTmpName);
			DM_FREE(pucTmpName);
    	}
		else
		{
			pucKey = DmChangeOrgNameToFullname(pPattern[i].pucNodeName);
		}

		if(NULL == pucKey)
		{
		    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmRegisterPatternReq malloc memory failure");
			if (plId != NULL)
			{
				DM_FREE(plId);
			}

			return DM_FALSE;
		}
		int iRet = DmHashAdd(&(pDmHandler->hashNameID), pucKey, plId);

		if (iRet == OSA_FAILURE)
		{
			if (plId != NULL)
			{
				DM_FREE(plId);
				plId = NULL;
			}

			if (pucKey != NULL)
			{
				DM_FREE(pucKey);
			}

            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Add name id to hash table failed...");
			return DM_FALSE;
		}

		DM_FREE(pucKey);

        if (pPattern[i].pBootAction != NULL && strlen(pPattern[i].pBootAction)>0)
        {
            if (bootResetArray == NULL)
            {
                bootResetArray = (INT32 *)DM_MALLOC(sizeof(INT32)*pDmHandler->lPatternLength);

                if (bootResetArray == NULL)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed..., bootResetArray == NULL in DmRegisterPatternReq");
                    return DM_FALSE;
                }
                else
                {
                    osa_mem_set(bootResetArray,0,sizeof(INT32)*pDmHandler->lPatternLength);
                }

                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ boot-reset action needed... in DmRegisterPatternReq");
            }

            bootResetArray[bootResetCnt] = i;
            bootResetCnt++;
        }
	}

    if (pDmHandler->bExtDb == DM_TRUE)
    {
        pDmHandler->pXmlDocument = NULL;
        return DM_TRUE;
    }

    /* support db file replacing from external entity */
    snprintf(dbXmlReplace,sizeof(dbXmlReplace),"%s.replace",pDmHandler->pLoadPath);
    snprintf(dbXmlCorrupted,sizeof(dbXmlCorrupted),"%s.replace.corrupted",pDmHandler->pLoadPath);   
    if (access(dbXmlReplace, F_OK) == 0)
    {
        xmlDocPtr pXmlDocument = NULL;

		bDecryptedFailure = DM_FALSE;    // if not encrypt mode, suppose decrypt is OK

        /* we should decrypt db files firstly */
        if (g_DmCtrl.encryptFunc)
        {
            if (g_DmCtrl.encryptFunc(DM_FALSE,dbXmlReplace,DM_DECRYPT_TMP_FILE)!=DM_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ decrypt %s failed... in DmRegisterPatternReq",dbXmlReplace);
                bDecryptedFailure = DM_TRUE;
            }
            else
            {
                snprintf(dbXmlReplace,sizeof(dbXmlReplace),DM_DECRYPT_TMP_FILE);    // change replace file to decrypted temp file
            }
        }

        pXmlDocument = !bDecryptedFailure ? xmlReadFile(dbXmlReplace, NULL, XML_PARSE_NOBLANKS) : NULL;
        if (pXmlDocument != NULL)
        {
            xmlFreeDoc(pXmlDocument);
            pXmlDocument = NULL;

            if (access(pDmHandler->pLoadPath, F_OK) == 0)
            {
                unlink(pDmHandler->pLoadPath);
            }

            if (g_DmCtrl.encryptFunc)
            {
                unlink(DM_DECRYPT_TMP_FILE);   // remove decrypted temp file
                snprintf(dbXmlReplace,sizeof(dbXmlReplace),"%s.replace",pDmHandler->pLoadPath); // restore replace file
            }

            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ rename %s to %s",dbXmlReplace,pDmHandler->pLoadPath);
            rename(dbXmlReplace, pDmHandler->pLoadPath);
        }
        else
        {
            if (access(dbXmlCorrupted,F_OK)==0)
            {
                unlink(dbXmlCorrupted);
            }

            if (g_DmCtrl.encryptFunc)
            {
                unlink(DM_DECRYPT_TMP_FILE);    // remove decrypted temp file
                snprintf(dbXmlReplace,sizeof(dbXmlReplace),"%s.replace",pDmHandler->pLoadPath);  // restore replace file
            }

            rename(dbXmlReplace, dbXmlCorrupted);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s file corrupted, we rename it to %s",dbXmlReplace,dbXmlCorrupted);
        }
    }

    snprintf(dbXmlReplace,sizeof(dbXmlReplace),"%s.replace",pDmHandler->pBackupPath);
    snprintf(dbXmlCorrupted,sizeof(dbXmlCorrupted),"%s.replace.corrupted",pDmHandler->pBackupPath);

    if (access(dbXmlReplace, F_OK) == 0)
    {
        xmlDocPtr pXmlDocument = NULL;

		bDecryptedFailure = DM_FALSE;    // if not encrypt mode, suppose decrypt is OK

        /* we should decrypt db files firstly */
        if (g_DmCtrl.encryptFunc)
        {
            if (g_DmCtrl.encryptFunc(DM_FALSE,dbXmlReplace,DM_DECRYPT_TMP_FILE)!=DM_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ decrypt %s failed... in DmRegisterPatternReq",dbXmlReplace);
                bDecryptedFailure = DM_TRUE;
            }
            else
            {
                snprintf(dbXmlReplace,sizeof(dbXmlReplace),DM_DECRYPT_TMP_FILE); // change replace file to decrypted temp file
            }
        }

        pXmlDocument = !bDecryptedFailure ? xmlReadFile(dbXmlReplace, NULL, XML_PARSE_NOBLANKS) : NULL;
        if (pXmlDocument != NULL)
        {
            xmlFreeDoc(pXmlDocument);
            pXmlDocument = NULL;
            if (access(pDmHandler->pBackupPath, F_OK) == 0)
            {
                unlink(pDmHandler->pBackupPath);
            }

            if (g_DmCtrl.encryptFunc)
            {
                unlink(DM_DECRYPT_TMP_FILE);    // remove decrypted temp file
                snprintf(dbXmlReplace,sizeof(dbXmlReplace),"%s.replace",pDmHandler->pBackupPath); // change replace file to decrypted temp file
            }

            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ rename %s to %s",dbXmlReplace,pDmHandler->pBackupPath);
            rename(dbXmlReplace, pDmHandler->pBackupPath);
        }
        else
        {
            if (access(dbXmlCorrupted,F_OK)==0)
            {
                unlink(dbXmlCorrupted);
            }

            if (g_DmCtrl.encryptFunc)
            {
                unlink(DM_DECRYPT_TMP_FILE);   // remove decrypted temp file
                snprintf(dbXmlReplace,sizeof(dbXmlReplace),"%s.replace",pDmHandler->pBackupPath); // change replace file to decrypted temp file
            }

            rename(dbXmlReplace, dbXmlCorrupted);
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s file corrupted, we rename it to %s",dbXmlReplace,dbXmlCorrupted);
        }
    }

	/*initial the xml document*/
    bDecryptedFailure = DM_FALSE;
    if (access(pDmHandler->pLoadPath, F_OK) == 0)
    {
        /* is loading-file ok? */

        /* we should decrypt db files if needed... */
        if (g_DmCtrl.encryptFunc)
        {
            if (g_DmCtrl.encryptFunc(DM_FALSE,pDmHandler->pLoadPath,pDmHandler->pucFilePath)!=DM_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ decrypt %s failed... in DmRegisterPatternReq",pDmHandler->pLoadPath);
                remove(pDmHandler->pucFilePath);
                bRet = DM_FALSE;
            }
            else if ((pDmHandler->pXmlDocument = xmlReadFile(pDmHandler->pucFilePath, NULL, XML_PARSE_NOBLANKS)) != NULL)
            {
                bLoadPathUsed = DM_TRUE;
                bRet = DM_TRUE;
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ decrypt %s to %s",pDmHandler->pLoadPath,pDmHandler->pucFilePath);
            }
            else
            {
                remove(pDmHandler->pucFilePath);
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s file corrupted, could not be decrypted...",pDmHandler->pLoadPath);
                bRet = DM_FALSE;
            }
        }
        else if ((pDmHandler->pXmlDocument = xmlReadFile(pDmHandler->pLoadPath, NULL, XML_PARSE_NOBLANKS)) != NULL)
        {
            xmlFreeDoc(pDmHandler->pXmlDocument);
            pDmHandler->pXmlDocument = NULL;

            bLoadPathUsed = DM_TRUE;

            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Copy %s to %s",pDmHandler->pLoadPath,pDmHandler->pucFilePath);

            /* copy loading-file to working-file */
            snprintf(dbLoadingCmd,sizeof(dbLoadingCmd),"cp -rf %s %s",pDmHandler->pLoadPath,pDmHandler->pucFilePath);
            system(dbLoadingCmd);
            sleep(1);

        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s file corrupted",pDmHandler->pLoadPath);
        }
    }

    if (bLoadPathUsed == DM_FALSE && access(pDmHandler->pBackupPath, F_OK) == 0)
    {
        /* is backup-file ok? */

        /* we should decrypt db files if needed... */
        if (g_DmCtrl.encryptFunc)
        {
            if (g_DmCtrl.encryptFunc(DM_FALSE,pDmHandler->pBackupPath,pDmHandler->pucFilePath)!=DM_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ decrypt %s failed... in DmRegisterPatternReq",pDmHandler->pBackupPath);
                remove(pDmHandler->pucFilePath);
                bRet = DM_FALSE;
            }
            else if ((pDmHandler->pXmlDocument = xmlReadFile(pDmHandler->pucFilePath, NULL, XML_PARSE_NOBLANKS)) != NULL)
            {
                bRet = DM_TRUE;
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ decrypt %s to %s",pDmHandler->pBackupPath,pDmHandler->pucFilePath);
            }
            else
            {
                remove(pDmHandler->pucFilePath);
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s file corrupted, could not be decrypted...",pDmHandler->pBackupPath);
                bRet = DM_FALSE;
            }
        }
        else if ((pDmHandler->pXmlDocument = xmlReadFile(pDmHandler->pBackupPath, NULL, XML_PARSE_NOBLANKS)) != NULL)
        {
            xmlFreeDoc(pDmHandler->pXmlDocument);
            pDmHandler->pXmlDocument = NULL;

            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Copy %s to %s",pDmHandler->pBackupPath,pDmHandler->pucFilePath);

            /* copy backup-file to working-file */
            snprintf(dbLoadingCmd,sizeof(dbLoadingCmd),"cp -rf %s %s",pDmHandler->pBackupPath,pDmHandler->pucFilePath);
            system(dbLoadingCmd);
            sleep(1);

        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s file corrupted",pDmHandler->pBackupPath);
        }

    }

	/*if xml file exist, load it.*/
	if(access(pDmHandler->pucFilePath, F_OK) == 0)
	{
        pDmHandler->pXmlDocument = xmlReadFile(pDmHandler->pucFilePath, NULL, XML_PARSE_NOBLANKS);

		if(NULL != pDmHandler->pXmlDocument)
		{
            bRet = DM_TRUE;
			bCreateNewXmlFile = DM_FALSE;     /* A new xml will be not created since load it successfully */
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmRegisterPatternReq parser file %s for db %s", pDmHandler->pucFilePath, pDmHandler->pucDbName);
		}
		else
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmRegisterPatternReq error parser file %s for db %s", pDmHandler->pucFilePath, pDmHandler->pucDbName);

			if (remove(pDmHandler->pucFilePath) != 0)
			{
				bCreateNewXmlFile = DM_FALSE;     /* A new xml will be not created if it cann't be loaded */
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Failed to remove the corrupted XML file %s for db %s (%d, %s)!!",
						pDmHandler->pucFilePath, pDmHandler->pucDbName, errno, strerror(errno) );
				return DM_FALSE;
			}
			else
			{
				sync();
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ The database file is corrupted! Delete it!");
			}
		}
	}

    /* loaded failed or no file to load, we create new one here. */
	if (bCreateNewXmlFile == DM_TRUE)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ create new file %s for db %s", pDmHandler->pucFilePath, pDmHandler->pucDbName);

		pDmHandler->pXmlDocument = xmlNewDoc(BAD_CAST"1.0");

	   	if(DM_SUCCESS != DmXmlFileGenerateReq((INT8 *)(pDmHandler->pucDbName)))
	   	{
			DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ error create xml file failure: %s\n", pDmHandler->pucDbName);
			bRet = DM_FALSE;
		}
	   	else
	   	{
			bRet = DM_TRUE;

			if(DmSyncDBFileReq(pDmHandler)==DM_SUCCESS)
			{
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ sync %s to %s and %s succssfully after DmXmlFileGenerateReq()",pDmHandler->pucFilePath,pDmHandler->pLoadPath,pDmHandler->pBackupPath);
			}
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ sync %s to %s and %s failed after DmXmlFileGenerateReq()",pDmHandler->pucFilePath,pDmHandler->pLoadPath,pDmHandler->pBackupPath);
            }
	   	}
	}
	else
	{
		remove(pDmHandler->pucFilePath);    /* remove temp loade xml file */
	}

    /* here we reset these params to defaut values */
    if (bootResetCnt > 0)
    {
        if (DmResetParams(pPara->pucRootName, bootResetCnt, bootResetArray)==DM_TRUE)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ reset %d params to default values for db %s", bootResetCnt, pDmHandler->pucDbName);

			if(DmSyncDBFileReq(pDmHandler)==DM_SUCCESS)
			{
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ sync %s to %s and %s succssfully after DmResetParams()",pDmHandler->pucFilePath,pDmHandler->pLoadPath,pDmHandler->pBackupPath);
			}
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ sync %s to %s and %s failed after DmResetParams()",pDmHandler->pucFilePath,pDmHandler->pLoadPath,pDmHandler->pBackupPath);
            }
        }
        else
        {
            bRet = DM_FALSE;
        }

        DM_FREE(bootResetArray);
    }


	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmRegisterPatternReq");	

    return bRet;
}

INT32 DmGetXpathExpression(INT8 *pFullPath, INT8 *pXpath)
{
    INT8 *pucTmpPath = pXpath;
	INT8 *pucOrgName = NULL;
	INT8 *pucTmpName = NULL;
	INT8 *pucToken = NULL;

	if(NULL == pFullPath || NULL == pXpath)
	{
	    return DM_FALSE;
	}

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DmGetXpathExpression %s", pFullPath);

    pucOrgName = strdup(pFullPath);

	pucToken = strtok_r(pucOrgName, ".", (char**)&pucTmpName);

    while(pucToken != NULL)
    {
        if(0 == strcmp(pucToken,"{i}"))
		{
            strcat(pXpath, "/*");
		}
        else
        {
            strcat(pXpath, "/");
            strcat(pXpath, pucToken);
            
        }
        pucToken = strtok_r(NULL, ".", (char**)(&pucTmpName));
    }

	DM_FREE(pucOrgName);

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DmGetXpathExpression %s", pucTmpPath);

    return DM_TRUE;
}

BOOL8 DmResetParams(INT8 *pRootName, INT32 resetCnt, INT32 *resetArray)
{
    DM_HANDLER_T *pDmHandler = NULL;
	DM_NODE_INFO_T *pPattern = NULL;
    INT32 cnt = 0;
    INT32 cntInner = 0;
    INT32 lIndex = 0;
    INT8 *pXpathExp = NULL;
    xmlXPathContextPtr	xpathCtx = NULL; 
	xmlXPathObjectPtr	xpathObj = NULL; 
	xmlNodeSetPtr		pXpathNode = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmResetParams rootname=%s, reset-cnt=%d",pRootName,resetCnt);

    /*get dm handler*/
	pDmHandler = GetDmHandlerByName(pRootName);

	if(NULL == pDmHandler)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to get dm-handler for %s in DmResetParams",pRootName);
	    return DM_FALSE;
	}

    pPattern = pDmHandler->pPattern;

    pXpathExp = (INT8 *)DM_MALLOC(sizeof(INT8)*MAX_XML_PATH);
    
    if (pXpathExp == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DmResetParams : pXpathExp = NULL");
        return DM_FALSE;
    }

    for (cnt=0; cnt<resetCnt; cnt++)
    {
        lIndex = resetArray[cnt];

        if (lIndex <= 0)
        {
            continue;
        }

        osa_mem_set(pXpathExp, 0, sizeof(INT8)*MAX_XML_PATH);

        if (DmIsMultipleObject(pPattern, lIndex) == DM_TRUE 
            && pPattern[lIndex].pBootAction != NULL 
            && strcmp(pPattern[lIndex].pBootAction,DM_BOOT_ACTION_MULTI_OBJ__DEL_ALL) == 0)
        {
            /* multi-object : all obj-instances will be deleted */
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ boot-action %s for %s",DM_BOOT_ACTION_MULTI_OBJ__DEL_ALL,pPattern[lIndex].pucNodeName);
            {
                DmGetXpathExpression(pPattern[lIndex].pucNodeName, pXpathExp);
            	xpathCtx = xmlXPathNewContext(pDmHandler->pXmlDocument);
            	xpathObj = xmlXPathEvalExpression((const xmlChar *)pXpathExp, xpathCtx);

                if (xpathObj != NULL && !xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
                {
                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ pXpathExp = %s for %s, xpathObj found",pXpathExp,pPattern[lIndex].pucNodeName);

            	    pXpathNode = xpathObj->nodesetval;
            		for(cntInner = 0; cntInner < pXpathNode->nodeNr; cntInner++)
            		{
                        xmlNodePtr commentPtr = NULL;

                        /* delete its value-range comment */
                        commentPtr = xmlPreviousElementSibling(pXpathNode->nodeTab[cntInner]);
                        if (commentPtr != NULL && commentPtr->type == XML_COMMENT_NODE)
                        {
                            xmlUnlinkNode(commentPtr);
                			xmlFreeNode(commentPtr);
                        }

                        /* delete its data-type comment */
                        commentPtr = xmlPreviousElementSibling(pXpathNode->nodeTab[cntInner]);
                        if (commentPtr != NULL && commentPtr->type == XML_COMMENT_NODE)
                        {
                            xmlUnlinkNode(commentPtr);
                			xmlFreeNode(commentPtr);
                        }

                        /* delete its description comment */
                        commentPtr = xmlPreviousElementSibling(pXpathNode->nodeTab[cntInner]);
                        if (commentPtr != NULL && commentPtr->type == XML_COMMENT_NODE)
                        {
                            xmlUnlinkNode(commentPtr);
                			xmlFreeNode(commentPtr);
                        }

                        /* delete the node */
            		    xmlUnlinkNode(pXpathNode->nodeTab[cntInner]);
            			xmlFreeNode(pXpathNode->nodeTab[cntInner]);
            		}

                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ pXpathExp = %s for %s, xpathObj not found",pXpathExp,pPattern[lIndex].pucNodeName);
                }

                if(NULL != xpathObj)
                {
                    xmlXPathFreeObject(xpathObj);
                }
                xmlXPathFreeContext(xpathCtx);

            }
        }
        else if (pDmHandler->pDbIsObjectFunc(pPattern[lIndex].ucParaCType)==DM_FALSE
            && pPattern[lIndex].pBootAction != NULL)
        {
            /* it is item, and boot action is not null */
            INT8 *pFullName = (INT8*)DM_MALLOC(strlen(pPattern[pPattern[lIndex].ulParentIdx].pucNodeName)+strlen(pPattern[lIndex].pucNodeName)+1);

            if (pFullName == NULL)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ pFullName == NULL for %s%s in DmResetParams",pPattern[pPattern[lIndex].ulParentIdx].pucNodeName,pPattern[lIndex].pucNodeName);
                return DM_FALSE;
            }

            osa_mem_set(pFullName,0,strlen(pPattern[pPattern[lIndex].ulParentIdx].pucNodeName)+strlen(pPattern[lIndex].pucNodeName)+1);

            snprintf(pFullName,strlen(pPattern[pPattern[lIndex].ulParentIdx].pucNodeName)+strlen(pPattern[lIndex].pucNodeName)+1,"%s%s",pPattern[pPattern[lIndex].ulParentIdx].pucNodeName,pPattern[lIndex].pucNodeName);

            DmGetXpathExpression(pFullName, pXpathExp);
        	xpathCtx = xmlXPathNewContext(pDmHandler->pXmlDocument);
        	xpathObj = xmlXPathEvalExpression((const xmlChar *)pXpathExp, xpathCtx);

            if (xpathObj != NULL && !xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
            {
                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ pXpathExp = %s for %s, xpathObj found",pXpathExp,pFullName);

                if (strcmp(pPattern[lIndex].pBootAction,DM_BOOT_ACTION_BOOLEAN__SET_FALSE) == 0)
                {
                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ boot-action %s for %s",DM_BOOT_ACTION_BOOLEAN__SET_FALSE,pFullName);

                    pXpathNode = xpathObj->nodesetval;
            		for(cntInner = 0; cntInner < pXpathNode->nodeNr; cntInner++)
            		{
                        INT8 *pcOrgValue = NULL;

            			pcOrgValue = (INT8 *)xmlNodeGetContent(pXpathNode->nodeTab[cntInner]);

            			if(0 != strcmp(pcOrgValue,(INT8 *)DM_STR__BOOLEAN_FALSE))
            			{
            				xmlNodeSetContent(pXpathNode->nodeTab[cntInner], (const xmlChar *)(DM_STR__BOOLEAN_FALSE));
            			}

                        DM_FREE(pcOrgValue);
            		}
                }
                else if (strcmp(pPattern[lIndex].pBootAction,DM_BOOT_ACTION_BOOLEAN__SET_TRUE) == 0)
                {
                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ boot-action %s for %s",DM_BOOT_ACTION_BOOLEAN__SET_TRUE,pFullName);

                    pXpathNode = xpathObj->nodesetval;
            		for(cntInner = 0; cntInner < pXpathNode->nodeNr; cntInner++)
            		{
                        INT8 *pcOrgValue = NULL;

            			pcOrgValue = (INT8 *)xmlNodeGetContent(pXpathNode->nodeTab[cntInner]);

            			if(0 != strcmp(pcOrgValue,(INT8 *)DM_STR__BOOLEAN_TRUE))
            			{
            				xmlNodeSetContent(pXpathNode->nodeTab[cntInner], (const xmlChar *)(DM_STR__BOOLEAN_TRUE));
            			}

                        DM_FREE(pcOrgValue);
            		}
                }
                else if (strcmp(pPattern[lIndex].pBootAction,DM_BOOT_ACTION_INTEGER__SET_ZERO) == 0)
                {
                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ boot-action %s for %s",DM_BOOT_ACTION_INTEGER__SET_ZERO,pFullName);

                    pXpathNode = xpathObj->nodesetval;
            		for(cntInner = 0; cntInner < pXpathNode->nodeNr; cntInner++)
            		{
                        INT8 *pcOrgValue = NULL;

            			pcOrgValue = (INT8 *)xmlNodeGetContent(pXpathNode->nodeTab[cntInner]);

            			if(0 != strcmp(pcOrgValue,(INT8 *)DM_STR__INTEGER_ZERO))
            			{
            				xmlNodeSetContent(pXpathNode->nodeTab[cntInner], (const xmlChar *)(DM_STR__INTEGER_ZERO));
            			}

                        DM_FREE(pcOrgValue);
            		}
                }
                else if (strcmp(pPattern[lIndex].pBootAction,DM_BOOT_ACTION_STRING__SET_EMPTY) == 0)
                {
                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ boot-action %s for %s",DM_BOOT_ACTION_STRING__SET_EMPTY,pFullName);

                    pXpathNode = xpathObj->nodesetval;
            		for(cntInner = 0; cntInner < pXpathNode->nodeNr; cntInner++)
            		{
                        INT8 *pcOrgValue = NULL;

            			pcOrgValue = (INT8 *)xmlNodeGetContent(pXpathNode->nodeTab[cntInner]);

            			if(0 != strcmp(pcOrgValue,(INT8 *)DM_STR__STRING_EMPTY))
            			{
            				xmlNodeSetContent(pXpathNode->nodeTab[cntInner], (const xmlChar *)(DM_STR__STRING_EMPTY));
            			}

                        DM_FREE(pcOrgValue);
            		}
                }
                else if (strcmp(pPattern[lIndex].pBootAction,DM_BOOT_ACTION_STRING__SET_ENUM_MIN) == 0)
                {
                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ boot-action %s for %s",DM_BOOT_ACTION_STRING__SET_ENUM_MIN,pFullName);

                    INT32 stringCnt = 0;
                    INT32 structCnt = 1;
                    INT32 structValue = 0;
                    INT8 *pEnumMinStr = (INT8*)DM_MALLOC(sizeof(INT8)*MAX_SET_STRING_VALUE_LEN);

                    osa_mem_set(pEnumMinStr,0,sizeof(INT8)*MAX_SET_STRING_VALUE_LEN);

                    pPattern[lIndex].pToStringFunc(structCnt,&structValue,&stringCnt,pEnumMinStr,pPattern[lIndex].pValueRange);

                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ get min-enum-string %s for %s",pEnumMinStr,pFullName);

                    pXpathNode = xpathObj->nodesetval;
            		for(cntInner = 0; cntInner < pXpathNode->nodeNr; cntInner++)
            		{
                        INT8 *pcOrgValue = NULL;

            			pcOrgValue = (INT8 *)xmlNodeGetContent(pXpathNode->nodeTab[cntInner]);

            			if(0 != strcmp(pcOrgValue,(INT8 *)pEnumMinStr))
            			{
            				xmlNodeSetContent(pXpathNode->nodeTab[cntInner], (const xmlChar *)(pEnumMinStr));
            			}

                        DM_FREE(pcOrgValue);
            		}

                    DM_FREE(pEnumMinStr);
                }
                else if (strcmp(pPattern[lIndex].pBootAction,DM_BOOT_ACTION_DATETIME__UNKNOWN) == 0)
                {
                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ boot-action %s for %s",DM_BOOT_ACTION_DATETIME__UNKNOWN,pFullName);

                    pXpathNode = xpathObj->nodesetval;
            		for(cntInner = 0; cntInner < pXpathNode->nodeNr; cntInner++)
            		{
                        INT8 *pcOrgValue = NULL;

            			pcOrgValue = (INT8 *)xmlNodeGetContent(pXpathNode->nodeTab[cntInner]);

            			if(0 != strcmp(pcOrgValue,(INT8 *)DM_STR__DATETIME_UNKNOWN))
            			{
            				xmlNodeSetContent(pXpathNode->nodeTab[cntInner], (const xmlChar *)(DM_STR__DATETIME_UNKNOWN));
            			}

                        DM_FREE(pcOrgValue);
            		}
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ unknown boot-action %s for %s",pPattern[lIndex].pBootAction,pFullName);
                }
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ pXpathExp = %s for %s, xpathObj not found",pXpathExp,pFullName);
            }

            if(NULL != xpathObj)
            {
                xmlXPathFreeObject(xpathObj);
            }
            xmlXPathFreeContext(xpathCtx);

            DM_FREE(pFullName);

        }

    }

    DM_FREE(pXpathExp);
    
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmResetParams rootname=%s, reset-cnt=%d",pRootName,resetCnt);

    return DM_TRUE;
}

INT32 DmConvertEntityNameToEntityId(DM_HANDLER_T *pDmHandler, INT8 *pEntityName)
{
    INT32 entityId = 0;
    DB_ACCESS_NODE_T *pAccessNode = pDmHandler->pAccessList;

    if (pEntityName == NULL || strlen(pEntityName) <= 0)
    {
        return entityId;
    }

    while (pAccessNode != NULL && pAccessNode->AccessUserName != NULL)
    {
        if(strcmp(pAccessNode->AccessUserName, pEntityName) == 0)
        {
            entityId = pAccessNode->AccessUserId;
            break;
        }

        pAccessNode++;
    }

    return entityId;
}

BOOL8 DmCheckAccessPath(DM_TRANSACTION_REQ_T *pTransaction, DM_HANDLER_T *pDmHandler, UINT32 ulParaId)
{
    BOOL8 bRet = DM_FALSE;
    INT32 lTopId = -1;
    INT32 lChildId = ulParaId;

    lTopId = DmGetIDFromPath(&(pDmHandler->hashNameID), pTransaction->RootNodeToLock);

    while (lTopId != -1 && lChildId != -1)
    {
        if (lChildId == lTopId)
        {
            bRet = DM_TRUE;
            break;
        }
        else
        {
            lChildId = pDmHandler->pPattern[lChildId].ulParentIdx;
        }
    }

    if (bRet == DM_FALSE)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ beyond sub-path, ulParaId=0x%x, lTopId=0x%x, RootNodeToLock=%s in DmCheckAccessPath",ulParaId,lTopId,pTransaction->RootNodeToLock);
    }

    return bRet;
}

BOOL8 DmCheckRight(UINT32 lTransactionID, DM_HANDLER_T *pDmHandler, INT32 lRightType, UINT32 ulParaId)
{
    DM_TRANSACTION_INFO_T *pTransInfo = NULL;
    DM_TRANSACTION_REQ_T *pTransaction = NULL;
    UINT32 ulRightBitMap = 0x00;
    INT32 lEntityID = 0;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmCheckRight transID:%u, T:%d, ID:%d", lTransactionID, lRightType, ulParaId);	

	if(0 >= lTransactionID)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid transaction id(%d), leave DmCheckRight",lTransactionID);
		return DM_FALSE;
	}

    /* get the transaction according to transaction id */
    pTransInfo = DmGetTransactionInfo(pDmHandler, lTransactionID);

    if (pTransInfo == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find the transaction(%d) info, leave DmCheckRight",lTransactionID);
        return DM_FALSE;
    }

    pTransaction = &pTransInfo->transReq;

    if (DmCheckAccessPath(pTransaction, pDmHandler, ulParaId) == DM_FALSE)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ parameters (0x%x %s) beyond the locked sub-tree (%s), access denied, leave DmCheckRight",ulParaId,pDmHandler->pPattern[ulParaId].pucNodeName,pTransaction->RootNodeToLock);
        return DM_FALSE;
    }

    lEntityID = DmConvertEntityNameToEntityId(pDmHandler, pTransaction->AccessEntity);

    if (lEntityID == 0xffff)
    {
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ root rights, leave DmCheckRight TRUE");
        return DM_TRUE;
    }
    else if (lEntityID <= 0)
    {
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ invalid entity(%d), leave DmCheckRight FALSE",lEntityID);
        return DM_FALSE;
    }

    if(lRightType == RIGHT_R)
	{
	    ulRightBitMap = pDmHandler->pPattern[ulParaId].ulRead;
	}
	else
	{
	    ulRightBitMap = pDmHandler->pPattern[ulParaId].ulWrite;
	}
	
	if((ulRightBitMap & lEntityID))
	{
	    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmCheckRight TRUE");
	    return DM_TRUE;
	}
	else
	{

	    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ ulRightBitMap(0x%x) & lEntityID(0x%x) = 0, leave DmCheckRight FALSE",ulRightBitMap,lEntityID);
	    return DM_FALSE;
	}
}

void DmCheckTransferNeed(DM_HANDLER_T *pDmHandler, UINT32 ulParaId)
{
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmCheckTransferNeed");

    if(1 == pDmHandler->pPattern[ulParaId].bAutoSaved)
	{
		pDmHandler->bNeedBak = 1;
	}

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmCheckTransferNeed");
	return;
}

BOOL8 DmIsSingleOfMultiple(INT8 *pucPath)
{
    BOOL8 bRtn = DM_FALSE;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmIsSingleOfMultiple %d", bRtn);
	if(NULL == pucPath)
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmIsSingleOfMultiple %d", bRtn);
	    return bRtn;
	}
	INT32 lLen = strlen(pucPath);
	if(lLen > 2)
	{
	    if(pucPath[lLen-1] == '.')
    	{
            lLen -= 2;

            while(isdigit(pucPath[lLen]))
        	{
        		lLen--;
        	}

            if(pucPath[lLen] == '.')
            {
    	        bRtn = DM_TRUE;
            }
    	}
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmIsSingleOfMultiple %d", bRtn);
	return bRtn;
}
BOOL8 DmIsMultipleObject(DM_NODE_INFO_T *pPattern, UINT32 ulParaId)
{
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmIsMultipleObject %d", ulParaId);
	INT8 *pucTmp = pPattern[ulParaId].pucNodeName; 
	pucTmp = pucTmp + strlen(pPattern[ulParaId].pucNodeName) - 4;
    if(strcmp(pucTmp, "{i}.") == 0)
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmIsMultipleObject TRUE");    	
   	    return DM_TRUE;
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmIsMultipleObject FALSE");	
    return DM_FALSE;
}
#if 0
BOOL8 DmRegisterGetOperationReq(DM_REG_GET_OPERATION_REQ_T *pPara)
{
	DM_HANDLER_T *pDmHandler = NULL;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmRegisterGetOperationReq");

	if(NULL == pPara || NULL == pPara->pucPath || NULL == pPara->pFunc)
	{
	    return DM_FALSE;
	}
	/*get dm handler*/
	pDmHandler = GetDmHandlerByName(pPara->pucPath);
	if(NULL == pDmHandler)
	{
	    return DM_FALSE;
	}
	
	INT8 *pucTmp = DmChangeOrgNameToFullname(pPara->pucPath);
	DmHashAdd(&(pDmHandler->hashGetCHandler), pucTmp, pPara->pFunc);
	DM_FREE(pucTmp);
	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmRegisterGetOperationReq");	
	return DM_TRUE;
}

BOOL8 DmRegisterSetOperationReq(DM_REG_SET_OPERATION_REQ_T *pPara)
{
	DM_HANDLER_T *pDmHandler = NULL;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmRegisterSetOperationReq");

	if(NULL == pPara || NULL == pPara->pucPath || NULL == pPara->pFunc)
	{
	    return DM_FALSE;
	}
	/*get dm handler*/
	pDmHandler = GetDmHandlerByName(pPara->pucPath);
	if(NULL == pDmHandler)
	{
	    return DM_FALSE;
	}

	INT8 *pucTmp = DmChangeOrgNameToFullname(pPara->pucPath);
	DmHashAdd(&(pDmHandler->hashSetCHandler), pucTmp, pPara->pFunc);
	DM_FREE(pucTmp);
	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmRegisterSetOperationReq");	
	return DM_TRUE;
}

BOOL8 DmRegisterAddOperationReq(DM_REG_ADD_OPERATION_REQ_T *pPara)
{
	DM_HANDLER_T *pDmHandler = NULL;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmRegisterAddOperationReq");

	if(NULL == pPara || NULL == pPara->pucPath || NULL == pPara->pFunc)
	{
	    return DM_FALSE;
	}
	/*get dm handler*/
	pDmHandler = GetDmHandlerByName(pPara->pucPath);
	if(NULL == pDmHandler)
	{
	    return DM_FALSE;
	}

	INT8 *pucTmp = DmChangeOrgNameToFullname(pPara->pucPath);
	DmHashAdd(&(pDmHandler->hashAddCHandler), pucTmp, pPara->pFunc);
	DM_FREE(pucTmp);
	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmRegisterAddOperationReq");	
	return DM_TRUE;
}

BOOL8 DmRegisterDelOperationReq(DM_REG_DEL_OPERATION_REQ_T *pPara)
{
	DM_HANDLER_T *pDmHandler = NULL;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmRegisterDelOperationReq");

	if(NULL == pPara || NULL == pPara->pucPath || NULL == pPara->pFunc)
	{
	    return DM_FALSE;
	}
	/*get dm handler*/
	pDmHandler = GetDmHandlerByName(pPara->pucPath);
	if(NULL == pDmHandler)
	{
	    return DM_FALSE;
	}

	INT8 *pucTmp = DmChangeOrgNameToFullname(pPara->pucPath);
	DmHashAdd(&(pDmHandler->hashDelCHandler), pucTmp, pPara->pFunc);
	DM_FREE(pucTmp);

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmRegisterDelOperationReq");	
	return DM_TRUE;
}
#endif


INT8 * DmGetLastNodeNameC(INT8 *pucObjectPath)
{
	UINT32 i = strlen(pucObjectPath)-2;

	while('.' != pucObjectPath[i])
	{
		i--;
	}

	INT8 *pRet = strdup(&pucObjectPath[i+1]);
	pRet[strlen(pRet)-1] = 0;
	return pRet;
}


INT8 * DmGetLastNodeName(INT8 *pucObjectPath)
{
	UINT32 i = strlen(pucObjectPath)-2;
	INT8 *pRet = NULL;

	while('.' != pucObjectPath[i])
	{
		i--;
	}

	if(isdigit(pucObjectPath[i+1]))
	{
		pRet = strdup(&pucObjectPath[i]);
		pRet[0] = 'i';
	}
	else
	{
		pRet = strdup(&pucObjectPath[i+1]);
	}
	pRet[strlen(pRet)-1] = 0;
	return pRet;
}

INT32 DmGetParaCValue(DM_GET_PARA_C_VALUE_REQ_T *  pGetPara)
{
    DM_HANDLER_T *pDmHandler = NULL;

	INT32           lId      = 0;
	INT32           lRet     = DM_RET_INTERNAL_ERROR;
	xmlXPathObjectPtr   xpathObj = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmGetParaCValue");

	if(NULL == pGetPara)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pGetPara is null, leave DmGetParaCValue");
	    return lRet;
	}
	/*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pGetPara->pucPath);

    if (pDmHandler == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pDmHandler is NULL for %s in DmGetParaCValue",pGetPara->pucPath);
        return DM_RET_INVALID_PARAM_NAME;
    }

	lId = pGetPara->ulParaId;
    /*check right*/
    if(!DmCheckRight(pGetPara->TransactionId, pDmHandler, RIGHT_R, lId))
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmCheckRight failed in DmGetParaCValue");
	    return DM_RET_REQUEST_DENIED;
	}

	//add by yuqiangc for CSP 558510 20120814 begin
	if(NULL == pDmHandler->xpathCtx)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetParaCValue: pDmHandler->xpathCtx is NULL !");
	    return DM_RET_INTERNAL_ERROR;
	}
	//add by yuqiangc for CSP 558510 20120814 end

	INT8 *pNodeName = DmGetLastNodeNameC(pGetPara->pucPath);

	xmlNodePtr nodeBak = pDmHandler->xpathCtx->node;

	xpathObj = DbGetXPathObjPtr(pDmHandler->xpathCtx, pNodeName);

	if(NULL == xpathObj)
	{
		DM_FREE(pNodeName); //add by yuqiangc
		return DM_RET_SUCCESS;
	}

    if(DM_TRUE ==  DmGetCHandler(pGetPara))
	{
	    lRet = DM_RET_SUCCESS;
	}
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmGetCHandler(pGetPara) failed for %s in DmGetParaCValue",pGetPara->pucPath);
    }

	xmlXPathFreeObject(xpathObj);

	pDmHandler->xpathCtx->node = nodeBak;

	DM_FREE(pNodeName);

    if (lRet != DM_RET_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ lRet=%d, leave DmGetParaCValue",lRet);	
    }
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmGetParaCValue");	
    }

	return lRet;
}



/* Yuqiang Chen add */
INT32 DmSetParaCValue  ( DM_SET_PARA_C_VALUE_REQ_T *  pSetPara )  
{
    DM_HANDLER_T *pDmHandler = NULL;

	INT32           lId      = 0;
	INT32           lRet     = DM_RET_INTERNAL_ERROR;
	xmlXPathObjectPtr   xpathObj = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmSetParaCValueReq");

	if(NULL == pSetPara)
	{
	    return lRet;
	}
	
	/*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pSetPara->pucPath);

    if (pDmHandler == NULL)
    {
        return DM_RET_INVALID_PARAM_NAME;
    }

	lId = pSetPara->ulParaId;
    /*check right*/
    if(!DmCheckRight(pSetPara->TransactionId, pDmHandler, RIGHT_W, lId))
	{
	    return DM_RET_REQUEST_DENIED;
	}
    /*call the handler*/
	/* if multiple object */
	/* if object  */
	/* if item */

	INT8 *pNodeName = DmGetLastNodeNameC(pSetPara->pucPath);

	xmlNodePtr nodeBak = pDmHandler->xpathCtx->node;

	xpathObj = DbGetXPathObjPtr(pDmHandler->xpathCtx, pNodeName);

	if(NULL == xpathObj)
	{
		return DM_RET_SUCCESS;
	}

	if(DM_TRUE ==  DmSetCHandler(pSetPara))
	{
	    lRet = DM_RET_SUCCESS;
	}

	xmlXPathFreeObject(xpathObj);
	pDmHandler->xpathCtx->node = nodeBak;
	DM_FREE(pNodeName);

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmSetParaCValueReq");	

	return lRet;
}


INT32 DmAddObjectById( DM_ADD_OBJ_BY_ID_REQ_T *  pAddPara )
{
    DM_HANDLER_T *pDmHandler = NULL;

	INT32           lId      = 0;
	INT32			ucRet = DM_RET_SUCCESS;

	if(NULL == pAddPara || NULL == pAddPara->pucPath)
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmAddObjectById parameter error");
	    return DM_RET_INTERNAL_ERROR;
	}
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmAddObjectById %s", pAddPara->pucPath);
	}
	/*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pAddPara->pucPath);

    if (pDmHandler == NULL)
    {
        return DM_RET_INVALID_PARAM_NAME;
    }

	lId = pAddPara->ulObjId;
	
    /*call the handler*/
	/* if multiple object */
	/* if object  */
	/* if item */
	INT8  aucValue[MAX_INSTANCE_NUMBER_SIZE] = {0};
	DM_HANDLER_PARA_T DmHandlerPara = {0};

    osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

	DmHandlerPara.TransactionId = pAddPara->TransactionId;
	DmHandlerPara.ulParaId	  = pAddPara->ulObjId;
	DmHandlerPara.pDbHandler = pDmHandler;
	DmHandlerPara.pucName = pAddPara->pucPath;
	DmHandlerPara.bCInterface = DM_TRUE;
	DmHandlerPara.bItem = DM_FALSE;
	DmHandlerPara.ulCnt = 1;
	DmHandlerPara.pValue = aucValue;
    DmHandlerPara.valueSize = pAddPara->objectSize;
	

	if(DM_RET_SUCCESS != DmCallAddHandler(pDmHandler->pPattern[lId].pNodeHandler, &DmHandlerPara)) /**< call add handler */
	{	
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmAddObjectById call DmCallAddHandler failure for multiple obj %s", DmHandlerPara.pucName);
		return DM_RET_INTERNAL_ERROR;
	}

	
	INT8 *pucChildObjName = calloc(strlen((char*)(DmHandlerPara.pucName))+strlen((char*)(DmHandlerPara.pValue))+2, sizeof(UINT8));
    if(NULL == pucChildObjName)
    {
        return 0;
    }
	INT8 *pucTmp = pAddPara->pucPath;
	strcpy((char*)pucChildObjName, (char*)(DmHandlerPara.pucName));

	if(isdigit(aucValue[1]))
	{
		strcat(pucChildObjName,(char*)(aucValue+1));
		strcat(pucChildObjName,".");
		*((UINT32 *)pAddPara->pObject) = atoi(aucValue+1);	/**<  set instance number */
	}
	
	DmHandlerPara.pucName = pucChildObjName;


	xmlXPathContextPtr	xpathCtx = NULL;

	xpathCtx = xmlXPathNewContext(pDmHandler->pXmlDocument);
	if(NULL == xpathCtx)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler can't get xpathCtx");
		return DM_FALSE;
	}
//	INT8 	ucXPathName[MAX_XML_PATH] = {0};

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{

		return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	ConvertPathToXPath(pucChildObjName, ucXPathName);
	pDmHandler->xpathParentObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

	DM_FREE(ucXPathName);

	pAddPara->pucPath = pucChildObjName;
	
	if(DM_FALSE == DmAddCHandler(pAddPara))
	{
		ucRet = DM_RET_INTERNAL_ERROR;
	}

	pAddPara->pucPath = pucTmp;
	DM_FREE(pucChildObjName);


	if(NULL != xpathCtx)
	{
    	xmlXPathFreeObject(pDmHandler->xpathParentObj);
		xmlXPathFreeContext(xpathCtx);
		pDmHandler->xpathParentObj = NULL;
	}

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmAddObjectById");

    return ucRet;
}

BOOL8 DmCallOneNotifyHandler(DM_TRANSACTION_REQ_T *pTransaction, DM_HANDLER_T *pDmHandler , DM_NOTIFY_ID_LIST_T *pNotifyIdList, INT8 *pEntity)
{
	INT32                     lCnt = 0;
    DM_NOTIFY_ID_LIST_T      *pTmpNotifyIdPtr = pNotifyIdList;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmCallOneNotifyHandler id %d path %s", pNotifyIdList->lId, pNotifyIdList->notifyPara.pucPath);

	DM_NOTIFY_T *pNotifyPara = (DM_NOTIFY_T*)DM_MALLOC(sizeof(DM_NOTIFY_T));

	if(NULL == pNotifyPara)
	{
	    return DM_FALSE;
	}

    osa_mem_set(pNotifyPara,0,sizeof(DM_NOTIFY_T));

    /* write modifier */
    osa_mem_copy(pNotifyPara->Modifier, pTransaction->AccessEntity, sizeof(pNotifyPara->Modifier));

    if (pNotifyIdList->notifyPara.pucChangedAttrs!=NULL)
    {
        pNotifyPara->bAttrNotify = DM_TRUE;
    }

	/*get the count*/
    while(pTmpNotifyIdPtr != NULL)
	{
        if (pNotifyPara->bAttrNotify == DM_FALSE)
        {
    	    DM_NODE_NOTIFY_HANDLER_T *pNotifyHandler = pDmHandler->pPattern[pTmpNotifyIdPtr->lId].pNotifyHandler;

    		while(pNotifyHandler != NULL)
    		{
    		    /*the id should be notified to the pFunc*/
    		    if(strcmp(pNotifyHandler->entity, pEntity)==0)
    	    	{
                    lCnt ++;
    				break;
    	    	}

    		    pNotifyHandler = pNotifyHandler->pNext;
    		}
        }
        else
        {
            INT8 *pNotifyEntities = NULL;
            INT8 *pDefaultEntity = NULL;
            INT8 *pTmp = NULL;

            if (pDmHandler->pPattern[pNotifyIdList->lId].pXmlAttributeNotificationEntities)
            {
        		pNotifyEntities = strdup(pDmHandler->pPattern[pNotifyIdList->lId].pXmlAttributeNotificationEntities);
                pDefaultEntity = strtok_r(pNotifyEntities,",",&pTmp);
        		while(pDefaultEntity)
        		{
        			if(strcmp(pDefaultEntity, pEntity)==0)
        			{
        				lCnt ++;
        				break;
        			}

        			pDefaultEntity = strtok_r(NULL,",",&pTmp);
        		}

                DM_FREE(pNotifyEntities);
            }
        }
	    pTmpNotifyIdPtr = pTmpNotifyIdPtr->pNext;
	}

	if(lCnt == 0)
	{
	    DM_FREE(pNotifyPara);
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ no params need to be notified, leave DmCallOneNotifyHandler");	
		return DM_TRUE;
	}

	/*alloc the memory*/
	pNotifyPara->ulParaCount = lCnt;
	pNotifyPara->pCont = (DM_NOTIFY_PARA_T*)DM_MALLOC(lCnt*sizeof(DM_NOTIFY_PARA_T));

	if(NULL == pNotifyPara->pCont)
	{

		DM_FREE(pNotifyPara);
	    return DM_FALSE;
	}

	/*copy the parameter to the allocated memory*/
	lCnt = 0;
	pTmpNotifyIdPtr = pNotifyIdList;

	while(pTmpNotifyIdPtr != NULL)
	{
        if (pNotifyPara->bAttrNotify == DM_FALSE)
        {
    	    DM_NODE_NOTIFY_HANDLER_T *pNotifyHandler = pDmHandler->pPattern[pTmpNotifyIdPtr->lId].pNotifyHandler;

    		while(pNotifyHandler != NULL)
    		{
    		    /*the id should be notified to the pFunc*/
    		    if(strcmp(pNotifyHandler->entity, pEntity)==0)
    	    	{
    	    	    memcpy(&(pNotifyPara->pCont[lCnt]), &(pTmpNotifyIdPtr->notifyPara), sizeof(DM_NOTIFY_PARA_T));
    				pNotifyPara->pCont[lCnt].pucPath = strdup(pTmpNotifyIdPtr->notifyPara.pucPath);
    	    	    lCnt ++;
    				break;
    	    	}

    		    pNotifyHandler = pNotifyHandler->pNext;
    		}
        }
        else
        {
            INT8 *pNotifyEntities = NULL;
            INT8 *pDefaultEntity = NULL;
            INT8 *pTmp = NULL;

            if (pDmHandler->pPattern[pNotifyIdList->lId].pXmlAttributeNotificationEntities)
            {
        		pNotifyEntities = strdup(pDmHandler->pPattern[pNotifyIdList->lId].pXmlAttributeNotificationEntities);
                pDefaultEntity = strtok_r(pNotifyEntities,",",&pTmp);
        		while(pDefaultEntity)
        		{
        			if(strcmp(pDefaultEntity, pEntity)==0)
        			{
        				memcpy(&(pNotifyPara->pCont[lCnt]), &(pTmpNotifyIdPtr->notifyPara), sizeof(DM_NOTIFY_PARA_T));
        				pNotifyPara->pCont[lCnt].pucPath = strdup(pTmpNotifyIdPtr->notifyPara.pucPath);
        	    	    lCnt ++;
        				break;
        			}

        			pDefaultEntity = strtok_r(NULL,",",&pTmp);
        		}

                DM_FREE(pNotifyEntities);
            }
        }

	    pTmpNotifyIdPtr = pTmpNotifyIdPtr->pNext;
	}

	/*call function*/
	DmOneNotifyHandlerCalling(pEntity, pNotifyPara);

    /* free memory */
    for (lCnt=0;lCnt<pNotifyPara->ulParaCount;lCnt++)
    {
        DM_FREE(pNotifyPara->pCont[lCnt].pucPath);
    }
    DM_FREE(pNotifyPara->pCont);
	DM_FREE(pNotifyPara);

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmCallOneNotifyHandler");	
	return DM_TRUE;
}


BOOL8 DmCallNotifyHandler(DM_HANDLER_T *pDmHandler, DM_TRANSACTION_INFO_T *pTransInfo )
{
    DM_TRANSACTION_REQ_T *pTransaction = NULL;
	DM_NOTIFY_ID_LIST_T *pNotifyIdList = NULL;
    DM_NOTIFY_ID_LIST_T *pNotifyIdRootList = NULL;
	DM_HASH_T               calledFuncHash = {0};
    DM_NOTIFY_ID_LIST_T *pTmpNotifyIdList = NULL;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmCallNotifyHandler");	


	if(NULL == pTransInfo)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pTransInfo is null,  leave DmCallNotifyHandler");	
	    return DM_FALSE;
	}


    pNotifyIdList = pTransInfo->pNotifyIdList;
    pNotifyIdRootList = pNotifyIdList;

    pTransInfo->pNotifyIdList = NULL;

	if(NULL != pNotifyIdList)
	{
        pTransaction = &pTransInfo->transReq;

        if(DM_FALSE == pTransaction->bNotCallNotify)
        {
        	DmHashCreate(&calledFuncHash, DM_HASH_INIT_SIZE);

        	/*call all the notify function */	
        	while(pNotifyIdList != NULL)
        	{
        	    DM_NODE_NOTIFY_HANDLER_T *pNotifyHandler = NULL;
        		
        		pNotifyHandler = pDmHandler->pPattern[pNotifyIdList->lId].pNotifyHandler;
        		while(pNotifyHandler)
        		{
        		    /*if called, next*/
        			INT8 *pEntity = pNotifyHandler->entity;
        			if(DmHashGetValue(&calledFuncHash, pEntity) == NULL)
        			{
        				/*else call it, and set it to the hash*/
        				DmCallOneNotifyHandler(pTransaction, pDmHandler, pNotifyIdList, pNotifyHandler->entity);
        				DmHashAdd(&calledFuncHash,pEntity,(void* )1);
        			}

        			pNotifyHandler = pNotifyHandler->pNext;
        		}
        		pNotifyIdList = pNotifyIdList->pNext;
        	}
        	DmHashDestroy(&calledFuncHash, DM_FALSE);
        }

    	/*clear the list*/
    	pNotifyIdList = pNotifyIdRootList;
    	pTmpNotifyIdList = pNotifyIdList;
    	while(pNotifyIdList != NULL)
    	{
    	   pNotifyIdList = pNotifyIdList->pNext;
    	   DM_FREE(pTmpNotifyIdList->notifyPara.pucPath);
    	   DM_FREE(pTmpNotifyIdList);
    	   pTmpNotifyIdList = pNotifyIdList;
    	}
	}
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ value notify list is null in DmCallNotifyHandler");
	}

    pNotifyIdList = pTransInfo->pAttrNotifyIdList;
    pNotifyIdRootList = pNotifyIdList;

    pTransInfo->pAttrNotifyIdList = NULL;

	if(NULL != pNotifyIdList)
	{
        pTransaction = &pTransInfo->transReq;

        if(DM_FALSE == pTransaction->bNotCallNotify)
        {
        	DmHashCreate(&calledFuncHash, DM_HASH_INIT_SIZE);

        	/*call all the notify function */	
        	while(pNotifyIdList != NULL)
        	{
        	    INT8 *pNotifyEntities = NULL;
                INT8 *pEntity = NULL;
                INT8 *pTmp = NULL;

                if (pDmHandler->pPattern[pNotifyIdList->lId].pXmlAttributeNotificationEntities)
                {
            		pNotifyEntities = strdup(pDmHandler->pPattern[pNotifyIdList->lId].pXmlAttributeNotificationEntities);
                    pEntity = strtok_r(pNotifyEntities,",",&pTmp);
            		while(pEntity)
            		{
            			if(DmHashGetValue(&calledFuncHash, pEntity) == NULL)
            			{
            				DmCallOneNotifyHandler(pTransaction, pDmHandler, pNotifyIdList, pEntity);
            				DmHashAdd(&calledFuncHash,pEntity,(void* )1);
            			}

            			pEntity = strtok_r(NULL,",",&pTmp);
            		}

                    DM_FREE(pNotifyEntities);
                }
        		pNotifyIdList = pNotifyIdList->pNext;
        	}
        	DmHashDestroy(&calledFuncHash, DM_FALSE);
        }

    	/*clear the list*/
    	pNotifyIdList = pNotifyIdRootList;
    	pTmpNotifyIdList = pNotifyIdList;
    	while(pNotifyIdList != NULL)
    	{
    	   pNotifyIdList = pNotifyIdList->pNext;
    	   DM_FREE(pTmpNotifyIdList->notifyPara.pucPath);
           DM_FREE(pTmpNotifyIdList->notifyPara.pucChangedAttrs);
    	   DM_FREE(pTmpNotifyIdList);
    	   pTmpNotifyIdList = pNotifyIdList;
    	}
	}
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ attribute notify list is null in DmCallNotifyHandler");
	}

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmCallNotifyHandler");	
    return DM_TRUE;
}



INT32 DmGetInstanceNumFromPath(INT8 *pucPath)
{
    INT32 lRet = 0;
	INT8 *pucName = NULL;
	UINT32 i = 0;
	
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmGetInstanceNumFromPath");	
	if(NULL != pucPath)
	{
	    pucName = pucPath + strlen(pucPath)-2; /* jump the last '.' */

		/*get the alphabet*/
		while(isdigit(*pucName))
		{
		    pucName --;
			i++;
		}
		pucName++;
		
		INT8 pucInstanceNum[MAX_INSTANCE_NUMBER_SIZE] = {0};
		memcpy(pucInstanceNum,pucName,i);

		lRet = atoi(pucInstanceNum);
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmGetInstanceNumFromPath");	
    return lRet;
}
BOOL8 DmAddToNotifyList(DM_HANDLER_T *pDmHandler, UINT32 TransactionId, INT32 lId, INT8 *pucName, UINT8 *pucValue, INT32 lChgType)
{
    INT8 *pName = NULL;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmAddToNotifyList TransactionId %u, lId %d, name %s, type %d (0: item, 1: single-obj, 2: obj-add, 3: obj-del, 4: obj-chg, 5: attr-chg)", TransactionId, lId, pucName, lChgType);	

    if(NULL == pucName || lId < 0 || TransactionId <= 0)
   	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid arguements in DmAddToNotifyList");
   	    return DM_FALSE;
   	}
	
    /*add to the handler list*/
	DM_NOTIFY_ID_LIST_T *pTmpNotifyList = (DM_NOTIFY_ID_LIST_T*)DM_MALLOC(sizeof(DM_NOTIFY_ID_LIST_T));

	if(NULL == pTmpNotifyList)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ memory allocation failure for pTmpNotifyList in DmAddToNotifyList");
	    return DM_FALSE;
	}

    osa_mem_set(pTmpNotifyList,0,sizeof(DM_NOTIFY_ID_LIST_T));

    pName = DmFilterParaName(pDmHandler, lId, pucName);/* freed by access entity. */

	if(NULL == pName)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmFilterParaName failed in DmAddToNotifyList");
	    return DM_FALSE;
	}
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ add %s to %s notify list, TransactionId=%u", pName, pucValue?"attr":"value", TransactionId);
    }

    /*set content*/
	pTmpNotifyList->lId = lId;
	pTmpNotifyList->notifyPara.ulParaId = lId;
	pTmpNotifyList->notifyPara.pucPath = pName;
	pTmpNotifyList->notifyPara.ulParaNotifyType = lChgType;
    if (lChgType==DM_ATTRIBUTE_CHANGED)
    {
        pTmpNotifyList->notifyPara.pucChangedAttrs = (INT8 *)pucValue;
    }
	pTmpNotifyList->pNext = NULL;
	pTmpNotifyList->pPrev = NULL;

    osa_thread_mutex_lock(&(pDmHandler->mutexNotifyList));

    if(lChgType!=DM_ATTRIBUTE_CHANGED)
    {
    	if( pDmHandler->pNotifyIdList == NULL)
    	{
            pTmpNotifyList->pPrev = pTmpNotifyList;/* the header node should remember the last node  */
            pTmpNotifyList->pNext = NULL;/* NULL identify the last node */

    	    pDmHandler->pNotifyIdList = pTmpNotifyList;
    	}
    	else
    	{
            /* append the new notify-node to notify-list */
            DM_NOTIFY_ID_LIST_T *pLastNode = pDmHandler->pNotifyIdList->pPrev;/* the last node */

            pTmpNotifyList->pPrev = pLastNode;
    		pLastNode->pNext = pTmpNotifyList;

            /* the header node should always remember the last node  */
    		pDmHandler->pNotifyIdList->pPrev = pTmpNotifyList;
    	}
    }
    else
    {
        if( pDmHandler->pAttrNotifyIdList == NULL)
    	{
            pTmpNotifyList->pPrev = pTmpNotifyList;/* the header node should remember the last node  */
            pTmpNotifyList->pNext = NULL;/* NULL identify the last node */

    	    pDmHandler->pAttrNotifyIdList = pTmpNotifyList;
    	}
    	else
    	{
            /* append the new notify-node to notify-list */
            DM_NOTIFY_ID_LIST_T *pLastNode = pDmHandler->pAttrNotifyIdList->pPrev;/* the last node */

            pTmpNotifyList->pPrev = pLastNode;
    		pLastNode->pNext = pTmpNotifyList;

            /* the header node should always remember the last node  */
    		pDmHandler->pAttrNotifyIdList->pPrev = pTmpNotifyList;
    	}
    }
    osa_thread_mutex_unlock(&(pDmHandler->mutexNotifyList));

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmAddToNotifyList");

	return DM_TRUE;

}

BOOL8 DefaultGetBefHandler(DM_HANDLER_PARA_T *pGetPara)
{
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultGetBefHandler");
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultGetBefHandler");

    return DM_TRUE;
}

BOOL8 DefaultGetCurHandler(DM_HANDLER_PARA_T *pGetPara)
{

	UINT8 *pucOrgValue = NULL;
	UINT32  lStructCnt = 1;
	BOOL8  bRet = DM_FALSE;
	DM_HANDLER_T *pDmHandler = NULL;
	DM_NODE_INFO_T *pPatternNode = NULL;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultGetCurHandler");	
	if(NULL == pGetPara)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pGetPara = null, leave DefaultGetCurHandler");	
	    return bRet;
	}

    pDmHandler = pGetPara->pDbHandler;
    pPatternNode = pDmHandler->pPattern;

	/*if from string interface*/
	if(pGetPara->bCInterface != DM_TRUE)
	{   
		bRet = DbGetCurHandler(pGetPara);
		if(DM_FALSE == bRet)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ can't get value from xml as string-interface");			    
		}
		else
		{
			DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultGetCurHandler");	
		}
		return bRet;
	
	}
	else
	{
		// if from struct interface
		
		if(isObject(pDmHandler, pGetPara->ulParaId))
		{
		    DM_GET_PARA_C_VALUE_REQ_T VGetPara = {0};
			VGetPara.TransactionId = pGetPara->TransactionId;
			VGetPara.ulParaId  = pGetPara->ulParaId;
			VGetPara.pucPath  = pGetPara->pucName;
			VGetPara.pValue   = pGetPara->pValue;
            VGetPara.ValueSize = pGetPara->valueSize;

			if(pGetPara->pDbHandler->pPattern[pGetPara->ulParaId].pGetHandler != NULL)
			{
			     DM_GET_VALUE_T Para = {0};
				 Para.ulParaId = pGetPara->ulParaId;
				 Para.pucPath = pGetPara->pucName;
				 Para.pValue  = pGetPara->pValue;
				 Para.bCValue = DM_TRUE;
                 Para.ValueSize = pGetPara->valueSize;
			     DmGetHandlerCalling(pGetPara->pDbHandler->pPattern[pGetPara->ulParaId].pGetHandler,&Para);

			}

			if(DM_RET_SUCCESS == DmGetParaCValue(&VGetPara))
			{
				bRet = DM_TRUE;
			}
			else
			{
				bRet = DM_FALSE;
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ can't get value from xml as object c-struct");	
			}
				    
			pGetPara->ulCnt = VGetPara.ulCnt;
		}
		else if(pGetPara->bItem)
		{
			INT8 *pucStringValue = NULL;
            UINT32    orgValueSize = 0;

			pucStringValue = (INT8 *)DM_MALLOC(sizeof(INT8)*MAX_STRING_VALUE_LEN);

			if (pucStringValue == NULL)
			{
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DefaultGetCurHandler : pucStringValue = NULL");	

				return DM_FALSE;
			}

			memset(pucStringValue,0,sizeof(INT8)*MAX_STRING_VALUE_LEN);
			
		    /*judge the type and id to call the convert function*/
			pucOrgValue = pGetPara->pValue;
            orgValueSize = pGetPara->valueSize;
			pGetPara->pValue = pucStringValue;
            pGetPara->valueSize = sizeof(INT8)*MAX_STRING_VALUE_LEN;
			bRet = DbGetCurHandler(pGetPara);
            pGetPara->valueSize = orgValueSize;
			if(DM_FALSE == bRet)
			{
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ can't get value from xml as item c-struct");			    
			}
			else
			{

				pPatternNode[pGetPara->ulParaId].pToStructFunc(strlen(pucStringValue), pucStringValue, &lStructCnt, pucOrgValue, pPatternNode[pGetPara->ulParaId].pValueRange);

				pGetPara->pValue = pucOrgValue;
				pGetPara->ulCnt = lStructCnt;
			}

			DM_FREE(pucStringValue);
		    
		}
		else
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ impossible branch!!");
		}
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultGetCurHandler");	
    return bRet;
}

BOOL8 DefaultGetAftHandler(DM_HANDLER_PARA_T *pGetPara)
{
    BOOL8  bRet = DM_TRUE;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultGetAftHandler");	
	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultGetAftHandler");	
    return bRet;
}

BOOL8 DefaultSetBefHandler(DM_HANDLER_PARA_T *pSetPara)
{
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultSetBefHandler");
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultSetBefHandler");

    return DM_TRUE;
}

BOOL8 DefaultSetCurHandler(DM_HANDLER_PARA_T *pSetPara)
{
//    UINT8  pucStringValue[MAX_SET_STRING_VALUE_LEN] = {0};
	UINT8 *pucOrgValue = NULL;
	INT32  lStringCnt = 0;
	INT32  lStructCnt = 0;
	BOOL8  bRet = DM_FALSE;



	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultSetCurHandler");	
	if(NULL == pSetPara)
	{
	    return bRet;
	}

    lStructCnt = pSetPara->ulCnt;

	//if from string interface
	if(pSetPara->bCInterface != DM_TRUE)
	{
		//will not enter this brance
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ ERROR! Will not enter this brance");	
	}
	else
	{
		// if from struct interface	
	    /*judge the type and id to call the convert function*/
		if(isObject(pSetPara->pDbHandler, pSetPara->ulParaId))
		{
		    DM_SET_PARA_C_VALUE_REQ_T VSetPara = {0};
			VSetPara.TransactionId = pSetPara->TransactionId;
			VSetPara.ulParaId  = pSetPara->ulParaId;
			VSetPara.pucPath  = pSetPara->pucName;
			VSetPara.pValue   = pSetPara->pValue;
			VSetPara.ulCnt    = pSetPara->ulCnt;
            VSetPara.ValueSize = pSetPara->valueSize;

			if(DM_RET_SUCCESS == DmSetParaCValue(&VSetPara))
			{
				bRet = DM_TRUE;
			}
			else
			{
				bRet = DM_FALSE;
			}
		}
		else
		{
			INT8 *pucStringValue = NULL;
            UINT32          orgValueSize = 0;
			pucStringValue = (INT8 *)malloc(sizeof(INT8)*MAX_SET_STRING_VALUE_LEN);

			if (pucStringValue == NULL)
			{

				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DefaultSetCurHandler : pucStringValue = NULL");

				return DM_FALSE;
			}
			memset(pucStringValue,0,sizeof(INT8)*MAX_SET_STRING_VALUE_LEN);
		
		   	if(pSetPara->bCInterface)
		   	{
				pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pToStringFunc(lStructCnt, pSetPara->pValue, &lStringCnt, pucStringValue, pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pValueRange);
		   	}
			pucOrgValue = pSetPara->pValue;
            orgValueSize = pSetPara->valueSize;
			pSetPara->pValue = pucStringValue;

            DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ %s : strValue=%s in DefaultSetCurHandler", pSetPara->pucName,pucStringValue);

			bRet = DbSetCurHandlerForC(pSetPara);
			pSetPara->pValue = pucOrgValue;
            pSetPara->valueSize = orgValueSize;

			DM_FREE(pucStringValue);
		}
	}
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultSetCurHandler");
    return bRet;
}

BOOL8 DefaultSetAttrAftHandler(DM_HANDLER_PARA_T *pSetParaAttr)
{
    BOOL8  bRet = DM_FALSE;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultSetAttrAftHandler");	
	if(NULL == pSetParaAttr)
	{
	    return bRet;
	}

	INT8 *pNotifyHandler = pSetParaAttr->pDbHandler->pPattern[pSetParaAttr->ulParaId].pXmlAttributeNotificationEntities;

	if(NULL == pNotifyHandler)
	{
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultSetAttrAftHandler");	
		return DM_TRUE;
	}
	else
	{
	}

	if(DmIsMultipleObject(pSetParaAttr->pDbHandler->pPattern, pSetParaAttr->ulParaId))
	{
		bRet = DM_FALSE;
	}
	else if(isObject(pSetParaAttr->pDbHandler,pSetParaAttr->ulParaId))
	{
		bRet = DM_FALSE;
	}
	else if (pSetParaAttr->pDbHandler->pPattern[pSetParaAttr->ulParaId].pXmlAttributeNotificationEntities)
	{
        INT8 *pAttrNameList = NULL;
        INT32 bufsize = 0;
        INT32 cnt = 0;
        DM_ATTR_NODE_T *pAttrNodes = pSetParaAttr->pValue;

        /* count the buffer size */
        for (cnt=0; cnt<pSetParaAttr->ulCnt; cnt++)
        {
            bufsize += strlen(pAttrNodes[cnt].pAttrName)+1;
        }

        bufsize++;

        pAttrNameList = (INT8 *)DM_MALLOC(bufsize);/* freed when transaction stop */
        if (pAttrNameList==NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ memory allocated failed for pAttrNameList, in DefaultSetAttrAftHandler");	
        }
        else
        {
            osa_mem_set(pAttrNameList,0,bufsize);

            for (cnt=0; cnt<pSetParaAttr->ulCnt; cnt++)
            {
                snprintf(pAttrNameList+strlen(pAttrNameList),bufsize-strlen(pAttrNameList),"%s,",pAttrNodes[cnt].pAttrName);
            }
        }

	    bRet =  DmAddToNotifyList(pSetParaAttr->pDbHandler, pSetParaAttr->TransactionId, pSetParaAttr->ulParaId, pSetParaAttr->pucName, (UINT8 *)pAttrNameList, DM_ATTRIBUTE_CHANGED);
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultSetAttrAftHandler");	
    return bRet;
}

BOOL8 DefaultSetAftHandler(DM_HANDLER_PARA_T *pSetPara)
{
    BOOL8  bRet = DM_FALSE;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultSetAftHandler");	
	if(NULL == pSetPara)
	{
	    return bRet;
	}

	DM_NODE_NOTIFY_HANDLER_T *pNotifyHandler = NULL;
	
	pNotifyHandler = pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pNotifyHandler;

	if(NULL == pNotifyHandler)
	{
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultSetAftHandler");	
		return DM_TRUE;
	}
	else
	{
	}


	if(DmIsMultipleObject(pSetPara->pDbHandler->pPattern, pSetPara->ulParaId))
	{
		bRet =  DmAddToNotifyList(pSetPara->pDbHandler, pSetPara->TransactionId, pSetPara->ulParaId, pSetPara->pucName, NULL, DM_MULTI_OBJECT_CHANGED);
	}
	else if(isObject(pSetPara->pDbHandler,pSetPara->ulParaId))
	{
		bRet =  DmAddToNotifyList(pSetPara->pDbHandler, pSetPara->TransactionId, pSetPara->ulParaId, pSetPara->pucName, NULL, DM_SINGLE_OBJECT_CHANGED);
	}
	else
	{
	    bRet =  DmAddToNotifyList(pSetPara->pDbHandler, pSetPara->TransactionId, pSetPara->ulParaId, pSetPara->pucName, NULL, DM_ITEM_CHANGED);
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultSetAftHandler");	
    return bRet;
}

BOOL8 DefaultAddBefHandler(DM_HANDLER_PARA_T *pAddPara)
{
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultAddBefHandler");
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultAddBefHandler");

    return DM_TRUE;
}

BOOL8 DefaultAddCurHandler(DM_HANDLER_PARA_T *pAddPara)
{
//    UINT8  pucStringValue[MAX_STRING_VALUE_LEN] = {0};
	UINT8 *pucOrgValue = NULL;
	INT32  lStringCnt = 0;
	INT32  lStructCnt = 0;
	BOOL8  bRet = DM_FALSE;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultAddCurHandler");	
	if(NULL == pAddPara)
	{
	    return bRet;
	}

    lStructCnt = pAddPara->ulCnt;

	//if from string interface
	if(pAddPara->bCInterface != DM_TRUE)
	{
	    bRet = DbAddCurHandler(pAddPara);
	}
	else
	{
		// if from struct interface	
	    /*judge the type and id to call the convert function*/
		if(isObject(pAddPara->pDbHandler, pAddPara->ulParaId))
		{
		    DM_ADD_OBJ_BY_ID_REQ_T VAddPara = {0};
			VAddPara.TransactionId = pAddPara->TransactionId;
			VAddPara.ulObjId= pAddPara->ulParaId;
			VAddPara.pucPath  = pAddPara->pucName;
			VAddPara.pObject= pAddPara->pValue;
            VAddPara.objectSize = pAddPara->valueSize;

			if(DM_RET_SUCCESS == DmAddObjectById(&VAddPara))
			{
				bRet = DM_TRUE;
			}
			else
			{
				bRet = DM_FALSE;
			}
		}
		else
		{
			INT8 *pucStringValue = NULL;
			pucStringValue = (INT8 *)malloc(sizeof(INT8)*MAX_STRING_VALUE_LEN);

			if (pucStringValue == NULL)
			{

				DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DefaultAddCurHandler : pucStringValue = NULL");

				return DM_FALSE;
			}

			memset(pucStringValue,0,sizeof(INT8)*MAX_STRING_VALUE_LEN);

		
		    if(pAddPara->bCInterface)
		    {
				pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].pToStringFunc(lStructCnt, pAddPara->pValue, &lStringCnt, pucStringValue, pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].pValueRange);
		   	}
			pucOrgValue = pAddPara->pValue;
			pAddPara->pValue = pucStringValue;
			bRet = DbAddCurHandler(pAddPara);
			pAddPara->pValue = pucOrgValue;

			DM_FREE(pucStringValue);
		}
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultAddCurHandler");	
    return bRet;
}

BOOL8 DefaultAddAftHandler(DM_HANDLER_PARA_T *pAddPara)
{
    BOOL8  bRet = DM_FALSE;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultAddAftHandler");	
	if(NULL == pAddPara)
	{
	    return bRet;
	}

	DM_NODE_NOTIFY_HANDLER_T *pNotifyHandler = NULL;
	
	pNotifyHandler = pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].pNotifyHandler;

	if(DM_TRUE == DmIsMultipleObject(pAddPara->pDbHandler->pPattern,pAddPara->ulParaId))
	{
		INT8 *pucName = NULL;
        INT8 *pRealName = NULL;
        INT32 objNumber = 0;

        if(NULL != pNotifyHandler)
        {
            pucName = (INT8 *)DM_MALLOC(strlen(pAddPara->pucName)+strlen(pAddPara->pValue)+2);

    		if (pucName == NULL)
    		{
    			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DefaultAddAftHandler : pucName = NULL");
    			return DM_FALSE;
    		}

    		osa_mem_set(pucName,0,strlen(pAddPara->pucName)+strlen(pAddPara->pValue)+2);
    		strcpy(pucName,pAddPara->pucName);
    		strcat(pucName,(INT8 *)pAddPara->pValue);
    		strcat(pucName,".");
    		bRet = DmAddToNotifyList(pAddPara->pDbHandler, pAddPara->TransactionId, pAddPara->ulParaId, pucName, NULL, DM_MULTI_OBJECT_ADD);
    		DM_FREE(pucName);
        }
        else
        {
            bRet = DM_TRUE;
        }

        pRealName = DmFilterParaName(pAddPara->pDbHandler, pAddPara->ulParaId, pAddPara->pucName);/* freed by access entity. */

        if (pRealName == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DefaultAddAftHandler : pRealName = NULL for NumberOfEntries");
            return bRet;
        }

        objNumber = DbGetObjectNum(pAddPara->pDbHandler,pRealName);

        /* here we should update its NumberOfEntries */
        pucName = (INT8 *)DM_MALLOC(strlen(pRealName)+strlen("NumberOfEntries"));

		if (pucName == NULL)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DefaultAddAftHandler : pucName = NULL for NumberOfEntries");
			return bRet;
		}

		osa_mem_set(pucName,0,strlen(pRealName)+strlen("NumberOfEntries"));
		strcpy(pucName,pRealName);
        pucName[strlen(pucName)-1] = '\0';
        strcat(pucName,"NumberOfEntries");

        DmUpdateNumberOfEntries(pucName,objNumber,pAddPara->pDbHandler);
        DM_FREE(pucName);
        DM_FREE(pRealName);
	}
	else if(NULL != pNotifyHandler)
	{
		bRet = DmAddToNotifyList(pAddPara->pDbHandler, pAddPara->TransactionId, pAddPara->ulParaId, pAddPara->pucName, NULL, DM_MULTI_OBJECT_ADD);
	}
    else
    {
        bRet = DM_TRUE;
    }

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultAddAftHandler");	
    return bRet;
}

INT8 *DmFilterParaName(DM_HANDLER_T *pDmHandler, INT32 lId, INT8 *pucName)
{
    INT8 *pTmpParent = pDmHandler->pPattern[pDmHandler->pPattern[lId].ulParentIdx].pucNodeName;
    INT8 *pTmpName = pDmHandler->pPattern[lId].pucNodeName;
    INT8 *pFullName = NULL;
    INT8 *pToken = NULL;
    INT8 *pTmp = NULL;
    INT8 *pDup = NULL;
    INT8 *pFilteredName = NULL;
    INT8 *pDupToken = NULL;
    INT8 *pDupTmp = NULL;
    INT32 len = 0;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmFilterParaName %s",pucName);	

    if(DM_TRUE != isObject(pDmHandler, lId))
    {
        pFullName = (INT8 *)DM_MALLOC(strlen(pTmpParent)+strlen(pTmpName)+1);

        if (pFullName == NULL)
        {
            return NULL;
        }

        memset(pFullName,0,strlen(pTmpParent)+strlen(pTmpName)+1);

        strcpy(pFullName, pTmpParent);
        strcat(pFullName, pTmpName);
    }
    else
    {
        pFullName = strdup(pTmpName);
    }

    pFilteredName = (INT8 *)DM_MALLOC(MAX_XML_PATH);

    if (pFilteredName == NULL)
    {
        DM_FREE(pFullName);
        return NULL;
    }

    memset(pFilteredName,0,MAX_XML_PATH);

    pDup = (INT8 *)strdup(pucName);

    if (pFullName == NULL || pFilteredName == NULL || pDup == NULL)
    {
        DM_FREE(pFullName);
        DM_FREE(pFilteredName);
        DM_FREE(pDup);
        return NULL;
    }

    pToken = strtok_r(pFullName, ".", &pTmp);

    if (pToken == NULL)/* just in case ... */
    {
        DM_FREE(pFullName);
        DM_FREE(pFilteredName);
        DM_FREE(pDup);
        return NULL;
    }

    strtok_r(pDup, ".", &pDupTmp);/* skip header token */
    
    strcpy(pFilteredName, pToken);
    strcat(pFilteredName, ".");
    
    while((pToken = strtok_r(NULL, ".", &pTmp)) != NULL)
    {
        pDupToken = strtok_r(NULL, ".", &pDupTmp);
    
        if (strcmp(pToken, "{i}") == 0)
        {
            if (pDupToken != NULL)
            {
                if (*pDupToken == 'i')
                {
                    strcat(pFilteredName,pDupToken+1);/* skip i */
                }
                else
                {
                    strcat(pFilteredName,pDupToken);
                }

                strcat(pFilteredName,".");
            }
        }
        else
        {
            strcat(pFilteredName,pToken);
            strcat(pFilteredName,".");
        }
    }

    if(DM_TRUE != isObject(pDmHandler, lId))
    {
        len = strlen(pFilteredName);
        *(pFilteredName+len-1) = '\0';/* delete the last '.' */
    }

    DM_FREE(pFullName);
    DM_FREE(pDup);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmFilterParaName %s ",pFilteredName?pFilteredName:"null");	

    return pFilteredName;
}

BOOL8 DefaultDelBefHandler(DM_HANDLER_PARA_T *pDelPara)
{
    
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultDelBefHandler");	
	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultDelBefHandler");	
    return DM_TRUE;
}

BOOL8 DefaultDelCurHandler(DM_HANDLER_PARA_T *pDelPara)
{
    BOOL8 bRet = DM_FALSE;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultDelCurHandler");    
	if(NULL == pDelPara)
	{
	    return bRet;
	}
	//if from string interface
	// if from struct interface	
    bRet = DbDelCurHandler(pDelPara);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultDelCurHandler");	
    return bRet;
}

BOOL8 DefaultDelAftHandler(DM_HANDLER_PARA_T *pDelPara)
{
    BOOL8  bRet = DM_FALSE;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DefaultDelAftHandler");	
	if(NULL == pDelPara)
	{
	    return bRet;
	}

	DM_NODE_NOTIFY_HANDLER_T *pNotifyHandler = NULL;
	
	pNotifyHandler = pDelPara->pDbHandler->pPattern[pDelPara->ulParaId].pNotifyHandler;

	if(NULL != pNotifyHandler)
	{
        bRet =  DmAddToNotifyList(pDelPara->pDbHandler, pDelPara->TransactionId, pDelPara->ulParaId, pDelPara->pucName, NULL, DM_MULTI_OBJECT_DELETE);
	}
    else
    {
        bRet = DM_TRUE;
    }

    if (DM_TRUE == DmIsMultipleObject(pDelPara->pDbHandler->pPattern,pDelPara->ulParaId))
    {
        /* here we should update its NumberOfEntries */
        INT8 *pucName = NULL;
        INT8 *pRealName = NULL;
        INT32 objectNum = 0;

        pRealName = DmFilterParaName(pDelPara->pDbHandler, pDelPara->ulParaId, pDelPara->pucName);/* freed by access entity. */
        
        if (pRealName == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DefaultDelAftHandler : pRealName = NULL for NumberOfEntries");
            return bRet;
        }

        pucName = (INT8 *)DM_MALLOC(strlen(pRealName)+strlen("NumberOfEntries"));
        if (pucName == NULL)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DefaultDelAftHandler : pucName = NULL for NumberOfEntries");
            return bRet;
        }
        
        osa_mem_set(pucName,0,strlen(pRealName)+strlen("NumberOfEntries"));

        strcpy(pucName,pRealName);

        if (DM_TRUE == DmIsSingleOfMultiple(pucName))
        {
            INT8 *pTmp = NULL;
            pucName[strlen(pucName)-1] = '\0';
            pTmp = strrchr(pucName,'.');
            if (pTmp)
            {
                *(pTmp+1) = '\0';
                objectNum = DbGetObjectNum(pDelPara->pDbHandler,pucName);
                *pTmp = '\0';
            }
            else
            {
                /* it is impossible here */
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ it is impossible here in DefaultDelAftHandler");	
            }
        }
        else
        {
            objectNum = DbGetObjectNum(pDelPara->pDbHandler,pucName);

            pucName[strlen(pucName)-1] = '\0';
        }

        strcat(pucName,"NumberOfEntries");

        DmUpdateNumberOfEntries(pucName,objectNum,pDelPara->pDbHandler);

        DM_FREE(pRealName);
        DM_FREE(pucName);
    }

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DefaultDelAftHandler");	
    return bRet;
}

/* this function is used for auto-generated code */
UINT32 DmGetObjectLengthOfInstance(INT8 *pucPath)
{
	UINT32 ulLength = 0;

	if(NULL == pucPath)
	{
		return 0;
	}
	
	ulLength = strlen(pucPath);

	if(ulLength < 4)
	{
		return 0;
	}

	ulLength -= 3;
	
	while((ulLength > 0)&&('.' != pucPath[ulLength]))
	{
		ulLength--;
	}
	if('.' == pucPath[ulLength])
	{
		return (ulLength+1);
	}
	else
	{
		return 0;
	}
}

BOOL8 DmCallDefaultSetHandler(DM_HANDLER_PARA_T *pDmHandlerPara)
{
    BOOL8              bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultSetHandler pDmHandlerPara->pDbHandler='0x%x'", pDmHandlerPara->pDbHandler);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultSetHandler pDmHandlerPara->TransactionId='%d'", pDmHandlerPara->TransactionId);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultSetHandler pDmHandlerPara->ulParaId='%d'", pDmHandlerPara->ulParaId);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultSetHandler pDmHandlerPara->ulCnt='%d'", pDmHandlerPara->ulCnt);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultSetHandler pDmHandlerPara->bCInterface='%d'", pDmHandlerPara->bCInterface);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultSetHandler pDmHandlerPara->bItem='%d'", pDmHandlerPara->bItem);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultSetHandler pDmHandlerPara->pucName='%s'", pDmHandlerPara->pucName);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultSetHandler pDmHandlerPara->pValue='0x%x'", pDmHandlerPara->pValue);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultSetHandler pDmHandlerPara->valueSize='%d'", pDmHandlerPara->valueSize);

	bRtn = DefaultSetBefHandler(pDmHandlerPara);
	if (!bRtn)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultSetBefHandler failed!\n", pDmHandlerPara->pucName);
		DM_FREE(pDmHandlerPara->pucName);

		return DM_FALSE;
	}
	bRtn = DefaultSetCurHandler(pDmHandlerPara);
	if (!bRtn)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultSetCurHandler failed!\n", pDmHandlerPara->pucName);
		DM_FREE(pDmHandlerPara->pucName);

		return DM_FALSE;
	}

	if(2 == bRtn)
	{
		bRtn = DM_TRUE;	/* yuqiangc: this is the case that the value to be set is the same as that in database file */
		return bRtn;
	}
	
	bRtn = DefaultSetAftHandler(pDmHandlerPara);
	if (!bRtn)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultSetAftHandler failed!\n", pDmHandlerPara->pucName);
		DM_FREE(pDmHandlerPara->pucName);

		return DM_FALSE;
	}

	return bRtn;
}


BOOL8 DmCallDefaultGetHandler(DM_HANDLER_PARA_T *pDmHandlerPara)
{
    BOOL8              bRtn = DM_TRUE;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultGetHandler pDmHandlerPara->pDbHandler='0x%x'", pDmHandlerPara->pDbHandler);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultGetHandler pDmHandlerPara->TransactionId='%d'", pDmHandlerPara->TransactionId);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultGetHandler pDmHandlerPara->ulParaId='%d'", pDmHandlerPara->ulParaId);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultGetHandler pDmHandlerPara->ulCnt='%d'", pDmHandlerPara->ulCnt);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultGetHandler pDmHandlerPara->bCInterface='%d'", pDmHandlerPara->bCInterface);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultGetHandler pDmHandlerPara->bItem='%d'", pDmHandlerPara->bItem);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultGetHandler pDmHandlerPara->pucName='%s'", pDmHandlerPara->pucName);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultGetHandler pDmHandlerPara->pValue='0x%x'", pDmHandlerPara->pValue);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultGetHandler pDmHandlerPara->valueSize='%d'", pDmHandlerPara->valueSize);

	bRtn = DefaultGetBefHandler(pDmHandlerPara);
	if (!bRtn)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultGetBefHandler failed!\n", pDmHandlerPara->pucName);
		DM_FREE(pDmHandlerPara->pucName);

		return DM_FALSE;
	}
	bRtn = DefaultGetCurHandler(pDmHandlerPara);
	if (!bRtn)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultGetCurHandler failed!\n", pDmHandlerPara->pucName);
		DM_FREE(pDmHandlerPara->pucName);

		return DM_FALSE;
	}
	bRtn = DefaultGetAftHandler(pDmHandlerPara);
	if (!bRtn)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultGetAftHandler failed!\n", pDmHandlerPara->pucName);
		DM_FREE(pDmHandlerPara->pucName);

		return DM_FALSE;
	}

	return bRtn;
}

BOOL8 DmCallDefaultDelHandler(DM_HANDLER_PARA_T *pDmHandlerPara)
{
    BOOL8              bRtn = DM_TRUE;
	
    bRtn = DefaultDelBefHandler(pDmHandlerPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultDelBefHandler failed!\n", pDmHandlerPara->pucName);
        DM_FREE(pDmHandlerPara->pucName);

        return DM_FALSE;
    }
    bRtn = DefaultDelCurHandler(pDmHandlerPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultDelCurHandler failed!\n", pDmHandlerPara->pucName);
        DM_FREE(pDmHandlerPara->pucName);

        return DM_FALSE;
    }
    bRtn = DefaultDelAftHandler(pDmHandlerPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultDelAftHandler failed!\n", pDmHandlerPara->pucName);
        DM_FREE(pDmHandlerPara->pucName);

        return DM_FALSE;
    }

	return bRtn;
}

BOOL8 DmDataIntegerValidCheck(INT8 *pValueRange, INT8 *pValue, INT32 integerType)
{
	BOOL8 bRet = DM_FALSE;
    INT32 cnt = 0;
    INT32 length = 0;
    INT8 *pChar = NULL;
    INT8 *pDefaultRange = NULL;
    INT8 *integerDefaultRange[]=
    {
        "[-128:127]",                                   /* DM_INTEGER_TYPE__INT8 */
        "[-32768:32767]",                               /* DM_INTEGER_TYPE__INT16 */
        "[-2147483648:2147483647]",                     /* DM_INTEGER_TYPE__INT32 */
        "[-9223372036854775808:9223372036854775807]",   /* DM_INTEGER_TYPE__INT64 */
        "[0:255]",                                      /* DM_INTEGER_TYPE__UINT8 */
        "[0:65535]",                                    /* DM_INTEGER_TYPE__UINT16 */
        "[0:4294967295]",                               /* DM_INTEGER_TYPE__UINT32 */
        "[0:18446744073709551615]",                     /* DM_INTEGER_TYPE__UINT64 */
    };

	if (pValueRange == NULL || pValue == NULL || strlen(pValue) <= 0 ||
		integerType < 0 || integerType >= sizeof(integerDefaultRange)/sizeof(INT8 *))  /* Enable int type in default range table */
	{
		DM_WARNING("Invalid parameters(NULL/empty) in DmDataIntegerValidCheck()");
		return bRet;
	}

	/* ValueRange is "" or "[]", so any value is OK */
	if (strlen(pValueRange) <= 2)
	{
		/* we use default range to replace it */
        pValueRange = integerDefaultRange[integerType];
	}

    pDefaultRange = integerDefaultRange[integerType];

    if (pValueRange[0]!='[' || pValueRange[strlen(pValueRange)-1]!=']')
    {
        DM_WARNING("Invalid value range format in DmDataIntegerValidCheck()");
		return bRet;
    }

    length = strlen(pValue);

    pChar = pValue;

    /* check if any invalid chars found */
    for (cnt=0;cnt<length;cnt++)
    {
        if (isdigit(pChar[cnt])
            || ((cnt==0) && (length>1) && (pChar[cnt]=='-') && (integerType>=DM_INTEGER_TYPE__INT8) && (integerType<=DM_INTEGER_TYPE__INT64)))
        {
            continue;
        }
        else
        {
            DM_WARNING("Invalid chars found in value(%s), in DmDataIntegerValidCheck()",pValue);
            bRet = DM_FALSE;
            return bRet;
        }
    }

    /* the length should not be beyond the int64 or uint64 */
    if ((pChar[0]=='-' && strtoll(pValue,NULL,10)==LLONG_MAX && errno==ERANGE)
        || (pValue[0]!='-' && strtoull(pValue,NULL,10)==ULLONG_MAX && errno==ERANGE))
    {
        DM_WARNING("integer overflow in DmDataIntegerValidCheck()");
        bRet = DM_FALSE;
        return bRet;
    }

    /* check if in the value range */
    if ((integerType>=DM_INTEGER_TYPE__INT8) && (integerType<=DM_INTEGER_TYPE__INT64))
    {
        INT64 min_value = 0;
        INT64 min_default_value = 0;
        INT64 max_value = 0;
        INT64 max_default_value = 0;
        INT64 cur_value = 0;

        cur_value = strtoll(pValue,NULL,10);

        if (strchr(pValueRange,',')==NULL)
        {
            if (strchr(pValueRange,':')==NULL)
            {
                /* [xxx] range, only one value in the range */
                max_value = strtoll(pValueRange+1,NULL,10);
                if (cur_value==max_value)
                {
                    bRet = DM_TRUE;
                    return bRet;
                }
                else
                {
                    DM_WARNING("value not in the range([xxx]) in DmDataIntegerValidCheck()");
                    bRet = DM_FALSE;
                    return bRet;
                }
            }
            else
            {
                /* [xxx:xxx] range */
                INT8 *pTest = NULL;
                min_default_value = strtoll(pDefaultRange+1,NULL,10);
                max_default_value = strtoll(strchr(pDefaultRange,':')+1,NULL,10);
                if (*(pValueRange+1)==':')
                {
                    min_value = min_default_value;
                }
                else
                {
                    min_value = strtoll(pValueRange+1,NULL,10);
                }

                pTest = strchr(pValueRange,':');
                if (!pTest || *(pTest+1)==']')
                {
                    max_value = max_default_value;
                }
                else
                {
                    max_value = strtoll(pTest+1,NULL,10);
                }

                if (cur_value<=max_value && cur_value>=min_value)
                {
                    bRet = DM_TRUE;
                    return bRet;
                }
                else
                {
                    DM_WARNING("value not in the range([xxx:xxx]) in DmDataIntegerValidCheck()");
                    bRet = DM_FALSE;
                    return bRet;
                }
            }
        }
        else
        {
            /* [xxx,xxx:xxx,xxx] range */
            INT8 *pToken = NULL;
            INT8 *pTmp = NULL;
            INT8 *pRangeTmp = strdup(pValueRange);

            if (pRangeTmp==NULL)
            {
                DM_WARNING("malloc() failed in DmDataIntegerValidCheck()");
                bRet = DM_FALSE;
                return bRet;
            }

            pToken = strtok_r(pRangeTmp+1,",",&pTmp);

            bRet = DM_FALSE;
            while(pToken)
            {
                if (strchr(pToken,':')==NULL)
                {
                    /* xxx range, only one value in the range */
                    max_value = strtoll(pToken,NULL,10);
                    if (cur_value==max_value)
                    {
                        DM_FREE(pRangeTmp);
                        bRet = DM_TRUE;
                        return bRet;
                    }
                }
                else
                {
                    /* xxx:xxx range */
                    INT8 *pTest = NULL;
                    min_default_value = strtoll(pDefaultRange+1,NULL,10);
                    max_default_value = strtoll(strchr(pDefaultRange,':')+1,NULL,10);
                    if (*pToken==':')
                    {
                        min_value = min_default_value;
                    }
                    else
                    {
                        min_value = strtoll(pToken,NULL,10);
                    }

                    pTest = strchr(pToken,':');
                    if (!pTest || *(pTest+1)=='\0' || *(pTest+1)==']')
                    {
                        max_value = max_default_value;
                    }
                    else
                    {
                        max_value = strtoll(pTest+1,NULL,10);
                    }

                    if (cur_value<=max_value && cur_value>=min_value)
                    {
                        DM_FREE(pRangeTmp);
                        bRet = DM_TRUE;
                        return bRet;
                    }
                }

                pToken = strtok_r(NULL,",",&pTmp);
            }

            if (bRet == DM_FALSE)
            {
                DM_WARNING("value out of the signed range([xxx,xxx:xxx]) in DmDataIntegerValidCheck()");
            }

            DM_FREE(pRangeTmp);
        }
    }
    else if ((integerType>=DM_INTEGER_TYPE__UINT8) && (integerType<=DM_INTEGER_TYPE__UINT64))
    {
        UINT64 min_value = 0;
        UINT64 min_default_value = 0;
        UINT64 max_value = 0;
        UINT64 max_default_value = 0;
        UINT64 cur_value = 0;

        cur_value = strtoll(pValue,NULL,10);

        if (strchr(pValueRange,',')==NULL)
        {
            if (strchr(pValueRange,':')==NULL)
            {
                /* [xxx] range, only one value in the range */
                max_value = strtoull(pValueRange+1,NULL,10);
                if (cur_value==max_value)
                {
                    bRet = DM_TRUE;
                    return bRet;
                }
                else
                {
                    DM_WARNING("value not in the unsigned range([xxx]) in DmDataIntegerValidCheck()");
                    bRet = DM_FALSE;
                    return bRet;
                }
            }
            else
            {
                /* [xxx:xxx] range */
                INT8 *pTest = NULL;
                min_default_value = strtoull(pDefaultRange+1,NULL,10);
                max_default_value = strtoull(strchr(pDefaultRange,':')+1,NULL,10);
                if (*(pValueRange+1)==':')
                {
                    min_value = min_default_value;
                }
                else
                {
                    min_value = strtoull(pValueRange+1,NULL,10);
                }

                pTest = strchr(pValueRange,':');
                if (!pTest || *(pTest+1)==']')
                {
                    max_value = max_default_value;
                }
                else
                {
                    max_value = strtoull(pTest+1,NULL,10);
                }

                if (cur_value<=max_value && cur_value>=min_value)
                {
                    bRet = DM_TRUE;
                    return bRet;
                }
                else
                {
                    DM_WARNING("value not in the unsigned range([xxx:xxx]) in DmDataIntegerValidCheck()");
                    bRet = DM_FALSE;
                    return bRet;
                }
            }
        }
        else
        {
            /* [xxx,xxx:xxx,xxx] range */
            INT8 *pToken = NULL;
            INT8 *pTmp = NULL;
            INT8 *pRangeTmp = strdup(pValueRange);

            if (pRangeTmp==NULL)
            {
                DM_WARNING("malloc() failed in DmDataIntegerValidCheck()");
                bRet = DM_FALSE;
                return bRet;
            }

            pToken = strtok_r(pRangeTmp+1,",",&pTmp);

            bRet = DM_FALSE;
            while(pToken)
            {
                if (strchr(pToken,':')==NULL)
                {
                    /* xxx range, only one value in the range */
                    max_value = strtoull(pToken,NULL,10);
                    if (cur_value==max_value)
                    {
                        DM_FREE(pRangeTmp);
                        bRet = DM_TRUE;
                        break;
                    }
                }
                else
                {
                    /* xxx:xxx range */
                    INT8 *pTest = NULL;
                    min_default_value = strtoull(pDefaultRange+1,NULL,10);
                    max_default_value = strtoull(strchr(pDefaultRange,':')+1,NULL,10);
                    if (*pToken==':')
                    {
                        min_value = min_default_value;
                    }
                    else
                    {
                        min_value = strtoull(pToken,NULL,10);
                    }

                    pTest = strchr(pToken,':');
                    if (!pTest || *(pTest+1)=='\0' || *(pTest+1)==']')
                    {
                        max_value = max_default_value;
                    }
                    else
                    {
                        max_value = strtoull(pTest+1,NULL,10);
                    }

                    if (cur_value<=max_value && cur_value>=min_value)
                    {
                        DM_FREE(pRangeTmp);
                        bRet = DM_TRUE;
                        break;
                    }
                }

                pToken = strtok_r(NULL,",",&pTmp);
            }

            if (bRet == DM_FALSE)
            {
                DM_WARNING("value out of the unsigned range([xxx,xxx:xxx]) in DmDataIntegerValidCheck()");
            }

            DM_FREE(pRangeTmp);
        }

    }

    return bRet;
}

BOOL8 DmDataUNT32HexValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
    INT8 *pChar = pValue;
    INT32 length = 0;
    INT32 cnt = 0;

	if (pValueRange == NULL || pValue == NULL || strlen(pValue) <= 2)
	{
		DM_WARNING("Invalid parameters(NULL/empty) in DmDataUNT32HexValidCheck()");
		return bRet;
	}

    length = strlen(pValue);
    if (length<2
        || (pChar[0]!='0' || (pChar[1]!='x' && pChar[1]!='X')))
    {
        DM_WARNING("hex number should have 0x prefix in DmDataUNT32HexValidCheck()");
		return bRet;
    }

    if (length>10 || length<=2)
    {
        DM_WARNING("Invalid length in DmDataUNT32HexValidCheck()");
		return bRet;
    }

    for (cnt=2;cnt<length;cnt++)
    {
        if ((pChar[cnt]>='0' && pChar[cnt]<='9')
            || (pChar[cnt]>='a' && pChar[cnt]<='f')
            || (pChar[cnt]>='A' && pChar[cnt]<='F'))
        {
            continue;
        }
        else
        {
            DM_WARNING("Invalid char found in the string in DmDataUNT32HexValidCheck()");
    		return bRet;
        }
    }

    return DM_TRUE;
}

BOOL8 DmDataUINT64ValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmDataIntegerValidCheck(pValueRange, pValue, DM_INTEGER_TYPE__UINT64);
}

BOOL8 DmDataUINT32ValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmDataIntegerValidCheck(pValueRange, pValue, DM_INTEGER_TYPE__UINT32);
}

BOOL8 DmDataUINT16ValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmDataIntegerValidCheck(pValueRange, pValue, DM_INTEGER_TYPE__UINT16);
}

BOOL8 DmDataUINT8ValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmDataIntegerValidCheck(pValueRange, pValue, DM_INTEGER_TYPE__UINT8);
}

BOOL8 DmDataINT64ValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmDataIntegerValidCheck(pValueRange, pValue, DM_INTEGER_TYPE__INT64);
}

BOOL8 DmDataINT32ValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmDataIntegerValidCheck(pValueRange, pValue, DM_INTEGER_TYPE__INT32);
}

BOOL8 DmDataINT16ValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmDataIntegerValidCheck(pValueRange, pValue, DM_INTEGER_TYPE__INT16);
}

BOOL8 DmDataINT8ValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmDataIntegerValidCheck(pValueRange, pValue, DM_INTEGER_TYPE__INT8);
}

BOOL8 DmDataBoolValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	INT8* pBoolRange = (INT8*)"0,1,true,false";
	return DmEnumStringValidCheck(pBoolRange, pValue);
}

BOOL8 DmDataStringLengthValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
	INT32 iStrLen = 0;

	if (pValue == NULL || pValueRange == NULL)
	{
		DM_WARNING("Invalid parameters(NULL) in DmDataStringLengthValidCheck");
		return bRet;
	}

	if (strlen(pValueRange) <= 0)
	{
		bRet = DM_TRUE;
		return bRet;
	}

	if (DmDataUINT64ValidCheck("",pValueRange) == DM_FALSE)
	{
		DM_WARNING("Bad value-range(non-digital) in DmDataStringLengthValidCheck!");
		return bRet;
	}

    bRet = DM_TRUE;
    while (*pValue!='\0')
    {
        if (*pValue>0 && *pValue<=127)
        {
            /* standard ascii chars 0~127 */
            iStrLen++;
            pValue++;
        }
        else
        {
            DM_WARNING("Invalid chars found in DmDataStringLengthValidCheck");
            bRet = DM_FALSE;
            break;
        }
    }

    if (iStrLen>atoi(pValueRange))
    {
        DM_WARNING("string beyond the max length in DmDataStringLengthValidCheck");
        bRet = DM_FALSE;
    }

	return bRet;
}

BOOL8 DmEnumStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
	INT8* pTmp = NULL;
	INT8* pToken = NULL;
	INT8* pRange = NULL;

	if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmEnumStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

	if (strlen(pValueRange) <= 0 || strlen(pValue) <= 0)
	{
        DM_WARNING("Invalid parameters(empty) in DmEnumStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
	}

	if (strchr(pValue,',') != NULL)
	{
        DM_WARNING("Invalid parameters(useless comma) in DmEnumStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

	pRange = strdup(pValueRange);

	if (pRange == NULL){
		DM_WARNING("Malloc failed!");
		return bRet;
	}

	pToken = strtok_r(pRange, ",", (char**)&pTmp);
	while(pToken != NULL)
	{
		if (strlen(pToken) == strlen(pValue) && strncmp(pToken, pValue, strlen(pValue)) == 0)
		{
			bRet = DM_TRUE;
			break;
		}

		pToken = strtok_r(NULL, ",", (char**)&pTmp);
	}

	DM_FREE(pRange);

	return bRet;
}

BOOL8 DmBitmapStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
	INT8 *pTmp = NULL;
	INT8 *pValueTmp = NULL;
	INT8 *pToken = NULL;

	if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmBitmapStringValidCheck");
		return bRet;
	}

	if (strlen(pValueRange) <= 0)
	{
        DM_WARNING("Invalid ValueRange(empty) in DmBitmapStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
	}

    if (strlen(pValue) == 0)
    {
        /* empty value */
        bRet = DM_TRUE;
        return bRet;
    }

    if (pValue[0]==',' || pValue[strlen(pValue)-1]==',' || strstr(pValue,",,")!=0)
    {
        DM_WARNING("Invalid parameters(useless comma) in DmBitmapStringValidCheck");
        bRet = DM_FALSE;
        return bRet;
    }

	pValueTmp = strdup(pValue);

	if (pValueTmp == NULL)
	{
		return bRet;
	}

	pToken = strtok_r(pValueTmp, ",", (char**)&pTmp);

	if (pToken == NULL)
	{
		bRet = DmEnumStringValidCheck(pValueRange, pValue);
		DM_FREE(pValueTmp);
		return bRet;
	}

	bRet = DM_TRUE;
	while(pToken != NULL)
	{
		bRet = DmEnumStringValidCheck(pValueRange, pToken);

		if (bRet == DM_FALSE)
		{
            DM_WARNING("Invalid value in DmBitmapStringValidCheck");
			break;
		}

		pToken = strtok_r(NULL, ",", (char**)&pTmp);
	}

    DM_FREE(pValueTmp);
	return bRet;
}

BOOL8 DmDataRangeValueValidCheck(INT8 *pValueRange, INT8 *pValue, INT32 integerType, INT32 *rangeCnt)
{
	BOOL8 bRet = DM_FALSE;
    INT8 *integerDefaultRange[]=
    {
        "[-128:127]",                                   /* DM_INTEGER_TYPE__INT8 */
        "[-32768:32767]",                               /* DM_INTEGER_TYPE__INT16 */
        "[-2147483648:2147483647]",                     /* DM_INTEGER_TYPE__INT32 */
        "[-9223372036854775808:9223372036854775807]",   /* DM_INTEGER_TYPE__INT64 */
        "[0:255]",                                      /* DM_INTEGER_TYPE__UINT8 */
        "[0:65535]",                                    /* DM_INTEGER_TYPE__UINT16 */
        "[0:4294967295]",                               /* DM_INTEGER_TYPE__UINT32 */
        "[0:18446744073709551615]",                     /* DM_INTEGER_TYPE__UINT64 */
    };

    if ((integerType>=DM_INTEGER_TYPE__INT8) && (integerType<=DM_INTEGER_TYPE__INT64))
    {
        INT8 *pTmpRange = NULL;
        INT8 *pTmpValue = NULL;
        INT8 *pToken = NULL;
        INT8 *pTmp = NULL;
        INT8 *pLow = NULL;
        INT8 *pHigh = NULL;
        INT64 lowValue = 0;
        INT64 highValue = 0;
        INT64 minValue = 0;
        INT64 maxValue = 0;
        INT8 *pDefaultRange = integerDefaultRange[integerType];
        INT64 min_default_value = strtoll(pDefaultRange+1,NULL,10);
        INT64 max_default_value = strtoll(strchr(pDefaultRange,':')+1,NULL,10);

        pTmpValue = strdup(pValue);
        pTmpRange = strdup(pValueRange);

        if (pTmpValue==NULL || pTmpRange==NULL)
        {
            DM_WARNING("malloc() failed in DmDataRangeValueValidCheck");
            DM_FREE(pTmpValue);
            DM_FREE(pTmpRange);
            bRet = DM_FALSE;
            return bRet;
        }

        pLow = pTmpValue;
        if ( (pHigh = strstr(pTmpValue,"..")) == NULL)
		{
            DM_WARNING("Invalid range value format (%s) in DmDataRangeValueValidCheck", pTmpValue);
            DM_FREE(pTmpValue);
            DM_FREE(pTmpRange);
            bRet = DM_FALSE;
            return bRet;
		}
        *pHigh = '\0';
        pHigh += 2;

        bRet = DmDataIntegerValidCheck(pValueRange, pLow, integerType);
        if (bRet==DM_FALSE)
        {
            DM_WARNING("invalid range value (low) in DmDataRangeValueValidCheck");
            DM_FREE(pTmpValue);
            DM_FREE(pTmpRange);
            bRet = DM_FALSE;
            return bRet;
        }

        bRet = DmDataIntegerValidCheck(pValueRange, pHigh, integerType);
        if (bRet==DM_FALSE)
        {
            DM_WARNING("invalid range value (high) in DmDataRangeValueValidCheck");
            DM_FREE(pTmpValue);
            DM_FREE(pTmpRange);
            bRet = DM_FALSE;
            return bRet;
        }

        lowValue = strtoll(pLow,NULL,10);
        highValue = strtoll(pHigh,NULL,10);

        if (lowValue>=highValue)
        {
            DM_WARNING("invalid range value (low>=high) in DmDataRangeValueValidCheck");
            DM_FREE(pTmpValue);
            DM_FREE(pTmpRange);
            bRet = DM_FALSE;
            return bRet;
        }

        if (strchr(pTmpRange,',')==NULL)
        {
            if ( (pHigh = strstr(pTmpRange,":")) == NULL)
            {
                /* range [xxx] */
                DM_WARNING("invalid unsigned range,  case [xxx], in DmDataRangeValueValidCheck");
                bRet = DM_FALSE;
            }
            else
            {
                /* range [xxx:xxx] */
                *pHigh = '\0';
                pHigh++;
				
                pLow = pTmpRange+1;   // skip "["
                if (*pLow=='\0')
                {
                    /* no min, use default */
                    minValue = min_default_value;
                }
                else
                {
                    minValue = strtoll(pLow,NULL,10);
                }

                if (*pHigh==']')
                {
                    /* no max, use default */
                    maxValue = max_default_value;
                }
                else
                {
                    maxValue = strtoll(pHigh,NULL,10);
                }

                if (lowValue>=minValue && highValue<=maxValue)
                {
                    /* matched */
                    bRet = DM_TRUE;
                    *rangeCnt = (highValue-lowValue)+1;
                }
                else
                {
                    DM_WARNING("invalid range value (not matched), case [xxx:xxx], lowValue=%lld minValue=%lld highValue=%lld maxValue=%lld in DmDataRangeValueValidCheck",lowValue,minValue,highValue,maxValue);
                    bRet = DM_FALSE;
                }
            }
        }
        else
        {
            /* range [xxx,xxx:xxx] */
            pToken = strtok_r(pTmpRange,",",&pTmp);

            while(pToken)
            {
                /* range [xxx:xxx] */
                pHigh = strstr(pToken,":");
                if (pHigh!=NULL)
                {
                    pLow = pToken++;
                    *pHigh = '\0';
                    pHigh++;

                    if (*pLow=='\0')
                    {
                        if (pToken==pTmpRange+1)
                        {
                            /* no min, use default */
                            minValue = min_default_value;
                        }
                        else
                        {
                            DM_WARNING("invalid value range (low) in DmDataRangeValueValidCheck");
                            bRet = DM_FALSE;
                            DM_FREE(pTmpValue);
                            DM_FREE(pTmpRange);
                            return bRet;
                        }
                    }
                    else
                    {
                        minValue = strtoll(pLow,NULL,10);
                    }

                    if (*pHigh=='\0')
                    {
                        DM_WARNING("invalid value range (high) in DmDataRangeValueValidCheck");
                        bRet = DM_FALSE;
                        DM_FREE(pTmpValue);
                        DM_FREE(pTmpRange);
                        return bRet;
                    }
                    else if (*pHigh==']')
                    {
                        /* no min, use default */
                        maxValue = max_default_value;
                    }
                    else
                    {
                        maxValue = strtoll(pHigh,NULL,10);
                    }

                    if (lowValue>=minValue && highValue<=maxValue)
                    {
                        /* matched */
                        bRet = DM_TRUE;
                        *rangeCnt = (highValue-lowValue)+1;
                    }
                    else
                    {
                        DM_WARNING("invalid range value (not matched), case [xxx,xxx:xxx], in DmDataRangeValueValidCheck");
                        bRet = DM_FALSE;
                    }
                }

                pToken = strtok_r(NULL,",",&pTmp);
            }
        }

        DM_FREE(pTmpValue);
        DM_FREE(pTmpRange);
    }
    else if ((integerType>=DM_INTEGER_TYPE__UINT8) && (integerType<=DM_INTEGER_TYPE__UINT64))
    {
        INT8 *pTmpRange = NULL;
        INT8 *pTmpValue = NULL;
        INT8 *pToken = NULL;
        INT8 *pTmp = NULL;
        INT8 *pLow = NULL;
        INT8 *pHigh = NULL;
        UINT64 lowValue = 0;
        UINT64 highValue = 0;
        UINT64 minValue = 0;
        UINT64 maxValue = 0;
        INT8 *pDefaultRange = integerDefaultRange[integerType];
        UINT64 min_default_value = strtoull(pDefaultRange+1,NULL,10);
        UINT64 max_default_value = strtoull(strchr(pDefaultRange,':')+1,NULL,10);

        pTmpValue = strdup(pValue);
        pTmpRange = strdup(pValueRange);

        if (pTmpValue==NULL || pTmpRange==NULL)
        {
            DM_WARNING("malloc() failed in DmDataRangeValueValidCheck");
            DM_FREE(pTmpValue);
            DM_FREE(pTmpRange);
            bRet = DM_FALSE;
            return bRet;
        }

        pLow = pTmpValue;
        if ( (pHigh = strstr(pTmpValue,"..")) == NULL)
		{
            DM_WARNING("Invalid range value format (%s) in DmDataRangeValueValidCheck", pTmpValue);
            DM_FREE(pTmpValue);
            DM_FREE(pTmpRange);
            bRet = DM_FALSE;
            return bRet;
		}		
        *pHigh = '\0';
        pHigh += 2;

        bRet = DmDataIntegerValidCheck(pValueRange, pLow, integerType);
        if (bRet==DM_FALSE)
        {
            DM_WARNING("invalid unsigned range value (low) in DmDataRangeValueValidCheck");
            DM_FREE(pTmpValue);
            DM_FREE(pTmpRange);
            bRet = DM_FALSE;
            return bRet;
        }

        bRet = DmDataIntegerValidCheck(pValueRange, pHigh, integerType);
        if (bRet==DM_FALSE)
        {
            DM_WARNING("invalid unsigned range value (high) in DmDataRangeValueValidCheck");
            DM_FREE(pTmpValue);
            DM_FREE(pTmpRange);
            bRet = DM_FALSE;
            return bRet;
        }

        lowValue = strtoull(pLow,NULL,10);
        highValue = strtoull(pHigh,NULL,10);

        if (lowValue>=highValue)
        {
            DM_WARNING("invalid unsigned range value (low>=high) in DmDataRangeValueValidCheck");
            DM_FREE(pTmpValue);
            DM_FREE(pTmpRange);
            bRet = DM_FALSE;
            return bRet;
        }

        if (strchr(pTmpRange,',')==NULL)
        {
			if ( (pHigh = strstr(pTmpRange,":")) == NULL)
            {
                /* range [xxx] */
                DM_WARNING("invalid unsigned range,  case [xxx], in DmDataRangeValueValidCheck");
                bRet = DM_FALSE;
            }
            else
            {
                /* range [xxx:xxx] */
                *pHigh = '\0';
                pHigh++;
				
                pLow = pTmpRange + 1;  // skip "["
                if (*pLow=='\0')
                {
                    /* no min, use default */
                    minValue = min_default_value;
                }
                else
                {
                    minValue = strtoull(pLow,NULL,10);
                }

                if (*pHigh==']')
                {
                    /* no max, use default */
                    maxValue = max_default_value;
                }
                else
                {
                    maxValue = strtoull(pHigh,NULL,10);
                }

                if (lowValue>=minValue && highValue<=maxValue)
                {
                    /* matched */
                    bRet = DM_TRUE;
                    *rangeCnt = (highValue-lowValue)+1;
                }
                else
                {
                    DM_WARNING("invalid unsigned range value (not matched), case [xxx:xxx], in DmDataRangeValueValidCheck");
                    bRet = DM_FALSE;
                }
            }
        }
        else
        {
            /* range [xxx,xxx:xxx] */
            pToken = strtok_r(pTmpRange,",",&pTmp);

            while(pToken)
            {
                /* range [xxx:xxx] */
                pHigh = strstr(pToken,":");
                if (pHigh!=NULL)
                {
                    pLow = pToken++;
                    *pHigh = '\0';
                    pHigh++;

                    if (*pLow=='\0')
                    {
                        if (pToken==pTmpRange+1)
                        {
                            /* no min, use default */
                            minValue = min_default_value;
                        }
                        else
                        {
                            DM_WARNING("invalid unsigned value range (low) in DmDataRangeValueValidCheck");
                            bRet = DM_FALSE;
                            DM_FREE(pTmpValue);
                            DM_FREE(pTmpRange);
                            return bRet;
                        }
                    }
                    else
                    {
                        minValue = strtoull(pLow,NULL,10);
                    }

                    if (*pHigh=='\0')
                    {
                        DM_WARNING("invalid unsigned value range (high) in DmDataRangeValueValidCheck");
                        bRet = DM_FALSE;
                        DM_FREE(pTmpValue);
                        DM_FREE(pTmpRange);
                        return bRet;
                    }
                    else if (*pHigh==']')
                    {
                        /* no min, use default */
                        maxValue = max_default_value;
                    }
                    else
                    {
                        maxValue = strtoull(pHigh,NULL,10);
                    }

                    if (lowValue>=minValue && highValue<=maxValue)
                    {
                        /* matched */
                        bRet = DM_TRUE;
                        *rangeCnt = (highValue-lowValue)+1;
                    }
                    else
                    {
                        DM_WARNING("invalid unsigned range value (not matched), case [xxx,xxx:xxx], in DmDataRangeValueValidCheck");
                        bRet = DM_FALSE;
                    }
                }

                pToken = strtok_r(NULL,",",&pTmp);
            }
        }

        DM_FREE(pTmpValue);
        DM_FREE(pTmpRange);

    }

    return bRet;
}

BOOL8 DmIntegerArrayValidCheck(INT8 *pValueRange, INT8 *pValue, INT32 integerType)
{
	BOOL8 bRet = DM_TRUE;
    INT32 cnt = 0;
    INT32 length = 0;
    INT32 arrayRange = 0;
    INT32 arrayLength = 0;
    INT8 *pSubRange = NULL;
    INT8 *pValueTmp = NULL;
    INT8 *pToken = NULL;
    INT8 *pTmp = NULL;

    if (pValueRange == NULL || pValue == NULL)
    {
        DM_WARNING("Invalid parameters(NULL) in DmIntegerArrayValidCheck");
        bRet = DM_FALSE;
        return bRet;
    }

    if (strlen(pValueRange) <= 0)
    {
        DM_WARNING("Invalid value range in DmIntegerArrayValidCheck");
        bRet = DM_FALSE;
        return bRet;
    }

    length = strlen(pValue);
    if (length == 0)
    {
        /* empty value */
        bRet = DM_TRUE;
        return bRet;
    }

    if (pValue[0]==',' || pValue[0]=='.' || pValue[length-1]==',' || pValue[length-1]=='.' || pValue[length-1]=='-'
        || strstr(pValue,",,")!=0 || strstr(pValue,",.")!=0 || strstr(pValue,".,")!=0 || strstr(pValue,"...")!=0)
    {
        DM_WARNING("Invalid parameters(useless comma) in DmIntegerArrayValidCheck");
        bRet = DM_FALSE;
        return bRet;
    }

    for (cnt=0;cnt<length;cnt++)
    {
        if ( isdigit(pValue[cnt])
            || (pValue[cnt]=='-' && isdigit(pValue[cnt+1]) && integerType>=DM_INTEGER_TYPE__INT8 && integerType<=DM_INTEGER_TYPE__INT64)
            || pValue[cnt]==','
            || (pValue[cnt]=='.' && (!isdigit(pValue[cnt-1]) || !isdigit(pValue[cnt+1]))))
        {
            continue;
        }
        else
        {
            DM_WARNING("Invalid chars found in DmIntegerArrayValidCheck");
            bRet = DM_FALSE;
            return bRet;
        }
    }

    arrayRange = strtol(pValueRange,&pSubRange,10);

    if (arrayRange<=0)
    {
        DM_WARNING("no length required for integer array in DmIntegerArrayValidCheck");
        bRet = DM_FALSE;
        return bRet;
    }

    if (strchr(pValue,',')==NULL)
    {
        if (strstr(pValue,"..")==NULL)
        {
            /* single value */
            bRet = DmDataIntegerValidCheck(pSubRange, pValue, integerType);

            if (bRet == DM_FALSE)
            {
                DM_WARNING("invalid integer found in DmIntegerArrayValidCheck");
            }
            else
            {
                arrayLength++;
            }
        }
        else if (strchr(pSubRange,':')==NULL)
        {
            DM_WARNING("not support range-value in DmIntegerArrayValidCheck");
            bRet = DM_FALSE;
            return bRet;
        }
        else
        {
            /* range value, xxx..xxx */
            INT32 rangeCnt = 0;
            bRet = DmDataRangeValueValidCheck(pSubRange, pValue, integerType, &rangeCnt);

            if (bRet == DM_FALSE)
            {
                DM_WARNING("invalid range-value found in DmIntegerArrayValidCheck");
            }
            else
            {
                arrayLength += rangeCnt;
            }
        }
    }
    else
    {
        /* xxx,xxx..xxx */
        pValueTmp = strdup(pValue);

        if (pValueTmp==NULL)
        {
            DM_WARNING("malloc() failed in DmIntegerArrayValidCheck");
            bRet = DM_FALSE;
            return bRet;
        }

        pToken = strtok_r(pValueTmp,",",&pTmp);

        while(pToken)
        {
            if (strstr(pToken,"..")==NULL)
            {
                /* single value */
                bRet = DmDataIntegerValidCheck(pSubRange, pToken, integerType);

                if (bRet == DM_FALSE)
                {
                    DM_WARNING("invalid integer found in DmIntegerArrayValidCheck");
					break;
                }
                else
                {
                    arrayLength++;
                }
            }
            else if (strchr(pSubRange,':')==NULL)
            {
                DM_WARNING("not support range-value in DmIntegerArrayValidCheck");
                bRet = DM_FALSE;
                break;
            }
            else
            {
                /* range value, xxx..xxx */
                INT32 rangeCnt = 0;
                bRet = DmDataRangeValueValidCheck(pSubRange, pToken, integerType, &rangeCnt);

                if (bRet == DM_FALSE)
                {
                    DM_WARNING("invalid range-value found in DmIntegerArrayValidCheck");
					break;
                }
                else
                {
                    arrayLength += rangeCnt;
                }
            }

            pToken = strtok_r(NULL,",",&pTmp);
        }

        DM_FREE(pValueTmp);
    }

    /* check the length */
    if (bRet == DM_TRUE && arrayLength > arrayRange)
    {
        DM_WARNING("invalid length for integer array in DmIntegerArrayValidCheck");
        bRet = DM_FALSE;
    }

    DM_FREE(pValueTmp);
    return bRet;
}

BOOL8 DmDataUINT64ArrayValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmIntegerArrayValidCheck(pValueRange,pValue,DM_INTEGER_TYPE__UINT64);
}

BOOL8 DmDataUINT32ArrayValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmIntegerArrayValidCheck(pValueRange,pValue,DM_INTEGER_TYPE__UINT32);
}

BOOL8 DmDataUINT16ArrayValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmIntegerArrayValidCheck(pValueRange,pValue,DM_INTEGER_TYPE__UINT16);
}

BOOL8 DmDataUINT8ArrayValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmIntegerArrayValidCheck(pValueRange,pValue,DM_INTEGER_TYPE__UINT8);
}

BOOL8 DmDataINT64ArrayValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmIntegerArrayValidCheck(pValueRange,pValue,DM_INTEGER_TYPE__INT64);
}

BOOL8 DmDataINT32ArrayValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmIntegerArrayValidCheck(pValueRange,pValue,DM_INTEGER_TYPE__INT32);
}

BOOL8 DmDataINT16ArrayValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmIntegerArrayValidCheck(pValueRange,pValue,DM_INTEGER_TYPE__INT16);
}

BOOL8 DmDataINT8ArrayValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    return DmIntegerArrayValidCheck(pValueRange,pValue,DM_INTEGER_TYPE__INT8);
}

/**
	pStringI: 				a.{i}.b.{i}.c.
	pStringDigit: 			a.1.b.1.
	return string will be: 	a.1.b.1.c.
*/
INT8* DmGetFullPathOfDigit(INT8 *pStringI, INT8 *pStringDigit)
{
	UINT32 i;
	INT8 *pRet;
    UINT32 lStrLen = strlen((char*)pStringI);
	if(lStrLen<8)
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmGetFullPathOfDigit parameter error");
		return NULL;
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmGetFullPathOfDigit %s %s", pStringI, pStringDigit);
	i = lStrLen-3;
	while(pStringI[i] != '.')
	{
		i--;
	}

	pRet = (INT8 *)calloc(1,strlen((char*)(pStringI+i+1))+strlen((char*)pStringDigit)+1);
	if(NULL == pRet)
	{
	    return NULL;
	}
	strcpy(pRet, (char*)pStringDigit);
	strcat(pRet,(char*)(pStringI+i+1));
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmGetFullPathOfDigit %s", pRet);
	return pRet;
}

void DmFreeGetParaValueReq(void *p)
{
    UINT32 i =0;
    DM_GET_PARA_VALUE_REQ_T *pMem = (DM_GET_PARA_VALUE_REQ_T*)p;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmFreeGetParaValueReq");
    if(NULL == pMem)
    {
        return;
    }

    /*DM_FREE pvList */
    if(NULL != pMem->pvList)
    {
        if(NULL != pMem->pvList->pstParamValue)
        {
            for(i = 0; i < pMem->pvList->udwCnt; i++)
            {
                DM_FREE(pMem->pvList->pstParamValue[i].pucName);
                DM_FREE(pMem->pvList->pstParamValue[i].pucValue);
                DM_FREE(pMem->pvList->pstParamValue[i].pucType);
            }
            DM_FREE(pMem->pvList->pstParamValue);
        }
        DM_FREE(pMem->pvList);
		pMem->pvList = NULL;
    }

   DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmFreeGetParaValueReq");	
}

INT8* DmGetObjectPath(INT8 *pucItemName)
{
	if((NULL == pucItemName)||(0 == strlen(pucItemName)))
	{
		return NULL;
	}

	
	UINT32 i = strlen(pucItemName)-1;
	while((i > 0)&&('.' != pucItemName[i]))
	{
		i--;
	}

	if(0 == i)
	{
		return NULL;
	}
	else
	{
		INT8 *pucRet = malloc(sizeof(INT8)*(i+2));
        if(NULL == pucRet)
        {
            return NULL;
        }
		memset(pucRet,0,sizeof(UINT8)*(i+2));
		memcpy(pucRet,pucItemName,(i+1));
		return pucRet;
	}
}

INT8* DmGetMulObjPathAndInstance(INT8 *pucObjectPath, UINT32 *pulInstanceNum)
{
	INT32 i = strlen(pucObjectPath);

	do
	{
		i--;
		while('.' != pucObjectPath[i])
		{
			i--;
		}
	}while(!isdigit(pucObjectPath[i+1]));
	
	*pulInstanceNum = atoi(pucObjectPath+i+1);

    INT8 *pucRet = (INT8 *)malloc(sizeof(INT8)*(i+2));
    if(NULL == pucRet)
    {
        return NULL;
    }
	memset(pucRet,0,sizeof(UINT8)*(i+2));

	strncpy(pucRet,pucObjectPath,sizeof(UINT8)*(i+1));

	return pucRet;
}



void DmFreeDmGetParaNameReq(void *p)
{
    UINT32 i =0;
    DM_GET_PARA_NAME_REQ_T *pMem = (DM_GET_PARA_NAME_REQ_T*)p;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmFreeDmGetParaNameReq");
    if(NULL == pMem)
    {
        return;
    }
 
    /*DM_FREE name */
    if(NULL != pMem->pNameList)
    {
        if(NULL != pMem->pNameList->pucParamName)
        {
            for(i = 0; i < pMem->pNameList->udwCnt; i++)
            {
                DM_FREE(pMem->pNameList->pucParamName[i].pucName);
            }
            DM_FREE(pMem->pNameList->pucParamName);
        }
        DM_FREE(pMem->pNameList);
    }
    
    /*DM_FREE pMem*/
   // DM_FREE(pMem);
   DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmFreeDmGetParaNameReq");	
}

INT32 DmGetChildrenCnt(DM_HANDLER_T   *pDmHandler, INT8* pucParamName, INT32 lId, UINT32 TransactionId)
{
    INT32 i = 0;
    INT32 lCnt = 0;
    INT32 ret = 0;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmGetChildrenCnt %s", pucParamName);
    /* if is a object path, get the children of it */
    if(DM_TRUE == pDmHandler->pDbIsObjectFunc(pDmHandler->pPattern[lId].ucParaType))
    {
        /* get the count of object, if 0 in db, return 0
        *  else return the number of object and number of children
        */
        //if(pucParamName.endwiths("{i}."))
        if(strcmp((char*)pucParamName+(strlen((char*)pucParamName)-4), "{i}.") == 0)
        {
            INT8 * tmp = (INT8*)malloc(strlen((char*)pucParamName)*sizeof(INT8));
            if(NULL == tmp)
            {
                return 0;
            }

            memset(tmp, 0, strlen((char*)pucParamName)*sizeof(UINT8));
            strncpy(tmp, (char*)pucParamName, strlen((char*)pucParamName)-4);
            lCnt = DbGetObjectNum(pDmHandler, tmp);
            DM_FREE(tmp);
			DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmGetChildrenCnt %d", ret);
            return lCnt;
        }
        else
        {
            lCnt = DbGetObjectNum(pDmHandler, pucParamName);
        }

        if(lCnt <= 0)
        {
          ret = 0;
        }
        else
        {

            for(i = lId; pDmHandler->pPattern[i].pucNodeName != NULL; i++)
            {
                if(pDmHandler->pPattern[i].ulParentIdx == lId)
                {
            /* if a object get all the number of it
                    * delete for tr069 not required to return the subobject as a.a.1 not a child, but a.a. is a child
                    *if(DATA_TYPE_OBJECT == pDmHandler->pPattern[i].ucParaType)
                    *{
                    *     get the name of object, and call GetObjectNumber from db 
                    *   UINT8 *pTmpStr = NULL;
                    *   UINT8 *pucObjName = NULL;
                    *   lStrLen = strlen(pucParamName) + strlen(pDmHandler->pPattern[i].pucNodeName)+ strlen(".")+1;
                    *   pTmpStr = (UINT8*)malloc(sizeof(UINT8)*lStrLen);
                    *   memset(pTmpStr, 0, sizeof(UINT8)*lStrLen);
                    *   strcat(pTmpStr, pucParamName);
                    *   pucObjName = getObjName(pDmHandler->pPattern[i].pucNodeName);
                    *   strcat(pTmpStr, pucObjName);
                    *   strcat(pTmpStr, ".");
                    *   UINT32 udwIndex[DM_MAXOBJ_NUM+1] = {0};
                    *   lCnt = ILDB_GetObjectNumber(g_pDmCwmpDb, pTmpStr, udwIndex);
                    *    more than 1, add to the return 
                    *   ret += lCnt;
                    *   DM_FREE(pTmpStr);
                    *   DM_FREE(pucObjName);
                    *}
                    * if not an object, add 1 
                    *else
                    *{*/

                    if (!DmCheckRight(TransactionId, pDmHandler, RIGHT_R, i))
                    {
                        continue;
                    }

                    ret ++;

                    /*}*/
                }
            }            
        }
    }
    /* if is a item, get nothing, only add itself*/
    else
    {
        if (DmCheckRight(TransactionId, pDmHandler, RIGHT_R, lId))
        {
            lCnt = DbGetObjectNum(pDmHandler, pucParamName);
            ret = lCnt;
        }
    }
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmGetChildrenCnt %d", ret);	
    return ret;
}

BOOL8 DmCheckParamExistInXml(DM_HANDLER_T *pDmHandler, INT8 *pucPath)
{
	xmlDocPtr pDoc = pDmHandler->pXmlDocument;
    INT8 *ucXPathName = NULL;
    INT32               i = 0;
    INT32               lIndex     = 0;
    xmlXPathContextPtr	xpathCtx; 
	xmlXPathObjectPtr	xpathObj; 

	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DmCheckParamExistInXml ucXPathName = NULL");
        return DM_FALSE;
	}
    memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	/*change the path to the xpath*/
	ConvertPathToXPath(pucPath, ucXPathName);

    i = strlen((char*)pucPath)-2;
		
	while(isdigit(pucPath[i]))
	{
		i--;
	}

	if(pucPath[i] == '.')
	{/* multi obj with instance */
	}
	else
	{
		lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID),pucPath);
		if(lIndex < 0)
		{
			free(ucXPathName);
		    return DM_FALSE;
		}
		if(DM_TRUE == DmIsMultipleObject(pDmHandler->pPattern,lIndex))
		{
			strcat((char*)ucXPathName,"/*");
		}
	}

	
	xpathCtx	=	xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

	DM_FREE(ucXPathName);
	
	
	/*check the element is existing, not existing, return error
	*/

	/*delete the element and return true*/
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))	
	{
	    if(xpathObj != NULL)
    	{
    	    xmlXPathFreeObject(xpathObj);
    	}
	    xmlXPathFreeContext(xpathCtx);
		return DM_FALSE;
	}

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);

    return DM_TRUE;
}

INT32 DmGetChildren(DM_GET_PARA_NAME_REQ_T *pGetParaNameReq)
{
    INT32 ret = DM_RET_SUCCESS;
    INT32 lCnt = 0;
    INT32 i = 0;
    INT32 lStrLen = 0;
    INT8 *pOriginalName = NULL;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmGetChildren");


    /* if path is "", get all the parameters, means from the root node */
    if((pGetParaNameReq->pucParamName != NULL) && (strlen((char*)pGetParaNameReq->pucParamName) > 0))
    {
    }
    else
    {
        /*when "", set it to the root node's name */
        pOriginalName = pGetParaNameReq->pucParamName;
        pGetParaNameReq->pucParamName = strdup("Device.");
    }


    DM_HANDLER_T   *pDmHandler = NULL;
    INT32           lId      = -1;

    /*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pGetParaNameReq->pucParamName);

    pGetParaNameReq->pfnFree = DmFreeDmGetParaNameReq;  //yuqiangc 2012.02.13: we move it to here to prevent from NULL pointer being used


    if(NULL == pDmHandler)
    {
        return DM_RET_INVALID_PARAM_NAME;
    }

    lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetParaNameReq->pucParamName);

    if(lId < 0)
    {
       ret = DM_RET_INVALID_PARAM_NAME;
       return ret;
    }

    if (!DmCheckRight(pGetParaNameReq->TransactionId, pDmHandler, RIGHT_R, lId))
    {
        ret = DM_RET_INVALID_PARAM_NAME;
        return ret;
    }

	if((DM_TRUE == DmIsMultipleObject(pDmHandler->pPattern,lId))&&(DM_FALSE == DmIsSingleOfMultiple(pGetParaNameReq->pucParamName))
		&&(0 != strcmp(pGetParaNameReq->pucParamName+strlen(pGetParaNameReq->pucParamName)-5,".{i}.")))
	{
        INT8 *tmpName = (INT8 *)malloc(strlen((char*)pGetParaNameReq->pucParamName)+ strlen("{i}.")+1);
        if(NULL == tmpName)
        {
            return DM_RET_INTERNAL_ERROR;
        }
        memset((char*)tmpName, 0, strlen((char*)pGetParaNameReq->pucParamName)+ strlen("{i}.")+1);
        strcat((char*)tmpName, (char*)pGetParaNameReq->pucParamName);
        strcat((char*)tmpName, "{i}.");
		
        pOriginalName = pGetParaNameReq->pucParamName;
        pGetParaNameReq->pucParamName = tmpName;
	}
    
    //pGetParaNameReq->pfnFree = DmFreeDmGetParaNameReq;
    lCnt = DmGetChildrenCnt(pDmHandler,pGetParaNameReq->pucParamName,lId,pGetParaNameReq->TransactionId);

    /* add the count of self */
    pGetParaNameReq->pNameList = (PARAMETER_NAME_LIST_T*)malloc(sizeof(PARAMETER_NAME_LIST_T));
    if(NULL == pGetParaNameReq->pNameList)
    {
        if (pOriginalName != NULL)
        {
            DM_FREE(pGetParaNameReq->pucParamName);
            pGetParaNameReq->pucParamName = pOriginalName;
        }
        return DM_RET_INTERNAL_ERROR;
    }
    memset(pGetParaNameReq->pNameList, 0, sizeof(PARAMETER_NAME_LIST_T));
    pGetParaNameReq->pNameList->udwCnt = lCnt;
    if(lCnt <= 0)
    {
       if (pOriginalName != NULL)
       {
           DM_FREE(pGetParaNameReq->pucParamName);
           pGetParaNameReq->pucParamName = pOriginalName;
       }
       return ret;
    }
    pGetParaNameReq->pNameList->pucParamName = (PARAMETER_NAME_T*)malloc(sizeof(PARAMETER_NAME_T)*lCnt);
    if(NULL == pGetParaNameReq->pNameList->pucParamName)
    {
        if (pOriginalName != NULL)
        {
            DM_FREE(pGetParaNameReq->pucParamName);
            pGetParaNameReq->pucParamName = pOriginalName;
        }
        return DM_RET_INTERNAL_ERROR;
    }
    memset(pGetParaNameReq->pNameList->pucParamName,0, sizeof(PARAMETER_NAME_T)*lCnt);

    /* if is a object path and not end with {i}., get the children of it */
    if(DM_TRUE == pDmHandler->pDbIsObjectFunc(pDmHandler->pPattern[lId].ucParaType) && strcmp((char*)pGetParaNameReq->pucParamName+(strlen((char*)pGetParaNameReq->pucParamName)-4), "{i}.") != 0)
    {
        INT32 j = 0;
        for(i = lId; pDmHandler->pPattern[i].pucNodeName != NULL; i++)
        {
            /* get the item name */
            if(DM_TRUE != pDmHandler->pDbIsObjectFunc(pDmHandler->pPattern[i].ucParaType) 
               && pDmHandler->pPattern[i].ulParentIdx == lId 
               && DmCheckRight(pGetParaNameReq->TransactionId, pDmHandler, RIGHT_R, i))
            {
                UINT8 *pTmpStr = NULL;
                lStrLen = strlen((char*)pGetParaNameReq->pucParamName) + strlen((char*)pDmHandler->pPattern[i].pucNodeName)+ strlen(".")+1; //strlen(".") ??
                pTmpStr = (UINT8*)malloc(sizeof(UINT8)*lStrLen);
                if(NULL == pTmpStr)
                {
                    return DM_RET_INTERNAL_ERROR;
                }
                memset(pTmpStr, 0, sizeof(UINT8)*lStrLen);
                strcat((char*)pTmpStr, (char*)pGetParaNameReq->pucParamName);
                strcat((char*)pTmpStr, (char*)pDmHandler->pPattern[i].pucNodeName);

                pGetParaNameReq->pNameList->pucParamName[j].pucName =  strdup((char*)pTmpStr);

                if(DmCheckRight(pGetParaNameReq->TransactionId, pDmHandler, RIGHT_W, i))
                {
                    pGetParaNameReq->pNameList->pucParamName[j].bWritable = DM_TRUE;
                }
                else
                {
                    pGetParaNameReq->pNameList->pucParamName[j].bWritable = DM_FALSE;
                }
                DM_FREE(pTmpStr);
                j++;
            }
            /* get the object name */
            else if(DM_TRUE == pDmHandler->pDbIsObjectFunc(pDmHandler->pPattern[i].ucParaType)
                    && pDmHandler->pPattern[i].ulParentIdx == lId 
                    && DmCheckRight(pGetParaNameReq->TransactionId, pDmHandler, RIGHT_R, i))
            {
                /* get the name of object, and call GetObjectNumber from db */
                UINT8 *pucTmpStr = NULL;
                INT8 *pucObjName = NULL;

                lStrLen = strlen((char*)pGetParaNameReq->pucParamName)+strlen((char*)pDmHandler->pPattern[i].pucNodeRealName)+ strlen(".")+1;
                pucTmpStr = (UINT8*)DM_MALLOC(sizeof(UINT8)*lStrLen);
                if(NULL == pucTmpStr)
                {
                    return DM_RET_INTERNAL_ERROR;
                }
                memset(pucTmpStr, 0, sizeof(UINT8)*lStrLen);
                strcat((char*)pucTmpStr, (char*)pGetParaNameReq->pucParamName);
                pucObjName = pDmHandler->pPattern[i].pucNodeRealName;
                strcat((char*)pucTmpStr, pucObjName);
                strcat((char*)pucTmpStr, ".");

                pGetParaNameReq->pNameList->pucParamName[j].pucName =  strdup((char*)pucTmpStr);

                if(DmCheckRight(pGetParaNameReq->TransactionId, pDmHandler, RIGHT_W, i))
                {
                    pGetParaNameReq->pNameList->pucParamName[j].bWritable = DM_TRUE;
                }
                else
                {
                    pGetParaNameReq->pNameList->pucParamName[j].bWritable = DM_FALSE;
                }

                j++;
                DM_FREE(pucTmpStr);


            }
            else{}
        }
    }
    else if(DM_TRUE == pDmHandler->pDbIsObjectFunc(pDmHandler->pPattern[lId].ucParaType) && strcmp((char*)pGetParaNameReq->pucParamName+(strlen((char*)pGetParaNameReq->pucParamName)-4), "{i}.") == 0)
    {
        INT32 k = 0;
        INT32 j = 0;
        /* get the name of object, and call GetObjectNumber from db */
    	INT8 *pucTmpStr = NULL;
        UINT8 *pucNum = NULL;

        lStrLen = strlen((char*)pGetParaNameReq->pucParamName)+strlen((char*)pDmHandler->pPattern[lId].pucNodeName)+ strlen(".")+1;
        pucTmpStr = (INT8*)malloc(sizeof(INT8)*lStrLen);

        if(NULL == pucTmpStr)
        {
            return DM_RET_INTERNAL_ERROR;
        }

        memset(pucTmpStr, 0, sizeof(UINT8)*lStrLen);
        strncpy(pucTmpStr, (char*)pGetParaNameReq->pucParamName, strlen((char*)pGetParaNameReq->pucParamName)-4);
        //pucObjName = getObjName(pDmHandler->pPattern[lId].pucNodeName);
        //strcat(pucTmpStr, pucObjName);
        //strcat(pucTmpStr, ".");               
        
        UINT32 *udwIndex = NULL;
        
		if(DM_TRUE != DbGetObjectIndexContOld(pDmHandler,pucTmpStr,&udwIndex))
		{
			//DmError();
			DM_FREE(pucTmpStr);
			return DM_RET_INTERNAL_ERROR; 
		}

        DM_FREE(pucTmpStr);

        if(NULL != udwIndex)
        {      
		    lCnt = udwIndex[0];

            for(k =1; k < lCnt+1; k++)
            {
                pucNum = (UINT8*)DmUintToStr((UINT32)udwIndex[k]);

                lStrLen = strlen((char*)pGetParaNameReq->pucParamName) + strlen((char*)pucNum) +
                     strlen((char*)pDmHandler->pPattern[lId].pucNodeName)+ strlen(".")+1;

                pucTmpStr = (INT8*)malloc(sizeof(INT8)*lStrLen);
                if(NULL == pucTmpStr)
                {
                    DM_FREE(pucNum);
					DM_FREE(udwIndex);
                    return DM_RET_INTERNAL_ERROR;
                }
                memset(pucTmpStr, 0, sizeof(UINT8)*lStrLen);
                strncpy(pucTmpStr, (char*)pGetParaNameReq->pucParamName, strlen((char*)pGetParaNameReq->pucParamName)-4);
                //strcat(pucTmpStr, pucObjName);
                //strcat(pucTmpStr, ".");                    
                if( 0 != udwIndex[k] )   //??
                {
                    strcat(pucTmpStr, (char*)pucNum);
                    strcat(pucTmpStr, ".");
                }

                pGetParaNameReq->pNameList->pucParamName[j].pucName =  strdup(pucTmpStr);
                if(DmCheckRight(pGetParaNameReq->TransactionId, pDmHandler, RIGHT_W, lId))
                {
                    pGetParaNameReq->pNameList->pucParamName[j].bWritable = DM_TRUE;
                }
                else
                {
                    pGetParaNameReq->pNameList->pucParamName[j].bWritable = DM_FALSE;
                }
                j++;
                DM_FREE(pucTmpStr);
                DM_FREE(pucNum);
            }

            DM_FREE(udwIndex);
        }
    }
    /* if is a item, get nothing, only add itself*/
    else
    {
        if (DmCheckParamExistInXml(pDmHandler,pGetParaNameReq->pucParamName)==DM_FALSE)
        {
            return DM_RET_INVALID_PARAM_NAME;
        }

        pGetParaNameReq->pNameList->pucParamName[0].pucName =  strdup(pGetParaNameReq->pucParamName);
        if(DmCheckRight(pGetParaNameReq->TransactionId, pDmHandler, RIGHT_W, lId))
        {
            pGetParaNameReq->pNameList->pucParamName[0].bWritable = DM_TRUE;
        }
        else
        {
            pGetParaNameReq->pNameList->pucParamName[0].bWritable = DM_FALSE;
        }
    }

    if (pOriginalName != NULL)
    {
        DM_FREE(pGetParaNameReq->pucParamName);
        pGetParaNameReq->pucParamName = pOriginalName;
    }

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmGetChildren");	
    return ret;
}

INT32 DmGetDescendantNames(DM_PARA_INFO_T *p)
{
	INT8* pucFullName = NULL;

    INT32 i = 0;
    INT32 retCnt = 0;
    INT32 lStrLen = 0;
    DM_PARA_NAME_T *pParaNameLast = NULL;
    DM_PARA_NAME_T *pParaNameNew = NULL;
    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmGetDescendantNames %s", p->pucParamName);


	DM_HANDLER_T *pDmHandler = p->pDmHandler;
	INT8* pucParamName = p->pucParamName;
	UINT32 TransactionId = p->TransactionId;
	INT32 lId = p->lId;


    pParaNameLast = (DM_PARA_NAME_T *)malloc(sizeof(DM_PARA_NAME_T));
    if(NULL == pParaNameLast)
    {
        return 0;
    }
    memset(pParaNameLast, 0, sizeof(DM_PARA_NAME_T));
    p->pParaNameFirst = pParaNameLast;

    if (!DmCheckRight(TransactionId, pDmHandler, RIGHT_R, lId))
    {
        return 0;
    }

    /* if is a object path, get the children of it */
    if(DM_TRUE == pDmHandler->pDbIsObjectFunc(pDmHandler->pPattern[lId].ucParaType) && (strcmp((char*)(pucParamName+(strlen((char*)pucParamName))-4), "{i}.") != 0))
    {
        /* add self */
        pParaNameLast->pucName = strdup((char*)pucParamName);

        if(DmCheckRight(TransactionId, pDmHandler, RIGHT_W, lId))
        {
            pParaNameLast->bWritable = DM_TRUE;
        }
        else
        {
            pParaNameLast->bWritable = DM_FALSE;
        }

        retCnt +=1;

        /*add all the children */  
        for(i = lId; pDmHandler->pPattern[i].pucNodeName != NULL; i++)
        {
            if(pDmHandler->pPattern[i].ulParentIdx != lId)
            {
                continue;
            }

            if (!DmCheckRight(TransactionId, pDmHandler, RIGHT_R, i))
            {
                continue;
            }

            if(DM_TRUE != pDmHandler->pDbIsObjectFunc(pDmHandler->pPattern[i].ucParaType))
            {
                UINT8 *pTmpStr = NULL;
                lStrLen = strlen((char*)pucParamName) + strlen((char*)pDmHandler->pPattern[i].pucNodeName)+ strlen(".") + 1;
                pTmpStr = (UINT8*)malloc(sizeof(UINT8)*lStrLen);
                if(NULL == pTmpStr)
                {
                    return 0;
                }
                memset(pTmpStr, 0, sizeof(UINT8)*lStrLen);
                strcat((char*)pTmpStr, (char*)pucParamName);
                strcat((char*)pTmpStr, (char*)pDmHandler->pPattern[i].pucNodeName);
               // DM_TRACE(DM_TRACE_LEVEL_LOW, "%d\n", retCnt);

                pParaNameNew = (DM_PARA_NAME_T *)malloc(sizeof(DM_PARA_NAME_T));
               if(NULL == pParaNameNew)
               {
				   if (pTmpStr != NULL)
				   	{
				   		DM_FREE(pTmpStr);
				   	}

                   return 0;
               }
                memset(pParaNameNew, 0, sizeof(DM_PARA_NAME_T));
    
                pParaNameNew->pucName = strdup((char*)pTmpStr);

                if(DmCheckRight(TransactionId, pDmHandler, RIGHT_W, i))
                {
                    pParaNameNew->bWritable = DM_TRUE;
                }
                else
                {
                    pParaNameNew->bWritable = DM_FALSE;
                }

                pParaNameLast->pNext = pParaNameNew;
                pParaNameLast = pParaNameNew;
                
                retCnt +=1;
                DM_FREE(pTmpStr);
            }
            /* get all the child from the table, and include {i} to real number*/
            else if(DM_TRUE == pDmHandler->pDbIsObjectFunc(pDmHandler->pPattern[i].ucParaType) 
                   && (strcmp((char*)(pDmHandler->pPattern[i].pucNodeName +(strlen((char*)pDmHandler->pPattern[i].pucNodeName))-4), "{i}.") == 0))
            {
                /*get all the child of object*/
                UINT32 ulInstanceCnt = 0;
                UINT8 *pucName = NULL;
                UINT32 *udwIndex = NULL;
                /*get the child object name */
                pucName = (UINT8*)pDmHandler->pPattern[i].pucNodeRealName;

                /*get the child object's number and index->i */
                lStrLen = strlen((char*)pucParamName) + strlen((char*)pucName) + 8;
                lStrLen ++;
                pucFullName = (INT8*)DM_MALLOC(sizeof(INT8)*lStrLen);
                if(NULL == pucFullName)
                {
                    return 0;
                }
                memset((char*)pucFullName, 0, sizeof(UINT8)*lStrLen);
                strcat((char*)pucFullName, (char*)pucParamName);
                strcat((char*)pucFullName, (char*)pucName);
                strcat((char*)pucFullName, ".");
                
                /*add this object self*/
                pParaNameNew = (DM_PARA_NAME_T *)malloc(sizeof(DM_PARA_NAME_T));
                if(NULL == pParaNameNew)
                {
					if (pucFullName != NULL)
					{
						DM_FREE(pucFullName);
					}
                    return 0;
                }
                memset(pParaNameNew, 0, sizeof(DM_PARA_NAME_T));

                
                pParaNameNew->pucName = strdup((char*)pucFullName);
                if(DmCheckRight(TransactionId, pDmHandler, RIGHT_W, i))
                {
                    pParaNameNew->bWritable = DM_TRUE;
                }
                else
                {
                    pParaNameNew->bWritable = DM_FALSE;
                }

                pParaNameLast->pNext = pParaNameNew;
                pParaNameLast = pParaNameNew;

                retCnt +=1;

                if(DM_TRUE != DbGetObjectIndexContOld(pDmHandler,pucFullName,&udwIndex))
                {
                    //DmError();
                    DM_FREE(pucFullName);
                    continue; 
                }

                DM_FREE(pucFullName);

                if(NULL != udwIndex)
                {      
                    ulInstanceCnt = udwIndex[0];

                    UINT32 j = 0;
                    /*recursive call to get all the child obj's children count*/
                    for(j=1; j<ulInstanceCnt+1; j++)
                    {
                        UINT8 *pucNum = (UINT8*)DmUintToStr((UINT32)udwIndex[j]);
                        /* length of parent object path, length of child obj path, length of "{}.\0"*/
                        lStrLen = strlen((char*)pucParamName) + strlen((char*)pucName) + strlen((char*)pucNum) + 8;

                        pucFullName = (INT8*)malloc(sizeof(INT8)*lStrLen);
                        if(NULL == pucFullName)
                        {
                            DM_FREE(pucNum);
                            DM_FREE(udwIndex);
                            return 0;
                        }
                        memset(pucFullName, 0, sizeof(UINT8)*lStrLen);
                        strcat((char*)pucFullName, (char*)pucParamName);
                        strcat((char*)pucFullName, (char*)pucName);
                        strcat((char*)pucFullName, ".");
                        if( 0 != udwIndex[j] && strcmp((char*)pDmHandler->pPattern[i].pValueRange, "0") != 0) //?? no need
                        {
    	                    strcat((char*)pucFullName, (char*)pucNum);
    	                    strcat((char*)pucFullName, ".");
                        }
                        strcat((char*)pucFullName, "\0");
                        DM_FREE(pucNum);

                        //SetDescendantParamNames(plArrayIdx, pParamInfo, pucFullName);
    					DM_PARA_INFO_T *pDmParaInfo = (DM_PARA_INFO_T *)malloc(sizeof(DM_PARA_INFO_T));
                        if(NULL == pDmParaInfo)
                        {
    						if (pucFullName != NULL)
    						{
    							DM_FREE(pucFullName);
    						}

                            DM_FREE(udwIndex);
                            return 0;
                        }
    					memset(pDmParaInfo,0,sizeof(DM_PARA_INFO_T));
    					pDmParaInfo->pDmHandler = pDmHandler;
    					pDmParaInfo->pucParamName = pucFullName;
    					pDmParaInfo->TransactionId = TransactionId;
    					pDmParaInfo->lId = i;

    					retCnt += DmGetDescendantNames(pDmParaInfo);

    					pParaNameLast->pNext = pDmParaInfo->pParaNameFirst;
    					pParaNameLast = pDmParaInfo->pParaNameLast;
    					DM_FREE(pDmParaInfo);
                        DM_FREE(pucFullName);
                    }

                    DM_FREE(udwIndex);
                }

            }
            /* get all the child from the table, and not subobj*/
            else if(DM_TRUE == pDmHandler->pDbIsObjectFunc(pDmHandler->pPattern[i].ucParaType) 
                   && (strcmp((char*)(pDmHandler->pPattern[i].pucNodeName +(strlen((char*)pDmHandler->pPattern[i].pucNodeName))-4), "{i}.") != 0))
            {
                /*get all the child of object*/
                UINT8 *pucName = NULL;

                /*get the child object name */
                pucName = (UINT8*)pDmHandler->pPattern[i].pucNodeRealName;

                /*get the child object's number and index->i */
                lStrLen = strlen((char*)pucParamName) + strlen((char*)pucName) + 8;
                lStrLen ++;
                pucFullName = (INT8*)DM_MALLOC(sizeof(INT8)*lStrLen);
                if(NULL == pucFullName)
                {
                    return 0;
                }
                memset(pucFullName, 0, sizeof(UINT8)*lStrLen);
                strcat((char*)pucFullName, (char*)pucParamName);
                strcat((char*)pucFullName, (char*)pucName);
                strcat((char*)pucFullName, ".");

                //SetDescendantParamNames(plArrayIdx, pParamInfo, pucFullName);
				DM_PARA_INFO_T *pDmParaInfo = (DM_PARA_INFO_T *)malloc(sizeof(DM_PARA_INFO_T));
                if(NULL == pDmParaInfo)
                {

					if (pucFullName != NULL)
					{
						DM_FREE(pucFullName);
					}

                    return 0;
                }

				memset(pDmParaInfo,0,sizeof(DM_PARA_INFO_T));
				pDmParaInfo->pDmHandler = pDmHandler;
				pDmParaInfo->pucParamName = pucFullName;
				pDmParaInfo->TransactionId = TransactionId;
				pDmParaInfo->lId = i;
				
				retCnt += DmGetDescendantNames(pDmParaInfo);
				
				pParaNameLast->pNext = pDmParaInfo->pParaNameFirst;
				pParaNameLast = pDmParaInfo->pParaNameLast;
				DM_FREE(pDmParaInfo);
				
                DM_FREE(pucFullName);
            }
            else{}
        }
    }
    else if(DM_TRUE == pDmHandler->pDbIsObjectFunc(pDmHandler->pPattern[lId].ucParaType) &&  (strcmp((char*)pucParamName+(strlen((char*)pucParamName)-4), "{i}.") == 0))
    {
        /*get all the child of object*/
		UINT32 ulInstanceCnt = 0;
        UINT32 *udwIndex = NULL;

        /*get the child object's number and index->i */
        lStrLen = strlen((char*)pucParamName)  + 8;
        lStrLen ++;
        pucFullName = (INT8*)malloc(sizeof(INT8)*lStrLen);
        if(NULL == pucFullName)
        {
            return 0;
        }
        memset(pucFullName, 0, sizeof(UINT8)*lStrLen);
        strncpy((char*)pucFullName, (char*)pucParamName, strlen((char*)pucParamName)-4);
        
        /*add this object self*/
		pParaNameLast->pucName = strdup((char*)pucFullName);
		
		if(DmCheckRight(TransactionId, pDmHandler, RIGHT_W, lId))
		{
			pParaNameLast->bWritable = DM_TRUE;
		}
		else
		{
			pParaNameLast->bWritable = DM_FALSE;
		}
		
        retCnt++;

		if(DM_TRUE != DbGetObjectIndexContOld(pDmHandler,pucFullName,&udwIndex))
		{
			//DmError();
			DM_FREE(pucFullName);
			p->pParaNameLast = pParaNameLast;
			return retCnt;
		}

        DM_FREE(pucFullName);

        if(NULL != udwIndex)
        {      
            ulInstanceCnt = udwIndex[0];

            UINT32 j = 0;
            /*recursive call to get all the child obj's children count*/
            for(j=1; j<ulInstanceCnt+1; j++)
            {
                UINT8 *pucNum = (UINT8*)DmUintToStr((UINT32)(udwIndex[j]));
                /* length of parent object path, length of child obj path, length of "{}.\0"*/
                lStrLen = strlen((char*)pucParamName) +  strlen((char*)pucNum) + 8;

                pucFullName = (INT8*)malloc(sizeof(INT8)*lStrLen);
                if(NULL == pucFullName)
                {
                    DM_FREE(pucNum);
                    DM_FREE(udwIndex);
                    return 0;
                }
                memset(pucFullName, 0, sizeof(UINT8)*lStrLen);
                strncpy((char*)pucFullName, (char*)pucParamName, strlen((char*)pucParamName)-4);

                if( 0 != udwIndex[j] && strcmp((char*)pDmHandler->pPattern[lId].pValueRange, "0") != 0)//?? no need
                {
                    strcat((char*)pucFullName, (char*)pucNum);
                    strcat((char*)pucFullName, ".");
                }
                strcat((char*)pucFullName, "\0");
                DM_FREE(pucNum);

                //SetDescendantParamNames(plArrayIdx, pParamInfo, pucFullName);

    			DM_PARA_INFO_T *pDmParaInfo = (DM_PARA_INFO_T *)malloc(sizeof(DM_PARA_INFO_T));
                if(NULL == pDmParaInfo)
                {

    				if (pucFullName != NULL)
    				{
    					DM_FREE(pucFullName);
    				}
                    DM_FREE(udwIndex);
                    return 0;
                }
    			memset(pDmParaInfo,0,sizeof(DM_PARA_INFO_T));
    			pDmParaInfo->pDmHandler = pDmHandler;
    			pDmParaInfo->pucParamName = pucFullName;
    			pDmParaInfo->TransactionId = TransactionId;
    			pDmParaInfo->lId = lId;
    			
    			retCnt += DmGetDescendantNames(pDmParaInfo);
    			
    			pParaNameLast->pNext = pDmParaInfo->pParaNameFirst;
    			pParaNameLast = pDmParaInfo->pParaNameLast;
    			DM_FREE(pDmParaInfo);
    				
                DM_FREE(pucFullName);
            }

            DM_FREE(udwIndex);
        }        

    }
    /* if is a item, get nothing, only add itself*/
    else
    {
		pParaNameLast->pucName = strdup((char*)pucParamName);
		
		if(DmCheckRight(TransactionId, pDmHandler, RIGHT_W, lId))
		{
			pParaNameLast->bWritable = DM_TRUE;
		}
		else
		{
			pParaNameLast->bWritable = DM_FALSE;
		}
        retCnt += 1;
    }
	
	p->pParaNameLast = pParaNameLast;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmGetDescendantNames");	
    return retCnt;
}


INT32 DmGetDescendant(DM_GET_PARA_NAME_REQ_T *pGetParaNameReq)
{
    /* if path is "", get all the parameters */
    INT32 lCnt = 0;
    INT32 ret = DM_RET_SUCCESS;
    INT8 *pKeep = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmGetDescendant");


    /* if path is "", get all the parameters, means from the root node */
    if((pGetParaNameReq->pucParamName != NULL) 
       && (strlen((char*)pGetParaNameReq->pucParamName) >0))
    {
        pKeep = pGetParaNameReq->pucParamName;/* to make sure the memory can be freed */
        pGetParaNameReq->pucParamName = strdup(pKeep);
    }
    else
    {
        /*when "", set it to the root node's name */
        pKeep = pGetParaNameReq->pucParamName;/* to make sure the memory can be freed */
        pGetParaNameReq->pucParamName = strdup("Device.");//?? we need add an dm handler for default database in this case
    }


    DM_HANDLER_T   *pDmHandler = NULL;
    INT32           lId      = -1;

    pGetParaNameReq->pfnFree = DmFreeDmGetParaNameReq;  //yuqiangc 2012.02.13: we move it to here to prevent from NULL pointer being used


    pGetParaNameReq->pNameList = (PARAMETER_NAME_LIST_T *)malloc(sizeof(PARAMETER_NAME_LIST_T));
    if(NULL == pGetParaNameReq->pNameList)
    {
        DM_FREE(pGetParaNameReq->pucParamName);
        pGetParaNameReq->pucParamName = pKeep;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed for pGetParaNameReq->pNameList in DmGetDescendant");
        return DM_RET_INTERNAL_ERROR;
    }
    memset(pGetParaNameReq->pNameList,0,sizeof(PARAMETER_NAME_LIST_T));


    /*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pGetParaNameReq->pucParamName);

    if(NULL == pDmHandler)
    {
        DM_FREE(pGetParaNameReq->pucParamName);
        pGetParaNameReq->pucParamName = pKeep;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find dmhandler in DmGetDescendant");
        return DM_RET_INVALID_PARAM_NAME;
    }
    
    lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pGetParaNameReq->pucParamName);

    if(lId < 0)
    {
        DM_FREE(pGetParaNameReq->pucParamName);
        pGetParaNameReq->pucParamName = pKeep;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ param does not exist in DmGetDescendant");
        ret = DM_RET_INVALID_PARAM_NAME;
        return ret;
    }
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ before DmCheckRight() in DmGetDescendant");
    if (!DmCheckRight(pGetParaNameReq->TransactionId, pDmHandler, RIGHT_R, lId))
    {
        DM_FREE(pGetParaNameReq->pucParamName);
        pGetParaNameReq->pucParamName = pKeep;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ param not readable in DmGetDescendant");
        ret = DM_RET_INVALID_PARAM_NAME;
        return ret;
    }
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ after DmCheckRight() in DmGetDescendant");

	if((DM_TRUE == DmIsMultipleObject(pDmHandler->pPattern,lId))&&(DM_FALSE == DmIsSingleOfMultiple(pGetParaNameReq->pucParamName))
		&&(0 != strcmp(pGetParaNameReq->pucParamName+strlen(pGetParaNameReq->pucParamName)-5,".{i}.")))
	{
        INT8 *tmpName = (INT8*)malloc(strlen((char*)pGetParaNameReq->pucParamName)+ strlen("{i}.")+1);
        if(NULL == tmpName)
        {
            DM_FREE(pGetParaNameReq->pucParamName);
            pGetParaNameReq->pucParamName = pKeep;
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc() failed for tmpName in DmGetDescendant");
            return DM_RET_INTERNAL_ERROR;
        }
        memset((char*)tmpName, 0, strlen((char*)pGetParaNameReq->pucParamName)+ strlen("{i}.")+1);
        strcat((char*)tmpName, (char*)pGetParaNameReq->pucParamName);
        strcat((char*)tmpName, "{i}.");
		
        DM_FREE(pGetParaNameReq->pucParamName);
        pGetParaNameReq->pucParamName = tmpName;
	}

    //pGetParaNameReq->pfnFree = DmFreeDmGetParaNameReq;


	DM_PARA_INFO_T DmParaInfo = {0};

	DmParaInfo.pDmHandler = pDmHandler;
	DmParaInfo.pucParamName = pGetParaNameReq->pucParamName;
	DmParaInfo.TransactionId = pGetParaNameReq->TransactionId;
	DmParaInfo.lId = lId;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ before DmGetDescendantNames() in DmGetDescendant");


    lCnt = DmGetDescendantNames(&DmParaInfo);
        
    if(lCnt <= 0)
    {
        DM_FREE(pGetParaNameReq->pucParamName);
        pGetParaNameReq->pucParamName = pKeep;
        ret = DM_RET_INVALID_PARAM_NAME;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ no param names available in DmGetDescendant");
        return ret;
    }

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ after DmGetDescendantNames() in DmGetDescendant");


    /* add the count of self */
    pGetParaNameReq->pNameList->udwCnt = (UINT32)lCnt;


    pGetParaNameReq->pNameList->pucParamName = (PARAMETER_NAME_T *)malloc(sizeof(PARAMETER_NAME_T)*lCnt);
    if(NULL == pGetParaNameReq->pNameList->pucParamName)
    {
        DM_FREE(pGetParaNameReq->pNameList);
        DM_FREE(pGetParaNameReq->pucParamName);
        pGetParaNameReq->pucParamName = pKeep;
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ malloc failed for pGetParaNameReq->pNameList->pucParamName, leave DmGetDescendant");	
        ret = DM_RET_INTERNAL_ERROR;
        return ret;
    }
    memset(pGetParaNameReq->pNameList->pucParamName,0, sizeof(PARAMETER_NAME_T)*lCnt);

    UINT32 i;
    DM_PARA_NAME_T *pParaNameList = DmParaInfo.pParaNameFirst;
    DM_PARA_NAME_T *pTemp = NULL;

    for(i = 0; i < (UINT32)lCnt; i++)
    {
        pGetParaNameReq->pNameList->pucParamName[i].pucName = pParaNameList->pucName;
        pGetParaNameReq->pNameList->pucParamName[i].bWritable = pParaNameList->bWritable;
        pTemp = pParaNameList;
        pParaNameList = pParaNameList->pNext;
        DM_FREE(pTemp);
    }

    DM_FREE(pGetParaNameReq->pucParamName);
    pGetParaNameReq->pucParamName = pKeep;

	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmGetDescendant");	
    return ret;
}


INT32 DmCallAddHandler(DM_NODE_HANDLER_T *pNodeHandler, DM_HANDLER_PARA_T *pPara)
{
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmCallAddHandler");

	if(NULL == pNodeHandler || NULL == pPara)
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallAddHandler parameter error"); 
		return DM_RET_INTERNAL_ERROR;
	}
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmCallAddHandler name %s, value %s", pPara->pucName, pPara->pValue);
	}
    if(pNodeHandler->pAddBefHandler != NULL)
    {
        if(!pNodeHandler->pAddBefHandler(pPara))
        {
            return DM_RET_INTERNAL_ERROR;
        }
    }
    
    if(pNodeHandler->pAddCurHandler != NULL)
    {
        if(!pNodeHandler->pAddCurHandler(pPara)) /**< multiple object instance number will be set in this handler */
        {
            return DM_RET_INTERNAL_ERROR;
        }
    }
    else
    {
        return DM_RET_INTERNAL_ERROR;
    }
    
    if(pNodeHandler->pAddAftHandler!= NULL)
    {
        if(!pNodeHandler->pAddAftHandler(pPara)) 
        {
            return DM_RET_INTERNAL_ERROR;
        }
    }
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmCallAddHandler");	
    return DM_RET_SUCCESS;
}

BOOL8 DmCallDefaultAddHandler(DM_HANDLER_PARA_T  *pDmHandlerPara)
{
	BOOL8              bRtn;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultAddHandler pDmHandlerPara->pDbHandler=0x%x", pDmHandlerPara->pDbHandler);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultAddHandler pDmHandlerPara->TransactionId=%u", pDmHandlerPara->TransactionId);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultAddHandler pDmHandlerPara->ulParaId=%u", pDmHandlerPara->ulParaId);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultAddHandler pDmHandlerPara->ulCnt=%u", pDmHandlerPara->ulCnt);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultAddHandler pDmHandlerPara->bCInterface=%d", pDmHandlerPara->bCInterface);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultAddHandler pDmHandlerPara->bItem=%d", pDmHandlerPara->bItem);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultAddHandler pDmHandlerPara->pucName=%s", pDmHandlerPara->pucName);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultAddHandler pDmHandlerPara->pValue=0x%x", pDmHandlerPara->pValue);
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmCallDefaultAddHandler pDmHandlerPara->valueSize=%u", pDmHandlerPara->valueSize);

    bRtn = DefaultAddBefHandler(pDmHandlerPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultAddBefHandler failed!\n", pDmHandlerPara->pucName);
        DM_FREE(pDmHandlerPara->pucName);

        return DM_FALSE;
    }
    bRtn = DefaultAddCurHandler(pDmHandlerPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultAddCurHandler failed!\n", pDmHandlerPara->pucName);
        DM_FREE(pDmHandlerPara->pucName);

        return DM_FALSE;
    }
    bRtn = DefaultAddAftHandler(pDmHandlerPara);
    if (!bRtn)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ Para %s DefaultAddAftHandler failed!\n", pDmHandlerPara->pucName);
        DM_FREE(pDmHandlerPara->pucName);

        return DM_FALSE;
    }
	return bRtn;
}

INT32 DmAddObject(DM_HANDLER_PARA_T *pPara)
{
    DM_HANDLER_T   *pDmHandler = pPara->pDbHandler;
    DM_NODE_INFO_T *pPattern = pDmHandler->pPattern;
    INT32           lId      = pPara->ulParaId;
    INT32           i        = 0;
	char 		   *pucTemp = NULL;
	
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmAddObject %s", pPara->pucName);
    if(DM_RET_SUCCESS != DmCallAddHandler(pPattern[lId].pNodeHandler,pPara)) /**< call add handler */
    {
    	return DM_RET_INTERNAL_ERROR;
    }

	pucTemp = (char*)(pPara->pValue);
	INT8 *pucChildObjName = calloc(strlen((char*)(pPara->pucName))+strlen(pucTemp)+2, sizeof(INT8));
    if(NULL == pucChildObjName)
    {
        return DM_RET_INTERNAL_ERROR;
    }
	INT8 *pucInstancePath = calloc(strlen((char*)(pPara->pucName))+strlen(pucTemp)+2, sizeof(INT8));
    if(NULL == pucInstancePath)
    {
		if (pucChildObjName != NULL)
		{
			DM_FREE(pucChildObjName);
		}

        return DM_RET_INTERNAL_ERROR;
    }
	strcpy((char*)pucChildObjName, (char*)(pPara->pucName));
	if(isdigit(pucTemp[1]))
	{
		strcat((char*)pucChildObjName,pucTemp);
		strcat((char*)pucChildObjName,".");
	}
	
	strcpy(pucInstancePath, (char*)(pPara->pucName));
	if(isdigit(pucTemp[1]))
	{
		strcat(pucInstancePath,pucTemp+1);
		strcat(pucInstancePath,".");
	}

	
	xmlXPathContextPtr	xpathCtx = NULL;
	if((pPattern[lId+1].ulParentIdx == pPara->ulParaId)&&(DM_TRUE != pDmHandler->pDbIsObjectFunc(pPattern[lId+1].ucParaType)))
	{
		xpathCtx = xmlXPathNewContext(pPara->pDbHandler->pXmlDocument);
		if(NULL == xpathCtx)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler can't get xpathCtx");

			if (pucChildObjName != NULL)
			{
				DM_FREE(pucChildObjName);
			}

			if (pucInstancePath != NULL)
			{
				DM_FREE(pucInstancePath);
			}

			return DM_FALSE;
		}
		INT8 *ucXPathName = NULL;
		ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

		if (ucXPathName == NULL)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DmAddObject : ucXPathName = NULL");

			if (pucChildObjName != NULL)
			{
				DM_FREE(pucChildObjName);
			}

			if (pucInstancePath != NULL)
			{
				DM_FREE(pucInstancePath);
			}

			if (xpathCtx != NULL)
			{
				DM_FREE(xpathCtx);
			}

			return DM_FALSE;
		}

		memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);


		ConvertPathToXPath(pucChildObjName, ucXPathName);
		pPara->pDbHandler->xpathParentObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);
		DM_FREE(ucXPathName);
	}


    for(i = lId+1; i < pDmHandler->lPatternLength; i++)
    {
        if(pPattern[i].ulParentIdx == pPara->ulParaId)
        {
            if(DM_TRUE != pDmHandler->pDbIsObjectFunc(pPattern[i].ucParaType))
            {
		        INT8 aucName[256] = {0};
                DM_HANDLER_PARA_T DmHandlerPara = {0};

                osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

                DmHandlerPara.TransactionId = pPara->TransactionId;
                DmHandlerPara.ulParaId    = i;
                DmHandlerPara.pDbHandler = pDmHandler;
                DmHandlerPara.pucName     = aucName;
				DmHandlerPara.bCInterface = DM_FALSE;
                DmHandlerPara.valueSize = sizeof(aucName);

                //get name
                strcpy(aucName, pucChildObjName);
                strcat(aucName, (char*)pPattern[i].pucNodeName);

                //get default value
				INT8 *pucInsNumSeq = DmGetInstanceNumSequence(aucName);
				DmHandlerPara.ulCnt = 1;
				DmHandlerPara.pValue = DmGetDefaultValue(pucInsNumSeq,pPattern[i].pDefaultValue);
				if(NULL != pucInsNumSeq)
				{
					DM_FREE(pucInsNumSeq);
				}

				
                if(DM_RET_SUCCESS != DmCallAddHandler(pPattern[i].pNodeHandler,&DmHandlerPara)) /**< call add handler */
                {
                    if(NULL != DmHandlerPara.pValue)
                    {
                        DM_FREE(DmHandlerPara.pValue);
                    }
					DM_FREE(pucChildObjName);
					DM_FREE(pucInstancePath);							
                    return DM_RET_INTERNAL_ERROR;
                }
                if(NULL != DmHandlerPara.pValue)
                {
                    DM_FREE(DmHandlerPara.pValue);
                }

            }
            else
            {
                if(0 != strcmp((char*)&(pPattern[i].pucNodeName[strlen((char*)pPattern[i].pucNodeName)-5]), ".{i}."))
                {/* single object */
                    UINT8 aucValue[MAX_INSTANCE_NUMBER_SIZE] = {0};
                    DM_HANDLER_PARA_T DmHandlerPara = {0};

					osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

                    DmHandlerPara.TransactionId = pPara->TransactionId;
                    DmHandlerPara.ulParaId    = i;
                    DmHandlerPara.pDbHandler = pDmHandler;
                    DmHandlerPara.pucName     = DmGetFullPathOfDigit(pPattern[i].pucNodeName,pucInstancePath);
					DmHandlerPara.ulCnt = 1;
                    DmHandlerPara.pValue = aucValue;
					DmHandlerPara.bCInterface = DM_FALSE;
                    DmHandlerPara.valueSize = sizeof(aucValue);
					
                    if(DM_RET_SUCCESS !=DmAddObject(&DmHandlerPara))
                    {   
                        DM_FREE(DmHandlerPara.pucName);
						DM_FREE(pucChildObjName);
						DM_FREE(pucInstancePath);							
                        return DM_RET_INTERNAL_ERROR;
                    }
                    else
                    {
                        DM_FREE(DmHandlerPara.pucName);
                    }
                }
                else
                {
                    /** for multiple object, we don't add any instance for it */
                }
            }
        }
        else
        {
            continue;
        }
    }

	if(NULL != xpathCtx)
	{
		xmlXPathFreeObject(pPara->pDbHandler->xpathParentObj);
		xmlXPathFreeContext(xpathCtx);
		pPara->pDbHandler->xpathParentObj = NULL;
	}
	DM_FREE(pucChildObjName);
	DM_FREE(pucInstancePath);	

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmAddObject");	
    return DM_RET_SUCCESS;
}




INT32 DmAddObjectWithInstance(DM_HANDLER_PARA_T *pPara)
{
    DM_HANDLER_T   *pDmHandler = pPara->pDbHandler;
    DM_NODE_INFO_T *pPattern = pDmHandler->pPattern;
    INT32           lId      = pPara->ulParaId;
    INT32           i        = 0;
	char 		   pucTemp[16] = {0};
	
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmAddObject %s", pPara->pucName);
	
	if(DM_TRUE != DbAddCurHandlerWithInstance(pPara)) /**< call add handler */
    {
    	return DM_RET_INTERNAL_ERROR;
    }

	snprintf(pucTemp,sizeof(pucTemp),"i%d",*(UINT32*)(pPara->pValue));
	
	INT8 *pucChildObjName = calloc(strlen((char*)(pPara->pucName))+strlen(pucTemp)+2, sizeof(INT8));
    if(NULL == pucChildObjName)
    {
        return DM_RET_INTERNAL_ERROR;
    }
	INT8 *pucInstancePath = calloc(strlen((char*)(pPara->pucName))+strlen(pucTemp)+2, sizeof(INT8));
    if(NULL == pucInstancePath)
    {
		if (pucChildObjName != NULL)
		{
			DM_FREE(pucChildObjName);
		}

        return DM_RET_INTERNAL_ERROR;
    }
	strcpy(pucChildObjName, (char*)(pPara->pucName));
	if(isdigit(pucTemp[1]))
	{
		strcat(pucChildObjName,pucTemp);
		strcat(pucChildObjName,".");
	}

	strcpy((char*)pucInstancePath, (char*)(pPara->pucName));
	if(isdigit(pucTemp[1]))
	{
		strcat((char*)pucInstancePath,pucTemp+1);
		strcat((char*)pucInstancePath,".");
	}

	
/* cyq add begin */
	xmlXPathContextPtr	xpathCtx = NULL;
	if((pPattern[lId+1].ulParentIdx == pPara->ulParaId)&&(DM_TRUE != pDmHandler->pDbIsObjectFunc(pPattern[lId+1].ucParaType)))
	{
		xpathCtx = xmlXPathNewContext(pPara->pDbHandler->pXmlDocument);
		if(NULL == xpathCtx)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler can't get xpathCtx");

			if (pucChildObjName != NULL)
			{
				DM_FREE(pucChildObjName);
			}

			if (pucInstancePath != NULL)
			{
				DM_FREE(pucInstancePath);
			}

			return DM_FALSE;
		}
//		INT8	ucXPathName[MAX_XML_PATH] = {0};

		INT8 *ucXPathName = NULL;
		ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

		if (ucXPathName == NULL)
		{

			if (pucChildObjName != NULL)
			{
				DM_FREE(pucChildObjName);
			}

			if (pucInstancePath != NULL)
			{
				DM_FREE(pucInstancePath);
			}

			return DM_FALSE;
		}

		memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

		ConvertPathToXPath(pucChildObjName, ucXPathName);
		pPara->pDbHandler->xpathParentObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

		DM_FREE(ucXPathName);
		
	}

/* cyq add end */

    for(i = lId+1; i < pDmHandler->lPatternLength; i++)
    {
        if(pPattern[i].ulParentIdx == pPara->ulParaId)
        {
            if(DM_TRUE != pDmHandler->pDbIsObjectFunc(pPattern[i].ucParaType))
            {
                INT8 aucName[256] = {0};
                DM_HANDLER_PARA_T DmHandlerPara = {0};

                osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

                DmHandlerPara.TransactionId = pPara->TransactionId;
                DmHandlerPara.ulParaId    = i;
                DmHandlerPara.pDbHandler = pDmHandler;
                DmHandlerPara.pucName     = aucName;
				DmHandlerPara.bCInterface = DM_FALSE;
                DmHandlerPara.valueSize = sizeof(aucName);

                //get name
                strcpy((char*)aucName, pucChildObjName);
                strcat((char*)aucName, (char*)pPattern[i].pucNodeName);

                //get default value
                #if 0
                if(NULL != pPattern[i].pDefaultValue && strlen((char*)(pPattern[i].pDefaultValue)) >0)
                {
                    /*the value should not include the [, \,,  ]*/
                    INT32 start = 0;
                    INT32 end = 0;
                    INT32 j = 0;
                    UINT8 *pucTmp =  pPattern[i].pDefaultValue;
                    
                    /*get the first position for start*/
                    while(*pucTmp == '[' || *pucTmp == '"')
                    {
                        j++;
                        pucTmp ++;
                    }
                    start = i;
                    /*get the second position for end*/
                    while(*pucTmp != '\0' &&*pucTmp != ']' && *pucTmp != ','&& *pucTmp !='\"')
                    {
                        j++;
                        pucTmp ++;
                    }
                    end = j;
                    /*copy the default value */
					DmHandlerPara.ulCnt = 1;
                    DmHandlerPara.pValue = (UINT8*)strndup((char*)((pPattern[i].pDefaultValue)+start), end-start);
                }
				#endif

				INT8 *pucInsNumSeq = DmGetInstanceNumSequence(aucName);
				DmHandlerPara.ulCnt = 1;
				DmHandlerPara.pValue = DmGetDefaultValue(pucInsNumSeq,pPattern[i].pDefaultValue);
				if(NULL != pucInsNumSeq)
				{
					DM_FREE(pucInsNumSeq);
				}

				
                //call handler
                if(DM_RET_SUCCESS != DmCallAddHandler(pPattern[i].pNodeHandler,&DmHandlerPara)) /**< call add handler */
                {
                    if(NULL != DmHandlerPara.pValue)
                    {
                        DM_FREE(DmHandlerPara.pValue);
                    }
					DM_FREE(pucChildObjName);
					DM_FREE(pucInstancePath);
                    return DM_RET_INTERNAL_ERROR;
                }
                if(NULL != DmHandlerPara.pValue)
                {
                    DM_FREE(DmHandlerPara.pValue);
                }

            }
            else
            {
                if(0 != strcmp((char*)&(pPattern[i].pucNodeName[strlen((char*)pPattern[i].pucNodeName)-5]), ".{i}."))
                {/* single object */
                    //DmAddObject();
                    UINT8 aucValue[MAX_INSTANCE_NUMBER_SIZE] = {0};
                    DM_HANDLER_PARA_T DmHandlerPara = {0};

                    osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

                    DmHandlerPara.TransactionId = pPara->TransactionId;
                    DmHandlerPara.ulParaId    = i;
                    DmHandlerPara.pDbHandler = pDmHandler;
                    DmHandlerPara.pucName     = DmGetFullPathOfDigit(pPattern[i].pucNodeName,pucInstancePath);
					DmHandlerPara.ulCnt = 1;
                    DmHandlerPara.pValue = aucValue;
					DmHandlerPara.bCInterface = DM_FALSE;
                    DmHandlerPara.valueSize = sizeof(aucValue);
					
                    if(DM_RET_SUCCESS !=DmAddObject(&DmHandlerPara))
                    {   
                        DM_FREE(DmHandlerPara.pucName);
						DM_FREE(pucChildObjName);
						DM_FREE(pucInstancePath);
                        return DM_RET_INTERNAL_ERROR;
                    }
                    else
                    {
                        DM_FREE(DmHandlerPara.pucName);
                    }
                }
                else
                {
                    /** for multiple object, we don't add any instance for it */
                }
            }
        }
        else
        {
            continue;
        }
    }

/* cyq add begin */
	if(NULL != xpathCtx)
	{
		xmlXPathFreeObject(pPara->pDbHandler->xpathParentObj);
		xmlXPathFreeContext(xpathCtx);
		pPara->pDbHandler->xpathParentObj = NULL;
	}
	DM_FREE(pucChildObjName);
	DM_FREE(pucInstancePath);
/* cyq add end */

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmAddObject");	
    return DM_RET_SUCCESS;
}




/**
 * Module A can use this API to add an object to DB file. The result will return synchronously.
 * @param pAddObjectReq the object instance to be added
 * @return 0: success   other: failure
 */
INT32   DmAddObjectReqWithInstance(DM_ADD_OBJECT_REQ_T *pAddObjectReq)
{

    DM_HANDLER_T   *pDmHandler = NULL;
    INT32           lId      = -1;
    
    //UINT8 aucValue[MAX_INSTANCE_NUMBER_SIZE] = {0};
    
    if(NULL == pAddObjectReq || NULL == pAddObjectReq->pucObjName)
    {
        //DmError("DmAddObjectReq input para pointer is NULL");
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmAddObjectReq parameter error");
        return -1;
    }
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmAddObjectReq %s", pAddObjectReq->pucObjName);
	}

    /*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pAddObjectReq->pucObjName);

    if(NULL == pDmHandler)
    {
        return DM_RET_INVALID_PARAM_NAME;
    }
    lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pAddObjectReq->pucObjName);
    if(lId < 0)
    {
        return DM_RET_INVALID_PARAM_NAME;
    }

    /*check right*/
    if(!DmCheckRight(pAddObjectReq->TransactionId, pDmHandler, RIGHT_W, lId))
    {
        return DM_RET_REQUEST_DENIED;
    }

    if(DbGetObjectNum(pDmHandler,pAddObjectReq->pucObjName) >= atoi((char*)pDmHandler->pPattern[lId].pValueRange))
    {
        return DM_RET_RESOURCE_EXCEEDED;
    }

    /////////////////////////////

    DM_HANDLER_PARA_T *pPara = (DM_HANDLER_PARA_T*)calloc(1, sizeof(DM_HANDLER_PARA_T));

	if(pPara == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmAddObjectReqWithInstance : pPara = NULL"); 

		return DM_RET_INTERNAL_ERROR;
	}

    osa_mem_set(pPara,0,sizeof(DM_HANDLER_PARA_T));

    pPara->TransactionId = pAddObjectReq->TransactionId;
    pPara->ulParaId   = lId;
    pPara->pDbHandler = pDmHandler;
    pPara->pucName    = pAddObjectReq->pucObjName;
	pPara->ulCnt = 1;
    pPara->pValue   = &pAddObjectReq->udwInstanceNum;
	pPara->bCInterface = DM_FALSE;
    pPara->valueSize = sizeof(pAddObjectReq->udwInstanceNum);



    if(DM_RET_SUCCESS == DmAddObjectWithInstance(pPara))
    {
        DM_FREE(pPara);
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmAddObjectReq");			
        return DM_RET_SUCCESS;
    }
    else
    {
        DM_FREE(pPara);
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmAddObjectReq");			
        return DM_RET_INTERNAL_ERROR;
    }
}


BOOL8 DmXmlFileGenerate(DM_HANDLER_PARA_T *pPara,BOOL8 bCallObjAddHanler)
{
    DM_HANDLER_T   *pDmHandler = NULL;
    DM_NODE_INFO_T *pPattern = NULL;
    INT32           lId      = 0;
    INT32           i        = 0;
	INT8           *pucName = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmXmlFileGenerate");
	if(NULL == pPara)
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ DmXmlFileGenerate parameter error null para");    
	    return DM_FALSE;
	}
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmXmlFileGenerate name %s id %d\n", pPara->pucName, pPara->ulParaId);
	}

    pDmHandler = pPara->pDbHandler;
    pPattern = pDmHandler->pPattern;
    lId      = pPara->ulParaId;

	if(DM_TRUE == bCallObjAddHanler)
	{
		if(DM_RET_SUCCESS != DmCallAddHandler(pPattern[lId].pNodeHandler,pPara)) /**< call add handler */
	    {
	        DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmXmlFileGenerate error of call DmCallAddhandler for add bCallObj\n");
	    	return DM_FALSE;
	    }
	}

/* cyq add begin */
	xmlXPathContextPtr	xpathCtx = NULL;
	if((pPattern[lId+1].ulParentIdx == pPara->ulParaId)&&(DM_TRUE != pDmHandler->pDbIsObjectFunc(pPattern[lId+1].ucParaType)))
	{
		xpathCtx = xmlXPathNewContext(pPara->pDbHandler->pXmlDocument);
		if(NULL == xpathCtx)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler can't get xpathCtx");
			return DM_FALSE;
		}
//		INT8	ucXPathName[MAX_XML_PATH] = {0};

		INT8 *ucXPathName = NULL;
		ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

		if (ucXPathName == NULL)
		{
			DM_ERROR(DM_ERR_MEMORY_ALLOCATION_FAILED, "DB Trace ------ DmXmlFileGenerate : ucXPathName = NULL");
			return DM_FALSE;

		}

		memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

		ConvertPathToXPath(pPara->pucName, ucXPathName);
		pPara->pDbHandler->xpathParentObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

		DM_FREE(ucXPathName);
	}
/* cyq add end */
	
    for(i = lId+1; i < pDmHandler->lPatternLength; i++)
    {
        if(pPattern[i].ulParentIdx == pPara->ulParaId)
        {
            /*if not object*/
            if(DM_TRUE != pDmHandler->pDbIsObjectFunc(pPattern[i].ucParaType))
            {
            	
                INT8 aucName[256] = {0};
                DM_HANDLER_PARA_T DmHandlerPara = {0};

                osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

                DmHandlerPara.TransactionId = pPara->TransactionId;
                DmHandlerPara.ulParaId    = i;
                DmHandlerPara.pDbHandler = pDmHandler;
                DmHandlerPara.pucName = aucName;
				DmHandlerPara.bCInterface = DM_FALSE;
                DmHandlerPara.valueSize = sizeof(aucName);

                //get name
                strcpy((char*)aucName, (char*)(pPara->pucName));
				//pucName = DmGetNodeNameFromPath(pPattern[i].pucNodeName);
				pucName = pPattern[i].pucNodeName;
                strcat((char*)aucName, (char*)pucName);
                //DM_FREE(pucName);
                //get default value

				INT8 *pucInsNumSeq = DmGetInstanceNumSequence(aucName);
				DmHandlerPara.ulCnt = 1;
				DmHandlerPara.pValue = DmGetDefaultValue(pucInsNumSeq,pPattern[i].pDefaultValue);
				if(NULL != pucInsNumSeq)
				{
					DM_FREE(pucInsNumSeq);
				}

                //call handler

                if(DM_RET_SUCCESS != DmCallAddHandler(pPattern[i].pNodeHandler,&DmHandlerPara)) /**< call add handler */
                {
                    if(NULL != DmHandlerPara.pValue)
                    {
                        DM_FREE(DmHandlerPara.pValue);
                    }
					DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmXmlFileGenerate error of call DmCallAddhandler for add item %s\n", DmHandlerPara.pucName);
                    return DM_FALSE;
                }

                if(NULL != DmHandlerPara.pValue)
                {
                    DM_FREE(DmHandlerPara.pValue);
                }

            }
            else  /*if  object*/
            {
                if(0 != strcmp((char*)&(pPattern[i].pucNodeName[strlen((char*)pPattern[i].pucNodeName)-5]), ".{i}.")) /* single object */
                {
                    DM_HANDLER_PARA_T DmHandlerPara = {0};

                    osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

                    DmHandlerPara.TransactionId = pPara->TransactionId;
                    DmHandlerPara.ulParaId    = i;
                    DmHandlerPara.pDbHandler = pDmHandler;
                    DmHandlerPara.pucName     = DmGetFullPathOfDigit(pPattern[i].pucNodeName,pPara->pucName);
					DmHandlerPara.bCInterface = DM_FALSE;
                    DmHandlerPara.valueSize = 0;
                    DmHandlerPara.pValue = NULL;

                    if(DM_TRUE != DmXmlFileGenerate(&DmHandlerPara,DM_TRUE))
                    {   
                        
						DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmXmlFileGenerate error of call DmXmlFileGenerate add object %s\n", DmHandlerPara.pucName);
						DM_FREE(DmHandlerPara.pucName);
                        return DM_FALSE;
                    }
                    else
                    {
                        DM_FREE(DmHandlerPara.pucName);
                    }
                }
                else /* multiple object */
                {
					//UINT32 udwIndex[DM_MAXOBJ_NUM+1] = {0};
					INT8 *pucFullName = NULL;
                    UINT8  aucValue[MAX_INSTANCE_NUMBER_SIZE] = {0};
					UINT32 j = 0;

					UINT32 ulMultiObjCnt = 0;
					INT8 *pucInsNumSeq = NULL;
					INT8 *pDefaultValue = NULL;

                    pucInsNumSeq = DmGetInstanceNumSequence(pPara->pucName);

                    pDefaultValue = DmGetDefaultValue(pucInsNumSeq,pPattern[i].pDefaultValue);

                    if (pDefaultValue == NULL)
                    {
                        DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DB Trace ------ DmXmlFileGenerate : pDefaultValue = NULL");
                        DM_FREE(pucInsNumSeq);
                        return DM_FALSE;
                    }

					ulMultiObjCnt = atoi((char*)(pDefaultValue));

					DM_FREE(pucInsNumSeq);
					DM_FREE(pDefaultValue);

					if(0 == ulMultiObjCnt)
					{
						continue;
					}


					//pPara->pucName 			a.1.b.
					//pPattern[i].pucNodeName	a.{i}.b.c.{i}.
				    
					pucFullName = (INT8 *)malloc(sizeof(INT8)*(strlen((char*)pPattern[i].pucNodeName)+9));

					if(NULL == pucFullName)
					{
					    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmXmlFileGenerate allocate memory failure for multiple obj");
						return DM_FALSE;
					}
					memset(pucFullName,0,sizeof(UINT8)*(strlen((char*)(pPattern[i].pucNodeName))+9));
					strncpy((char*)(pucFullName) ,(char*)(pPattern[i].pucNodeName),strlen((char*)(pPattern[i].pucNodeName))-4);

					INT8 *pTemp = DmGetFullPathOfDigit(pucFullName,pPara->pucName);
					memset(pucFullName,0,sizeof(UINT8)*(strlen((char*)(pPattern[i].pucNodeName))+9));
					strcpy((char*)pucFullName,(char*)pTemp);
					DM_FREE(pTemp);
					
					
					/*add this object self*/
					DM_HANDLER_PARA_T DmHandlerPara = {0};

                    osa_mem_set(&DmHandlerPara,0,sizeof(DM_HANDLER_PARA_T));

					DmHandlerPara.TransactionId = pPara->TransactionId;
					DmHandlerPara.ulParaId	 = i;
					DmHandlerPara.pDbHandler = pDmHandler;
					DmHandlerPara.pucName = pucFullName;
					DmHandlerPara.ulCnt = 1;
                    DmHandlerPara.pValue = aucValue;
					DmHandlerPara.bCInterface = DM_FALSE;
                    DmHandlerPara.valueSize = sizeof(aucValue);

					for(j = 0; j < ulMultiObjCnt; j++)
					{
						//call handler

						if(DM_RET_SUCCESS != DmCallAddHandler(pPattern[i].pNodeHandler,&DmHandlerPara)) /**< call add handler */
						{   
						    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmXmlFileGenerate call DmCallAddHandler failure for multiple obj %s", DmHandlerPara.pucName);
                            if(NULL != pucFullName)
							{
						        DM_FREE(pucFullName);
							}
							return DM_FALSE;
						}
                        INT8 *pucChildObjName = calloc(strlen((char*)(DmHandlerPara.pucName))+strlen((char*)(DmHandlerPara.pValue))+2, sizeof(INT8));

						if (pucChildObjName == NULL)
						{
							DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmXmlFileGenerate : pucChildObjName = NULL");

							DM_FREE(pucFullName);

							return DM_FALSE;
						}

						INT8 *pucTmp = DmHandlerPara.pucName;
						strcpy((char*)pucChildObjName, (char*)(DmHandlerPara.pucName));
						strcat((char*)pucChildObjName,(char*)(DmHandlerPara.pValue));
                        strcat((char*)pucChildObjName,".");
						DmHandlerPara.pucName = pucChildObjName;
						DmXmlFileGenerate(&DmHandlerPara,DM_FALSE);
						DmHandlerPara.pucName = pucTmp; 
						DM_FREE(pucChildObjName);

					}
					DM_FREE(pucFullName);
                }

            }
        }
        else
        {
            continue;
        }
    }


	if(NULL != xpathCtx)
	{
    	xmlXPathFreeObject(pPara->pDbHandler->xpathParentObj);
		xmlXPathFreeContext(xpathCtx);
		pPara->pDbHandler->xpathParentObj = NULL;
	}

	
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmXmlFileGenerate");	
    return DM_TRUE;
}


INT32 DmXmlFileGenerateReq(INT8 *pucRootNode)
{
    DM_HANDLER_T   *pDmHandler = NULL;
    INT32           lId      = 0;
	INT8          *pucRootName = NULL;
    UINT8 aucValue[MAX_INSTANCE_NUMBER_SIZE] = {0};
    INT32           lRet = DM_FAILURE;
    DM_TRANSACTION_REQ_T transReq;
    DM_NOTIFY_ID_LIST_T *pNotifyIdList = NULL;
    DM_NOTIFY_ID_LIST_T *pNotifyIdTmp = NULL;
    INT8             opDefaultFile[128] = {0};

    if(NULL == pucRootNode)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ root-node is null, leave DmXmlFileGenerateReq");
        return lRet;
    }

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmXmlFileGenerateReq root-node=%s",pucRootNode);

	pucRootName = calloc(strlen(pucRootNode)+2, sizeof(UINT8));
	if(NULL == pucRootName)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmXmlFileGenerateReq memory allocate failure");	
	    return lRet;
	}
    strcpy((char*)pucRootName, pucRootNode);
	strcat((char*)pucRootName, ".");

    /*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pucRootNode);

    if(NULL == pDmHandler)
    {
    	DM_FREE(pucRootName);
        return lRet;
    }

    lId = DmGetIDFromPath(&(pDmHandler->hashNameID), pucRootName);
    if(lId < 0)
    {
        DM_FREE(pucRootName);
        return lRet;
    }


    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));

    strcpy(transReq.AccessEntity, "ROOT");
    strcpy(transReq.RootNodeToLock,pucRootName);

    transReq.timeout = 0;
    transReq.DataFlush = -1;
    transReq.bNotCallNotify = DM_TRUE;
    transReq.TransactionId = 0;

    lRet = DmTransactionStartReq(&transReq);

    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmXmlFileGenerateReq : DmTransactionStartReq failed lRet=%d",lRet);	
        DM_FREE(pucRootName);
        return lRet;
    }

    DM_HANDLER_PARA_T *pPara = (DM_HANDLER_PARA_T*)calloc(1, sizeof(DM_HANDLER_PARA_T));

	if (pPara == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmXmlFileGenerateReq : pPara = NULL");	
		DM_FREE(pucRootName);
	    return lRet;

	}

    osa_mem_set(pPara,0,sizeof(DM_HANDLER_PARA_T));

    pPara->TransactionId = transReq.TransactionId;
    pPara->ulParaId   = lId;
    pPara->pDbHandler = pDmHandler;
    pPara->pucName    = pucRootName;
    pPara->ulCnt = 1;
    pPara->pValue   = aucValue;
	pPara->bCInterface = DM_FALSE;
    pPara->valueSize = sizeof(aucValue);

    if(DM_TRUE == DmXmlFileGenerate(pPara,DM_TRUE))
    {
        DM_FREE(pPara);
        lRet =  DM_SUCCESS;
    }
    else
    {
        DM_FREE(pPara);
        lRet =  DM_FAILURE;
    }

    pDmHandler->bNeedBak = DM_FALSE;

    /* clear notify list if any */
    pNotifyIdList = pDmHandler->pNotifyIdList;
    pDmHandler->pNotifyIdList = NULL;
    while (pNotifyIdList != NULL)
    {
        pNotifyIdTmp = pNotifyIdList;
        pNotifyIdList = pNotifyIdList->pNext;

        DM_FREE(pNotifyIdTmp->notifyPara.pucPath);
        DM_FREE(pNotifyIdTmp);
    }

    if (pDmHandler->bExtDb==DM_FALSE)
    {
        /* apply hardware-default-value to new xml file */
        snprintf(opDefaultFile,sizeof(opDefaultFile),"%s.hardware",pDmHandler->pLoadPath);

        if (access(opDefaultFile,F_OK)==0)
        {
            DmApplyDefaultValueFile(pDmHandler, opDefaultFile, transReq.TransactionId, pucRootName, DM_DEFAULT_FILE__HARDWARE);
        }
        else
        {
            snprintf(opDefaultFile,sizeof(opDefaultFile),"%s.hardware",pDmHandler->pBackupPath);
            if (access(opDefaultFile,F_OK)==0)
            {
                DmApplyDefaultValueFile(pDmHandler, opDefaultFile, transReq.TransactionId, pucRootName, DM_DEFAULT_FILE__HARDWARE);
            }
        }

        /* apply operator-default-value to new xml file */
        snprintf(opDefaultFile,sizeof(opDefaultFile),"%s.default",pDmHandler->pLoadPath);

        if (access(opDefaultFile,F_OK)==0)
        {
            DmApplyDefaultValueFile(pDmHandler, opDefaultFile, transReq.TransactionId, pucRootName, DM_DEFAULT_FILE__OPERATOR);
        }
        else
        {
            snprintf(opDefaultFile,sizeof(opDefaultFile),"%s.default",pDmHandler->pBackupPath);
            if (access(opDefaultFile,F_OK)==0)
            {
                DmApplyDefaultValueFile(pDmHandler, opDefaultFile, transReq.TransactionId, pucRootName, DM_DEFAULT_FILE__OPERATOR);
            }
        }
    }

    DM_FREE(pucRootName);

    DmTransactionStopReq(transReq.TransactionId);

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmXmlFileGenerateReq, root-node=%s",pucRootNode);	
	return lRet;
}

INT32 DmApplyDefaultValueFile(DM_HANDLER_T *pDmHandler, INT8 *pDefaultFile, UINT32 TransactionId, INT8 *pucRootNode, DM_DEFAULT_FILE_TYPE defaultType)
{
    INT32 lRet = 0;
    UINT32 cnt = 0;
    xmlDocPtr pXmlDocument = NULL;
    INT8 *pucFilePath = NULL;
    DM_GET_PARA_VALUE_REQ_T getAllValues;
    DM_SET_PARA_VALUE_REQ_T setOneValue;
    PARAMETER_NAME_LIST_T nameList;
    PARAMETER_NAME_T paramName;
    PARAMETER_VALUE_LIST_T valueList;
    PARAMETER_VALUE_T paramValue;
    UINT16 faultCode = 0;
    INT32 lIndex = -1;

    if (pDmHandler==NULL || pDefaultFile == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ null arguments, just leave DmApplyDefaultValueFile");
        return DM_FAILURE;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmApplyDefaultValueFile %s",pDefaultFile);

    pXmlDocument = pDmHandler->pXmlDocument;
    pucFilePath = pDmHandler->pucFilePath;

    pDmHandler->pucFilePath = pDefaultFile;
    pDmHandler->pXmlDocument = xmlReadFile(pDmHandler->pucFilePath, NULL, XML_PARSE_NOBLANKS);

    if (pDmHandler->pXmlDocument == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ bad default-file %s, leave DmApplyDefaultValueFile",pDefaultFile);

        pDmHandler->pXmlDocument = pXmlDocument;
        pDmHandler->pucFilePath = pucFilePath;
        return DM_FAILURE;
    }

    osa_mem_set(&getAllValues, 0, sizeof(DM_GET_PARA_VALUE_REQ_T));
    osa_mem_set(&nameList, 0, sizeof(PARAMETER_NAME_LIST_T));
    osa_mem_set(&paramName, 0, sizeof(PARAMETER_NAME_T));

    getAllValues.TransactionId = TransactionId;
    getAllValues.pNameList = &nameList;
    getAllValues.pNameList->udwCnt = 1;
    getAllValues.pNameList->pucParamName = &paramName;
    getAllValues.pNameList->pucParamName[0].pucName = pucRootNode;
    getAllValues.pvList = NULL;
    getAllValues.pfnFree = NULL;

    lRet = DmGetParaValueReq(&getAllValues);

    if (lRet != DM_RET_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmGetParaValueReq failed lRet=%d, leave DmApplyDefaultValueFile",lRet);
        getAllValues.pfnFree(&getAllValues);

        pDmHandler->pXmlDocument = pXmlDocument;
        pDmHandler->pucFilePath = pucFilePath;
        return DM_FAILURE;
    }
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DmGetParaValueReq fetch %d params from old db xml, in DmApplyDefaultValueFile",getAllValues.pvList->udwCnt);
    }

    xmlFreeDoc(pDmHandler->pXmlDocument);
    pDmHandler->pXmlDocument = NULL;

    /* switch to new file */
    pDmHandler->pXmlDocument = pXmlDocument;
    pDmHandler->pucFilePath = pucFilePath;

    osa_mem_set(&setOneValue, 0, sizeof(DM_SET_PARA_VALUE_REQ_T));
    osa_mem_set(&valueList, 0, sizeof(PARAMETER_VALUE_LIST_T));
    osa_mem_set(&paramValue, 0, sizeof(PARAMETER_VALUE_T));

    setOneValue.TransactionId = TransactionId;
    setOneValue.pFaultCodeList = &faultCode;
    setOneValue.bCreateIfNotExit = DM_FALSE;
    setOneValue.pvList = &valueList;
    setOneValue.pvList->udwCnt = 1;
    setOneValue.pvList->pstParamValue = &paramValue;
    
    for (cnt=0;cnt<getAllValues.pvList->udwCnt;cnt++)
    {
        faultCode = 0;
        lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID), getAllValues.pvList->pstParamValue[cnt].pucName);

        if(lIndex < 0)
        {
            continue;
        }

        if (defaultType==DM_DEFAULT_FILE__HARDWARE && pDmHandler->pPattern[lIndex].bUseHardwareDefault==DM_TRUE)
        {
            setOneValue.bCreateIfNotExit = DM_FALSE;
        }
        else if (defaultType==DM_DEFAULT_FILE__OPERATOR && pDmHandler->pPattern[lIndex].bUseOperatorDefault==DM_TRUE)
        {
            setOneValue.bCreateIfNotExit = DM_FALSE;
        }
        else
        {
            continue;
        }

        setOneValue.pvList->pstParamValue[0].pucName = getAllValues.pvList->pstParamValue[cnt].pucName;
        setOneValue.pvList->pstParamValue[0].pucValue = getAllValues.pvList->pstParamValue[cnt].pucValue;

        if(setOneValue.pvList->pstParamValue[0].pucValue)
		{
			lRet = DmSetParaValueReq(&setOneValue);

            if (lRet != DM_RET_SUCCESS)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmSetParaValueReq %s = %s failed lRet=%d faultCode=%d, skip it in DmApplyDefaultValueFile()",setOneValue.pvList->pstParamValue[0].pucName,setOneValue.pvList->pstParamValue[0].pucValue,lRet,faultCode);
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmSetParaValueReq %s = %s successfully in DmApplyDefaultValueFile()",setOneValue.pvList->pstParamValue[0].pucName,setOneValue.pvList->pstParamValue[0].pucValue);
            }
		}
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s not exists in default file, in DmApplyDefaultValueFile",setOneValue.pvList->pstParamValue[0].pucName);
        }

    }

    getAllValues.pfnFree(&getAllValues);

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmApplyDefaultValueFile");

    return DM_SUCCESS;
}

BOOL8 DmValidityCheckCallBack(DM_VALID_CHECK_T *pPara)
{
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ enter DmValidityCheckCallBack");
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pPara->ulParaId = %d",pPara->ulParaId);
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pPara->pucPath = %s",pPara->pucPath);
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pPara->pValue = %s",pPara->pValue);
	
	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmValidityCheckCallBack");
	return DM_TRUE;
}


DM_RW_MUTEX_T *DmGetRwMutexForTransaction(DM_HANDLER_T *pDmHandler, INT8 *pRootNodeToLock, INT8 *pAccessEntity)
{
    BOOL8 bConflict = DM_FALSE;
    INT32 cnt = 0;
    INT32 lockLen = 0;
    INT32 toLockLen = 0;
    DM_RW_MUTEX_T *pDmRwMutex = NULL;
    DM_RW_MUTEX_T *pDMRwMutexes = pDmHandler->resourceRW;
    INT8 *pStrippedPath = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmGetRwMutexForTransaction");
    /* here should strip pRootNodeToLock. Just delete {i}. */

    pStrippedPath = DmChangeOrgNameToFullname(pRootNodeToLock);
    toLockLen = strlen(pStrippedPath);

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ try to lock %s >>>stripped>>> %s in DmGetRwMutexForTransaction",pRootNodeToLock,pStrippedPath);

    for (cnt=0; cnt<DM_RW_RESOURCE_MAX; cnt++)
    {
        if (strlen(pDMRwMutexes[cnt].lockPath)>0)
        {
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ RW Path [%d] : %s ",cnt+1,pDMRwMutexes[cnt].lockPath);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ RW Path [%d] : NULL ",cnt+1);
        }
    }

    for (cnt=0; cnt<DM_RW_RESOURCE_MAX; cnt++)
    {
        lockLen = strlen(pDMRwMutexes[cnt].lockPath);
        if ((lockLen>toLockLen && strstr(pDMRwMutexes[cnt].lockPath, pStrippedPath) != NULL)
            || (lockLen>0 && lockLen<toLockLen && strstr(pStrippedPath, pDMRwMutexes[cnt].lockPath) != NULL)
            || (lockLen==toLockLen && strcmp(pStrippedPath, pDMRwMutexes[cnt].lockPath) == 0))
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ conflict lockPath(%s) by %s, toLockPath(%s) by %s, in DmGetRwMutexForTransaction",pDMRwMutexes[cnt].lockPath,pDMRwMutexes[cnt].AccessEntity,pStrippedPath,pAccessEntity);
            bConflict = DM_TRUE;
            break;
        }
        else if (lockLen == 0 && pDmRwMutex == NULL)
        {
            pDmRwMutex = &(pDMRwMutexes[cnt]);/* record and return this null node if not conflictable. */
        }
    }

    if (bConflict != DM_TRUE)
    {
        if (pDmRwMutex != NULL)
        {
            strcpy(pDmRwMutex->lockPath, pStrippedPath);
            strcpy(pDmRwMutex->AccessEntity,pAccessEntity);
            DM_FREE(pStrippedPath);
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ %s get rw mutex (%s) in DmGetRwMutexForTransaction",pDmRwMutex->AccessEntity,pDmRwMutex->lockPath);
        }

        for (cnt=0; cnt<DM_RW_RESOURCE_MAX; cnt++)
        {
            if (strlen(pDMRwMutexes[cnt].lockPath)>0)
            {
                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ RW Path [%d] : %s ",cnt+1,pDMRwMutexes[cnt].lockPath);
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ RW Path [%d] : NULL ",cnt+1);
            }
        }


        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmGetRwMutexForTransaction");
        return pDmRwMutex;
    }
    else
    {
        for (cnt=0; cnt<DM_RW_RESOURCE_MAX; cnt++)
        {
            if (strlen(pDMRwMutexes[cnt].lockPath)>0)
            {
                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ RW Path [%d] : %s ",cnt+1,pDMRwMutexes[cnt].lockPath);
            }
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ RW Path [%d] : NULL ",cnt+1);
            }
        }


        DM_FREE(pStrippedPath);
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmGetRwMutexForTransaction NULL");
        return NULL;
    }

}

BOOL8 DmReleaseRwMutexOfTransaction(DM_HANDLER_T *pDmHandler, INT8 *pRootNodeToLock, INT8 *pAccessEntity)
{
    BOOL8 bRet = DM_FALSE;
    INT32 cnt = 0;
    DM_RW_MUTEX_T *pDMRwMutexes = pDmHandler->resourceRW;
    INT8 *pStrippedPath = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmReleaseRwMutexOfTransaction");
    /* here should strip pRootNodeToLock. Just delete {i}. */

    pStrippedPath = DmChangeOrgNameToFullname(pRootNodeToLock);

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ try to unlock %s >>>stripped>>> %s by %s in DmReleaseRwMutexOfTransaction",pRootNodeToLock,pStrippedPath,pAccessEntity);

    for (cnt=0; cnt<DM_RW_RESOURCE_MAX; cnt++)
    {
        if (strlen(pDMRwMutexes[cnt].lockPath)>0)
        {
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ RW Path [%d] : %s ",cnt+1,pDMRwMutexes[cnt].lockPath);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ RW Path [%d] : NULL ",cnt+1);
        }
    }


    for (cnt=0; cnt<DM_RW_RESOURCE_MAX; cnt++)
    {
        if ((strlen(pDMRwMutexes[cnt].lockPath) > 0) 
            && (strcmp(pDMRwMutexes[cnt].lockPath, pStrippedPath) == 0))
        {
            osa_mem_set(&(pDMRwMutexes[cnt]), 0, sizeof(DM_RW_MUTEX_T));
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ reset one rw mutex (%s) entity(%s) in DmReleaseRwMutexOfTransaction",pStrippedPath,pAccessEntity);

            bRet = DM_TRUE;
            break;
        }
    }

    DM_FREE(pStrippedPath);

    if (bRet == DM_FALSE)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to find %s by %s in RW table of %s db ",pRootNodeToLock,pAccessEntity,pDmHandler->pucDbName);
    }

    for (cnt=0; cnt<DM_RW_RESOURCE_MAX; cnt++)
    {
        if (strlen(pDMRwMutexes[cnt].lockPath)>0)
        {
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ RW Path [%d] : %s ",cnt+1,pDMRwMutexes[cnt].lockPath);
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ RW Path [%d] : NULL ",cnt+1);
        }
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmReleaseRwMutexOfTransaction bRet=%d",bRet);
    return bRet;
}

BOOL8 DmDupTransactionInfo(DM_HANDLER_T *pDmHandler, DM_TRANSACTION_REQ_T *pTransaction)
{
    BOOL8 bRet = DM_FALSE;
    DM_TRANSACTION_INFO_T *pBackup = NULL;
    INT8 *pucFuncAddress = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmDupTransactionInfo");

    pBackup = (DM_TRANSACTION_INFO_T *)DM_MALLOC(sizeof(DM_TRANSACTION_INFO_T));

    if (pBackup == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmDupTransactionInfo NULL");
        return bRet;
    }

    osa_mem_set(pBackup, 0, sizeof(DM_TRANSACTION_INFO_T));

    /* add the mutex to make sure unique transaction id */
    osa_thread_mutex_lock(&(g_DmCtrl.transIdMutex));

    g_DmCtrl.transIdMax++;
    if (g_DmCtrl.transIdMax==0)
    {
        /* skip 0 */
        g_DmCtrl.transIdMax++;
    }
    pTransaction->TransactionId = g_DmCtrl.transIdMax;
    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ new transaction id(%u) allocated for %s to access %s",g_DmCtrl.transIdMax,pTransaction->AccessEntity,pTransaction->RootNodeToLock);

    osa_thread_mutex_unlock(&(g_DmCtrl.transIdMutex));

    pucFuncAddress = DmUintToStr((UINT32)pTransaction->TransactionId);

    osa_mem_copy(&(pBackup->transReq), pTransaction, sizeof(DM_TRANSACTION_REQ_T));

    osa_thread_mutex_lock(&(pDmHandler->mutexXml));

    DmHashAdd(&(pDmHandler->hashTransactions),pucFuncAddress,(void*)pBackup);

    osa_thread_mutex_unlock(&(pDmHandler->mutexXml));

    DM_FREE(pucFuncAddress);

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmDupTransactionInfo");

    return bRet;
}

DM_TRANSACTION_INFO_T *DmDropTransactionInfo(DM_HANDLER_T *pDmHandler, UINT32 transactionId)
{
    DM_TRANSACTION_INFO_T *pTransaction = NULL;
    INT8 *pucFuncAddress = NULL;

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ enter DmDropTransactionInfo");

    pucFuncAddress = DmUintToStr((UINT32)transactionId);

    osa_thread_mutex_lock(&(pDmHandler->mutexXml));

    pTransaction = DmHashDel(&(pDmHandler->hashTransactions),pucFuncAddress);

    osa_thread_mutex_unlock(&(pDmHandler->mutexXml));

    DM_FREE(pucFuncAddress);

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ leave DmDropTransactionInfo");

    return pTransaction;
}

DM_TRANSACTION_INFO_T *DmGetTransactionInfo(DM_HANDLER_T *pDmHandler, UINT32 transactionId)
{
    DM_TRANSACTION_INFO_T *pTransaction = NULL;
    INT8 *pucFuncAddress = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmGetTransactionInfo");

    pucFuncAddress = DmUintToStr((UINT32)transactionId);

    pTransaction = DmHashGetValue(&(pDmHandler->hashTransactions),pucFuncAddress);

    DM_FREE(pucFuncAddress);

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmGetTransactionInfo");

    return pTransaction;
}

void DmUpdateTransactionInfo(DM_HANDLER_T *pDmHandler, UINT32 transactionId)
{
    DM_TRANSACTION_INFO_T *pTransInfo = NULL;
    DM_NOTIFY_ID_LIST_T *pNotifyIdList = NULL;

    pTransInfo = DmGetTransactionInfo(pDmHandler,transactionId);

    if (pTransInfo==NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ pTransInfo is null, transactionId=%d db=%s leave DmGetTransactionInfo",transactionId,pDmHandler->pucDbName);
        return;
    }

    /* update bNeedBak */
    if (pDmHandler->bNeedBak)
    {
        pTransInfo->bNeedBak = pDmHandler->bNeedBak;
        pDmHandler->bNeedBak = 0;
    }

    /* if any notified parameters */
    if (pDmHandler->pNotifyIdList != NULL)
    {
        if (pTransInfo->pNotifyIdList != NULL)
        {
            /* find the last node in pTransInfo->pNotifyIdList */
            pNotifyIdList = pTransInfo->pNotifyIdList->pPrev;

            /* append pDmHandler->pNotifyIdList to pTransInfo->pNotifyIdList  */
            pNotifyIdList->pNext = pDmHandler->pNotifyIdList;
            pDmHandler->pNotifyIdList->pPrev = pNotifyIdList;
            pDmHandler->pNotifyIdList = NULL;
        }
        else
        {
            pTransInfo->pNotifyIdList = pDmHandler->pNotifyIdList;
            pDmHandler->pNotifyIdList = NULL;
        }
    }

    /* if any notified parameters */
    if (pDmHandler->pAttrNotifyIdList != NULL)
    {
        if (pTransInfo->pAttrNotifyIdList != NULL)
        {
            /* find the last node in pTransInfo->pNotifyIdList */
            pNotifyIdList = pTransInfo->pAttrNotifyIdList->pPrev;

            /* append pDmHandler->pNotifyIdList to pTransInfo->pNotifyIdList  */
            pNotifyIdList->pNext = pDmHandler->pAttrNotifyIdList;
            pDmHandler->pAttrNotifyIdList->pPrev = pNotifyIdList;
            pDmHandler->pAttrNotifyIdList = NULL;
        }
        else
        {
            pTransInfo->pAttrNotifyIdList = pDmHandler->pAttrNotifyIdList;
            pDmHandler->pAttrNotifyIdList = NULL;
        }
    }

    return;
}


void* DmHashGetFromPath(DM_HASH_T *pHash, INT8* key)
{
	INT8 *pucTmp = DmChangeOrgNameToFullname(key);
	void *pRet = DmHashGetValue(pHash,pucTmp);

	DM_FREE(pucTmp);

	return pRet;
}


INT32 DmTransactionGroupHandler(void *pResourceData, OSA_RESOURCE_GRP_STATUS_E status, void *pExtraData)
{
    INT32 lRet = OSA_FAILURE;
    DM_HANDLER_T *pHandler = (DM_HANDLER_T *)pResourceData;
    DM_TRANSACTION_REQ_T *pTransaction = NULL;
    DM_TRANSACTION_INFO_T *pTransInfo = NULL;
    DM_TRANSACTION_INFO_T *pBackup = NULL;
    DM_RW_MUTEX_T *pRwMutex = NULL;

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ enter DmTransactionGroupHandler");

    switch(status)
    {
        case OSA_RESOURCE_WAIT:
            pTransaction = (DM_TRANSACTION_REQ_T *)pExtraData;
            if (pHandler != NULL)
            {
                pRwMutex = DmGetRwMutexForTransaction(pHandler, pTransaction->RootNodeToLock, pTransaction->AccessEntity);
                if (pRwMutex != NULL)
                {
                    /* get the rw access rights */
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ get rw mutex for transaction(%d) successfully, entity=%s, lock-path=%s in DmTransactionGroupHandler.",pTransaction->TransactionId, pTransaction->AccessEntity, pTransaction->RootNodeToLock);
                    DmDupTransactionInfo(pHandler, pTransaction);
                    lRet = OSA_SUCCESS;
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ failed to get rw mutex for transaction. it is null, entity=%s, lock-path=%s in DmTransactionGroupHandler.", pTransaction->AccessEntity, pTransaction->RootNodeToLock);
                    lRet = OSA_FAILURE;
                }
            }
            break;
        case OSA_RESOURCE_RELEASE:
            /* get the transaction according to transaction id */
            pBackup = (DM_TRANSACTION_INFO_T *)pExtraData;
            pTransInfo = DmDropTransactionInfo(pHandler, pBackup->transReq.TransactionId);
            pTransaction = &pTransInfo->transReq;

            osa_mem_copy(pBackup, pTransInfo, sizeof(DM_TRANSACTION_INFO_T));
            
            /* save xml files to Flash */
            if(pHandler->bExtDb == DM_FALSE && ((-1 != pTransaction->DataFlush && DM_TRUE == pTransInfo->bNeedBak) || DM_TRUE == pTransaction->DataFlush))
            {
                osa_thread_mutex_lock(&(pHandler->mutexXml));

                if(DmSyncDBFileReq(pHandler) == DM_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ sync %s to %s and %s succssfully in DmTransactionGroupHandler()",pHandler->pucFilePath,pHandler->pLoadPath,pHandler->pBackupPath);
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ sync %s to %s and %s failed in DmTransactionGroupHandler()",pHandler->pucFilePath,pHandler->pLoadPath,pHandler->pBackupPath);
                }

                osa_thread_mutex_unlock(&(pHandler->mutexXml));
            }

            if (DmReleaseRwMutexOfTransaction(pHandler, pTransaction->RootNodeToLock, pTransaction->AccessEntity) == DM_TRUE)
            {
                lRet = OSA_SUCCESS;
            }
            else
            {
                lRet = OSA_FAILURE;
            }

            DM_FREE(pTransInfo);

            break;
        case OSA_RESOURCE_GRP_DELETED:
            lRet = OSA_SUCCESS;
            break;
        default:
            break;
    }

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DM Trace ------ leave DmTransactionGroupHandler lRet=%d",lRet);

    return lRet;
}


INT32 DmLoadDbReq(DB_CONFIG_T *pDbConfig, void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath)
{
    BOOL8 bRet = DM_FALSE;
    DM_HANDLER_T *pHandler = NULL;
    INT8 path[256] = {0};

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "enter DmLoadDbReq, dbName=%s Version=%s",pDbConfig->pDbName,pDbConfig->pDbVersion);

    if (g_DmCtrl.bAsServer == DM_FALSE)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "no need to load any Db as dm-client(%s), leave DmLoadDbReq, dbName=%s",g_DmCtrl.entity,pDbConfig->pDbName);
        return DM_SUCCESS;
    }

    if (DmHashGetValue(&g_dmHandlerHash,pDbConfig->pDbName) != NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "no need to reload %s Db, leave DmLoadDbReq(%s)",pDbConfig->pDbName,g_DmCtrl.entity);
        return DM_SUCCESS;
    }

    pHandler = (DM_HANDLER_T *)OSA_MEM_ALLOC(sizeof(DM_HANDLER_T));
    
    if (pHandler == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "Failed to malloc memory for pHandler in DmLoadDbReq()");
        return DM_FAILURE;
    }
    
    /* initialize the dm handler */
    osa_mem_set(pHandler, 0, sizeof(DM_HANDLER_T));
    
    /*initialize the database name*/
    pHandler->pucDbName = strdup(pDbConfig->pDbName);
    
    pHandler->bExtDb = pDbConfig->bExtDb;
    
    /* keep dl-hanlder */
    pHandler->pDlHandler = pDlHandler;
    
    /*initialize the database file path*/
    /*extended db has no need about these parameters*/
    if (pHandler->bExtDb == DM_FALSE)
    {
        if (pWorkPath != NULL && strlen(pWorkPath)>0)
        {
            /* change the working path */
            snprintf(path,sizeof(path),"%s%s.xml",pWorkPath,pHandler->pucDbName);
            pHandler->pucFilePath = strdup(path);
        }
        else
        {
            /* use default working path */
            pHandler->pucFilePath = strdup(pDbConfig->pFilePath);
        }

        if (pLoadPath != NULL && strlen(pLoadPath)>0)
        {
            /* change the loadng path */
            snprintf(path,sizeof(path),"%s%s.xml",pLoadPath,pHandler->pucDbName);
            pHandler->pLoadPath = strdup(path);
        }
        else
        {
            /* use default loading path */
            pHandler->pLoadPath = strdup(pDbConfig->pLoadPath);
        }

        if (pBackupPath != NULL && strlen(pBackupPath)>0)
        {
            /* change the backup path */
            snprintf(path,sizeof(path),"%s%s.xml",pBackupPath,pHandler->pucDbName);
            pHandler->pBackupPath = strdup(path);
        }
        else
        {
            /* use default loadng path */
            pHandler->pBackupPath = strdup(pDbConfig->pBackupPath);
        }

        if (pDbConfig->pMappingXmlPath != NULL)
        {
            /* use default loadng path */
            pHandler->pMappingXmlPath = strdup(pDbConfig->pMappingXmlPath);
        }
    }

    /* initialize the user list */
    pHandler->pAccessList = pDbConfig->pAccessList;

    /*initialize the hash tables */
    DmHashCreate(&(pHandler->hashNameID), DM_HASH_INIT_SIZE);
    DmHashCreate(&(pHandler->hashTransactions), DM_HASH_INIT_SIZE);

    /*initialize the mutex & res-group for dm*/
    osa_mem_set(pHandler->resourceRW, 0, sizeof(DM_RW_MUTEX_T)*DM_RW_RESOURCE_MAX);

    osa_resource_group_create(&(pHandler->resourceGrp4RW), pHandler, DmTransactionGroupHandler);

    osa_thread_mutex_create(&(pHandler->mutexXml));
    osa_thread_mutex_create(&(pHandler->mutexNotifyList));

    /* add handler to hash table */
    DmHashAdd(&g_dmHandlerHash,pHandler->pucDbName,pHandler);

    /* c-type&data-type relevant things */
    pHandler->pDataVaildChecker = pDbConfig->pDbDataValidChecker;
    pHandler->pCTypeStrHooker = pDbConfig->pDbGetCTypeStr;
    pHandler->pDataTypeStrHooker = pDbConfig->pDbGetDataTypeStr;
    pHandler->pDbIsObjectFunc = pDbConfig->pDbIsObjectFunc;

    /*initial the pattern handler*/
    bRet = pDbConfig->pDmPatternRegisterHandler();

    if(bRet != DM_TRUE)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "%sDmPatternRegisterHandler failue",pDbConfig->pDbName);

        return DM_FAILURE;
    }
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "%sDmPatternRegisterHandler successfully",pDbConfig->pDbName);
    }

    /* transfer data if data model changed */
    if (pHandler->bExtDb == DM_FALSE && bDbReset == DM_TRUE)
    {
        /* reset db-xml */
        if(DmDataModelTransferHandler(pHandler->pucDbName, DM_DATA_TRANSFER_MODE__RESET) != DM_SUCCESS)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "%s db reset failed",pDbConfig->pDbName);

            return DM_FAILURE;
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "%s db reset successfully",pDbConfig->pDbName);
        }
    }
    else if (pHandler->bExtDb == DM_FALSE && DmDataModelVersionChecker(pDbConfig->pDbName) == DM_FAILURE)
    {
        /* upgrade db-xml */
        printf("data transferring in %s DB...\n",pDbConfig->pDbName);
        if(DmDataModelTransferHandler(pDbConfig->pDbName, DM_DATA_TRANSFER_MODE__UPGRADE) != DM_SUCCESS)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "%s db upgrade failed",pDbConfig->pDbName);
            printf("data transferring failed in %s DB...\n",pDbConfig->pDbName);
            return DM_FAILURE;
        }
        else
        {
            DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "%s db upgrade successfully",pDbConfig->pDbName);
            printf("data transferring done in %s DB...\n",pDbConfig->pDbName);
        }
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "leave DmLoadDbReq, dbName=%s",pDbConfig->pDbName);

    return DM_SUCCESS;
}

INT32 DmDataModelVersionChecker(INT8 *pDbName)
{
    INT32 lResult = DM_SUCCESS;
    DM_HANDLER_T *pHandler = NULL;
    UINT32 cnt = 0;
    UINT32 paramIndex = 0;
    INT8 paramFullName[128] = {0};
    INT8 versionInXml[DB_VERSION_PARAM_VALUE_LENGTH] = {0};
    INT8 versionInCode[DB_VERSION_PARAM_VALUE_LENGTH] = {0};
    INT32 lRet = 0;
    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getCValue;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "enter DmDataModelVersionChecker, dbName=%s",pDbName);

    pHandler = GetDmHandlerByName(pDbName);

    if (pHandler == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "failed to find dmhandler for %s, leave DmDataModelVersionChecker", pDbName);
        return lResult;
    }

    /* get version from pattern table */
    for (cnt=0; cnt<pHandler->lPatternLength; cnt++)
    {
        if (pHandler->pPattern[cnt].pucNodeName != NULL && strstr(pHandler->pPattern[cnt].pucNodeName, DB_VERSION_PARAM_NAME) != NULL)
        {
            paramIndex = cnt;
            snprintf(paramFullName,sizeof(paramFullName),"%s.%s",pDbName,pHandler->pPattern[cnt].pucNodeName);
            snprintf(versionInCode,sizeof(versionInCode),"%s",pHandler->pPattern[cnt].pDefaultValue);
            break;
        }
    }

    if (paramIndex <= 0)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "failed to find %s param for %s database, leave DmDataModelVersionChecker", DB_VERSION_PARAM_NAME, pDbName);
        return lResult;
    }
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "version path : %s , version-value-in-code : %s in DmDataModelVersionChecker, dbName=%s",paramFullName,versionInCode,pDbName);
    }

    /* get version from xml file */
    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getCValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,paramFullName);
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    lRet = DmTransactionStartReq(&transReq);

    if (lRet != 0)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmTransactionStartReq failed lRet=%d, leave DmDataModelVersionChecker",lRet);
        return lResult;
    }

    getCValue.TransactionId = transReq.TransactionId;
    getCValue.ulParaId = paramIndex;
    getCValue.pucPath = paramFullName;
    getCValue.ulCnt = 1;
    getCValue.pValue = (void*)versionInXml;
    getCValue.ValueSize = sizeof(versionInXml);

    lRet = DmGetParaCValueReq(&getCValue);

    if (lRet != DM_RET_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmGetParaCValueReq failed lRet=%d in DmDataModelVersionChecker",lRet);

        /* here we should handle this case as empty version */
        memset(versionInXml,0,sizeof(versionInXml));
    }

    lRet = DmTransactionStopReq(transReq.TransactionId);

    if (lRet != DM_RET_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmTransactionStopReq to get param-values failed lRet=%d, leave DmDataModelVersionChecker",lRet);
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "version path : (%s), version-value-in-xml : (%s), in DmDataModelVersionChecker, dbName=%s",paramFullName,versionInXml,pDbName);

    /* compare version_in_xml to version_in_code */
    if (strlen(versionInXml) <= 0 || strlen(versionInXml) != strlen(versionInCode) || strncmp(versionInXml, versionInCode, DB_VERSION_PARAM_VALUE_LENGTH) != 0)
    {
        /* version not match, so we should transfer data to new data model */
        lResult = DM_FAILURE;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "leave DmDataModelVersionChecker, dbName=%s lResult=%d",pDbName,lResult);

    return lResult;
}

INT32 DmDataModelTransferHandler(INT8 *pDbName, DM_DATA_TRANSFER_MODE_E mode)
{
    INT32 lResult = DM_FAILURE;
    DM_HANDLER_T *pHandler = NULL;
    UINT32 cnt = 0;
    INT32 lRet = 0;
    INT8 rootNode[32] = {0};
    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_VALUE_REQ_T getAllValues;
    DM_SET_PARA_VALUE_REQ_T setOneValue;
    PARAMETER_NAME_LIST_T nameList;
    PARAMETER_NAME_T paramName;
    PARAMETER_VALUE_LIST_T valueList;
    PARAMETER_VALUE_T paramValue;
    UINT16 faultCode = 0;
    INT32 lIndex = -1;
    BOOL8 bTransfer = DM_FALSE;

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "enter DmDataModelTransferHandler, dbName=%s, mode=%d (0:reset, 1:upgrade)",pDbName,mode);

    pHandler = GetDmHandlerByName(pDbName);

    if (pHandler == NULL)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "failed to find dmhandler for %s, leave DmDataModelTransferHandler", pDbName);
        return lResult;
    }

    snprintf(rootNode,sizeof(rootNode),"%s.",pDbName);

    /* get all param values from old db xml */
    osa_mem_set(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,rootNode);
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_TRUE;
    transReq.timeout = 0;

    lRet = DmTransactionStartReq(&transReq);

    if (lRet != 0)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmTransactionStartReq to get param-values failed lRet=%d, leave DmDataModelTransferHandler",lRet);
        return lResult;
    }

    osa_mem_set(&getAllValues, 0, sizeof(DM_GET_PARA_VALUE_REQ_T));
    osa_mem_set(&nameList, 0, sizeof(PARAMETER_NAME_LIST_T));
    osa_mem_set(&paramName, 0, sizeof(PARAMETER_NAME_T));

    getAllValues.TransactionId = transReq.TransactionId;
    getAllValues.pNameList = &nameList;
    getAllValues.pNameList->udwCnt = 1;
    getAllValues.pNameList->pucParamName = &paramName;
    getAllValues.pNameList->pucParamName[0].pucName = rootNode;
    getAllValues.pvList = NULL;
    getAllValues.pfnFree = NULL;

    lRet = DmGetParaValueReq(&getAllValues);

    if (lRet != DM_RET_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmGetParaValueReq failed lRet=%d, leave DmDataModelTransferHandler",lRet);
        getAllValues.pfnFree(&getAllValues);

        lRet = DmTransactionStopReq(transReq.TransactionId);

        if (lRet != DM_RET_SUCCESS)
        {
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmTransactionStopReq to get param-values failed lRet=%d, leave DmDataModelTransferHandler",lRet);
        }
        return lResult;
    }
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DmGetParaValueReq fetch %d params from old db xml, in DmDataModelTransferHandler",getAllValues.pvList->udwCnt);
    }

    lRet = DmTransactionStopReq(transReq.TransactionId);

    if (lRet != DM_RET_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmTransactionStopReq to get param-values failed lRet=%d, leave DmDataModelTransferHandler",lRet);
        getAllValues.pfnFree(&getAllValues);
        return lResult;
    }

    /* remove old db xml, and generate new db xml according to new data model */
    if(NULL != pHandler->pXmlDocument)
    {
		xmlFreeDoc(pHandler->pXmlDocument);
    }

    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "old db-xml deleted for %s, in DmDataModelTransferHandler",pDbName);

    pHandler->pXmlDocument = xmlNewDoc(BAD_CAST"1.0");
    lRet = DmXmlFileGenerateReq(pDbName);

    if (lRet != DM_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmXmlFileGenerateReq failed for %s, leave DmDataModelTransferHandler",pDbName);
        getAllValues.pfnFree(&getAllValues);
        return lResult;
    }

    /* set old param-values to new xml file */
    osa_mem_set(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,rootNode);
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_TRUE;
    transReq.timeout = 0;

    lRet = DmTransactionStartReq(&transReq);

    if (lRet != 0)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmTransactionStartReq to set params failed lRet=%d, leave DmDataModelTransferHandler",lRet);
        getAllValues.pfnFree(&getAllValues);
        return lResult;
    }

    osa_mem_set(&setOneValue, 0, sizeof(DM_SET_PARA_VALUE_REQ_T));
    osa_mem_set(&valueList, 0, sizeof(PARAMETER_VALUE_LIST_T));
    osa_mem_set(&paramValue, 0, sizeof(PARAMETER_VALUE_T));

    setOneValue.TransactionId = transReq.TransactionId;
    setOneValue.pFaultCodeList = &faultCode;
    setOneValue.bCreateIfNotExit = DM_FALSE;
    setOneValue.pvList = &valueList;
    setOneValue.pvList->udwCnt = 1;
    setOneValue.pvList->pstParamValue = &paramValue;

    for (cnt=0;cnt<getAllValues.pvList->udwCnt;cnt++)
    {
        faultCode = 0;
        lIndex = DmGetIDFromPath(&(pHandler->hashNameID), getAllValues.pvList->pstParamValue[cnt].pucName);

        if(lIndex < 0)
        {
            continue;
        }

        if (mode==DM_DATA_TRANSFER_MODE__RESET && pHandler->pPattern[lIndex].bResetTransfer==DM_TRUE)
        {
            setOneValue.bCreateIfNotExit = DM_TRUE;
            bTransfer = DM_TRUE;
        }
        else if (mode==DM_DATA_TRANSFER_MODE__UPGRADE && pHandler->pPattern[lIndex].bUpgradeTransfer==DM_TRUE)
        {
            setOneValue.bCreateIfNotExit = DM_TRUE;
            bTransfer = DM_TRUE;
        }
        else
        {
            setOneValue.bCreateIfNotExit = DM_FALSE;
            bTransfer = DM_FALSE;
        }

		if(bTransfer == DM_TRUE)
		{
            setOneValue.pvList->pstParamValue[0].pucName = getAllValues.pvList->pstParamValue[cnt].pucName;
            setOneValue.pvList->pstParamValue[0].pucValue = getAllValues.pvList->pstParamValue[cnt].pucValue;

            if(setOneValue.pvList->pstParamValue[0].pucValue)
			{
				lRet = DmSetParaValueReq(&setOneValue);

                if (lRet != DM_RET_SUCCESS)
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmSetParaValueReq %s = %s failed lRet=%d faultCode=%d, skip it in DmDataModelTransferHandler()",setOneValue.pvList->pstParamValue[0].pucName,setOneValue.pvList->pstParamValue[0].pucValue,lRet,faultCode);
                }
                else
                {
                    DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DmSetParaValueReq %s = %s successfully in DmDataModelTransferHandler()",setOneValue.pvList->pstParamValue[0].pucName,setOneValue.pvList->pstParamValue[0].pucValue);
                }
			}
            else
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "%s not exits in old db file, skip it in DmDataModelTransferHandler()",setOneValue.pvList->pstParamValue[0].pucName);
            }
		}
    }

    lRet = DmTransactionStopReq(transReq.TransactionId);

    if (lRet != DM_RET_SUCCESS)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmTransactionStopReq to set param-values failed lRet=%d, leave DmDataModelTransferHandler",lRet);
    }
    else
    {
        lResult = DM_SUCCESS;
    }

    getAllValues.pfnFree(&getAllValues);

    if(DmSyncDBFileReq(pHandler)==DM_SUCCESS)
	{
		DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "sync %s to %s and %s succssfully in DmDataModelTransferHandler()",pHandler->pucFilePath,pHandler->pLoadPath,pHandler->pBackupPath);
	}
    else
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "sync %s to %s and %s failed in DmDataModelTransferHandler()",pHandler->pucFilePath,pHandler->pLoadPath,pHandler->pBackupPath);
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "leave DmDataModelTransferHandler, dbName=%s, lResult=%d",pDbName,lResult);

    return lResult;
}

INT32 DmUpdateNumberOfEntries(INT8 *pucName, INT32 numberOfEntries, DM_HANDLER_T *pDmHandler)
{
    INT32 lRet = DM_FAILURE;
    INT8 numberOfEntriesStr[16] = {0};
    xmlDocPtr pDoc = NULL;
    xmlXPathObjectPtr   xpathObj = NULL; 
	xmlXPathContextPtr	xpathCtx = NULL;
    xmlNodeSetPtr       pXpathNode  = NULL;
	INT8 *ucXPathName = NULL;

	if(NULL == pDmHandler || NULL == pucName)
    {
        return lRet;
    }

    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DmUpdateNumberOfEntries, pucName=%s, numberOfEntries=%d",pucName,numberOfEntries);

	pDoc = pDmHandler->pXmlDocument;

	if(NULL == pDoc)
	{
        return lRet;
	}

	ucXPathName = (INT8 *)DM_MALLOC(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbNodeExistCheck : malloc() failed");
		return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

    ConvertPathToXPathSimple(pucName, ucXPathName);
	xpathCtx = xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
	{
        if(NULL != xpathObj)
    	{
    	   xmlXPathFreeObject(xpathObj);
    	}
		xmlXPathFreeContext(xpathCtx); 

		DM_FREE(ucXPathName);
		return lRet;
	}
    else
    {
        snprintf(numberOfEntriesStr,sizeof(numberOfEntriesStr),"%d",numberOfEntries);

        pXpathNode = xpathObj->nodesetval;
        xmlNodeSetContent(pXpathNode->nodeTab[0], (const xmlChar *)numberOfEntriesStr);

        lRet = DM_SUCCESS;
    }

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);

	DM_FREE(ucXPathName);

    return lRet;
}

INT32 DmCheckOwnerPermission(INT8 *pucName, pid_t callerPid)
{
	INT32 lRet = DM_SUCCESS;
	
	INT8  *pDbName = GetRootNameFromPath(pucName);
	if (pDbName == NULL)
	{
		lRet = DM_RET_INVALID_ARGUMENTS;
		DM_ERROR(DM_RET_INVALID_ARGUMENTS, "DM Trace ------ Failed to obtain DB name from %s in DmCheckOwnerPermission(%s)", 
				 (pucName == NULL ? "NULL" : pucName), g_DmCtrl.entity);
		return lRet;
	}
	
	DM_HANDLER_T *pDmHandler = GetDmHandlerByName(pDbName);
	DM_FREE(pDbName);

	if(NULL == pDmHandler)
	{
		lRet = DM_RET_INVALID_ARGUMENTS;
		DM_ERROR(DM_ERR_INVALID_ARGUMENTS, "DM Trace ------ Invalid DB name(%s) in DmCheckOwnerPermission(%s)", pDbName, g_DmCtrl.entity);
		return lRet;
	}

	/* check if caller pid can access this DB handler */
	if (pDmHandler->ownerPid != 0 && callerPid != 0 && pDmHandler->ownerPid != callerPid)
	{
		lRet = DM_RET_NO_OWNNER_PID;
		DM_ERROR(DM_ERR_PROCESSING_COMMON_ERROR, "DM Trace ------ invalid caller pid (caller pid=%d db pid=%d) to DB(%s) in DmCheckOwnerPermission(%s)",
				 callerPid, pDmHandler->ownerPid, pDmHandler->pucDbName,  g_DmCtrl.entity);
	}
	
	return lRet;
}

extern UINT32 DmConvertEntityToMsgId(INT8 *pEntity);

INT32 DmSendCnfMsgIfNoTrans(UINT32 receiver_id, UINT32 ReqMsgId, DM_SOCK_MSG_HEADER_T *pSockMsgSnd)
{
    INT32 lRet = DM_SUCCESS;

    pSockMsgSnd->receiver_id = receiver_id;
    pSockMsgSnd->sender_id = DmConvertEntityToMsgId(DM_SOCK_SERVER_NAME);
    pSockMsgSnd->msg_id = ReqMsgId + 1;
    pSockMsgSnd->word_data = DM_RET_INVALID_TRANSACTION;
    pSockMsgSnd->msgLength = 0;
            
    lRet = DmSockMsgSendReq(&g_DmCtrl.sockMsgQ, pSockMsgSnd);
    if (lRet != DM_SUCCESS)
    {
         DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ send msg id (0x%x) failed in DmSendCnfMsgIfNoTrans", pSockMsgSnd->msg_id);
    }
    else
    {
         DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ send msg id (0x%x) successfully in DmSendCnfMsgIfNoTrans", pSockMsgSnd->msg_id);
    }

	return lRet;
}


INT32 DmCreateShmKey(INT8* pTokenFile, INT32 TokenFileSize, UINT32* shm_key)
{
    INT32 lRet = DM_FAILURE;
	INT32 i;
	INT8  oldFileName[256] = { '\0' };
	INT8  shmNodeKey[24];

	for(i = 0; i < 10; i ++)
	{
		/* create a token file to compute shm-key */
		snprintf(pTokenFile, TokenFileSize, "/tmp/dmsrv/%u.XXXXXX", g_DmCtrl.shm_counter);

		int fd = mkstemp(pTokenFile);
		if(fd == -1)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM Trace ------ DmCreateShmKey(%s) failed to create file (%s)",g_DmCtrl.entity, pTokenFile);
			break;
		}
		close(fd);

		/* generate key from temp file */
		UINT32 newShmKey = ftok(pTokenFile, 0xDB);
		if(newShmKey == -1)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM Trace ------ DmCreateShmKey(%s) failed to call ftok with %s.",g_DmCtrl.entity, pTokenFile);
			break;
		}
		else
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmCreateShmKey(%s) %s created",g_DmCtrl.entity,pTokenFile);
		}

		/* remove failed token file */
		if(oldFileName[0] != '\0')
		{
			remove(oldFileName);
			oldFileName[0] = '\0';
		}

		/* check key file is duplicated or not, if duplicated, try to generate again */
		snprintf(shmNodeKey, sizeof(shmNodeKey), "%u", newShmKey);
		DM_MEM_REQ_NODE_T *pNode = DmHashGetValue(&g_DmCtrl.shmInUseHash, shmNodeKey);
		if(pNode == NULL)
		{
			g_DmCtrl.shm_counter++;
			
			lRet = DM_SUCCESS;
			*shm_key = newShmKey;

			break;
		}
		else
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM Trace ------ DmCreateShmKey(%s) key(0x%08x:%s) is duplicated with (0x%08x:%s) ",g_DmCtrl.entity, newShmKey, pTokenFile, pNode->shm.key, pNode->fileName);

			snprintf(oldFileName, sizeof(oldFileName), "%s", pTokenFile);
			pTokenFile[0] = '\0';
		}
	}

	return lRet;
}

INT32 DmReleaseShmKey(INT8* pTokenFile, UINT32 shm_key)
{
    INT32 lRet = DM_SUCCESS;

	/* remove the token file */
	if (remove(pTokenFile)!=0)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ DmReleaseShmKey(%s) failed to remove %s",g_DmCtrl.entity, pTokenFile);
		lRet = DM_FAILURE;
	}
	else
	{
		DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ DmReleaseShmKey(%s) %s removed",g_DmCtrl.entity, pTokenFile);
	}

	return lRet;
}

BOOL8 DmAttrVaildCheck(DM_HANDLER_T *pDmHandler, INT32 lId, DM_ATTR_NODE_T *pAttrNode)
{
    BOOL8 bRet = DM_FALSE;
    INT8 *pSupportedXmlAttributes = NULL;
    INT8 *pAttrName = NULL;
    INT8 *pTmp = NULL;

    if (!pDmHandler || lId<0 || !pAttrNode || !pAttrNode->pAttrName || strlen(pAttrNode->pAttrName)<=0 || !pAttrNode->pAttrValue || !pAttrNode->ulAttrValueSize)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ invalid arguments in DmAttrVaildCheck(%s)",g_DmCtrl.entity);
        return bRet;
    }

    /* first to check if the attribute supported */
    if (pDmHandler->pPattern[lId].pSupportedXmlAttributes
        && strlen(pDmHandler->pPattern[lId].pSupportedXmlAttributes)>0
        && strstr(pDmHandler->pPattern[lId].pSupportedXmlAttributes,pAttrNode->pAttrName)!=NULL)
    {
        pSupportedXmlAttributes = strdup(pDmHandler->pPattern[lId].pSupportedXmlAttributes);
        pAttrName = strtok_r(pSupportedXmlAttributes,",",&pTmp);
        while(pAttrName)
        {
            if (strcmp(pAttrName,pAttrNode->pAttrName)==0)
            {
                bRet = DM_TRUE;
                break;
            }
            pAttrName = strtok_r(NULL,",",&pTmp);
        }

        DM_FREE(pSupportedXmlAttributes);
    }

    /* second to check if the attribute value is valid */
    if (bRet == DM_TRUE)
    {
        INT32 attrId = DmGetAttrId(pDmHandler, pAttrNode->pAttrName);
        INT8 strValue[1024] = {0};
        INT8 *pAttrValue = pAttrNode->pAttrValue;

        if (attrId>=0)
        {
            if (pAttrNode->bCType==DM_TRUE)
            {
                DmConvertAttributeToString(pDmHandler,pAttrNode->pAttrName,pAttrNode->pAttrValue,pAttrNode->ulAttrValueCnt,strValue);
                pAttrValue = strValue;
            }

            bRet = pDmHandler->pDataVaildChecker(pDmHandler->pAttrPattern[attrId].ucParaCType,pDmHandler->pAttrPattern[attrId].pValueRange,pAttrNode->pAttrValue);
        }
        else
        {
            bRet = DM_FALSE;
        }
    }

    return bRet;
}

BOOL8 DmConvertAttributeToString(DM_HANDLER_T *pDmHandler, INT8 *pAttrName, INT8 *pAttrValue, UINT32 attrValueCnt, INT8 *pStringValue)
{
    INT32 attrId = DmGetAttrId(pDmHandler, pAttrName);

    if (attrId>=0)
    {
        INT32 cnt = 0;
        return pDmHandler->pAttrPattern[attrId].pToStringFunc(attrValueCnt,pAttrValue,&cnt,pStringValue,pDmHandler->pAttrPattern[attrId].pValueRange);
    }

    return DM_FALSE;
}

BOOL8 DmConvertAttributeToStruct(DM_HANDLER_T *pDmHandler, INT8 *pAttrName, INT8 *pAttrValue, UINT32 *attrValueCnt, INT8 *pStructValue, UINT32 *structValueSize)
{
    INT32 attrId = DmGetAttrId(pDmHandler, pAttrName);

    if (attrId>=0)
    {
        BOOL8 bRet = pDmHandler->pAttrPattern[attrId].pToStructFunc(strlen(pAttrValue),pAttrValue,attrValueCnt,pStructValue,pDmHandler->pAttrPattern[attrId].pValueRange);
        if (bRet == DM_TRUE)
        {
            *structValueSize = (*attrValueCnt)*pDmHandler->pAttrPattern[attrId].ulCTypeSize;
        }
        return bRet;
    }

    return DM_FALSE;
}

INT32 DmGetAttrId(DM_HANDLER_T *pDmHandler, INT8 *pAttrName)
{
    INT32 attrId = 0;

    while(pDmHandler->pAttrPattern && pDmHandler->pAttrPattern[attrId].pucAttrName)
    {
        if (strcmp(pDmHandler->pAttrPattern[attrId].pucAttrName,pAttrName)==0)
        {
            return attrId;
        }

        attrId++;
    }

    return -1;
}

BOOL8 DmCheckIfAttrCreatedByDefault(DM_HANDLER_T *pDmHandler, INT32 lDbId, INT8 *pAttrName)
{
    INT32 attrId = DmGetAttrId(pDmHandler, pAttrName);

    if (attrId>=0 && pDmHandler->pAttrPattern[attrId].bCreatedByDefault==DM_TRUE)
    {
        return DM_TRUE;
    }

    return DM_FALSE;
}

INT8 *DmGetDefaultAttrValue(DM_HANDLER_T *pDmHandler, INT32 lDbId, INT8 *pAttrName)
{
    INT8 *pAttrValue = NULL;
    INT32 attrId = 0;

    /* first to check if any default value in pDmHandler->pPattern[lDbId].pXmlAttributeDefaultValue */
    if (pDmHandler->pPattern[lDbId].pXmlAttributeDefaultValue
        && strlen(pDmHandler->pPattern[lDbId].pXmlAttributeDefaultValue)>0
        && strstr(pDmHandler->pPattern[lDbId].pXmlAttributeDefaultValue,pAttrName)!=NULL)
    {
        INT8 *pAttrDefaultValue = strdup(pDmHandler->pPattern[lDbId].pXmlAttributeDefaultValue);
        INT8 *pTmp = NULL;

        pAttrValue = strtok_r(pAttrDefaultValue,"\n",&pTmp);

        while(pAttrValue)
        {
            /* attribute default-format as name="value" */
            if (strncmp(pAttrValue,pAttrName,strlen(pAttrName))==0
                && pAttrValue[strlen(pAttrName)]=='='
                && pAttrValue[strlen(pAttrName)+1]=='"'
                && pAttrValue[strlen(pAttrValue)-1]=='"')
            {
                pAttrValue[strlen(pAttrValue)-1]='\0';
                pAttrValue += strlen(pAttrName)+1+1;
                pAttrValue = strdup(pAttrValue);
                DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ Attribute %s=\"%s\" from paramter-attribute-default-value in DmGetDefaultAttrValue(%s)",pDmHandler->pPattern[lDbId].pucNodeName,pAttrName,pAttrValue,g_DmCtrl.entity);
                break;
            }
            pAttrValue = strtok_r(NULL,"\n",&pTmp);
        }

        DM_FREE(pAttrDefaultValue);
    }

    /* if not, second to check if any default value in pDmHandler->pAttrPattern[attrId].pDefaultValue */
    attrId = DmGetAttrId(pDmHandler, pAttrName);
    if (pAttrValue==NULL
        && attrId>=0
        && pDmHandler->pAttrPattern[attrId].pDefaultValue)
    {
        pAttrValue = strdup(pDmHandler->pAttrPattern[attrId].pDefaultValue);
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ %s  check attribute Attribute %s=\"%s\" from attribute default-value in DmGetDefaultAttrValue(%s)",pDmHandler->pPattern[lDbId].pucNodeName,pAttrName,pAttrValue,g_DmCtrl.entity);
    }

    /* if all failed, "" instead */
    if (pAttrValue==NULL)
    {
        pAttrValue=strdup("");
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ %s  check attribute Attribute %s=\"%s\" by default in DmGetDefaultAttrValue(%s)",pDmHandler->pPattern[lDbId].pucNodeName,pAttrName,pAttrValue,g_DmCtrl.entity);
    }

    return pAttrValue;
}

INT32 DmLoadAddOn(INT8* pAddonPath, INT8 *pAddOnName)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;
	INT8 addonNameFullPath[PATH_MAX + 1];

	DM_ADDON_NODE_T *pCurAddOnNode;
	for(pCurAddOnNode = g_DmCtrl.pAddOnList; pCurAddOnNode; pCurAddOnNode = pCurAddOnNode->next)
	{
		if(strcmp(pCurAddOnNode->addonName, pAddOnName) == 0)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ add-on %s has loadded in DmLoadAddOn", pAddOnName);
			return DM_RET_SUCCESS;
		}
	}

	osa_mem_set(addonNameFullPath, 0, sizeof(addonNameFullPath));
	snprintf(addonNameFullPath, PATH_MAX, "%s/Dm%sAddon.so", pAddonPath, pAddOnName);

	if (access(addonNameFullPath, F_OK) != 0)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM Trace ------ %s does not exist in DmLoadAddOn", addonNameFullPath);
		return lRet;
	}

	void *pDlHandler = dlopen(addonNameFullPath, RTLD_NOW|RTLD_GLOBAL);
	if (pDlHandler == NULL)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM Trace ------ dlopen %s failed, %s", addonNameFullPath, dlerror());
		return lRet;
	}
	else
	{

		DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ dlopen %s successfully", addonNameFullPath);

		INT8 dlFuncName[sizeof(addonNameFullPath)] = { 0 };
		snprintf(dlFuncName, sizeof(dlFuncName), "Dm%sHandler", pAddOnName);
	
		DB_ADDON_MESSAGE_HANDLER pMsgHandlerFunc = (DB_ADDON_MESSAGE_HANDLER)dlsym(pDlHandler, dlFuncName);
		if (pMsgHandlerFunc == NULL)
		{
			DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM Trace ------ dlopen %s failed, %s", dlFuncName, dlerror());
		}
		else
		{
			DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ dlsym %s successfully", dlFuncName);	

			pCurAddOnNode = DM_MALLOC(sizeof(DM_ADDON_NODE_T));
			if (pCurAddOnNode == NULL)
			{
				DM_TRACE(DM_TRACE_LEVEL_ERROR, "DM Trace ------ malloc failed for DM_ADDON_NODE_T in DmLoadAddOn");
			}
			else
			{
				pCurAddOnNode->pDlHandler = pDlHandler;
				pCurAddOnNode->msgHandler = pMsgHandlerFunc;
				snprintf(pCurAddOnNode->addonName, sizeof(pCurAddOnNode->addonName), "%s", pAddOnName);

				pCurAddOnNode->next = g_DmCtrl.pAddOnList;
				g_DmCtrl.pAddOnList = pCurAddOnNode;

				lRet = DM_RET_SUCCESS;
			}
		}
	}

	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ leave DmLoadAddOn");
	return lRet;
}	

INT32 DmUnloadAddOn(INT8 *pAddOnName)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;
	DM_ADDON_NODE_T *pCurAddOnNode = NULL, *pPrevAddOnNode = NULL;
	
	for(pCurAddOnNode = g_DmCtrl.pAddOnList; 
	    pCurAddOnNode; 
	    pPrevAddOnNode = pCurAddOnNode, pCurAddOnNode = pCurAddOnNode->next)
	{
		if(strcmp(pCurAddOnNode->addonName, pAddOnName) == 0)
		{
			if(pPrevAddOnNode)
			{ 
				pPrevAddOnNode->next = pCurAddOnNode->next;
			} 
			else
			{
				g_DmCtrl.pAddOnList = pCurAddOnNode->next;
			}	
			lRet = DM_RET_SUCCESS;
			break;
		}
	}

	if(pCurAddOnNode != NULL)
	{
		if (dlclose(pCurAddOnNode->pDlHandler) != 0)
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DM Trace ------ %s dlclose failed : %s in DmUnoadAddOn(%s)", pCurAddOnNode->addonName, dlerror(),g_DmCtrl.entity);
		}
		else
		{
			DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DM Trace ------ %s dlclose successfully in DmUnoadAddOn(%s)",pCurAddOnNode->addonName,g_DmCtrl.entity);
		}
		DM_FREE(pCurAddOnNode);
	}

	return lRet;
}


INT32 DmUnloadAllAddOn(void)
{
	DM_RET_E lRet = DM_RET_INTERNAL_ERROR;
	DM_ADDON_NODE_T *pCurAddOnNode;
	
	for(pCurAddOnNode = g_DmCtrl.pAddOnList; pCurAddOnNode; pCurAddOnNode = pCurAddOnNode->next)
	{
		dlclose(pCurAddOnNode->pDlHandler);
		DM_FREE(pCurAddOnNode);
	}

	return lRet;
}
