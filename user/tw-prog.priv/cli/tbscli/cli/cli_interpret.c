/****************************************************************************
 文件名  : CLI_Interpret.c
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述:  实现了命令解释的过程。包括命令的匹配、联想、交互输入
            与解释成功的操作执行
 备注   :
 函数列表:
 提供给外部的接口:   本文件提供给外部的接口

 需要外部提供的接口:    本文件需要外部提供的接口
            (1) 输入输出接口
            (2) 命令注册的数据结构
            (3) 终端数据

 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
        2. ...
 ****************************************************************************/
#ifdef __cplusplus
    extern "C" {
#endif

#include <ctype.h>
#include "cli_private.h"
#include "cli_term.h"
#include "cli_interpret.h"
#include "cli_io.h"

extern ST_CLI_MODE m_sCliSysModes[MAX_MODE_NUM];
extern ST_CLI_USERTYPE m_sCliUserType[CLI_MAX_USERTYPES];

ULONG  m_ulLogTag = 0;
ST_CLI_WS  m_stCliWS;                          /* 状态结构变量             */


ULONG  CLI_ResetEnviroment()
{
    ULONG  i;

    m_stCliWS.szCmdString[0] = 0;
    m_stCliWS.ulLevel = CLI_AL_QUERY;
    m_stCliWS.ulCurrentMode = ROOT_MODE;
    m_stCliWS.cmdStack.ucTop = 0;
    for(i = 0; i < CLI_MAX_MODE_STACK; i++)
    {
        m_stCliWS.cmdStack.ulMode[i] = NULL_MODE;
    }
    for(i = 0; i < CLI_MAX_PARAM_NUM; i++)
    {
        m_stCliWS.aulIndexTable[i] = 0;
        m_stCliWS.aszParamBuff[i][0]  = 0;
        m_stCliWS.aszNameBuff[i][0] = 0;
    }
    m_stCliWS.ulParamIndex = 0;
    m_stCliWS.ulValidParams = 0;
    m_stCliWS.ulStatus = 0;
    m_stCliWS.ulExpectEle = 0;         //此信息只在交互下的第一个参数输入时提供保存当前
                                  //待输入参数的指针，其它时候一律清为零

    return TBS_SUCCESS;

} /* End of function CLI_ResetEnviroment() */

/*********************************************************************/
/* 函数名称 : CLI_IntpKernelProc()                                   */
/* 函数功能 : 命令解释核心函数，本文件的其它函数都围绕该函数而展开   */
/* 输入参数 : pWS 环境变量                                           */
/* 输出参数 :                                                        */
/* 返回     : SUCCESS: 解释成功，目前调用者对此返回值不敏感          */
/*            FAILURE: 由于特定的原因导致解释失败，调用者对此也不敏感*/
/*            CLI_INTP_RET_HELP: 帮助信息获取结束的返回，如果        */
/*                 是这种情况，编辑缓冲是不会清除的                  */
/*            CLI_INTP_RET_NEWLINE: 由于解释过程需要用户输入         */
/*                 新的命令字，输入完毕后应当返回该值，命令执行者会  */
/*                 据此重新调用本函数，以便支持正常的行编辑操作      */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_IntpKernelProc()
{
    char  *szCmd;
    char  szToken[CLI_MAX_CMDLEN + 1];
    ULONG  ulStatus=0;
    ULONG  ulUserRightLevel;
    ULONG  ulCurrentMode;
    BOOL bFirstToken;
    ULONG  ulRet;
    ST_CLI_POINTER pointer;

    if ((m_stCliWS.ulStatus & CLI_STAT_ENV_NEWLINE) == 0)
    {
        m_stCliWS.ulValidParams = 0;
        CLI_ResetIndexTable();
    }

    pointer.curP.pObj   = NULL;
    pointer.curP.pParam = NULL;
    pointer.curP.pValue = NULL;

    pointer.expP.pObj   = NULL;
    pointer.expP.pParam = NULL;
    pointer.expP.pValue = NULL;
    pointer.pIndexParam = NULL;
    pointer.szPatch[0] = 0;
    pointer.szToken = &szToken[0];

    pointer.ulValueIndex = m_stCliWS.ulParamIndex;

    szToken[0] = 0;
    szCmd = m_stCliWS.szCmdString;
    _rtrim(szCmd);
    ulCurrentMode = m_stCliWS.ulCurrentMode;

    if(NULL != strchr(szCmd,KEY_CTRL_C) || NULL != strchr(szCmd,KEY_CTRL_Z))
    {
        szCmd[0] = '\0';
        return TBS_SUCCESS;
    }

    if(szCmd[0] == '!')
    {
        szCmd[0] = '\0';
        return TBS_SUCCESS;
    }


    if(STB_StrLen(szCmd) == 0)
    {
        return TBS_SUCCESS;
    }

    if (!STB_StriCmp(szCmd, "cm..") && !CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
    {
        STB_StrCpy (szCmd, CMD_CM "..");
    }
    else if ((!STB_StriCmp(szCmd, "ll") || !STB_StriCmp(szCmd, "ls"))
          && !CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
    {
        STB_StrCpy (szCmd, "?");
    }

    SET_WAIT_KEY(ulStatus);
    if(CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
        ulStatus |= CLI_STAT_ENV_SILENT ;

    /* 初始化pointer指针 */

    pointer.ulStatus = &ulStatus;
    pointer.curP.pObj = m_sCliSysModes[ulCurrentMode].pCmdKey;
    if(pointer.curP.pObj == NULL)
    {
        MT_ERRLOG(ulCurrentMode);
        return TBS_FAILED;
    }

    if(NULL != pointer.curP.pObj->pSubCmdKey)
    {
        pointer.expP.pObj = pointer.curP.pObj->pSubCmdKey;
        pointer.expP.pParam = NULL;
        pointer.expP.pValue = NULL;

    }
    else
    {
        if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
            MT_ERRLOG(0);
        return TBS_FAILED;
    }

    /* First Token */
    bFirstToken = TRUE;
    ulUserRightLevel = CLI_GetUserRightLevel();

    for(;;)
    {
        while((ulRet = CLI_GetCmdToken( &szCmd, &szToken[0], &ulStatus, &pointer ))
               == TBS_SUCCESS )
        {
            pointer.szPatch[0] = '\0';

            /* If token include '?', show help information */
            if( NULL != strchr(szToken,'?') )
            {
                m_ulLogTag = 1;
                /* 交互状态下，该值由用户输入获取，因此需要填入，作用见于函数 CLI_CmdRecord*/
                if (IS_INTERACTIVE(ulStatus))
                {
                    STB_StrCat(szCmd, " ");
                    STB_StrCat(szCmd, szToken);
                }
                CLI_ShowHelp( &pointer, ulCurrentMode, ulStatus);
                return CLI_INTP_RET_HELP;
            }

            if((NULL != strchr(szToken,KEY_CTRL_C)) || (NULL != strchr(szToken,KEY_CTRL_Z)))
            {
                szToken[0] = '\0';
                SET_WAIT_NULL(ulStatus);
                return TBS_SUCCESS;
            }
            /* if this is the first token, 'no' token has a
               special meaning of no_form_command.          */
            if( ( bFirstToken == TRUE) &&
                ( STB_StriCmp(szToken, "no") == 0 ) )
            {
                SET_NOFORM(ulStatus);
                SET_MATCHSUCC(ulStatus);
                bFirstToken = FALSE;
                continue;
            }

            /* Check the token with the waiting element */
            if( CLI_CheckElement(szToken, &pointer, &ulStatus)
                  != TBS_SUCCESS )
            {
                if((pointer.szPatch[0]) && (!CLI_IS_ENV_ENTER(m_stCliWS.ulStatus)))
                {
                    if (STB_StrLen(m_stCliWS.szCmdString) + STB_StrLen(pointer.szPatch) >= CLI_MAX_CMDBUF)
                    {
                        DBG_Out("\r\n IntpKernel::命令缓冲濒临溢出");
                        MT_ERRLOG(0);
                        return TBS_SUCCESS;
                    }
                    STB_StrCat( m_stCliWS.szCmdString, &pointer.szPatch[0]);
                    return TBS_SUCCESS;
                }
                if(IS_WAIT_KEY(ulStatus))
                {
                    if((!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
                      && (CLI_IfCommandHelp() != TBS_SUCCESS))
                    {
                        IO_Print(CLI_INTP_CMD_ERR);
                        IO_OutString(m_stCliWS.szCmdString);
                        IO_OutString("(");
                        IO_OutString(pointer.expP.pObj->szName);
                        IO_OutString(")");
                    }
                    return TBS_FAILED;
                }
                else if(IS_WAIT_PARAM(ulStatus))
                {
                    if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
                    {
                        IO_Print(CLI_INTP_INVALID_PARAM) ;
                        IO_OutString(szToken);
                        IO_OutString("(");
                        IO_OutString(pointer.expP.pParam->szName);
                        IO_OutString(")");
                        return TBS_FAILED;
                    }
                }
                else if(IS_WAIT_VALUE(ulStatus))
                {
                    if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
                    {
                        IO_Print(CLI_INTP_INVALID_VALUE) ;
                        IO_OutString(szToken);
                        IO_OutString("(");
                        IO_OutString(pointer.expP.pValue->szName);
                        IO_OutString(")");
                        return TBS_FAILED;
                    }
                }

            }
            else
            {
                if( CLI_UserPermitted(ulUserRightLevel, pointer.curP.pObj, ulStatus)
                     != TBS_SUCCESS )
                {
                    if(CLI_IfCommandHelp() != TBS_SUCCESS)
                    {
                        if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
                        {
                            IO_Print(CLI_INTP_CMD_ERR);
                            IO_OutString(szToken);
                        }
                    }

                    return TBS_FAILED;
                }

                if( IS_INTERACTIVE(ulStatus) )
                {
                    IO_OutString(pointer.szPatch);
                    if(pointer.ucMatchFlag != CTMF_FULL)
                        STB_StrCat(szToken, pointer.szPatch);
                    pointer.szPatch[0] = '\0';
                    if(szToken[0])
                    {
                        if((STB_StrLen(m_stCliWS.szCmdString) + STB_StrLen(szToken) + 1)
                            < CLI_MAX_CMDBUF - 1)
                        {
                            STB_StrCat(m_stCliWS.szCmdString, " ");
                            STB_StrCat(m_stCliWS.szCmdString, szToken);
                        }
                        szCmd = szCmd + STB_StrLen(szToken) + 1;
                    }
                }
            }

            if( CLI_GetExpectedElement( &pointer, &ulStatus )
                 != TBS_SUCCESS )
            {
                if(strchr(szCmd,'?') != NULL)
                {
                    m_ulLogTag = 1;
                    if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
                    {
                        CLI_ShowHelp( &pointer, ulCurrentMode, ulStatus);
                    }
                    return CLI_INTP_RET_HELP;
                }

                break;
            }

            bFirstToken = FALSE;

        }
        if (ulRet == CLI_INTP_RET_NEWLINE)
            return CLI_INTP_RET_NEWLINE;

        if(IS_BIGTOKEN(ulStatus))
        {
            return TBS_FAILED;
        }

        if(IS_UNWANTED(ulStatus))
        {
            if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus) && (*szCmd != 0))
            {
                IO_Print(CLI_INTP_INNEED_PARAM) ;
                IO_OutString(szCmd);
            }
            return TBS_FAILED;
        }

        if( !IS_COMPLETE(ulStatus) && !CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
        {
            if (IS_WAIT_PARAM(ulStatus)
             && (pointer.expP.pParam->stParamType.ucAttrib == CLI_PRMPROP_NULL)
             && (pointer.expP.pParam->valueList.ulNumber != 0))
            {
                pointer.expP.pValue = pointer.expP.pParam->valueList.pValue;
                SET_WAIT_VALUE(ulStatus);
            }
            break;
        }
        else if(*szCmd != 0)
        {
            if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
            {
                IO_Print(CLI_INTP_INNEED_PARAM) ;
                IO_OutString(szCmd);
            }
            return TBS_FAILED;
        }
        else
        {
            break;
        }
    }

    if ((!IS_NOFORM(m_stCliWS.ulStatus)
      && (CLI_IsObjectExecutable(pointer.curP.pObj,HAVEACTIONS) != TBS_SUCCESS))
     || (IS_NOFORM(m_stCliWS.ulStatus)
      && (CLI_IsObjectExecutable(pointer.curP.pObj,HAVENOACTIONS) != TBS_SUCCESS)))
    {
        if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
        {
            IO_Print(CLI_INTP_CMD_INCOMPLETE);
            STB_StrCpy(pointer.szToken,"?");
            CLI_ShowHelp( &pointer, ulCurrentMode, ulStatus);
            return CLI_INTP_RET_HELP;
        }
    }

    else if(!IS_COMPLETE(ulStatus) && !CLI_IS_ENV_SILENT(m_stCliWS.ulStatus)
            && !IS_NOFORM(m_stCliWS.ulStatus))
    {
        if(IS_WAIT_KEY(ulStatus))
        {
            IO_Print(CLI_INTP_CMD_INCOMPLETE);
            IO_OutString(m_stCliWS.szCmdString);
            IO_Print(CLI_INTP_NEED_COMWORD);
            IO_OutString(pointer.expP.pObj->szName);
            return TBS_FAILED;
        }

        if(IS_WAIT_PARAM(ulStatus) && (CLI_BrotherParameterOptional(&pointer.expP.pParam) != TBS_SUCCESS))
        {
            IO_Print(CLI_INTP_CMD_INCOMPLETE);
            IO_OutString(m_stCliWS.szCmdString);
            IO_Print(CLI_INTP_NEED_PARAM);
            IO_OutString(pointer.expP.pParam->szName);
            return TBS_FAILED;
        }
        if(IS_WAIT_VALUE(ulStatus) && (CLI_BrotherValueOptional(&pointer.expP.pValue) != TBS_SUCCESS))
        {
            IO_Print(CLI_INTP_CMD_INCOMPLETE);
            IO_OutString(m_stCliWS.szCmdString);
            IO_Print(CLI_INTP_NEED_VALUE);
            IO_OutString(pointer.expP.pValue->szName);
            return TBS_FAILED;
        }
    }
    else if(!IS_MATCHSUCC(ulStatus) && !CLI_IS_ENV_SILENT(m_stCliWS.ulStatus)
            && !IS_NOFORM(m_stCliWS.ulStatus) && STB_StrLen(szToken))
    {
        if(IS_WAIT_PARAM(ulStatus))
        {
            IO_Print(CLI_INTP_INVALID_PARAM);
            IO_OutString(szToken);
        }
        else if(IS_WAIT_VALUE(ulStatus))
        {
            IO_Print(CLI_INTP_INVALID_VALUE);
            IO_OutString(szToken);
        }
        else
        {
            IO_Print(CLI_INTP_INNEED_PARAM);
            IO_OutString(szToken);
        }
        return TBS_FAILED;
    }

    ulUserRightLevel = CLI_GetUserRightLevel();
    if( CLI_UserPermitted(ulUserRightLevel, pointer.curP.pObj, ulStatus)
         == TBS_SUCCESS )
    {
        if((STB_StrLen(m_stCliWS.szCmdString) + STB_StrLen(&(pointer.szPatch[0])))
            < CLI_MAX_CMDBUF - 1)
        {
            if ( !CLI_IS_ENV_RUNCFG(m_stCliWS.ulStatus) )
                STB_StrCat( m_stCliWS.szCmdString, &pointer.szPatch[0]);
        }

        if((!CLI_IS_ENV_ENTER(m_stCliWS.ulStatus)) && (IS_MATCHSUCC(ulStatus)))
        {
            if(((pointer.curP.pObj->noAction.pFunction == NULL)
                &&(pointer.curP.pObj->action.pFunction == NULL)
                &&(pointer.curP.pObj->action.modeConv == CLI_MC_NULL))
                ||(!STB_StriCmp(szToken,"no"))
                ||(!IS_COMPLETE(ulStatus)))

                STB_StrCat( m_stCliWS.szCmdString, " ");
            else if(IS_COMPLETE(ulStatus))
            {
                STB_StrCat( m_stCliWS.szCmdString, " ");
            }
        }

        if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
        {
            ulRet = CLI_TakeAction(pointer.curP.pObj, ulStatus);

            if (ulRet != TBS_SUCCESS)
            {
                if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus) && CLI_CMD_FAILED_SILENT != ulRet)
                    IO_Print(CLI_INTP_CMD_FAILED);
                return ulRet;
            }
            else
            {
                m_ulLogTag = 1;
            }
        }
    }
    else
    {
        if(!CLI_IS_ENV_SILENT(m_stCliWS.ulStatus))
            IO_OutString("\r\n Access denied.") ;
        return TBS_FAILED;
    }

    return TBS_SUCCESS;

}

/*********************************************************************/
/* 函数名称 : CLI_GetUserType()                                       */
/* 函数功能 : 获取用户自定义类型                                     */
/* 返回     : 用户自定义的类型结构                                   */
/*********************************************************************/
ST_CLI_USERTYPE * CLI_GetUserType(ULONG  ulType)
{
    return &m_sCliUserType[ulType - CLI_DATA_USER];
}

ULONG  CLI_ValidateUserType(ULONG  ulType)
{
    if((ulType >= CLI_DATA_USER) && (ulType < CLI_DATA_BOTTOM))
    {
        if((m_sCliUserType[ulType-CLI_DATA_USER].ucTypeID == ulType)
            && (m_sCliUserType[ulType-CLI_DATA_USER].pCheckFunc != NULL))
        {
            return TBS_SUCCESS;
        }
    }

    return TBS_FAILED;

}
/*※※※※※※※※※※※※※※*/
/* 【1】命令字的获取          */
/*※※※※※※※※※※※※※※*/
/*********************************************************************/
/* 函数名称 : CLI_GetCmdToken                                         */
/* 函数功能 : 以字符串形式获取下一个命令字                           */
/* 输入参数 : szCmdString 命令缓冲中的命令串                         */
/*            ulStatus    命令解释状态                               */
/*            pPointer    命令解释核心结构                           */
/* 输出参数 : szToken     所获取的命令字                             */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetCmdToken( char  **szCmdString, char  *szToken,
                 ULONG  *ulStatus, PST_CLI_POINTER pPointer)
{

    if(IS_INTERACTIVE(*ulStatus))
    {
        if( IS_SKIP_FLAG(*ulStatus) && szToken[0])
        {
            return TBS_SUCCESS;
        }

        return CLI_GetCmdTokenFromUser( szToken, *ulStatus, pPointer );
    }
    else
    {
        if( IS_SKIP_FLAG(*ulStatus) )
        {
            return TBS_SUCCESS;
        }
        return CLI_GetCmdTokenFromBuffer( szCmdString, szToken, ulStatus );
    }

}

/*********************************************************************/
/* 函数名称 : CLI_GetInteractiveObject()                                 */
/* 函数功能 : 给出交互状态下的对象提示                               */
/* 输入参数 : pPointer  命令解释核心结构                             */
/*            ulStatus  命令解释状态                                 */
/* 输出参数 : szSelect  所给出的对象名                               */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
char  *CLI_GetInteractiveObject(PST_CLI_POINTER pPointer, ULONG  ulStatus, char  *szSelect)
{
    PST_CLI_CMDKEY pCmdKey;
    ULONG  ulLen,ulNameLen;

    pCmdKey = pPointer->expP.pObj;
    STB_StrCpy(szSelect,"<");
    ulLen = STB_StrLen(szSelect);
    ulNameLen = 0;

    while(pCmdKey != NULL)
    {
        if(CLI_IS_CURRENT_KNOWN_KEY(pCmdKey)
          &&((IS_NOFORM(ulStatus) && (pCmdKey->ulNoFlag))||(!IS_NOFORM(ulStatus))))
        {
            if(ulNameLen != 0)
            {
                STB_StrCpy(szSelect+ulLen,",");
                ulLen++;
            }
            ulNameLen = STB_StrLen(pCmdKey->szName);
            if(( ulNameLen + ulLen + 1) <= 500)
            {
                STB_StrCpy(szSelect+ulLen,pCmdKey->szName);
                ulLen += ulNameLen;
            }
            else
            {
                STB_StrCpy(szSelect+ulLen,"...");
                ulLen += 3;
                break;
            }
        }/* end of if-block */
        pCmdKey = pCmdKey->pNext;

    } /* end of while() */

    STB_StrCpy(szSelect+ulLen,">");

    return szSelect;
}


/*********************************************************************/
/* 函数名称 : CLI_GetInteractiveObject()                                 */
/* 函数功能 : 给出交互状态下的参数提示                               */
/* 输入参数 : pPointer  命令解释核心结构                             */
/* 输出参数 : szSelect  所给出的参数名                               */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
char  *CLI_GetInteractiveParameter(PST_CLI_POINTER pPointer, char  *szSelect)
{
    PST_CLI_PARAMETER pParam;
    ULONG  ulLen,ulNameLen;
    BOOL bOptional;
    ULONG  ulMtIndex;

    ulLen = 0;
    pParam = pPointer->expP.pParam;
    if(CLI_PARAM_IS_OPTIONAL(pParam))
    {
        STB_StrCpy(szSelect,"[");
        bOptional = TRUE;
    }
    else
    {
        STB_StrCpy(szSelect,"<");
        bOptional = FALSE;
    }

    ulLen ++;
    ulNameLen = 0;

    if(CLI_PARAM_IS_MUTEX(pParam))
    {
        ulMtIndex = pParam->ulAbsoluteIndex;

        while(pParam != NULL
           && CLI_PARAM_IS_MUTEX(pParam)
           && pParam->ulAbsoluteIndex == ulMtIndex)
        {

            if(ulNameLen != 0)
            {
                STB_StrCpy(szSelect+ulLen,",");
                ulLen++;
            }
            ulNameLen = STB_StrLen(pParam->szName);
            if(( ulNameLen + ulLen + 1) <= 500)
            {
                STB_StrCpy(szSelect+ulLen,pParam->szName);
                ulLen += ulNameLen;
            }
            else
            {
                STB_StrCpy(szSelect+ulLen,"...");
                ulLen += 3;
                break;
            }
            pParam = pParam->pNext;
        }
    }
    else
    {
        STB_StrCpy(szSelect+ulLen,pParam->szName);
        ulLen += STB_StrLen(pParam->szName);
    }

    if(bOptional == TRUE)
    {
        STB_StrCpy(szSelect+ulLen,"]");
    }
    else
    {
        STB_StrCpy(szSelect+ulLen,">");
    }

    return szSelect;

}

/*********************************************************************/
/* 函数名称 : CLI_GetInteractiveObject()                             */
/* 函数功能 : 给出交互状态下的参数值提示                             */
/* 输入参数 : pPointer  命令解释核心结构                             */
/* 输出参数 : szSelect  所给出的参数值                               */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
char  *CLI_GetInteractiveValue(PST_CLI_POINTER pPointer, char  *szSelect)
{
    PST_CLI_VALUE pValue;
    PST_CLI_ENUM pKeyword = NULL;
    ULONG  ulLen,ulElement;
    char  s8Tmp[CLI_MAX_PARAMLEN + 3];

    ulLen = 0;
    pValue = pPointer->expP.pValue;

    /* Add value name to string */
    if(CLI_VALUE_IS_OPTIONAL(pValue))
    {
        STB_StrCpy(szSelect,"[");
        STB_StrCat(szSelect,pValue->szName);
        STB_StrCat(szSelect,"]");
    }
    else
    {
        STB_StrCpy(szSelect,"<");
        STB_StrCat(szSelect,pValue->szName);
        STB_StrCat(szSelect,">");
    }

    if( CLI_VALUE_USER_CHECK(pValue) )
    {
        ulLen = STB_StrLen(szSelect);
        pValue->pCheckFunc(NULL,szSelect+ulLen);
        return szSelect;
    }

    if( CLI_ValidateUserType((ULONG )pValue->valueType.ucDataType) == TBS_SUCCESS)
    {
        ulLen = STB_StrLen(szSelect);
        CLI_GetUserType((ULONG )pValue->valueType.ucDataType)->pCheckFunc(NULL,szSelect+ulLen);
        return szSelect;
    }

    if( CLI_VALUE_NEED_CHECK(pValue) )
    {
        if (pValue->valueType.ucDataType == CLI_DATA_VI
          ||pValue->valueType.ucDataType == CLI_DATA_VIK)
        {
            STB_StrCat(szSelect,"[");
            STB_Sprintf(s8Tmp,  "%d",pValue->lIntFilter[0]);
            STB_StrCat(szSelect,s8Tmp);
            STB_StrCat(szSelect,",");
            STB_Sprintf(s8Tmp,  "%d",pValue->lIntFilter[1]);
            STB_StrCat(szSelect,s8Tmp);
            STB_StrCat(szSelect,"]");
        }
        else if (pValue->valueType.ucDataType == CLI_DATA_VU
              || pValue->valueType.ucDataType == CLI_DATA_VUK)
        {
            STB_StrCat(szSelect,"[");
            STB_Sprintf(s8Tmp,  "%u",(ULONG )pValue->lIntFilter[0]);
            STB_StrCat(szSelect,s8Tmp);
            STB_StrCat(szSelect,",");
            STB_Sprintf(s8Tmp,  "%u",(ULONG )pValue->lIntFilter[1]);
            STB_StrCat(szSelect,s8Tmp);
            STB_StrCat(szSelect,"]");
        }
        else if (pValue->valueType.ucDataType == CLI_DATA_VH
              || pValue->valueType.ucDataType == CLI_DATA_VHK)
        {
            STB_StrCat(szSelect,"[");
            STB_Sprintf(s8Tmp,  "%x",(ULONG )pValue->lIntFilter[0]);
            STB_StrCat(szSelect,s8Tmp);
            STB_StrCat(szSelect,",");
            STB_Sprintf(s8Tmp,  "%x",(ULONG )pValue->lIntFilter[1]);
            STB_StrCat(szSelect,s8Tmp);
            STB_StrCat(szSelect," (hex)]");
        }
        else if( (pValue->valueType.ucDataType == CLI_DATA_VS) && (pValue->valueType.ulDataLen != 0) )
        {
            STB_StrCat(szSelect,"[");
            STB_Sprintf(s8Tmp,  "<= %d chars",pValue->valueType.ulDataLen);
            STB_StrCat(szSelect,s8Tmp);
            STB_StrCat(szSelect,"]");
        }
        else if (pValue->valueType.ucDataType == CLI_DATA_DATE)
        {
            STB_StrCat(szSelect, "(yyyy-mm-dd)");
        }
        else if (pValue->valueType.ucDataType == CLI_DATA_TIME)
        {
            STB_StrCat(szSelect, "(hh:mm:ss)");
        }
        else if (pValue->valueType.ucDataType == CLI_DATA_IP)
        {
            STB_StrCat(szSelect, "(IP)");
        }
        else if (pValue->valueType.ucDataType == CLI_DATA_MASK)
        {
            STB_StrCat(szSelect, "(MASK)");
        }
    }
    else
    {
        if (pValue->valueType.ucDataType == CLI_DATA_VI
         || pValue->valueType.ucDataType == CLI_DATA_VIK)
        {
            STB_StrCat(szSelect,"(I)");
        }
        else if (pValue->valueType.ucDataType == CLI_DATA_VU
              || pValue->valueType.ucDataType == CLI_DATA_VUK)
        {
            STB_StrCat(szSelect,"(U)");
        }
        else if (pValue->valueType.ucDataType == CLI_DATA_VH
              || pValue->valueType.ucDataType == CLI_DATA_VHK)
        {
            STB_StrCat(szSelect,"(H)");
        }
        else if (pValue->valueType.ucDataType == CLI_DATA_VS)
        {
            STB_StrCat(szSelect,"(S)");
        }
    }

    ulElement = 0;
    ulLen = STB_StrLen(szSelect);

    if ( (pValue->valueType.ucDataType == CLI_DATA_K)
      || (pValue->valueType.ucDataType == CLI_DATA_VIK)
      || (pValue->valueType.ucDataType == CLI_DATA_VUK)
      || (pValue->valueType.ucDataType == CLI_DATA_VHK)
      || (pValue->valueType.ucDataType == CLI_DATA_IPK)
      || (pValue->valueType.ucDataType == CLI_DATA_MACK) )
    {
        STB_StrCat(szSelect,"{");
        ulLen++;
        pKeyword = pValue->keywords.pKeyword;
        while(pKeyword != NULL)
        {
            if(ulElement != 0)
            {
                STB_StrCat(szSelect,",");
                ulLen ++;
            }
            STB_Sprintf(s8Tmp,"%s",pKeyword->pK);
            ulElement = STB_StrLen(s8Tmp);
            if((ulLen + ulElement + 1) <= 500)
            {
                STB_StrCat(szSelect,s8Tmp);
                ulLen += ulElement;
            }
            else
            {
                STB_StrCat(szSelect,"...");
                ulLen += 3;
                break;
            }
            pKeyword = pKeyword->pNext;
        }
        STB_StrCat(szSelect,"}");
        ulLen++;
    }

    if( CLI_VALUE_HAVE_DEFAULT(pValue) )
    {
        if( NULL != pValue->szDefault )
        {
            STB_StrCat(szSelect,"[");
            STB_StrCat(szSelect,pValue->szDefault);
            STB_StrCat(szSelect,"]");
        }
    }

    return szSelect;

}

/*********************************************************************/
/* 函数名称 : CLI_GetCmdTokenFromUser()                              */
/* 函数功能 : 以交互形式获取下一个命令字                             */
/* 输入参数 : ulStatus 命令解释状态                                  */
/*            pPointer 命令解释核心结构                              */
/* 输出参数 : szToken  获取的命令字                                  */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_GetCmdToken                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetCmdTokenFromUser( char  *szToken, ULONG  ulStatus, PST_CLI_POINTER pPointer )
{
    char  szPrompt[512];
    char  szSelect[512];
    ULONG  rc;
    ULONG  ulFlag;

    ulFlag = TRUE;
    szPrompt[0] = '\0';
    szSelect[0] ='\0';

    if( IS_WAIT_KEY(ulStatus) )
    {
        STB_Sprintf( &szPrompt[0], "\n\r%s %s:",
                "Command word",
                CLI_GetInteractiveObject(pPointer,ulStatus,&szSelect[0]) );
    }
    else if( IS_WAIT_PARAM(ulStatus) )
    {
        if(CLI_PARAM_NEED_NAME(pPointer->expP.pParam))
        {
            STB_Sprintf( &szPrompt[0], "\n\r%s:",
                    CLI_GetInteractiveParameter(pPointer,&szSelect[0]) );
            m_stCliWS.ulExpectEle = (ULONG )pPointer->expP.pParam;
        }
        else
        {
            ulFlag = FALSE;
        }
    }
    else if( IS_WAIT_VALUE(ulStatus) )
    {
        STB_Sprintf( &szPrompt[0], "\n\r%s:",
                CLI_GetInteractiveValue(pPointer,&szSelect[0]) );
        m_stCliWS.ulExpectEle = 0;
    }
    else
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    if(ulFlag == TRUE)
    {
        IO_OutString( &szPrompt[0] );
        CLI_SetPromptLen( STB_StrLen(szPrompt)-2 ) ;
        m_stCliWS.ulStatus |= CLI_STAT_ENV_NEWLINE;
        if (IS_WAIT_VALUE(*pPointer->ulStatus) && pPointer->expP.pValue != NULL)
            rc = CLI_GetString( szToken, pPointer->expP.pValue->valueType.ulDataLen + 1);
        else
            rc = CLI_GetString( szToken, CLI_MAX_PARAMLEN);
    }
    else
    {
        *szToken = 0;
        rc = TBS_SUCCESS;
    }

    return rc;
}

/*********************************************************************/
/* 函数名称 : CLI_GetCmdTokenFromBuffer()                            */
/* 函数功能 : 从命令缓冲中获取下一个命令字                           */
/* 输入参数 : szCmdString  命令缓冲中的命令串                        */
/*            ulStatus     命令的解释状态                            */
/* 输出参数 : szToken 所获取的命令字                                 */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_GetCmdToken                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetCmdTokenFromBuffer( char  **szCmdString, char  *szToken, ULONG  *ulStatus )
{
    char  *szCmd,*szTkn;
    int    iTmp;

    szCmd = *szCmdString;
    szTkn = szToken;
    iTmp = 0;

    if( IS_SKIP_FLAG(*ulStatus) )
    {
        return TBS_SUCCESS;
    }

    while( *szCmd == 0x20 )
    {
        szCmd++;
    }

    while( ((*szCmd) != 0x20) && ((*szCmd) != 0) )
    {
        *(szTkn++) = *(szCmd++);
        iTmp++;
        if(iTmp >= MAX_TOKEN_SIZE)
        {
            if(!CLI_IS_ENV_SILENT(*ulStatus))
                IO_Print(CLI_CMD_TOO_BIG) ;
            SET_BIGTOKEN(*ulStatus);
            return TBS_FAILED;
        }
    }

    if(iTmp == 0)
    {
        return TBS_FAILED;
    }

    *szTkn = 0;
    *szCmdString = szCmd;

    return TBS_SUCCESS;
}

/*※※※※※※※※※※※※※※*/
/* 【2】命令字的检验          */
/*※※※※※※※※※※※※※※*/
/*********************************************************************/
/* 函数名称 : CLI_CheckElementObject()                                   */
/* 函数功能 : 检验命令对象的合法性                                   */
/* 输入参数 : szToken   当前的命令字                                 */
/*            pPointer  命令解释核心结构                             */
/*            pulStatus 命令解释状态                                 */
/* 输出参数 : pulStatus 命令解释状态                                 */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_CheckElement                                           */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_CheckElementObject(char  *szToken,
                    PST_CLI_POINTER pPointer,
                    ULONG  *pulStatus )
{
    PST_CLI_CMDKEY pCmdKey, pSaveObject;
    PST_CLI_INTP_POINTER pexpP;
    ULONG  ulCount = 0 ;
    char  szResult[256];

    szResult[0] = '\0';
    pCmdKey = pPointer->expP.pObj;
    pexpP = &(pPointer->expP);

    pSaveObject = pCmdKey ;
    while(pCmdKey != NULL)
    {
        if(CLI_IS_CURRENT_KNOWN_KEY(pCmdKey))
        {
			/* 处理全字匹配命令对象的情况 */
            if( STB_StriCmp( szToken, pCmdKey->szName ) == 0 )
            {
                pSaveObject = pCmdKey ;
				/* 只要出现全字匹配，就进入ulCount=1的流程(即使在全字匹配前ulCount>=1) */
                ulCount = 1;
                break;
            }

            if( strmatch( szToken, pCmdKey->szName ) == 0 )
            {
                if((IS_NOFORM(*pulStatus) && (pCmdKey->ulNoFlag == 1))
                  || (!IS_NOFORM(*pulStatus)))
                {
                    ulCount++;
                    if(ulCount == 1)    /* 由于先作加法后判断，所以此处判断是否为1 */
                        pSaveObject = pCmdKey ;
                    CLI_MatchMore(szResult,pCmdKey->szName);
                }
            }
        }

        /* match next fraternal object */
        pCmdKey = pCmdKey->pNext;
    }/* end of while() */

    if(ulCount == 1)        /* 由于先作加法后判断，所以此处判断是否为1 */
    {
        if(IS_NOFORM(*pulStatus) && (pSaveObject->ulNoFlag == 0))
            return TBS_FAILED;
        STB_StrCpy( &(pPointer->szPatch[0]),
              (pSaveObject->szName + STB_StrLen(szToken)) );
        pexpP->pObj = pSaveObject;
        pexpP->pParam = NULL;
        pexpP->pValue = NULL;
        COPY_POINTER_TO_CUR( pPointer );
        SET_MATCHSUCC(*pulStatus);
        return TBS_SUCCESS;
    }
    else if(ulCount > 1)    /* check多个字符 */
    {
        STB_StrCpy( &(pPointer->szPatch[0]),CLI_SetPatch(szToken,&szResult[0]));
        pPointer->ucMatchFlag = CTMF_PARTIAL;
        return TBS_FAILED;
    }
    if(pCmdKey == NULL)
    {
        return TBS_FAILED;
    }

    SET_MATCHSUCC(*pulStatus);
    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数名称 : CLI_CheckElementParameter()                                */
/* 函数功能 : 检验命令参数的合法性                                   */
/* 输入参数 : szToken   当前的命令字                                 */
/*            pPointer  命令解释核心结构                             */
/*            pulStatus 命令解释状态                                 */
/* 输出参数 : pulStatus 命令解释状态                                 */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_CheckElement                                           */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_CheckElementParameter(char  *szToken,
                    PST_CLI_POINTER pPointer,
                    ULONG  *pulStatus )
{
    PST_CLI_INTP_POINTER pexpP;
    PST_CLI_PARAMETER pParam, pSaveParam, pParamMutex;
    BOOL fParamMatched;
    ULONG  ulIndex;
    ULONG  ulMtIndex;
    ULONG  ulCount = 0 ;
    char  szResult[256];

    szResult[0] = '\0';
    pexpP = &(pPointer->expP);
    pParam = pPointer->expP.pParam;
    fParamMatched = FALSE;

    if(pParam == NULL)
    {
        return TBS_FAILED;
    }
    CLR_MUTEXPARAM(*pulStatus);

    if(CLI_PARAM_IS_INDEX(pParam) && (NULL == pPointer->pIndexParam))
    {
        pPointer->pIndexParam = pParam;
    }
    if( CLI_PARAM_IS_MUTEX(pParam) &&
       ((szToken != NULL) && (*szToken != '\0')))
    {
        pSaveParam = pParam ;
        ulMtIndex = pParam->ulAbsoluteIndex;

        while((pParam != NULL)
           && (CLI_PARAM_IS_MUTEX(pParam))
           && (pParam->ulAbsoluteIndex == ulMtIndex))
        {
            /* 处理全字匹配命令参数的情况 */
            if( STB_StriCmp( szToken, pParam->szName ) == 0 )
            {
                /* 只要出现全字匹配，就进入ulCount=1的流程(即使在全字匹配前ulCount>=1) */
                ulCount = 1;
                pSaveParam = pParam ;
                break;
            }

           /* Partial Matching */
            else if( strmatch( szToken, pParam->szName ) == 0 )
            {
                ulCount++;
                pSaveParam = pParam ;
                CLI_MatchMore(szResult,pParam->szName);
            }

            pParam = pParam->pNext;

        }

        if( ulCount == 1)
        {
            /* 此处pc-lint有告警,确认不会有问题 */
            STB_StrCpy( &(pPointer->szPatch[0]),
                    (pSaveParam->szName + STB_StrLen(szToken)) );
            STB_StrCat(szToken, pPointer->szPatch);
            pPointer->ucMatchFlag = CTMF_FULL;
            fParamMatched = TRUE;
            pexpP->pParam = pSaveParam;
            pParam = pSaveParam;
            COPY_POINTER_TO_CUR( pPointer );
            SET_MUTEXPARAM(*pulStatus);
            if( IS_VIRTUAL_P(*pulStatus) )
            {
                CLR_VIRTUAL_P(*pulStatus);
            }

        }
        else if(ulCount > 1)
        {
            STB_StrCpy( &(pPointer->szPatch[0]),CLI_SetPatch(szToken,&szResult[0]));
            pPointer->ucMatchFlag = CTMF_PARTIAL;
            if( IS_VIRTUAL_P(*pulStatus) )
            {
                CLR_VIRTUAL_P(*pulStatus);
            }

            return TBS_FAILED;
        }


    }
    else if( !CLI_PARAM_NEED_NAME(pParam) )
    {

        if( CLI_PARAM_IS_OPTIONAL(pParam) )
        {

            SET_VIRTUAL_P(*pulStatus);
        }

        SET_SKIP_FLAG(*pulStatus);
        fParamMatched = TRUE;
        COPY_POINTER_TO_CUR( pPointer );
    }
    else if( strmatch( szToken, pexpP->pParam->szName ) == 0 )
    {
        if(szToken == NULL)
        {
            return TBS_FAILED;
        }

        // 可选参数在交互后的行编辑情况下比较特殊，作以下处理
        if ((ULONG )pParam != m_stCliWS.ulExpectEle && m_stCliWS.ulExpectEle != 0)
        {
            pSaveParam = (PST_CLI_PARAMETER)m_stCliWS.ulExpectEle;
            // 如果当前参数为可选，则依次往下判断，看是否有合适的参数
            if (CLI_PARAM_IS_OPTIONAL(pSaveParam))
            {
                if (strmatch(szToken, pSaveParam->szName) != 0)
                {
                    pexpP->pParam = pParam;
                    COPY_POINTER_TO_CUR( pPointer );
                    return TBS_SUCCESS;
                }
                pexpP->pParam = pSaveParam;
                pParam = pSaveParam;
            }
        }
        STB_StrCpy( &(pPointer->szPatch[0]),
                 (pParam->szName + STB_StrLen(szToken)) );
        STB_StrCat(szToken, pPointer->szPatch);
        pPointer->ucMatchFlag = CTMF_FULL;
        fParamMatched = TRUE;
        COPY_POINTER_TO_CUR( pPointer );
        if( IS_VIRTUAL_P(*pulStatus) )
        {
            CLR_VIRTUAL_P(*pulStatus);
        }

    }

    if(fParamMatched == FALSE)
    {
        /* 如果参数互斥，这期望参数指针指向最后一个互斥参数 */
        if( CLI_PARAM_IS_MUTEX(pexpP->pParam) )
        {

            /* 参数互斥可选 */
            if(CLI_PARAM_IS_OPTIONAL(pexpP->pParam))
            {
                pParam = pexpP->pParam;
                pParamMutex = NULL;
                ulMtIndex = pParam->ulAbsoluteIndex;
                while((pParam != NULL)
                     && CLI_PARAM_IS_OPTIONAL(pParam)
                     && CLI_PARAM_IS_MUTEX(pParam)
                     && pParam->ulAbsoluteIndex == ulMtIndex)
                {
                    pParamMutex = pParam;
                    pParam = pParam->pNext;
                }
                pexpP->pParam = pParamMutex;
                SET_SKIP_FLAG(*pulStatus);
                COPY_POINTER_TO_CUR( pPointer );
                return TBS_SUCCESS;
            }
            else
            {
                return TBS_FAILED;
            }
        }
        else if( CLI_PARAM_IS_OPTIONAL(pexpP->pParam) || IS_VIRTUAL_P(*pulStatus))
        {
            if (szToken == NULL)
            {
                return TBS_FAILED ;
            }

            if ( (IS_INTERACTIVE(*pulStatus) || ((ULONG )pexpP->pParam == m_stCliWS.ulExpectEle))
              && (*szToken != 0) )
            {
                return TBS_FAILED ;
            }

            SET_SKIP_FLAG(*pulStatus);
            COPY_POINTER_TO_CUR( pPointer );
            return TBS_SUCCESS;
        }
        return TBS_FAILED;
    }
    else
    {
        if (pParam == NULL)
        {
            return TBS_FAILED ;
        }

        if( CLI_PARAM_IS_INDEX(pParam)
         && !IS_INTERACTIVE(*pulStatus)
         && !CLI_IS_ENV_NEWLINE(m_stCliWS.ulStatus))
            CLI_AdjustParam(pPointer);

        if(!IS_SKIP_FLAG(*pulStatus))
        {
            ulIndex = pParam->ulAbsoluteIndex
                        + pPointer->ulValueIndex;
            if( CLI_SaveParameter( pParam->szName, pParam->szName,
                    ulIndex) != TBS_SUCCESS )
            {
                IO_Print(CLI_PARAM_CANNOT_SAVE, szToken, ulIndex);
                return TBS_FAILED;
            }
        }
    }

    SET_MATCHSUCC(*pulStatus);
    return TBS_SUCCESS;

}

/*********************************************************************/
/* 函数名称 : CLI_CheckElementValue()                                 */
/* 函数功能 : 检验命令参数值的合法性                                 */
/* 输入参数 : szToken   当前的命令字                                 */
/*            pPointer  命令解释核心结构                             */
/*            pulStatus 命令解释状态                                 */
/* 输出参数 : pulStatus 命令解释状态                                 */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_CheckElement                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_CheckElementValue( char  *szToken,
                    PST_CLI_POINTER pPointer,
                    ULONG  *pulStatus )
{
    BOOL bValueMatched;
    int    iVal;
    ULONG  uVal;
    ULONG  ulIndex;
    PCSTR strInfo= CLI_ML_NULL;
    PST_CLI_INTP_POINTER pexpP;
    PST_CLI_ENUM pKeyword;
    char  szTmpToken[2][MAX_TOKEN_SIZE+1];
    UCHAR ucDataType;

    szTmpToken[0][0] = 0 ;
    szTmpToken[1][0] = 0 ;
    pexpP = &(pPointer->expP);
    ulIndex = pexpP->pValue->ulAbsoluteIndex + pPointer->ulValueIndex;

    if ( IS_INTERACTIVE(*pulStatus) && (0 == szToken[0]) )
    {
        if ( CLI_VALUE_HAVE_DEFAULT(pexpP->pValue) )
        {
            STB_StrCpy(szToken,pexpP->pValue->szDefault);
            IO_OutString(szToken);
        }
    }

    if ( IS_INTERACTIVE(*pulStatus) && (strchr(szToken, ' ') != NULL) )
    {
        IO_Print(CLI_VALUE_HAS_BLANK);
        return TBS_FAILED ;
    }

    bValueMatched = FALSE;
    ucDataType = pexpP->pValue->valueType.ucDataType;

    switch(ucDataType)
    {
        case CLI_DATA_VIK    :
        case CLI_DATA_VI     :
                bValueMatched = ((ULONG )_isintstr(szToken) == TBS_SUCCESS) ;
                if( TRUE ==  bValueMatched )
                {
                    if(CLI_VALUE_USER_CHECK(pexpP->pValue))
                    {
                        bValueMatched = (pexpP->pValue->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                    }
                    else if((pexpP->pValue->valueType.ucAttrib & CLI_VALPROP_CHECK))
                    {
                        if((ULONG )_VerifyAndCvtDwordI(szToken,
                            pexpP->pValue->lIntFilter[0],
                            pexpP->pValue->lIntFilter[1],
                            &iVal) != TBS_SUCCESS)
                        {
                            bValueMatched = FALSE;
                            strInfo = CLI_INTEG_OVERFLOW;
                        }

                        /* 去掉整型字符串前面的0，方便以后处理 */
                        else if ( STB_StriCmp( szToken, "" ) != 0 )
                        {
                            STB_Sprintf(szToken, "%d", iVal);
                        }
                    }
                }
                break;
        case CLI_DATA_VUK    :
        case CLI_DATA_VU     :
                bValueMatched = ((ULONG )_isdecstr(szToken) == TBS_SUCCESS) ;
                if( TRUE == bValueMatched )
                {
                    if(CLI_VALUE_USER_CHECK(pexpP->pValue))
                    {
                        bValueMatched = (pexpP->pValue->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                    }
                    else if((pexpP->pValue->valueType.ucAttrib & CLI_VALPROP_CHECK))
                    {
                        if((ULONG )_VerifyAndCvtDword(szToken,
                            (ULONG )pexpP->pValue->lIntFilter[0],
                            (ULONG )pexpP->pValue->lIntFilter[1],
                            &uVal) != TBS_SUCCESS)
                        {
                            bValueMatched = FALSE;
                            strInfo = CLI_INTEG_OVERFLOW;
                        }

                        else if ( STB_StriCmp( szToken, "" ) != 0 )
                        {
                            STB_Sprintf(szToken, "%d", uVal);
                        }
                    }
                }
                break;
        case CLI_DATA_VHK    :
        case CLI_DATA_VH     :
                bValueMatched = ((ULONG )_isxdecstr(szToken) == TBS_SUCCESS) ;
                if( TRUE == bValueMatched )
                {
                    if(CLI_VALUE_USER_CHECK(pexpP->pValue))
                    {
                        bValueMatched = (pexpP->pValue->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                    }
                    else if((pexpP->pValue->valueType.ucAttrib & CLI_VALPROP_CHECK))
                    {
                        if((ULONG )_VerifyAndCvtDwordH(szToken,
                            (ULONG )pexpP->pValue->lIntFilter[0],
                            (ULONG )pexpP->pValue->lIntFilter[1],
                            &uVal) != TBS_SUCCESS)
                        {
                            bValueMatched = FALSE;
                            strInfo = CLI_HEX_OVERFLOW;
                        }
                    }
                }
                break;

        case CLI_DATA_VS     :
                if(CLI_VALUE_USER_CHECK(pexpP->pValue))
                {
                    bValueMatched = (pexpP->pValue->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                }
                else
                {
                    if(STB_StrLen(szToken) == 0)
                        break;

                    bValueMatched = TRUE;
                    if((pexpP->pValue->valueType.ucAttrib & CLI_VALPROP_CHECK))
                    {
#if 0 /* Do not block characters such as '&', '%', '`', some parameters can have such characters. */
                        int i;
#endif

                        if( (pexpP->pValue->valueType.ulDataLen > 0)
                            && (STB_StrLen(szToken) > pexpP->pValue->valueType.ulDataLen) )
                        {
                            bValueMatched = FALSE;
                            strInfo = CLI_STRING_TOO_LONG;
                            break;
                        }
#if 0 /* Do not block characters such as '&', '%', '`', some parameters can have such characters. */
                        for (i = 0; szToken[i] != '\0'; i++)
                        {
                            if ((szToken[i] == '&' )
                             || (szToken[i] == '%' )
                             || (szToken[i] == '`' ))
                            {
                                bValueMatched = FALSE;
                                strInfo = "--";
                                break;
                            }
                        }
#endif
                    }

                }
                break;
        /* Keyword Value Type */
        case CLI_DATA_K      :
                if(CLI_VALUE_USER_CHECK(pexpP->pValue))
                {
                    bValueMatched = (pexpP->pValue->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                }
                else
                {
                    bValueMatched = ((ULONG )_isempty(szToken)!=TBS_SUCCESS) ;
                    if( TRUE == bValueMatched )
                    {
                        pKeyword = pexpP->pValue->keywords.pKeyword;
                        if(CLI_MatchKeywords(szToken,pKeyword,pPointer) == TBS_SUCCESS)
                        {
                            if(pPointer->ucMatchFlag == CTMF_FULL)
                                break ;
                        }
                        else
                        {
                            bValueMatched = FALSE;
                            strInfo = CLI_INVALID_KEY;
                        }
                    }
                }
                break;

        case CLI_DATA_IPK     :
        case CLI_DATA_IP      :
                if(CLI_VALUE_USER_CHECK(pexpP->pValue))
                {
                    bValueMatched = (pexpP->pValue->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                }
                else if((pexpP->pValue->valueType.ucAttrib & CLI_VALPROP_CHECK))
                {
                    if(_VerifyGeneralIPAddress(szToken) != TBS_SUCCESS)
                    {
                        /* Match failed */
                        bValueMatched = FALSE;
                        strInfo = CLI_INVALID_IP;
                    }
                    else bValueMatched = TRUE;
                }

                break;

        case CLI_DATA_MACK     :
        case CLI_DATA_MAC      :
                if(CLI_VALUE_USER_CHECK(pexpP->pValue))
                {
                    bValueMatched = (pexpP->pValue->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                }
                else if((pexpP->pValue->valueType.ucAttrib & CLI_VALPROP_CHECK))
                {
                    if(_VerifyMACAddress(szToken) != TBS_SUCCESS)
                    {
                        /* Match failed */
                        bValueMatched = FALSE;
                        strInfo = CLI_INVALID_MAC;
                    }
                    else bValueMatched = TRUE;

                }
                break;
        case CLI_DATA_MASK      :
                if(CLI_VALUE_USER_CHECK(pexpP->pValue))
                {
                    bValueMatched = (pexpP->pValue->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                }
                else if((pexpP->pValue->valueType.ucAttrib & CLI_VALPROP_CHECK))
                {
                    if(_VerifyMASKAddress(szToken) != TBS_SUCCESS)
                    {
                        /* Match failed */
                        bValueMatched = FALSE;
                        strInfo = CLI_INVALID_MASK;
                    }
                    else bValueMatched = TRUE;

                }

                break;

        case CLI_DATA_DATE   :
                if(CLI_VALUE_USER_CHECK(pexpP->pValue))
                {
                    bValueMatched = (pexpP->pValue->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                }
                else if((pexpP->pValue->valueType.ucAttrib & CLI_VALPROP_CHECK))
                {
                    if(_VerifyDate(szToken) != TBS_SUCCESS)
                    {
                        /* Match failed */
                        bValueMatched = FALSE;
                        strInfo = CLI_INVALID_DATE;
                    }
                    else bValueMatched = TRUE;

                }
                break;

        case CLI_DATA_TIME   :
                if(CLI_VALUE_USER_CHECK(pexpP->pValue))
                {
                    bValueMatched = (pexpP->pValue->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                }
                else if((pexpP->pValue->valueType.ucAttrib & CLI_VALPROP_CHECK))
                {
                    if(_VerifyTime(szToken) != TBS_SUCCESS)
                    {
                        /* Match failed */
                        bValueMatched = FALSE;
                        strInfo = CLI_INVALID_TIME;
                    }
                    else bValueMatched = TRUE;

                }
                break;

        default :
                if(CLI_ValidateUserType(pexpP->pValue->valueType.ucDataType) == TBS_SUCCESS)
                {
                    bValueMatched =
                        (CLI_GetUserType(pexpP->pValue->valueType.ucDataType)->pCheckFunc(szToken,NULL) == TBS_SUCCESS);
                }
                else
                {
                    MT_ERRLOG(ucDataType);
                    IO_OutString(" [VALUE=^") ;
                    IO_OutString( szToken );
                    IO_OutString("]");
                    return TBS_FAILED;
                }
    }

    if (bValueMatched != TRUE
     && (ucDataType == CLI_DATA_VIK || ucDataType == CLI_DATA_VUK || ucDataType == CLI_DATA_VHK
      || ucDataType == CLI_DATA_IPK || ucDataType == CLI_DATA_MACK))
    {
        bValueMatched = ((ULONG )_isempty(szToken) != TBS_SUCCESS) ;
        if( TRUE == bValueMatched )
        {
            pKeyword = pexpP->pValue->keywords.pKeyword;
            if(CLI_MatchKeywords(szToken,pKeyword,pPointer) != TBS_SUCCESS)
            {
                bValueMatched = FALSE;
            }
        }
    }

    if( bValueMatched == TRUE )
    {
        if( IS_VIRTUAL_P( *pulStatus ) )
        {
            CLR_VIRTUAL_P( *pulStatus );
        }

        COPY_POINTER_TO_CUR( pPointer );
        if(CLI_VALUE_IS_MUTEX(pexpP->pValue))
            SET_MUTEXVALUE(*pulStatus);
        if( (CLI_SaveParameter( szToken, pexpP->pValue->szName,
                    ulIndex) != TBS_SUCCESS )
           && (!CLI_IS_ENV_SILENT(*pulStatus)))
        {

            IO_Print(CLI_PARAM_CANNOT_SAVE, szToken, ulIndex);
            return TBS_FAILED;
        }

        if (szTmpToken[0][0] != 0)
            STB_StrCpy(szToken, szTmpToken[0]) ;
    }
    else
    {
        if( !IS_INTERACTIVE(*pulStatus) )
        {
            if((!CLI_IS_ENV_SILENT(*pulStatus)))
            {
                if (strInfo != CLI_ML_NULL)
                    IO_Print(strInfo);
                return TBS_FAILED;
            }
        }
        else
        {
            if (*szToken != 0)
            {
                if(!CLI_IS_ENV_SILENT(*pulStatus))
                {
                    if (strInfo != CLI_ML_NULL)
                        IO_Print(strInfo);
                }
                return TBS_FAILED;
            }

            if( (CLI_VALUE_IS_OPTIONAL(pexpP->pValue)
                || IS_VIRTUAL_P(*pulStatus))||((CLI_VALUE_IS_MUTEX(pexpP->pValue))
                 && (pexpP->pValue->pNext != NULL)
                 && CLI_VALUE_IS_MUTEX(pexpP->pValue->pNext)))
            {
                SET_SKIP_FLAG(*pulStatus);
                COPY_POINTER_TO_CUR( pPointer );
                return TBS_SUCCESS;
            }
            else if(!CLI_IS_ENV_SILENT(*pulStatus))
            {
                if (strInfo != CLI_ML_NULL)
                    IO_Print(strInfo);
                return TBS_FAILED;
            }
        }
        return TBS_FAILED;
    }

    SET_MATCHSUCC(*pulStatus);
    return TBS_SUCCESS;

}


/*********************************************************************/
/* 函数名称 : CLI_CheckElementParameter()                             */
/* 函数功能 : 检验命令字的合法性                                     */
/* 输入参数 : szToken   当前的命令字                                 */
/*            pPointer  命令解释核心结构                             */
/*            pulStatus 命令解释状态                                 */
/* 输出参数 : pulStatus 命令解释状态                                 */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_CheckElement                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_CheckElement(char  *szToken, PST_CLI_POINTER pPointer, ULONG  *pulStatus)
{
    ULONG  ulRet;
    ULONG  ulSts;

    CLR_SKIP_FLAG(*pulStatus);
    ulSts = *pulStatus;

    CLR_MATCHSUCC(*pulStatus);
    if(IS_WAIT_NULL(ulSts))
    {
        IO_OutString("\r\n Command Interpreter status error(waiting null element)! (Program Error)") ;
        MT_ERRLOG(*pulStatus);
        ulRet = 1;
    }
    else if(IS_WAIT_KEY(ulSts))
    {
        ulRet = CLI_CheckElementObject(szToken, pPointer, pulStatus );
    }
    else if(IS_WAIT_PARAM(ulSts))
    {
        ulRet = CLI_CheckElementParameter(szToken, pPointer, pulStatus );
    }
    else if(IS_WAIT_VALUE(ulSts))
    {
        ulRet = CLI_CheckElementValue(szToken, pPointer, pulStatus );
    }
    else
    {
        MT_ERRLOG(*pulStatus);
        return TBS_FAILED;
    }

    return ulRet;

}
/*※※※※※※※※※※※※※※*/
/* 【3】命令的可执行性分析    */
/*※※※※※※※※※※※※※※*/
/*********************************************************************/
/* 函数名称 : CLI_UserPermitted()                                     */
/* 函数功能 : 检验执行该命令的用户权限                               */
/* 输入参数 : ulLevel   用户级别                                     */
/*            pCmdKey   所检验的对象                                 */
/*            ulStatus  命令解释的状态                               */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_IntpKernelProc                                      */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_UserPermitted(ULONG  ulLevel, PST_CLI_CMDKEY pCmdKey, ULONG  ulStatus)
{
    CLI_OPERAT_LEVEL_T level;

    if(ulLevel > CLI_AL_DEBUG)
    {
        DBG_Out("\r\n User have an invalid right level! (level = %ld)", ulLevel) ;
        MT_ERRLOG(ulLevel);

        CLI_SetTermStatus(TERM_SLEEPING);
        return TBS_FAILED;
    }
    else
    {
        if( IS_NOFORM(ulStatus) )
        {
            level = pCmdKey->noAction.rightLevel;
        }
        else
        {
            level = pCmdKey->action.rightLevel;
        }

        /* The bigger level is the higher one */
        if( ulLevel >= (ULONG )level )
        {
            return TBS_SUCCESS;
        }
        else
        {
            return TBS_FAILED;
        }
    }
}

/*********************************************************************/
/* 函数名称 : CLI_GetUserRightLevel()                                 */
/* 函数功能 : 获取用户的级别                                         */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     : 当前用户级别                                           */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetUserRightLevel()
{
    return m_stCliWS.ulLevel;
}

/*********************************************************************/
/* 函数名称 : CLI_IsObjectExecutable()                                */
/* 函数功能 : 检查对象是否具有可执行的属性                           */
/* 输入参数 : pCmdKey:检查对象                                       */
/*            ulStatus:命令解释状态                                  */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_IsObjectExecutable(PST_CLI_CMDKEY pCmdKey, ULONG  ulStatus)
{
    if(ulStatus == HAVEACTIONS)
    {
        if( (pCmdKey->noAction.pFunction != NULL) ||
                (pCmdKey->noAction.modeConv != CLI_MC_NULL)
            ||
            (pCmdKey->action.pFunction != NULL) ||
                (pCmdKey->action.modeConv != CLI_MC_NULL))
        {
            return TBS_SUCCESS;
        }
    }
    else if(IS_NOFORM(ulStatus))
    {
        if( (pCmdKey->noAction.pFunction != NULL) ||
                (pCmdKey->noAction.modeConv != CLI_MC_NULL))
        {
            return TBS_SUCCESS;
        }
    }
    else
    {
        if( (pCmdKey->action.pFunction != NULL) ||
                (pCmdKey->action.modeConv != CLI_MC_NULL))
        {
            return TBS_SUCCESS;
        }
    }

    return TBS_FAILED;
}

/*※※※※※※※※※※※※※※*/
/* 【4】获取期望的命令字类型  */
/*※※※※※※※※※※※※※※*/
/*********************************************************************/
/* 函数名称 : CLI_BackTopVal()                                       */
/* 函数功能 : 参数值遍历完之后的回溯                                 */
/* 输入参数 : pPointer:命令解释核心函数                              */
/*            pulStatus:命令解释状态                                 */
/* 输出参数 :                                                        */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_BackTopVal( PST_CLI_POINTER pPointer, ULONG  * pulStatus )
{
    /* from value backdate to parameter */
    /* Here only expected pPointer->expP is usable */
    PST_CLI_PARAMETER pParam;
    PST_CLI_VALUE pValue;
    PST_CLI_INTP_POINTER pexpP;
    ULONG  ulMutexValue = 0;
    ULONG  ulMtIndex;

    pexpP = &(pPointer->expP);
    //pCmdKey = pexpP->pObj;
    pParam = pexpP->pParam;
    pValue = pexpP->pValue;
    if(IS_MUTEXVALUE(*pulStatus))
    {
        CLR_MUTEXVALUE(*pulStatus);
        ulMutexValue = 1;
    }
    /* Check if any subparam needed */
    if( (!IS_SKIP_FLAG(*pulStatus) || CLI_VALUE_IS_OPTIONAL(pValue) || ulMutexValue)
        && CLI_PARAM_NEED_SUB(pParam) )
    {
        pParam = pParam->subParam;

        pexpP->pParam = pParam;
        pexpP->pValue = NULL;
        SET_WAIT_PARAM( *pulStatus );
        return TBS_SUCCESS;
    }
    /* 如果参数互斥，这得到非互斥的第一个兄弟参数指针 */
    else if(CLI_PARAM_IS_MUTEX(pParam))
    {
        ulMtIndex = pParam->ulAbsoluteIndex;
        while((pParam) && CLI_PARAM_IS_MUTEX(pParam)
            && pParam->ulAbsoluteIndex == ulMtIndex)
        {
            pParam = pParam->pNext;
        }
        if(NULL != pParam)
        {
            pexpP->pParam = pParam;
            pexpP->pValue = NULL;
            SET_WAIT_PARAM( *pulStatus );
            return TBS_SUCCESS;
        }
    }

    else if( CLI_PARAM_YOUNGER_BROTHER(pParam))

    {
        pParam = pParam->pNext;
        pexpP->pParam = pParam;
        pexpP->pValue = NULL;
        SET_WAIT_PARAM( *pulStatus );
        if( IS_VIRTUAL_P(*pulStatus) )
        {
            CLR_VIRTUAL_P( *pulStatus );
        }

        return TBS_SUCCESS;
    }
    /* 如果参数不为空，则回溯 */
    pParam = pexpP->pParam;
    if(NULL != pParam)
    {
        /* Back to parent parameter */
        if( CLI_PARAM_PARENT(pParam) )
        {
            pexpP->pParam = pParam->pParent;
            pexpP->pValue = NULL;
            return (CLI_BackTopParam( pPointer, pulStatus ));
        }
        else
        {
            pexpP->pParam = NULL;
            pexpP->pValue = NULL;
            return (CLI_BackTopKey( pPointer, pulStatus ));
        }
    }
    else
    {
        pexpP->pParam = NULL;
        pexpP->pValue = NULL;
        return (CLI_BackTopKey( pPointer, pulStatus ));
    }
}


/*********************************************************************/
/* 函数名称 : CLI_BackTopParam()                                         */
/* 函数功能 : 参数遍历完后的回溯(回溯到父参数)                       */
/*********************************************************************/
 ULONG  CLI_BackTopParam( PST_CLI_POINTER pPointer, ULONG  * pulStatus )
{
    PST_CLI_PARAMETER pParam, pParamTemp;
    PST_CLI_INTP_POINTER pexpP;
    ULONG  ulMtIndex;

    pexpP = &(pPointer->expP);
    pParam = pexpP->pParam;

    if(pParam == NULL)
    {
        return (CLI_BackTopKey( pPointer, pulStatus ));
    }
    if(CLI_PARAM_IS_MUTEX(pParam))
    {
        ulMtIndex = pParam->ulAbsoluteIndex;
        while((pParam) && CLI_PARAM_IS_MUTEX(pParam))
        {
            pParam = pParam->pNext;
        }

        if(NULL != pParam)
        {
            pexpP->pParam = pParam;
            pexpP->pValue = NULL;
            SET_WAIT_PARAM( *pulStatus );
            return TBS_SUCCESS;
        }
    }
    else if( CLI_PARAM_YOUNGER_BROTHER(pParam) )
    {

        pParam = pParam->pNext;

        pexpP->pParam = pParam;
        pexpP->pValue = NULL;
        SET_WAIT_PARAM( *pulStatus );
        if( IS_VIRTUAL_P(*pulStatus) )
        {
            CLR_VIRTUAL_P( *pulStatus );
        }

        return TBS_SUCCESS;
    }
    pParam = pexpP->pParam;
    if(NULL != pParam)
    {
        while( CLI_PARAM_PARENT(pParam) )
        {
            if( IS_VIRTUAL_P(*pulStatus) )
            {
                CLR_VIRTUAL_P( *pulStatus );
            }

            pParam = pParam->pParent;
            if(NULL == pParam)
            {
                break;
            }
            if( CLI_PARAM_YOUNGER_BROTHER(pParam))
            {
                pParamTemp = pParam;
                if(CLI_PARAM_IS_MUTEX(pParamTemp))
                {
                    ulMtIndex = pParam->ulAbsoluteIndex;
                    while((pParamTemp) && CLI_PARAM_IS_MUTEX(pParamTemp)
                        && ulMtIndex == pParam->ulAbsoluteIndex)
                    {
                        pParamTemp = pParamTemp->pNext;
                    }
                }
                else
                {
                    pParamTemp = pParamTemp->pNext;
                }
                if(NULL != pParamTemp)
                {
                    pexpP->pParam = pParamTemp;
                    pexpP->pValue = NULL;

                    SET_WAIT_PARAM( *pulStatus );
                    return TBS_SUCCESS;
                }
            }

        }

        pexpP->pParam = pParam;
        pexpP->pValue = NULL;
        return (CLI_BackTopKey( pPointer, pulStatus ));
    }
    else
    {
        pexpP->pParam = pParam;
        pexpP->pValue = NULL;
        return (CLI_BackTopKey( pPointer, pulStatus ));
    }

}


/*********************************************************************/
/* 函数名称 : CLI_BackTopParam()                                         */
/* 函数功能 : 参数遍历完后的回溯(回溯到对象)                         */
/*********************************************************************/
ULONG  CLI_BackTopKey( PST_CLI_POINTER pPointer, ULONG  * pulStatus )
{
    PST_CLI_CMDKEY pCmdKey;
    PST_CLI_INTP_POINTER pexpP;


    if( IS_VIRTUAL_P(*pulStatus) )
    {
        CLR_VIRTUAL_P( *pulStatus );
    }

    pexpP = &(pPointer->expP);
    pCmdKey = pexpP->pObj;

    if( CLI_IsObjectExecutable(pCmdKey, HAVEACTIONS) == TBS_SUCCESS )
    {
        SET_COMPLETE( *pulStatus );
        if(IS_LASTTOKEN(*pulStatus) && IS_SKIP_FLAG(*pulStatus))
        {
            SET_UNWANTED(*pulStatus);
        }
        return 2;   /* 留作以后扩展接口用，以区分错误种类 */
    }

    if( CLI_KEY_HAVE_SUBOBJECTS(pCmdKey) )
    {
        pexpP->pObj = pCmdKey->pSubCmdKey;
        pexpP->pParam = NULL;
        pexpP->pValue = NULL;
        SET_WAIT_KEY( *pulStatus );
        return TBS_SUCCESS;
    }

    SET_COMPLETE( *pulStatus );
    if(IS_LASTTOKEN(*pulStatus) && IS_SKIP_FLAG(*pulStatus))
    {
        SET_UNWANTED(*pulStatus);
    }

    return TBS_FAILED;

}

/*********************************************************************/
/* 函数名称 : CLI_AdjustParam()                                       */
/* 函数功能 : 对于带有索引调整属性的参数、调整其索引位置             */
/*********************************************************************/
ULONG  CLI_AdjustParam(PST_CLI_POINTER pPointer)
{
    PST_CLI_PARAMETER pParam, pCurParam, pPrevious, pNext;

    pCurParam = pPointer->curP.pParam;
    if(NULL == pCurParam)
        return TBS_FAILED;
    if(pCurParam == pPointer->pIndexParam)
    {
        pPointer->pIndexParam = pCurParam->pNext;
        return TBS_SUCCESS;
    }
    if(NULL == pPointer->pIndexParam)
    {
        return TBS_SUCCESS;
    }
    else
    {
        pParam = pPointer->pIndexParam;
        pPointer->pIndexParam = pPointer->pIndexParam->pPrevious;
        pPrevious = pCurParam->pPrevious;
        pNext = pCurParam->pNext;

        if(NULL != pPrevious)
            pPrevious->pNext = pNext;
        if(NULL != pNext)
            pNext->pPrevious = pPrevious;

        if(NULL == pPointer->pIndexParam)
        {
            pCurParam->pPrevious = NULL;
            if(pParam->pParent)
            {
                pCurParam->pParent = pParam->pParent;
                pCurParam->pParent->subParam = pCurParam;
            }
        }
        else
        {
            pPointer->pIndexParam->pNext = pCurParam;
            pCurParam->pPrevious = pPointer->pIndexParam;
        }

        pCurParam->pNext = pParam;
        pParam->pPrevious = pCurParam;
        pPointer->pIndexParam = pCurParam->pNext ;
    }
    return TBS_SUCCESS;
}
/*********************************************************************/
/* 函数名称 : CLI_GetNextElement_O()                                 */
/* 函数功能 : 获取下一个期望的命令字类型                             */
/* 输入参数 : pPointer:命令解释核心结构                              */
/*            pulStatus:命令解释状态                                 */
/* 输出参数 : pulStatus:命令解释状态                                 */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_GetExpectedElement                                  */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetNextElement_O( PST_CLI_POINTER pPointer, ULONG  * pulStatus )
{
    PST_CLI_CMDKEY pCmdKey;
    PST_CLI_PARAMETER pParam;
    PST_CLI_INTP_POINTER pexpP;
    PST_CLI_INTP_POINTER pcurP;

    pexpP = &(pPointer->expP);
    pcurP = &(pPointer->curP);
    pCmdKey = pcurP->pObj;

    if(pCmdKey == NULL)
    {
        return TBS_FAILED;
    }

    if( IS_NOFORM(*pulStatus) )
    {
        if( CLI_KEY_HAVE_PARAMETER1(pCmdKey) )
        {
            pParam = pCmdKey->noAction.pParamLink->pParam;
        }
        else
        {
            pParam = NULL;
        }
    }
    else
    {
        if( CLI_KEY_HAVE_PARAMETER0(pCmdKey) )
        {
            pParam = pCmdKey->action.pParamLink->pParam;
        }
        else
        {
            pParam = NULL;
        }
    }

    if( pParam != NULL )
    {
        pexpP->pObj = pCmdKey;
        pexpP->pParam = pParam;
        pexpP->pValue = NULL;
        SET_WAIT_PARAM( *pulStatus );
        return TBS_SUCCESS;
    }

    if ( (CLI_IsObjectExecutable(pCmdKey, HAVEACTIONS) == TBS_SUCCESS )
       &&(pexpP->pObj == pcurP->pObj) )
    {
        SET_COMPLETE( *pulStatus );
        if(IS_LASTTOKEN(*pulStatus) && IS_SKIP_FLAG(*pulStatus))
        {
            SET_UNWANTED(*pulStatus);
        }
        return 2;
    }

    if( CLI_KEY_HAVE_SUBOBJECTS(pCmdKey) )
    {
        pexpP->pObj = pCmdKey->pSubCmdKey;
        pexpP->pParam = NULL;
        pexpP->pValue = NULL;
        SET_WAIT_KEY( *pulStatus );
        return TBS_SUCCESS;
    }

    SET_COMPLETE( *pulStatus );
    if(IS_LASTTOKEN(*pulStatus) && IS_SKIP_FLAG(*pulStatus))
    {
        SET_UNWANTED(*pulStatus);
    }

    /* return get element error */
    return TBS_FAILED;

}

/*********************************************************************/
/* 函数名称 : CLI_GetNextElement_P()                                  */
/* 函数功能 : 当命令字类型为参数的情况下，获取下一个期望的命令字类型 */
/* 输入参数 : pPointer:命令解释核心结构                              */
/*            pulStatus:命令解释状态                                 */
/* 输出参数 : pulStatus:命令解释状态                                 */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_GetExpectedElement                                  */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetNextElement_P( PST_CLI_POINTER pPointer, ULONG  * pulStatus )
{
    PST_CLI_CMDKEY pCmdKey;
    PST_CLI_PARAMETER pParam;
    PST_CLI_VALUE pValue;
    PST_CLI_INTP_POINTER pexpP;
    PST_CLI_INTP_POINTER pcurP;
    ULONG  ulMtIndex;

    pexpP = &(pPointer->expP);
    pcurP = &(pPointer->curP);
    pCmdKey = pcurP->pObj;
    pParam  = pcurP->pParam;

    if( (pCmdKey == NULL) || (pParam == NULL) )
    {
        return TBS_FAILED;
    }

    if( !( IS_SKIP_FLAG(*pulStatus)
           && CLI_PARAM_NEED_NAME(pParam) )
        && CLI_PARAM_NEED_VALUE(pParam)      )
    {
        pValue = pParam->valueList.pValue;

        pexpP->pParam = pParam;
        pexpP->pValue = pValue;
        SET_WAIT_VALUE( *pulStatus );
        return TBS_SUCCESS;
    }
    else if( !( IS_SKIP_FLAG(*pulStatus)
                && CLI_PARAM_NEED_NAME(pParam)   )
             && ( CLI_PARAM_NEED_SUB(pParam) ) )
    {
        pParam = pParam->subParam;

        pexpP->pParam = pParam;
        pexpP->pValue = NULL;
        SET_WAIT_PARAM( *pulStatus );
        return TBS_SUCCESS;
    }
    else if(CLI_PARAM_IS_MUTEX(pParam))
    {
        ulMtIndex = pParam->ulAbsoluteIndex;

        while((pParam) && CLI_PARAM_IS_MUTEX(pParam)
            && pParam->ulAbsoluteIndex == ulMtIndex)
        {
            pParam = pParam->pNext;
        }

        if(NULL != pParam)
        {
            pexpP->pParam = pParam;
            pexpP->pValue = NULL;
            SET_WAIT_PARAM( *pulStatus );
            return TBS_SUCCESS;
        }

    }

    else if( CLI_PARAM_YOUNGER_BROTHER(pParam) )

    {
        pParam = pParam->pNext;

        pexpP->pParam = pParam;
        pexpP->pValue = NULL;
        SET_WAIT_PARAM( *pulStatus );
        return TBS_SUCCESS;
    }

    pParam  = pcurP->pParam;
    if(NULL != pParam)
    {
        if( CLI_PARAM_PARENT(pParam) )
        {
            pexpP->pParam = pParam->pParent;
            pexpP->pValue = NULL;
            return (CLI_BackTopParam( pPointer, pulStatus ));
        }
        else
        {
            pexpP->pParam = NULL;
            pexpP->pValue = NULL;
            return (CLI_BackTopKey( pPointer, pulStatus ));
        }
    }
    else
    {
        pexpP->pParam = NULL;
        pexpP->pValue = NULL;
        return (CLI_BackTopKey( pPointer, pulStatus ));
    }

}

/**********************************************************************/
/* 函数名称 : CLI_GetNextElement_V()                                   */
/* 函数功能 : 当命令字类型为参数值的情况下，获取下一个期望的命令字类型*/
/* 输入参数 : pPointer:命令解释核心结构                               */
/*            pulStatus:命令解释状态                                  */
/* 输出参数 : pulStatus:命令解释状态                                  */
/* 返回     : 成功、失败                                              */
/* 上层函数 : CLI_GetExpectedElement                                   */
/* 创建者   :                                                         */
/* 修改记录 :                                                         */
/**********************************************************************/
ULONG  CLI_GetNextElement_V( PST_CLI_POINTER pPointer, ULONG  * pulStatus )
{
    PST_CLI_CMDKEY pCmdKey;
    PST_CLI_PARAMETER pParam;
    PST_CLI_VALUE pValue;
    PST_CLI_INTP_POINTER pexpP;
    PST_CLI_INTP_POINTER pcurP;

    pexpP = &(pPointer->expP);
    pcurP = &(pPointer->curP);
    pCmdKey = pcurP->pObj;
    pParam  = pcurP->pParam;
    pValue  = pcurP->pValue;

    if( (pCmdKey == NULL) || (pParam == NULL) || (pValue == NULL) )
    {
        return TBS_FAILED;
    }

    if( (!IS_SKIP_FLAG(*pulStatus) || CLI_VALUE_IS_OPTIONAL(pValue) || CLI_VALUE_IS_MUTEX(pValue))
        && CLI_VALUE_YOUNGER_BROTHER(pValue)
        && !IS_MUTEXVALUE(*pulStatus))
    {
        pValue = pValue->pNext;

        pexpP->pValue = pValue;
        SET_WAIT_VALUE( *pulStatus );
        return TBS_SUCCESS;
    }
    else
    {
        return (CLI_BackTopVal( pPointer, pulStatus ));
    }

}

/*********************************************************************/
/* 函数名称 : CLI_GetExpectedElement()                                */
/* 函数功能 : 获取下一个期望的命令字类型                             */
/* 输入参数 : pPointer:命令解释核心结构                              */
/*            pulStatus:命令解释状态                                 */
/* 输出参数 : pulStatus:命令解释状态                                 */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetExpectedElement( PST_CLI_POINTER pPointer, ULONG  * pulStatus )
{
    ULONG  ulRet;
    ULONG  ulOldStatus;

    ulOldStatus = *(pulStatus);

    if( IS_WAIT_NULL(ulOldStatus) )
    {
        pPointer->curP.pParam = NULL;
        pPointer->curP.pValue = NULL;
        COPY_POINTER_TO_EXP( pPointer );
        SET_WAIT_KEY(*(pulStatus));
        ulRet = TBS_SUCCESS;
    }
    else if(IS_WAIT_KEY(ulOldStatus))
    {
        ulRet = CLI_GetNextElement_O( pPointer, pulStatus );
    }
    else if(IS_WAIT_PARAM(ulOldStatus))
    {
        ulRet = CLI_GetNextElement_P( pPointer, pulStatus );
    }
    else if(IS_WAIT_VALUE(ulOldStatus))
    {
        ulRet = CLI_GetNextElement_V( pPointer, pulStatus );
    }
    else
    {
        ulRet = 1;
    }

    return ulRet;
}

/*※※※※※※※※※※※※※※*/
/* 【5】命令的执行            */
/*※※※※※※※※※※※※※※*/
/*********************************************************************/
/* 函数名称 : CLI_TakeAction()                                       */
/* 函数功能 : 执行已经解释成功的命令                                 */
/* 输入参数 : pCmdKey: 可执行的命令对象                              */
/*            ulStatus: 命令解释状态                                 */
/*            pWS: 命令解释获得的环境变量                            */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_IntpKernelProc                                     */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_TakeAction(PST_CLI_CMDKEY pCmdKey, ULONG  ulStatus)
{
    PFUN_CLI_CALLBACK_EXEC pFunc;

    /* no form command */
    if( IS_NOFORM(ulStatus) )
    {
        pFunc = pCmdKey->noAction.pFunction;
        if(pFunc == NULL)
        {
            IO_Print(CLI_INTP_CMD_NOEXEC);
            return TBS_FAILED;
        }
        else
        {
            return ( pFunc() );
        }
    }
    /* normal form command */
    else
    {
        if(CLI_KEY_CHANGE_MODE(pCmdKey))
        {
            /* While changing mode, we check the pFunc first */
            pFunc = pCmdKey->action.pFunction;
            if(pFunc != NULL)
            {
                if(pFunc() != TBS_SUCCESS)
                {
                    return TBS_FAILED;
                }
            }

            if( CLI_ConvertMode(pCmdKey) != TBS_SUCCESS )
            {
                return TBS_FAILED;
            }
            else
            {
                return TBS_SUCCESS;
            }
        }
        else
        {
            pFunc = pCmdKey->action.pFunction;
            if(pFunc == NULL)
            {
                IO_Print(CLI_INTP_CMD_NOEXEC);
                return TBS_FAILED;
            }
            else
            {
                return ( pFunc() );
            }
        }
    } /* end of if-else block */

}

/*※※※※※※※※※※※※※※*/
/* 【5】模式转换的处理        */
/*※※※※※※※※※※※※※※*/
/*********************************************************************/
/* 函数名称 : CLI_GetNewMode()                                        */
/* 函数功能 : 根据对象获取所要进入的新模式                           */
/* 输入参数 : pCmdKey:转换对象                                       */
/* 输出参数 : 无                                                     */
/* 返回     : 模式ID                                                 */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetNewMode(PST_CLI_CMDKEY pCmdKey)
{
    ULONG  i;
    for(i = 0; i < MAX_MODE_NUM; i++)
    {
        if(m_sCliSysModes[i].pCmdKey == pCmdKey)
        {
            return (m_sCliSysModes[i].ulMode);
        }
    }
    return NULL_MODE;

}

/*********************************************************************/
/* 函数名称 : CLI_GetExitMode()                                       */
/* 函数功能 : 获取回退的模式(父模式)                                 */
/* 输入参数 : ulMode:当前模式                                        */
/* 输出参数 : 无                                                     */
/* 返回     : 模式ID                                                 */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetExitMode(ULONG  ulMode)
{
    return (m_sCliSysModes[ulMode].ulParent);
}


/* 函数名称 : CLI_GetEndMode()                                           */
/* 函数功能 : 获取特权模式ID                                         */
 ULONG  CLI_GetEndMode(_VOID   )
{
     return END_MODE;
}


/*********************************************************************/
/* 函数名称 : CLI_GetModeTotalParams()                                */
/* 函数功能 : 获取模式的参数个数                                     */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetModeTotalParams(ULONG  ulMode)
{
    ULONG  Count;
    ULONG  ulCurMode;

    if(ulMode < MAX_MODE_NUM)
    {
        if(m_sCliSysModes[ulMode].ulMode == ulMode)
        {
            Count = 0;
            ulCurMode = ulMode;

            while(ulCurMode != NULL_MODE)
            {
                Count += m_sCliSysModes[ulCurMode].ulParamCount;
                ulCurMode = m_sCliSysModes[ulCurMode].ulParent;
            }

            return Count;
        }
    }

    return ((ULONG )-1);

}

/*********************************************************************/
/* 函数名称 : CLI_CmdPush()                                           */
/* 函数功能 : 进入新模式的模式压栈                                   */
/* 输入参数 : pStk: 模式栈                                           */
/* 输出参数 :                                                        */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_CmdPush(CLI_CMD_STACK_P pStk, ULONG  ulMode)
{
    if( pStk->ucTop == (CLI_MAX_MODE_STACK - 1) )
    {
        DBG_Out( "\r\n CLI_CmdPush::模式栈濒临溢出");
        return TBS_FAILED;
    }
    else
    {
        pStk->ulMode[pStk->ucTop] = ulMode;
        pStk->ucTop++;
    }
    return TBS_SUCCESS;
}


/*********************************************************************/
/* 函数名称 : CLI_CmdPop()                                            */
/* 函数功能 : 退回到上一模式的模式出栈                               */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_CmdPop(CLI_CMD_STACK_P pStk, char  *s)
{
    if (pStk->ucTop == 0)
    {
        return TBS_FAILED;
    }

    if(pStk->ucTop != 0)
        (pStk->ucTop)--;

    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数名称 : CLI_ConvertMode()                                       */
/* 函数功能 : 模式转换执行函数                                       */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_ConvertMode( PST_CLI_CMDKEY pCmdKey)
{
    char  s[CLI_MAX_CMDBUF];
    ULONG  ulMode;
    char  *pParamNO;

    switch(pCmdKey->action.modeConv)
    {
        case CLI_MC_NEW    :
            ulMode = CLI_GetNewMode(pCmdKey);
            if( ulMode >= NULL_MODE )
            {
                IO_Print(CLI_NO_SUCH_MODE) ;
                return TBS_FAILED;
            }
            if(CLI_CmdPush(&(m_stCliWS.cmdStack), ulMode) != TBS_SUCCESS )
            {
                IO_Print(CLI_PUSH_CMD_FAILED);
                return TBS_FAILED;
            }
            m_stCliWS.ulCurrentMode = ulMode;
            m_stCliWS.ulParamIndex += m_sCliSysModes[ulMode].ulParamCount;

            pParamNO = CLI_GetModeParameter(ulMode, 2) ;
            CLI_EnterMode(ulMode, pParamNO) ;

            break;
        case CLI_MC_EXIT   :
            pParamNO = CLI_GetModeParameter(pCmdKey->action.ulMode, 2) ;
            if ( CLI_ExitMode(pCmdKey->action.ulMode, pParamNO) != TBS_SUCCESS )
            {
                IO_Print(CLI_MODE_DEL_FAILED);
                return TBS_FAILED;
            }

            if (pCmdKey->action.ulMode == CTM_DIAG)
            {
                if (m_stCliWS.cmdStack.ucTop == 1)
                    ulMode = ROOT_MODE;
                else
                    ulMode = m_stCliWS.cmdStack.ulMode[m_stCliWS.cmdStack.ucTop - 2];
            }
            else
            {
                ulMode = CLI_GetExitMode(pCmdKey->action.ulMode);
            }
            if( ulMode >= NULL_MODE )
            {
                IO_Print(CLI_NO_SUCH_MODE) ;
                return TBS_FAILED;
            }
            if( CLI_CmdPop(&(m_stCliWS.cmdStack), &s[0]) != TBS_SUCCESS )
            {
                IO_Print(CLI_POP_CMD_FAILED);
                return TBS_FAILED;
            }
            m_stCliWS.ulParamIndex -=
                m_sCliSysModes[m_stCliWS.ulCurrentMode].ulParamCount;
            m_stCliWS.ulCurrentMode = ulMode;
            CLI_ResetIndexTable();
            break;

        case CLI_MC_END    :
            pParamNO = CLI_GetModeParameter(pCmdKey->action.ulMode, 2) ;

            if ( CLI_ExitMode(pCmdKey->action.ulMode, pParamNO) != TBS_SUCCESS )
            {
                IO_Print(CLI_MODE_DEL_FAILED);
                return TBS_FAILED;
            }

            while(CLI_CmdPop(&(m_stCliWS.cmdStack), &s[0]) == TBS_SUCCESS)
            {
                ;
            }
            CLI_CmdPush(&(m_stCliWS.cmdStack), END_MODE);
            m_stCliWS.ulCurrentMode = CLI_GetEndMode();
            m_stCliWS.ulParamIndex =
                CLI_GetModeTotalParams(m_stCliWS.ulCurrentMode);
            CLI_ResetIndexTable();
            break;
        default :
            IO_Print(CLI_MODE_CONV_FAILED);
            return TBS_FAILED;
    }
    return TBS_SUCCESS;

}


ST_CLI_MODE *CLI_GetMode(ULONG  ulMode)
{
    if(ulMode < MAX_MODE_NUM)
    {
        if(m_sCliSysModes[ulMode].ulMode == ulMode)
        {
            return &(m_sCliSysModes[ulMode]);
        }
    }

    return NULL;

}

char  *CLI_GetModePrompt(ULONG  ulMode)
{
    if(ulMode < MAX_MODE_NUM)
    {
        if(m_sCliSysModes[ulMode].ulMode == ulMode)
        {
            return &(m_sCliSysModes[ulMode].szModePrompt[0]);
        }
    }

    return NULL;

}

/*※※※※※※※※※※※※※※*/
/* 【6】命令联想的处理        */
/*※※※※※※※※※※※※※※*/
_VOID    CLI_MatchMore(char  *szResult,char  *szNew)
{
    char  *szTmpResult, *szTmpNew;

    if((szResult == NULL) || (szNew == NULL))
    {
        return;
    }

    szTmpResult = szResult;
    szTmpNew = szNew;
    if(*szResult == 0)
    {
        if(STB_StrLen(szNew) < MAX_TOKEN_SIZE)
        {
            STB_StrCpy(szResult,szNew);
        }
        else
        {
            STB_StrNCpy(szResult,szNew,MAX_TOKEN_SIZE);
            szResult[MAX_TOKEN_SIZE] = 0 ;
        }
    }
    else
    {
        while(*szTmpResult==*szTmpNew)
        {
            szTmpResult++;
            szTmpNew++;
        }
        *szTmpResult = 0;
    }

    return;
}

char  *CLI_SetPatch(char  *szBase,char  *szResult)
{
    char  *szTmpBase,*szTmpResult;
    static char  szEmpty[2]="";

    if((szBase == NULL) || (szResult == NULL))
    {
        return szEmpty;
    }
    else
    {
        szTmpBase = szBase;
        szTmpResult = szResult;
        if(STB_StrLen(szTmpBase) > STB_StrLen(szTmpResult))
        {
            return szEmpty;
        }
        while(*szTmpBase)
        {
            if(toupper(*szTmpBase) == toupper(*szTmpResult))
            {
                szTmpResult++;
                szTmpBase++;
            }
            else
            {
                return szEmpty;
            }
        }
        return szTmpResult;
    }
}


/*********************************************************************/
/* 函数名称 : CLI_MatchKeywords()                                     */
/* 函数功能 : 关键字列表匹配                                         */
/*********************************************************************/
ULONG  CLI_MatchKeywords(char  *szToken,PST_CLI_ENUM pKeyword,PST_CLI_POINTER pPointer)
{
    PST_CLI_ENUM pKeyword1;
    ULONG  ulCount;
    char  szResult[MAX_TOKEN_SIZE + 1];

    szResult[0] = '\0';
    ulCount = 0;

    pKeyword1 = pKeyword;

    while(pKeyword != NULL)
    {
        if( STB_StriCmp(szToken, pKeyword->pK) == 0 )
        {
            pPointer->ucMatchFlag = CTMF_FULL;
            return TBS_SUCCESS;
        }

        if( strmatch(szToken, pKeyword->pK) == 0 )
        {
            ulCount ++;
            CLI_MatchMore(szResult,pKeyword->pK);
            pKeyword1 = pKeyword;
        }
        pKeyword = pKeyword->pNext;
    }
    if(ulCount < 1)
    {
        return TBS_FAILED;
    }
    else if(ulCount > 1)
    {
        STB_StrCpy( &(pPointer->szPatch[0]),CLI_SetPatch(szToken,&szResult[0]));
        pPointer->ucMatchFlag = CTMF_PARTIAL;
        return TBS_FAILED;
    }
    else
    {
        /* 此处pc-lint有告警,确认不会有问题 */
        STB_StrCpy( &(pPointer->szPatch[0]),
            (pKeyword1->pK + STB_StrLen(szToken)) );
        STB_StrCat(szToken, pPointer->szPatch);
        pPointer->ucMatchFlag = CTMF_FULL;
        return TBS_SUCCESS;
    }

}

/*※※※※※※※※※※※※※※*/
/* 【7】帮助显示的处理        */
/*※※※※※※※※※※※※※※*/
/***********************************************************************
*    函数名        : CLI_IfCommandHelp
*    函数功能      : 分析命令是否为类似"show ?"之类的命令
*                    即?前面有空格和命令字的命令
*    输入参数      : PTerm_Data_S pTermStruct : 任务数据指针
*    输出参数      : 无
*    返回值        : 0:成功
*                    1:失败
*    调用函数      :
*    被调用        :
***********************************************************************/
ULONG  CLI_IfCommandHelp()
{
    char  CommandBuf[CLI_MAX_CMDBUF] ;

    STB_StrCpy(CommandBuf, m_stCliWS.szCmdString);
    _btrim(CommandBuf);

    if (STB_StrLen(CommandBuf) < 3) return TBS_FAILED;
    if (strstr(CommandBuf , " ") && strstr(CommandBuf , "?")) return TBS_SUCCESS;
    else return TBS_FAILED;
}

/********************************************************************
*
*    函 数 名   : CLI_PrintHelpString
*    函数功能   : 拆分打印帮助信息
*    参数说明   : char  *szString  要显示的字符串
*    调用函数   : none
*********************************************************************/
ULONG  CLI_PrintHelpString(char  *szString)
{
    char  *tempStr,a[100];
    int posit=0;

    tempStr=a;

    for(;*szString!='\0';)
    {
        if(*szString=='\r')
        {
            *tempStr=*szString;
            szString++;
            tempStr++;
            posit++;
            if( (*szString=='\n' && *szString!='\0') || (posit>97) )
            {
                *tempStr=*szString;
                tempStr++;
                posit++;
                szString++;
                *tempStr = '\0';
                IO_OutString(a);
                tempStr=tempStr-posit;
                posit=0;
            }
        }
        else if(*szString=='\n')
        {
            *tempStr=*szString;
            szString++;
            tempStr++;
            posit++;
            if( (*szString=='\r'&& *szString!='\0') || (posit>97) )
            {
                *tempStr=*szString;
                tempStr++;
                posit++;
                szString++;
                *tempStr='\0';
                IO_OutString(a);
                tempStr=tempStr-posit;
                posit=0;
            }
        }
        else
        {
            *tempStr=*szString;
            szString++;
            tempStr++;
            posit++;
            if(posit>97)
            {
                *tempStr=*szString;
                tempStr++;
                posit++;
                szString++;
                *tempStr='\0';
                IO_OutString(a);
                tempStr=tempStr-posit;
                posit=0;
            }
        }
    }
    *tempStr='\0';
    IO_OutString(a);
    return TBS_SUCCESS;

}

/********************************************************************
*
*    函 数 名   : CLI_GetObjectHelpString
*    函数功能   : 从对象帮助信息提取其功能帮助信息
*    参数说明   : char  *szString     要提取帮助信息的字符串
*                 char  helpStr[128]  返回的帮助信息
*    调用函数   : none
*********************************************************************/
ULONG  CLI_GetObjectHelpString(PCSTR szString,char  helpStr[128])
{
    char  *tempStr;

    CLI_MemSet(helpStr, 0, 128);
    tempStr=helpStr;

    if(szString == NULL)
        return TBS_FAILED;

    if ((szString = strstr(szString, HELP_FUN)) == NULL)
    {
        return TBS_FAILED;
    }

    szString += STB_StrLen(HELP_FUN);

    for(;*szString!='\0';)
    {
        if(*szString=='\r' || *szString == '\n' )
        {
            break;
        }
        *tempStr=*szString;
        szString++;
        tempStr++;
    }
    *tempStr='\0';

    return TBS_SUCCESS;

}

/*********************************************************************/
/*    函 数 名   : CLI_ShowParamHelp                                  */
/*    函数功能   : 针对单个参数显示其帮助信息                        */
/*    参数说明   : pName     参数名称                                */
/*                 szCmdHelp 整条命令的帮助信息                      */
/*    调用函数   :                                                   */
/*********************************************************************/
ULONG  CLI_ShowParamHelp(char  *pName, const char  *szCmdHelp)
{
    char  szHelpInfo[2048];
    char  szParamHelp[1024] = ""; // 单个参数的最大长度
    char  *pCur = NULL, *pTmp = NULL, *pHelpStart;

    CLI_ASSURE_OR_FAIL(pName != NULL && szCmdHelp != NULL && *pName != '\0');

    STB_StrCpy(szHelpInfo, szCmdHelp);

    pTmp = &szHelpInfo[0];
    /* 从帮助信息中寻找与当前参数名相同的字串 */
    while ((pCur = strstr(pTmp, pName)) != NULL && pCur != pTmp)
    {
        if ((pCur > szHelpInfo) && *(pCur - 1) != ' ')
        {
            pTmp = pCur + 1;
            continue;
        }
        pHelpStart = pCur;
        pCur = pCur+ STB_StrLen(pName);
        while (*pCur == ' ')
            pCur ++;

        /* 如果该字串的下一个有效字符为参数说明分隔符，那么找的正是地方 */
        if (*pCur == ':')
        {
            STB_StrNCpy(szParamHelp, pHelpStart, 1024);
            szParamHelp[1023] = 0;
            pTmp = &szParamHelp[0];
            /* 找到下一处换行的地方，如果换行后缩进比参数缩进多，则说明 */
            /* 下一行仍为本参数说明，继续显示                           */
            while (((pTmp = strchr(pTmp, '\r')) != NULL))
            {
                if (*(pTmp + STB_StrLen(HELP_SPC)) != ' ')
                {
                    *pTmp = '\0';
                    break;
                }
                pTmp++;
            }
        }
        pTmp = pCur;
    }

    if (szParamHelp[0] != 0)
    {
        STB_Sprintf(szHelpInfo, "%s%s", HELP_SPC, szParamHelp);
        IO_OutString(szHelpInfo);
        return TBS_SUCCESS;
    }
    else
    {
        return TBS_FAILED;
    }

}

/*********************************************************************/
/*    函 数 名   : CLI_ShowCommandHelp()                              */
/*    函数功能   : 显示命令的帮助信息                                */
/*    输    入   :                                                   */
/*           pCmdKey: 可执行的命令对象                               */
/*           ulLevel: 当前用户级别                                   */
/*           ulStatus: 命令解析状态，记录是否为no命令                */
/*    输    出   : TBS_SUCCESS: 设置成功;  TBS_FAILED: 设置失败             */
/*    调用函数   :                                                   */
/*********************************************************************/
ULONG  CLI_ShowCommandHelp( PST_CLI_CMDKEY pCmdKey, PST_CLI_POINTER pPointer, ULONG  ulStatus)
{
    char  *pName = NULL;
    PST_CLI_PARAMETER pParam = NULL;
    PST_CLI_VALUE  pValue = NULL;

    if (pCmdKey == NULL)
    {
        return TBS_FAILED;
    }

    /*用户级别检验*/
    if (!CLI_IS_CURRENT_KNOWN_KEY(pCmdKey))
    {
        return TBS_FAILED;
    }

    /*no命令检验*/
    if(  (IS_NOFORM(ulStatus))
       &&((pCmdKey->ulNoFlag == 0)||(pCmdKey->noAction.pFunction == NULL)))
    {
        return TBS_FAILED;
    }

    /*不可执行的对象*/
    if (TBS_SUCCESS != CLI_IsObjectExecutable(pCmdKey, HAVEACTIONS))
    {
        return TBS_FAILED;
    }

    /*取帮助信息*/
    if (pCmdKey->action.strObjHelp == CLI_ML_NULL)
    {
        IO_Print(CLI_CMD_NO_HELPINFO);
    }
    else
    {
        if (IS_INTERACTIVE(*pPointer->ulStatus))
        {
            if ( pPointer->expP.pValue != NULL)
            {
                pValue = pPointer->expP.pValue;
                pName = pValue->szName;
            }
            else if (pPointer->expP.pParam != NULL)
            {
                pParam = pPointer->expP.pParam;
                pName = pParam->szName;
            }
        }
        else
        {
            IO_Print(pCmdKey->action.strObjHelp);
            return TBS_SUCCESS;
        }

        IO_OutString("\r\n");
        if (TBS_SUCCESS != CLI_ShowParamHelp(pName, pCmdKey->action.strObjHelp))
        {
            IO_Print(pCmdKey->action.strObjHelp);
            return TBS_SUCCESS;
        }
        if (pParam != NULL && CLI_PARAM_IS_MUTEX(pParam))
        {
            while ((pParam->pNext != NULL) && CLI_PARAM_IS_MUTEX(pParam))
            {
                if (pParam->pNext->ulAbsoluteIndex != pParam->ulAbsoluteIndex)
                    break;
                pParam = pParam->pNext;
                CLI_ShowParamHelp(pParam->szName, pCmdKey->action.strObjHelp);
            }
        }
    }

    return TBS_SUCCESS;
}

/*********************************************************************/
/*    函 数 名   : CLI_GetSubObjectsNumber()                          */
/*    函数功能   : 获取符合条件的子对象个数                          */
/*    输    入   :                                                   */
/*           pCmdKey: 父命令对象                                     */
/*           ulLevel: 当前用户级别                                   */
/*           ulStatus: 命令解析状态，记录是否为no命令                */
/*    输    出   : 子对象个数                                        */
/*    调用函数   :                                                   */
/*********************************************************************/
ULONG  CLI_GetSubObjectsNumber(PST_CLI_CMDKEY pCmdKey, PST_CLI_POINTER pPointer, ULONG  ulStatus)
{
    ULONG            dwResult = 0;
    PST_CLI_CMDKEY     pSubCmdKey;

    pSubCmdKey = pCmdKey->pSubCmdKey;
    while(pSubCmdKey != NULL)
    {
        if(  (CLI_IS_CURRENT_KNOWN_KEY(pSubCmdKey))
           &&(  (IS_NOFORM(ulStatus)&&(pSubCmdKey->ulNoFlag == 1))
              ||(!IS_NOFORM(ulStatus))
             )
          )
        {
            if(  (TBS_SUCCESS == CLI_IsObjectExecutable(pSubCmdKey,HAVEACTIONS))
                ||(  (TBS_SUCCESS != CLI_IsObjectExecutable(pSubCmdKey,HAVEACTIONS))
                &&( 0 < CLI_GetSubObjectsNumber(pSubCmdKey,pPointer, ulStatus))
                ))
            {
                dwResult ++;
            }
        }
        pSubCmdKey = pSubCmdKey->pNext;
    }

    return dwResult;

}

/*********************************************************************/
/*    函 数 名   : CLI_ShowObjectHelp()                               */
/*    函数功能   : 显示对象的帮助信息;                               */
/*                 可执行的对象显示命令功能描述                      */
/*                 有子对象的对象显示符合条件子对象的个数            */
/*    输    入   :                                                   */
/*           pCmdKey : 当前命令对象                                  */
/*           ulLevel : 当前用户级别                                  */
/*           ulStatus: 命令解析状态，记录是否为no命令                */
/*           pszToken: 当前的输入段，即子对象的前缀                  */
/*    输    出   : TBS_SUCCESS: 设置成功;  TBS_FAILED: 设置失败         */
/*    调用函数   :                                                   */
/*********************************************************************/
ULONG  CLI_ShowObjectHelp( PST_CLI_CMDKEY pCmdKey, PST_CLI_POINTER pPointer, ULONG  ulStatus,
    char  *pszToken)
{
    char             cSave;
	PCSTR		     szSave;
    char             szHelp[200], szTemp[200];
    ULONG            dwObjectNumber = 0;
    ULONG            dwLoop, dwResult = TBS_FAILED;
    char             szObjName[200], szObjectName[21]; /* 名称长度为:18(对象名) + 2(对齐空格) + 1(结束符)*/
    UCHAR            ucCount = 0;

    if (pCmdKey == NULL)
    {
        return TBS_FAILED;
    }

    if (  (*pszToken != 0)
        &&(TBS_SUCCESS != strmatch(pszToken, pCmdKey->szName)))
    {
        return TBS_FAILED;
    }

    /*用户级别检验*/
    if (!CLI_IS_CURRENT_KNOWN_KEY(pCmdKey))
    {
        return TBS_FAILED;
    }

    /*no命令检验*/
    if(IS_NOFORM(ulStatus)&&(pCmdKey->ulNoFlag == 0))
    {
        return TBS_FAILED;
    }

    STB_StrCpy(szObjName, pCmdKey->szName) ;
    if (STB_StrLen(szObjName) > 16)
    {
        szTemp[0] = 0 ;
        while(STB_StrLen(szObjName) > 16)
        {
            szSave = szObjName + 16;
            cSave = szObjName[16];
            szObjName[16] = '\0';
            STB_StrCat(szTemp, "\r\n  ");
            STB_StrCat(szTemp,szObjName);
            szObjName[16] = cSave;
            STB_StrCpy(&szObjName[0], "  ");
            STB_StrCat(szObjName, szSave);
            if (STB_StrLen(szTemp)+18 >= 200)
                break ;
        }

        IO_OutString(szTemp);
    }
/*  STB_Sprintf(szObjectName, "\r\n  %-16s", szObjName);   --原来的实现代码，被以下代码替换*/
/***************************************************************************/
	/*
	 * 判断当前对象的父对象是否是当前命令模式下或全局模式下的第一个对象
	 */
	if(pCmdKey->pParent == m_sCliSysModes[m_stCliWS.ulCurrentMode].pCmdKey
		|| STB_StrCmp(pCmdKey->pParent->szName, "global") == 0)
	{
		/*在满足if的条件下，如果命令对象有no形式，在其帮助信息前显示[no]*/
		if(pCmdKey->ulNoFlag == 1)
		{
			STB_Sprintf(szObjectName, "\r\n  [no] %-16s", szObjName);
		}
		else
		{
			STB_Sprintf(szObjectName, "\r\n  %-16s", szObjName);
		}
	}
	else
	{
		STB_Sprintf(szObjectName, "\r\n  %-16s", szObjName);
	}
/***************************************************************************/
    szObjectName[20] = '\0';

    if(TBS_SUCCESS == CLI_IsObjectExecutable(pCmdKey,HAVEACTIONS))
    {
        /*对象可执行，显示命令的功能描述(只有一行，以"\r\n"区分)*/
        /*取帮助信息*/
        szSave = pCmdKey->action.strObjHelp;

        dwResult = CLI_GetObjectHelpString(szSave,szTemp);
        if (STB_StrLen(szTemp) >= 60)
        {
            for (szSave = szTemp; szSave < szTemp + 56; szSave++)
            {
                if (*szSave < 0)
                {
                    ucCount ++;
                }
            }
            if (ucCount % 2 == 0)
            {
                STB_StrCpy(szTemp + 56, "...");
            }
            else
            {
                STB_StrCpy(szTemp + 57, "...");
            }
        }
        if (dwResult != TBS_SUCCESS)
        {
            STB_Sprintf(szHelp, "%-18s  %s", szObjectName, CLI_CMD_NO_HELPINFO);
        }
        else
        {
            STB_Sprintf(szHelp, "%-18s  %s", szObjectName, szTemp);
        }
    }
    else
    {
        /*对象不可执行*/

        if (pCmdKey->pSubCmdKey)
        {
            /*有子对象，显示子对象个数*/

            /*获取子对象个数，满足用户级别和no命令要求*/
            dwObjectNumber = CLI_GetSubObjectsNumber(pCmdKey, pPointer, ulStatus);
            if (0 == dwObjectNumber)
            {
                return TBS_FAILED;
            }

            _btrim(szObjectName);

            /*以空格补齐长度20*/
            for(dwLoop = STB_StrLen(szObjectName); dwLoop < 20; dwLoop ++)
            {
                szObjectName[dwLoop] = ' ';
            }
            szObjectName[20] = '\0';
        }

         /*对象不可执行，显示对象帮助信息*/
        /*中文模式*/
        if (CLI_ML_NULL == pCmdKey->action.strObjHelp)
        {
            if (dwObjectNumber >= 1)
            {
                STB_Sprintf(szHelp, "%-18s  [%d] %s", szObjectName, dwObjectNumber,
                    CLI_CMD_NO_HELPINFO);
            }
            else
            {
                STB_Sprintf(szHelp, "%-18s  %s", szObjectName,
                    CLI_CMD_NO_HELPINFO);
            }
        }
        else
        {
            if (dwObjectNumber >= 1)
            {
                STB_Sprintf(szHelp, "%-18s  [%d] %s", szObjectName, dwObjectNumber,
                    pCmdKey->action.strObjHelp);
            }
            else
            {
                STB_Sprintf(szHelp, "%-18s  %s", szObjectName,
                    pCmdKey->action.strObjHelp);
            }
        }
    }

    dwResult = IO_OutString(szHelp);

    return dwResult;
}

/*********************************************************************/
/*    函 数 名   : CLI_ShowHelp()                                    */
/*    函数功能   : 根据当前命令行输入，显示帮助信息                  */
/*    输    入   :                                                   */
/*           pCmdKey : 当前命令对象                                  */
/*           ulLevel : 当前用户级别                                  */
/*           ulStatus: 命令解析状态，记录是否为no命令                */
/*           pszToken: 当前的输入段，即子对象的前缀                  */
/*    输    出   : TBS_SUCCESS: 设置成功;  TBS_FAILED: 设置失败      */
/*    调用函数   :                                                   */
/*********************************************************************/
ULONG  CLI_ShowHelp( PST_CLI_POINTER pPointer, ULONG  ulCurMode, ULONG  ulStatus)
{
    PST_CLI_CMDKEY     pSubCmdKey;
    char             szToken[MAX_TOKEN_SIZE+1];
    ULONG            dwLoop = 0;

    if(pPointer == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    if(ulCurMode >= MAX_MODE_NUM)
    {
        return TBS_FAILED;
    }

    /*取掉'?'*/
    if (STB_StrLen(pPointer->szToken) > MAX_TOKEN_SIZE)
    {
        *(pPointer->szToken + MAX_TOKEN_SIZE) = '\0';
    }
    STB_StrCpy(szToken, pPointer->szToken);
    szToken[STB_StrLen(szToken)] = '\0';
    for (dwLoop = 0; dwLoop < STB_StrLen(szToken); dwLoop ++)
    {
        if (szToken[dwLoop] == '?')
        {
            szToken[dwLoop] = '\0';
            break;
        }
    }

    /*--------------------------------------------------*/
    /* 显示命令帮助信息                                 */
    /*--------------------------------------------------*/
    if((pPointer->curP.pObj != m_sCliSysModes[ulCurMode].pCmdKey)
      &&(TBS_SUCCESS == CLI_IsObjectExecutable(pPointer->curP.pObj,HAVEACTIONS) ))
    {
        return CLI_ShowCommandHelp(pPointer->curP.pObj, pPointer, ulStatus);
    }

    /*打印表头*/
    if(m_sCliSysModes[ulCurMode].ulMode == ulCurMode)
    {
        if(m_sCliSysModes[ulCurMode].pCmdKey == pPointer->curP.pObj)
        {
            IO_Print(CLI_HELPINFO_HEAD);
        }
        else if(CLI_KEY_HAVE_SUBOBJECTS(pPointer->curP.pObj))
        {
            IO_Print(CLI_HELPINFO_NEXT);
        }
        IO_Print(PUB_TABLE_EMBODY);
    }
    else
    {
        return TBS_FAILED;
    }

    /********************************************************/
    /*显示所有子对象的帮助信息                              */
    /********************************************************/
    dwLoop = 0;
    pSubCmdKey = (pPointer->curP.pObj)->pSubCmdKey;
    while(pSubCmdKey != NULL)
    {
        if(strcmp(pSubCmdKey->szName, "sh") != 0 && strcmp(pSubCmdKey->szName, "shell") != 0)
        {        
            if (pSubCmdKey->action.ulMode != CTM_GLOBAL)
            {
                if (!CLI_ShowObjectHelp(pSubCmdKey, pPointer, ulStatus, szToken))
                    dwLoop++;
            }
        }
        pSubCmdKey = pSubCmdKey->pNext;
    }
    if (dwLoop > 0)
        IO_Print(PUB_TABLE_EMBODY);

    dwLoop = 0;
    pSubCmdKey = (pPointer->curP.pObj)->pSubCmdKey;
    while(pSubCmdKey != NULL)
    {
        if(strcmp(pSubCmdKey->szName, "sh") != 0 && strcmp(pSubCmdKey->szName, "shell") != 0)
        {
            if (pSubCmdKey->action.ulMode == CTM_GLOBAL)
            {
                if (!CLI_ShowObjectHelp(pSubCmdKey, pPointer, ulStatus, szToken))
                    dwLoop++;
            }
        }
        pSubCmdKey = pSubCmdKey->pNext;
    }
    if (dwLoop > 0)
        IO_Print(PUB_TABLE_EMBODY);

    return TBS_SUCCESS;
}

ULONG  CLI_GetString(char  *szString, ULONG  ulLength)
{
    if (IO_GetString(szString, CLI_MAX_CMDLEN, FALSE) == TBS_SUCCESS)
    {
        if (szString[0] == '?' || szString[0] == '\0' || szString[0] == KEY_CTRL_C)
        {
            return TBS_SUCCESS;
        }
        return CLI_INTP_RET_NEWLINE;
    }
    szString[0] = '\0';
    return TBS_SUCCESS;

}

ULONG  CLI_BrotherParameterOptional(PST_CLI_PARAMETER *ppParam)
{
    while(*ppParam != NULL)
    {
        if( !CLI_PARAM_IS_OPTIONAL(*ppParam) && ((*ppParam)->stParamType.ucAttrib != CLI_PRMPROP_NULL))
            return TBS_FAILED;

        *ppParam = (*ppParam)->pNext;
    }

    return TBS_SUCCESS;
}
ULONG  CLI_BrotherValueOptional(PST_CLI_VALUE *ppValue)
{
    while(*ppValue != NULL)
    {
        if( !CLI_VALUE_IS_OPTIONAL(*ppValue) )
            return TBS_FAILED;

        *ppValue = (*ppValue)->pNext;
    }

    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数名称 : CLI_SetCommandString()                                  */
/* 函数功能 : 设置命令缓冲中的命令串                                 */
 /*********************************************************************/
ULONG  CLI_SetCommandString( char  *szCmd)
{
    if(szCmd != NULL)
    {
        if(STB_StrLen(szCmd) < CLI_MAX_CMDBUF && STB_StrLen(szCmd) > 0)
        {
            STB_StrCpy(m_stCliWS.szCmdString,szCmd);
            return TBS_SUCCESS;
        }
    }

    return TBS_FAILED;
}

/*********************************************************************/
/* 函数名称 : CLI_GetCommandPatch()                                   */
/* 函数功能 : 获取命令解释所得的补丁字符串                           */
/*********************************************************************/
char  *CLI_GetCommandPatch(ULONG  ulCmdLen)
{
    return ( &(m_stCliWS.szCmdString[0]) + ulCmdLen );
}


/*********************************************************************/
/* 函数名称 : CLI_SaveParameter()                                     */
/* 函数功能 : 参数(值)的保存                                         */
/* 输入参数 : szToken :输入的实际参数                                */
/*            szName  :参数对应的参数名称                            */
/*            ulIndex :参数在缓冲中的存储索引                        */
/* 输出参数 : pWS     :解释状态结构                                  */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_SaveParameter( char  *szToken, char  *szName, ULONG  ulIndex)
{
    /* Check all the pointers */
    if(szToken == NULL)
    {
        return TBS_FAILED;
    }

    if((ulIndex == 0) || (ulIndex >= CLI_MAX_PARAM_NUM))
    {
        return 2;   /* 留作以后扩展接口用，以区分错误种类 */
    }

    if(STB_StrLen(szToken) >= CLI_MAX_PARAMLEN)
    {
        return 3;   /* 留作以后扩展接口用，以区分错误种类 */
    }

    STB_StrCpy(&(m_stCliWS.aszParamBuff[ulIndex][0]),szToken);
    STB_StrCpy(&(m_stCliWS.aszNameBuff[ulIndex][0]),szName);

    m_stCliWS.aulIndexTable[ulIndex] = (ULONG )(&(m_stCliWS.aszParamBuff[ulIndex][0]));
    m_stCliWS.ulValidParams ++;

    return TBS_SUCCESS;

}

/*********************************************************************/
/* 函数名称 : CLI_GetCmdWord()                                        */
/* 函数功能 : 从命令中的输入位置直接获取参数                         */
/* 输入参数 : ulIndex: 输入位置                                      */
/* 输出参数 : Outp   : 获取的参数                                    */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetCmdWord(ULONG  ulIndex, char  *Outp)
{
    ULONG  ulTmp = 0;
    char  *szCmdString;
    char  *pSave;


    if(NULL == Outp)
    {
        return TBS_FAILED;
    }

    pSave = Outp;
    szCmdString = m_stCliWS.szCmdString;
    _btrim(szCmdString);
    while(*szCmdString)
    {
        if( (*szCmdString == 0) || (ulTmp + 1 == ulIndex) )
            break;
        if(*szCmdString == ' ')
        {
            while(*szCmdString == ' ')
                szCmdString ++;
            ulTmp ++;
        }
        else
            szCmdString ++;
    }

    if(ulTmp + 1 == ulIndex)
    {
        while((*szCmdString) && (*szCmdString != ' '))
        {
            *Outp = *szCmdString;
            Outp ++;
            szCmdString ++;
        }
        *Outp = '\0';
        Outp = pSave;
        return TBS_SUCCESS;
    }
    return TBS_FAILED;
}

/*********************************************************************/
/* 函数名称 : CLI_GetCurrentMode()                                    */
/* 函数功能 : 当前模式                                               */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetCurrentMode(_VOID   )
{
    return m_stCliWS.ulCurrentMode;
}

/*********************************************************************/
/* 函数名称 : CLI_GetParameter()                                      */
/* 函数功能 : 根据存储索引获取参数                                   */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
char  * CLI_GetParameter( ULONG  ulIndex)
{
    if ( (m_stCliWS.ulParamIndex + ulIndex) >= CLI_MAX_PARAM_NUM )
    {
       return NULL;
    }
    return ((char *)m_stCliWS.aulIndexTable[m_stCliWS.ulParamIndex + ulIndex]);
}

/*----------------------------------------------------------------------*/
/* 函数名称 : CLI_GetParamByName                                         */
/* 功    能 : 通过参数名获取参数                                        */
/* 输入参数 : szName: 参数名                                            */
/*            ulTermID: 当前的终端ID                                    */
/* 输出参数 : 无                                                        */
/* 返回     : 获取的参数字符串指针                                      */
/* 调用说明 : 调用者应对返回值进行判断. 详细说明可参见命令行说明文档.   */
/* 典型示例 : char  *pParam;                                              */
/*            1) pParam = CLI_GetParamByName("userName/2",  ulTermID));  */
/*            2) pParam = CLI_GetParamByName("xmodem|tftp", ulTermID));  */
/*            3) if ((pParam = CLI_GetParamByName("ip-mask/2", ulTermID))*/
/*                  == NULL)                                          */
/*                   return TBS_FAILED;                                  */
/*----------------------------------------------------------------------*/
char  * CLI_GetParamByName(const char  *szName)
{
    ULONG  ulIndex = 0, ulPos ;
    char  szValName[MAX_TOKEN_SIZE];
    char  *pcPos;
    ULONG   ulFlag = 0;
    CLI_ASSURE_OR_NULL(szName != NULL && *szName != '\0');
    STB_StrCpy(szValName, szName);
    _AffirmStrBeLower(szValName);


    if (strchr (szValName, '|') != NULL)
    {
        ulFlag = 1;
    }
    else if ((pcPos = strchr(szValName, '/')) != NULL)
    {
        if (STB_atoul(pcPos + 1, &ulFlag)!= 0 || ulFlag > 6)
            return NULL;
        *pcPos = '\0';
    }
    else
    {
        ulFlag = 0;
    }

    ulPos = 1;

    for (ulIndex = m_stCliWS.ulParamIndex + 1; ulIndex < CLI_MAX_PARAM_NUM; ulIndex++)
    {
        if (ulFlag == 0)
        {
            if (STB_StriCmp(szValName, m_stCliWS.aszNameBuff[ulIndex] )== 0)
            {
                break;
            }
        }
        else if (ulFlag == 1)
        {
            if ((pcPos = strstr(szValName, m_stCliWS.aszNameBuff[ulIndex])) != NULL)
            {
                /* 如果该参数名的前后都不是字母，认为获取正确*/
                if (m_stCliWS.aszNameBuff[ulIndex][0] == '\0')
                    continue;
                if ((pcPos == szValName || _IsCmdChar(*(pcPos - 1)) != 0)
                 && ((ULONG )_IsCmdChar(*(pcPos + STB_StrLen(m_stCliWS.aszNameBuff[ulIndex]))) != TBS_SUCCESS))
                    break;
            }
        }
        else
        {
            if (STB_StriCmp(szValName, m_stCliWS.aszNameBuff[ulIndex] ) == 0)
            {
                if (ulPos == ulFlag)
                    break;
                else
                    ulPos ++;
            }
        }
    }
    if (ulIndex >= CLI_MAX_PARAM_NUM)
        return NULL;
    else
        return m_stCliWS.aszParamBuff[ulIndex];
}

/*-------------------------------------------------------------------*/
/* 函数名称 : CLI_GetLongByName                                       */
/* 功    能 : 通过参数名获取长整型参数                               */
/* 输入参数 : szName: 参数名                                         */
/*            ulTermID: 当前的终端ID                                 */
/* 输出参数 : lResult 获取结果                                       */
/* 返回     : 获取成功/失败                                          */
/* 调用说明 : 调用者应对返回值进行判断. 详细说明可参见命令行说明文档.*/
/* 典型示例 : CLI_GetLongByName("time", ulTermID, &Time);             */
/*-------------------------------------------------------------------*/
ULONG  CLI_GetLongByName(const char  *szName, int    *lResult)
{
    char  *pParam;

    pParam = CLI_GetParamByName(szName);
    if (pParam == NULL)
        return TBS_FAILED;

    return STB_atol(pParam, lResult);
}

/*-------------------------------------------------------------------*/
/* 函数名称 : CLI_GetLongByName                                       */
/* 功    能 : 通过参数名获取长整型参数                               */
/* 输入参数 : szName: 参数名                                         */
/*            ulTermID: 当前的终端ID                                 */
/* 输出参数 : lResult 获取结果                                       */
/* 返回     : 获取成功/失败                                          */
/* 调用说明 : 调用者应对返回值进行判断. 详细说明可参见命令行说明文档.*/
/* 典型示例 : CLI_GetLongByName("time", ulTermID, &Time);             */
/*-------------------------------------------------------------------*/
ULONG  CLI_GetUlongByName(const char  * szName, ULONG  * ulResult)
{
    char  *pParam;

    pParam = CLI_GetParamByName(szName);
    if (pParam == NULL)
        return TBS_FAILED;

    return STB_atoul(pParam, ulResult);
}

/*-------------------------------------------------------------------*/
/* 函数名称 : CLI_GetHexByName                                        */
/* 功    能 : 通过参数名获取十六进制参数                             */
/* 输入参数 : szName: 参数名                                         */
/*            ulTermID: 当前的终端ID                                 */
/* 输出参数 : lResult 获取结果                                       */
/* 返回     : 获取成功/失败                                          */
/* 调用说明 : 调用者应对返回值进行判断. 详细说明可参见命令行说明文档.*/
/* 典型示例 : CLI_GetLongByName("time", ulTermID, &Time);             */
/*-------------------------------------------------------------------*/
ULONG  CLI_GetHexByName(const char  *szName, ULONG  *lResult)
{
    char  *pParam;

    pParam = CLI_GetParamByName(szName);
    if (pParam == NULL)
        return TBS_FAILED;

    return STB_atoulx(pParam, lResult);
}

/*********************************************************************/
/* 函数名称 : CLI_GetModeParameter()                                 */
/* 函数功能 : 获取模式参数                                           */
/* 输入参数 : ulMode: 模式名                                         */
/*            ulIndex: 存储索引                                      */
/* 输出参数 :                                                        */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
char  * CLI_GetModeParameter( ULONG  ulMode, ULONG  ulIndex)
{
    ST_CLI_MODE *pMode;
    ULONG  mode;
    ULONG  ulModeIndex;   /* Start ulIndex of the mode in Param Buffer */

    if(ulMode < CTM_TOTAL)
    {
        mode = m_stCliWS.ulCurrentMode;
        ulModeIndex = m_stCliWS.ulParamIndex;
        while(mode != NULL_MODE)
        {
            pMode = CLI_GetMode(mode);
            if(NULL == pMode)
                return NULL;
            ulModeIndex -= pMode->ulParamCount;
            if(mode == ulMode)
            {
                if(ulIndex > pMode->ulParamCount)
                {
                    return NULL;
                }
                else
                {
                    if ( (ulModeIndex+ulIndex) >= CLI_MAX_PARAM_NUM )
                    {
                        return NULL;
                    }
                    return((char *)(m_stCliWS.aulIndexTable[ulModeIndex+ulIndex]));
                }
            }
            else
            {
                mode = pMode->ulParent;
            }
        } /* end of while() */
    }

    return NULL;

}

/*********************************************************************/
/* 函数名称 : CLI_GetCurrentPrompt()                                  */
/* 函数功能 : 获取当前的提示符串                                     */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
char  *CLI_GetCurrentPrompt()
{
    return CLI_GetModePrompt(m_stCliWS.ulCurrentMode);
}


/*********************************************************************/
/* 函数名称 : CLI_ResetIndexTable()                                   */
/* 函数功能 : 清除参数存储缓冲中的内容                               */
/*********************************************************************/
ULONG  CLI_ResetIndexTable()
{
    ULONG  i;

    for(i = (m_stCliWS.ulParamIndex+1); i < CLI_MAX_PARAM_NUM; i++)
    {
        m_stCliWS.aulIndexTable[i] = 0;
        m_stCliWS.aszParamBuff[i][0]  = 0;
        m_stCliWS.aszNameBuff[i][0] = 0;
    }

    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数名称 : CLI_SetWSStatus()                                       */
/* 函数功能 : 设置当前解释状态的属性                                 */
/*********************************************************************/
ULONG  CLI_SetWSStatus(ULONG  ulStatus)
{
    if (!CLI_IS_ENV_NEWLINE(m_stCliWS.ulStatus))
    {
        m_stCliWS.ulStatus = ulStatus;
    }
    else
    {
        m_stCliWS.ulStatus = ulStatus;
        m_stCliWS.ulStatus |= CLI_STAT_ENV_NEWLINE;
    }
    return TBS_SUCCESS;
}

ULONG  CLI_FindModeIdx(ULONG  ulMode, char  * szModeName)
{
    ULONG  ulRslt = 0xFF;
//    ULONG  i, ulDMode ;

    switch(ulMode)
    {
        default :
        {
            /* 非动态模式 */
            ulRslt = 0xFF;

            break;
        }
    }

    return ulRslt ;
}

/*********************************************************************/
/* 函数名称 : CLI_EnterMode()                                         */
/* 函数功能 : 进入模式的处理                                         */
/*********************************************************************/
_VOID    CLI_EnterMode(ULONG  ulMode, char  *szModeNme)
{
    ULONG  ulIndex; //ulDMode;

    ulIndex = CLI_FindModeIdx(ulMode, szModeNme) ;
    if (ulIndex == 0xFF)
        return ;

    switch(ulMode)
    {
        default :
        {
            IO_OutString("\r\nMode Error(program error)");
            MT_ERRLOG(0);
            break ;
        }
    }

    return ;
}

/*********************************************************************/
/* 函数名称 : CLI_ExitMode()                                          */
/* 函数功能 : 模式退出的处理                                         */
/* 输入参数 : ulMode:模式ID                                          */
/*            szModeName:模式名                                      */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_ExitMode(ULONG  ulMode, char  *szModeName)
{
    ULONG  ulIndex;  //ulDmode;

    ulIndex = CLI_FindModeIdx(ulMode, szModeName) ;
    if (ulIndex == 0xFF)
    {
        /* 如果返回0xff，该模式为静态模式，直接返回成功 */
        return TBS_SUCCESS;
    }

    switch(ulMode)
    {

        default :
        {
            IO_OutString("\r\nMode Error(program error)");
            MT_ERRLOG(0);
            break ;
        }
    }

    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数名称 : CLI_DelMode()                                           */
/* 函数功能 : 模式删除                                               */
/*********************************************************************/
ULONG  CLI_DelMode(ULONG  ulMode, char  * szModeName)
{
    ULONG  ulIndex, ulMutexFlag = 0 ;
    //ULONG  ulDmode;

    ulIndex = CLI_FindModeIdx(ulMode, szModeName) ;
    if (ulIndex == 0xFF)
        return TBS_SUCCESS ;

    switch(ulMode)
    {
        default :
        {
            IO_OutString("\r\nMode Error(program error)");
            MT_ERRLOG(0);
            break ;
        }
    }

    if (ulMutexFlag)
    {
        IO_Print(CLI_MODE_DEL_BANNED);
        return TBS_FAILED ;
    }

    return TBS_SUCCESS ;
}

/*  暂时只使能支持本板模拟发送 */

/*==============================================================*/
/*      函数名      :CLI_CmdExecute                              */
/*      函数功能    :对接收到的命令进行分析并执行               */
/*      输入参数    :PTerm_Data_S pTermStruct  任务数据指针     */
/*      输出参数    :PTerm_Data_S pTermStruct  任务数据指针     */
/*      返回值      : ULONG  TBS_SUCCESS: 成功                      */
/*                         TBS_FAILED: 失败                      */
/*      调用函数    :CLI_IntpKernelProc ( )                      */
/*      被调函数    :                                           */
/*==============================================================*/
ULONG  CLI_CmdExecute ( )
{
    ULONG  ulRet = CLI_INTP_RET_NEWLINE;

    if(CLI_SetCommandString( m_stCliTerm.szCommandBuf) == TBS_SUCCESS )
    {
        m_stCliWS.ulStatus |= CLI_STAT_ENV_ENTER ;
        m_stCliWS.ulStatus &= ~CLI_STAT_ENV_SYSTEM ;
        m_stCliWS.ulStatus &= ~CLI_STAT_ENV_NEWLINE;

        CLI_SetTermStatus(TERM_EXECCMD);
        m_stCliWS.ulExpectEle = 0;

        while (ulRet == CLI_INTP_RET_NEWLINE)
        {
            ulRet = CLI_IntpKernelProc();
            m_stCliTerm.szInputCmd[0] = '\0';
            m_stCliWS.ulStatus |= CLI_STAT_ENV_NEWLINE;
        }

        m_stCliWS.ulStatus &= ~CLI_STAT_ENV_NEWLINE;
        m_stCliWS.ulExpectEle = 0;

        if (ulRet != CLI_INTP_RET_HELP)
        {
            m_stCliTerm.szEditBuf[0] = '\0';
            m_stCliTerm.iCurrentPos = 0;
        }

        if (m_stCliTerm.nTermStatus == TERM_EXECCMD)
        {
            CLI_SetTermStatus(TERM_LOGED);
        }

        if (ulRet == TBS_SUCCESS)
        {
            m_stCliTerm.bCommandExecOk = TRUE ;
        }
    }

    STB_StrCpy(m_stCliTerm.szCommandBuf, m_stCliWS.szCmdString);

    return ulRet;
}


#ifdef __cplusplus
    }
#endif
