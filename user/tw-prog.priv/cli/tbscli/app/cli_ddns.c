
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

//以下这三个在apps/include/common.h里有定义
#define MAX_PATH_LEN        257
#define MAX_NODE_NAME_LEN   50
#define MAX_NODE_VALUE_LEN  517

//以下这三个在apps/logic/ddns/ddns.h里有定义
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
 *因为有几个地方用到相同的代码，所以就写成宏了
 */
 
#define CLI_WAN__OPERATION_CALL(pFun) do \
{ \
    if (pFun() == TBS_SUCCESS) \
    { \
        return TBS_SUCCESS;\
    } \
}while(0);

//这个用来取参数
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


//主函数
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
功能: 处理添加操作的函数
参数: 无
返回: TBS_SUCCESS :添加成功
      TBS_FAILED  :添加失败
备注: 处理如下三个命令分支:
        ddns add tzo <connection-name> <hostname> <e-mail> <key> 
        ddns add dyndns <connection-name> <hostname> <username> <password>
        ddns add gnudip <connection-name> <hostname> <ddns-server> <username> <password>
*************************************************************************/
static ULONG CLI_Add(void)
{
    char *pOperation    = NULL;
    char *pDdnsType     = NULL;

    CLI_GET_PARAMETER_ASSERT(pOperation, "add");

    //获取DDNS类型
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
功能: 通过WAN连接名称获取该WAN连接的路径，如:
      InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1
参数: pszConnName:输入参数，连接名称
      pszPath    :输出参数，WAN连接名称
      iLen       :输入参数，指出pszPath指向的空间的大小
返回: TBS_SUCCESS :添加成功，pszPath有效
      TBS_FAILED  :添加失败，pszPath无效
备注: 不要试图在这个函数中释放pstMsg，因为这个pstMsg指向cli_comm.c中
      的全局变量g_pstCliMsg，而cli_comm.c中会释放g_pstCliMsg。如果你
      在本函数中释放了pstMsg，即g_pstCliMsg指向的空间，但是g_pstCliMsg
      又没被赋值为NULL，那么cli_comm.c中再次释放时就会出现内存错误
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

        //发送GET消息，获取szPath指定的路径下所有的节点及值
        pstMsg = CLI_GetNode(szPath);
        if (pstMsg == NULL)
        {
            return TBS_FAILED;
        }

        //获取item count,需要跳过result字段
        ulCount = *(ULONG *)(pstMsg->szMsgBody+sizeof(ULONG));
        //这个item count不能太大
        if (ulCount > 1024)
        {
            return TBS_FAILED;
        }

        if(ulCount == 0)
        {
            IO_Print("%s :The items is 0\n", __FUNCTION__);
            return TBS_FAILED;
        }

        //跳过reult字段和item count字段
        pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);
        
        for (ulTemp = 0; ulTemp < ulCount; ulTemp++)
        {
            memset(szName, 0, MAX_NODE_VALUE_LEN);

            /*
             *只对路径中有个".WANIPConnection."或
             *".WANPPPConnection."的路径进行扫描
             */
             
            if (strstr(pCurPos, "."CLI_WAN_IP_CONNECTION".") != NULL)
            {
                memset(szName, 0, sizeof(szName));
                sscanf(pCurPos, CLI_WAN_IP_CONNECTION_NAME, &ulIndex1, &ulIndex2, &ulIndex3, szName);
                if (strcmp(szName, pszConnName) == 0)
                {
                    //找到了
                    char *pPos = strstr(pCurPos, "Name");
                    //因为pPos不可能为NULL，所以这里不判断
                    //生成路径,因为这里的路径前面带有值类型和空格，所要跳过
                    //strlen("string ")的值是7
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
                    //找到了
                    char *pPos = strstr(pCurPos, "Name");
                    //因为pPos不可能为NULL，所以这里不判断
                    //生成路径,因为这里的路径前面带有值类型和空格，所要跳过
                    //strlen("string ")的值是7
                    strncpy(pszPath, pCurPos+7, (pPos - pCurPos -1 - 7));

                    return TBS_SUCCESS;
                }
            }

            //指向下一个item
            if (pCurPos)
                pCurPos += (strlen(pCurPos) + 1);
                
        }
        ulWanDevIndex++;
    }

    IO_Print("\r\nconnection name:%s not exist!\n", pszConnName);
    return TBS_FAILED;
}
/************************************************************************
功能: 判断是否存在相同的hostname
参数: 
返回: TBS_SUCCESS :没有相同的hostname
      TBS_FAILED  :有相同的hostname
*************************************************************************/
static ULONG CLI_judge_hostname(char *hostname)
{
		ST_MSG *pstMsg = NULL;
	
		ULONG ulCount  = 0;
		ULONG ulInstID = 0; 		//DDNS实例ID号
		ULONG ulTemp   = 0;
		ULONG ulTempID = 0;
		char *pCurPos  = NULL;
		char *pPos	   = NULL;

		char szHostname[64] = {0};
        char szTemp[256]    = {0};
        char szValue[256]   = {0};	
		//获取InternetGatewayDevice.X_TWSZ-COM_DDNS.下的实例
		pstMsg = CLI_GetNode(CLI_DDNS_PATH);
		if (pstMsg == NULL)
		{
			return TBS_FAILED;
		}
	
		/*
		 *遍历并显示
		 */
	
		//获取item count,要跳过result字段
		ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
		//item count不能太大
		if (ulCount > 1024)
		{
			return TBS_FAILED;
		}
	
		//指向item,需要跳过result字段和item count字段
		pCurPos = pstMsg->szMsgBody + sizeof(ULONG)*2;
	
		//遍历
		for (ulTemp=0; ulTemp<ulCount; )
		{

			/*
			 *把实例号相同的item在do..while中处理完，这要求
			 *实例号相同的item在返回的消息体中是连续的
			 */
	
			//先初始化

			memset(szHostname, 0, sizeof(szHostname));
	
			sscanf(pCurPos, "string InternetGatewayDevice.X_TWSZ-COM_DDNS.%lu.%s", &ulTempID, szTemp);
			ulInstID = ulTempID;
	
			do
			{
				/*
				 *一个item类似于:
				 *"string InternetGatewayDevice.X_TWSZ-COM_DDNS.1.Key=ss"
				 */
			  
				
	
				//获取叶节点名，因为这时的szTemp的值类似:Name=Value
				pPos = strchr(szTemp, '=');
				*pPos = '\0';
	
				//获取节点值
				strcpy(szValue, ++pPos);
	
				//指向下一个item
				if (pCurPos)
				{
					pCurPos += (strlen(pCurPos) + 1);
					ulTemp++;
				}
				
				
	
				//把DDNS信息保存在相应的变量中

				if (strcmp(szTemp, "Hostname") ==0)
				{
					strcpy(szHostname, szValue);
					if (strcmp(szHostname, hostname) ==0)
						{
						    IO_Print("\n Hostname exist already");
                            return TBS_FAILED;
						}
				}
	
				//ulInstID为0表明这个item是第一个item，这时还没获取DDNS实例的信息
				if (((ulTempID != ulInstID) && (ulInstID != 0)) 		//不止一个DDNS实例的情况
					|| (ulTemp >= ulCount)) 						  //针对只有一个DDNS实例的情况
				{
					break;
				}
	
				sscanf(pCurPos, "string InternetGatewayDevice.X_TWSZ-COM_DDNS.%lu.%s", &ulTempID, szTemp);
	
			}while(1);
	
		}
	
		return TBS_SUCCESS;


}
/*************************************************************************
功能: 添加TZO类型的DDNS，包括设置值
参数: 无
返回: TBS_SUCCESS :添加成功
      TBS_FAILED  :添加失败
备注: 处理命令:
        ddns add tzo <connection-name> <hostname> <e-mail> <key> 
      中<connection-name> <hostname> <e-mail> <key>部分
*************************************************************************/
static ULONG CLI_AddTzo(void)
{
    ULONG ulInstIndex        = 0;//实例号
    char *pszConnectionName  = NULL;
    char *pszHostName        = NULL;
    char *pszEmail           = NULL;
    char *pszKey             = NULL;
    char szPath[MAX_PATH_LEN]= {0} ;
    char aszPath[5][CLI_NODE__LEN] ;
    char aszValue[5][CLI_NODE__LEN];   /*
                                       *有5个值要设，一个是DDNS类型，这里
                                       *是TZO，第二个是WANConnectionID，
                                       *第三个是hostname，第四个是e-mail，
                                       *第五个是key
                                       */
 
    //是否有connection-name
    CLI_GET_PARAMETER_ASSERT(pszConnectionName, "connection-name");

    //是否有hostname
    CLI_GET_PARAMETER_ASSERT(pszHostName, "hostname");

    //是否有e-mail
    CLI_GET_PARAMETER_ASSERT(pszEmail, "e-mail");

    //是否有key
    CLI_GET_PARAMETER_ASSERT(pszKey, "key");

    //是否存在相同的hostname
	if(CLI_judge_hostname(pszHostName) == TBS_FAILED)
	{
        return TBS_FAILED;
	}
	//填充WANConnectionID
    if (CLI_GetPathByConnName(pszConnectionName, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    /*
     *添加实例
     */
    //发送ADD消息
    if (CLI_AddObjectEx(CLI_DDNS_PATH, &ulInstIndex) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    /*
     *设置值
     */

    //填充DDNS类型
    sprintf(aszPath[0], "%s%lu.DDNSProvider", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[0], "%s", DDNS_PROVIDER_TZO);

    sprintf(aszPath[1], "%s%lu.WANConnectionID", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[1], "%s", szPath);

    //填充hostname
    sprintf(aszPath[2], "%s%lu.Hostname", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[2], "%s", pszHostName);

    //填充Email
    sprintf(aszPath[3], "%s%lu.Email", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[3], "%s", pszEmail);

    //填充Key
    sprintf(aszPath[4], "%s%lu.Key", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[4], "%s", pszKey);

    //设置
    if (CLI_SetLeaf(5, aszPath, aszValue) != TBS_SUCCESS)
    {  
        return TBS_FAILED;
    }

    //设置值
    return TBS_SUCCESS;
}

/*************************************************************************
功能: 添加dyndns类型的DDNS，包括设置值
参数: 无
返回: TBS_SUCCESS :添加成功
      TBS_FAILED  :添加失败
备注: 处理命令:
        ddns add dyndns <connection-name> <hostname> <username> <password> 
      中<connection-name> <hostname> <username> <password>部分
*************************************************************************/
static ULONG CLI_AddDyndns(void)
{
    ULONG ulInstIndex        = 0;//实例号
    char *pszConnectionName  = NULL;
    char *pszHostName        = NULL;
    char *pszUsername        = NULL;
    char *pszPassword        = NULL;
    char szPath[MAX_PATH_LEN]= {0} ;
    char aszPath[5][CLI_NODE__LEN] ;
    char aszValue[5][CLI_NODE__LEN];   /*
                                       *有5个值要设，一个是DDNS类型，这里
                                       *是TZO，第二个是WANConnectionID，
                                       *第三个是hostname，第四个是e-mail，
                                       *第五个是key
                                       */
 
    //是否有connection-name
    CLI_GET_PARAMETER_ASSERT(pszConnectionName, "connection-name");

    //是否有hostname
    CLI_GET_PARAMETER_ASSERT(pszHostName, "hostname");

    //是否有username
    CLI_GET_PARAMETER_ASSERT(pszUsername, "username");

    //是否有password
    CLI_GET_PARAMETER_ASSERT(pszPassword, "password");

    //是否存在相同的hostname
	if(CLI_judge_hostname(pszHostName) == TBS_FAILED)
	{
        return TBS_FAILED;
	}

    //填充WANConnectionID
    if (CLI_GetPathByConnName(pszConnectionName, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    /*
     *添加实例
     */
    //发送ADD消息
    if (CLI_AddObjectEx(CLI_DDNS_PATH, &ulInstIndex) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    /*
     *设置值
     */

    //填充DDNS类型
    sprintf(aszPath[0], "%s%lu.DDNSProvider", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[0], "%s", DDNS_PROVIDER_DYNDNS);

    sprintf(aszPath[1], "%s%lu.WANConnectionID", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[1], "%s", szPath);

    //填充hostname
    sprintf(aszPath[2], "%s%lu.Hostname", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[2], "%s", pszHostName);

    //填充Email
    sprintf(aszPath[3], "%s%lu.Username", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[3], "%s", pszUsername);

    //填充Key
    sprintf(aszPath[4], "%s%lu.Password", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[4], "%s", pszPassword);

    //设置
    if (CLI_SetLeaf(5, aszPath, aszValue) != TBS_SUCCESS)
    {  
        return TBS_FAILED;
    }

    //设置值
    return TBS_SUCCESS;
}

/*************************************************************************
功能: 添加gnudip类型的DDNS，包括设置值
参数: 无
返回: TBS_SUCCESS :添加成功
      TBS_FAILED  :添加失败
备注: 处理命令:
        ddns add gnudip <connection-name> <hostname> <ddns-server> <username> <password> 
      中<connection-name> <hostname> <ddns-server> <username> <password>部分
*************************************************************************/
static ULONG CLI_AddGnudip(void)
{
    ULONG ulInstIndex        = 0;//实例号
    char *pszConnectionName  = NULL;
    char *pszHostName        = NULL;
    char *pszDdnsServer      = NULL;
    char *pszUsername        = NULL;
    char *pszPassword        = NULL;
    char szPath[MAX_PATH_LEN]= {0} ;
    char aszPath[6][CLI_NODE__LEN] ;
    char aszValue[6][CLI_NODE__LEN];   /*
                                       *有5个值要设，一个是DDNS类型，这里
                                       *是TZO，第二个是WANConnectionID，
                                       *第三个是hostname，第四个是e-mail，
                                       *第五个是key
                                       */
 
    //是否有connection-name
    CLI_GET_PARAMETER_ASSERT(pszConnectionName, "connection-name");

    //是否有hostname
    CLI_GET_PARAMETER_ASSERT(pszHostName, "hostname");

    //是否有ddns-server
    CLI_GET_PARAMETER_ASSERT(pszDdnsServer, "ddns-server");

    //是否有username
    CLI_GET_PARAMETER_ASSERT(pszUsername, "username");

    //是否有password
    CLI_GET_PARAMETER_ASSERT(pszPassword, "password");

    //是否存在相同的hostname
	if(CLI_judge_hostname(pszHostName) == TBS_FAILED)
	{
        return TBS_FAILED;    
	}

	//填充WANConnectionID
    if (CLI_GetPathByConnName(pszConnectionName, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    /*
     *添加实例
     */
    //发送ADD消息
    if (CLI_AddObjectEx(CLI_DDNS_PATH, &ulInstIndex) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    /*
     *设置值
     */

    //填充DDNS类型
    sprintf(aszPath[0], "%s%lu.DDNSProvider", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[0], "%s", DDNS_PROVIDER_GNUDIP);

    sprintf(aszPath[1], "%s%lu.WANConnectionID", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[1], "%s", szPath);

    //填充hostname
    sprintf(aszPath[2], "%s%lu.Hostname", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[2], "%s", pszHostName);

    //填充hostname
    sprintf(aszPath[3], "%s%lu.DdnsHost", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[3], "%s", pszDdnsServer);

    //填充Email
    sprintf(aszPath[4], "%s%lu.Username", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[4], "%s", pszUsername);

    //填充Key
    sprintf(aszPath[5], "%s%lu.Password", CLI_DDNS_PATH, ulInstIndex);
    sprintf(aszValue[5], "%s", pszPassword);

    //设置
    if (CLI_SetLeaf(6, aszPath, aszValue) != TBS_SUCCESS)
    {  
        return TBS_FAILED;
    }

    //设置值
    return TBS_SUCCESS;
}


/*************************************************************************
功能: 处理删除操作的函数
参数: 无
返回: TBS_SUCCESS :添加成功
      TBS_FAILED  :添加失败
备注: 处理如下命令:
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

    //是否是del操作
    CLI_GET_PARAMETER_ASSERT(pOperation, "del");

    //获取hostname参数
    CLI_GET_PARAMETER_ASSERT(pHostName, "hostname");

    //查找hostname对应的DDNS实例的路径,
    //因为hostname是唯一的
    if ((pstMsg = CLI_GetNode(CLI_DDNS_PATH)) == NULL) 
    {
        return TBS_FAILED;
    }

    /*
     *从消息中解析出路径，如:
     *InternetGatewayDevice.X_TWSZ-COM_DDNS.1.
     */
     
    //获取count，需要跳过result字段
    ulCount = *(ULONG*)(pstMsg->szMsgBody + sizeof(ULONG));
    //这个ulCount值不能太大
    if (ulCount > 1024)
    {
        return TBS_FAILED;
    }

    //指向item，但要跳过result和item count连个字段
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
                //判断hostname值是否相同
                if (strcmp(pHostName, pPos+1) == 0)
                {
                    /*
                     *跳过前面的"string "，共7个字节
                     *因为CMM返回的一个item类似这样的:
                     *"string InternetGatewayDevice.X_TWSZ-COM_DDNS.1.Hostname=test.tzo.com"
                     */
                    strncpy(szDDNSInstPath, pCurPos+7, pTempPos-(pCurPos+7));

                    break;
                }
            }
        }

        //指向下一个item
        pCurPos += strlen(pCurPos) + 1;
    }

    //如果找不到
    if (strlen(szDDNSInstPath) == 0)
    {
        return TBS_FAILED;
    }
    //找到了，删除之
    else if (CLI_DelObject(szDDNSInstPath) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
     
    return TBS_SUCCESS;
}

//处理show操作的函数
static ULONG CLI_Show(void)
{
    ST_MSG *pstMsg = NULL;

    ULONG ulCount  = 0;
    ULONG ulInstID = 0;         //DDNS实例ID号
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

    //获取InternetGatewayDevice.X_TWSZ-COM_DDNS.下的实例
    pstMsg = CLI_GetNode(CLI_DDNS_PATH);
    if (pstMsg == NULL)
    {
        return TBS_FAILED;
    }

    /*
     *遍历并显示
     */

    //获取item count,要跳过result字段
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    //item count不能太大
    if (ulCount > 1024)
    {
        return TBS_FAILED;
    }

    //指向item,需要跳过result字段和item count字段
    pCurPos = pstMsg->szMsgBody + sizeof(ULONG)*2;

    //遍历
    for (ulTemp=0; ulTemp<ulCount; )
    {
#if 1
        /*
         *把实例号相同的item在do..while中处理完，这要求
         *实例号相同的item在返回的消息体中是连续的
         */

        //先初始化
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
             *一个item类似于:
             *"string InternetGatewayDevice.X_TWSZ-COM_DDNS.1.Key=ss"
             */
          
            

            //获取叶节点名，因为这时的szTemp的值类似:Name=Value
            pPos = strchr(szTemp, '=');
            *pPos = '\0';

            //获取节点值
            strcpy(szValue, ++pPos);

            //指向下一个item
            if (pCurPos)
            {
                pCurPos += (strlen(pCurPos) + 1);
                ulTemp++;
            }
            
            

            //把DDNS信息保存在相应的变量中
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

            //ulInstID为0表明这个item是第一个item，这时还没获取DDNS实例的信息
            if (((ulTempID != ulInstID) && (ulInstID != 0))         //不止一个DDNS实例的情况
                || (ulTemp >= ulCount))                           //针对只有一个DDNS实例的情况
            {
                //显示DDNS信息
                CLI_ShowOneDdnsInst(szKey, szEMail,
                                    szPassword,
                                    szUsername,
                                    szConnName,
                                    szDdnsState,
                                    szHostname,
                                    szDdnsType,
                                    szDdnsHost);

                //开始下一个DDNS实例信息的获取
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
    //这几个不能为空
    if ((pszDDNSProvider == NULL)
        || (pszHostname == NULL)
        || (pszConnName == NULL)
        || (pszHostname == NULL))
    {
        return ;
    }
   
    //显示DYNDNS的信息
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
    //显示TZO的信息
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
    //显示GNUDIP的信息
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

//处理commit操作的函数
static ULONG CLI_Commit(void)
{
    if (CLI_SendMsgWithoutBody(MSG_CMM_COMMIT) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

//处理cancel操作的函数
static ULONG CLI_Cancel(void)
{
    if (CLI_SendMsgWithoutBody(MSG_CMM_CANCEL) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}



/*************************************************************************
功能: 发送MSG_CMM_COMMIT消息(MID_CLI=>MID_CMM)
参数: 无
返回: TBS_SUCCESS:发送成功
      TBS_FAILED :发送失败
备注:   
*************************************************************************/
ULONG CLI_SendMsgWithoutBody(unsigned short usMsgType)
{
    ST_MSG *pstMsg;      /* 发送的消息指针 */

    //分配消息空间
    pstMsg = MSG_CreateMessage(sizeof(pstMsg->stMsgHead));
    if (pstMsg == NULL)
    {
        return TBS_FAILED;
    }

    //初始化消息
    memset(&(pstMsg->stMsgHead), 0, sizeof(pstMsg->stMsgHead));
    pstMsg->stMsgHead.ulBodyLength = 0;
    pstMsg->stMsgHead.usDstMID     = MID_CMM;
    pstMsg->stMsgHead.usMsgType    = usMsgType;
    pstMsg->stMsgHead.usSrcMID     = MID_CLI;

    //发送消息
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
