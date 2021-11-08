
#include "../cli_pub.h"
#include "../cli/cli_term.h"
#include "../cli/cli_io.h"
#include "../cli/cli_interpret.h"
#include "cli_cmd.inc"

#include "../cli_comm.h"
//#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/
#define WAN_TYPE_COUNT          4
#define CMD_NAME_MAX_LEN        31

//������������apps/include/common.h���ж���
#define MAX_PATH_LEN        257
#define MAX_NODE_NAME_LEN   50
#define MAX_NODE_VALUE_LEN  517

//������������apps/logic/ddns/ddns.h���ж���
#define DDNS_PROVIDER_DYNDNS        "DynDNS.org"
#define DDNS_PROVIDER_TZO           "TZO"
#define DDNS_PROVIDER_GNUDIP        "GnuDIP"

#define CLI_WAN_PRINT       printf

#define CLI_WAN_IP_CONNECTION_NAME      "string InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANIPConnection.%lu.Name=%s"
#define CLI_WAN_PPP_CONNECTION_NAME     "string InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANPPPConnection.%lu.Name=%s"
#define CLI_WAN_DEVICE_BASE             "InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice."
#define CLI_WAN_IP_CONNECTION           "WANIPConnection"
#define CLI_WAN_PPP_CONNECTION          "WANPPPConnection"
#define CLI_DDNS_PATH                   "InternetGatewayDevice.X_TWSZ-COM_DDNS."


/*
 *��Ϊ�м����ط��õ���ͬ�Ĵ��룬���Ծ�д�ɺ���
 */
 
#define CLI_WAN__OPERATION_CALL(pFun) do \
{ \
    if (pFun() == TBS_SUCCESS) \
    { \
        return TBS_SUCCESS;\
    } \
}while(0);

//�������ȡ����
#define CLI_GET_PARAMETER_ASSERT(pPointer, name) do \
{ \
    (pPointer) = CLI_GetParamByName((name)); \
    if ((pPointer) == NULL) \
    { \
        return TBS_FAILED; \
    } \
}while(0);


/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/
struct stNodeCmd
{
    char szLeafNodeName[MAX_NODE_NAME_LEN];
    char szCmdName[CMD_NAME_MAX_LEN+1];
};


/******************************************************************************
 *                               GLOBAL VAR                                   *
 ******************************************************************************/




/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/
 static ULONG CLI_Add(void);
static ULONG CLI_Del(void);
static ULONG CLI_Show(void);
static ULONG CLI_Commit(void);
static ULONG CLI_Cancel(void);

static ULONG CLI_AddTzo(void);
static ULONG CLI_AddDyndns(void);
static ULONG CLI_AddGnudip(void);
static ULONG CLI_GetPathByConnName(char *pszConnName, char *pszPath, int iLen);
ULONG CLI_SendMsgWithoutBody(unsigned short usMsgType);

static void CLI_ShowOneDdnsInst(char *pszKey,
                                char *pszEmail,
                                char *pszPassword,
                                char *pszUsername,
                                char *pszConnName,
                                char *pszDDNSState,
                                char *pszHostname,
                                char *pszDDNSProvider,
                                char *szDdnsHost);


//������
ULONG CLI_Ddns(void)
{
    if (CLI_GetParamByName("add") != NULL)
    {
        CLI_WAN__OPERATION_CALL(CLI_Add);
    }

    if (CLI_GetParamByName("del") != NULL)
    {
        CLI_WAN__OPERATION_CALL(CLI_Del);
    }

    if (CLI_GetParamByName("show") != NULL)
    {
        CLI_WAN__OPERATION_CALL(CLI_Show);
    }

    if (CLI_GetParamByName("commit") != NULL)
    {
        CLI_WAN__OPERATION_CALL(CLI_Commit);
    }

    if (CLI_GetParamByName("cancel") != NULL)
    {
        CLI_WAN__OPERATION_CALL(CLI_Cancel);
    }
    
    return TBS_FAILED;
}

/*************************************************************************
����: ������Ӳ����ĺ���
����: ��
����: TBS_SUCCESS :��ӳɹ�
      TBS_FAILED  :���ʧ��
��ע: �����������������֧:
        ddns add tzo <connection-name> <hostname> <e-mail> <key> 
        ddns add dyndns <connection-name> <hostname> <username> <password>
        ddns add gnudip <connection-name> <hostname> <ddns-server> <username> <password>
*************************************************************************/
static ULONG CLI_Add(void)
{
    char *pOperation    = NULL;
    char *pDdnsType     = NULL;

    CLI_GET_PARAMETER_ASSERT(pOperation, "add");

    //��ȡDDNS����
    if ((pDdnsType = CLI_GetParamByName("tzo")) != NULL)
    {
        return CLI_AddTzo();
    }
    else if ((pDdnsType = CLI_GetParamByName("dyndns")) != NULL)
    {
        return CLI_AddDyndns();
    }
    else if ((pDdnsType = CLI_GetParamByName("gnudip")) != NULL)
    {
        return CLI_AddGnudip();
    }
 
    return TBS_FAILED;
}


/*************************************************************************
����: ͨ��WAN�������ƻ�ȡ��WAN���ӵ�·������:
      InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1
����: pszConnName:�����������������
      pszPath    :���������WAN��������
      iLen       :���������ָ��pszPathָ��Ŀռ�Ĵ�С
����: TBS_SUCCESS :��ӳɹ���pszPath��Ч
      TBS_FAILED  :���ʧ�ܣ�pszPath��Ч
��ע: ��Ҫ��ͼ������������ͷ�pstMsg����Ϊ���pstMsgָ��cli_comm.c��
      ��ȫ�ֱ���g_pstCliMsg����cli_comm.c�л��ͷ�g_pstCliMsg�������
      �ڱ��������ͷ���pstMsg����g_pstCliMsgָ��Ŀռ䣬����g_pstCliMsg
      ��û����ֵΪNULL����ôcli_comm.c���ٴ��ͷ�ʱ�ͻ�����ڴ����
*************************************************************************/
static ULONG CLI_GetPathByConnName(char *pszConnName, char *pszPath, int iLen)
{
    ST_MSG *pstMsg = NULL;
    ULONG  ulCount = 0;
    ULONG  ulTemp  = 0;
    ULONG  ulIndex1= 0;
    ULONG  ulIndex2= 0;
    ULONG  ulIndex3= 0;
    char *pCurPos  = NULL;
    char szName[MAX_NODE_VALUE_LEN] = {0};
    char szPath[MAX_PATH_LEN] = {0};
    ULONG  ulWanDevIndex = 1;

    if ((pszConnName == NULL)
        || (pszPath == NULL)
        || (iLen < MAX_PATH_LEN))
    {
        return TBS_FAILED;
    }
    
    while(1)
    {
        memset(szPath, 0 , MAX_PATH_LEN);
        sprintf(szPath, CLI_WAN_DEVICE_BASE, ulWanDevIndex);

        //����GET��Ϣ����ȡszPathָ����·�������еĽڵ㼰ֵ
        pstMsg = CLI_GetNode(szPath);
        if (pstMsg == NULL)
        {
            return TBS_FAILED;
        }

        //��ȡitem count,��Ҫ����result�ֶ�
        ulCount = *(ULONG *)(pstMsg->szMsgBody+sizeof(ULONG));
        //���item count����̫��
        if (ulCount > 1024)
        {
            return TBS_FAILED;
        }

        if(ulCount == 0)
        {
            IO_Print("%s :The items is 0\n", __FUNCTION__);
            return TBS_FAILED;
        }

        //����reult�ֶκ�item count�ֶ�
        pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);
        
        for (ulTemp = 0; ulTemp < ulCount; ulTemp++)
        {
            memset(szName, 0, MAX_NODE_VALUE_LEN);

            /*
             *ֻ��·�����и�".WANIPConnection."��
             *".WANPPPConnection."��·������ɨ��
             */
             
            if (strstr(pCurPos, "."CLI_WAN_IP_CONNECTION".") != NULL)
            {
                memset(szName, 0, sizeof(szName));
                sscanf(pCurPos, CLI_WAN_IP_CONNECTION_NAME, &ulIndex1, &ulIndex2, &ulIndex3, szName);
                if (strcmp(szName, pszConnName) == 0)
                {
                    //�ҵ���
                    char *pPos = strstr(pCurPos, "Name");
                    //��ΪpPos������ΪNULL���������ﲻ�ж�
                    //����·��,��Ϊ�����·��ǰ�����ֵ���ͺͿո���Ҫ����
                    //strlen("string ")��ֵ��7
                    strncpy(pszPath, pCurPos+7, (pPos - pCurPos -1 - 7));

                    return TBS_SUCCESS;
                }
            }
            else if (strstr(pCurPos, "."CLI_WAN_PPP_CONNECTION".") != NULL)
            {
                memset(szName, 0, sizeof(szName));
                sscanf(pCurPos, CLI_WAN_PPP_CONNECTION_NAME, &ulIndex1, &ulIndex2, &ulIndex3, szName);
                if (strcmp(szName, pszConnName) == 0)
                {
                    //�ҵ���
                    char *pPos = strstr(pCurPos, "Name");
                    //��ΪpPos������ΪNULL���������ﲻ�ж�
                    //����·��,��Ϊ�����·��ǰ�����ֵ���ͺͿո���Ҫ����
                    //strlen("string ")��ֵ��7
                    strncpy(pszPath, pCurPos+7, (pPos - pCurPos -1 - 7));

                    return TBS_SUCCESS;
                }
            }

            //ָ����һ��item
            if (pCurPos)
                pCurPos += (strlen(pCurPos) + 1);
                
        }
        ulWanDevIndex++;
    }

    IO_Print("\r\nconnection name:%s not exist!\n", pszConnName);
    return TBS_FAILED;
}
/************************************************************************
����: �ж��Ƿ������ͬ��hostname
����: 
����: TBS_SUCCESS :û����ͬ��hostname
      TBS_FAILED  :����ͬ��hostname
*************************************************************************/
static ULONG CLI_judge_hostname(char *hostname)
{
		ST_MSG *pstMsg = NULL;
	
		ULONG ulCount  = 0;
		ULONG ulInstID = 0; 		//DDNSʵ��ID��
		ULONG ulTemp   = 0;
		ULONG ulTempID = 0;
		char *pCurPos  = NULL;
		char *pPos	   = NULL;

		char szHostname[64] = {0};
        char szTemp[256]    = {0};
        char szValue[256]   = {0};	
		//��ȡInternetGatewayDevice.X_TWSZ-COM_DDNS.�µ�ʵ��
		pstMsg = CLI_GetNode(CLI_DDNS_PATH);
		if (pstMsg == NULL)
		{
			return TBS_FAILED;
		}
	
		/*
		 *��������ʾ
		 */
	
		//��ȡitem count,Ҫ����result�ֶ�
		ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
		//item count����̫��
		if (ulCount > 1024)
		{
			return TBS_FAILED;
		}
	
		//ָ��item,��Ҫ����result�ֶκ�item count�ֶ�
		pCurPos = pstMsg->szMsgBody + sizeof(ULONG)*2;
	
		//����
		for (ulTemp=0; ulTemp<ulCount; )
		{

			/*
			 *��ʵ������ͬ��item��do..while�д����꣬��Ҫ��
			 *ʵ������ͬ��item�ڷ��ص���Ϣ������������
			 */
	
			//�ȳ�ʼ��

			memset(szHostname, 0, sizeof(szHostname));
	
			sscanf(pCurPos, "string InternetGatewayDevice.X_TWSZ-COM_DDNS.%lu.%s", &ulTempID, szTemp);
			ulInstID = ulTempID;
	
			do
			{
				/*
				 *һ��item������:
				 *"string InternetGatewayDevice.X_TWSZ-COM_DDNS.1.Key=ss"
				 */
			  
				
	
				//��ȡҶ�ڵ�������Ϊ��ʱ��szTemp��ֵ����:Name=Value
				pPos = strchr(szTemp, '=');
				*pPos = '\0';
	
				//��ȡ�ڵ�ֵ
				strcpy(szValue, ++pPos);
	
				//ָ����һ��item
				if (pCurPos)
				{
					pCurPos += (strlen(pCurPos) + 1);
					ulTemp++;
				}
				
				
	
				//��DDNS��Ϣ��������Ӧ�ı�����

				if (strcmp(szTemp, "Hostname") ==0)
				{
					strcpy(szHostname, szValue);
					if (strcmp(szHostname, hostname) ==0)
						{
						    IO_Print("\n Hostname exist already");
                            return TBS_FAILED;
						}
				}
	
				//ulInstIDΪ0�������item�ǵ�һ��item����ʱ��û��ȡDDNSʵ������Ϣ
				if (((ulTempID != ulInstID) && (ulInstID != 0)) 		//��ֹһ��DDNSʵ�������
					|| (ulTemp >= ulCount)) 						  //���ֻ��һ��DDNSʵ�������
				{
					break;
				}
	
				sscanf(pCurPos, "string InternetGatewayDevice.X_TWSZ-COM_DDNS.%lu.%s", &ulTempID, szTemp);
	
			}while(1);
	
		}
	
		return TBS_SUCCESS;


}
/*************************************************************************
����: ���TZO���͵�DDNS����������ֵ
����: ��
����: TBS_SUCCESS :��ӳɹ�
      TBS_FAILED  :���ʧ��
��ע: ��������:
        ddns add tzo <connection-name> <hostname> <e-mail> <key> 
      ��<connection-name> <hostname> <e-mail> <key>����
*************************************************************************/
static ULONG CLI_AddTzo(void)
{
    ULONG ulInstIndex        = 0;//ʵ����
    char *pszConnectionName  = NULL;
    char *pszHostName        = NULL;
    char *pszEmail           = NULL;
    char *pszKey             = NULL;
    char szPath[MAX_PATH_LEN]= {0} ;
    char aszPath[5][CLI_NODE__LEN] ;
    char aszValue[5][CLI_NODE__LEN];   /*
                                       *��5��ֵҪ�裬һ����DDNS���ͣ�����
                                       *��TZO���ڶ�����WANConnectionID��
                                       *��������hostname�����ĸ���e-mail��
                                       *�������key
                                       */
 
    //�Ƿ���connection-name
    CLI_GET_PARAMETER_ASSERT(pszConnectionName, "connection-name");

    //�Ƿ���hostname
    CLI_GET_PARAMETER_ASSERT(pszHostName, "hostname");

    //�Ƿ���e-mail
    CLI_GET_PARAMETER_ASSERT(pszEmail, "e-mail");

    //�Ƿ���key
    CLI_GET_PARAMETER_ASSERT(pszKey, "key");

    //�Ƿ������ͬ��hostname
	if(CLI_judge_hostname(pszHostName) == TBS_FAILED)
	{
        return TBS_FAILED;
	}
	//���WANConnectionID
    if (CLI_GetPathByConnName(pszConnectionName, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    /*
     *���ʵ��
     */
    //����ADD��Ϣ
    if (CLI_AddObjectEx(CLI_DDNS_PATH, &ulInstIndex) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    /*
     *����ֵ
     */

    //���DDNS����
    sprintf(aszPath[0], "%s%lu.DDNSProvider", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[0], "%s", DDNS_PROVIDER_TZO);

    sprintf(aszPath[1], "%s%lu.WANConnectionID", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[1], "%s", szPath);

    //���hostname
    sprintf(aszPath[2], "%s%lu.Hostname", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[2], "%s", pszHostName);

    //���Email
    sprintf(aszPath[3], "%s%lu.Email", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[3], "%s", pszEmail);

    //���Key
    sprintf(aszPath[4], "%s%lu.Key", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[4], "%s", pszKey);

    //����
    if (CLI_SetLeaf(5, aszPath, aszValue) != TBS_SUCCESS)
    {  
        return TBS_FAILED;
    }

    //����ֵ
    return TBS_SUCCESS;
}

/*************************************************************************
����: ���dyndns���͵�DDNS����������ֵ
����: ��
����: TBS_SUCCESS :��ӳɹ�
      TBS_FAILED  :���ʧ��
��ע: ��������:
        ddns add dyndns <connection-name> <hostname> <username> <password> 
      ��<connection-name> <hostname> <username> <password>����
*************************************************************************/
static ULONG CLI_AddDyndns(void)
{
    ULONG ulInstIndex        = 0;//ʵ����
    char *pszConnectionName  = NULL;
    char *pszHostName        = NULL;
    char *pszUsername        = NULL;
    char *pszPassword        = NULL;
    char szPath[MAX_PATH_LEN]= {0} ;
    char aszPath[5][CLI_NODE__LEN] ;
    char aszValue[5][CLI_NODE__LEN];   /*
                                       *��5��ֵҪ�裬һ����DDNS���ͣ�����
                                       *��TZO���ڶ�����WANConnectionID��
                                       *��������hostname�����ĸ���e-mail��
                                       *�������key
                                       */
 
    //�Ƿ���connection-name
    CLI_GET_PARAMETER_ASSERT(pszConnectionName, "connection-name");

    //�Ƿ���hostname
    CLI_GET_PARAMETER_ASSERT(pszHostName, "hostname");

    //�Ƿ���username
    CLI_GET_PARAMETER_ASSERT(pszUsername, "username");

    //�Ƿ���password
    CLI_GET_PARAMETER_ASSERT(pszPassword, "password");

    //�Ƿ������ͬ��hostname
	if(CLI_judge_hostname(pszHostName) == TBS_FAILED)
	{
        return TBS_FAILED;
	}

    //���WANConnectionID
    if (CLI_GetPathByConnName(pszConnectionName, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    /*
     *���ʵ��
     */
    //����ADD��Ϣ
    if (CLI_AddObjectEx(CLI_DDNS_PATH, &ulInstIndex) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    /*
     *����ֵ
     */

    //���DDNS����
    sprintf(aszPath[0], "%s%lu.DDNSProvider", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[0], "%s", DDNS_PROVIDER_DYNDNS);

    sprintf(aszPath[1], "%s%lu.WANConnectionID", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[1], "%s", szPath);

    //���hostname
    sprintf(aszPath[2], "%s%lu.Hostname", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[2], "%s", pszHostName);

    //���Email
    sprintf(aszPath[3], "%s%lu.Username", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[3], "%s", pszUsername);

    //���Key
    sprintf(aszPath[4], "%s%lu.Password", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[4], "%s", pszPassword);

    //����
    if (CLI_SetLeaf(5, aszPath, aszValue) != TBS_SUCCESS)
    {  
        return TBS_FAILED;
    }

    //����ֵ
    return TBS_SUCCESS;
}

/*************************************************************************
����: ���gnudip���͵�DDNS����������ֵ
����: ��
����: TBS_SUCCESS :��ӳɹ�
      TBS_FAILED  :���ʧ��
��ע: ��������:
        ddns add gnudip <connection-name> <hostname> <ddns-server> <username> <password> 
      ��<connection-name> <hostname> <ddns-server> <username> <password>����
*************************************************************************/
static ULONG CLI_AddGnudip(void)
{
    ULONG ulInstIndex        = 0;//ʵ����
    char *pszConnectionName  = NULL;
    char *pszHostName        = NULL;
    char *pszDdnsServer      = NULL;
    char *pszUsername        = NULL;
    char *pszPassword        = NULL;
    char szPath[MAX_PATH_LEN]= {0} ;
    char aszPath[6][CLI_NODE__LEN] ;
    char aszValue[6][CLI_NODE__LEN];   /*
                                       *��5��ֵҪ�裬һ����DDNS���ͣ�����
                                       *��TZO���ڶ�����WANConnectionID��
                                       *��������hostname�����ĸ���e-mail��
                                       *�������key
                                       */
 
    //�Ƿ���connection-name
    CLI_GET_PARAMETER_ASSERT(pszConnectionName, "connection-name");

    //�Ƿ���hostname
    CLI_GET_PARAMETER_ASSERT(pszHostName, "hostname");

    //�Ƿ���ddns-server
    CLI_GET_PARAMETER_ASSERT(pszDdnsServer, "ddns-server");

    //�Ƿ���username
    CLI_GET_PARAMETER_ASSERT(pszUsername, "username");

    //�Ƿ���password
    CLI_GET_PARAMETER_ASSERT(pszPassword, "password");

    //�Ƿ������ͬ��hostname
	if(CLI_judge_hostname(pszHostName) == TBS_FAILED)
	{
        return TBS_FAILED;    
	}

	//���WANConnectionID
    if (CLI_GetPathByConnName(pszConnectionName, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    /*
     *���ʵ��
     */
    //����ADD��Ϣ
    if (CLI_AddObjectEx(CLI_DDNS_PATH, &ulInstIndex) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    /*
     *����ֵ
     */

    //���DDNS����
    sprintf(aszPath[0], "%s%lu.DDNSProvider", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[0], "%s", DDNS_PROVIDER_GNUDIP);

    sprintf(aszPath[1], "%s%lu.WANConnectionID", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[1], "%s", szPath);

    //���hostname
    sprintf(aszPath[2], "%s%lu.Hostname", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[2], "%s", pszHostName);

    //���hostname
    sprintf(aszPath[3], "%s%lu.DdnsHost", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[3], "%s", pszDdnsServer);

    //���Email
    sprintf(aszPath[4], "%s%lu.Username", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[4], "%s", pszUsername);

    //���Key
    sprintf(aszPath[5], "%s%lu.Password", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[5], "%s", pszPassword);

    //����
    if (CLI_SetLeaf(6, aszPath, aszValue) != TBS_SUCCESS)
    {  
        return TBS_FAILED;
    }

    //����ֵ
    return TBS_SUCCESS;
}


/*************************************************************************
����: ����ɾ�������ĺ���
����: ��
����: TBS_SUCCESS :��ӳɹ�
      TBS_FAILED  :���ʧ��
��ע: ������������:
        ddns del <hostname>
*************************************************************************/
static ULONG CLI_Del(void)
{
    ST_MSG *pstMsg          = NULL;
    ULONG ulCount           = 0;
    ULONG ulTemp            = 0;
    char *pOperation        = NULL;                    //wan connection name
    char *pHostName         = NULL;
    char *pCurPos           = NULL;
    char *pTempPos          = NULL;
    char szDDNSInstPath[MAX_PATH_LEN]  = {0};

    //�Ƿ���del����
    CLI_GET_PARAMETER_ASSERT(pOperation, "del");

    //��ȡhostname����
    CLI_GET_PARAMETER_ASSERT(pHostName, "hostname");

    //����hostname��Ӧ��DDNSʵ����·��,
    //��Ϊhostname��Ψһ��
    if ((pstMsg = CLI_GetNode(CLI_DDNS_PATH)) == NULL) 
    {
        return TBS_FAILED;
    }

    /*
     *����Ϣ�н�����·������:
     *InternetGatewayDevice.X_TWSZ-COM_DDNS.1.
     */
     
    //��ȡcount����Ҫ����result�ֶ�
    ulCount = *(ULONG*)(pstMsg->szMsgBody + sizeof(ULONG));
    //���ulCountֵ����̫��
    if (ulCount > 1024)
    {
        return TBS_FAILED;
    }

    //ָ��item����Ҫ����result��item count�����ֶ�
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    for (ulTemp=0; ulTemp<ulCount; ulTemp++)
    {
        //
        pTempPos = strstr(pCurPos, "Hostname=");
        if (pTempPos != NULL)
        {
            char *pPos = strchr(pCurPos, '=');
            if (pPos != NULL)
            {
                //�ж�hostnameֵ�Ƿ���ͬ
                if (strcmp(pHostName, pPos+1) == 0)
                {
                    /*
                     *����ǰ���"string "����7���ֽ�
                     *��ΪCMM���ص�һ��item����������:
                     *"string InternetGatewayDevice.X_TWSZ-COM_DDNS.1.Hostname=test.tzo.com"
                     */
                    strncpy(szDDNSInstPath, pCurPos+7, pTempPos-(pCurPos+7));

                    break;
                }
            }
        }

        //ָ����һ��item
        pCurPos += strlen(pCurPos) + 1;
    }

    //����Ҳ���
    if (strlen(szDDNSInstPath) == 0)
    {
        return TBS_FAILED;
    }
    //�ҵ��ˣ�ɾ��֮
    else if (CLI_DelObject(szDDNSInstPath) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
     
    return TBS_SUCCESS;
}

//����show�����ĺ���
static ULONG CLI_Show(void)
{
    ST_MSG *pstMsg = NULL;

    ULONG ulCount  = 0;
    ULONG ulInstID = 0;         //DDNSʵ��ID��
    ULONG ulTemp   = 0;
    ULONG ulTempID = 0;
    char *pCurPos  = NULL;
    char *pPos     = NULL;
    char szDdnsType[64] = {0};
    char szHostname[64] = {0};
    char szConnName[256]= {0};
    char szDdnsState[64]= {0};
    char szUsername[64] = {0};
    char szPassword[64] = {0};
    char szEMail[64]    = {0};
    char szKey[64]      = {0};
    char szTemp[256]    = {0};
    char szValue[256]   = {0};
    char szDdnsHost[256] = {0};

    //��ȡInternetGatewayDevice.X_TWSZ-COM_DDNS.�µ�ʵ��
    pstMsg = CLI_GetNode(CLI_DDNS_PATH);
    if (pstMsg == NULL)
    {
        return TBS_FAILED;
    }

    /*
     *��������ʾ
     */

    //��ȡitem count,Ҫ����result�ֶ�
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    //item count����̫��
    if (ulCount > 1024)
    {
        return TBS_FAILED;
    }

    //ָ��item,��Ҫ����result�ֶκ�item count�ֶ�
    pCurPos = pstMsg->szMsgBody + sizeof(ULONG)*2;

    //����
    for (ulTemp=0; ulTemp<ulCount; )
    {
#if 1
        /*
         *��ʵ������ͬ��item��do..while�д����꣬��Ҫ��
         *ʵ������ͬ��item�ڷ��ص���Ϣ������������
         */

        //�ȳ�ʼ��
        memset(szDdnsState, 0, sizeof(szDdnsState));
        memset(szDdnsType, 0, sizeof(szDdnsType));
        memset(szConnName, 0, sizeof(szConnName));
        memset(szHostname, 0, sizeof(szHostname));
        memset(szUsername, 0, sizeof(szUsername));
        memset(szPassword, 0, sizeof(szPassword));
        memset(szEMail, 0, sizeof(szEMail));
        memset(szKey, 0, sizeof(szKey));
        memset(szDdnsHost, 0, sizeof(szDdnsHost));

        sscanf(pCurPos, "string InternetGatewayDevice.X_TWSZ-COM_DDNS.%lu.%s", &ulTempID, szTemp);
        ulInstID = ulTempID;

        do
        {
            /*
             *һ��item������:
             *"string InternetGatewayDevice.X_TWSZ-COM_DDNS.1.Key=ss"
             */
          
            

            //��ȡҶ�ڵ�������Ϊ��ʱ��szTemp��ֵ����:Name=Value
            pPos = strchr(szTemp, '=');
            *pPos = '\0';

            //��ȡ�ڵ�ֵ
            strcpy(szValue, ++pPos);

            //ָ����һ��item
            if (pCurPos)
            {
                pCurPos += (strlen(pCurPos) + 1);
                ulTemp++;
            }
            
            

            //��DDNS��Ϣ��������Ӧ�ı�����
            if (strcmp(szTemp, "Key") ==0)
            {
                strcpy(szKey, szValue);
            }
            else if (strcmp(szTemp, "Email") ==0)
            {
                strcpy(szEMail, szValue);
            }
            else if (strcmp(szTemp, "Password") ==0)
            {
                strcpy(szPassword, szValue);
            }
            else if (strcmp(szTemp, "Username") ==0)
            {
                strcpy(szUsername, szValue);
            }
            else if (strcmp(szTemp, "WANConnectionID") ==0)
            {
                strcat(szTemp, ".Name");
                CLI_GetLeaf(szValue, szConnName);
            }
            else if (strcmp(szTemp, "DDNSState") ==0)
            {
                strcpy(szDdnsState, szValue);
            }
            else if (strcmp(szTemp, "Hostname") ==0)
            {
                strcpy(szHostname, szValue);
            }
            else if (strcmp(szTemp, "DDNSProvider") ==0)
            {
                strcpy(szDdnsType, szValue);
            }
            else if(strcmp(szTemp, "DdnsHost") == 0)
            {
                strcpy(szDdnsHost, szValue);
            }

            //ulInstIDΪ0�������item�ǵ�һ��item����ʱ��û��ȡDDNSʵ������Ϣ
            if (((ulTempID != ulInstID) && (ulInstID != 0))         //��ֹһ��DDNSʵ�������
                || (ulTemp >= ulCount))                           //���ֻ��һ��DDNSʵ�������
            {
                //��ʾDDNS��Ϣ
                CLI_ShowOneDdnsInst(szKey, szEMail,
                                    szPassword,
                                    szUsername,
                                    szConnName,
                                    szDdnsState,
                                    szHostname,
                                    szDdnsType,
                                    szDdnsHost);

                //��ʼ��һ��DDNSʵ����Ϣ�Ļ�ȡ
                break;
            }

            sscanf(pCurPos, "string InternetGatewayDevice.X_TWSZ-COM_DDNS.%lu.%s", &ulTempID, szTemp);

        }while(1);

#else
        break;
#endif
    }

    return TBS_SUCCESS;
}

static void CLI_ShowOneDdnsInst(char *pszKey,
                                char *pszEmail,
                                char *pszPassword,
                                char *pszUsername,
                                char *pszConnName,
                                char *pszDDNSState,
                                char *pszHostname,
                                char *pszDDNSProvider,
                                char *szDdnsHost)
{
    //�⼸������Ϊ��
    if ((pszDDNSProvider == NULL)
        || (pszHostname == NULL)
        || (pszConnName == NULL)
        || (pszHostname == NULL))
    {
        return ;
    }
   
    //��ʾDYNDNS����Ϣ
    if (strcmp(pszDDNSProvider, DDNS_PROVIDER_DYNDNS) == 0)
    {
        IO_Print("\n==============================================\n");
        IO_Print("\tProvider:%s\n", pszDDNSProvider);
        IO_Print("\tHostname:%s\n", pszHostname);
        IO_Print("\tUsername:%s\n", pszUsername);
        IO_Print("\tPassword:%s\n", pszPassword);
        IO_Print("\tState   :%s\n", ((pszDDNSState != NULL) 
                                     && (strlen(pszDDNSState) == 0)) ? "Not Updated" : "Updated");
    }
    //��ʾTZO����Ϣ
    else if (strcmp(pszDDNSProvider, DDNS_PROVIDER_TZO) == 0)
    {
        IO_Print("\n==============================================\n");
        IO_Print("\tProvider:%s\n", pszDDNSProvider);
        IO_Print("\tHostname:%s\n", pszHostname);
        IO_Print("\tE-mail  :%s\n", pszEmail);
        IO_Print("\tKey     :%s\n", pszKey);
        IO_Print("\tState   :%s\n", ((pszDDNSState != NULL) 
                                     && (strlen(pszDDNSState) == 0)) ? "Not Updated" : "Updated");
    }
    //��ʾGNUDIP����Ϣ
    else if (strcmp(pszDDNSProvider, DDNS_PROVIDER_GNUDIP) == 0)
    {
        IO_Print("\n==============================================\n");
        IO_Print("\tProvider:%s\n", pszDDNSProvider);
        IO_Print("\tHostname:%s\n", pszHostname);
        IO_Print("\tDdnsServer:%s\n", szDdnsHost);        
        IO_Print("\tUsername:%s\n", pszUsername);
        IO_Print("\tPassword:%s\n", pszPassword);
        IO_Print("\tState   :%s\n", ((pszDDNSState != NULL) 
                                     && (strlen(pszDDNSState) == 0)) ? "Not Updated" : "Updated");
    }
    IO_Print("\n");
    
}

//����commit�����ĺ���
static ULONG CLI_Commit(void)
{
    if (CLI_SendMsgWithoutBody(MSG_CMM_COMMIT) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

//����cancel�����ĺ���
static ULONG CLI_Cancel(void)
{
    if (CLI_SendMsgWithoutBody(MSG_CMM_CANCEL) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}



/*************************************************************************
����: ����MSG_CMM_COMMIT��Ϣ(MID_CLI=>MID_CMM)
����: ��
����: TBS_SUCCESS:���ͳɹ�
      TBS_FAILED :����ʧ��
��ע:   
*************************************************************************/
ULONG CLI_SendMsgWithoutBody(unsigned short usMsgType)
{
    ST_MSG *pstMsg;      /* ���͵���Ϣָ�� */

    //������Ϣ�ռ�
    pstMsg = MSG_CreateMessage(sizeof(pstMsg->stMsgHead));
    if (pstMsg == NULL)
    {
        return TBS_FAILED;
    }

    //��ʼ����Ϣ
    memset(&(pstMsg->stMsgHead), 0, sizeof(pstMsg->stMsgHead));
    pstMsg->stMsgHead.ulBodyLength = 0;
    pstMsg->stMsgHead.usDstMID     = MID_CMM;
    pstMsg->stMsgHead.usMsgType    = usMsgType;
    pstMsg->stMsgHead.usSrcMID     = MID_CLI;

    //������Ϣ
    if (OM_FAILED(MSG_SendMessage(pstMsg)))
    {
        CLI_ComErrorAdd("Send message to CMM failed");
        CLI_MSG_RELEASE(pstMsg);
        return TBS_FAILED;
    }

    CLI_MSG_RELEASE(pstMsg);
    return TBS_SUCCESS;
}


/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif
