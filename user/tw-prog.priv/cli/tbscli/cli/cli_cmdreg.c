
/****************************************************************************
 �ļ���  : CLI_Cmdreg.c
 ����    : liuzequn
 �汾    :
 �������:
 �ļ�����:  �����е�����������������ע��ӿڵ��ṩ
 ��ע   :
 ��Ҫ�����б�:

�ṩ���ⲿ�Ľӿ�:   ���ļ��ṩ���ⲿ�Ľӿ�
            (1) CLI_CmdAutoRegist

��Ҫ�ⲿ�ṩ�Ľӿ�:    ���ļ���Ҫ�ⲿ�ṩ�Ľӿ�
            (1) ��������ӿ�
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

/* �ӹ���ȫ�ֱ������� */
static UCHAR m_ucPosIndex = 1;   /* �����ľ���λ������ */

ULONG  CLI_CmdRegist(const char  *szCmd,  UCHAR  ucMode,
                CLI_OPERAT_LEVEL_T   ucLevel,  PFUN_CLI_CALLBACK_EXEC pFunc,
                PCSTR  strHelp1, PCSTR  strHelp2, PCSTR  strHelp3)
{
    return CLI_CmdAutoRegist(szCmd, ucMode, ucLevel, pFunc,
                             CLI_MC_NULL, NULL_MODE,
                             strHelp1, strHelp2, strHelp3);
}



/*-------------------------------------------------------------------*/
/* �������� : CLI_CmdAutoRegist                                       */
/* ��    �� : �����Զ�ע�ắ���ӿ�                                   */
/* ������� : ����˵������������                                     */
/* ������� : ��                                                     */
/* ����     : ע��ɹ�/ʧ��                                          */
/* ����˵�� : �������Լ���֤�����﷨����ȷ��,��ϸ˵�����Բμ�������  */
/*            ˵���ĵ�. ע�����ʽ������,��ȷ��������Ŀ϶���ʽ��  */
/*            ��ע��,��ע��no timeout����֮ǰӦ���Ѿ�ע��timeout���� */
/* ����ʾ�� : CLI_CmdAutoRegist("cmdtest user-test: <type(u1~3)>",    */
/*             CTM_DBG, CLI_AL_OPERATOR,  CLI_CmdTest4, CLI_MC_NULL,   */
/*             NULL_MODE, CLI_ML_NULL,  CLI_ML_NULL,  CLI_ML_NULL ); */
/*-------------------------------------------------------------------*/
ULONG  CLI_CmdAutoRegist
              (const char  *szCmd,              //�����ʽ����
               UCHAR  ucMode,                   //��������ģʽ
               CLI_OPERAT_LEVEL_T   ucLevel,    //����ʹ�õȼ�
               PFUN_CLI_CALLBACK_EXEC pFunc,    //����ִ�к���
               UCHAR  ucModeChange,             //ģʽת������(��ģʽת��������CLI_MC_NULL)
               UCHAR  ucNewMode,                //��ģʽ��ID  (������һ����ΪCLI_MC_NEWʱ������ )
               PCSTR  strHelp1,                 //��һ������İ�����Ϣ
               PCSTR  strHelp2,                 //�ڶ�������İ�����Ϣ
               PCSTR  strHelp3)
{
    char  szCmdString[CLI_MAX_CMD_FORMAT + 1];  /* ����ı��ʽ���Գ��������ʵ�ʳ���256,�ݶ�����Ϊ1000*/
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

    /* DEBUGģʽ������ֻ���ڵ��԰汾�д��� */
#if !CLI_DEBUG_ON
    if (ucMode == CTM_DBG)
        return TBS_SUCCESS;
#endif

    STB_StrCpy(szCmdString, szCmd);
    (_VOID   )_AffirmStrBeLower(szCmdString);

    /* �����ע���﷨��� */
    if (CLI_IsCmdValid(szCmdString) != TBS_SUCCESS)
    {
        REG_OUT("\r\n CLI: �����ʽ����ȷ:%s",szCmd);
        DBG_ASSERT(0);
        return TBS_FAILED;
    }

    if (STB_StrNiCmp(szCmdString, "show ", 5) == 0)
    {
        ucMode = CTM_GLOBAL;
    }

    /* ���������Ϸ��Լ�� */
    if (ucMode >= MAX_MODE_NUM || ucNewMode >= MAX_MODE_NUM || ucLevel > CLI_AL_DEBUG)
    {
        REG_OUT("\r\n CLI: ע��������ڲ��Ϸ�����:mode-%d, new mode-%d, level-%d", ucMode, ucNewMode, ucLevel);
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
    /* ����Ƿ���ʽ���������������Ӧ���Ѿ�������϶���ʽ��
       ��˲����ٴ������󣬶���ȡ�����������ʽ */
    if (STB_StrNCmp("no ", pCurPos, 3) == 0)
    {
        ucIsNoForm = 1;
        pCurPos   += 3;    //����"no "
        if ((sMode = CLI_GetMode(ucMode)) == NULL)
        {
            REG_OUT("\r\n CLI: ������Ŀ϶���ʽ��δע�� : %s", szCmd);
            DBG_ASSERT(0);
            return TBS_FAILED;
        }
        pObj = sMode->pCmdKey;   /* ��ģʽ����ʼ���� */
    }

    /* ע����� */
    while((ulRet = CLI_GetObj(pCurPos, szToken)) != TBS_FAILED)
    {
        if (ulRet == TOKEN_GET_ONLYSPACE)
        {
            pCurPos++;
            continue;
        }
        if (ulRet == TOKEN_GET_CMDNULL)
        {
            REG_OUT("\r\n CLI: ��:%s �л�ȡ������ʧ�� : %s", pCurPos, szCmd);
            DBG_ASSERT(0);
            return TBS_FAILED;
        }

        pCurPos += STB_StrLen(szToken);

        /* ע��3�����϶��������ɹ� */
        if (ulIndex > 2)
        {
            REG_OUT("\r\n  CLI: �ؼ������ֹ��� : %s!", szCmd);
            DBG_ASSERT(0);
            return TBS_FAILED;
        }

        if (STB_StrLen(szToken) >= MAX_TOKEN_SIZE)
        {
            REG_OUT("\r\n  CLI: ����������ע�� : %s!", szToken);
            DBG_ASSERT(0);
            return TBS_FAILED;
        }

        /* �϶���ʽ�Ķ����Զ�ע�� */
        if (ucIsNoForm == 0)
        {
            if (ulRet == TOKEN_GET_LASTOBJ)
            {
                /* ���һ�����������Ҫ�������������: �ص�������ģʽת�� */
                switch (ucModeChange)
                {
                    case CLI_MC_NULL:
                        /* ���ڷ�ģʽת����������Ҫ��֤���ص����� */
                        if (pFunc == NULL)
                        {
                            REG_OUT("\r\n CLI: ����(%s)\r\n      �Ķ���(%s)û��ִ�в���!", szCmd, szToken);
                            DBG_ASSERT(0);
                            return TBS_FAILED;
                        }
                        pCmdKey[ulIndex] = CLI_CreateObject(szToken, ucMode, ucLevel,
                                        pFunc, FALSE, CLI_MC_NULL, NULL);
                        break;
                    /* �����ǶԸ���ģʽת�����͵Ķ���ע�� */
                    case CLI_MC_END:
                        pCmdKey[ulIndex] = CLI_CreateObject(szToken, ucMode, ucLevel,
                                        pFunc, TRUE, CLI_MC_END, NULL);
                        break;
                    case CLI_MC_EXIT:
                        pCmdKey[ulIndex] = CLI_CreateObject(szToken, ucMode, ucLevel,
                                        pFunc, TRUE, CLI_MC_EXIT, NULL);
                        break;
                    case CLI_MC_NEW:
                        /* �����ת������ģʽ����Ҫ��֤��ģʽ����ȷ�� */
                        if (ucNewMode == NULL_MODE)
                        {
                            REG_OUT("\r\n CLI: ģʽת������ע�����: ��ģʽID: %d", ucNewMode);
                            DBG_ASSERT(0);
                            return TBS_FAILED;
                        }

                        pCmdKey[ulIndex] = CLI_CreateObject(szToken, ucMode, ucLevel,
                                        pFunc, TRUE, CLI_MC_NEW, NULL);

                        /* �����ת������ģʽ������Ҫ���ö���ע��Ϊ��ģʽ��ģʽ����*/
                        if (*pCurPos == ':' || *(pCurPos + 1) == ':')  /*������Ҫ�Ľ�*/
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
                REG_OUT("\r\n CLI: ���󴴽�ʧ��: %s", szCmd);
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
                    REG_OUT("\r\n CLI: ����(%s)\r\n      �Ķ���(%s) ȱ�ٰ�����Ϣ!", szCmd, szToken);
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
                        REG_OUT("\r\n CLI: ����(%s)\r\n      �Ķ���(%s) ������Ϣ��ʽ����ȷ!", szCmd, szToken);
                        DBG_ASSERT(0);
                        return TBS_FAILED;
                    }
                }
            }
            CLI_RegCmdHelp(pCmdKey[ulIndex], strObjHelp[ulIndex]);

        }

        /* ����ʽ������ע��: �Զ����ҿ϶���ʽ������޸������� */
        /* ʹ֮��������ķ���ʽ                                 */
        else
        {
            if (ulRet == TOKEN_GET_LASTOBJ)
            {
                pObj = CLI_GetSubObjByName(pObj, szToken);

                if (pObj == NULL)
                {
                    REG_OUT("\r\n CLI: ������Ŀ϶���ʽ��δע�� : %s", szCmd);
                    DBG_ASSERT(0);
                    return TBS_FAILED;
                }

                pParamLink = &(pObj->noAction.pParamLink);
                /* ���øö���ķ����� */
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
                    REG_OUT("\r\n CLI: ������Ŀ϶���ʽ��δע�� : %s", szCmd);
                    DBG_ASSERT(0);
                    return TBS_FAILED;
                }
                pObj->ulNoFlag = 1;
            }
        }

        ulIndex++;

        /* ����Ѿ������һ������������ѭ�� */
        if (ulRet == TOKEN_GET_LASTOBJ)
            break;
    }

    /* ע�����ģʽ����Ҫ�����ж��󴴽����֮�����, */
    /* �����޷�ע�������ͬ��������                  */
    if (ucIsNoForm == 0)
    {
        if (CLI_RegObjToMode(ucMode, pFirstObj))
        {
            REG_OUT( "\r\n CLI: ����ע��ʧ��: %s", szCmd);
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

    /* ���������ʾ�ָ��������Ѿ�û�в�������ֱ�ӷ��� */
    if (*pCurPos == '\0')
    {
        return TBS_SUCCESS;
    }

    if (pParamLink == NULL)   /* ���жϲ������������pc-lint�澯 */
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    /*ע����������ֵ*/
    *pParamLink = CLI_CreatGroup(pCurPos);
    if (*pParamLink == NULL)
    {
        REG_OUT("\r\n CLI: ����(%s)����Ⱥע��ʧ��!", szCmd);
        DBG_ASSERT(0);
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}


/*********************************************************************/
/* �������� : CLI_IsCmdValid()                                        */
/* �������� : �����ʽ�Ϸ��Եļ򵥼�麯��                           */
/* ������� : szCmd �����ʽ��                                       */
/* ������� : ��                                                     */
/* ����     : �ɹ���ʧ��                                             */
/* �ϲ㺯�� : CLI_CmdAutoRegist                                       */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
/*********************************************************************/
ULONG  CLI_IsCmdValid(char  * szCmd)
{
    ULONG  ulPos, ulStrLen;
    UCHAR ucBig = 0, ucMid = 0, ucSmall = 0, ucNeedle = 0; //��ʼ�����С�С�������ŵ�δƥ�����
    UCHAR ucIsOnlyObj = 1;   //�Ƿ�ֻ���ֶ��������֣�Ĭ��Ϊ��(1)
    char  cTmp;

    ulStrLen = STB_StrLen(szCmd);
    if (szCmd == NULL|| (ulStrLen  > CLI_MAX_CMD_FORMAT) || ulStrLen == 0)
    {
        return TBS_FAILED;
    }

    for (ulPos = 0; ulPos < ulStrLen; ulPos ++)
    {
        cTmp = *(szCmd + ulPos);
        //�ո���Сд��ĸ������Ϊ��ȫ�Ϸ�������
        if ((cTmp >= 'a' && cTmp <= 'z')
         || (cTmp >= '0' && cTmp <= '9')
         || (cTmp == ' ' || cTmp == '-')
         ||  cTmp == '.' || cTmp == ',' 
         || cTmp == '_')
            continue;

        else if(cTmp != ':' && ucIsOnlyObj == 1) //���Ŀǰֻ���ֶ�������ֽṹ�ַ����Ƿ�
            return TBS_FAILED;

        switch(cTmp)
        {
            case ':':
                if (ucIsOnlyObj == 0)
                    return TBS_FAILED;  //������ֶ��':'���Ƿ�
                ucIsOnlyObj = 0;
                break;
            /* ���¼��鼸��������ƥ����� */
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
            case '~':   /*���ֵ����ų����� ����ֵ���Է�Χ֮�⣬�Ƿ� */
                if(ucSmall == 0)
                    return TBS_FAILED;
                break;
            case '/':
                if(ucNeedle == 0) /* ���ȱʡֵ�����ڲ���ֵ��Χ֮�⣬�Ƿ�*/
                    return TBS_FAILED;
                break;
            case '|':
                break;
            default:
                return TBS_FAILED;
        }
    }
    if ((ucBig | ucSmall | ucMid | ucNeedle) != 0) /* �����һ������ƥ�䲻��ȫ���Ƿ� */
        return TBS_FAILED;
    return TBS_SUCCESS;
}

/*********************************************************************/
/* �������� : CLI_GetSubObjByName()                                   */
/* �������� : ���ݶ�������ȡָ��������Ӷ���                         */
/* ������� : pCmdKey ָ������                                       */
/*            szName  ��Ҫ��ȡ�Ķ���Ķ�����                         */
/* ������� : ��                                                     */
/* ����     : ��ȡ�Ķ���ָ��                                         */
/* �ϲ㺯�� : CLI_CmdAutoRegist                                       */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
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

/* ���в��������ɺ����������������п��Է��ز�����ָ��Ȼ��������ֹ����� */
/* �Ķ���ָ�������Ϳ���Ϊ���������ͬ�������Ķ��������� */
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

    /* ����ע���״̬�ṹջ�����ڼ�¼��ǰע���Ԫ�������ĸ�������״̬      */
    /* ע���������ͷŵ�ƥ��, �����ʧ�ܲ�����û���ͷŸ��ڴ棬������ʱ���ԣ�*/
    /* ��Ϊ�����������ϵͳ��ʼ��ʧ��, �Ѿ�������������                    */
    pPointStackCur = CLI_NewPointStack();
    pPointStackCur->pParamLink = CLI_CreateParamLink();
    if (pPointStackCur->pParamLink == NULL)
    {
        REG_OUT("\r\n CLI: ����������ʧ��: %s", szParmGroup);
        return NULL;
    }

    szLeft = szParmGroup;
    ucState = 0;
    m_ucPosIndex = 1;

    for (; *szLeft != '\0'; )
    {
        /* ��ȡ��ǰ������Ԫ�� */
        ulRet = CLI_GetRegistToken(szLeft, szToken);
        if (ulRet == TOKEN_GET_CMDNULL)
            break;
        if (STB_StrLen(szToken) >= CLI_MAX_PARAMLEN)
        {
            REG_OUT("\r\n CLI: ����������ע�� : %s!", szToken);
            return NULL;
        }
        switch(ulRet)
        {
            case TOKEN_GET_ONLYSPACE:
                szLeft++;
                break;   //������ط��ֵ�ǰ��Ϊ�ո�ֱ������

            /* �Կո��������� */
            case TOKEN_GET_SPACEEND:
                if(*szToken != 0)
                {
                    szLeft += STB_StrLen(szToken);
                    CLI_SetProperty(&ucState, CLI_PRMPROP_NAME);
                    if (CLI_AutoCreatParam(pPointStackCur, szToken, &ucState) == NULL)
                    {
                        REG_OUT("\r\n!!!��������ʧ��: %s", szToken);
                        return NULL;
                    }
                }
                else
                    szLeft++;
                break;

            /* ����������ʾ������Ŀ�ʼ */
            case TOKEN_GET_MUTEXBEGIN:
                szLeft++;  //�˴������break
            case TOKEN_GET_GROUPBEGIN:
                if((pcTmp= CLI_GetEndSignal(szLeft)) == 0) //��ȡ��������λ��
                    return NULL;
                /* �����飬��Ҫ�ȴ���һ���µĻ��������ṹջ */
                pPointStackTmp = pPointStackCur;
                pPointStackCur = CLI_NewPointStack();
                pPointStackCur->Number = pPointStackTmp->Number;
                pPointStackCur->pPre = pPointStackTmp;
                pPointStackCur->pParamLink = pPointStackTmp->pParamLink;
                pPointStackCur->pParam = pPointStackTmp->pParam;

                //�˴�Ҫ�����õ�һ��ΪNEEDNAME���ԣ���Ϊ����{}�еĵ�һ����������ΪNEEDNAME����
                //�������������������������µĿ�����
                CLI_SetProperty(&ucState, CLI_PRMPROP_NAME);
                /* ���ǰ������ò�������֮���������л�����ţ�����ʶ�ò���Ϊ��������*/
                if(*(pcTmp + 1) == '|' || (pPointStackCur->pPre->ucMutexIndex != 0))
                {
                    CLI_SetProperty(&ucState, CLI_PRMPROP_MUTEX | CLI_PRMPROP_NAME);

                    if(pPointStackCur->pPre->ucMutexIndex != 0)
                    {   //����Ǽ̳��ϴεĻ�����������λ������Ӧ����һ������һ��
                        m_ucPosIndex = pPointStackCur->pPre->ucMutexIndex;
                    }
                    else
                    {
                        pPointStackCur->pPre->ucMutexIndex = m_ucPosIndex;
                    }

                    if (*(pcTmp + 1) != '|')  //����ò�����֮��Ĳ��������⣬�򽫻���λ�����
                        pPointStackCur->pPre->ucMutexIndex = 0;
                }

                szLeft++;
                /* �������пո�*/
                while (CLI_GetRegistToken(szLeft, szToken) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                /* ���������֮��ĵ�һ��Ԫ�ز�����Ҫ�������Ĳ��������� */
                if (*szToken == 0)
                {
                    REG_OUT("\r\n CLI: Error in :%s", szParmGroup);
                    return NULL;
                }
                szLeft += STB_StrLen(szToken);
                pPointStackCur->pParam = CLI_AutoCreatParam(pPointStackCur, szToken, &ucState);
                if (pPointStackCur->pParam == NULL)
                {
                    REG_OUT("\r\n CLI: ��������ʧ�� :%s", szToken);
                    return NULL;
                }
                break;

            /* ����������ѡ������Ŀ�ʼ */
            case TOKEN_GET_OPTIONBGIN:
                if((pcTmp= CLI_GetEndSignal(szLeft)) == 0) //��ȡ��������λ��
                    return NULL;
                szLeft++;
                /* �����飬�ȴ���һ���µ�״̬�ṹջ */
                pPointStackTmp = pPointStackCur;
                pPointStackCur             = CLI_NewPointStack();
                pPointStackCur->Number     = pPointStackTmp->Number;
                pPointStackCur->pParamLink = pPointStackTmp->pParamLink;
                pPointStackCur->pPre       = pPointStackTmp;
                pPointStackCur->pParam     = pPointStackTmp->pParam;

                /* ���������������: ��Ҫ����������ѡ�������ɵ��� */
                CLI_SetProperty(&ucState, CLI_PRMPROP_NAME | CLI_PRMPROP_OPTIONAL | CLI_PRMPROP_INDEX);

                /* �������еĿո� */
                while (CLI_GetRegistToken(szLeft, szToken) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                if (*szToken == 0)
                    return NULL;
                szLeft += STB_StrLen(szToken);
                pPointStackCur->pParam = CLI_AutoCreatParam(pPointStackCur, szToken, &ucState);
                if (pPointStackCur->pParam == NULL)
                {
                    REG_OUT("\r\n CLI: ��������ʧ��:%s", szToken);
                    return NULL;
                }
                break;

            /* ��������� */
            case TOKEN_GET_OPTIONEND:
            case TOKEN_GET_GROUPEND:
                if (*szToken != 0)
                {
                    szLeft += STB_StrLen(szToken);
                    CLI_SetProperty(&ucState, CLI_PRMPROP_NAME);
                    CLI_AutoCreatParam(pPointStackCur, szToken, &ucState);
                }
                szLeft++;
                /* ����״̬ջ, �ͷ�״̬��Դ */
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

            /* �������, ����ֵ��ʼ */
            case TOKEN_GET_VALBEGIN:
                if(CLI_GetEndSignal(szLeft) == 0) //��ȡ��������λ��
                    return NULL;
                szLeft++;
                /* �������пո� */
                while (CLI_GetRegistToken(szLeft, szToken) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                if (*szToken == 0)
                {
                    REG_OUT("\r\n CLI: Cannot get value!");
                    return NULL;
                }
                szLeft += STB_StrLen(szToken);
                STB_StrCpy(szDesc, szToken);

                /* ����Ϊ����ֵ�Ƿ�������Ե��ж�*/
                while ((ulRet = CLI_GetRegistToken(szLeft, szToken)) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                if (ulRet != TOKEN_GET_PROPBEGIN)
                {
                    REG_OUT("\r\n CLI: ����ֵȱ������: %s", szToken);
                    return NULL;
                }

                break;

            /* ��Բ����, �������Կ�ʼ */
            case TOKEN_GET_PROPBEGIN:
                szLeft++;
                /* �������пո� */
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

            /* б�ܣ�ȱʡֵ���ÿ�ʼ��־ */
            case TOKEN_GET_DEFAULT:
                szLeft++;
                if (*szToken != 0)
                {
                    REG_OUT("\r\n CLI: Default Value position error!(%s)", szToken);
                    return NULL;
                }
                /* �������пո� */
                while (CLI_GetRegistToken(szLeft, szToken) == TOKEN_GET_ONLYSPACE)
                    szLeft++;
                if (*szToken == 0 || *szDesc == 0)
                {
                    REG_OUT("\r\n CLI: No default of value:%s!", szDesc);
                    return NULL;
                }
                szLeft += STB_StrLen(szToken);

                /* ������ⶨ���ֵΪ�ò���ֵ�ĺϷ����ԣ�����Ϊȱʡֵ */
                if (TBS_SUCCESS == CLI_CheckForValue(pPointStackCur->pValue, szToken))
                {
                    if (CLI_SetValueDefault(pPointStackCur->pValue, szToken) != TBS_SUCCESS)
                    {
                        REG_OUT("\r\n CLI: Set default for %s failed!", szToken);
                        return NULL;
                    }
                }
                /* ��������Ϊ�ò���ֵ�����д� */
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
                szLeft++;    //�˴������break
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
/* �������� : CLI_GetObj()                                            */
/* �������� : �Ӹ�ʽ�����л�ȡ������                                 */
/* ������� : pCurPos ��ʽ�����ĵ�ǰλ��                             */
/* ������� : szObj   ����ȡ�Ķ�����                                 */
/* ����     : �ɹ���ʧ��                                             */
/* �ϲ㺯�� :                                                        */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
/*********************************************************************/
ULONG  CLI_GetObj(char  *pCurPos, char  *szObj)
{
    char  *pTmp;

    pTmp = pCurPos;

    /* ������׼�Ϊ������, ���ؿ� */
    if(szObj == NULL || pTmp == NULL || *pTmp == 0 || *pTmp == ':')
        return TOKEN_GET_CMDNULL;

    memset(szObj, 0, MAX_TOKEN_SIZE);

    if (*pTmp == KEY_SPACE)
        return TOKEN_GET_ONLYSPACE;

    for(; *pTmp != '\0' && *pTmp != KEY_SPACE; pTmp++)
    {
        /* ����Զ������������, ��ʾ�Ѿ��ﵽ���һ������ */
        if(*pTmp == ':')
            return TOKEN_GET_LASTOBJ;
        *szObj++ = *pTmp;
    }
    while (*pTmp == KEY_SPACE)
        pTmp++;

    /* �����ȡ��һ�����ǿո���ַ�Ϊ�ջ�Ϊ':',��ʾ��ǰ��ȡ��Ϊ���һ������ */
    if (*pTmp == '\0' || *pTmp == ':')
        return TOKEN_GET_LASTOBJ;

    return TOKEN_GET_SPACEEND;
}

/*********************************************************************/
/* �������� : CLI_GetRegistToken()                                    */
/* �������� : ��ȡ��ǰλ�õ�������                                   */
/* ������� : szSrc ��ʽ����λ��                                     */
/* ������� : szDrc ����ȡ��������                                   */
/* ����     : ��ȡ�����������ķָ�����                               */
/* �ϲ㺯�� :                                                        */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
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
            /* �������п����Ե��ַ�,������Ӧ����Ϣ */
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
/*���õ�ǰ������ĳ����*/
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
/* �������� : CLI_GetEndSignal()                                      */
/* �������� : ��ȡ�뵱ǰ����ƥ��ķ���(��ҪΪ{} [] <>��ƥ��)         */
/* ������� : ��ǰλ��                                               */
/* ������� :                                                        */
/* ����     : �õ���ƥ��ķ���λ��                                   */
/* �ϲ㺯�� :                                                        */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
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
/* �������� : CLI_GetValType()                                        */
/* �������� : ����ֵ�����Ի�ȡ                                       */
/* ������� : szSrc ����ֵ���ַ�����ʽ                               */
/* ������� : ucType   ����ֵ����                                    */
/*            piLow    ����ֵ����                                    */
/*            piHigh   ����ֵ����                                    */
/*            piStrLen ����ֵ����                                    */
/* ����     : ��                                                     */
/* �ϲ㺯�� :                                                        */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
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

            /* ����������ת��Ϊ���͵�������, ת��ʧ������ΪĬ��ȡֵ */
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

            /* ����������ת��Ϊ���͵�������, ת��ʧ������ΪĬ��ȡֵ */
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
            /* �����������ַ�������󳤶�, ת��ʧ������ΪĬ��ȡֵ 80 */
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

/*״̬�ṹ��ʼ������ */
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
/* �������� : CLI_AutoCreatVal()                                      */
/* �������� : ����ֵ���Զ�ע��                                       */
/* ������� : pPointStack ע��Ļ����ṹ                             */
/*            szName      ����ֵ��                                   */
/*            ucStat      ��ǰ״̬                                   */
/*            szType      ����ֵ����                                 */
/* ������� : ��                                                     */
/* ����     : �Զ�ע��Ĳ���ֵ�ṹָ��                               */
/* �ϲ㺯�� :                                                        */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
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
    //���ǰһ�������Ѿ����ֵܲ������Ӳ����ˣ���ǰ����ֵ����ֱ��
    //�ò����£���Ϊͬ��λ���½��͵����ȼ�Ϊ ����ֵ > �Ӳ��� > �ֵܲ���
    //���ֱ��ע�ᣬ��ò���ֵ�����ڴ�ǰע����������������ͣ����Ӧ�����·�֧
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
/* �������� : CLI_GetRangeStr()                                      */
/* �������� : ���ַ����ڻ�ȡ����ֵ��������                           */
/* ������� : szSrc  ����ַ���                                      */
/* ������� : szHigh ��ʾ���޵��ַ���                                */
/*            szLow  ��ʾ���޵��ַ���                                */
/* ����     : �ɹ���ʧ��                                             */
/* �ϲ㺯�� :                                                        */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
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
/* �������� : CLI_CheckForValue()                                     */
/* �������� : ���ֵ�Ƿ񸴺ϲ���ֵ������                             */
/* ������� : pValue  ����ֵ                                         */
/*            szToken �����Ĳ���ֵ                                 */
/* ����     : �ɹ���ʧ��                                             */
/* �ϲ㺯�� :                                                        */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
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

//�Զ�ע�����  }}}}}***************************************

#ifdef __cplusplus
}
#endif

