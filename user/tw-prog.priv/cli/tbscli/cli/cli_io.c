/****************************************************************************
 文件名  : CLI_Io.c
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述:  本文件实现主机软件基本的输入输出，对外提供统一的接口；
            在实现上依赖终端数据，终端的输入输出功能。
 备注   :
 函数列表:
            (1);
            (2);
            (3);
            ......

提供给外部的接口:   本文件提供给外部的接口
            (1) IO_PrintToTerm
            (2) IO_Print
            (6) IO_GetChar
            (7) IO_GetCharByPeek
            (8) IO_OutString
            (9) IO_GetConfirm
            (10)IO_GetString

需要外部提供的接口:    本文件需要外部提供的接口
            (1) CLI_SendToTerm
            (2) CLI_GetTermData


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

#include <stdarg.h>

static UCHAR m_CLIucScroll = SCROLL_MANUAL;


/*-------------------------------------------------------------------*/
/* 函数名称 : IO_Print                                               */
/* 功    能 : 将格式化串输出到当前终端                               */
/* 输入参数 : ulInfoID 多语种资源ID                                  */
/* 输出参数 : 无                                                     */
/* 返回值   : 成功、失败                                             */
/* 调用说明 : 调用本函数需要确保ulInfoID在语种资源中已经有定义，且该 */
/*            资源字符串的最长不得超过2K                             */
/* 典型示例 : IO_Print(CLI_LOG_FAILED_TIME,3)                        */
/*            其中CLI_LOG_FAILED_TIME 对于中文语种信息为:            */
/*                            "\r\n  你已连续登录失败%d次! "         */
/*-------------------------------------------------------------------*/
ULONG   IO_Print(PCSTR szFormat, ...)
{
    char  szOutPut[OUTB_DIRRECLI_LENTGH] = "";
    va_list arglist ;

    if (m_stCliTerm.ulDispLineCount == CLI_INVALID
     && m_stCliTerm.nTermStatus == TERM_EXECCMD)
        return TBS_SUCCESS;

    /* 格式化串的组织 */
    if (szFormat == NULL || STB_StrLen(szFormat) > OUTB_DIRRECLI_LENTGH - 200)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }
    va_start(arglist, szFormat) ;
    if (vsprintf(szOutPut, szFormat, arglist) >= OUTB_DIRRECLI_LENTGH)
    {
        return TBS_FAILED;
    }
    szOutPut[(OUTB_DIRRECLI_LENTGH) - 1] = '\0';
    va_end(arglist) ;

    return IO_OutToTerm(szOutPut);
}

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_OutString                                           */
/* 功    能 : 直接输出字符串到当前终端                               */
/* 输入参数 : szOutput 输出信息串                                    */
/* 输出参数 : 无                                                     */
/* 返回值   : 成功、失败                                             */
/* 调用说明 : 直接输出，该函数的调用者多为调试信息输出与特殊要求的   */
/*            输出,如果当前任务非终端任务,则输出到串口               */
/* 典型示例 : IO_OutString("\r\n  正确输出.")                        */
/*-------------------------------------------------------------------*/
ULONG   IO_OutString(const char  *szOutput)
{
    return IO_OutToTerm(szOutput);
}

/*********************************************************************/
/* 函数名称 : IO_OutToTerm()                                         */
/* 函数功能 : 组织输出字符串，并根据终端调用函数输出                 */
/* 输入参数 : pTermStruct   终端任务数据                             */
/*            szOutString  输出字符串                                */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  IO_OutToTerm(const char  *szOutString)
{
    char  cTmp;
    const char  *szOut;
    const char  *pcTmp;
    ULONG  ulStrLen;
    ULONG  ulTmp;
    BOOL bIsAsynDisplay = FALSE;

    if (m_stCliTerm.ucScroll==SCROLL_AUTO)
    {
        m_stCliTerm.ulDispLineCount = 0;
        bIsAsynDisplay = TRUE;
    }

    if ((m_stCliTerm.ulDispLineCount == CLI_INVALID
             && m_stCliTerm.nTermStatus == TERM_EXECCMD)
      || m_stCliTerm.nTermStatus == TERM_SLEEPING)
        return TBS_SUCCESS;

    szOut = szOutString;
    /* 如果发送缓冲中还存在没有显示的内容，先显示之 */
    if (m_stCliTerm.szSendBuf[0] != '\0')
        CLI_SendToTerm();

    /* 如果输出的信息长度超过任务缓冲允许的范围，分段输出 */
    pcTmp = szOut;
    ulStrLen = STB_StrLen(szOutString);

    while (*pcTmp != '\0' && pcTmp < szOutString + ulStrLen)
    {
        if (*pcTmp == '\r' || *pcTmp == '\n' || pcTmp - szOut > LINE_LENGTH)
        {
            STB_StrNCpy(m_stCliTerm.szSendBuf, szOut, (ULONG )(pcTmp - szOut));
            m_stCliTerm.szSendBuf[pcTmp - szOut] = '\0';
            CLI_SendToTerm();

            if (*pcTmp == '\r' || *pcTmp == '\n')
                szOut = pcTmp++; /* 这里需要递加，跳过接下来的\r或\n */
            else
                szOut += (pcTmp - szOut);

            /* 显示每满一屏要作处理， 需要预留一行用作提示信息输出  */
            if (++m_stCliTerm.ulDispLineCount >= LINE_PER_PAGE - 1)
            {
                m_stCliTerm.ulDispLineCount = 0;

                /* 如果是手动分屏，需要按任意键继续 */
                if ((m_stCliTerm.ucScroll == SCROLL_MANUAL && m_stCliTerm.nTermStatus == TERM_EXECCMD)
                 || (bIsAsynDisplay == FALSE)) // 不是异步输出才可以进行该处理
                {
                    STB_StrCpy(m_stCliTerm.szSendBuf,
                           "\r\n  --- More ---");
                    CLI_SendToTerm();

                    /*如果输入CTL_C或终端非正常状态，终止显示*/
                    while ((cTmp = IO_GetCharByPeek()) != '\0'
                         && cTmp != KEY_CTRL_C)
                    {
                        ;
                    }
                    if ((cTmp == KEY_CTRL_C)
                     || (cTmp = IO_GetChar()) == KEY_CTRL_C
                     || (cTmp == '\0')
                     || !IS_TERM_NORMAL(m_stCliTerm))
                    {
                        for (ulTmp = 0; ulTmp < STB_StrLen("  --- More ---"); ulTmp++)
                        {
                            STB_StrCat(m_stCliTerm.szSendBuf, "\x1b[1D");
                        }
						for(ulTmp = 0; ulTmp < LINE_LENGTH-1; ulTmp++)
						{
	                        STB_StrCat(m_stCliTerm.szSendBuf, " ");
						}/* 在一行的末尾上跳一行 */
						STB_StrCat(m_stCliTerm.szSendBuf, "\x1b[1A");
                        CLI_SendToTerm();

                        m_stCliTerm.ulDispLineCount = CLI_INVALID;
                        return TBS_SUCCESS;
                    }
                    else if (cTmp == '\r' || cTmp == '\n')
                    {
                        m_stCliTerm.ulDispLineCount = LINE_PER_PAGE - 1;
                    }
                    for (ulTmp = 0; ulTmp < STB_StrLen("  --- More ---"); ulTmp++)
					{
                        STB_StrCat(m_stCliTerm.szSendBuf, "\x1b[1D");
					}
					for(ulTmp = 0; ulTmp < LINE_LENGTH-1; ulTmp++)
					{
                        STB_StrCat(m_stCliTerm.szSendBuf, " ");
					}/* 在一行的末尾上跳一行 */
					STB_StrCat(m_stCliTerm.szSendBuf, "\x1b[1A");

                    CLI_SendToTerm();
                }
                /* 如果是自动分屏，查看是否有键入CTRL+C试图终止，没有则继续 */
                else
                {
                    while ((cTmp = IO_GetCharByPeek()) != '\0'
                         && cTmp != KEY_CTRL_C)
                    {
                        ;
                    }

                    if (cTmp == KEY_CTRL_C)
                    {
                        m_stCliTerm.ulDispLineCount = CLI_INVALID;
                        return TBS_SUCCESS;
                    }

                    CLI_DELAY(100);
                    while ((cTmp = IO_GetCharByPeek()) != '\0'
                         && cTmp != KEY_CTRL_C)
                    {
                        ;
                    }

                    if (cTmp == KEY_CTRL_C)
                    {
                        m_stCliTerm.ulDispLineCount = CLI_INVALID;
                        return TBS_SUCCESS;
                    }
                }
            }
        }
        pcTmp++;
    }

    STB_StrCpy(m_stCliTerm.szSendBuf, szOut);
    return CLI_SendToTerm();
}


/*-------------------------------------------------------------------*/
/* 函数名称 : IO_GetChar                                             */
/* 功    能 : 从终端接收一个字符                                     */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回值   : 接收到的字符                                           */
/* 调用说明 : 该函数调用导致任务阻塞，直到终端有输入或超时。注意该函 */
/*            数的调用需要判断返回值，如果返回NULL则可能是终端异常， */
/*            建议程序释放当前资源，不再继续处理当前事件             */
/* 典型示例 : cTmp = IO_GetChar();                                   */
/*-------------------------------------------------------------------*/
char  IO_GetChar(_VOID   )
{
    char  cCharReceived;

    if (m_stCliTerm.nTermType == CLI_TERMT_FOR_MML)
    {
        MT_ERRLOG(0);
        return '\0';
    }

    if (CLI_ReceiveFromTerm(m_stCliTerm.ulTimeLeft) != TBS_SUCCESS)
    {
        return '\0';
    }

    cCharReceived = m_stCliTerm.szRecvBuf[0];
    m_stCliTerm.szRecvBuf[0] = '\0';
    m_stCliTerm.iRecvLen     = 0;

    return cCharReceived;
}

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_GetChar                                             */
/* 功    能 : 从终端接收缓冲读取一个字符                             */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回值   : 读取到的字符                                           */
/* 调用说明 : 该调用直接读取当前终端的接收缓冲，如果无内容则返回空   */
/* 典型示例 : cTmp = IO_GetCharByPeek();                             */
/*-------------------------------------------------------------------*/
char   IO_GetCharByPeek(_VOID   )
{
    char  cCharReceived;

    CLI_ReceiveFromTerm(0);

    cCharReceived = m_stCliTerm.szRecvBuf[0];
    m_stCliTerm.szRecvBuf[0] = '\0';
    m_stCliTerm.iRecvLen = 0;

    return cCharReceived;
}

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_GetString                                           */
/* 功    能 : 从当前终端接收一个字符串                               */
/* 输入参数 : ulSize  :字符串允许的最大长度                          */
/*            bInvisable  :输入模式:FALSE－字符回显，TRUE－星号回显  */
/* 输出参数 : szInput :接收的的输入字符串                            */
/* 返回值   : 成功、失败                                             */
/* 调用说明 : 该调用导致终端任务阻塞，直到超时或接收到回车键. 调用者 */
/*            应对返回值进行判断, 如果失败,则认为接收到的内容无效，  */
/*            导致这种情况的最大可能是终端异常，程序应当立即返回，让 */
/*            系统去识别并处理这种异常                               */
/* 典型示例 : if (IO_GetString(&szUsername,16,FALSE) != TBS_SUCCESS) */
/*                return TBS_FAILED;                                */
/*-------------------------------------------------------------------*/
ULONG   IO_GetString(char  *szInput, ULONG  ulSize, BOOL  bInvisable)
{
    int    i;
    ULONG  ulRet;

    if ( NULL == szInput)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }
    if (m_stCliTerm.nTermType == CLI_TERMT_FOR_MML)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    /* 初始化任务数据的编辑环境变量(缺省状态下为适应命令行的输入)  */
    m_stCliTerm.iMaxLen      = (int)ulSize ;
    m_stCliTerm.szEditBuf[0] = '\0' ;
    memset(m_stCliWS.szCmdString, 0, sizeof(m_stCliWS.szCmdString));
    m_stCliTerm.iCurrentPos  = 0 ;

    if (!bInvisable)
    {
        STB_StrCpy(m_stCliTerm.szInputCmd, m_stCliWS.szCmdString);
#if 0
//不知道这个是干什么用的，我用IO_GetString来获取输入时，
//只输入"admin"，但却给我返回" admin"，明显多了个空格
        if (m_stCliTerm.szInputCmd[strlen(m_stCliTerm.szInputCmd) - 1] != 0x20)
        {
            STB_StrCat(m_stCliTerm.szInputCmd, " ");
        }
#endif
        STB_StrCat(m_stCliTerm.szInputCmd, m_stCliTerm.szEditBuf);
        STB_StrCpy(m_stCliTerm.szEditBuf,  m_stCliTerm.szInputCmd);
        m_stCliTerm.iCurrentPos  = strlen(m_stCliTerm.szEditBuf) ;

        m_stCliTerm.iEditStatus  = COMMAND_INPUT;
    }
    else
        m_stCliTerm.iEditStatus  = STRING_INPUT;

    for(;;)
    {
        /* 接收来自Client端的数据  */
        if ( CLI_ReceiveFromTerm(m_stCliTerm.ulTimeLeft) != TBS_SUCCESS)
        {
            if (m_stCliTerm.nTermStatus < TERM_LOGED || m_stCliTerm.ulTimeLeft == 0)
                return TBS_FAILED;
            continue ;
        }

        if (!IS_TERM_NORMAL(m_stCliTerm))
            return TBS_FAILED;

        /* 编辑接收到的字符串 */
        if (!(ulRet = CLI_EditString()))
        {
            if (!bInvisable && !(strcmp(m_stCliTerm.szEditBuf,  m_stCliTerm.szInputCmd))
             && (m_stCliTerm.szRecvBuf[0] == 0xa || m_stCliTerm.szRecvBuf[0] == 0xd))
            {
                return TBS_FAILED;
            }
            if ( !bInvisable )
            {
                break ;
			}
        }
        if (!bInvisable && m_stCliTerm.szRecvBuf[0] == '?')
        {
            if ( !bInvisable)
                break;
        }

        if (m_stCliTerm.iSendLen == 0 && ulRet != TBS_SUCCESS)
            continue;
        /* 回显输入信息                                     */
        /* 如果输入模式为星号回显，用星号替代输出缓冲的字符 */
        if ( bInvisable)
        {
            for (i = 0; i < m_stCliTerm.iSendLen; i++)
            {
                if (m_stCliTerm.szSendBuf[i] < 0x20
                 || m_stCliTerm.szSendBuf[i] > 0x7e)
                {
                    if (m_stCliTerm.szSendBuf[i] == 0)
                        break;
                    //如果在输入的时候出现方向控制符，唯一的可能性就是左移删除"\x1b[1D"
                    //如果存在其它的可能性，此处可能需修改
                    if (STB_StrNCmp(&m_stCliTerm.szSendBuf[i], "\x1b[1D", 4) == 0)
                    {
                        i += 3;   //这里只递增3是因为循环本身会进行一次递增
                        if (m_stCliTerm.szSendBuf[i + 1] == 0x20)
                            i++;
                    }
                    else if (STB_StrNCmp(&m_stCliTerm.szSendBuf[i], "\x1b[D", 4) == 0)
                    {
                        i += 3;   //这里只递增3是因为循环本身会进行一次递增
                        if (m_stCliTerm.szSendBuf[i + 1] == 0x20)
                            i++;
                    }
                    else if (STB_StrNCmp(&m_stCliTerm.szSendBuf[i], "\x1b[1C", 4) == 0)
                    {
                        i += 3;
                    }
                    else if (STB_StrNCmp(&m_stCliTerm.szSendBuf[i], "\x1b[C", 3) == 0)
                    {
                        i += 2;
                    }

                    continue;
                }
                m_stCliTerm.szSendBuf[i] = '*';
            }
        }
        CLI_SendToTerm();
        if (ulRet == TBS_SUCCESS)
        {
            break;    /* 处理进入星号回显流程可能造成的死循环: 避免之上的continue语句造成死循环 */
        }

    }

    STB_StrCpy( szInput, m_stCliTerm.szEditBuf );

    if (!bInvisable )
    {
        STB_StrCpy(m_stCliTerm.szCommandBuf, m_stCliTerm.szEditBuf);
        /*如果该输入是被"?"中断的，输入为"?"*/
        if ( STB_StrLen(szInput) > 0 && szInput[STB_StrLen ( szInput ) - 1] == '?' )
        {
            szInput[0] = '?';
            szInput[1] = '\0';
        }
    }
    /* 恢复任务数据的编辑状态到适应命令行 */
    m_stCliTerm.iEditStatus  = COMMAND_INPUT;
    m_stCliTerm.iMaxLen      = CLI_MAX_CMDBUF;
    m_stCliTerm.szEditBuf[0] = '\0' ;
    m_stCliTerm.iCurrentPos  = 0 ;

    /*如果该输入是被CTCR+C中断的，输入为空*/
    if ( STB_StrLen(szInput) > 0 && szInput[STB_StrLen ( szInput ) - 1] == KEY_CTRL_Z )
        szInput[0] = '\0';

    return TBS_SUCCESS ;
}

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_GetConfirm                                          */
/* 功    能 : 等待用户输入确认信息                                   */
/* 输入参数 : ulInfoID       :语种ID                                 */
/*            ulDefaultChoice:缺省输入                               */
/* 输出参数 : 无                                                     */
/* 返回     : INPUT_NO/INPUT_YES/INPUT_TIMEOUT                       */
/* 调用说明 : 该调用导致终端任务阻塞，直到超时或接收到按键输入. 如果 */
/*            输入回车或三次均为'n'与'y'之外的内容, 返回缺省选项.    */
/*            超时时间统一定义为5秒                                  */
/* 典型示例 : if (IO_GetConfirm(CLI_KICK_OFF_ANTHOR, INPUT_NO)       */
/*                != INPUT_YES)                                      */
/*                ...... ;                                           */
/*-------------------------------------------------------------------*/
ULONG   IO_GetConfirm(PCSTR szFormat,  ULONG  ulDefaultChoice)
{
    char  szPrompt[20];
    ULONG   ulCount;
    ULONG   ulRet;
    char   cInput;

    ulRet = ulDefaultChoice;

    if (m_stCliTerm.nTermType == CLI_TERMT_FOR_MML)
    {
        return INPUT_YES;
    }

    if (ulDefaultChoice == INPUT_YES)
        STB_StrCpy(szPrompt, " Y/N[Y]:");
    else
        STB_StrCpy(szPrompt, " Y/N[N]:");

    /* 对于输入错误的情况，提供最多3次输入机会，之后以缺省输入返回*/
    for (ulCount = 0; ulCount < 3; ulCount ++)
    {
        IO_Print(szFormat);
        IO_OutToTerm(szPrompt);

        /* 等待终端输入确认信息，5秒超时 */
        m_stCliTerm.szRecvBuf[0] = '\0';
        CLI_ReceiveFromTerm(5);
        if (!IS_TERM_NORMAL(m_stCliTerm))
        {
            return ulDefaultChoice;
        }
        else if (m_stCliTerm.szRecvBuf[0] == '\0')
        {
            IO_Print(CLI_INPUT_CONFIRM_TIMEOUT);
            return INPUT_TIMEOUT;
        }

        cInput = m_stCliTerm.szRecvBuf[0];
        m_stCliTerm.szRecvBuf[0] = '\0';
        m_stCliTerm.iRecvLen = 0;

        if ( cInput != '\0')
        {
            switch (cInput)
            {
                case 'y':
                case 'Y':
                    IO_OutString("Yes\r\n");
                    return INPUT_YES;
                case 'n':
                case 'N':
                    IO_OutString("No\r\n");
                    return INPUT_NO;
                /* 如果输入回车, 则返回缺省值 */
                case '\r':
                case '\n':
                    IO_OutString("\r\n");
                    return ulRet;
                default:
                    if(ulCount >= 2)
                    {
                        return ulRet;
                    }
                    IO_Print(CLI_INPUT_CONFIRM_ERROR);
                    break;
            }
        }
    }
    return ulRet;
}


/*-------------------------------------------------------------------*/
/* 函数名称 : IO_SetScrollAuto                                       */
/* 功    能 : 设置当前终端为自动滚屏                                 */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 调用说明 :                                                        */
/*-------------------------------------------------------------------*/
ULONG  IO_SetScrollAuto(_VOID   )
{
    m_CLIucScroll = m_stCliTerm.ucScroll;
    m_stCliTerm.ucScroll = SCROLL_AUTO;

    return TBS_SUCCESS;
}

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_SetScrollBack                                       */
/* 功    能 : 恢复当前终端的滚屏方式                                 */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 调用说明 : 该接口须与IO_SetScrollAuto配套使用                     */
/*-------------------------------------------------------------------*/
ULONG  IO_SetScrollBack(_VOID   )
{
    m_stCliTerm.ucScroll = m_CLIucScroll;

    return TBS_SUCCESS;
}

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_IsOutputBroken                                      */
/* 功    能 : 判断当前的输出是否被中止                               */
/* 输入参数 : ulTermID: 终端ID                                       */
/* 输出参数 : 无                                                     */
/* 返回     : TRUE/FALSE                                             */
/* 调用说明 : 该接口一般用于有较多的输出过程中调用                   */
/*-------------------------------------------------------------------*/
BOOL IO_IsOutputBroken()
{
    if (m_stCliTerm.nTermStatus >= TERM_LOGED && m_stCliTerm.ulDispLineCount == CLI_INVALID)
        return TRUE;

    return FALSE;
}


#ifdef __cplusplus
}
#endif

