/****************************************************************************
 �ļ���  : CLI_Io.c
 ����    : liuzequn
 �汾    :
 �������:
 �ļ�����:  ���ļ�ʵ�����������������������������ṩͳһ�Ľӿڣ�
            ��ʵ���������ն����ݣ��ն˵�����������ܡ�
 ��ע   :
 �����б�:
            (1);
            (2);
            (3);
            ......

�ṩ���ⲿ�Ľӿ�:   ���ļ��ṩ���ⲿ�Ľӿ�
            (1) IO_PrintToTerm
            (2) IO_Print
            (6) IO_GetChar
            (7) IO_GetCharByPeek
            (8) IO_OutString
            (9) IO_GetConfirm
            (10)IO_GetString

��Ҫ�ⲿ�ṩ�Ľӿ�:    ���ļ���Ҫ�ⲿ�ṩ�Ľӿ�
            (1) CLI_SendToTerm
            (2) CLI_GetTermData


 �޸���ʷ:
        1. �޸���   :
           ʱ��     :
           �汾     :
           �޸�ԭ�� :
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
/* �������� : IO_Print                                               */
/* ��    �� : ����ʽ�����������ǰ�ն�                               */
/* ������� : ulInfoID ��������ԴID                                  */
/* ������� : ��                                                     */
/* ����ֵ   : �ɹ���ʧ��                                             */
/* ����˵�� : ���ñ�������Ҫȷ��ulInfoID��������Դ���Ѿ��ж��壬�Ҹ� */
/*            ��Դ�ַ���������ó���2K                             */
/* ����ʾ�� : IO_Print(CLI_LOG_FAILED_TIME,3)                        */
/*            ����CLI_LOG_FAILED_TIME ��������������ϢΪ:            */
/*                            "\r\n  ����������¼ʧ��%d��! "         */
/*-------------------------------------------------------------------*/
ULONG   IO_Print(PCSTR szFormat, ...)
{
    char  szOutPut[OUTB_DIRRECLI_LENTGH] = "";
    va_list arglist ;

    if (m_stCliTerm.ulDispLineCount == CLI_INVALID
     && m_stCliTerm.nTermStatus == TERM_EXECCMD)
        return TBS_SUCCESS;

    /* ��ʽ��������֯ */
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
/* �������� : IO_OutString                                           */
/* ��    �� : ֱ������ַ�������ǰ�ն�                               */
/* ������� : szOutput �����Ϣ��                                    */
/* ������� : ��                                                     */
/* ����ֵ   : �ɹ���ʧ��                                             */
/* ����˵�� : ֱ��������ú����ĵ����߶�Ϊ������Ϣ���������Ҫ���   */
/*            ���,�����ǰ������ն�����,�����������               */
/* ����ʾ�� : IO_OutString("\r\n  ��ȷ���.")                        */
/*-------------------------------------------------------------------*/
ULONG   IO_OutString(const char  *szOutput)
{
    return IO_OutToTerm(szOutput);
}

/*********************************************************************/
/* �������� : IO_OutToTerm()                                         */
/* �������� : ��֯����ַ������������ն˵��ú������                 */
/* ������� : pTermStruct   �ն���������                             */
/*            szOutString  ����ַ���                                */
/* ������� : ��                                                     */
/* ����     : �ɹ���ʧ��                                             */
/* �ϲ㺯�� :                                                        */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
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
    /* ������ͻ����л�����û����ʾ�����ݣ�����ʾ֮ */
    if (m_stCliTerm.szSendBuf[0] != '\0')
        CLI_SendToTerm();

    /* ����������Ϣ���ȳ������񻺳�����ķ�Χ���ֶ���� */
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
                szOut = pcTmp++; /* ������Ҫ�ݼӣ�������������\r��\n */
            else
                szOut += (pcTmp - szOut);

            /* ��ʾÿ��һ��Ҫ������ ��ҪԤ��һ��������ʾ��Ϣ���  */
            if (++m_stCliTerm.ulDispLineCount >= LINE_PER_PAGE - 1)
            {
                m_stCliTerm.ulDispLineCount = 0;

                /* ������ֶ���������Ҫ����������� */
                if ((m_stCliTerm.ucScroll == SCROLL_MANUAL && m_stCliTerm.nTermStatus == TERM_EXECCMD)
                 || (bIsAsynDisplay == FALSE)) // �����첽����ſ��Խ��иô���
                {
                    STB_StrCpy(m_stCliTerm.szSendBuf,
                           "\r\n  --- More ---");
                    CLI_SendToTerm();

                    /*�������CTL_C���ն˷�����״̬����ֹ��ʾ*/
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
						}/* ��һ�е�ĩβ����һ�� */
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
					}/* ��һ�е�ĩβ����һ�� */
					STB_StrCat(m_stCliTerm.szSendBuf, "\x1b[1A");

                    CLI_SendToTerm();
                }
                /* ������Զ��������鿴�Ƿ��м���CTRL+C��ͼ��ֹ��û������� */
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
/* �������� : IO_GetChar                                             */
/* ��    �� : ���ն˽���һ���ַ�                                     */
/* ������� : ��                                                     */
/* ������� : ��                                                     */
/* ����ֵ   : ���յ����ַ�                                           */
/* ����˵�� : �ú������õ�������������ֱ���ն��������ʱ��ע��ú� */
/*            ���ĵ�����Ҫ�жϷ���ֵ���������NULL��������ն��쳣�� */
/*            ��������ͷŵ�ǰ��Դ�����ټ�������ǰ�¼�             */
/* ����ʾ�� : cTmp = IO_GetChar();                                   */
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
/* �������� : IO_GetChar                                             */
/* ��    �� : ���ն˽��ջ����ȡһ���ַ�                             */
/* ������� : ��                                                     */
/* ������� : ��                                                     */
/* ����ֵ   : ��ȡ�����ַ�                                           */
/* ����˵�� : �õ���ֱ�Ӷ�ȡ��ǰ�ն˵Ľ��ջ��壬����������򷵻ؿ�   */
/* ����ʾ�� : cTmp = IO_GetCharByPeek();                             */
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
/* �������� : IO_GetString                                           */
/* ��    �� : �ӵ�ǰ�ն˽���һ���ַ���                               */
/* ������� : ulSize  :�ַ����������󳤶�                          */
/*            bInvisable  :����ģʽ:FALSE���ַ����ԣ�TRUE���ǺŻ���  */
/* ������� : szInput :���յĵ������ַ���                            */
/* ����ֵ   : �ɹ���ʧ��                                             */
/* ����˵�� : �õ��õ����ն�����������ֱ����ʱ����յ��س���. ������ */
/*            Ӧ�Է���ֵ�����ж�, ���ʧ��,����Ϊ���յ���������Ч��  */
/*            ����������������������ն��쳣������Ӧ���������أ��� */
/*            ϵͳȥʶ�𲢴��������쳣                               */
/* ����ʾ�� : if (IO_GetString(&szUsername,16,FALSE) != TBS_SUCCESS) */
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

    /* ��ʼ���������ݵı༭��������(ȱʡ״̬��Ϊ��Ӧ�����е�����)  */
    m_stCliTerm.iMaxLen      = (int)ulSize ;
    m_stCliTerm.szEditBuf[0] = '\0' ;
    memset(m_stCliWS.szCmdString, 0, sizeof(m_stCliWS.szCmdString));
    m_stCliTerm.iCurrentPos  = 0 ;

    if (!bInvisable)
    {
        STB_StrCpy(m_stCliTerm.szInputCmd, m_stCliWS.szCmdString);
#if 0
//��֪������Ǹ�ʲô�õģ�����IO_GetString����ȡ����ʱ��
//ֻ����"admin"����ȴ���ҷ���" admin"�����Զ��˸��ո�
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
        /* ��������Client�˵�����  */
        if ( CLI_ReceiveFromTerm(m_stCliTerm.ulTimeLeft) != TBS_SUCCESS)
        {
            if (m_stCliTerm.nTermStatus < TERM_LOGED || m_stCliTerm.ulTimeLeft == 0)
                return TBS_FAILED;
            continue ;
        }

        if (!IS_TERM_NORMAL(m_stCliTerm))
            return TBS_FAILED;

        /* �༭���յ����ַ��� */
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
        /* ����������Ϣ                                     */
        /* �������ģʽΪ�ǺŻ��ԣ����Ǻ�������������ַ� */
        if ( bInvisable)
        {
            for (i = 0; i < m_stCliTerm.iSendLen; i++)
            {
                if (m_stCliTerm.szSendBuf[i] < 0x20
                 || m_stCliTerm.szSendBuf[i] > 0x7e)
                {
                    if (m_stCliTerm.szSendBuf[i] == 0)
                        break;
                    //����������ʱ����ַ�����Ʒ���Ψһ�Ŀ����Ծ�������ɾ��"\x1b[1D"
                    //������������Ŀ����ԣ��˴��������޸�
                    if (STB_StrNCmp(&m_stCliTerm.szSendBuf[i], "\x1b[1D", 4) == 0)
                    {
                        i += 3;   //����ֻ����3����Ϊѭ����������һ�ε���
                        if (m_stCliTerm.szSendBuf[i + 1] == 0x20)
                            i++;
                    }
                    else if (STB_StrNCmp(&m_stCliTerm.szSendBuf[i], "\x1b[D", 4) == 0)
                    {
                        i += 3;   //����ֻ����3����Ϊѭ����������һ�ε���
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
            break;    /* ��������ǺŻ������̿�����ɵ���ѭ��: ����֮�ϵ�continue��������ѭ�� */
        }

    }

    STB_StrCpy( szInput, m_stCliTerm.szEditBuf );

    if (!bInvisable )
    {
        STB_StrCpy(m_stCliTerm.szCommandBuf, m_stCliTerm.szEditBuf);
        /*����������Ǳ�"?"�жϵģ�����Ϊ"?"*/
        if ( STB_StrLen(szInput) > 0 && szInput[STB_StrLen ( szInput ) - 1] == '?' )
        {
            szInput[0] = '?';
            szInput[1] = '\0';
        }
    }
    /* �ָ��������ݵı༭״̬����Ӧ������ */
    m_stCliTerm.iEditStatus  = COMMAND_INPUT;
    m_stCliTerm.iMaxLen      = CLI_MAX_CMDBUF;
    m_stCliTerm.szEditBuf[0] = '\0' ;
    m_stCliTerm.iCurrentPos  = 0 ;

    /*����������Ǳ�CTCR+C�жϵģ�����Ϊ��*/
    if ( STB_StrLen(szInput) > 0 && szInput[STB_StrLen ( szInput ) - 1] == KEY_CTRL_Z )
        szInput[0] = '\0';

    return TBS_SUCCESS ;
}

/*-------------------------------------------------------------------*/
/* �������� : IO_GetConfirm                                          */
/* ��    �� : �ȴ��û�����ȷ����Ϣ                                   */
/* ������� : ulInfoID       :����ID                                 */
/*            ulDefaultChoice:ȱʡ����                               */
/* ������� : ��                                                     */
/* ����     : INPUT_NO/INPUT_YES/INPUT_TIMEOUT                       */
/* ����˵�� : �õ��õ����ն�����������ֱ����ʱ����յ���������. ��� */
/*            ����س������ξ�Ϊ'n'��'y'֮�������, ����ȱʡѡ��.    */
/*            ��ʱʱ��ͳһ����Ϊ5��                                  */
/* ����ʾ�� : if (IO_GetConfirm(CLI_KICK_OFF_ANTHOR, INPUT_NO)       */
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

    /* ������������������ṩ���3��������ᣬ֮����ȱʡ���뷵��*/
    for (ulCount = 0; ulCount < 3; ulCount ++)
    {
        IO_Print(szFormat);
        IO_OutToTerm(szPrompt);

        /* �ȴ��ն�����ȷ����Ϣ��5�볬ʱ */
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
                /* �������س�, �򷵻�ȱʡֵ */
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
/* �������� : IO_SetScrollAuto                                       */
/* ��    �� : ���õ�ǰ�ն�Ϊ�Զ�����                                 */
/* ������� : ��                                                     */
/* ������� : ��                                                     */
/* ����     : �ɹ���ʧ��                                             */
/* ����˵�� :                                                        */
/*-------------------------------------------------------------------*/
ULONG  IO_SetScrollAuto(_VOID   )
{
    m_CLIucScroll = m_stCliTerm.ucScroll;
    m_stCliTerm.ucScroll = SCROLL_AUTO;

    return TBS_SUCCESS;
}

/*-------------------------------------------------------------------*/
/* �������� : IO_SetScrollBack                                       */
/* ��    �� : �ָ���ǰ�ն˵Ĺ�����ʽ                                 */
/* ������� : ��                                                     */
/* ������� : ��                                                     */
/* ����     : �ɹ���ʧ��                                             */
/* ����˵�� : �ýӿ�����IO_SetScrollAuto����ʹ��                     */
/*-------------------------------------------------------------------*/
ULONG  IO_SetScrollBack(_VOID   )
{
    m_stCliTerm.ucScroll = m_CLIucScroll;

    return TBS_SUCCESS;
}

/*-------------------------------------------------------------------*/
/* �������� : IO_IsOutputBroken                                      */
/* ��    �� : �жϵ�ǰ������Ƿ���ֹ                               */
/* ������� : ulTermID: �ն�ID                                       */
/* ������� : ��                                                     */
/* ����     : TRUE/FALSE                                             */
/* ����˵�� : �ýӿ�һ�������н϶����������е���                   */
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

