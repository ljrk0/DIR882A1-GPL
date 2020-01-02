/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : CLI_CmdTree.c
 文件描述 :


 函数列表 :


 修订记录 :
          1 创建 : liuzequn
            日期 : 2007-10-26
            描述 :

**********************************************************************/

//#include "cli_cmdtree.h"
#include "cli_cmdreg.h"
#include "cli_term.h"
#include "cli_interpret.h"

#include "cli_io.h"

/******************************************************************************
 *                                GLOBAL                                      *
 ******************************************************************************/


ST_CLI_MODE m_sCliSysModes[MAX_MODE_NUM];    /* 命令树 */
ST_CLI_USERTYPE m_sCliUserType[CLI_MAX_USERTYPES]; /* 用户类型 */

/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/


/*********************************************************************/
/* 函数名称 : CLI_InitUserType()                                      */
/* 函数功能 : 可供用户自己扩展的参数值数据结构初始化                 */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 无                                                     */
/* 上层函数 : 初始化过程中调用                                       */
/* 创建者   : liuzequn                                               */
/* 修改记录 :                                                        */
/*********************************************************************/
_VOID     CLI_InitUserType(_VOID   )
{
    ULONG  i;

    for(i = 0; i < CLI_MAX_USERTYPES; i++)
    {
        m_sCliUserType[i].ucTypeID = 0;
        m_sCliUserType[i].szTypeName[0] = 0;
        m_sCliUserType[i].pCheckFunc = NULL;
    }

    return ;
}

/*  系统模式结构数组初始化  */
_VOID     CLI_InitSysModes(_VOID   )
{
    ULONG  ulCountTemp;

    for(ulCountTemp = 0; ulCountTemp < MAX_MODE_NUM; ulCountTemp++)
    {
        m_sCliSysModes[ulCountTemp].ulMode          = NULL_MODE;
        m_sCliSysModes[ulCountTemp].szModeName[0]   = 0;
        m_sCliSysModes[ulCountTemp].szModePrompt[0] = 0;
        m_sCliSysModes[ulCountTemp].rightLevel      = CLI_AL_QUERY;
        m_sCliSysModes[ulCountTemp].ulParent        = NULL_MODE;
        m_sCliSysModes[ulCountTemp].pCmdKey         = NULL;
        m_sCliSysModes[ulCountTemp].ulHierarchy     = 0;
        m_sCliSysModes[ulCountTemp].ulParamCount    = 0;
    }

    return ;
}

ULONG  CLI_InitModeObject(_VOID   )
{
    PST_CLI_CMDKEY pObj;

    pObj = CLI_CreateObject("GENERAL", CTM_GENL, CLI_AL_QUERY,
                            NULL, FALSE, CLI_MC_NULL, NULL);
    if(pObj == NULL)
        return TBS_FAILED;

    if(CLI_RegModeObject(ROOT_MODE,pObj,0) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    pObj = CLI_CreateObject("GLOBAL", CTM_GLOBAL, CLI_AL_QUERY,
                            NULL, FALSE, CLI_MC_NULL, NULL);
    if(pObj == NULL)
        return TBS_FAILED;

    if (CLI_RegModeObject(CTM_GLOBAL,pObj,0) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

ULONG   CLI_InitCmdTree(ST_CMD_MODE_INFO *pModeInfo)
{
    int i ;
    for (i = 0; pModeInfo[i].ulMode < CTM_TOTAL; i++ )
    {
        if (CLI_FAILED(CLI_SetMode(pModeInfo[i].ulMode,
                    pModeInfo[i].szModeName, pModeInfo[i].szModePrompt,
                    pModeInfo[i].rightLevel, pModeInfo[i].ulParent, NULL)))
        {
            STB_Printf("\r\n  Mode initial failed!");
            return TBS_FAILED;
        }
    }

    for (i = 0; pModeInfo[i].ulMode < CTM_TOTAL; i++ )
    {
        PST_CLI_CMDKEY pObj;

        if (pModeInfo[i].strObjHelp == NULL)
        {
            pObj = CLI_CreateObject(pModeInfo[i].szModeName, pModeInfo[i].ulMode,
                        pModeInfo[i].rightLevel, NULL, FALSE, CLI_MC_NULL, NULL);
            if(pObj == NULL || CLI_RegModeObject(pModeInfo[i].ulMode,pObj,0) != TBS_SUCCESS)
            {
                STB_Printf("\r\n  Root object register failed!");
                return TBS_FAILED;
            }
        }
        else
        {
            char szCmd[CLI_MAX_CMD_FORMAT];
            sprintf(szCmd, "%s%s", CMD_CM, pModeInfo[i].szModeName);
            if (CLI_FAILED(CLI_CmdAutoRegist(szCmd,
                                   (UCHAR)pModeInfo[i].ulParent,
                                   pModeInfo[i].rightLevel,
                                   pModeInfo[i].pFunction,
                                   CLI_MC_NEW,
                                   (UCHAR)pModeInfo[i].ulMode,
                                   CMDHELP_GLB_CM,
                                   pModeInfo[i].strObjHelp,
                                   CLI_ML_NULL)))
            {
                STB_Printf("\r\n  Mode command register failed(%s, %s)", pModeInfo[i].szModeName, szCmd);
                return TBS_FAILED;
            }
        }
        if (pModeInfo[i].ulMode != CTM_GENL && pModeInfo[i].ulMode != CTM_GLOBAL)
        {
            ULONG  ulRet = 0;
            ulRet += CLI_CmdAutoRegist(CMD_CM "..",
                                       (UCHAR)pModeInfo[i].ulMode,
                                       CLI_AL_QUERY,
                                       NULL,
                                       CLI_MC_EXIT,
                                       NULL_MODE,
                                       CMDHELP_GLB_CM,
                                       CMDHELP_GLB_CM_PARENT,
                                       CLI_ML_NULL);
            ulRet += CLI_CmdAutoRegist(CMD_CM "root",
                                       (UCHAR)pModeInfo[i].ulMode,
                                       CLI_AL_QUERY,
                                       NULL,
                                       CLI_MC_END,
                                       NULL_MODE,
                                       CMDHELP_GLB_CM,
                                       CMDHELP_GLB_CM_ROOT,
                                       CLI_ML_NULL);
    #if 0
            if (ulMode != CTM_DIAG)
            {
                ulRet += CLI_CmdAutoRegist(CMD_CM "diagnose",
                                           (UCHAR)ulMode,
                                           CLI_AL_DEBUG,
                                           CLI_EnterDiagMode,
                                           CLI_MC_NULL,
                                           NULL_MODE,
                                           CMDHELP_GLB_CM,
                                           CMDHELP_GENL_CM_DIAGNOSE,
                                           CLI_ML_NULL);
            }
    #endif
            if (CLI_FAILED(ulRet))
            {
                STB_Printf("\r\n  Mode back command register failed");
                return TBS_FAILED;
            }

        }

    }

    return TBS_SUCCESS;
}

ULONG  CLI_RegisterUserType(ULONG  *ulID,char  *szName,CLI_TYPECHECK_FUNC_T pFunc)
{
    ULONG  i;

    if((ulID == NULL) || (pFunc == NULL))
    {
        return TBS_FAILED;
    }

    for(i = 0; i < CLI_MAX_USERTYPES; i++)
    {
        if(m_sCliUserType[i].ucTypeID != (CLI_DATA_USER+i))
        {
            if(m_sCliUserType[i].ucTypeID == 0)
            {
                m_sCliUserType[i].ucTypeID = (UCHAR)(CLI_DATA_USER + i);
                if(szName != NULL)
                {
                    STB_StrCpy(&m_sCliUserType[i].szTypeName[0],szName);
                }
                m_sCliUserType[i].pCheckFunc = pFunc;
                *ulID = m_sCliUserType[i].ucTypeID;
                return TBS_SUCCESS;
            }
            else
            {
                return TBS_FAILED;
            }
        }
    }

    return TBS_FAILED;
}


ULONG  CLI_FreeValue( PST_CLI_VALUE pValue )
{
    PST_CLI_VALUE pTemp,pValueNext;

    if(pValue == NULL)
        return TBS_FAILED;

    pTemp = pValue;

    while( pTemp != NULL )
    {
        pValueNext = pTemp->pNext;

        /* 1、释放名字 */
        if(pTemp->szName != NULL) CLI_Free(pTemp->szName);


        /* 2、释放关键字 */
        if(pTemp->keywords.pKeyword != NULL)
            if(CLI_FreeKeyword(pTemp->keywords.pKeyword) != TBS_SUCCESS)
            {
                //统计
            }

        /* 3、释放自己 */
        CLI_Free(pTemp);
        pTemp = pValueNext;

    }

    return TBS_SUCCESS;
}


ULONG  CLI_FreeKeyword(PST_CLI_ENUM pKW)
{
    PST_CLI_ENUM pKeyW, pNextKW;

    if(pKW == NULL)
        return TBS_FAILED;

    pKeyW = pKW;

    while(pKeyW != NULL)
    {
        pNextKW = pKeyW->pNext;

        if(pKeyW->pK != NULL)
        {
            CLI_Free(pKeyW->pK);
        }

        CLI_Free(pKeyW);

        pKeyW = pNextKW;

    }

    return TBS_SUCCESS;
}

PST_CLI_ENUM CLI_NewKeywords(char  *szKeys, ULONG  *ulCount)
{
    PST_CLI_ENUM pKeyWord = NULL, p1 = NULL, pReturn = NULL;
    char  *TmpKey, KeyStr[MAX_TOKEN_SIZE];
    ULONG  i;

    if((szKeys == NULL) || (ulCount == NULL))
        return NULL;

    *ulCount = 0;

    TmpKey = szKeys;
    i = 0;
    KeyStr[0] = 0;
    while(*TmpKey != 0)
    {
        if(*TmpKey == KEY_SPACE)
        {
            TmpKey++;
            continue;
        }
        else if(*TmpKey == ',')
        {
            if(i == 0)
            {
                TmpKey++; continue;
            }

            pKeyWord = CLI_NewKeyword();
            if(pKeyWord == NULL)
                return NULL;
            pKeyWord->pK = CLI_Malloc(i+1);
            if(pKeyWord->pK == NULL)
            {
                    if(CLI_FreeKeyword(pKeyWord) != TBS_SUCCESS)
                    {
                        //统计
                    }
                return NULL;
            }
            KeyStr[i]=0;
            STB_StrCpy(pKeyWord->pK,&KeyStr[0]);
            (*ulCount)++;

            if(p1 != NULL)
            {
                p1->pNext = pKeyWord;
                pKeyWord->pPrevious = p1;
            }
            else
            {
                if(pReturn == NULL)
                    pReturn = pKeyWord;
            }
            p1 = pKeyWord;
            TmpKey++;
            i = 0;
            KeyStr[0] = 0;
        }
        else
        {
            KeyStr[i++] = *TmpKey++;
            if(i > 79)
                return NULL;
        }
    }

    if(i > 0)
    {
        pKeyWord = CLI_NewKeyword();
        if(pKeyWord == NULL)
            return NULL;
        pKeyWord->pK = CLI_Malloc(i+1);
        if(pKeyWord->pK == NULL)
        {
                if(CLI_FreeKeyword(pKeyWord) != TBS_SUCCESS)
                {
                    //统计
                }

            return NULL;
        }
        KeyStr[i]=0;
        STB_StrCpy(pKeyWord->pK,&KeyStr[0]);
        (*ulCount)++;

        if(p1 != NULL)
        {
            p1->pNext = pKeyWord;
            pKeyWord->pPrevious = p1;
        }
        else
        {
            if(pReturn == NULL)
                pReturn = pKeyWord;
        }
    }

    return pReturn;
}

ULONG  CLI_FreeParameter(PST_CLI_PARAMETER pParam)
{
    PST_CLI_PARAMETER pParamTemp,pParamNext;

    if(pParam == NULL)  return TBS_FAILED;

    pParamTemp = pParam;

    while(pParamTemp != NULL)
    {
        pParamNext = pParamTemp->pNext;

        /* 1. Free the heir parameter */
        if(pParamTemp->subParam != NULL)
        {
            if (CLI_FreeParameter(pParamTemp->subParam) != TBS_SUCCESS)
            {
                return TBS_FAILED;
            }
        }

        /* 2. Free itself */
        if(pParamTemp->szName != NULL)
        {
            CLI_Free(pParamTemp->szName);
        }
        if(pParamTemp->valueList.pValue != NULL)
        {
            if (CLI_FreeValue(pParamTemp->valueList.pValue) != TBS_SUCCESS)
            {
                return TBS_FAILED;
            }
        }
        CLI_Free(pParamTemp);

        pParamTemp = pParamNext;

    }

    return TBS_SUCCESS;
}

ULONG  CLI_FreeParamlink(PST_CLI_PARAMLINK pParamLink)
{
    if(pParamLink == NULL)  return TBS_FAILED;

    if(pParamLink->pParam != NULL)
    {
        if (CLI_FreeParameter(pParamLink->pParam) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
    }

    CLI_Free(pParamLink);

    return TBS_SUCCESS;
}

ULONG  CLI_FreeObject(PST_CLI_CMDKEY pCmdKey)
{
    if(pCmdKey == NULL) return TBS_FAILED;

    /* 1. Free the szName String            */
    if(pCmdKey->szName != NULL)
        CLI_Free(pCmdKey->szName);

    /* 2. Free action's parameter link      */
    if(pCmdKey->action.pParamLink != NULL)
    {
        if (CLI_FreeParamlink(pCmdKey->action.pParamLink) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
    }

    /* 3. Free noAction's parameter link    */
    if(pCmdKey->noAction.pParamLink != NULL)
    {
        if (CLI_FreeParamlink(pCmdKey->noAction.pParamLink) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
    }

    /* 4. Free the Object   */
    CLI_Free(pCmdKey);
    pCmdKey = NULL;
    return TBS_SUCCESS;
}

PST_CLI_VALUE CLI_NewValue(_VOID   )
{
    PST_CLI_VALUE pValue;

    pValue = (PST_CLI_VALUE) CLI_Malloc(sizeof(ST_CLI_VALUE));

    if(pValue != NULL)
    {
        pValue->szName = NULL;
        pValue->lIntFilter[0] = 0;
        pValue->lIntFilter[1] = 0;
        pValue->pPrevious = NULL;
        pValue->pNext = NULL;
        pValue->ulAbsoluteIndex = 0;

        /* intialize valueType */
        pValue->valueType.ucAttrib = 0;
        pValue->valueType.ucDataType = CLI_DATA_NULL;
        pValue->valueType.ulDataLen = 0;

        /* intialize keywords */
        pValue->keywords.ulNumber = 0;
        pValue->keywords.pKeyword = NULL;

        /* default value */
        pValue->szDefault = NULL;

        /* intialize check function pointer */
        pValue->pCheckFunc = NULL;
    }

    return pValue;

}

PST_CLI_VALUELINK CLI_NewValuelink(_VOID   )
{
    PST_CLI_VALUELINK pValue;

    pValue = (PST_CLI_VALUELINK) CLI_Malloc(sizeof(ST_CLI_VALUELINK));

    if(pValue != NULL)
    {
        pValue->ulNumber = 0;
        pValue->pValue = NULL;
    }

    return pValue;
}

PST_CLI_ENUM CLI_NewKeyword(_VOID   )
{
    PST_CLI_ENUM pKeyW;

    pKeyW = (PST_CLI_ENUM) CLI_Malloc(sizeof(ST_CLI_ENUM));

    if(pKeyW != NULL)
    {
        pKeyW->pK = NULL;
        pKeyW->pPrevious = NULL;
        pKeyW->pNext = NULL;
    }

    return pKeyW;
}

PST_CLI_CMDKEY CLI_NewObject(_VOID   )
{
    PST_CLI_CMDKEY pObj;

    pObj = (PST_CLI_CMDKEY) CLI_Malloc(sizeof(ST_CLI_CMDKEY));

    if(pObj != NULL)
    {
        pObj->szName = NULL;
        pObj->ulObjectCount = 0;
        pObj->pSubCmdKey = NULL;
        pObj->pPrevious = NULL;
        pObj->pNext = NULL;
        pObj->pParent = NULL;
        pObj->ulNoFlag = 0;
        /* Initailize the action */
        pObj->action.ulMode = NULL_MODE;
        pObj->action.rightLevel = CLI_AL_QUERY;
        pObj->action.pFunction = NULL;
        pObj->action.strObjHelp = NULL;
        pObj->action.modeConv = CLI_MC_NULL;
        pObj->action.pParamLink = NULL;

        pObj->noAction.ulMode = NULL_MODE;
        pObj->noAction.rightLevel = CLI_AL_QUERY;
        pObj->noAction.pFunction = NULL;
        pObj->noAction.strObjHelp = CLI_ML_NULL;
        pObj->noAction.modeConv = CLI_MC_NULL;
        pObj->noAction.pParamLink = NULL;

    }

    return pObj;

}

PST_CLI_PARAMETER CLI_NewParameter(_VOID   )
{
    PST_CLI_PARAMETER pParam;

    pParam = (PST_CLI_PARAMETER) CLI_Malloc(sizeof(ST_CLI_PARAMETER));

    if(pParam != NULL)
    {
        pParam->szName = NULL;
        pParam->subParam = NULL;
        pParam->pPrevious = NULL;
        pParam->pNext = NULL;
        pParam->pParent = NULL;

        /* intialize the stParamType */
        pParam->stParamType.ucAttrib = 0;

        /* intialize the valueList */
        pParam->valueList.ulNumber = 0;
        pParam->valueList.pValue = NULL;
    }

    return pParam;
}


PST_CLI_PARAMLINK CLI_NewParamlink(_VOID   )
{
    PST_CLI_PARAMLINK pParamLink;

    pParamLink = (PST_CLI_PARAMLINK) CLI_Malloc(sizeof(ST_CLI_PARAMLINK));

    if(pParamLink != NULL)
    {
        pParamLink->ulNumber = 0;
        pParamLink->pParam = NULL;
    }

    return pParamLink;
}

PST_CLI_CMDKEY CLI_CreateObject(char  *szName, ULONG  ulMode,
            CLI_OPERAT_LEVEL_T level, PFUN_CLI_CALLBACK_EXEC pFunc,
            BOOL bChangeMode, CLI_MODE_CHANGE_T modeConv,
            PFUN_CLI_CALLBACK_EXEC pNoFunc)
{
    PST_CLI_CMDKEY pObjTmp;
    ST_CLI_MODE *pMode;
    ULONG  ulModeLevel;
    (void)bChangeMode;

    if(ulMode >= CTM_TOTAL)
        return NULL;

    if((szName == NULL) || (*szName == 0))
        return NULL;

    if(!STB_StriCmp(szName, "show"))
        if(ulMode != CTM_GLOBAL)
            return NULL ;

    pObjTmp = CLI_NewObject();
    if(pObjTmp == NULL)
        return NULL;

    /* 2. set object Name */
    pObjTmp->szName = CLI_Malloc(STB_StrLen(szName) + 1);
    if(pObjTmp->szName == NULL)
    {
        if (CLI_FreeObject(pObjTmp) != TBS_SUCCESS)
        {
            //统计
        }
        return NULL;
    }
    STB_StrCpy(pObjTmp->szName,szName);
    _AffirmStrBeLower(pObjTmp->szName);

    /* 3. Set action */
    pObjTmp->action.ulMode = ulMode;
    pMode = CLI_GetMode(ulMode);
    if(pMode == NULL)
    {
        if (CLI_FreeObject(pObjTmp) != TBS_SUCCESS)
        {
            //统计
        }
        return NULL;
    }

    ulModeLevel = pMode->rightLevel;
    if(level != CLI_AL_NULL)
    {
        if((ULONG )level >= ulModeLevel)
            pObjTmp->action.rightLevel = level;
        else
            pObjTmp->action.rightLevel = (CLI_OPERAT_LEVEL_T)ulModeLevel;
    }
    else
        pObjTmp->action.rightLevel = (CLI_OPERAT_LEVEL_T)ulModeLevel;

    pObjTmp->action.modeConv = modeConv;
    pObjTmp->action.pFunction = pFunc;

    /* 4. Set no form action  */
    if( pNoFunc != NULL )
    {
        pObjTmp->noAction.ulMode = ulMode;
        pObjTmp->noAction.rightLevel = pObjTmp->action.rightLevel;
        pObjTmp->noAction.pFunction  = pNoFunc;
        pObjTmp->ulNoFlag = 1;
    }

    return pObjTmp;
}

PST_CLI_PARAMETER CLI_CreateParameter( char  *szNameStr, UCHAR ucTypeAttrib,
                                   UCHAR ucIndex, ULONG  ulIndex )
{
    char  szNameTmp[15];
    PST_CLI_PARAMETER pTempParam;
    (void)ucIndex;
    /* 1. Malloc memory */
    pTempParam = CLI_NewParameter();
    if(pTempParam == NULL)
        return NULL;

    /* 2. Set name */
    if(szNameStr == NULL)
    {
        /* if name needed, NULL cause an error! */
        if(ucTypeAttrib & CLI_PRMPROP_NAME)
        {
            if (CLI_FreeParameter(pTempParam) != TBS_SUCCESS)
            {
                //统计
            }
            return NULL;
        }
        else
        {
            STB_StrCpy(szNameTmp, "virtual-param");
            pTempParam->szName = CLI_Malloc(STB_StrLen(szNameTmp)+1);
            if(pTempParam->szName == NULL)
            {
                if (CLI_FreeParameter(pTempParam) != TBS_SUCCESS)
                {
                    //统计
                }
                return NULL;
            }
            STB_StrCpy(pTempParam->szName,szNameTmp);
            _AffirmStrBeLower(pTempParam->szName);
        }
    }
    else
    {
        pTempParam->szName = CLI_Malloc(STB_StrLen(szNameStr) + 1);
        if(pTempParam->szName == NULL)
        {
            if (CLI_FreeParameter(pTempParam) != TBS_SUCCESS)
            {
                //统计
            }
            return NULL;
        }
        STB_StrCpy(pTempParam->szName,szNameStr);
    }

    /* 3. Set attrib & Index */
    pTempParam->stParamType.ucAttrib = ucTypeAttrib;

    /* 4. Set absolute ulIndex */
    pTempParam->ulAbsoluteIndex = ulIndex;

    return pTempParam;

}

PST_CLI_PARAMLINK CLI_CreateParamLink(_VOID   )
{
    return CLI_NewParamlink();
}

PST_CLI_VALUE CLI_CreateValue(char  *szName, UCHAR ucDataType,
                          ULONG ulDataLen, char  *szKeys,  int    iLow, int    iHigh,
                          ULONG  ulIndex)
{
    ULONG  ulCount;
    char  szNameTmp[15];
    PST_CLI_VALUE pTempValue;
    PST_CLI_ENUM pKeyword = NULL;

    /* 1. Malloc memory */
    pTempValue  = CLI_NewValue();
    if(pTempValue == NULL)
        return NULL;

    /* 2. Set Name */
    if(szName == NULL)
    {
        STB_StrCpy(szNameTmp, "value");
        pTempValue->szName = CLI_Malloc(STB_StrLen(szNameTmp)+1);
        if(pTempValue->szName == NULL)
        {
            if (CLI_FreeValue(pTempValue) != TBS_SUCCESS)
            {
                //统计
            }
            return NULL;
        }
        STB_StrCpy(pTempValue->szName,szNameTmp);
        _AffirmStrBeLower(pTempValue->szName);
    }
    else
    {
        pTempValue->szName = CLI_Malloc(STB_StrLen(szName)+1);
        if(pTempValue->szName == NULL)
        {
            if (CLI_FreeValue(pTempValue) != TBS_SUCCESS)
            {
                //统计
            }
            return NULL;
        }
        STB_StrCpy(pTempValue->szName,szName);
    }

    /* 3. set value type */
    pTempValue->valueType.ucAttrib  = CLI_VALPROP_CHECK;
    pTempValue->valueType.ulDataLen = ulDataLen;
    pTempValue->valueType.ucDataType = ucDataType;

    /* 4. Set absolute ulIndex */
    pTempValue->ulAbsoluteIndex = ulIndex;

    /* 5. other data set according to datatype */
    switch(ucDataType)
    {
        case CLI_DATA_VU:
        case CLI_DATA_VH:
                if((ULONG )iLow <= (ULONG )iHigh)
                {
                    /* Unsigned value saved as signed */
                    pTempValue->lIntFilter[0] = iLow;
                    pTempValue->lIntFilter[1] = iHigh;
                }
                else
                {
                    if (CLI_FreeValue(pTempValue) != TBS_SUCCESS)
                    {
                        //统计
                    }
                    return NULL;
                }
                break;
        case CLI_DATA_VI:
                if(iLow <= iHigh)
                {
                    pTempValue->lIntFilter[0] = iLow;
                    pTempValue->lIntFilter[1] = iHigh;
                }
                else
                {
                    if (CLI_FreeValue(pTempValue) != TBS_SUCCESS)
                    {
                        //统计
                    }
                    return NULL;
                }
                break;

        case CLI_DATA_VS:
                if(ulDataLen >= CLI_MAX_PARAMLEN)
                {
                    if (CLI_FreeValue(pTempValue) != TBS_SUCCESS)
                    {
                        //统计
                    }
                    return NULL;
                }
                break;
        case CLI_DATA_K:
                pKeyword = CLI_NewKeywords(szKeys,&ulCount);
                if(pKeyword == NULL)
                {
                    if (CLI_FreeValue(pTempValue) != TBS_SUCCESS)
                    {
                        //统计
                    }
                    return NULL;
                }
                pTempValue->keywords.pKeyword = pKeyword;
                pTempValue->keywords.ulNumber = ulCount;
                break;

        case CLI_DATA_IP:
        case CLI_DATA_MAC:
        case CLI_DATA_MASK:
        case CLI_DATA_DATE:
        case CLI_DATA_TIME:
                break;
        default:
                if((ucDataType >= CLI_DATA_USER) && (ucDataType < CLI_DATA_BOTTOM))
                {
                    if(CLI_ValidateUserType((ULONG )ucDataType) != TBS_SUCCESS)
                    {
                        if (CLI_FreeValue(pTempValue) != TBS_SUCCESS)
                        {
                            //统计
                        }
                        return NULL;
                    }
                }
                else
                {
                    if (CLI_FreeValue(pTempValue) != TBS_SUCCESS)
                    {
                        //统计
                    }
                    return NULL;
                }
    }

    return pTempValue;

}

PST_CLI_VALUE CLI_CreateValueWithCheck(char  *szName,
                                UCHAR ucAttrib, UCHAR ucDataType, ULONG ulDataLen,
                                char  *szKeys,  int    iLow, int    iHigh,
                                ULONG  ulIndex,
                                FUN_USER_CHECK pFunc)
{
    ULONG  ulCount;
    char  szNameTmp[15];
    PST_CLI_VALUE pValue;
    PST_CLI_ENUM pKeyword = NULL;

    /* 1. Malloc memory */
    pValue = CLI_NewValue();
    if(pValue == NULL)
        return NULL;

    /* 2. Set Name */
    if(szName == NULL)
    {
        STB_StrCpy(szNameTmp, "value");
        pValue->szName = CLI_Malloc(STB_StrLen(szNameTmp)+1);
        if(pValue->szName == NULL)
        {
            if (CLI_FreeValue(pValue) != TBS_SUCCESS)
            {
                //统计
            }
            return NULL;
        }
        STB_StrCpy(pValue->szName,szNameTmp);
    }
    else
    {
        pValue->szName = CLI_Malloc(STB_StrLen(szName)+1);
        if(pValue->szName == NULL)
        {
            if (CLI_FreeValue(pValue) != TBS_SUCCESS)
            {
                //统计
            }
            return NULL;
        }
        STB_StrCpy(pValue->szName,szName);
    }

    /* 3. set value type */
    /* 数据的长度不得超过最大参数长度*/
    if (ulDataLen >= CLI_MAX_PARAMLEN)
    {
        return NULL;
    }
    pValue->valueType.ulDataLen = ulDataLen;
    pValue->valueType.ucAttrib  = CLI_VALPROP_NULL;
    pValue->valueType.ucDataType = ucDataType;

    /* 4. Set absolute ulIndex */
    pValue->ulAbsoluteIndex = ulIndex;

    /* 5. other data set according to datatype */
    switch(ucDataType)
    {
        case CLI_DATA_VU:
        case CLI_DATA_VH:
                if(ucAttrib & CLI_VALPROP_CHECK)
                {
                    if((ULONG )iLow <= (ULONG )iHigh)
                    {
                        /* Unsigned value saved as signed */
                        pValue->lIntFilter[0] = iLow;
                        pValue->lIntFilter[1] = iHigh;
                    }
                    else
                    {
                        if (CLI_FreeValue(pValue) != TBS_SUCCESS)
                        {
                            //统计
                        }
                        return NULL;
                    }
                }
                break;
        case CLI_DATA_VI:
                if(ucAttrib & CLI_VALPROP_CHECK)
                {
                    if(iLow <= iHigh)
                    {
                        pValue->lIntFilter[0] = iLow;
                        pValue->lIntFilter[1] = iHigh;
                    }
                    else
                    {
                        if (CLI_FreeValue(pValue) != TBS_SUCCESS)
                        {
                            //统计
                        }
                        return NULL;
                    }
                }
                break;
        case CLI_DATA_VS:
                break;
        case CLI_DATA_K:
                pKeyword = CLI_NewKeywords(szKeys,&ulCount);
                if(pKeyword == NULL)
                {
                    if (CLI_FreeValue(pValue) != TBS_SUCCESS)
                    {
                        //统计
                    }
                    return NULL;
                }
                pValue->keywords.pKeyword = pKeyword;
                pValue->keywords.ulNumber = ulCount;
                break;
        default:
                /* Here we get an invalid data type */
                if (CLI_FreeValue(pValue) != TBS_SUCCESS)
                {
                    //统计
                }
                return NULL;
    }

    /* Register Value Check User Defined Function */
    pValue->pCheckFunc = pFunc;

    return pValue;

}

ULONG  CLI_DefineValueCheck(PST_CLI_VALUE pValue,FUN_USER_CHECK pFunc)
{
    if(pValue == NULL)
    {
        return TBS_FAILED;
    }

    pValue->pCheckFunc = pFunc;

    return TBS_SUCCESS;
}

ULONG  CLI_SetValueDefault(PST_CLI_VALUE pInValue, char  *szDefault)
{
    if(szDefault != NULL)
    {
        if(pInValue->szDefault != NULL)
        {
            CLI_Free(pInValue->szDefault);
            pInValue->szDefault = NULL;
        }

        pInValue->szDefault = CLI_Malloc(STB_StrLen(szDefault)+1);
        if(pInValue->szDefault == NULL)
            return TBS_FAILED;

        STB_StrCpy(pInValue->szDefault,szDefault);
        pInValue->valueType.ucAttrib |= CLI_VALPROP_DEFAULT;
    }
    else
    {
        if(pInValue->szDefault != NULL)
        {
            CLI_Free(pInValue->szDefault);
            pInValue->szDefault = NULL;
        }

        pInValue->valueType.ucAttrib &= ~CLI_VALPROP_DEFAULT;
    }

    return TBS_SUCCESS;
}

ULONG  CLI_SetMode(ULONG  ulMode, char  *szName, char  *szPrompt,
              CLI_OPERAT_LEVEL_T level,
              ULONG  ulParentMode, PST_CLI_CMDKEY pCmdKey)
{
    ST_CLI_MODE ModeTemp;

    if(ulMode >= CTM_TOTAL)
    {
        return TBS_FAILED;
    }
    ModeTemp.ulMode = ulMode;

    if( (szName == NULL)
     || (szPrompt == NULL))
    {
        return TBS_FAILED;
    }
    if( (STB_StrLen(szName) >= CLI_MAX_MODE_NAME - 1)
     || (STB_StrLen(szPrompt) >= CLI_MAX_MODE_PROMPT - 1))
    {
        return TBS_FAILED;
    }
    STB_StrCpy(ModeTemp.szModeName,szName);
    STB_StrCpy(ModeTemp.szModePrompt,szPrompt);

    ModeTemp.rightLevel = level;

    if((ulParentMode >= ulMode) && (ulParentMode != NULL_MODE))
        return TBS_FAILED;

    ModeTemp.ulParent = ulParentMode;
    ModeTemp.pCmdKey = pCmdKey;

    return (CLI_RegMode(&ModeTemp));

}

ULONG  CLI_RegMode(ST_CLI_MODE *pMode)
{

    if(pMode == NULL)
        return TBS_FAILED;

    if(pMode->ulMode < MAX_MODE_NUM)
    {
        if(m_sCliSysModes[pMode->ulMode].ulMode == NULL_MODE)
            m_sCliSysModes[pMode->ulMode].ulMode = pMode->ulMode;
        else
            return TBS_FAILED;
    }
    else
        return TBS_FAILED;

    STB_StrCpy(&(m_sCliSysModes[pMode->ulMode].szModeName[0]),
           &(pMode->szModeName[0]));

    STB_StrCpy(&(m_sCliSysModes[pMode->ulMode].szModePrompt[0]),
           &(pMode->szModePrompt[0]));

    m_sCliSysModes[pMode->ulMode].rightLevel = pMode->rightLevel;

    m_sCliSysModes[pMode->ulMode].ulParent  = pMode->ulParent;

    m_sCliSysModes[pMode->ulMode].pCmdKey   = pMode->pCmdKey;

    return TBS_SUCCESS;
}

ULONG  CLI_RegModeObject(ULONG  ulMode,PST_CLI_CMDKEY pCmdKey, ULONG  ulParamCount)
{
    ULONG  ulParentMode, ulRet = TBS_FAILED;
    PST_CLI_CMDKEY pCmdKey1;

    if(ulMode < MAX_MODE_NUM)
    {
        if(m_sCliSysModes[ulMode].ulMode == ulMode)
        {
            m_sCliSysModes[ulMode].pCmdKey = pCmdKey;
            if(ulParamCount > 0)
            {
                /* 转换到新模式的命令 */
                m_sCliSysModes[ulMode].ulParamCount = ulParamCount;
            }

            if(ulMode > 0)
            {
                pCmdKey1 = pCmdKey->pParent;
                while( (pCmdKey1 != NULL)&& !CLI_KEY_CHANGE_MODE(pCmdKey1)  )
                {
                    pCmdKey1 = pCmdKey1->pParent;
                }
                ulParentMode = CLI_GetNewMode(pCmdKey1) ;
                m_sCliSysModes[ulMode].ulHierarchy = m_sCliSysModes[ulParentMode].ulHierarchy + 1 ;
            }
            ulRet = TBS_SUCCESS;
        }
    }

    return ulRet;

}

/* 该函数为一扩展的注册模式对象的函数,在注册该对象时可以为exit,end
   命令指定一个执行函数   */
ULONG  CLI_RegModeObjectExt( ULONG  ulMode, PST_CLI_CMDKEY pCmdKey,
                        ULONG  ulParamCount, PFUN_CLI_CALLBACK_EXEC pFunc)
{
    ULONG  ulParentMode, ulRet = TBS_FAILED;
    PST_CLI_CMDKEY pCmdKey1;

    if(ulMode < MAX_MODE_NUM)
    {
        if(m_sCliSysModes[ulMode].ulMode == ulMode)
        {
            m_sCliSysModes[ulMode].pCmdKey = pCmdKey;
            if(ulParamCount > 0)
            {
                m_sCliSysModes[ulMode].ulParamCount = ulParamCount;
            }

            if(ulMode > 0)
            {
                pCmdKey1 = pCmdKey->pParent;
                while( !CLI_KEY_CHANGE_MODE(pCmdKey1) && (pCmdKey1 != NULL) )
                {
                    pCmdKey1 = pCmdKey1->pParent;
                }
                ulParentMode = CLI_GetNewMode(pCmdKey1) ;
                m_sCliSysModes[ulMode].ulHierarchy = m_sCliSysModes[ulParentMode].ulHierarchy + 1 ;
            }
            ulRet = TBS_SUCCESS;
        }
    }

    if ((ulRet == TBS_SUCCESS) && (ulMode != CTM_GENL) && (ulMode != CTM_GLOBAL))
    {
        ulRet += CLI_CmdAutoRegist(CMD_CM "..",
                                   (UCHAR)ulMode,
                                   CLI_AL_QUERY,
                                   pFunc,
                                   CLI_MC_EXIT,
                                   NULL_MODE,
                                   CMDHELP_GLB_CM,
                                   CMDHELP_GLB_CM_PARENT,
                                   CLI_ML_NULL);
        ulRet += CLI_CmdAutoRegist(CMD_CM "root",
                                   (UCHAR)ulMode,
                                   CLI_AL_QUERY,
                                   pFunc,
                                   CLI_MC_END,
                                   NULL_MODE,
                                   CMDHELP_GLB_CM,
                                   CMDHELP_GLB_CM_ROOT,
                                   CLI_ML_NULL);

    }

    return ulRet;

}

ULONG  CLI_RegObject(PST_CLI_CMDKEY pParent, PST_CLI_CMDKEY pCmdKey)
{
    PST_CLI_CMDKEY pObj,pObj1,pCmdKey1;
    ULONG  ulMode;

    if((pParent == NULL) || (pCmdKey == NULL))
    {
        return TBS_FAILED;
    }

    pObj = pParent->pSubCmdKey;
    if(pObj == NULL)
    {
        pParent->pSubCmdKey = pCmdKey;
        pCmdKey->pParent = pParent;
        pParent->ulObjectCount++;
        if((pCmdKey->ulNoFlag))
        {
            ulMode = pCmdKey->action.ulMode;
            while((pCmdKey) && (pCmdKey->action.ulMode == ulMode))
            {
                pCmdKey->ulNoFlag = 1;
                pCmdKey = pCmdKey->pParent;
            }
        }
    }
    else
    {
        pObj1 = pObj;

        while(pObj != NULL)
        {
            if(CLI_IS_SAME_KEY(pObj,pCmdKey))
            {
                /* 不允许一个模式下的同名对象有不同的帮助信息 */
                if (pObj->action.strObjHelp != pCmdKey->action.strObjHelp
                 && (pObj->action.strObjHelp != CLI_ML_NULL)
                 && (pCmdKey->action.strObjHelp != CLI_ML_NULL))
                {
                    REG_OUT("\r\n CLI: 同名对象的帮助ID不同:%s (%s), %s (%s)",
                        pObj->szName, pObj->pSubCmdKey->szName, pCmdKey->szName, pCmdKey->pSubCmdKey->szName);

                    return TBS_FAILED;
                }

                /* 注册相同命令时，取低权限 */
                if(pObj->action.rightLevel > pCmdKey->action.rightLevel)
                {
                    pObj->action.rightLevel = pCmdKey->action.rightLevel;
                }

                pParent = pObj;
                pCmdKey1 = pCmdKey;
                /* Set pCmdKey->pSubCmdKey as the header    */
                pCmdKey = pCmdKey1->pSubCmdKey;
                if(pCmdKey == NULL)
                {
                    return TBS_FAILED;
                }

                /* release the pCmdKey1 */
                if (CLI_FreeObject(pCmdKey1) != TBS_SUCCESS)
                {
                    return TBS_FAILED;
                }

                pCmdKey1 = NULL;

                pCmdKey->pParent = NULL;
                pObj = pObj->pSubCmdKey;

                if((NULL == pObj))
                {
                    while(pCmdKey)
                    {
                        pCmdKey1 = pCmdKey->pSubCmdKey;
                        if (CLI_FreeObject(pCmdKey) != TBS_SUCCESS)
                        {
                            return TBS_FAILED;
                        }
                        pCmdKey = pCmdKey1;
                    }
                    return TBS_FAILED;
                }
                continue;
            }
            else if(CLI_KEY_BIGGER_THAN(pObj,pCmdKey))
            {
                pObj1 = pObj->pPrevious;
                if(pObj1 == NULL)
                {
                    pParent->pSubCmdKey = pCmdKey;
                    pCmdKey->pNext = pObj;
                    pObj->pPrevious = pCmdKey;
                    pCmdKey->pParent = pParent;
                    pParent->ulObjectCount++;
                    if((NULL != pCmdKey) && (pCmdKey->ulNoFlag))
                    {
                        ulMode = pCmdKey->action.ulMode;
                        while((pCmdKey) && (pCmdKey->action.ulMode == ulMode))
                        {
                            pCmdKey->ulNoFlag = 1;
                            pCmdKey = pCmdKey->pParent;
                        }
                    }
                    return TBS_SUCCESS;
                }
                else
                {
                    pObj1->pNext = pCmdKey;
                    pCmdKey->pParent = pParent;
                    pCmdKey->pPrevious = pObj1;
                    pCmdKey->pNext = pObj;
                    pObj->pPrevious = pCmdKey;
                    pParent->ulObjectCount++;
                    if((NULL != pCmdKey) && (pCmdKey->ulNoFlag))
                    {
                        ulMode = pCmdKey->action.ulMode;
                        while((pCmdKey) && (pCmdKey->action.ulMode == ulMode))
                        {
                            pCmdKey->ulNoFlag = 1;
                            pCmdKey = pCmdKey->pParent;
                        }
                    }

                    return TBS_SUCCESS;
                }
            }
            pObj1 = pObj;
            pObj = pObj->pNext;
        }

        pObj1->pNext = pCmdKey;
        if(pCmdKey != NULL)
        {
            pCmdKey->pPrevious = pObj1;
            pCmdKey->pParent = pParent;
        }

        pParent->ulObjectCount++;
        if((NULL != pCmdKey) && (pCmdKey->ulNoFlag))
        {
            ulMode = pCmdKey->action.ulMode;
            while((pCmdKey) && (pCmdKey->action.ulMode == ulMode))
            {
                pCmdKey->ulNoFlag = 1;
                pCmdKey = pCmdKey->pParent;
            }
        }
    }

    return TBS_SUCCESS;
}

ULONG  CLI_RegObjToMode(ULONG  ulMode, PST_CLI_CMDKEY pCmdKey)
{
    PST_CLI_CMDKEY pObj,pObj1;
    ST_CLI_MODE *pMode;
    ULONG  i;

    if(pCmdKey == NULL)
    {
        return TBS_FAILED;
    }

    if(ulMode < MAX_MODE_NUM)
    {
        /* 检验全局模式与其它模式是否会出现同名对象的情况 */
        /* 如果出现同名对象, 则将该对象注册到全局模式     */
        if (ulMode != CTM_GLOBAL && m_sCliSysModes[CTM_GLOBAL].pCmdKey != NULL)
        {
            pObj = m_sCliSysModes[CTM_GLOBAL].pCmdKey->pSubCmdKey;
            while (pObj != NULL)
            {
                if (CLI_IS_SAME_KEY(pObj,pCmdKey))
                {
                    pCmdKey->action.ulMode = CTM_GLOBAL;
                    ulMode = CTM_GLOBAL;
                    break;
                }
                pObj = pObj->pNext;
            }
        }
        else
        {
            for (i = 0; i <  MAX_MODE_NUM; i++)
            {
                if (i == CTM_GLOBAL || m_sCliSysModes[i].pCmdKey == NULL)
                    continue;
                pObj = m_sCliSysModes[i].pCmdKey->pSubCmdKey;

                while (pObj != NULL)
                {
                    if (CLI_IS_SAME_KEY(pObj,pCmdKey))
                    {
                        pObj->pParent->ulObjectCount--;
                        pObj->pNext->pPrevious = pObj->pPrevious;
                        pObj->pPrevious->pNext = pObj->pNext;
                        pObj->action.ulMode = CTM_GLOBAL;

                        if (TBS_SUCCESS != CLI_RegObject(m_sCliSysModes[CTM_GLOBAL].pCmdKey,pObj))
                        {
                            return TBS_FAILED;
                        }

                        ulMode = CTM_GLOBAL;
                        break;
                    }
                    pObj = pObj->pNext;
                }
            }
        }
        /* 检验完毕 */

        if(m_sCliSysModes[ulMode].ulMode == ulMode)
        {
            pObj = m_sCliSysModes[ulMode].pCmdKey;
            if(pObj == NULL)
            {
                /* No Object Pointer of the mode */
                return TBS_FAILED;
            }

            /* Check the right level and set it to correct value */
            pObj1 = pCmdKey;
            while(pObj1 != NULL)
            {
                if((pObj1->action.rightLevel == CLI_AL_NULL) ||
                   (pObj1->action.rightLevel < m_sCliSysModes[ulMode].rightLevel) )
                {
                    pObj1->action.rightLevel = m_sCliSysModes[ulMode].rightLevel;
                }
                if((pObj1->noAction.rightLevel == CLI_AL_NULL) ||
                   (pObj1->noAction.rightLevel < m_sCliSysModes[ulMode].rightLevel) )
                {
                    pObj1->noAction.rightLevel = m_sCliSysModes[ulMode].rightLevel;
                }
                pObj1 = pObj1->pSubCmdKey;
            }

            if(!STB_StriCmp(pCmdKey->szName, "exit") || (!STB_StriCmp(pCmdKey->szName, "end")))
            {
                pMode = CLI_GetMode(ulMode);
                if(NULL != pMode)
                {
                    pMode = CLI_GetMode(pMode->ulParent);
                    if(NULL != pMode)
                        pCmdKey->action.rightLevel = pMode->rightLevel;
                }
            }

            return CLI_RegObject(pObj,pCmdKey);

        }
    }

    return TBS_FAILED;

}

ULONG  CLI_RegCmdHelp(PST_CLI_CMDKEY pCmdKey, PCSTR szHelpInfoID)
{
    if(pCmdKey == NULL)
    {
        return TBS_FAILED;
    }

    pCmdKey->action.strObjHelp = szHelpInfoID;
    return TBS_SUCCESS;
}

ULONG  CLI_RegParamlink(PST_CLI_CMDKEY pCmdKey, BOOL bNoform,
                         PST_CLI_PARAMLINK pParamLink)
{
    if((pCmdKey == NULL) || (pParamLink == NULL))
    {
        return TBS_FAILED;
    }

    /* bNoform == FALSE: normal action */
    if(!bNoform)
    {
        if(pCmdKey->action.pParamLink != NULL)
        {
            /* Paramlink already exist */
            return TBS_FAILED;
        }
        else
        {
            pCmdKey->action.pParamLink = pParamLink;
            return TBS_SUCCESS;
        }
    }
    /* ulFlag != 0 : no form action */
    else
    {
        if(pCmdKey->noAction.pParamLink != NULL)
        {
            return TBS_FAILED;
        }
        else
        {
            pCmdKey->noAction.pParamLink = pParamLink;
            return TBS_SUCCESS;
        }
    }

}

ULONG  CLI_RegValueToParam(PST_CLI_PARAMETER pParam,
                                PST_CLI_VALUE pValue)
{
    PST_CLI_VALUE pV,pV1;

    if((pParam == NULL) || (pValue == NULL))
    {
        return TBS_FAILED;
    }

    pV = pParam->valueList.pValue;
    if(pV == NULL)
    {
        pParam->valueList.pValue = pValue;
        pParam->valueList.ulNumber++;
    }
    else
    {
        pV1 = pV;
        while(pV != NULL)
        {
            pV1 = pV;
            pV = pV->pNext;
        }
        pV1->pNext = pValue;
        pValue->pPrevious = pV1;
    }

    return TBS_SUCCESS;

}

ULONG  CLI_RegParamAsHeir(PST_CLI_PARAMETER pOldParam,
                     PST_CLI_PARAMETER pNewParam)
{
    PST_CLI_PARAMETER pParam,pParam1;

    if((pOldParam == NULL) || (pNewParam == NULL))
    {
        return TBS_FAILED;
    }

    pParam = pOldParam->subParam;
    if(pParam == NULL)
    {
        pOldParam->subParam = pNewParam;
        pNewParam->pParent = pOldParam;
    }
    else
    {
        pParam1 = pParam;

        while(pParam != NULL)
        {
            if(CLI_PARAM_NEED_NAME(pParam) && CLI_PARAM_NEED_NAME(pNewParam))
            {
                if(CLI_IS_SAME_PARAM(pParam,pNewParam))
                {
                    return TBS_FAILED;
                }
            }
            pParam1 = pParam;
            pParam = pParam->pNext;
        }
        /* 此处pc-lint有告警,确认不会有问题 */
        pParam1->pNext = pNewParam;
        pNewParam->pPrevious = pParam1;
        pNewParam->pParent = pOldParam;
    }

    return TBS_SUCCESS;

}

ULONG  CLI_RegParamToLink(PST_CLI_PARAMLINK pParamLink,
                                PST_CLI_PARAMETER pNewParam)
{
    PST_CLI_PARAMETER pParam,pParam1;

    if((pParamLink == NULL) || (pNewParam == NULL))
    {
        return TBS_FAILED;
    }

    pParam = pParamLink->pParam;
    if(pParam == NULL)
    {
        pParamLink->pParam = pNewParam;
        pParamLink->ulNumber++;
    }
    else
    {
        pParam1 = pParam;
        while(pParam != NULL)
        {
            if(CLI_PARAM_NEED_NAME(pParam) && CLI_PARAM_NEED_NAME(pNewParam))
            {
                if(CLI_IS_SAME_PARAM(pParam,pNewParam))
                {
                    return TBS_FAILED;
                }
            }
            pParam1 = pParam;
            pParam = pParam->pNext;
        }
        /* All parameters in link have null-parent-pointers */
        pParam1->pNext = pNewParam;
        pNewParam->pPrevious = pParam1;
        pParamLink->ulNumber++;
    }

    return TBS_SUCCESS;

}

/*********************************************************************/
/* 函数名称 : CLI_GlobalCmdLink()                                    */
/* 函数功能 : 全局模式的命令链接                                     */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_ProcessInit                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GlobalCmdLink(_VOID   )
{
    ULONG  ulMode;
    PST_CLI_CMDKEY pCmdKey, pGlbObj;

    if (m_sCliSysModes[CTM_GLOBAL].pCmdKey == NULL
     || m_sCliSysModes[CTM_GLOBAL].pCmdKey->pSubCmdKey == NULL)
        return TBS_FAILED;

    pGlbObj = m_sCliSysModes[CTM_GLOBAL].pCmdKey->pSubCmdKey;

    for (ulMode = 0; ulMode < MAX_MODE_NUM; ulMode++)
    {
        if (ulMode == CTM_GLOBAL || m_sCliSysModes[ulMode].pCmdKey == NULL )
            continue;
        pCmdKey = m_sCliSysModes[ulMode].pCmdKey->pSubCmdKey;
        if (pCmdKey == NULL)
            continue;

        while (pCmdKey->pNext != NULL)
            pCmdKey = pCmdKey->pNext;
        pCmdKey->pNext = pGlbObj;
    }
    return TBS_SUCCESS;
}









/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/



