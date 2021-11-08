/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 �ļ����� : cli_comm.c
 �ļ����� : �����н��̵�ͨ��ģ�鴦��


 �����б� :


 �޶���¼ :
          1 ���� : liuzequn
            ���� : 2007-11-6
            ���� :

**********************************************************************/

#include <crypt.h>
#include "../cli_comm.h"
#include "../include/tbsutil.h"

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/
/*
 *�ú����MSG_CMM_GET_VAL_ACK���͵���Ϣ������
 *�Ӹ�������Ϣ�е�һ��item����ȡ��ֵ.
 *MSG_CMM_GET_VAL_ACK���͵���Ϣ������鿴WIKI
 *�е�<TBS R2 ��Ϣ��ʽ>
 */
#define CLI_GET_LEAF_VALUE(pcPointer, item) do \
{ \
    (pcPointer) = strchr((item), '='); \
    if ((pcPointer) == NULL) \
    { \
        IO_Print("Message format error!!"); \
        CLI_MSG_RELEASE(g_pstCliMsg); \
        return TBS_FAILED; \
    } \
    (pcPointer)++; \
}while(0);

/*
 *����Ҳ�����MSG_CMM_GET_VAL_ACK��Ϣ
 *ָ���Զ��屾�꣬�����ӿɶ���
 */
#define CLI_MOVE2_NEXT_ITEM(curItemStart) do \
{ \
    (curItemStart) += STB_StrLen((curItemStart)) + 1; \
}while(0);

/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/



/******************************************************************************
 *                                GLOBAL                                      *
 ******************************************************************************/

static char     g_szCliFeedbackError[1024] = "";    // ������Ϣ
static ST_MSG  *g_pstCliMsg = NULL;
//�û��б�
static struct stUserInfoList g_stUserList;


/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/
static int CLI_StoreUserInfo(void);
static ULONG CLI_ConfigLever2CliLevel(ULONG ulConfigLevel);


/*************************************************************************
����:���������ж�ȡ�û���Ϣ
����:��
����:
     TBS_SUCCESS:��ȡ��Ϣ�ɹ�
     TBS_FAILED :��ȡ��Ϣʧ��
��ע:ʹ����ȫ�ֱ���g_pstCliMsg
 *************************************************************************/
int CLI_GetUserInfo(void)
{
    ST_MSG *pstMsg;      /* ���͵���Ϣָ�� */
    char   szPath[] = "InternetGatewayDevice.X_TWSZ-COM_Authentication.UserList.";
    int    iRet = TBS_FAILED; 
    USHORT usLen = strlen(szPath);

    //������Ϣ��ռ�
    // ׼ȷӦ���� sizeof(ULONG) + usLen + '\0'
    pstMsg = MSG_CreateMessage(usLen + 8);
    if (pstMsg == NULL)
    {
        IO_Print("Out of Memory");
        return TBS_FAILED;
    }

    //������Ϣ
    pstMsg->stMsgHead.ulBodyLength = strlen(szPath) + 1 + sizeof(ULONG);
    *(ULONG *)pstMsg->szMsgBody = 1;
    strcpy(pstMsg->szMsgBody+ sizeof(ULONG), szPath);
    pstMsg->stMsgHead.ulMsgID   = 0;
    pstMsg->stMsgHead.usSrcMID  = MID_CLI;
    pstMsg->stMsgHead.usDstMID  = MID_CMM;
    pstMsg->stMsgHead.usMsgType = MSG_CMM_GET_VAL;

    //������Ϣ
    if (OM_FAILED(MSG_SendMessage(pstMsg)))
    {
        IO_Print("Send message to CMM failed");
        CLI_MSG_RELEASE(pstMsg);
        return TBS_FAILED;
    }

    //�ͷ�������ڴ�
    CLI_MSG_RELEASE(pstMsg);

    //ÿ�ε���MSG_ReceiveMessageǰ���ͷ�һ��
    CLI_MSG_RELEASE(g_pstCliMsg);

    //������Ϣ
    if (OM_FAILED(MSG_ReceiveMessage(&g_pstCliMsg, CLI_COMM_TIMEOUT)))
    {
        IO_Print("Wait message failed");
        return TBS_FAILED;
    }
    if (g_pstCliMsg->stMsgHead.usMsgType != MSG_CMM_GET_VAL_ACK)
    {
        IO_Print("Received unrespected message");
        return TBS_FAILED;
    }

    //������Ϣ
    iRet = CLI_StoreUserInfo();
    
    return iRet;
}

/*************************************************************************
����:����MSG_CMM_GET_VAL_ACK��Ϣ���Ѵ���������ȡ��������Ϣ�洢��
     g_stUserList��
����:��
����:
     TBS_SUCCESS:��Ϣ����ɹ�
     TBS_FAILED :��Ϣ����ʧ��
��ע:ʹ����ȫ�ֱ���g_pstCliMsg,Ҫ��Ȿ��������ο�MSG_CMM_GET_VAL_ACK
     ��Ϣ����
 *************************************************************************/
static int CLI_StoreUserInfo(void)
{
    ST_MSG *pstMsg = g_pstCliMsg;
    ULONG  ulRet = 0;
    ULONG  ulCount = 0;
    ULONG  ulTemp  = 0;
    int    iIndex  = 0;
    char   *pcCurItem = NULL;
    char   *pcTempPos1= NULL;

    //�ж�GET�����Ƿ�ɹ�
    ulRet = *(ULONG *)pstMsg->szMsgBody;
    if (ulRet != 0)
    {
        IO_Print("Get User info failed");
        return TBS_FAILED;
    }

    //�жϻ�ȡ�������ǲ��ǺϷ�������ֻ�ж��Ƿ�Ϊ0��0�򲻺Ϸ�
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount == 0)
    {
        IO_Print("No item getted!");
        return TBS_FAILED;
    }
    /*��Ϊһ���û���Ϣʵ����Ҷ�ڵ���3��:username,password,level
     *����ulCount������3�ı���
     */
    else if ((ulCount%3) != 0)
    {
        //IO_Print("Error item count!");
        //return TBS_FAILED;
    }
    
    /*�����Ϊ�û���̫����޷�ȫ�����棬������ʾ,�����÷���
     *��Ϊÿ���û���Ӧ3�����Ҫ����3
     */
    if ((ulCount > (MAX_USER_COUNT*3)))
    {
        IO_Print("Too many items getted, some user info may lost!");
    }

    //����Result��ResItemCount
    pcCurItem = pstMsg->szMsgBody + sizeof(ULONG)*2;

    //�������洢�û���Ϣ
    g_stUserList.iUserCount = 0;
    for (ulTemp = 0; ulTemp < ulCount; )
    {
        iIndex = g_stUserList.iUserCount;
       
        if (strstr(pcCurItem, ".UserName"))
        {
            //�洢�û���
            CLI_GET_LEAF_VALUE(pcTempPos1, pcCurItem);
            STB_StrNCpy(g_stUserList.arrayUserInfo[iIndex].szUsername, pcTempPos1, 31);
            CLI_MOVE2_NEXT_ITEM(pcCurItem);
            ulTemp++;
        }

        if (strstr(pcCurItem, ".Password"))
        {
            //�洢����
            CLI_GET_LEAF_VALUE(pcTempPos1, pcCurItem);
            STB_StrNCpy(g_stUserList.arrayUserInfo[iIndex].szPassword, pcTempPos1, 31);
            CLI_MOVE2_NEXT_ITEM(pcCurItem);
            ulTemp++;
        }

        if (strstr(pcCurItem, ".Level"))
        {
            //�洢�û��ȼ�
            CLI_GET_LEAF_VALUE(pcTempPos1, pcCurItem);
            g_stUserList.arrayUserInfo[iIndex].ulLevel= strtoul(pcTempPos1, 0, 0);
            CLI_MOVE2_NEXT_ITEM(pcCurItem);
            ulTemp++;
        }

        //�û�����1
        g_stUserList.iUserCount++; 

        //�ж��Ƿ񳬹���������û���
        if (g_stUserList.iUserCount >= MAX_USER_COUNT)
        {
            break;
        }
    }

    return TBS_SUCCESS;
}
    

/*************************************************************************
����:�ж��û����������Ƿ���ȷ�������ȷ��������Ӧ��Ϣ
����:szUsername:�û���
     szPassword:����
����:TBS_FAILED:��Ϣ����ʧ��
     ���ڵ���0 :���û�����ִ�еȼ�
��ע:
 *************************************************************************/
ULONG CLI_Authenticate(char *szUsername, char *szPassword, ULONG *ulIndex)
{
#if 0
    int iUserCount = g_stUserList.iUserCount;
    int iTemp      = 0;

    //�û�������Ϊ�գ������������
    if (szUsername == NULL)
    {
        IO_Print("CLI_Authenticate:Error input!");
        return TBS_FAILED;
    }

    for (iTemp = 0; iTemp < iUserCount; iTemp++)
    {
        //�Ƚ��û���
        if (STB_StrCmp(szUsername, g_stUserList.arrayUserInfo[iTemp].szUsername) == 0)
        {
            //����Ϊ�յ����
            if ((szPassword == NULL || STB_StrLen(szPassword) == 0 )
                && (STB_StrLen(g_stUserList.arrayUserInfo[iTemp].szPassword) == 0))
            {
                //��֤ͨ��
                return CLI_ConfigLever2CliLevel(g_stUserList.arrayUserInfo[iTemp].ulLevel);
            }
            //���벻Ϊ�յ����
            else if(STB_StrCmp(szPassword, crypt(g_stUserList.arrayUserInfo[iTemp].szPassword, AUTH_KEY)) == 0)
            {
                //��֤ͨ��
                return CLI_ConfigLever2CliLevel(g_stUserList.arrayUserInfo[iTemp].ulLevel);
            }
        }
    }
#else
    //����MSG_AUTH��Ϣ����AUTHģ����֤
    
    ST_MSG *pstMsg;                 /* ���͵���Ϣָ�� */
    int    iRet = TBS_SUCCESS;
    char    aszAuthInfo[4][128];
    char    *pPos = NULL;
    USHORT usLen = 0;
    ULONG  ulResult = 1;
    ULONG  ulLevel = 0;

    //�û�������Ϊ�գ������������
    if (szUsername == NULL)
    {
        IO_Print("CLI_Authenticate:Error input!");
        return TBS_FAILED;
    }

    //�����1��ʾ������ռһ�ֽ�
    usLen += sprintf(aszAuthInfo[0], "username=%s", szUsername) + 1;
    usLen += sprintf(aszAuthInfo[1], "password=%s", szPassword) + 1;
    usLen += sprintf(aszAuthInfo[2], "action=%s", "login") + 1;
    //���session id�����д�ģ������session��ͻ�ĸ����ǰ����֮һ
    usLen += sprintf(aszAuthInfo[3], "sessionid=%s", "11111111") + 1;
    
    /*------------������Ϣ��ռ�--------*/
    // ׼ȷӦ���� sizeof(ULONG) + usLen
    pstMsg = MSG_CreateMessage(usLen + sizeof(ULONG));
    if (pstMsg == NULL)
    {
        IO_Print("Out of Memory");
        return TBS_FAILED;
    }

    /*------------������Ϣ-----------*/
    /*
     *�ڿ���Ϣ����ǰ����ο�MSG_AUTH����Ϣ�ṹ
     */
    pstMsg->stMsgHead.ulBodyLength = usLen + sizeof(ULONG);
    //4��
    *(ULONG *)pstMsg->szMsgBody = (ULONG)4;
    pPos = pstMsg->szMsgBody + sizeof(ULONG);

    //��ӵ�һ��
    STB_StrCpy(pPos, aszAuthInfo[0]);
    pPos += (STB_StrLen(aszAuthInfo[0]) + 1);

    //��ӵڶ���
    STB_StrCpy(pPos, aszAuthInfo[1]);
    pPos += (STB_StrLen(aszAuthInfo[1]) + 1);

    //��ӵ�����
    STB_StrCpy(pPos, aszAuthInfo[2]);
    pPos += (STB_StrLen(aszAuthInfo[2]) + 1);

    //��ӵ�����
    STB_StrCpy(pPos, aszAuthInfo[3]);
    pPos += (STB_StrLen(aszAuthInfo[3]) + 1);
    
    pstMsg->stMsgHead.ulMsgID   = 0;
    pstMsg->stMsgHead.usSrcMID  = MID_CLI;
    pstMsg->stMsgHead.usDstMID  = MID_AUTH;
    pstMsg->stMsgHead.usMsgType = MSG_AUTH;

    /*-----------������Ϣ-----*/
    if (OM_FAILED(MSG_SendMessage(pstMsg)))
    {
        IO_Print("Send message to CMM failed");
        iRet = TBS_FAILED;
        goto end_release;
    }

    //�ͷ�������ڴ�
    CLI_MSG_RELEASE(pstMsg);


    /*----------------������Ϣ---------------*/
    if (OM_FAILED(MSG_ReceiveMessage(&pstMsg, CLI_COMM_TIMEOUT)))
    {
        IO_Print("Wait message failed");
        return TBS_FAILED;
    }
    if (pstMsg->stMsgHead.usMsgType != MSG_AUTH_ACK)
    {
        IO_Print("Received unrespected message");
        iRet = TBS_FAILED;
        goto end_release;
    }

    /*-------------------������Ϣ-------------*/
    /*����count���������AUTHģ���ʵ�֣�count��������
     *����resultֵ������������levelֵ,�ڿ����½���ǰ����
     *�ο�WIKI��MSG_AUTH_ACK�Ĺ��췽ʽ
     */

    /*����ж����Ǹ���AUTHģ���MSG_AUTH_ACK�����д��
     *�����Ļ�����ȥ����MSG_AUTH_ACK����ô�����
     *���apps/logic/auth/auth.c��AUTH_RespAuthMsg����
     */
    //if (*(ULONG*)(pstMsg->szMsgBody) != 0 || *(ULONG*)(pstMsg->szMsgBody+sizeof(ULONG)) != 3)
    if (*(ULONG*)(pstMsg->szMsgBody) != 0)
    {
        iRet = TBS_FAILED;
        goto end_release;
    }
    
    pPos = pstMsg->szMsgBody + sizeof(ULONG)*2;
    sscanf(pPos, "result=%lu", &ulResult);
    if (ulResult != CLI_AUTH_SUCCESS)
    {
        iRet = ulResult;
        goto end_release;
    }

    pPos += (STB_StrLen(pPos) + 1);
    sscanf(pPos, "level=%lu", &ulLevel);
     
    *ulIndex = CLI_ConfigLever2CliLevel(ulLevel);
end_release:
    //�ͷ���Ϣ
    CLI_MSG_RELEASE(pstMsg);
    return iRet;
#endif

    //��֤ʧ��
    return TBS_FAILED;
}


/*************************************************************************
����:�����������е��û��ȼ���CLI���û��ȼ����ֲ�һ�£����������������
     ת��
����:ulConfigLevel:�������е��û��ȼ�
����:�����ִ�еȼ�
��ע:Ȩ��:CLI_AL_ADMIN > CLI_AL_OPERATOR > CLI_AL_QUERY
 *************************************************************************/
static ULONG CLI_ConfigLever2CliLevel(ULONG ulConfigLevel)
{
    switch(ulConfigLevel)
    {
    //�����������е�admin�û�
    case 1:
        return CLI_AL_ADMIN;
        
    //�����������е�user�û�    
    case 2:
        return CLI_AL_OPERATOR;

    //�����������е�guest�û�    
    case 3:
        return CLI_AL_QUERY;
    default:
        break;
    }

    return CLI_AL_QUERY;
}

/*=========================================================================*/
/*  ��������: CLI_ComStatusStart                                           */
/*  ��������: ͨѶ����״̬��ʼ��                                           */
/*  ����˵��: �ú���Ӧ�������е��ûص�����֮ǰʹ��                         */
/*  ��  ��  : liuzequn / 2007-11-6                                         */
/*=========================================================================*/
void CLI_ComStatusStart()
{
    g_szCliFeedbackError[0] = '\0';
    CLI_MSG_RELEASE(g_pstCliMsg); 
}

void CLI_ComStatusEnd()
{
    CLI_MSG_RELEASE(g_pstCliMsg); 
}

void CLI_ComErrorAdd(PCSTR szErrorInfo)
{
    DBG_ASSERT(szErrorInfo != NULL);

    strcat(g_szCliFeedbackError, "\n  ");
    strcat(g_szCliFeedbackError, szErrorInfo);
}

ULONG CLI_ComMsgInteract(ST_MSG *pstMsg, USHORT usType)
{
    DBG_ASSERT(pstMsg != NULL);

    pstMsg->stMsgHead.ulMsgID   = 0;
    pstMsg->stMsgHead.usSrcMID  = MID_CLI;
    pstMsg->stMsgHead.usDstMID  = MID_CMM;
    pstMsg->stMsgHead.usMsgType = usType;

    if (OM_FAILED(MSG_SendMessage(pstMsg)))
    {
        CLI_ComErrorAdd("Send message to CMM failed");
        return TBS_FAILED;
    }

    //����MSG_ReceiveMessageǰҪ���ͷ�
    CLI_MSG_RELEASE(g_pstCliMsg); 

    // ���µ�pstMsg��ָ����յ�����Ϣ
    if (OM_FAILED(MSG_ReceiveMessage(&g_pstCliMsg, CLI_COMM_TIMEOUT)))
    {
        CLI_ComErrorAdd("Wait message failed");
        return TBS_FAILED;
    }
    if (g_pstCliMsg->stMsgHead.usMsgType != (RESPONSE_FLAG |usType))
    {
        CLI_ComErrorAdd("Received unrespected message");
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

int CLI_AddStrToLink(ST_CLISTR_LINK **ppHead, char *szStr)
{
    ST_CLISTR_LINK *pObjAdd, *pObjTmp;

    DBG_ASSERT(ppHead != 0 && szStr != 0);

    pObjAdd = malloc(sizeof(ST_CLISTR_LINK));
    if (pObjAdd == NULL)
   {
       return TBS_FAILED;   //KW check, wjf
   }
    
    if (pObjAdd != NULL)
    {
        pObjAdd->pstrValue = malloc(strlen(szStr) + 1);

        //KW check,wjf
        if( pObjAdd->pstrValue == NULL)
        {
             free(pObjAdd);
             return TBS_FAILED;   
        }
        
        pObjAdd->pNext = NULL;
        strcpy(pObjAdd->pstrValue, szStr);
    }

    /*if (pObjAdd == NULL || pObjAdd->pstrValue == NULL)
        return TBS_FAILED; */

    if (*ppHead == NULL)
        *ppHead = pObjAdd;
    else
    {
        pObjTmp = *ppHead;
        while (pObjTmp->pNext != NULL) pObjTmp = pObjTmp->pNext;
        pObjTmp->pNext = pObjAdd;
    }
    return TBS_SUCCESS;
}

void CLI_DestroyStrLink(ST_CLISTR_LINK *pHead)
{

    while (pHead != 0)
    {
        ST_CLISTR_LINK *pTmp = pHead;
        pHead = pHead->pNext;
        free(pTmp->pstrValue);
        free(pTmp);
    }
}

/*=========================================================================*/
/*  ��������: CLI_CommInit                                                 */
/*  ��������: ������ͨ�ų�ʼ��                                             */
/*  �������:                                                              */
/*  ��  ��  : �ɹ���ʧ��                                                   */
/*  ��  ��  : liuzequn / 2007-11-6                                         */
/*=========================================================================*/
ULONG CLI_CommInit()
{
    /* ��Ϣģ���ע�� */
    if(OM_FAILED(MSG_RegModule(MID_CLI, NULL)))  /* ͨѶ��ʼ��ʧ�� */
    {
        /* kill old cmd process  ɱ��CMD���̣�ʹTELNET�������µ�½*/
		char szCmd[256] = {0};	
    	FILE *fp;            
        pid_t pid1;
        FILE *fd = NULL;

	    if((fp = fopen("/var/cmd_pid","r")) == NULL){
			printf("err on opening file");
			return 0;
		}
		fscanf(fp, "%d", &pid1); 
		fclose(fp);
        sprintf(szCmd, "ps -aux | grep cmd");

        fd = (FILE *)popen(szCmd, "r");   
        if(NULL != fd)   
        {
            fgets(szCmd, sizeof(szCmd), fd);
            pclose(fd);            
        }

		if( (strstr(szCmd, "-cmd") || strstr(szCmd, "/usr/bin/cmd")) && (pid1 > 0)){
            sprintf(szCmd, "kill -9 %d", pid1);
            tbsSystem(szCmd, 0);
    		tbsSystem("echo -1 > /var/cmd_pid", 0); 
        }        

//		sprintf(szDefaultRule,"rm -f /var/cmd_pid");
//		tbsSystem(szDefaultRule, 0);
		if (OM_FAILED(MSG_RegModule(MID_CLI, NULL)))
		{
            DBG_Out("Module register error. Exiting.\n");
            return TBS_FAILED;
		}
    }
    MSG_AllModStartOK();

    return TBS_SUCCESS;
}

// ����Ҷ�ӽڵ���Ϣ�Ļ�ȡ
ULONG CLI_GetLeaf(char *szLeafName, char *szLeafVal)
{
    ST_MSG *pstMsg;      /* ���͵���Ϣָ�� */
    USHORT  usLen = strlen(szLeafName);
    int     nRet;
    char    *pValPos = NULL;

    DBG_ASSERT(szLeafName != NULL);
    pstMsg = MSG_CreateMessage(usLen + 8);  // ׼ȷӦ���� sizeof(ULONG) + usLen + '\0'
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    pstMsg->stMsgHead.ulBodyLength = strlen(szLeafName) + 1 + sizeof(ULONG);
    *(ULONG *)pstMsg->szMsgBody = 1;
    strcpy(pstMsg->szMsgBody+ sizeof(ULONG), szLeafName);

    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_GET_VAL)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //���뿪ǰ�����ͷ��Ѿ�������ڴ�
        CLI_MSG_RELEASE(pstMsg); 

        return TBS_FAILED;
    }

    //�����¸�ָ�븳ֵǰ���Ȱ�֮ǰָ����ڴ��ͷŵ�
    CLI_MSG_RELEASE(pstMsg);

    pstMsg = g_pstCliMsg;
    nRet = (int)*(ULONG *)pstMsg->szMsgBody;
    if (nRet != 0)
    {
        CLI_ComErrorAdd("Get leaf info failed");
        return TBS_FAILED;
    }

    pValPos = strchr(pstMsg->szMsgBody + sizeof(ULONG) * 2, '=');

    if (pValPos == NULL)
        return TBS_FAILED;

    if (szLeafVal != NULL)
        strcpy(szLeafVal, pValPos + 1);
    else
        strcpy(szLeafName, pValPos + 1);
    return TBS_SUCCESS;

}

/*************************************************************************
����: ����MSG_CMM_GET_VAL������GET�Ľ��
����: pszNodePath :��Ҫ��ȡ�Ľڵ��·����
                   �����Ƿ�Ҷ�ڵ㣬Ҳ������
                   Ҷ�ڵ�
����: NULL        :GETʧ��
      ��NULLֵ    :GET�ɹ������ص��ǽ��յ���
                   ��Ϣ��ָ��
��ע: ����Ҫ����죬����ָ������Ӧ���д���Ҫ��
*************************************************************************/
ST_MSG *CLI_GetNode(char *pszNodePath)
{
    ST_MSG *pstMsg;      /* ���͵���Ϣָ�� */
    USHORT  usLen = 0;
    
    DBG_ASSERT(pszNodePath != NULL);
    usLen = strlen(pszNodePath);

    //������Ϣ
    pstMsg = MSG_CreateMessage(usLen + 8);  // ׼ȷӦ���� sizeof(ULONG) + usLen + '\0'
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return NULL;
    }

    //��ʼ��Ϣ��һЩ����
    pstMsg->stMsgHead.ulBodyLength = strlen(pszNodePath) + 1 + sizeof(ULONG);
    *(ULONG *)pstMsg->szMsgBody = 1;
    strcpy(pstMsg->szMsgBody+ sizeof(ULONG), pszNodePath);

    //���������
    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_GET_VAL)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //���뿪ǰ�����ͷ��Ѿ�������ڴ�
        CLI_MSG_RELEASE(pstMsg); 

        return NULL;
    }

    //�����¸�ָ�븳ֵǰ���Ȱ�֮ǰָ����ڴ��ͷŵ�
    CLI_MSG_RELEASE(pstMsg);

    pstMsg = g_pstCliMsg;

    //�ж���Ϣ�����Ƿ���ȷ
    if (pstMsg->stMsgHead.usMsgType != MSG_CMM_GET_VAL_ACK)
    {
        CLI_ComErrorAdd("Error Msg Type\n");
        return NULL;
    }

    //��Ϣ���ճɹ���������Ϣָ��
    return pstMsg;

}

// ������һ��ڵ����ƵĻ�ȡ
int CLI_GetNextLevelName(char *szPathName, char aszNextLevel[CLI_MAX_OBJ_NUM][CLI_LEAF__LEN])
{
    int     nLine, i;     // ��ȡ��������
    char   *pGetMsgBody, *pPosCur;
    ST_MSG* pstMsg = MSG_CreateMessage(256);    /* ����POST ��Ϣ*/

    if (szPathName == NULL)
    {
        CLI_ComErrorAdd("Parameter error");
        return TBS_FAILED;
    }


    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    pstMsg->szMsgBody[0] = 1;       // �ñ�Ǳ�ʾֻȡ��һ��
    strcpy(pstMsg->szMsgBody + 1, szPathName);
    pstMsg->stMsgHead.ulBodyLength = strlen(pstMsg->szMsgBody) + 1;     //  ����ѵ�0���ֽ�Ҳ���ȥ��

    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_GET_NAME)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //���뿪ǰ�����ͷ��Ѿ�������ڴ�
        CLI_MSG_RELEASE(pstMsg); 
        return -1;
    }

    CLI_MSG_RELEASE(pstMsg);

    // ����Ӧ�õ����������������б�
    pGetMsgBody = pPosCur = g_pstCliMsg->szMsgBody;
    if (*(unsigned long*)pPosCur != 0)   // ulResult !=0 ��ʾ��ȡʧ��
    {
        CLI_ComErrorAdd("Get name failed");
        return -1;
    }
    pPosCur += sizeof(unsigned long);
    nLine = *(unsigned long*)pPosCur;   // count Num
    pPosCur += sizeof(unsigned long);

    // ���û�����ݣ� ����Ҫ���κ����飬 Ҳ����ʧ��
    if (nLine == 0)
        return 0;

    // ����Ӧ����Ϣ�е�Ҷ����ȡ�������浽aszNextLevel;
    for (i = 0; i < nLine; i++)
    {
        char *pEqlPos;
        pPosCur = strchrR(pPosCur, '.'); // ��ȡҶ�����Ƶ�λ��
        if (pPosCur == NULL || pPosCur + 1 >= pGetMsgBody + g_pstCliMsg->stMsgHead.ulBodyLength)
        {
            CLI_ComErrorAdd("Get name message format error");
            return -1;
        }
        ++pPosCur;
        pEqlPos = strchr(pPosCur, ' ');
        if (pEqlPos == NULL)    // ����name attibute�ĸ�ʽ�� ����
        {
            CLI_ComErrorAdd("Get name conntent format error");
            return -1;
        }
        *pEqlPos = '\0';    // ��ģ��ֻ���ġ�=��֮ǰ������
        strcpy(aszNextLevel[i], pPosCur);
        *pEqlPos = ' ';     // �ָ��� ��
        pPosCur += strlen(pPosCur) + 1;
    }
    return nLine;

}

// ����ڵ���Ϣ�Ļ�ȡ
// ע�Ȿ�������ص��ַ���������Ҫ������ɾ��
ST_CLISTR_LINK *CLI_GetObject(char *szPathName, int nValNum, char aszLeafName[][CLI_LEAF__LEN])
{
    int   nRow, i;     // ��ȡ��������
    ST_MSG* pstMsg = NULL;    /* ������Ϣ*/
    char   *pGetMsgBody, *pPosCur;
    ST_CLISTR_LINK *pStrLink = NULL;

    if (szPathName == NULL && aszLeafName == NULL)
    {
        CLI_ComErrorAdd("GetObject Parameter error");
        return NULL;
    }

    pstMsg = MSG_CreateMessage(1024);
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return NULL;
    }

    pPosCur = pstMsg->szMsgBody;

    // �������
    *(unsigned long *)pPosCur = nValNum;
    pPosCur += sizeof(unsigned long);

    // �������ȡ����
    for (i = 0; i < nValNum; i++)
    {
        pPosCur += sprintf(pPosCur, "%s%s", szPathName, aszLeafName[i]) + 1;
    }
    pstMsg->stMsgHead.ulBodyLength = pPosCur - pstMsg->szMsgBody;

    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_GET_VAL)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //�뿪ǰ���ͷ��ڴ�
        CLI_MSG_RELEASE(pstMsg);
        return NULL;
    }

    CLI_MSG_RELEASE(pstMsg);

    // ����Ӧ�õ����������������б�
    pGetMsgBody = pPosCur = g_pstCliMsg->szMsgBody;
    if (*(unsigned long *)pPosCur != 0)   // ulResult !=0 ��ʾ��ȡʧ��
    {
        CLI_ComErrorAdd("Get value list failed");
        return NULL;
    }
    pPosCur += sizeof(unsigned long);

    nRow = *(unsigned long *)pPosCur;   // count Num
    pPosCur += sizeof(unsigned long);

    // ���û�����ݣ� ����Ҫ���κ����飬 Ҳ����ʧ��
    if (nRow != nValNum)
    {
        CLI_ComErrorAdd("Get Value num is unrespected");
        return NULL;
    }

    // ����Ӧ����Ϣ�е�ֵ��ȡ�������浽szValRet;
    for (i = 0; i < nValNum; i++)
    {
        pPosCur = strchr(pPosCur, '=');
        if(pPosCur == NULL) return NULL; //KW check, wjf
        if (*pPosCur == '\0' || pPosCur >= pGetMsgBody + g_pstCliMsg->stMsgHead.ulBodyLength)
        {
            CLI_ComErrorAdd("Get too less value!");
            CLI_DestroyStrLink(pStrLink);
            return NULL;
        }
        CLI_AddStrToLink(&pStrLink, pPosCur + 1);
        pPosCur += strlen(pPosCur) + 1;
    }

    return pStrLink;
}

// Ҷ�ӽڵ���Ϣ������
int CLI_SetLeaf(int nValNum, char aszName[][CLI_NODE__LEN], char aszVal[][CLI_NODE__LEN])
{
    ST_MSG         *pstMsg;
    int i;
    USHORT          usSetLen = sizeof(ULONG);
    char           szFaultCode[10];

    if (aszName == NULL || aszVal == NULL || nValNum == 0)
        return TBS_FAILED;

    pstMsg = MSG_CreateMessage(nValNum * 2 * CLI_NODE__LEN);
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    for (i = 0; i < nValNum; i++)
    {
        usSetLen += sprintf(pstMsg->szMsgBody + usSetLen, "%s=%s",
                            aszName[i], aszVal[i]) + 1;
    }
    pstMsg->stMsgHead.ulBodyLength = usSetLen;
    *(unsigned long *)pstMsg->szMsgBody = nValNum;

    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_SET_VAL)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //�뿪ǰ���ͷ��ڴ�
        CLI_MSG_RELEASE(pstMsg);
        return TBS_FAILED;
    }

    CLI_MSG_RELEASE(pstMsg);

    pstMsg = g_pstCliMsg;

    // skip result+status+errcount

    if (*(unsigned long*)pstMsg->szMsgBody != 0)
    {
    	snprintf(szFaultCode,sizeof(szFaultCode)-1,"%ld",*(unsigned long*)pstMsg->szMsgBody);
        CLI_ComErrorAdd(szFaultCode);
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

// ����ڵ���Ϣ�����
int CLI_AddObject(char *szPathName)
{
#if 0
    ST_MSG         *pstMsg = MSG_CreateMessage(256);

    if (szPathName == NULL)
        return TBS_FAILED;

    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    pstMsg->stMsgHead.ulBodyLength = sprintf(pstMsg->szMsgBody, "%s", szPathName) + 1;

    /* ����������Ϣ */
    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_ADD_NODE)))
    {
        CLI_ComErrorAdd("Communcate failed");
        return TBS_FAILED;
    }

    pstMsg = g_pstCliMsg;

    // �����Ӳ��ɹ��� �ṩ������
    if (*(unsigned long*)pstMsg->szMsgBody != 0)    // Result
    {
        CLI_ComErrorAdd("Add object failed");
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
#else
    return CLI_AddObjectEx(szPathName, NULL);
#endif
    
}


/*************************************************************************
����: ���һ��ʵ��
����: szPathName :�����������Ҫ��ӵ�ʵ���ĸ�·��
      iInstIndex :������������ʵ���ɹ����ʵ����
����: TBS_FAILED :���ʵ��ʧ��
      TBS_SUCCESS:���ʵ���ɹ�
��ע:   
*************************************************************************/
int CLI_AddObjectEx(char *szPathName, ULONG *pulInstIndex)
{
    ST_MSG         *pstMsg = NULL;

    if (szPathName == NULL)
        return TBS_FAILED;

    pstMsg = MSG_CreateMessage(256);
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    pstMsg->stMsgHead.ulBodyLength = sprintf(pstMsg->szMsgBody, "%s", szPathName) + 1;

    /* ����������Ϣ */
    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_ADD_NODE)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //�뿪ǰ���ͷ��ڴ�
        CLI_MSG_RELEASE(pstMsg);
        return TBS_FAILED;
    }

    CLI_MSG_RELEASE(pstMsg);
    pstMsg = g_pstCliMsg;

    //�ж���Ϣ�����Ƿ���ȷ
    if (pstMsg->stMsgHead.usMsgType != MSG_CMM_ADD_NODE_ACK)
    {
        CLI_ComErrorAdd("Unexpected message type recieved");
        return TBS_FAILED;
    }

    // �����Ӳ��ɹ��� �ṩ������
    if (*(unsigned long*)pstMsg->szMsgBody != 0)    // Result
    {
        CLI_ComErrorAdd("Add object failed");
        return TBS_FAILED;
    }

    //��ȡʵ����
    if (pulInstIndex != NULL)
    {
        *pulInstIndex = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    }

    return TBS_SUCCESS;
}

// ����ڵ���Ϣ��ɾ��
int CLI_DelObject(char *szPathName)
{
    ST_MSG         *pstMsg = NULL;

    if (szPathName == NULL)
        return TBS_FAILED;

    pstMsg = MSG_CreateMessage(256);
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    pstMsg->stMsgHead.ulBodyLength = sprintf(pstMsg->szMsgBody, "%s", szPathName) + 1;

    /* ����������Ϣ */
    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_DEL_NODE)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //�뿪ǰ���ͷ��ڴ�
        CLI_MSG_RELEASE(pstMsg);
        return TBS_FAILED;
    }

    pstMsg = g_pstCliMsg;

    // �����Ӳ��ɹ��� �ṩ������
    if (*(unsigned long*)pstMsg->szMsgBody != 0)    // Result
    {
        CLI_ComErrorAdd("Del object failed");
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

void CLI_CommErrorPrint()
{
   //lan config ��ʾ��dhcp �صĳ�ͻ
	if(!strcmp(g_szCliFeedbackError,"\n  15762308"))
		{
		   IO_Print("\n Interface address and dhcp address pool not match");	
		   return;
		}
	else if(!strcmp(g_szCliFeedbackError,"\n  15749201"))
		{
		   IO_Print("\n CPE IP conflicts with DHCP pool IP addr\n"); 
		   return;
		}

    IO_Print(g_szCliFeedbackError);
}
/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/



