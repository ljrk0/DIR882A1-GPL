
#include "../cli_pub.h"
#include "../cli/cli_term.h"
#include "../cli/cli_io.h"
#include "../cli/cli_interpret.h"
#include "cli_cmd.inc"

#include "../cli_comm.h"
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/
#define CMD_NAME_MAX_LEN        31

#define CLI_WAN_IP_CONNECTION_NAME      "string InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANIPConnection.%lu.Name=%s"
#define CLI_WAN_PPP_CONNECTION_NAME     "string InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANPPPConnection.%lu.Name=%s"
#define CLI_WAN_DEVICE_BASE             "InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice."
#define CLI_WAN_IP_CONNECTION           "WANIPConnection"
#define CLI_WAN_PPP_CONNECTION          "WANPPPConnection"
#define CLI_WAN_DEVICE_BASE_ETH             "InternetGatewayDevice.WANDevice.2.WANConnectionDevice."
#define CLI_WAN_DEVICE_BASE_DSL             "InternetGatewayDevice.WANDevice.1.WANConnectionDevice."

#define CLI_DNS_TRACE(x) //IO_Print x

enum{
  CLI_WAN_CONN_NAME = 0,
  CLI_WAN_CONN_DNS,

  CLI_WAN_CONN_STATUS,  //yuan
  CLI_WAN_CONN_PVC,
  CLI_WAN_CONN_DSLTYPE,
  CLI_WAN_CONN_VLANID,

  CLI_WAN_CONN_END
};

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

#define GET_LEFT_TO_VAL(tmpPath, formal, basePath, Node, Val) do \
	{ \
		sprintf((tmpPath),(formal), (basePath), (Node));\
		if( CLI_GetLeaf((tmpPath), (Val)) != TBS_SUCCESS) \
		{ \
			return TBS_FAILED; \
		} \
	}while(0);

/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/
 static ULONG CLI_GetPathByConnName(char *pszConnName, char *pszPath, int iLen);
 static ULONG CLI_DNS_Set(void);
static ULONG CLI_DNS_Show(void);

//主函数
ULONG CLI_Dns(void)
{
    if (CLI_GetParamByName("set") != NULL)
    {
        CLI_WAN__OPERATION_CALL(CLI_DNS_Set);
    }

    if (CLI_GetParamByName("show") != NULL)
    {
        CLI_WAN__OPERATION_CALL(CLI_DNS_Show);
    }
    
    return TBS_FAILED;
}
/*************************************************************************
功能: 处理添加操作的函数
参数: 无
返回: TBS_SUCCESS :添加成功
      TBS_FAILED  :添加失败
备注: 
*************************************************************************/
static ULONG CLI_DNS_Set(void)
{
    char *pszServerName        = NULL;
    char *pszWanInterface      = NULL;

    char szPath[MAX_PATH_LEN]  = {0};
    char aszPath[1][CLI_NODE__LEN];
    char aszValue[1][CLI_NODE__LEN];

    //Check if get parameter of wan interface
    CLI_GET_PARAMETER_ASSERT(pszWanInterface, "wan");

    //Check if get parameter of protocol
    CLI_GET_PARAMETER_ASSERT(pszServerName, "server");

    //Get the path of the wan interface node
    if (CLI_GetPathByConnName(pszWanInterface, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    CLI_DNS_TRACE(("Get the node path : %s\n", szPath));
    
    //fill the elements of the NATP rule
    sprintf(aszPath[0], "%s.DNSServers", szPath);
    sprintf(aszValue[0], "%s", pszServerName);

    //Begin set the values
    if (CLI_SetLeaf(1, aszPath, aszValue) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
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

//处理show操作的函数
static ULONG CLI_DNS_Show(void)
{
	char  tmp_Path[MAX_PATH_LEN];
	char * pTmpPathPos = NULL;
	char * pTmpPathCurPos = NULL;
    ST_MSG *pstMsg = NULL;
    char *pCurPos  = NULL;
    char *pTempPos = NULL;
    ULONG ulCount  = 0;
    ULONG ulTemp   = 0;
    char *pszName    = NULL;
    char szPath[MAX_PATH_LEN] = {0};
    char szPathSub[MAX_PATH_LEN] = {0};
    char szValue[256] = {0};
	char aszPvcData[CLI_WAN_CONN_END][256];
    ULONG  ulIndex1= 0;
    ULONG  ulIndex2= 0;
    ULONG  ulIndex3= 0;
    char szWanMode[MAX_NODE_VALUE_LEN] = {0};
	
    pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE_DSL);
    if (pstMsg == NULL)
    {
        CLI_DNS_TRACE(("%s: Failed to the node of %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE_DSL));
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_DNS_TRACE(("%s: The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    IO_Print("\nDSL list\n");
	IO_Print("%18s%12s%15s\t%15s\n", "Name", "PVC","Status", "DNS");
    for(; ulTemp < ulCount; ulTemp++)
    {
        pTempPos = strstr(pCurPos, ".Name=");
        if (pTempPos != NULL)
        {
            //IO_Print("  %s\n", pTempPos + strlen(".Name="));
		 	memset(szPath, 0 , MAX_PATH_LEN);
			memset(szPathSub, 0, MAX_PATH_LEN);
			memset(tmp_Path, 0, MAX_PATH_LEN);
			memset(aszPvcData, 0, CLI_WAN_CONN_END * 256);
			
		 	safe_strncpy( aszPvcData[CLI_WAN_CONN_NAME], pTempPos + strlen(".Name="),256);
			
			if (CLI_GetPathByConnName(aszPvcData[CLI_WAN_CONN_NAME], szPath, MAX_PATH_LEN) != TBS_SUCCESS)
	        {
    		    CLI_DNS_TRACE(("\n%s: Failed to get the path of : %s\n", __FUNCTION__, pszName));
	            return TBS_FAILED;
	        }
            CLI_DNS_TRACE(("  %s\n", szPath));
			// szPath is InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection/WANPPPConnection.{i}
			// back to  InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.
            sscanf(szPath, "InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.%s.%lu", &ulIndex1, &ulIndex2, szWanMode, &ulIndex3 );
            sprintf(szPathSub, "InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.", ulIndex1, ulIndex2);
            CLI_DNS_TRACE(("  %s\n", szPathSub));
            
			GET_LEFT_TO_VAL(tmp_Path, "%s.%s", szPath, "ConnectionStatus", szValue);
			if( safe_strcmp(szValue, "Connected") != 0)
			{
				sprintf(aszPvcData[CLI_WAN_CONN_STATUS], "%s", "Disconnect");
			}else{	
            	sprintf(aszPvcData[CLI_WAN_CONN_STATUS], "%s", szValue);
			}

            GET_LEFT_TO_VAL(tmp_Path, "%s.%s", szPath, "DNSServers", szValue);
            sprintf(aszPvcData[CLI_WAN_CONN_DNS], "%s", szValue);	 
            
			GET_LEFT_TO_VAL(tmp_Path, "%s%s", szPathSub, "X_TWSZ-COM_DSLType", szValue);
			sprintf(aszPvcData[CLI_WAN_CONN_DSLTYPE], "%s", szValue);

			GET_LEFT_TO_VAL(tmp_Path, "%s%s", szPathSub, "X_TWSZ-COM_VLANID", szValue);
			sprintf(aszPvcData[CLI_WAN_CONN_VLANID], "%s", szValue);
			
			if (safe_strcmp(aszPvcData[CLI_WAN_CONN_DSLTYPE], "ATM") == 0)
			{
				GET_LEFT_TO_VAL(tmp_Path, "%s%s", szPathSub, "WANDSLLinkConfig.DestinationAddress", szValue);
				sprintf(aszPvcData[CLI_WAN_CONN_PVC], "%s", szValue);
			}else{
				sprintf(aszPvcData[CLI_WAN_CONN_PVC], "%s", "PTM");
			}

//				IO_Print("%15s%15s%15s%15s%15s\n", "Name", "PVC", "VLanId","Protocol","Status");	
			IO_Print("%18s%12s%15s\t%15s\n", aszPvcData[CLI_WAN_CONN_NAME], 
			                                  aszPvcData[CLI_WAN_CONN_PVC],
											  aszPvcData[CLI_WAN_CONN_STATUS],
											  aszPvcData[CLI_WAN_CONN_DNS]);	
        }

        pCurPos += (strlen(pCurPos) + 1);
    }

#ifdef CONFIG_APPS_LOGIC_ETHWAN

	ulTemp = 0;
	ulCount = 0;
	
	pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE_ETH);
    if (pstMsg == NULL)
    {
        CLI_DNS_TRACE(("%s: Failed to the node of %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE_ETH));
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_DNS_TRACE(("%s: The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    IO_Print("\nEthernet list:\n");
	IO_Print("%18s%8s%15s\t%15s\n", "Name", "VLanId","Status", "DNS");
    for(; ulTemp < ulCount; ulTemp++)
    {
        pTempPos = strstr(pCurPos, ".Name=");
        if (pTempPos != NULL)
        {
         //   IO_Print("  %s\n", pTempPos + strlen(".Name="));
		 	memset(szPath, 0 , MAX_PATH_LEN);
			memset(szPathSub, 0, MAX_PATH_LEN);
			memset(tmp_Path, 0, MAX_PATH_LEN);
			memset(aszPvcData, 0, CLI_WAN_CONN_END * 256);
			
		 	safe_strncpy( aszPvcData[CLI_WAN_CONN_NAME], pTempPos + strlen(".Name="),256);
			
			if (CLI_GetPathByConnName(aszPvcData[CLI_WAN_CONN_NAME], szPath, MAX_PATH_LEN) != TBS_SUCCESS)
	        {
    		    CLI_DNS_TRACE(("\n%s: Failed to get the path of : %s\n", __FUNCTION__, pszName));
	            return TBS_FAILED;
	        }
			// szPath is InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection/WANPPPConnection.{i}
			// back to  InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.
            sscanf(szPath, "InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.%s.%lu", &ulIndex1, &ulIndex2, szWanMode, &ulIndex3 );
            sprintf(szPathSub, "InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.", ulIndex1, ulIndex2);

			GET_LEFT_TO_VAL(tmp_Path, "%s.%s", szPath, "ConnectionStatus", szValue);
			if( safe_strcmp(szValue, "Connected") != 0)
			{
				sprintf(aszPvcData[CLI_WAN_CONN_STATUS], "%s", "Disconnect");
			}else{	
            	sprintf(aszPvcData[CLI_WAN_CONN_STATUS], "%s", szValue);
			}

            GET_LEFT_TO_VAL(tmp_Path, "%s.%s", szPath, "DNSServers", szValue);
            sprintf(aszPvcData[CLI_WAN_CONN_DNS], "%s", szValue);			            
	
            GET_LEFT_TO_VAL(tmp_Path, "%s%s", szPathSub, "X_TWSZ-COM_VLANID", szValue);
			sprintf(aszPvcData[CLI_WAN_CONN_VLANID], "%s", szValue);

//				IO_Print("%15s%15s%15s%15s%15s\n", "Name", "PVC", "VLanId","Protocol","Status");	
			IO_Print("%18s%8s%15s\t%15s\n", aszPvcData[CLI_WAN_CONN_NAME], 
											  aszPvcData[CLI_WAN_CONN_VLANID],
											  aszPvcData[CLI_WAN_CONN_STATUS],
											  aszPvcData[CLI_WAN_CONN_DNS]);	
        }

        pCurPos += (strlen(pCurPos) + 1);
    }
#endif	
    return TBS_SUCCESS;
}
/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif
