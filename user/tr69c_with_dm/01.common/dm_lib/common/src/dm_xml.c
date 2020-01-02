/*
* ==============================================================================
*   Copyright (C) 2009, Percello Ltd, Inc.
* ==============================================================================
*/
/*******************************************************************************
* File name         : db_api.c
* System name    : femtocell PS engine
* Module            : Database Module
* description       : Database API for R/W DB file
* 
* ------------------------------------------------------------------------------
* Author            : Baizhan.Li
* Department        : 
* Date created      : 2010-11-10
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


#include "dm_internal.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif




/*******************************************************************************
  Module   : DB
  Function : Convert Path To XPath
  Input     : pucPath, the path to be conver to xpath
  Output   : pcuXPath, the converted xpath
  NOTE     : null
  Return    : DM_TRUE, OK. others, NG
  History   :
 *******************************************************************************/
BOOL8 ConvertPathToXPath(INT8 *pucPath, INT8 *pucXPath)
{
    INT8 *pucTmpPath = pucXPath;
	if(NULL == pucPath || NULL == pucXPath)
	{
	    return DM_FALSE;
	}
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter ConvertPathToXPath %s\n", pucPath);

	INT8 *pucOrgName = NULL;
	INT8 *pucTmpName = NULL;
	INT8 *pucToken = NULL;
    pucOrgName = strdup(pucPath);
	
	pucToken = strtok_r(pucOrgName, ".", (char**)&pucTmpName);

    while(pucToken != NULL)
    {
        if(isdigit(pucToken[0]))
        {
            strcat(pucXPath, "/i");
			strcat(pucXPath, pucToken);
        }
		else if(0 == strcmp(pucToken,"{i}"))
		{
            strcat(pucXPath, "/i1");
		}
        else
        {
            strcat(pucXPath, "/");
            strcat(pucXPath, pucToken);
            
        }
        pucToken = strtok_r(NULL, ".", (char**)(&pucTmpName));
    }
	DM_FREE(pucOrgName);

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave ConvertPathToXPath %s\n", pucTmpPath);
    return DM_TRUE;
}


/*******************************************************************************
  Module   : DB
  Function : Convert Path To XPath
  Input     : pucPath, the path to be conver to xpath
  Output   : pcuXPath, the converted xpath
  NOTE     : null
  Return    : DM_TRUE, OK. others, NG
  History   :
 *******************************************************************************/
BOOL8 ConvertPathToXPathSimple(INT8 *pucPath, INT8 *pucXPath)
{
	if(NULL == pucPath || NULL == pucXPath)
	{
	    return DM_FALSE;
	}

	INT8 *pucOrgName = NULL;
	INT8 *pucTmpName = NULL;
	INT8 *pucToken = NULL;
    pucOrgName = strdup(pucPath);
	
	pucToken = strtok_r(pucOrgName, ".", (char**)&pucTmpName);

    while(pucToken != NULL)
    {
        if(isdigit(pucToken[0]))
        {
            strcat(pucXPath, "/i");
			strcat(pucXPath, pucToken);
        }
        else
        {
            strcat(pucXPath, "/");
            strcat(pucXPath, pucToken);
            
        }
        pucToken = strtok_r(NULL, ".", (char**)(&pucTmpName));
    }
	free(pucOrgName);

    return DM_TRUE;
}

/*******************************************************************************
  Module   : DB
  Function : get the node name from path like "a.b.c. or a.b.c.1. or a.b.c.a"
  Input     : pucName, the path to be processed
  Output   : null
  NOTE     : null
  Return    : the node name
  History   :
 *******************************************************************************/
INT8* DmGetNodeNameFromPath(INT8 *pucName)
{
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DmGetNodeNameFromPath %s\n", pucName);
	INT8 *pucLastIdx = NULL;
	INT8 *pName      = NULL;
	INT32        lNameLen = 0;
	if(NULL == pucName)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ leave DmGetNodeNameFromPath with wrong parameter %s\n", pucName);
	    return NULL;
	}
	lNameLen = strlen(pucName);

    /*if the path is object */
	if(pucName[lNameLen- 1] == '.')
	{
		/*if root object*/
	    if(rindex(pucName, '.') == index(pucName, '.'))
		{
		    pName = calloc(1, sizeof(UINT8)*(lNameLen));
			if(NULL == pName)
			{
			    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DmGetNodeNameFromPath allocate memory error");
				return pName;
			}
		    strncpy((char*)pName, pucName, lNameLen-1); 
			DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DmGetNodeNameFromPath");
			return pName;
		}
	    lNameLen = 0;
	    pucLastIdx = rindex(pucName, '.');
		pucLastIdx --;
		while(*pucLastIdx != '.')
		{
		     pucLastIdx --;
			 lNameLen ++;
		}
		pName = calloc(1, sizeof(UINT8)*(lNameLen+1));
		if(NULL == pName)
		{
		    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DmGetNodeNameFromPath allocate memory error");
			return pName;
		}
	    strncpy((char*)pName, (char*)pucLastIdx+1, lNameLen);
	}
	else
	{
	    /*if the path is element*/
	    pucLastIdx = rindex(pucName, '.');
		pName = calloc(1, sizeof(UINT8)*(strlen(pucName)-abs(pucName-pucLastIdx)+1));
		if(NULL == pName)
		{
		    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DmGetNodeNameFromPath allocate memory error");
			return pName;
		}
	    strcpy((char*)pName, pucLastIdx+1);
	}
	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DmGetNodeNameFromPath");
	return pName;
}
/*******************************************************************************
  Module   : DB
  Function : get the parent path  from path like "a.b.c. or a.b.c.1. or a.b.c.a"
  Input     : pucName, the path to be processed
  Output   : null
  NOTE     : null
  Return    : the parent path 
  History   :
 *******************************************************************************/
INT8 *DmGetNodeParentPathFromPath(INT8 *pucName)
{
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DmGetNodeParentPathFromPath %s\n", pucName);
	if(NULL == pucName)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ leave DmGetNodeParentPathFromPath with wrong parameter");
	    return NULL;
	}
	
	INT8 *pucLastIdx = NULL;
	INT8 *pName = NULL;
	/*if object*/
	if(pucName[strlen(pucName)- 1] == '.')
	{
	    pucLastIdx = rindex(pucName, '.');
		pucLastIdx --;
		while(*pucLastIdx != '.')
		{
		     pucLastIdx --;
		}
		pName = calloc(abs(pucName - pucLastIdx)+1, sizeof(UINT8));
		if(NULL == pName)
		{
		    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DmGetNodeParentPathFromPath allocate memory failure");
			return pName;
		}
	}
	else
	{
		/*if item*/
		pucLastIdx = rindex(pucName, '.');
		pName = calloc(abs(pucName - pucLastIdx)+1, sizeof(UINT8));
		if(NULL == pName)
		{
		    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DmGetNodeParentPathFromPath allocate memory failure");
			return pName;
		}
	}
	strncpy((char*)pName, pucName, abs(pucName-pucLastIdx));
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DmGetNodeParentPathFromPath");
	return pName;

}
/*******************************************************************************
  Module   : DB
  Function : get the new instance number for add object
  Input     : pHandler, the database handler
                pucPath, the object path, should be "a.b.c."
  Output   : null
  NOTE     : null
  Return    : the new index
  History   :
 *******************************************************************************/
INT32 GetNewObjectIndex(DM_HANDLER_T *pHandler, UINT8 *pucPath)
{
    INT32               lResult = 1;
	xmlXPathContextPtr  xpathCtx; 
	xmlXPathObjectPtr   xpathObj; 
	xmlNodeSetPtr	    pXpathNode  = NULL;
	INT32			    i = 0;
    xmlDocPtr pDoc = NULL;
	INT8 *ucXPathName = NULL;

	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ GetNewObjectIndex : malloc failed");
		return lResult;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	if(!pHandler ||!pucPath)
	{
		free(ucXPathName);
	    return lResult;
	}

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter GetNewObjectIndex %s\n", pucPath);
	/*get the document handler*/

    pDoc = pHandler->pXmlDocument;

	if(!pDoc)
	{
		free(ucXPathName);
	    return lResult;
	}

	/*change the path to the xpath*/
	strcpy((char*)ucXPathName, (char*)pucPath);
	xpathCtx    =   xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);


	/*check the element is existing, not existing, return error
	* exist, get the element to be set parameter
	*/
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval)) 	   //Could not 
	{
	   xmlXPathFreeObject(xpathObj);
	   xmlXPathFreeContext(xpathCtx);

	   free(ucXPathName);
	   return 1;
	}
	else
	{
	
    	/*get the element*/
	    pXpathNode = xpathObj->nodesetval;
		/*get the children element*/
		/*convert to digit, compare and get the max number*/

	    xmlNodePtr node = NULL;
		for(node = pXpathNode->nodeTab[0]->children;node != NULL;node = node->next)
		{
		    /*jump the i1's i */
			i = atoi((const char *)((node->name)+1));
			lResult = lResult > i? lResult:i+1;
		}
		/*add 1 to the max one, and return*/
		
	}
	DM_FREE(ucXPathName);
	
	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx); 
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave GetNewObjectIndex");
	return lResult;
}
/*******************************************************************************
  Module   : DB
  Function : get the id from path
  Input     : table, the hash table whick holds the relationship of path and id
  Output   : null
  NOTE     : null
  Return    : id of path, or -1 for not exist one.
  History   :
 *******************************************************************************/
INT32 DmGetIDFromPath(DM_HASH_T *pTable, INT8 *pucPath)
{
    INT32 lRetId = -1;
	INT32 *plId = NULL;
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DmGetIDFromPath %s\n", pucPath);
	plId = DmHashGetFromPath(pTable, pucPath);
	if(NULL != plId)
	{
		lRetId = *plId;
	}
	else
	{
	    lRetId = -1;
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DmGetIDFromPath %d", lRetId);
	return lRetId;
}
/*******************************************************************************
  Module   : DB
  Function : judge is the lId is an object id
  Input     : pHandler, the database handler
                 lId, the id of object
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, object, others, not an object
  History   :
 *******************************************************************************/
BOOL8 isObject(DM_HANDLER_T *pHandler, UINT32 lId)
{
    BOOL8 bRet = DM_FALSE;
    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter isObject %d", lId);
	bRet = pHandler->pDbIsObjectFunc(pHandler->pPattern[lId].ucParaType);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave isObject %d", bRet);
    return bRet;
}

typedef struct _DbXmlEscapeChar {
    char * escapeChar;
    char * originalChar;
}DbXmlEscapeChar;

static DbXmlEscapeChar escapeChars[] = {
    {"&lt;","<"},
    {"&gt;",">"},
    {"&amp;","&"},
    {"&apos;","'"},
    {"&quot;","\""},
    {NULL,NULL}
};

#if 0
static INT8 *DbTranslateFromEscapeChars(INT8 *pValue)
{
    if (pValue == NULL || strlen(pValue) <= 0)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbTranslateFromEscapeChars null");
        return strdup("");
    }
    else
    {
        INT8 *pDest = DM_MALLOC(strlen(pValue)+1);
        INT8 *pToken = pValue;
        INT8 *pLast = pValue;
        INT8 *pDestTmp = pDest;
        DbXmlEscapeChar *pEscapeChar = NULL;

        osa_mem_set(pDest, 0, strlen(pValue)+1);

        while ((pToken=strchr(pToken,'&'))!=NULL)
        {
            /* check if any esacpe-chars, if so, replace it */
            pEscapeChar = escapeChars;
            while (pEscapeChar && pEscapeChar->escapeChar)
            {
                if (strncmp(pEscapeChar->escapeChar,pToken,strlen(pEscapeChar->escapeChar))==0)
                {
                    strncat(pDestTmp,pLast,pToken-pLast);
                    strcat(pDestTmp,pEscapeChar->originalChar);

                    pLast = pToken + strlen(pEscapeChar->escapeChar);
                    pDestTmp += strlen(pDestTmp);
                    break;
                }

                pEscapeChar++;
            }

            pToken++;
        }

        strcpy(pDestTmp,pLast);

        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbTranslateFromEscapeChars %s -> %s",pValue,pDest);

        return pDest;
    }
}
#endif

INT8 *DbTranslateToEscapeChars(INT8 *pValue)
{
    INT8 *pTransValue = NULL;
    INT8 *pTmp = NULL;
    DbXmlEscapeChar *pEscapeChar = NULL;
    INT32 expendedLength = 0;

    if (pValue == NULL || strlen(pValue) <= 0)
    {
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbTranslateToEscapeChars null");
        return strdup("");
    }

    /* here we should compute the max length after translating */
    pEscapeChar = escapeChars;
    while (pEscapeChar && pEscapeChar->originalChar)
    {
        pTmp = pValue;
        while ((pTmp=strstr(pTmp, pEscapeChar->originalChar))!=NULL)
        {
            expendedLength += strlen(pEscapeChar->escapeChar)-1;
            pTmp++;
        }

        pEscapeChar++;
    }

    if (expendedLength == 0)
    {
        /* no escape found */
        DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbTranslateToEscapeChars %s no change",pValue);
        return strdup(pValue);
    }

    pTransValue = DM_MALLOC(strlen(pValue)+expendedLength+1);

    osa_mem_set(pTransValue, 0, strlen(pValue)+expendedLength+1);

    /* translate each char in the variable pValue */
    pTmp = pValue;
    while (pTmp && *pTmp != '\0')
    {
        pEscapeChar = escapeChars;
        while(pEscapeChar && pEscapeChar->originalChar)
        {
            if (*pTmp == *(pEscapeChar->originalChar))
            {
                /* match any escape char */
                strcat(pTransValue,pEscapeChar->escapeChar);
                break;
            }
            else
            {
                pEscapeChar++;
            }
        }

        if (pEscapeChar == NULL || pEscapeChar->originalChar == NULL)
        {
            /* no need to translate the char */
            *(pTransValue + strlen(pTransValue)) = *pTmp;
        }

        pTmp++;
    }

    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbTranslateToEscapeChars %s -> %s",pValue,pTransValue);

    return pTransValue;
}


/*******************************************************************************
  Module   : DB
  Function : get datavalue from xml file
  Input     : pGetPara, the parameter used to get 
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, OK, others, NG
  History   :
 *******************************************************************************/
BOOL8 DbGetCurHandlerWithFullPath(DM_HANDLER_PARA_T *pGetPara)
{
	UINT8              *pValue     = NULL;
    xmlXPathContextPtr  xpathCtx; 
    xmlXPathObjectPtr   xpathObj; 
	xmlNodeSetPtr       pXpathNode  = NULL;


    /*get the document handler*/
    
	if(NULL == pGetPara ||NULL == pGetPara->pucName || NULL == pGetPara->pDbHandler)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetCurHandlerWithFullPath parameter error");
        return DM_FALSE;
    }
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbGetCurHandlerWithFullPath %s", pGetPara->pucName);
	}
	xmlDocPtr pDoc = pGetPara->pDbHandler->pXmlDocument;
	if(NULL == pDoc)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetCurHandlerWithFullPath xml content is error");
		
        return DM_FALSE;
	}
	/*if registered a get handler, call it*/
	if(pGetPara->pDbHandler->pPattern[pGetPara->ulParaId].pGetHandler != NULL)
	{    
	     DM_GET_VALUE_T Para = {0};
		 Para.ulParaId = pGetPara->ulParaId;
		 Para.pucPath = pGetPara->pucName;
		 Para.pValue  = pGetPara->pValue;
         Para.ValueSize = pGetPara->valueSize;
	     DmGetHandlerCalling(pGetPara->pDbHandler->pPattern[pGetPara->ulParaId].pGetHandler,&Para);
		 DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetCurHandlerWithFullPath");
		 
   		 return DM_TRUE;
	}

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetCurHandlerWithFullPath : malloc() failed");
		
        return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);
	
	/*change the path to the xpath*/
    ConvertPathToXPath(pGetPara->pucName, ucXPathName);
	xpathCtx    =   xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

    
	/*check the element is existing, not existing, return error
	* exist, get the element to be set parameter
	*/
    if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))        //Could not 
    {
        if(NULL != xpathObj)
    	{
    	   xmlXPathFreeObject(xpathObj);
    	}
        xmlXPathFreeContext(xpathCtx);
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetCurHandler xml content is error");
		free(ucXPathName);
		
        return DM_FALSE;
    }
	else
	{
		pXpathNode = xpathObj->nodesetval;
		/*
		* if object, set the number of object to the pucValue and return True
		* if element, set the value of item to the pucValue and return True
		*/

		if(isObject(pGetPara->pDbHandler, pGetPara->ulParaId))
		{
			DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetCurHandlerWithFullPath, this branch is useless and will never come in, %s", pGetPara->pDbHandler->pPattern[pGetPara->ulParaId].pucNodeName);
		}
		else
		{
    		pValue = (UINT8*)xmlNodeGetContent(pXpathNode->nodeTab[0]);
			strcpy((char*)pGetPara->pValue, (char*)pValue);
			free(pValue);
		}
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx); 
	}
	free(ucXPathName);
	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetCurHandler");
    return DM_TRUE;
}

/*******************************************************************************
  Module   : DB
  Function : check whether node exist in database
  Input     : pDmHandler, the dm handler
              : pcNodeName, the node name to check
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, DM_FALSE
  History   :
 *******************************************************************************/
BOOL8 DbNodeExistCheck(DM_HANDLER_T *pDmHandler,INT8 *pcNodeName)
{
    xmlXPathObjectPtr   xpathObj; 
	xmlXPathContextPtr	xpathCtx;




	if(NULL == pDmHandler ||NULL == pcNodeName)
    {
        return DM_FALSE;
    }

	xmlDocPtr pDoc = pDmHandler->pXmlDocument;
	
	if(NULL == pDoc)
	{
        return DM_FALSE;
	}

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbNodeExistCheck : malloc() failed");

		return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

    ConvertPathToXPathSimple(pcNodeName, ucXPathName);
	xpathCtx    =   xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);
	
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
	{
        if(NULL != xpathObj)
    	{
    	   xmlXPathFreeObject(xpathObj);
    	}
		xmlXPathFreeContext(xpathCtx); 

		free(ucXPathName);
		return DM_FALSE;
	}

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);

	free(ucXPathName);
	
    return DM_TRUE;
}



/*******************************************************************************
  Module   : DB
  Function : get datavalue from xml file
  Input     : pGetPara, the parameter used to get 
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, OK, others, NG
  History   :
 *******************************************************************************/
BOOL8 DbGetCurHandler(DM_HANDLER_PARA_T *pGetPara)
{
	UINT8              *pValue     = NULL;
    xmlXPathObjectPtr   xpathObj; 
	xmlNodeSetPtr       pXpathNodeSet  = NULL;


    /*get the document handler*/
    
	if(NULL == pGetPara ||NULL == pGetPara->pucName || NULL == pGetPara->pDbHandler)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetCurHandler parameter error");
        return DM_FALSE;
    }
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbGetCurHandler %s", pGetPara->pucName);
	}
	xmlDocPtr pDoc = pGetPara->pDbHandler->pXmlDocument;
	if(NULL == pDoc)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetCurHandler xml content is error");
        return DM_FALSE;
	}

	/*if registered a get handler, call it*/
	if(pGetPara->pDbHandler->pPattern[pGetPara->ulParaId].pGetHandler != NULL)
	{    
	     DM_GET_VALUE_T Para = {0};
		 Para.ulParaId = pGetPara->ulParaId;
		 Para.pucPath = pGetPara->pucName;
		 Para.pValue  = pGetPara->pValue;
         Para.ValueSize = pGetPara->valueSize;
	     DmGetHandlerCalling(pGetPara->pDbHandler->pPattern[pGetPara->ulParaId].pGetHandler,&Para);
		 DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetCurHandler");
   		 return DM_TRUE;
	}

	xpathObj = xmlXPathEvalExpression((const xmlChar *)(pGetPara->pDbHandler->pPattern[pGetPara->ulParaId].pucNodeName), pGetPara->pDbHandler->xpathCtx);
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
	{
        if(NULL != xpathObj)
    	{
    	   xmlXPathFreeObject(xpathObj);
    	}
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ xml node %s is empty", pGetPara->pDbHandler->pPattern[pGetPara->ulParaId].pucNodeName);
		return DM_FALSE;
	}

	pXpathNodeSet = xpathObj->nodesetval;
	/*
	* if object, set the number of object to the pucValue and return True
	* if element, set the value of item to the pucValue and return True
	*/

	if(isObject(pGetPara->pDbHandler, pGetPara->ulParaId))
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetCurHandler, this branch is useless and will never come in, %s", pGetPara->pDbHandler->pPattern[pGetPara->ulParaId].pucNodeName);
	}
	else
	{
		pValue = (UINT8*)xmlNodeGetContent(pXpathNodeSet->nodeTab[0]);
		strcpy((char*)pGetPara->pValue, (char*)pValue);
		free(pValue);
	}

	xmlXPathFreeObject(xpathObj);

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetCurHandler");

    return DM_TRUE;
}

BOOL8 DbGetAttrCurHandler(DM_HANDLER_PARA_T *pGetParaAttr)
{
    xmlXPathObjectPtr   xpathObj; 
    INT8 ucXPathName[MAX_XML_PATH] = {0};
    /*get the document handler*/

	if(NULL == pGetParaAttr ||NULL == pGetParaAttr->pucName || NULL == pGetParaAttr->pDbHandler)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ null parameters in DbGetAttrCurHandler");
        if (pGetParaAttr)
        {
            pGetParaAttr->ulCnt = 0;
        }
        return DM_FALSE;
    }
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbGetAttrCurHandler %s", pGetParaAttr->pucName);
	}

	xmlDocPtr pDoc = pGetParaAttr->pDbHandler->pXmlDocument;
	if(NULL == pDoc)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetAttrCurHandler xml content is error");
        pGetParaAttr->ulCnt = 0;
        return DM_FALSE;
	}

    ConvertPathToXPath(pGetParaAttr->pucName, ucXPathName);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, pGetParaAttr->pDbHandler->xpathCtx);
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
	{
        if(NULL != xpathObj)
    	{
    	   xmlXPathFreeObject(xpathObj);
    	}

        pGetParaAttr->ulCnt = 0;

		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ xml node %s is empty, failed to get attributes for %s", pGetParaAttr->pDbHandler->pPattern[pGetParaAttr->ulParaId].pucNodeName, pGetParaAttr->pucName);
		return DM_FALSE;
	}

	/*
	* if object, set the number of object to the pucValue and return True
	* if element, set the value of item to the pucValue and return True
	*/

	if(isObject(pGetParaAttr->pDbHandler, pGetParaAttr->ulParaId))
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetAttrCurHandler, this branch is useless and will never come in, %s", pGetParaAttr->pDbHandler->pPattern[pGetParaAttr->ulParaId].pucNodeName);
	}
	else
	{
        INT32 maxAttrNumber = pGetParaAttr->ulCnt;
        DM_ATTR_NODE_T *pRetAttrNode = pGetParaAttr->pValue;
        xmlNodePtr pXmlNode = xpathObj->nodesetval->nodeTab[0];
		xmlAttr *pXmlAttrNode = pXmlNode->properties;

        pGetParaAttr->ulCnt = 0;

        while(pXmlAttrNode && pGetParaAttr->ulCnt<maxAttrNumber)
        {
            INT8 structValue[1024] = {0};
            INT8 *pAttrValue = (INT8 *)xmlGetProp(pXmlNode,pXmlAttrNode->name);

            pRetAttrNode[pGetParaAttr->ulCnt].pAttrName = strdup((INT8 *)pXmlAttrNode->name);
            pRetAttrNode[pGetParaAttr->ulCnt].bCType = pGetParaAttr->bCInterface;

            if (pAttrValue)
            {
                pRetAttrNode[pGetParaAttr->ulCnt].pAttrValue = strdup(pAttrValue);
            }
            else
            {
                pRetAttrNode[pGetParaAttr->ulCnt].pAttrValue = strdup("");
            }

            if (pRetAttrNode[pGetParaAttr->ulCnt].bCType==DM_TRUE)
            {
                DmConvertAttributeToStruct(pGetParaAttr->pDbHandler,pRetAttrNode[pGetParaAttr->ulCnt].pAttrName,pRetAttrNode[pGetParaAttr->ulCnt].pAttrValue,&pRetAttrNode[pGetParaAttr->ulCnt].ulAttrValueCnt,structValue,&pRetAttrNode[pGetParaAttr->ulCnt].ulAttrValueSize);
                osa_mem_copy(pRetAttrNode[pGetParaAttr->ulCnt].pAttrValue,structValue,pRetAttrNode[pGetParaAttr->ulCnt].ulAttrValueSize);
            }
            else
            {
                pRetAttrNode[pGetParaAttr->ulCnt].ulAttrValueCnt = 1;
                pRetAttrNode[pGetParaAttr->ulCnt].ulAttrValueSize = strlen(pRetAttrNode[pGetParaAttr->ulCnt].pAttrValue)+1;
            }

            pGetParaAttr->ulCnt++;

            DM_FREE(pAttrValue);
            pXmlAttrNode = pXmlAttrNode->next;
        }
	}

	xmlXPathFreeObject(xpathObj);

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetAttrCurHandler");

    return DM_TRUE;
}

/*******************************************************************************
  Module   : DB
  Function : set datavalue to xml file
  Input     : pSetPara, the parameter used to set 
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, OK, others, NG
  History   :
 *******************************************************************************/
BOOL8 DbSetCurHandler(DM_HANDLER_PARA_T *pSetPara)
{
	
    xmlXPathContextPtr  xpathCtx; 
    xmlXPathObjectPtr   xpathObj; 
	xmlNodeSetPtr       pXpathNode  = NULL;
	INT32               lDbId = 0;

	BOOL8 				bRet = DM_FALSE;
	
	/*get the document handler*/


	
	if(NULL == pSetPara ||NULL == pSetPara->pucName)
    {
    	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetCurHandler parameter is error");
        return bRet;
    }
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbSetCurHandler %s %s", pSetPara->pucName, pSetPara->pValue);
	}
	xmlDocPtr pDoc = pSetPara->pDbHandler->pXmlDocument;
	if(NULL == pDoc)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetCurHandler xml doc is error");
        return bRet;
	}

    /*if registered a set handler, call it*/
    if(pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pSetHandler != NULL)
    {    
         DM_SET_VALUE_T Para = {0};
         INT8 *pStructValue = NULL;
         UINT32 structLength = 0;

         Para.ulParaId = pSetPara->ulParaId;
         Para.pucPath = pSetPara->pucName;
         if (pSetPara->bCInterface==DM_FALSE)
         {
            pStructValue = (INT8*)DM_MALLOC(pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].ulCTypeSize);

            if (!pStructValue)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetCurHandlerForC malloc() error for pStructValue");
                return DM_FALSE;
            }

            memset(pStructValue,0,pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].ulCTypeSize);

            pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pToStructFunc(strlen(pSetPara->pValue),pSetPara->pValue,&structLength,pStructValue,pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pValueRange);

            Para.pValue  = pStructValue;
            Para.ValueSize = pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].ulCTypeSize;
         }
         else
         {
            Para.pValue  = pSetPara->pValue;
            Para.ValueSize = pSetPara->valueSize;
         }
         Para.bCValue = DM_TRUE;
         DmSetHandlerCalling(pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pSetHandler,&Para);

         DM_FREE(pStructValue);
         DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbSetCurHandler");
         return DM_TRUE;
    }

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)DM_MALLOC(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetCurHandler malloc() failed");
        return bRet;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	
    /*change the path to the xpath*/
    ConvertPathToXPath(pSetPara->pucName, ucXPathName);
	xpathCtx    =   xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);
	/*check the element is existing, not existing, return error
	* exist, get the element to be set parameter
	*/
	
	/*
	* if object  return True
	*/

	/*
	* if element, write the pucValue to the value of item and return True
	*/
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        if(NULL != xpathObj)
        {
        	xmlXPathFreeObject(xpathObj);
        }
		xmlXPathFreeContext(xpathCtx);
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetCurHandler xml content is error, path=%s",pSetPara->pucName);

		free(ucXPathName);
        return bRet;
    }
	else
	{
	    pXpathNode = xpathObj->nodesetval;
		lDbId = pSetPara->ulParaId;
		if(!isObject(pSetPara->pDbHandler, lDbId))
		{
			INT8			   *pcOrgValue	   = NULL;

			pcOrgValue = (INT8 *)xmlNodeGetContent(pXpathNode->nodeTab[0]);

			if(0 != strcmp(pcOrgValue,(INT8 *)pSetPara->pValue))
			{
                /* here we should convert escape chars if any before setting the value to xml file */
                INT8 *pTransValue = DbTranslateToEscapeChars(pSetPara->pValue);
                xmlNodeSetContent(pXpathNode->nodeTab[0], (const xmlChar *)(pTransValue));
                DM_FREE(pTransValue);
                bRet = DM_TRUE;
			}
			else
			{
				bRet = 2;
			}

			DM_FREE(pcOrgValue);
		}
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx); 
	}
	DM_FREE(ucXPathName);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbSetCurHandler");
	
    return bRet;
}


BOOL8 DbSetAttrCurHandler(DM_HANDLER_PARA_T *pSetParaAttr)
{
    xmlXPathContextPtr  xpathCtx; 
    xmlXPathObjectPtr   xpathObj; 
	xmlNodeSetPtr       pXpathNode  = NULL;

	BOOL8 				bRet = DM_FALSE;
	
	if(NULL == pSetParaAttr ||NULL == pSetParaAttr->pucName)
    {
    	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetAttrCurHandler parameter is error");
        return bRet;
    }
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbSetAttrCurHandler %s", pSetParaAttr->pucName);
	}

	xmlDocPtr pDoc = pSetParaAttr->pDbHandler->pXmlDocument;

	if(NULL == pDoc)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetAttrCurHandler xml doc is error");
        return bRet;
	}

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)DM_MALLOC(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetAttrCurHandler malloc() failed");
        return bRet;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	
    /*change the path to the xpath*/
    ConvertPathToXPath(pSetParaAttr->pucName, ucXPathName);
	xpathCtx    =   xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        if(NULL != xpathObj)
        {
        	xmlXPathFreeObject(xpathObj);
        }
		xmlXPathFreeContext(xpathCtx);
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetAttrCurHandler xml content is error, path=%s",pSetParaAttr->pucName);

		DM_FREE(ucXPathName);
        return bRet;
    }
	else
	{
	    pXpathNode = xpathObj->nodesetval;

		//if(!isObject(pSetParaAttr->pDbHandler, pSetParaAttr->ulParaId))
		{
            INT32 cnt = 0;
            DM_ATTR_NODE_T *pAttrNode = (DM_ATTR_NODE_T *)pSetParaAttr->pValue;

            for (cnt=0; cnt<pSetParaAttr->ulCnt; cnt++)
            {
                INT8 *pAttrValue = pAttrNode[cnt].pAttrValue;
                INT8 strValue[1024];

                if (pAttrNode[cnt].bCType==DM_TRUE)
                {
                    /* convert it to string */
                    DmConvertAttributeToString(pSetParaAttr->pDbHandler,pAttrNode[cnt].pAttrName,pAttrValue,pAttrNode[cnt].ulAttrValueCnt,strValue);
                    pAttrValue = strValue;
                }

                if(xmlHasProp(pXpathNode->nodeTab[0],  BAD_CAST pAttrNode[cnt].pAttrName))
                {
                    INT8 *pOrgValue = (INT8 *)xmlGetProp(pXpathNode->nodeTab[0],  BAD_CAST pAttrNode[cnt].pAttrName);

                    /* update the attribute */
                    xmlSetProp(pXpathNode->nodeTab[0], (const xmlChar *)pAttrNode[cnt].pAttrName, (const xmlChar *)pAttrValue);

                    if (!pOrgValue || strcmp(pOrgValue,pAttrValue)!=0)
                    {
                        bRet = DM_TRUE;
                    }

                    if (pOrgValue)
                    {
                        xmlFree(pOrgValue);
                    }
                }
                else
                {
                    /* add new attribute */
                    xmlNewProp(pXpathNode->nodeTab[0], (const xmlChar *)pAttrNode[cnt].pAttrName, (const xmlChar *)pAttrValue);
                    bRet = DM_TRUE;
                }
            }

            if (bRet == DM_FALSE)
            {
                /* no change */
                bRet = 2;
            }
		}
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx); 
	}
	DM_FREE(ucXPathName);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbSetAttrCurHandler");
	
    return bRet;
}

/*******************************************************************************
  Module   : DB
  Function : set datavalue to xml file
  Input     : pSetPara, the parameter used to set 
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, OK, others, NG
  History   :
 *******************************************************************************/
BOOL8 DbSetCurHandlerForC(DM_HANDLER_PARA_T *pSetPara)
{
    xmlXPathObjectPtr   xpathObj; 
	xmlNodeSetPtr       pXpathNode  = NULL;
	INT32               lDbId = 0;

	BOOL8 				bRet = DM_FALSE;

	
	/*get the document handler*/
	
	if(NULL == pSetPara ||NULL == pSetPara->pucName)
    {
    	DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetCurHandlerForC parameter is error");
        return bRet;
    }
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbSetCurHandlerForC %s %s", pSetPara->pucName, pSetPara->pValue);
	}

    /*if registered a set handler, call it*/
    if(pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pSetHandler != NULL)
    {    
         DM_SET_VALUE_T Para = {0};
         INT8 *pStructValue = NULL;
         UINT32 structLength = 0;

         Para.ulParaId = pSetPara->ulParaId;
         Para.pucPath = pSetPara->pucName;
         if (pSetPara->bCInterface==DM_FALSE)
         {
            pStructValue = (INT8*)DM_MALLOC(pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].ulCTypeSize);

            if (!pStructValue)
            {
                DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetCurHandlerForC malloc() error for pStructValue");
                return DM_FALSE;
            }

            memset(pStructValue,0,pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].ulCTypeSize);

            pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pToStructFunc(strlen(pSetPara->pValue),pSetPara->pValue,&structLength,pStructValue,pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pValueRange);

            Para.pValue  = pStructValue;
            Para.ValueSize = pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].ulCTypeSize;
         }
         else
         {
            Para.pValue  = pSetPara->pValue;
            Para.ValueSize = pSetPara->valueSize;
         }

         Para.bCValue = DM_TRUE;
         DmSetHandlerCalling(pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pSetHandler,&Para);

         DM_FREE(pStructValue);

         DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbSetCurHandlerForC");
         return DM_TRUE;
    }

	xpathObj = xmlXPathEvalExpression((const xmlChar *)(pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pucNodeName), pSetPara->pDbHandler->xpathCtx);
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
	{
        if(NULL != xpathObj)
    	{
    	   xmlXPathFreeObject(xpathObj);
    	}
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ xml node %s is empty", pSetPara->pDbHandler->pPattern[pSetPara->ulParaId].pucNodeName);
		return bRet;
	}

    pXpathNode = xpathObj->nodesetval;
	lDbId = pSetPara->ulParaId;
	if(!isObject(pSetPara->pDbHandler, lDbId))
	{
		INT8			   *pcOrgValue	   = NULL;

		pcOrgValue = (INT8 *)xmlNodeGetContent(pXpathNode->nodeTab[0]);

        DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ %s : old=%s new=%s in DbSetCurHandlerForC", pSetPara->pucName,pcOrgValue,(INT8 *)pSetPara->pValue);

		if(0 != strcmp(pcOrgValue,(INT8 *)pSetPara->pValue))
		{
            INT8 *pTranslatedValue = DbTranslateToEscapeChars(pSetPara->pValue);
			xmlNodeSetContent(pXpathNode->nodeTab[0], (const xmlChar *)(pTranslatedValue));
            DM_FREE(pTranslatedValue);
			bRet = DM_TRUE;
		}
		else
		{
			bRet = 2;
		}

		DM_FREE(pcOrgValue);
	}

	xmlXPathFreeObject(xpathObj);
	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbSetCurHandlerForC");
    return bRet;
}

/*******************************************************************************
  Module   : DB
  Function : add object or item to xml file
  Input     : pAddPara, the parameter used to add 
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, OK, others, NG
  History   :
 *******************************************************************************/
BOOL8 DbAddCurHandler(DM_HANDLER_PARA_T *pAddPara)
{
	xmlXPathContextPtr	xpathCtx; 
	xmlXPathObjectPtr	xpathParentObj;
	INT32				lDbId = 0;


	/*get the document handler*/
	if(NULL == pAddPara || NULL == pAddPara->pucName)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler parameter error");
		return DM_FALSE;
	}
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbAddCurHandler %s\n", pAddPara->pucName);
	}

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler ucXPathName = NULL");
        return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	INT8 *pstrIndex = NULL;
	pstrIndex = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (pstrIndex == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler pstrIndex = NULL");
		DM_FREE(ucXPathName);
        return DM_FALSE;
	}

	memset(pstrIndex,0,sizeof(INT8)*MAX_XML_PATH);

	
    xmlDocPtr pDoc = pAddPara->pDbHandler->pXmlDocument;
    INT8 tmpStr[1024] = {0};
    INT8 *pDescription = pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].pucDescription;
    INT8 *pValueRange = pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].pValueRange;
    INT8 *pDataType = pAddPara->pDbHandler->pDataTypeStrHooker(pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].ucParaType);
    INT8 *pCType = pAddPara->pDbHandler->pCTypeStrHooker(pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].ucParaCType);

	/*
	* if object, create an element of object and return the instanceNumber as pucValue
	* and return true
	*/

	/*
	*if element, create an element of item and write the pucValue to the value of item 
	*and return true
	*/
	lDbId = pAddPara->ulParaId;
	if(lDbId == 0) /*add the root node*/
	{
		INT8 *pucName = DmGetNodeNameFromPath(pAddPara->pucName);
		xmlNodePtr root_node = xmlNewNode(NULL, (const xmlChar *)pucName);
		xmlDocSetRootElement(pDoc,root_node);

        DbAddDefaultAttrHandler(pAddPara->pDbHandler,lDbId,root_node);

		if (pAddPara->pDbHandler->bXmlWithComment==DM_TRUE)
		{
			snprintf(tmpStr, sizeof(tmpStr), "Description : %s", pDescription);
			xmlNodePtr doc_cmt = xmlNewDocComment(pDoc, (const xmlChar *)tmpStr);/* add doc description */
			xmlAddPrevSibling(root_node, doc_cmt);
		}

		DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandler add rootnode  %s\n", pucName);
		DM_FREE(pucName);
	}
	else
	{
		if(isObject(pAddPara->pDbHandler, lDbId))
		{
			xpathCtx	=	xmlXPathNewContext(pDoc);
			if(NULL == xpathCtx)
			{
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler can't get xpathCtx");

				DM_FREE(ucXPathName);
				DM_FREE(pstrIndex);
				return DM_FALSE;
			}

		
			INT8 *pName = DmGetNodeNameFromPath(pAddPara->pucName);
			INT8 *pParentPath = DmGetNodeParentPathFromPath(pAddPara->pucName);
			memset(ucXPathName, 0, MAX_XML_PATH);
			ConvertPathToXPath(pParentPath, ucXPathName);
			 /*if multiple object,
			 *get the index of object 
			 *and add the index as an object and return it.
			 */
			if(DmIsMultipleObject(pAddPara->pDbHandler->pPattern, pAddPara->ulParaId))
			{   
				/* if the parent object isn't exist, add the parent, a.b.c. 
							   *and then add the child a.b.c.{i}. , the {i} will be replaced by index number*/
				xpathParentObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);
				if (xpathParentObj == NULL || xmlXPathNodeSetIsEmpty(xpathParentObj->nodesetval))
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler %s not exist",ucXPathName);
					DM_FREE(ucXPathName);
					DM_FREE(pstrIndex);
					DM_FREE(pName);
					DM_FREE(pParentPath);
					return DM_FALSE;
				}

				UINT8 *pucMultPathName = calloc(strlen((char*)ucXPathName) + strlen("/") + strlen((char*)pName) + 1, sizeof(UINT8));
				if(NULL == pucMultPathName)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler allocate memory error");
					DM_FREE(ucXPathName);
					DM_FREE(pstrIndex);
					DM_FREE(pName);
					DM_FREE(pParentPath);
					return DM_FALSE;
				}
				memset(pucMultPathName,0,(strlen((char*)ucXPathName) + strlen("/") + strlen((char*)pName) + 1)*sizeof(UINT8));
				xmlXPathObjectPtr	xpathmultiObj = NULL;
				strcat((char*)pucMultPathName, (char*)ucXPathName);
				strcat((char*)pucMultPathName, "/");
				strcat((char*)pucMultPathName, (char*)pName);
				xpathmultiObj = xmlXPathEvalExpression((const xmlChar *)pucMultPathName, xpathCtx);

				if (xpathmultiObj == NULL)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ failed to find xpath (%s) for multi-obj (%s) in DbAddCurHandler",xpathmultiObj,pAddPara->pucName);
					DM_FREE(ucXPathName);
					DM_FREE(pstrIndex);
					DM_FREE(pName);
					DM_FREE(pParentPath);
					return DM_FALSE;
				}

				if(xmlXPathNodeSetIsEmpty(xpathmultiObj->nodesetval))
				{
					xmlNodePtr node = xmlNewNode(NULL, (const xmlChar *)pName);
					xmlAddChild(xpathParentObj->nodesetval->nodeTab[0], node);

					if (pAddPara->pDbHandler->bXmlWithComment==DM_TRUE)
					{
						snprintf(tmpStr, sizeof(tmpStr), "Description : %s", pDescription);
						xmlNodePtr node_cmt = xmlNewComment((const xmlChar *)tmpStr);/* add node description */
						xmlAddPrevSibling(node, node_cmt);

						snprintf(tmpStr, sizeof(tmpStr), "Data Type : %s (%s)", pDataType, pCType);
						xmlNodePtr node_type = xmlNewComment((const xmlChar *)tmpStr);/* add node data-type */
						xmlAddPrevSibling(node, node_type);

						snprintf(tmpStr, sizeof(tmpStr), "Value Range : %s", pValueRange);
						xmlNodePtr node_range = xmlNewComment((const xmlChar *)tmpStr);/* add node value-range */
						xmlAddPrevSibling(node, node_range);
					}

					DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandler add node %s\n", pName);
					INT32 iNewIndex = GetNewObjectIndex(pAddPara->pDbHandler, pucMultPathName);
					snprintf((char*)pstrIndex, sizeof(INT8)*MAX_XML_PATH, "i%d", iNewIndex);
					xmlNodePtr node1 = xmlNewNode(NULL, (const xmlChar *)pstrIndex);
					xmlAddChild(node, node1);

                    DbAddDefaultAttrHandler(pAddPara->pDbHandler,pAddPara->ulParaId,node1);

					DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandler add node %s\n", pstrIndex);
					strcpy((char*)pAddPara->pValue, (char*)pstrIndex);
				}
				else
				{
					INT32 iNewIndex = GetNewObjectIndex(pAddPara->pDbHandler, pucMultPathName);
					snprintf((char*)pstrIndex, sizeof(INT8)*MAX_XML_PATH, "i%d", iNewIndex);
					xmlNodePtr node1 = xmlNewNode(NULL, (const xmlChar *)pstrIndex);
					xmlAddChild(xpathmultiObj->nodesetval->nodeTab[0], node1);

                    DbAddDefaultAttrHandler(pAddPara->pDbHandler,pAddPara->ulParaId,node1);

					DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandler add node %s\n", pstrIndex);
					strcpy((char*)pAddPara->pValue, (char*)pstrIndex);
				}
				xmlXPathFreeObject(xpathmultiObj);
				DM_FREE(pucMultPathName);
				
			}
			else
			{
				xpathParentObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);
				if (xpathParentObj == NULL || xmlXPathNodeSetIsEmpty(xpathParentObj->nodesetval))
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler %s not exist",ucXPathName);
					DM_FREE(ucXPathName);
					DM_FREE(pstrIndex);
					DM_FREE(pName);
					DM_FREE(pParentPath);
					return DM_FALSE;
				}

				xmlNodePtr node = xmlNewNode(NULL, (const xmlChar *)pName);
				xmlAddChild(xpathParentObj->nodesetval->nodeTab[0], node);

                DbAddDefaultAttrHandler(pAddPara->pDbHandler,pAddPara->ulParaId,node);

				DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandler add node %s\n", pName);

				if (pAddPara->pDbHandler->bXmlWithComment==DM_TRUE)
				{
					snprintf(tmpStr, sizeof(tmpStr), "Description : %s", pDescription);
					xmlNodePtr node_cmt = xmlNewComment((const xmlChar *)tmpStr);/* add node description */
					xmlAddPrevSibling(node, node_cmt);

					snprintf(tmpStr, sizeof(tmpStr), "Data Type : %s (%s)", pDataType, pCType);
					xmlNodePtr node_type = xmlNewComment((const xmlChar *)tmpStr);/* add node data-type */
					xmlAddPrevSibling(node, node_type);

					snprintf(tmpStr, sizeof(tmpStr), "Value Range : %s", pValueRange);
					xmlNodePtr node_range = xmlNewComment((const xmlChar *)tmpStr);/* add node value-range */
					xmlAddPrevSibling(node, node_range);
				}
	        }		
			
			xmlXPathFreeObject(xpathParentObj);
			xmlXPathFreeContext(xpathCtx); 
			DM_FREE(pParentPath);
			DM_FREE(pName);
		}
		else
		{
			INT8 *pName = DmGetNodeNameFromPath(pAddPara->pucName);
			xpathParentObj = pAddPara->pDbHandler->xpathParentObj;
			if (xpathParentObj == NULL || xmlXPathNodeSetIsEmpty(xpathParentObj->nodesetval))
			{
				DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandler %s not exist",ucXPathName);
				DM_FREE(ucXPathName);
				DM_FREE(pstrIndex);
				DM_FREE(pName);
				return DM_FALSE;
			}

			xmlNodePtr node = xmlNewNode(NULL, (const xmlChar *)pName);
			xmlAddChild(xpathParentObj->nodesetval->nodeTab[0], node);

            DbAddDefaultAttrHandler(pAddPara->pDbHandler,pAddPara->ulParaId,node);

			if (pAddPara->pValue!=NULL && strlen(pAddPara->pValue)>0)
			{
				INT8 *pTranslatedValue = DbTranslateToEscapeChars(pAddPara->pValue);
				xmlNodeSetContent(node, BAD_CAST pTranslatedValue);
				DM_FREE(pTranslatedValue);
				DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandler add node %s value %s\n", pName, pAddPara->pValue);
			}

			DM_FREE(pName);

			if (pAddPara->pDbHandler->bXmlWithComment==DM_TRUE)
			{
				snprintf(tmpStr, sizeof(tmpStr), "Description : %s", pDescription);
				xmlNodePtr node_cmt = xmlNewComment((const xmlChar *)tmpStr);/* add node description */
				xmlAddPrevSibling(node, node_cmt);

				snprintf(tmpStr, sizeof(tmpStr), "Data Type : %s (%s)", pDataType, pCType);
				xmlNodePtr node_type = xmlNewComment((const xmlChar *)tmpStr);/* add node data-type */
				xmlAddPrevSibling(node, node_type);

				snprintf(tmpStr, sizeof(tmpStr), "Value Range : %s", pValueRange);
				xmlNodePtr node_range = xmlNewComment((const xmlChar *)tmpStr);/* add node value-range */
				xmlAddPrevSibling(node, node_range);
			}
		}

	}

	DM_FREE(ucXPathName);
	DM_FREE(pstrIndex);
	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbAddCurHandler");
    return DM_TRUE;
}


/*******************************************************************************
  Module   : DB
  Function : add object or item to xml file
  Input     : pAddPara, the parameter used to add 
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, OK, others, NG
  History   :
 *******************************************************************************/
BOOL8 DbAddCurHandlerWithInstance(DM_HANDLER_PARA_T *pAddPara)
{
	xmlXPathContextPtr	xpathCtx; 
	xmlXPathObjectPtr	xpathParentObj;
	INT32				lDbId = 0;


	/*get the document handler*/
	if(NULL == pAddPara || NULL == pAddPara->pucName)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandlerWithInstance parameter error");
		return DM_FALSE;
	}
	else
	{
	    DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbAddCurHandlerWithInstance %s\n", pAddPara->pucName);
	}

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandlerWithInstance ucXPathName = NULL");
        return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	INT8 *pstrIndex = NULL;
	pstrIndex = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (pstrIndex == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandlerWithInstance ucXPathName = NULL");
		DM_FREE(ucXPathName);
        return DM_FALSE;
	}

	memset(pstrIndex,0,sizeof(INT8)*MAX_XML_PATH);

	
    xmlDocPtr pDoc = pAddPara->pDbHandler->pXmlDocument;
    INT8 tmpStr[1024] = {0};
    INT8 *pDescription = pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].pucDescription;
    INT8 *pValueRange = pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].pValueRange;
    INT8 *pDataType = pAddPara->pDbHandler->pDataTypeStrHooker(pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].ucParaType);
    INT8 *pCType = pAddPara->pDbHandler->pCTypeStrHooker(pAddPara->pDbHandler->pPattern[pAddPara->ulParaId].ucParaCType);

	/*
	* if object, create an element of object and return the instanceNumber as pucValue
	* and return true
	*/

	/*
	*if element, create an element of item and write the pucValue to the value of item 
	*and return true
	*/
		lDbId = pAddPara->ulParaId;
		if(lDbId == 0) /*add the root node*/
		{
		    INT8 *pucName = DmGetNodeNameFromPath(pAddPara->pucName);
		    xmlNodePtr root_node = xmlNewNode(NULL, (const xmlChar *)pucName);
			xmlDocSetRootElement(pDoc,root_node);

            DbAddDefaultAttrHandler(pAddPara->pDbHandler,lDbId,root_node);

            if (pAddPara->pDbHandler->bXmlWithComment==DM_TRUE)
            {
                snprintf(tmpStr, sizeof(tmpStr), "Description : %s", pDescription);
                xmlNodePtr doc_cmt = xmlNewDocComment(pDoc, (const xmlChar *)tmpStr);/* add doc description */
                xmlAddPrevSibling(root_node, doc_cmt);
            }

			DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandlerWithInstance add rootnode  %s\n", pucName);
			DM_FREE(pucName);
		}
		else
		{
 			if(isObject(pAddPara->pDbHandler, lDbId))
			{
				xpathCtx	=	xmlXPathNewContext(pDoc);
				if(NULL == xpathCtx)
				{
					DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandlerWithInstance can't get xpathCtx");

					DM_FREE(ucXPathName);
					DM_FREE(pstrIndex);
					return DM_FALSE;
				}

			
				INT8 *pName = DmGetNodeNameFromPath(pAddPara->pucName);
				INT8 *pParentPath = DmGetNodeParentPathFromPath(pAddPara->pucName);
				memset(ucXPathName, 0, MAX_XML_PATH);
				ConvertPathToXPath(pParentPath, ucXPathName);
                 /*if multiple object,
				 *get the index of object 
				 *and add the index as an object and return it.
				 */
	            if(DmIsMultipleObject(pAddPara->pDbHandler->pPattern, pAddPara->ulParaId))
	        	{   
                    /* if the parent object isn't exist, add the parent, a.b.c. 
                                   *and then add the child a.b.c.{i}. , the {i} will be replaced by index number*/
					xpathParentObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);
                    if (xpathParentObj == NULL || xmlXPathNodeSetIsEmpty(xpathParentObj->nodesetval))
                    {
                        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandlerWithInstance %s not exist",ucXPathName);
						DM_FREE(ucXPathName);
						DM_FREE(pstrIndex);
						DM_FREE(pName);
                        DM_FREE(pParentPath);
						return DM_FALSE;
                    }

					UINT8 *pucMultPathName = calloc(strlen((char*)ucXPathName) + strlen("/") + strlen((char*)pName) + 1, sizeof(UINT8));
					if(NULL == pucMultPathName)
					{
					    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandlerWithInstance allocate memory error");
						DM_FREE(ucXPathName);
						DM_FREE(pstrIndex);						
						return DM_FALSE;
					}
					memset(pucMultPathName,0,(strlen((char*)ucXPathName) + strlen("/") + strlen((char*)pName) + 1)*sizeof(UINT8));
					xmlXPathObjectPtr	xpathmultiObj = NULL;
					strcat((char*)pucMultPathName, (char*)ucXPathName);
					strcat((char*)pucMultPathName, "/");
					strcat((char*)pucMultPathName, (char*)pName);
					xpathmultiObj = xmlXPathEvalExpression((const xmlChar *)pucMultPathName, xpathCtx);

                    if (xpathmultiObj == NULL)
                    {
                        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ failed to find xpath (%s) for multi-obj (%s) in DbAddCurHandlerWithInstance",xpathmultiObj,pAddPara->pucName);
                        DM_FREE(ucXPathName);
                        DM_FREE(pstrIndex);
                        DM_FREE(pName);
                        DM_FREE(pParentPath);
                        return DM_FALSE;
                    }

					if(xmlXPathNodeSetIsEmpty(xpathmultiObj->nodesetval))
					{
					    xmlNodePtr node = xmlNewNode(NULL, (const xmlChar *)pName);
						xmlAddChild(xpathParentObj->nodesetval->nodeTab[0], node);

                        if (pAddPara->pDbHandler->bXmlWithComment==DM_TRUE)
                        {
                            snprintf(tmpStr, sizeof(tmpStr), "Description : %s", pDescription);
                            xmlNodePtr node_cmt = xmlNewComment((const xmlChar *)tmpStr);/* add node description */
                            xmlAddPrevSibling(node, node_cmt);

                            snprintf(tmpStr, sizeof(tmpStr), "Data Type : %s (%s)", pDataType, pCType);
                            xmlNodePtr node_type = xmlNewComment((const xmlChar *)tmpStr);/* add node data-type */
                            xmlAddPrevSibling(node, node_type);

                            snprintf(tmpStr, sizeof(tmpStr), "Value Range : %s", pValueRange);
                            xmlNodePtr node_range = xmlNewComment((const xmlChar *)tmpStr);/* add node value-range */
                            xmlAddPrevSibling(node, node_range);
                        }

                        DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandlerWithInstance add node %s\n", pName);
						INT32 iNewIndex = *((UINT32 *)pAddPara->pValue);
						snprintf((char*)pstrIndex, sizeof(INT8)*MAX_XML_PATH, "i%d", iNewIndex);
						xmlNodePtr node1 = xmlNewNode(NULL, (const xmlChar *)pstrIndex);
						xmlAddChild(node, node1);

                        DbAddDefaultAttrHandler(pAddPara->pDbHandler,pAddPara->ulParaId,node1);

						DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandlerWithInstance add node %s\n", pstrIndex);
					}
					else
					{
					    INT32 iNewIndex = *((UINT32 *)pAddPara->pValue);
						
						snprintf((char*)pstrIndex, sizeof(INT8)*MAX_XML_PATH, "i%d", iNewIndex);
						xmlNodePtr node1 = xmlNewNode(NULL, (const xmlChar *)pstrIndex);
						xmlAddChild(xpathmultiObj->nodesetval->nodeTab[0], node1);

                        DbAddDefaultAttrHandler(pAddPara->pDbHandler,pAddPara->ulParaId,node1);

						DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandlerWithInstance add node %s\n", pstrIndex);
					}
					xmlXPathFreeObject(xpathmultiObj);
					DM_FREE(pucMultPathName);
					
	        	}
				else
				{
				    xpathParentObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);
                    if (xpathParentObj == NULL || xmlXPathNodeSetIsEmpty(xpathParentObj->nodesetval))
                    {
                        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandlerWithInstance %s not exist",ucXPathName);
						DM_FREE(ucXPathName);
						DM_FREE(pstrIndex);
						DM_FREE(pName);
                        DM_FREE(pParentPath);
						return DM_FALSE;
                    }

		    		xmlNodePtr node = xmlNewNode(NULL, (const xmlChar *)pName);
					xmlAddChild(xpathParentObj->nodesetval->nodeTab[0], node);

                    DbAddDefaultAttrHandler(pAddPara->pDbHandler,pAddPara->ulParaId,node);

					DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandlerWithInstance add node %s\n", pName);

                    if (pAddPara->pDbHandler->bXmlWithComment==DM_TRUE)
                    {
                        snprintf(tmpStr, sizeof(tmpStr), "Description : %s", pDescription);
                        xmlNodePtr node_cmt = xmlNewComment((const xmlChar *)tmpStr);/* add node description */
                        xmlAddPrevSibling(node, node_cmt);
                        
                        snprintf(tmpStr, sizeof(tmpStr), "Data Type : %s (%s)", pDataType, pCType);
                        xmlNodePtr node_type = xmlNewComment((const xmlChar *)tmpStr);/* add node data-type */
                        xmlAddPrevSibling(node, node_type);
                        
                        snprintf(tmpStr, sizeof(tmpStr), "Value Range : %s", pValueRange);
                        xmlNodePtr node_range = xmlNewComment((const xmlChar *)tmpStr);/* add node value-range */
                        xmlAddPrevSibling(node, node_range);
                    }
				}
	        	
				
				
				xmlXPathFreeObject(xpathParentObj);
				xmlXPathFreeContext(xpathCtx); 
 				DM_FREE(pParentPath);
				DM_FREE(pName);
 			}
	        else
	    	{
	    	    INT8 *pName = DmGetNodeNameFromPath(pAddPara->pucName);
				xpathParentObj = pAddPara->pDbHandler->xpathParentObj;
                if (xpathParentObj == NULL || xmlXPathNodeSetIsEmpty(xpathParentObj->nodesetval))
                {
                    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbAddCurHandlerWithInstance %s not exist",ucXPathName);
					DM_FREE(ucXPathName);
					DM_FREE(pstrIndex);
					DM_FREE(pName);
					return DM_FALSE;
                }

	    		xmlNodePtr node = xmlNewNode(NULL, (const xmlChar *)pName);
				xmlAddChild(xpathParentObj->nodesetval->nodeTab[0], node);

                DbAddDefaultAttrHandler(pAddPara->pDbHandler,pAddPara->ulParaId,node);

                if (pAddPara->pValue!=NULL && strlen(pAddPara->pValue)>0)
                {
                    INT8 *pTranslatedValue = DbTranslateToEscapeChars(pAddPara->pValue);
    				xmlNodeSetContent(node, BAD_CAST pTranslatedValue);
                    DM_FREE(pTranslatedValue);
    				DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ DbAddCurHandlerWithInstance add node %s value %s\n", pName, pAddPara->pValue);
                }

				DM_FREE(pName);

                if (pAddPara->pDbHandler->bXmlWithComment==DM_TRUE)
                {
                    snprintf(tmpStr, sizeof(tmpStr), "Description : %s", pDescription);
                    xmlNodePtr node_cmt = xmlNewComment((const xmlChar *)tmpStr);/* add node description */
                    xmlAddPrevSibling(node, node_cmt);
                    
                    snprintf(tmpStr, sizeof(tmpStr), "Data Type : %s (%s)", pDataType, pCType);
                    xmlNodePtr node_type = xmlNewComment((const xmlChar *)tmpStr);/* add node data-type */
                    xmlAddPrevSibling(node, node_type);
                    
                    snprintf(tmpStr, sizeof(tmpStr), "Value Range : %s", pValueRange);
                    xmlNodePtr node_range = xmlNewComment((const xmlChar *)tmpStr);/* add node value-range */
                    xmlAddPrevSibling(node, node_range);
                }
	    	}

		}

	DM_FREE(ucXPathName);
	DM_FREE(pstrIndex);	
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbAddCurHandlerWithInstance");
    return DM_TRUE;
}




/*******************************************************************************
  Module   : DB
  Function : del datavalue from xml file
  Input     : pDelPara, the parameter used to del
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, OK, others, NG
  History   :
 *******************************************************************************/
BOOL8 DbDelCurHandler(DM_HANDLER_PARA_T *pDelPara)
{
	
	xmlXPathContextPtr	xpathCtx; 
	xmlXPathObjectPtr	xpathObj; 
	xmlNodeSetPtr		pXpathNode	= NULL;
	INT32               i = 0;
	INT8 *ucXPathName = NULL;
    xmlDocPtr pDoc = NULL;

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbDelCurHandler");

	/*get the document handler*/

	if(!pDelPara ||!pDelPara->pucName)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbDelCurHandler pDelPara is null");
		return DM_FALSE;
	}

	pDoc = pDelPara->pDbHandler->pXmlDocument;

    if(!pDoc)
    {
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbDelCurHandler xml content is error");
        return DM_FALSE;
    }


	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbDelCurHandler ucXPathName = NULL");
        return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);


	/*change the path to the xpath*/
	ConvertPathToXPath(pDelPara->pucName, ucXPathName);
	xpathCtx	=	xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);


	DM_FREE(ucXPathName);
	/*check the element is existing, not existing, return error
	*/

	/*delete the element and return true*/
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))    
    {
        xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx);
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbDelCurHandler xml content is error");
        return DM_FALSE;
    }
	else
	{
	    pXpathNode = xpathObj->nodesetval;
		for(i = 0; i < pXpathNode->nodeNr; i++)
		{
            if (pDelPara->pDbHandler->bXmlWithComment==DM_TRUE)
            {
                xmlNodePtr commentPtr = NULL;

                /* delete its value-range comment */
                commentPtr = xmlPreviousElementSibling(pXpathNode->nodeTab[i]);
                if (commentPtr != NULL && commentPtr->type == XML_COMMENT_NODE)
                {
                    xmlUnlinkNode(commentPtr);
        			xmlFreeNode(commentPtr);
                }

                /* delete its data-type comment */
                commentPtr = xmlPreviousElementSibling(pXpathNode->nodeTab[i]);
                if (commentPtr != NULL && commentPtr->type == XML_COMMENT_NODE)
                {
                    xmlUnlinkNode(commentPtr);
        			xmlFreeNode(commentPtr);
                }

                /* delete its description comment */
                commentPtr = xmlPreviousElementSibling(pXpathNode->nodeTab[i]);
                if (commentPtr != NULL && commentPtr->type == XML_COMMENT_NODE)
                {
                    xmlUnlinkNode(commentPtr);
        			xmlFreeNode(commentPtr);
                }
            }

            /* delete the node */
		    xmlUnlinkNode(pXpathNode->nodeTab[i]);
			xmlFreeNode(pXpathNode->nodeTab[i]);
		}
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx); 

	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbDelCurHandler");
    return DM_TRUE;
}

/*******************************************************************************
  Module   : DB
  Function : set property for node in DB xml file
  Input    : pucPath, the node path
             pucAttrName, the attribute name
             pucAttrValue, the attribute value
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, OK, others, NG
  History   :
 *******************************************************************************/
BOOL8 DbSetNodeAttribute(INT8 *pucPath, INT8 *pucAttrName, INT8 *pucAttrValue)
{
	xmlNodePtr 			pNode = NULL;
    DM_HANDLER_T 	   *pDmHandler = NULL;

	xmlXPathContextPtr	xpathCtx; 
	xmlXPathObjectPtr	xpathObj; 
	xmlNodeSetPtr		pNodeSet = NULL;

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbSetNodeAttribute");

	/*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pucPath);
	if (pDmHandler == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetNodeAttribute pDmHandler = NULL");
        return DM_FALSE;
	}

	/*get the document handler*/
	xmlDocPtr pDoc = pDmHandler->pXmlDocument;
	if(!pDoc || !pucPath ||!pucAttrName ||!pucAttrValue)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetNodeAttribute input parameter is NULL");
		return DM_FALSE;
	}
	
	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetNodeAttribute ucXPathName = NULL");
        return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	
	/*change the path to the xpath*/
	ConvertPathToXPath(pucPath, ucXPathName);
	xpathCtx	=	xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);


	DM_FREE(ucXPathName);
	/*check the element is existing, not existing, return error
	*/

	/*delete the element and return true*/
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))	
	{
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx);
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetNodeAttribute xml content is error");
		return DM_FALSE;
	}
	else
	{
		pNodeSet = xpathObj->nodesetval;

		pNode = pNodeSet->nodeTab[0];

		xmlSetProp(pNode, (const xmlChar *)pucAttrName, (const xmlChar *)pucAttrValue);
		
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx);
	}
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbSetNodeAttribute");
	return DM_TRUE;
}

/*******************************************************************************
  Module   : DB
  Function : unset property for node in DB xml file
  Input    : pucPath, the node path
             pucAttrName, the attribute name
  Output   : null
  NOTE     : null
  Return   : DM_TRUE, OK, others, NG
  History  :
 *******************************************************************************/
BOOL8 DbUnsetNodeAttribute(INT8 *pucPath, INT8 *pucAttrName)
{
	xmlNodePtr 			pNode = NULL;
    DM_HANDLER_T 	   *pDmHandler = NULL;

	xmlXPathContextPtr	xpathCtx; 
	xmlXPathObjectPtr	xpathObj; 
	xmlNodeSetPtr		pNodeSet = NULL;
	INT32 				lResult = 0;

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbSetNodeAttribute");

	/*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pucPath);
	if (pDmHandler == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbUnsetNodeAttribute pDmHandler = NULL");
        return DM_FALSE;
	}

	/*get the document handler*/
	xmlDocPtr pDoc = pDmHandler->pXmlDocument;
	if(!pDoc || !pucPath ||!pucAttrName)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetNodeAttribute input parameter is NULL");
		return DM_FALSE;
	}

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbUnsetNodeAttribute ucXPathName = NULL");
        return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);
	
	/*change the path to the xpath*/
	ConvertPathToXPath(pucPath, ucXPathName);
	xpathCtx	=	xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

	DM_FREE(ucXPathName);
	
	/*check the element is existing, not existing, return error
	*/

	
	/*delete the element and return true*/
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))	
	{
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx);
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbSetNodeAttribute xml content is error");
		return DM_FALSE;
	}
	else
	{
		pNodeSet = xpathObj->nodesetval;

		pNode = pNodeSet->nodeTab[0];

		lResult = xmlUnsetProp(pNode,(const xmlChar *)pucAttrName);
		
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx);
	}

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbSetNodeAttribute");

	if(-1 == lResult)
	{
		return DM_FALSE;
	}

	return DM_TRUE;
}



/*******************************************************************************
  Module   : DB
  Function : get number of object
  Input     : pDmHandler, the database handler
                 pucPath, the data path
  Output   : null
  NOTE     : null
  Return    : the number of object
  History   :
 *******************************************************************************/
INT32 DbGetObjectNum(DM_HANDLER_T *pDmHandler, INT8 *pucPath)
{
    INT32               lRet = 0;
	xmlXPathContextPtr	xpathCtx; 
	xmlXPathObjectPtr	xpathObj; 
	xmlNodeSetPtr		pXpathNode	= NULL;
	INT32				i = 0;
	INT32         		lIndex     = 0;

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbGetObjectNum %s", pucPath);
	/*get the document handler*/
	
	if(!pucPath ||!pDmHandler)
	{
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetObjectNum xml content is error");
		return DM_FALSE;
	}
	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetObjectNum ucXPathName = NULL");
        return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	
	xmlDocPtr pDoc = pDmHandler->pXmlDocument;
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
            DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetObjectNum(%s=%d), lIndex invalid, leave DbGetObjectNum",pucPath,lRet);
		    return 0;
		}
		if(DM_TRUE == DmIsMultipleObject(pDmHandler->pPattern,lIndex))
		{
			strcat((char*)ucXPathName,"/*");
		}
	}

	
	xpathCtx	=	xmlXPathNewContext(pDoc);
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);

	free(ucXPathName);
	
	
	/*check the element is existing, not existing, return error
	*/

	/*delete the element and return true*/
	if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))	
	{
        DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DB Trace ------ DbGetObjectNum(%s=%d), null objs, leave DbGetObjectNum",pucPath,lRet);
	    if(xpathObj != NULL)
    	{
    	    xmlXPathFreeObject(xpathObj);
    	}
	    xmlXPathFreeContext(xpathCtx);
		return lRet;
	}
	else
	{
		pXpathNode = xpathObj->nodesetval;
		lRet = pXpathNode->nodeNr;
	}
	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);
	DM_TRACE(DM_TRACE_LEVEL_MIDDLE, "DB Trace ------ DbGetObjectNum(%s=%d) leave DbGetObjectNum",pucPath,lRet);

	return lRet;
}
/*******************************************************************************
  Module   : DB
  Function : get all the index of object
  Input     : pDmHandler, the database handler
                 pucPath, the object path
                 pValue, the count and indexes.
  Output   : null
  NOTE     : null
  Return    : DM_TRUE, OK, Others, NG
  History   :
 *******************************************************************************/
BOOL8 DbGetObjectIndexCont(DM_HANDLER_T *pDmHandler, INT8 *pucPath, UINT32 *pValue)
{    /*the type of instance is  UINT32*/
	BOOL8				bRet = DM_FALSE;
	xmlXPathObjectPtr	xpathObj; 
	xmlNodeSetPtr		pXpathNode	= NULL;
	UINT32				i = 0;
	INT32         		lIndex     = 0;

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbGetObjectIndexCont %s", pucPath);
	/*get the document handler*/
	
	if(!pucPath ||!pDmHandler)
	{
	    *pValue = 0;
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetObjectIndexCont xml content is error");
		return bRet;
	}

	xpathObj = xmlXPathEvalExpression((const xmlChar *)"*", pDmHandler->xpathCtx);

	/*check the element is existing, not existing, return error
	*/

	/*delete the element and return true*/
	if(xpathObj == NULL)
	{
	    *pValue = 0;
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetObjectIndexCont xml content is error");
		return bRet;
	}
	if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))	
	{
		*pValue = 0;
		xmlXPathFreeObject(xpathObj);
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetObjectIndexCont");
		return DM_TRUE;
	}
	else
	{
		lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID),pucPath);
		if(lIndex < 0)
		{
		    *pValue = 0;
		    return bRet;
		}
		UINT32 ulMaxInstNum = (UINT32)-1;
		if(0 != pDmHandler->pPattern[lIndex].pValueRange[0])
		{
			ulMaxInstNum = (UINT32)atoi((const char *)(pDmHandler->pPattern[lIndex].pValueRange));
		}


		pXpathNode = xpathObj->nodesetval;
		*pValue = pXpathNode->nodeNr;

		if(*pValue > ulMaxInstNum)
		{
			*pValue = ulMaxInstNum;
		}
		else
		{
			ulMaxInstNum = pXpathNode->nodeNr;
		}

		
		pValue++;

		for(i = 0; i < ulMaxInstNum; i++)
		{
		    *pValue = atoi((const char *)(pXpathNode->nodeTab[i]->name+1));
			pValue++;
		}
	}
	
	xmlXPathFreeObject(xpathObj);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetObjectIndexCont");

     return DM_TRUE;
}

BOOL8 DbGetObjectIndexContOld(DM_HANDLER_T *pDmHandler, INT8 *pucPath, UINT32 **pValue)
{    /*the type of instance is  UINT32*/
	BOOL8				bRet = DM_FALSE;
	xmlXPathContextPtr	xpathCtx; 
	xmlXPathObjectPtr	xpathObj; 
	xmlNodeSetPtr		pXpathNode	= NULL;
	UINT32				i = 0;
	INT32         		lIndex     = 0;

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbGetObjectIndexCont %s", pucPath);

	/*get the document handler*/
	
	if(!pucPath ||!pDmHandler)
	{
	    *pValue = 0;
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetObjectIndexCont xml content is error");
		return bRet;
	}
	
	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetObjectIndexContOld ucXPathName = NULL");
        return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	
	xmlDocPtr pDoc = pDmHandler->pXmlDocument;

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
		    *pValue = 0;
			DM_FREE(ucXPathName);
		    return bRet;
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
	if(xpathObj == NULL)
	{
	    *pValue = 0;
		xmlXPathFreeContext(xpathCtx);
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetObjectIndexCont xml content is error");
		return bRet;
	}
	if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))	
	{
        *pValue = 0;
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx);
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetObjectIndexCont");
		return DM_TRUE;
	}
	else
	{
		lIndex = DmGetIDFromPath(&(pDmHandler->hashNameID),pucPath);
		if(lIndex < 0)
		{
		    *pValue = 0;
		    return bRet;
		}
		UINT32 ulMaxInstNum = (UINT32)-1;
		if(0 != pDmHandler->pPattern[lIndex].pValueRange[0])
		{
			ulMaxInstNum = (UINT32)atoi((const char *)(pDmHandler->pPattern[lIndex].pValueRange));
            
            *pValue = (UINT32 *)malloc(sizeof(UINT32)*(ulMaxInstNum+1));
            if(*pValue==NULL)
            {
                xmlXPathFreeObject(xpathObj);
            	xmlXPathFreeContext(xpathCtx);
                return bRet;
            }
            memset(*pValue,0,sizeof(UINT32)*(ulMaxInstNum+1));
		}
        else
        {
            *pValue = (UINT32 *)malloc(sizeof(UINT32)*(DM_MAXOBJ_NUM+1));
            if(*pValue==NULL)
            {
                xmlXPathFreeObject(xpathObj);
            	xmlXPathFreeContext(xpathCtx);
                return bRet;
            }
            memset(*pValue,0,sizeof(UINT32)*(DM_MAXOBJ_NUM+1));
        }

        UINT32 *pObjIndex = NULL;

        pObjIndex = *pValue;

		pXpathNode = xpathObj->nodesetval;
		*pObjIndex = pXpathNode->nodeNr;

		if(*pObjIndex > ulMaxInstNum)
		{
			*pObjIndex = ulMaxInstNum;
		}
		else
		{
			ulMaxInstNum = pXpathNode->nodeNr;
		}

		
		pObjIndex++;

		for(i = 0; i < ulMaxInstNum; i++)
		{
		    *pObjIndex = atoi((const char *)(pXpathNode->nodeTab[i]->name+1));
			pObjIndex++;
		}
	}
	
	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetObjectIndexCont");

     return DM_TRUE;
}

/*******************************************************************************
  Module   : DbGetFirstInstanceNum
  Function : get first instance number of specified multiple object
  Input     : pucPath, the item path
  Output   : pValue, the first instance number
  NOTE     : 
  Return   :  DM_TRUE, DM_FALSE
  History  :
 *******************************************************************************/
BOOL8 DbGetFirstInstanceNum(INT8 *pucPath, UINT32 *pValue)
{    /*the type of instance is  UINT32*/
	xmlXPathContextPtr	xpathCtx; 
	xmlXPathObjectPtr	xpathObj; 
	xmlNodeSetPtr		pXpathNode	= NULL;
	INT32				i = 0;
	INT32         		lIndex     = 0;

	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ enter DbGetFirstInstanceNum %s", pucPath);
	/*get the document handler*/
	
	if(!pucPath)
	{
	    *pValue = 0;
		return DM_FALSE;
	}

    DM_HANDLER_T   *pDmHandler = NULL;
    /*get the dm handler*/
    pDmHandler = GetDmHandlerByName(pucPath);

	if (pDmHandler == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetFirstInstanceNum pDmHandler = NULL");
        return DM_FALSE;
	}

	INT8 *ucXPathName = NULL;
	ucXPathName = (INT8 *)malloc(sizeof(INT8)*MAX_XML_PATH);

	if (ucXPathName == NULL)
	{
	    DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetFirstInstanceNum ucXPathName = NULL");
        return DM_FALSE;
	}

	memset(ucXPathName,0,sizeof(INT8)*MAX_XML_PATH);

	
	xmlDocPtr pDoc = pDmHandler->pXmlDocument;

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
		    *pValue = 0;
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

	free(ucXPathName);
	
	/*check the element is existing, not existing, return error
	*/

	/*delete the element and return true*/
	if(xpathObj == NULL)
	{
	    *pValue = 0;
		xmlXPathFreeContext(xpathCtx);
		DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ DbGetObjectIndexCont xml content is error");
		return DM_FALSE;
	}
	if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))	
	{
		*pValue = 0;
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx);
		DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetObjectIndexCont");
		return DM_FALSE;
	}
	else
	{
		pXpathNode = xpathObj->nodesetval;
	    *pValue = atoi((const char *)(pXpathNode->nodeTab[0]->name+1));
	}
	
	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);
	DM_TRACE(DM_TRACE_LEVEL_LOW, "DB Trace ------ leave DbGetFirstInstanceNum");

     return DM_TRUE;
}



/*******************************************************************************
  Module   : DB
  Function : get X-Path object pointer for one specified path
  Input    : pDbHandler, the database handler
             pucPath, the item path
  Output   : 
  NOTE     : 
  Return   : xmlXPathObjectPtr
  History  :
 *******************************************************************************/
xmlXPathObjectPtr DbGetXPathObjPtr(xmlXPathContextPtr xpathCtx,INT8 *ucXPathName)
{
    xmlXPathObjectPtr   xpathObj = NULL;
    
	xpathObj = xmlXPathEvalExpression((const xmlChar *)ucXPathName, xpathCtx);
    
	/*check the element is existing, not existing, return error
	* exist, get the element to be set parameter
	*/
    if(xpathObj == NULL || xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        if(NULL != xpathObj)
    	{
    	   xmlXPathFreeObject(xpathObj);
    	}
        DM_TRACE(DM_TRACE_LEVEL_HIGH, "DB Trace ------ xml node \"%s\" is empty",ucXPathName);
        return NULL;
    }

    xpathCtx->node = xpathObj->nodesetval->nodeTab[0];
    
    return xpathObj;
}

INT32 DbAddDefaultAttrHandler(DM_HANDLER_T *pDmHandler, INT32 lDbId, xmlNodePtr nodePtr)
{
    INT32 lRet = DM_FAILURE;

    if (pDmHandler->pPattern[lDbId].pSupportedXmlAttributes)
    {
        INT8 *pAttributeList = strdup(pDmHandler->pPattern[lDbId].pSupportedXmlAttributes);
        INT8 *pAttrName = pAttributeList;
        INT8 *pTmp = NULL;

        pAttrName = strtok_r(pAttrName, ",", &pTmp);
        while(pAttrName)
        {
            if (DmCheckIfAttrCreatedByDefault(pDmHandler, lDbId, pAttrName)==DM_TRUE)
            {
                INT8 *pAttrValue = DmGetDefaultAttrValue(pDmHandler, lDbId, pAttrName);

                xmlNewProp(nodePtr, (const xmlChar *)pAttrName, (const xmlChar *)pAttrValue);
            }

            pAttrName = strtok_r(NULL, ",", &pTmp);
        }

        DM_FREE(pAttributeList);
    }

    return lRet;
}

