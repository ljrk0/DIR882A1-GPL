
/****************************************************************************
 文件名  : CLI_Cmdreg.c
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述:  命令行的命令树构造与命令注册接口的提供
 备注   :
 主要函数列表:

提供给外部的接口:   本文件提供给外部的接口
            (1) CLI_CmdAutoRegist

需要外部提供的接口:    本文件需要外部提供的接口
            (1) 输入输出接口
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
#include "cli_private.h"
#include "cli_term.h"
#include "cli_interpret.h"
#include "cli_io.h"

/* 子功能全局变量声明 */
static UCHAR m_ucPosIndex = 1;   /* 变量的绝对位置索引 */

ULONG  CLI_CmdRegist(const char  *szCmd,  UCHAR  ucMode,
                CLI_OPERAT_LEVEL_T   ucLevel,  PFUN_CLI_CALLBACK_EXEC pFunc,
                PCSTR  strHelp1, PCSTR  strHelp2, PCSTR  strHelp3)
{
    return CLI_CmdAutoRegist(szCmd, ucMode, ucLevel, pFunc,
                             CLI_MC_NULL, NULL_MODE,
                             strHelp1, strHelp2, strHelp3);
}



/*-------------------------------------------------------------------*/
/* 函数名称 : CLI_CmdAutoRegist                                       */
/* 功    能 : 命令自动注册函数接口                                   */
/* 输入参数 : 参数说明见函数定义                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 注册成功/失败                                          */
/* 调用说明 : 调用者自己保证命令语法的正确性,详细说明可以参见命令行  */
/*            说明文档. 注册否定形式的命令,须确定该命令的肯定形式已  */
/*            经注册,如注册no timeout命令之前应该已经注册timeout命令 */
/* 典型示例 : CLI_CmdAutoRegist("cmdtest user-test: <type(u1~3)>",    */
/*             CTM_DBG, CLI_AL_OPERATOR,  CLI_CmdTest4, CLI_MC_NULL,   */
/*             NULL_MODE, CLI_ML_NULL,  CLI_ML_NULL,  CLI_ML_NULL ); */
/*-------------------------------------------------------------------*/
ULONG  CLI_CmdAutoRegist
              (const char  *szCmd,              //命令格式化串
               UCHAR  ucMode,                   //命令所属模式
               CLI_OPERAT_LEVEL_T   ucLevel,    //命令使用等级
               PFUN_CLI_CALLBACK_EXEC pFunc,    //命令执行函数
               UCHAR  ucModeChange,             //模式转换属性(非模式转换命令填CLI_MC_NULL)
               UCHAR  ucNewMode,                //新模式的ID  (仅当上一参数为CLI_MC_NEW时有意义 )
               PCSTR  strHelp1,                 //第一个对象的帮助信息
               PCSTR  strHelp2,                 //第二个对象的帮助信息
               PCSTR  strHelp3)
{
    char  szCmdString[CLI_MAX_CMD_FORMAT + 1];  /* 命令的表达式可以超过命令的实际长度256,暂定上限为1000*/
    char  szToken[CLI_MAX_CMDLEN];
    char  *pCurPos;
    ULONG   ulRet, ulIndex = 0;
    PCSTR strObjHelp[CLI_MAX_KEY_NUM], strCheckID = NULL;
    UCHAR   ucIsNoForm = 0;
    PST_CLI_CMDKEY pCmdKey[CLI_MAX_KEY_NUM], pObj = NULL;
    PST_CLI_CMDKEY pFirstObj = NULL;
    ST_CLI_MODE   *sMode;
    PST_CLI_PARAMLINK *pParamLink = NULL;

    CLI_ASSURE_OR_FAIL(szCmd != NULL && STB_StrLen(szCmd) > 0);

    /* DEBUG模式的命令只能在调试版本中存在 */
#if !CLI_DEBUG_ON
    if (ucMode == CTM_DBG)
        return TBS_SUCCESS;
#endif

    STB_StrCpy(szCmdString, szCmd);
    (_VOID   )_AffirmStrBeLower(szCmdString);

    /* 命令的注册语法检查 */
    if (CLI_IsCmdValid(szCmdString) != TBS_SUCCESS)
    {
        REG_OUT("\r\n CLI: 命令格式不正确:%s",szCmd);
        DBG_ASSERT(0);
        return TBS_FAILED;
    }

    if (STB_StrNiCmp(szCmdString, "show ", 5) == 0)
    {
        ucMode = CTM_GLOBAL;
    }

    /* 其它参数合法性检查 */
    if (ucMode >= MAX_MODE_NUM || ucNewMode >= MAX_MODE_NUM || ucLevel > CLI_AL_DEBUG)
    {
        REG_OUT("\r\n CLI: 注册命令存在不合法参数:mode-%d, new mode-%d, level-%d", ucMode, ucNewMode, ucLevel);
        DBG_ASSERT(0);
        return TBS_FAILED;
    }

    strObjHelp[0] = strHelp1;
    strObjHelp[1] = strHelp2;
    strObjHelp[2] = strHelp3;

    for (ulIndex = 0; ulIndex < 3; ulIndex++)
    {
        if (strObjHelp[ulIndex] != NULL)
            strCheckID = strObjHelp[ulIndex];
    }

    if(!STB_StrNCmp(szCmdString, "show ", 5))
        ucLevel = CLI_AL_QUERY;

    pCurPos = szCmdString;
    ulIndex = 0;
    /* 如果是否定形式的命令，则命令树中应该已经存在其肯定形式，
       因此不用再创建对象，而采取搜索对象的形式 */
    if (STB_StrNCmp("no ", pCurPos, 3) == 0)
    {
        ucIsNoForm = 1;
        pCurPos   += 3;    //跳过"no "
        if ((sMode = CLI_GetMode(ucMode)) == NULL)
        {
            REG_OUT("\r\n CLI: 该命令的肯定形式尚未注册 : %s", szCmd);
            DBG_ASSERT(0);
            return TBS_FAILED;
        }
        pObj = sMode->pCmdKey;   /* 从模式对象开始搜索 */
    }

    /* 注册对象 */
    while((ulRet = CLI_GetObj(pCurPos, szToken)) != TBS_FAILED)
    {
        if (ulRet == TOKEN_GET_ONLYSPACE)
        {
            pCurPos++;
            continue;
        }
        if (ulRet == TOKEN_GET_CMDNULL)
        {
            REG_OUT("\r\n CLI: 从:%s 中获取命令字失败 : %s", pCurPos, szCmd);
            DBG_ASSERT(0);
            return TBS_FAILED;
        }

        pCurPos += STB_StrLen(szToken);

        /* 注册3个以上对象的命令不成功 */
        if (ulIndex > 2)
        {
            REG_OUT("\r\n  CLI: 关键命令字过多 : %s!", szCmd);
            DBG_ASSERT(0);
            return TBS_FAILED;
        }

        if (STB_StrLen(szToken) >= MAX_TOKEN_SIZE)
        {
            REG_OUT("\r\n  CLI: 超长命令字注册 : %s!", szToken);
            DBG_ASSERT(0);
            return TBS_FAILED;
        }

        /* 肯定形式的对象自动注册 */
        if (ucIsNoForm == 0)
        {
            if (ulRet == TOKEN_GET_LASTOBJ)
            {
                /* 最后一个命令对象需要处理其操作方法: 回调函数或模式转换 */
                switch (ucModeChange)
                {
                    case CLI_MC_NULL:
                        /* 对于非模式转换的命令需要保证带回调函数 */
                        if (pFunc == NULL)
                        {
                            REG_OUT("\r\n CLI: 命令(%s)\r\n      的对象(%s)没有执行操作!", szCmd, szToken);
                            DBG_ASSERT(0);
                            return TBS_FAILED;
                        }
                        pCmdKey[ulIndex] = CLI_CreateObject(szToken, ucMode, ucLevel,
                                        pFunc, FALSE, CLI_MC_NULL, NULL);
                        break;
                    /* 以下是对各种模式转换类型的对象注册 */
                    case CLI_MC_END:
                        pCmdKey[ulIndex] = CLI_CreateObject(szToken, ucMode, ucLevel,
                                        pFunc, TRUE, CLI_MC_END, NULL);
                        break;
                    case CLI_MC_EXIT:
                        pCmdKey[ulIndex] = CLI_CreateObject(szToken, ucMode, ucLevel,
                                        pFunc, TRUE, CLI_MC_EXIT, NULL);
                        break;
                    case CLI_MC_NEW:
                        /* 如果是转换到新模式，需要保证新模式的正确性 */
                        if (ucNewMode == NULL_MODE)
                        {
                            REG_OUT("\r\n CLI: 模式转换命令注册错误: 新模式ID: %d", ucNewMode);
                            DBG_ASSERT(0);
                            return TBS_FAILED;
                        }

                        pCmdKey[ulIndex] = CLI_CreateObject(szToken, ucMode, ucLevel,
                                        pFunc, TRUE, CLI_MC_NEW, NULL);

                        /* 如果是转换到新模式，还需要将该对象注册为新模式的模式对象*/
                        if (*pCurPos == ':' || *(pCurPos + 1) == ':')  /*这里需要改进*/
                            CLI_RegModeObject(ucNewMode, pCmdKey[ulIndex], 2);
                        else
                            CLI_RegModeObject(ucNewMode, pCmdKey[ulIndex], 0);
                        break;
                    default:
                        DBG_ASSERT(0);
                        return TBS_FAILED;

                }
                pParamLink = &(pCmdKey[ulIndex]->action.pParamLink);
            }
            else
            {
                pCmdKey[ulIndex] = CLI_CreateObject(szToken, ucMode, ucLevel,
                                NULL, FALSE, CLI_MC_NULL, NULL);
            }
            if (pCmdKey[ulIndex] == NULL)
            {
                REG_OUT("\r\n CLI: 对象创建失败: %s", szCmd);
                DBG_ASSERT(0);
                return TBS_FAILED;
            }

            if (ulIndex == 0)
            {
                pFirstObj = pCmdKey[ulIndex];
            }
            else
            {
                CLI_RegObject(pCmdKey[ulIndex - 1], pCmdKey[ulIndex]);
            }

            if (ucMode != CTM_DBG && ucMode != CTM_DIAG)
            {
                if (strObjHelp[ulIndex] == CLI_ML_NULL)
                {
                    REG_OUT("\r\n CLI: 命令(%s)\r\n      的对象(%s) 缺少帮助信息!", szCmd, szToken);
                    DBG_ASSERT(0);
                    return TBS_FAILED;
                }
                if (((CLI_IsObjectExecutable(pCmdKey[ulIndex], HAVEACTIONS) == TBS_SUCCESS)
                  && (strstr(strObjHelp[ulIndex], HELP_FUN) == NULL))
                 || ((CLI_IsObjectExecutable(pCmdKey[ulIndex], HAVEACTIONS) != TBS_SUCCESS)
                  && strstr(strObjHelp[ulIndex], HELP_FUN) != NULL))
                {
                    if (STB_StriCmp(szToken, "cm"))
                    {
                        REG_OUT("\r\n CLI: 命令(%s)\r\n      的对象(%s) 帮助信息格式不正确!", szCmd, szToken);
                        DBG_ASSERT(0);
                        return TBS_FAILED;
                    }
                }
            }
            CLI_RegCmdHelp(pCmdKey[ulIndex], strObjHelp[ulIndex]);

        }

        /* 否定形式的命令注册: 自动查找肯定形式的命令并修改其属性 */
        /* 使之具有命令的否定形式                                 */
        else
        {
            if (ulRet == TOKEN_GET_LASTOBJ)
            {
                pObj = CLI_GetSubObjByName(pObj, szToken);

                if (pObj == NULL)
                {
                    REG_OUT("\r\n CLI: 该命令的肯定形式尚未注册 : %s", szCmd);
                    DBG_ASSERT(0);
                    return TBS_FAILED;
                }

                pParamLink = &(pObj->noAction.pParamLink);
                /* 设置该对象的否定属性 */
                pObj->noAction.ulMode     = pObj->action.ulMode;
                pObj->noAction.rightLevel = pObj->action.rightLevel;
                pObj->noAction.pFunction  = pFunc;
                pObj->ulNoFlag            = 1;
            }
            else
            {
                pObj = CLI_GetSubObjByName(pObj, szToken);
                if (pObj == NULL)
                {
                    REG_OUT("\r\n CLI: 该命令的肯定形式尚未注册 : %s", szCmd);
                    DBG_ASSERT(0);
                    return TBS_FAILED;
                }
                pObj->ulNoFlag = 1;
            }
        }

        ulIndex++;

        /* 如果已经是最后一个对象，则跳出循环 */
        if (ulRet == TOKEN_GET_LASTOBJ)
            break;
    }

    /* 注册命令到模式必须要在所有对象创建完成之后进行, */
    /* 否则无法注册具有相同对象的情况                  */
    if (ucIsNoForm == 0)
    {
        if (CLI_RegObjToMode(ucMode, pFirstObj))
        {
            REG_OUT( "\r\n CLI: 命令注册失败: %s", szCmd);
            DBG_ASSERT(0);
        }
    }
    while (*pCurPos == KEY_SPACE)
        pCurPos++;

    if (*pCurPos != ':')
    {
        return TBS_SUCCESS;
    }

    while (*(++pCurPos) == KEY_SPACE)
        ;

    /* 这种情况表示分隔符后面已经没有参数链，直接返回 */
    if (*pCurPos == '\0')
    {
        return TBS_SUCCESS;
    }

    if (pParamLink == NULL)   /* 此判断不会进来，消除pc-lint告警 */
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    /*注册参数与参数值*/
    *pParamLink = CLI_CreatGroup(pCurPos);
    if (*pParamLink == NULL)
    {
        REG_OUT("\r\n CLI: 命令(%s)参数群注册失败!", szCmd);
        DBG_ASSERT(0);
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}


/*********************************************************************/
/* 函数名称 : CLI_IsCmdValid()                                        */
/* 函数功能 : 命令格式合法性的简单检查函数                           */
/* 输入参数 : szCmd 命令格式串                                       */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 上层函数 : CLI_CmdAutoRegist                                       */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_IsCmdValid(char  * szCmd)
{
    ULONG  ulPos, ulStrLen;
    UCHAR ucBig = 0, ucMid = 0, ucSmall = 0, ucNeedle = 0; //初始化大、中、小、尖括号的未匹配个数
    UCHAR ucIsOnlyObj = 1;   //是否只发现对象命令字，默认为是(1)
    char  cTmp;

    ulStrLen = STB_StrLen(szCmd);
    if (szCmd == NULL|| (ulStrLen  > CLI_MAX_CMD_FORMAT) || ulStrLen == 0)
    {
        return TBS_FAILED;
    }

    for (ulPos = 0; ulPos < ulStrLen; ulPos ++)
    {
        cTmp = *(szCmd + ulPos);
        //空格与小写字母和数字为完全合法的内容
        if ((cTmp >= 'a' && cTmp <= 'z')
         || (cTmp >= '0' && cTmp <= '9')
         || (cTmp == ' ' || cTmp == '-')
         ||  cTmp == '.' || cTmp == ',' 
         || cTmp == '_')
            continue;

        else if(cTmp != ':' && ucIsOnlyObj == 1) //如果目前只发现对象而出现结构字符，非法
            return TBS_FAILED;

        switch(cTmp)
        {
            case ':':
                if (ucIsOnlyObj == 0)
                    return TBS_FAILED;  //如果出现多次':'，非法
                ucIsOnlyObj = 0;
                break;
            /* 以下检验几种括符的匹配情况 */
            case '{':   ucBig++; break;
            case '}':
                if(ucBig == 0)
                    return TBS_FAILED;
                ucBig--;
                break;
            case '[':   ucMid++; break;
            case ']':
                if(ucMid== 0)
                    return TBS_FAILED;
                ucMid--;
                break;
            case '<':   ucNeedle++;  break;
            case '>':
                if(ucNeedle== 0)
                    return TBS_FAILED;
                ucNeedle--;
                break;
            case '(':   ucSmall++; break;
            case ')':
                if(ucSmall== 0)
                    return TBS_FAILED;
                ucSmall--;
                break;
            case '~':   /*如果值域符号出现在 参数值属性范围之外，非法 */
                if(ucSmall == 0)
                    return TBS_FAILED;
                break;
            case '/':
                if(ucNeedle == 0) /* 如果缺省值出现在参数值范围之外，非法*/
                    return TBS_FAILED;
                break;
            case '|':
                break;
            default:
                return TBS_FAILED;
        }
    }
    if ((ucBig | ucSmall | ucMid | ucNeedle) != 0) /* 如果有一种括号匹配不完全，非法 */
        return TBS_FAILED;
    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数名称 : CLI_GetSubObjByName()                                   */
/* 函数功能 : 根据对象名获取指定对象的子对象                         */
/* 输入参数 : pCmdKey 指定对象                                       */
/*            szName  所要获取的对象的对象名                         */
/* 输出参数 : 无                                                     */
/* 返回     : 获取的对象指针                                         */
/* 上层函数 : CLI_CmdAutoRegist                                       */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
PST_CLI_CMDKEY CLI_GetSubObjByName(PST_CLI_CMDKEY pCmdKey, char  *szName)
{
    PST_CLI_CMDKEY pObj;

    CLI_ASSURE_OR_NULL(pCmdKey != NULL);

    pObj = pCmdKey->pSubCmdKey;
    while (pObj != NULL)
    {
        if (STB_StriCmp(pObj->szName, szName) == 0)
            return pObj;
        pObj = pObj->pNext;
    }
    return NULL;
}

/* 所有参数的生成函数，在主机程序中可以返回参数链指针然后可以用手工生成 */
/* 的对象指向它，就可以为多个具有相同参数链的对象所用了 */
PST_CLI_PARAMLINK CLI_CreatGroup(char  * szParmGroup)
{
    char  *szLeft, *pcTmp;
    char  szToken[CLI_MAX_CMDLEN], szDesc[MAX_TOKEN_SIZE] = "";
    ULONG   ulRet, ulTmp;
    UCHAR  ucState;
    PST_CLI_PARAMLINK pPLink;
    ST_POINT_STACK *pPointStackCur, *pPointStackTmp;

    if (szParmGroup == NULL || *szParmGroup == 0)
        return NULL;

    /* 参数注册的状态结构栈，用于记录当前注册的元素所出的各种属性状态      */
    /* 注意申请与释放的匹配, 如果有失败操作而没有释放该内存，可以暂时忽略，*/
    /* 因为这种情况导致系统初始化失败, 已经不能正常运行                    */
    pPointStackCur = CLI_NewPointStack();
    pPointStackCur->pParamLink = CLI_CreateParamLink();
    if (pPointStackCur->pParamLink == NULL)
    {
        REG_OUT("\r\n CLI: 参数链创建失败: %s", szParmGroup);
        return NULL;
    }

    szLeft = szParmGroup;
    ucState = 0;
    m_ucPosIndex = 1;

    for (; *szLeft != '\0'; )
    {
        /* 获取当前的命令元素 */
        ulRet = CLI_GetRegistToken(szLeft, szToken);
        if (ulRet == TOKEN_GET_CMDNULL)
            break;
        if (STB_StrLen(szToken) >= CLI_MAX_PARAMLEN)
        {
            REG_OUT("\r\n CLI: 超长参数字注册 : %s!", szToken);
            return NULL;
        }
        switch(ulRet)
        {
            case TOKEN_GET_ONLYSPACE:
                szLeft++;
                break;   //如果返回发现当前的为空格，直接跳过

            /* 以空格结束的情况 */
            case TOKEN_GET_SPACEEND:
                if(*szToken != 0)
                {
                    szLeft += STB_StrLen(szToken);
                    CLI_SetProperty(&ucState, CLI_PRMPROP_NAME);
                    if (CLI_AutoCreatParam(pPointStackCur, szToken, &ucState) == NULL)
                    {
                        REG_OUT("\r\n!!!参数创建失败: %s", szToken);
                        return NULL;
                    }
                }
                else
                    szLeft++;
                break;

            /* 左花括符，表示参数组的开始 */
            case TOKEN_GET_MUTEXBEGIN:
                szLeft++;  //此处不需加break
            case TOKEN_GET_GROUPBEGIN:
                if((pcTmp= CLI_GetEndSignal(szLeft)) == 0) //获取结束符的位置
                    return NULL;
                /* 参数组，需要先创建一个新的环境环境结构栈 */
                pPointStackTmp = pPointStackCur;
                pPointStackCur = CLI_NewPointStack();
                pPointStackCur->Number = pPointStackTmp->Number;
                pPointStackCur->pPre = pPointStackTmp;
                pPointStackCur->pParamLink = pPointStackTmp->pParamLink;
                pPointStackCur->pParam = pPointStackTmp->pParam;

                //此处要先设置第一个为NEEDNAME属性，是为了让{}中的第一个参数必须为NEEDNAME属性
                //避免下面的条件不成立的情况下的空属性
                CLI_SetProperty(&ucState, CLI_PRMPROP_NAME);
                /* 如果前面决定该参数组与之互斥或后面有互斥符号，都标识该参数为互斥属性*/
                if(*(pcTmp + 1) == '|' || (pPointStackCur->pPre->ucMutexIndex != 0))
                {
                    CLI_SetProperty(&ucState, CLI_PRMPROP_MUTEX | CLI_PRMPROP_NAME);

                    if(pPointStackCur->pPre->ucMutexIndex != 0)
                    {   //如果是继承上次的互斥结果，则其位置索引应与上一个参数一致
                        m_ucPosIndex = pPointStackCur->pPre->ucMutexIndex;
                    }
                    else
                    {
                        pPointStackCur->pPre->ucMutexIndex = m_ucPosIndex;
                    }

                    if (*(pcTmp + 1) != '|')  //如果该参数与之后的参数不互斥，则将互斥位置清除
                        pPointStackCur->pPre->ucMutexIndex = 0;
                }

                szLeft++;
                /* 跳过所有空格*/
                while (CLI_GetRegistToken(szLeft, szToken) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                /* 如果参数组之后的第一个元素不是需要参数名的参数，错误 */
                if (*szToken == 0)
                {
                    REG_OUT("\r\n CLI: Error in :%s", szParmGroup);
                    return NULL;
                }
                szLeft += STB_StrLen(szToken);
                pPointStackCur->pParam = CLI_AutoCreatParam(pPointStackCur, szToken, &ucState);
                if (pPointStackCur->pParam == NULL)
                {
                    REG_OUT("\r\n CLI: 参数创建失败 :%s", szToken);
                    return NULL;
                }
                break;

            /* 左方括符，可选参数组的开始 */
            case TOKEN_GET_OPTIONBGIN:
                if((pcTmp= CLI_GetEndSignal(szLeft)) == 0) //获取结束符的位置
                    return NULL;
                szLeft++;
                /* 参数组，先创建一个新的状态结构栈 */
                pPointStackTmp = pPointStackCur;
                pPointStackCur             = CLI_NewPointStack();
                pPointStackCur->Number     = pPointStackTmp->Number;
                pPointStackCur->pParamLink = pPointStackTmp->pParamLink;
                pPointStackCur->pPre       = pPointStackTmp;
                pPointStackCur->pParam     = pPointStackTmp->pParam;

                /* 赋给参数组的属性: 需要参数名、可选、索引可调整 */
                CLI_SetProperty(&ucState, CLI_PRMPROP_NAME | CLI_PRMPROP_OPTIONAL | CLI_PRMPROP_INDEX);

                /* 跳过所有的空格 */
                while (CLI_GetRegistToken(szLeft, szToken) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                if (*szToken == 0)
                    return NULL;
                szLeft += STB_StrLen(szToken);
                pPointStackCur->pParam = CLI_AutoCreatParam(pPointStackCur, szToken, &ucState);
                if (pPointStackCur->pParam == NULL)
                {
                    REG_OUT("\r\n CLI: 参数创建失败:%s", szToken);
                    return NULL;
                }
                break;

            /* 参数组结束 */
            case TOKEN_GET_OPTIONEND:
            case TOKEN_GET_GROUPEND:
                if (*szToken != 0)
                {
                    szLeft += STB_StrLen(szToken);
                    CLI_SetProperty(&ucState, CLI_PRMPROP_NAME);
                    CLI_AutoCreatParam(pPointStackCur, szToken, &ucState);
                }
                szLeft++;
                /* 弹出状态栈, 释放状态资源 */
                pPointStackTmp = pPointStackCur;
                pPointStackCur = pPointStackCur->pPre;
                if (pPointStackCur->ucMaxIndex < m_ucPosIndex)
                {
                    pPointStackCur->ucMaxIndex = m_ucPosIndex;
                }
                else
                {
                    m_ucPosIndex = pPointStackCur->ucMaxIndex;
                }

                CLI_Free(pPointStackTmp);
                break;

            /* 左尖括符, 参数值开始 */
            case TOKEN_GET_VALBEGIN:
                if(CLI_GetEndSignal(szLeft) == 0) //获取结束符的位置
                    return NULL;
                szLeft++;
                /* 跳过所有空格 */
                while (CLI_GetRegistToken(szLeft, szToken) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                if (*szToken == 0)
                {
                    REG_OUT("\r\n CLI: Cannot get value!");
                    return NULL;
                }
                szLeft += STB_StrLen(szToken);
                STB_StrCpy(szDesc, szToken);

                /* 以下为参数值是否存在属性的判断*/
                while ((ulRet = CLI_GetRegistToken(szLeft, szToken)) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                if (ulRet != TOKEN_GET_PROPBEGIN)
                {
                    REG_OUT("\r\n CLI: 参数值缺乏属性: %s", szToken);
                    return NULL;
                }

                break;

            /* 左圆括符, 参数属性开始 */
            case TOKEN_GET_PROPBEGIN:
                szLeft++;
                /* 跳过所有空格 */
                while (CLI_GetRegistToken(szLeft, szToken) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                if (*szToken == 0 || *szDesc == 0)
                {
                    REG_OUT("\r\n CLI: No property of value:%s!", szDesc);
                    return NULL;
                }
                szLeft += STB_StrLen(szToken);

                if (CLI_AutoCreatVal(pPointStackCur, szDesc, &ucState, szToken) == NULL)
                {
                    REG_OUT("\r\n CLI: Creat value faile:%s", szDesc);
                    return NULL;
                }
                break;

            /* 斜杠，缺省值设置开始标志 */
            case TOKEN_GET_DEFAULT:
                szLeft++;
                if (*szToken != 0)
                {
                    REG_OUT("\r\n CLI: Default Value position error!(%s)", szToken);
                    return NULL;
                }
                /* 跳过所有空格 */
                while (CLI_GetRegistToken(szLeft, szToken) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                if (*szToken == 0 || *szDesc == 0)
                {
                    REG_OUT("\r\n CLI: No default of value:%s!", szDesc);
                    return NULL;
                }
                szLeft += STB_StrLen(szToken);

                /* 如果额外定义的值为该参数值的合法属性，设置为缺省值 */
                if (TBS_SUCCESS == CLI_CheckForValue(pPointStackCur->pValue, szToken))
                {
                    if (CLI_SetValueDefault(pPointStackCur->pValue, szToken) != TBS_SUCCESS)
                    {
                        REG_OUT("\r\n CLI: Set default for %s failed!", szToken);
                        return NULL;
                    }
                }
                /* 否则，设置为该参数值的序列串 */
                else
                {
                    pPointStackCur->pValue->keywords.pKeyword = CLI_NewKeywords(szToken, &ulTmp);
                    CLI_ASSURE_OR_NULL(NULL != pPointStackCur->pValue->keywords.pKeyword);
                    pPointStackCur->pValue->keywords.ulNumber = ulTmp;
                    switch (pPointStackCur->pValue->valueType.ucDataType)
                    {
                        case CLI_DATA_VI:
                            pPointStackCur->pValue->valueType.ucDataType = CLI_DATA_VIK;
                            break;
                        case CLI_DATA_VU:
                            pPointStackCur->pValue->valueType.ucDataType = CLI_DATA_VUK;
                            break;
                        case CLI_DATA_VH:
                            pPointStackCur->pValue->valueType.ucDataType = CLI_DATA_VHK;
                            break;
                        case CLI_DATA_IP:
                            pPointStackCur->pValue->valueType.ucDataType = CLI_DATA_IPK;
                            break;
                        case CLI_DATA_MAC:
                            pPointStackCur->pValue->valueType.ucDataType = CLI_DATA_MACK;
                            break;
                        default:
                            break;
                    }
                }
                break;

            case TOKEN_GET_MUTEXEND:
                szLeft++;    //此处不需加break
            case TOKEN_GET_MUTEXLINE:
            case TOKEN_GET_PROPEND:
            case TOKEN_GET_VALEND:
                szLeft++;
                break;
            default:
                break;
        }
    }
    pPLink = pPointStackCur->pParamLink;
    CLI_Free(pPointStackCur);
    return pPLink;
}

/*********************************************************************/
/* 函数名称 : CLI_GetObj()                                            */
/* 函数功能 : 从格式化串中获取对象名                                 */
/* 输入参数 : pCurPos 格式化串的当前位置                             */
/* 输出参数 : szObj   所获取的对象名                                 */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetObj(char  *pCurPos, char  *szObj)
{
    char  *pTmp;

    pTmp = pCurPos;

    /* 如果串首即为结束符, 返回空 */
    if(szObj == NULL || pTmp == NULL || *pTmp == 0 || *pTmp == ':')
        return TOKEN_GET_CMDNULL;

    memset(szObj, 0, MAX_TOKEN_SIZE);

    if (*pTmp == KEY_SPACE)
        return TOKEN_GET_ONLYSPACE;

    for(; *pTmp != '\0' && *pTmp != KEY_SPACE; pTmp++)
    {
        /* 如果以对象结束符结束, 表示已经达到最后一个对象 */
        if(*pTmp == ':')
            return TOKEN_GET_LASTOBJ;
        *szObj++ = *pTmp;
    }
    while (*pTmp == KEY_SPACE)
        pTmp++;

    /* 如果获取下一个不是空格的字符为空或为':',表示当前所取的为最后一个对象 */
    if (*pTmp == '\0' || *pTmp == ':')
        return TOKEN_GET_LASTOBJ;

    return TOKEN_GET_SPACEEND;
}

/*********************************************************************/
/* 函数名称 : CLI_GetRegistToken()                                    */
/* 函数功能 : 获取当前位置的命令字                                   */
/* 输入参数 : szSrc 格式化串位置                                     */
/* 输出参数 : szDrc 所获取的命令字                                   */
/* 返回     : 获取过程中遇到的分隔类型                               */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetRegistToken(char  * szSrc, char  * szDst)
{
    char  *pSrcPos, *pDstPos;

    memset(szDst, 0, MAX_TOKEN_SIZE);

    if (szDst == NULL || szSrc == NULL || *szSrc == '\0')
        return TOKEN_GET_CMDNULL;
    if (*szSrc == KEY_SPACE)
        return TOKEN_GET_ONLYSPACE;
    if (*szSrc == '|')
        return TOKEN_GET_MUTEXLINE;

    pSrcPos = szSrc;
    pDstPos = szDst;
    for(; *pSrcPos != '\0'; pSrcPos++, pDstPos++)
    {
        switch(*pSrcPos)
        {
            /* 遇见带有控制性的字符,返回相应的信息 */
            case ' ':   return TOKEN_GET_SPACEEND;
            case '{':
                if (*(pSrcPos + 1) == '{')
                    return TOKEN_GET_MUTEXBEGIN;
                return TOKEN_GET_GROUPBEGIN;
            case '}':
                if (*(pSrcPos + 1) == '}')
                    return TOKEN_GET_MUTEXEND;
                return TOKEN_GET_GROUPEND;
            case '[':   return TOKEN_GET_OPTIONBGIN;
            case ']':   return TOKEN_GET_OPTIONEND;
            case '(':   return TOKEN_GET_PROPBEGIN;
            case ')':   return TOKEN_GET_PROPEND;
            case '<':   return TOKEN_GET_VALBEGIN;
            case '>':   return TOKEN_GET_VALEND;
            case '/':   return TOKEN_GET_DEFAULT;
            case '|':
            default:
                *pDstPos = *pSrcPos;
        }
    }
    return TOKEN_GET_SPACEEND;
}
/*设置当前环境的某属性*/
_VOID    CLI_SetProperty(UCHAR *ucState, UCHAR usProp)
{
    *ucState |= usProp;
    return;
}
ULONG  CLI_IsProp(UCHAR ucState, UCHAR usProp)
{
    if((ucState & usProp) != 0)
        return TBS_SUCCESS;
    return TBS_FAILED;
}

/*********************************************************************/
/* 函数名称 : CLI_GetEndSignal()                                      */
/* 函数功能 : 获取与当前符号匹配的符号(主要为{} [] <>的匹配)         */
/* 输入参数 : 当前位置                                               */
/* 输出参数 :                                                        */
/* 返回     : 得到的匹配的符号位置                                   */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
char  *CLI_GetEndSignal(char  *pCurPos)
{
    char  cBegin, cEnd;
    ULONG   ulCount = 1;
    if (*pCurPos == '{')
    {
        cBegin = '{';
        cEnd   = '}';
    }
    else if (*pCurPos == '[')
    {
        cBegin = '[';
        cEnd   = ']';
    }
    else if (*pCurPos == '<')
    {
        cBegin = '<';
        cEnd   = '>';
    }
    else return NULL;

    pCurPos++;

    for (; *pCurPos != '\0'; pCurPos++)
    {
        if(*pCurPos == cBegin)
            ulCount++;
        if(*pCurPos == cEnd)
            ulCount--;
        if (ulCount == 0)
            return pCurPos;
    }
    REG_OUT("\r\n No matched of :%s!", pCurPos);
    return 0;

}

/*********************************************************************/
/* 函数名称 : CLI_GetValType()                                        */
/* 函数功能 : 参数值的属性获取                                       */
/* 输入参数 : szSrc 参数值的字符串形式                               */
/* 输出参数 : ucType   参数值类型                                    */
/*            piLow    整型值上限                                    */
/*            piHigh   整型值下限                                    */
/*            piStrLen 参数值长度                                    */
/* 返回     : 无                                                     */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
_VOID    CLI_GetValType(char  *szSrc, UCHAR *ucType, int    *piLow,
                   int    *piHigh, int    *piStrLen)
{
    char  szLow[20], szHigh[20];

    if (szSrc == NULL || ucType == NULL)
    {
        return ;
    }

    switch(*szSrc)
    {
        case 'I':
        case 'i':
        case 'U':
        case 'u':
            if (*szSrc == 'i' || *szSrc == 'I')
                *ucType = CLI_DATA_VI;
            else
                *ucType = CLI_DATA_VU;

            if (CLI_GetRangeStr(szSrc + 1, szLow, szHigh) == TBS_FAILED)
                return ;

            /* 将属性内容转换为整型的上下限, 转换失败则设为默认取值 */
            if ((ULONG )STB_atol(szLow,  piLow)  != TBS_SUCCESS)
                *piLow = 0;
            if ((ULONG )STB_atol(szHigh, piHigh) != TBS_SUCCESS)
                *piHigh = 0x7fffffff;
            *piStrLen = 10;
            break;

        case 'H':
        case 'h':
            *ucType = CLI_DATA_VH;
            if (CLI_GetRangeStr(szSrc + 1, szLow, szHigh) == TBS_FAILED)
                return ;

            /* 将属性内容转换为整型的上下限, 转换失败则设为默认取值 */
            if ((ULONG )STB_atoulx(szLow, (ULONG  *)piLow) != TBS_SUCCESS)
                *piLow = 0x0;
            if ((ULONG )STB_atoulx(szHigh, (ULONG  *)piHigh) != TBS_SUCCESS)
                *piHigh = 0x7fffffff;
            *piStrLen = 10;
            break;

        case 'S':
        case 's':
            *ucType = CLI_DATA_VS;
            if (CLI_GetRangeStr(szSrc + 1, szLow, szHigh) == TBS_FAILED)
                return ;
            /* 将属性内容字符串的最大长度, 转换失败则设为默认取值 80 */
            if ((ULONG )STB_atoul(szLow, (ULONG  *)piStrLen) != TBS_SUCCESS)
                *piStrLen = MAX_TOKEN_SIZE;
            break;

        case 'K':
        case 'k':
            *ucType = CLI_DATA_K;
            break;

        case 'P':
        case 'p':
            *ucType = CLI_DATA_IP;
            *piStrLen = 15;
            break;

        case 'M':
        case 'm':
            *ucType = CLI_DATA_MASK;
            *piStrLen = 15;
            break;

        case 'C':
        case 'c':
            *ucType = CLI_DATA_MAC;
            *piStrLen = 17;
            break;

        case 'D':
        case 'd':
            *ucType = CLI_DATA_DATE;
            *piStrLen = 10;
            break;

        case 'T':
        case 't':
            *ucType = CLI_DATA_TIME;
            *piStrLen = 8;
            break;
        default:
            *ucType = CLI_DATA_NULL;
            break;
    }

    return ;
}

/*状态结构初始化函数 */
ST_POINT_STACK *CLI_NewPointStack(_VOID   )
{
    ST_POINT_STACK *pPointStack;
    pPointStack = CLI_Malloc(sizeof(ST_POINT_STACK));
    if (pPointStack == NULL)
        return NULL;
    pPointStack->pParam = NULL;
    pPointStack->Number = 0;
    pPointStack->pPre = NULL;
    pPointStack->ucMaxIndex = 0;
    pPointStack->ucMutexIndex = 0;
    return pPointStack;
}

PST_CLI_PARAMETER CLI_AutoCreatParam(ST_POINT_STACK *pPointStack, char  *szName, UCHAR *ucStat)
{
    PST_CLI_PARAMETER pParam;
    CLI_ASSURE_OR_NULL(pPointStack != NULL && szName != NULL && ucStat != NULL);

    if ((pPointStack->pParam == NULL )
     && (pPointStack->pParamLink->pParam == NULL)
     && (TBS_SUCCESS == CLI_IsProp(*ucStat, CLI_PRMPROP_OPTIONAL)))
    {
        pParam = CLI_CreateParameter(szName, CLI_PRMPROP_NULL, 0, m_ucPosIndex++);
        CLI_RegParamToLink(pPointStack->pParamLink, pParam);
    }

    pParam = CLI_CreateParameter(szName, *ucStat, 0, m_ucPosIndex++);
    if (pPointStack->pParam == NULL)
        CLI_RegParamToLink(pPointStack->pParamLink, pParam);
    else
        CLI_RegParamAsHeir(pPointStack->pParam, pParam);

    pPointStack->pParam = pParam;
    *ucStat = 0;
    return pParam;
}

/*********************************************************************/
/* 函数名称 : CLI_AutoCreatVal()                                      */
/* 函数功能 : 参数值的自动注册                                       */
/* 输入参数 : pPointStack 注册的环境结构                             */
/*            szName      参数值名                                   */
/*            ucStat      当前状态                                   */
/*            szType      参数值类型                                 */
/* 输出参数 : 无                                                     */
/* 返回     : 自动注册的参数值结构指针                               */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
PST_CLI_VALUE CLI_AutoCreatVal(ST_POINT_STACK *pPointStack, char  *szName, UCHAR *ucStat, char  *szType)
{
    PST_CLI_VALUE pValue;
    PST_CLI_PARAMETER pParam;
    UCHAR  ucValType;
    char  szKeyLink[MAX_TOKEN_SIZE];
    char  *p, *p1;
    int    iLow = 0, iHigh = 0, iStrLen = CLI_MAX_PARAMLEN;

    CLI_ASSURE_OR_NULL(pPointStack != NULL && szName != NULL
                 && ucStat != NULL && szType != NULL);

    p = szType;
    while ((p1 = strchr(p, '|')) != p && p1 != 0)
    {
        p = p1;
        *p = ',';
    }
    if (p != szType)
    {
        STB_StrCpy(szKeyLink, szType);
        ucValType = CLI_DATA_K;
    }
    else
        CLI_GetValType(szType, &ucValType, &iLow, &iHigh, &iStrLen);

    if (pPointStack->pParam == NULL)
    {
        pParam = CLI_CreateParameter(szName, CLI_PRMPROP_NULL, 0, m_ucPosIndex++);
        CLI_RegParamToLink(pPointStack->pParamLink, pParam);
        pPointStack->pParam = pParam;
    }
    //如果前一个参数已经有兄弟参数或子参数了，则当前参数值不能直接
    //该参数下，因为同等位置下解释的优先级为 参数值 > 子参数 > 兄弟参数
    //如果直接注册，则该参数值会先于此前注册的其它参数被解释，因此应有以下分支
    else if (pPointStack->pParam->pNext != NULL)

    {
        pParam = CLI_CreateParameter(szName, CLI_PRMPROP_NULL, 0, m_ucPosIndex++);
        if (pPointStack->pParam->pParent == 0)
            CLI_RegParamToLink(pPointStack->pParamLink, pParam);
        else
            CLI_RegParamAsHeir(pPointStack->pParam->pParent, pParam);
        pPointStack->pParam = pParam;
    }
    else if (pPointStack->pParam->subParam != NULL)
    {
        pParam = CLI_CreateParameter(szName, CLI_PRMPROP_NULL, 0, m_ucPosIndex++);
        CLI_RegParamAsHeir(pPointStack->pParam, pParam);
        pPointStack->pParam = pParam;
    }
    else
    {
        pParam = pPointStack->pParam;
    }

    pValue = CLI_CreateValue(szName, ucValType, (ULONG)iStrLen,
                             szKeyLink,  iLow, iHigh, m_ucPosIndex++);
    pPointStack->pValue = pValue;
    CLI_RegValueToParam(pParam, pValue);

    *ucStat = 0;
    return pValue;
}


/*********************************************************************/
/* 函数名称 : CLI_GetRangeStr()                                      */
/* 函数功能 : 从字符串内获取整型值的上下限                           */
/* 输入参数 : szSrc  表达字符串                                      */
/* 输出参数 : szHigh 表示上限的字符串                                */
/*            szLow  表示下限的字符串                                */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_GetRangeStr(char  *szSrc, char  *szLow, char  *szHigh)
{
    char  *szDest;

    CLI_ASSURE_OR_FAIL(szSrc != NULL && szLow != NULL && szHigh != NULL);

    szDest = szLow;
    memset(szLow, 0, 20);
    memset(szHigh, 0, 20);

    for(;*szSrc != ')' && *szSrc != '\0'; )
    {
        if ((*szSrc >= 'a' && *szSrc <= 'f')
         || (*szSrc >= 'A' && *szSrc <= 'F')
         || (*szSrc >= '0' && *szSrc <= '9')
         ||  *szSrc == '.' || *szSrc == '-'
         ||  *szSrc == 'x' || *szSrc == 'X')
        {
            *szDest++ = *szSrc++;
            continue;
        }
        else if(*(szSrc++) == '~')
            szDest = szHigh;
        else if (*szSrc == KEY_SPACE)
            continue;
        else
            return TBS_FAILED;
    }
    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数名称 : CLI_CheckForValue()                                     */
/* 函数功能 : 检查值是否复合参数值的类型                             */
/* 输入参数 : pValue  参数值                                         */
/*            szToken 待检查的参数值                                 */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_CheckForValue(PST_CLI_VALUE pValue, char  *szToken)
{
    ULONG  ulVal;
    int    iVal;
    switch (pValue->valueType.ucDataType)
    {
        case CLI_DATA_VI:
            return _VerifyAndCvtDwordI(szToken,
                                    pValue->lIntFilter[0],
                                    pValue->lIntFilter[1],
                                    &iVal);
        case CLI_DATA_VU:
            return _VerifyAndCvtDword(szToken,
                                    (ULONG )pValue->lIntFilter[0],
                                    (ULONG )pValue->lIntFilter[1],
                                    &ulVal);
        case CLI_DATA_VH:
            return _VerifyAndCvtDwordH(szToken,
                                    (ULONG )pValue->lIntFilter[0],
                                    (ULONG )pValue->lIntFilter[1],
                                    &ulVal);
        case CLI_DATA_IP:
            return _VerifyGeneralIPAddress(szToken);
        case CLI_DATA_MAC:
            return _VerifyMACAddress(szToken);
        default:
            return TBS_SUCCESS;
    }
}

//自动注册结束  }}}}}***************************************

#ifdef __cplusplus
}
#endif

