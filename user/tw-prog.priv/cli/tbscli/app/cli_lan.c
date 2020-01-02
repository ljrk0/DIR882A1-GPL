
/****************************************************************************
 文件名  : CLI_lan.c
 作者    : Huang Mengxiang
 版本    :
 完成日期:
 文件描述: 对lan(包括DHCP)进行设置或显示
 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
 ****************************************************************************/

#include "../cli_pub.h"
#include "../cli/cli_term.h"
#include "../cli/cli_io.h"
#include "../cli/cli_interpret.h"
#include "cli_cmd.inc"

#include "../cli_comm.h"


#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
*						MACRO										   *
**********************************************************************/
#define  CLI_LAN_DHCPSERVER_ENABLE        "1"
#define  CLI_LAN_DHCPSERVER_DISABLE       "0"

#define  CLI_LAN_IPINTERFACE_ENABLE       "1"
#define  CLI_LAN_IPINTERFACE_DISABLE      "0"

#define  CLI_LAN_IPINTERFACE_SIZEBIT       10

#define  CLI_LAN_PRINT                                  printf

#define CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH       "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement."
#define CLI_LAN_IPINTERFACE_PATH                                   "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.IPInterface."

#define NODE_DHCP_PORT_WLAN1 "InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.X_TWSZ-COM_DHCPEnabled"
#define NODE_DHCP_PORT_WLAN2 "InternetGatewayDevice.LANDevice.1.WLANConfiguration.2.X_TWSZ-COM_DHCPEnabled"
#define NODE_DHCP_PORT_WLAN3 "InternetGatewayDevice.LANDevice.1.WLANConfiguration.3.X_TWSZ-COM_DHCPEnabled"
#define NODE_DHCP_PORT_WLAN4 "InternetGatewayDevice.LANDevice.1.WLANConfiguration.4.X_TWSZ-COM_DHCPEnabled"
#define NODE_DHCP_PORT_LAN1     "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.1.DhcpServerEnable"
#define NODE_DHCP_PORT_LAN2     "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.2.DhcpServerEnable"
#define NODE_DHCP_PORT_LAN3     "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.3.DhcpServerEnable"
#define NODE_DHCP_PORT_LAN4     "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.4.DhcpServerEnable"


/**************************************************/
/* MACRO for DHCP                                                         */
/***************************************************/
#define CLI_LAN_DHCP_OPERATION_CALL(pFun) do \
{ \
    if (pFun() == TBS_SUCCESS) \
    { \
        return TBS_SUCCESS;\
    } \
}while(0);


/**************************************************/
/* MACRO for LANIPInterface                                              */
/***************************************************/
#define CLI_LAN_IPInterface_OPERATION_CALL(pFun) do \
{ \
    if (pFun() == TBS_SUCCESS) \
    { \
        return TBS_SUCCESS;\
    } \
}while(0);


/*********************************************************************
*						FUNCTION									   *
**********************************************************************/

static ULONG CLI_DHCP_Show(void);
static ULONG CLI_DHCP_Config(void);
static ULONG CLI_DHCP_Enable(void);
static ULONG CLI_DHCP_Disable(void);
static ULONG CLI_DHCP_Port(void);

static ULONG CLI_IPInterface_Show(void);
static ULONG CLI_IPInterface_Config(void);
static ULONG CLI_IPInterface_Enable(void);
static ULONG CLI_IPInterface_Disable(void);

static ULONG CLI_IPInterface_Show_ExactInterface(char* szInterface);
static ULONG CLI_IPInterface_Enable_ExactInterface(char* szInterface,BOOL bEnable);


/************************************************************************/
/* 函数功能: 设置获取LAN IPInterface信息用                     */
/*************************************************************************/
ULONG CLI_LAN_IPInterface(void)
{
    if(CLI_GetParamByName("show") != NULL)
    {
        CLI_LAN_IPInterface_OPERATION_CALL(CLI_IPInterface_Show);
    }
    else if(CLI_GetParamByName("config") != NULL)
    {
        CLI_LAN_IPInterface_OPERATION_CALL(CLI_IPInterface_Config);
    }
    else if(CLI_GetParamByName("enable") != NULL)
    {
        CLI_LAN_IPInterface_OPERATION_CALL(CLI_IPInterface_Enable);
    }
    else if(CLI_GetParamByName("disable") != NULL)
    {
        CLI_LAN_IPInterface_OPERATION_CALL(CLI_IPInterface_Disable);
    }
    else
        return TBS_FAILED;

    return TBS_SUCCESS;
}


/*********************************************************************/
/* 函数功能 :设置获取DHCP信息用                             */
/*********************************************************************/

ULONG CLI_DHCPServer(void)
{
    if(CLI_GetParamByName("show") != NULL)
    {
        CLI_LAN_DHCP_OPERATION_CALL(CLI_DHCP_Show);
    }
    else if(CLI_GetParamByName("config") != NULL)
    {
        CLI_LAN_DHCP_OPERATION_CALL(CLI_DHCP_Config);
    }
    else if(CLI_GetParamByName("enable") != NULL)
    {
        CLI_LAN_DHCP_OPERATION_CALL(CLI_DHCP_Enable);
    }
    else if(CLI_GetParamByName("disable") != NULL)
    {
        CLI_LAN_DHCP_OPERATION_CALL(CLI_DHCP_Disable);
    }
    else if(CLI_GetParamByName("port") != NULL)
    {
        CLI_LAN_DHCP_OPERATION_CALL(CLI_DHCP_Port);
    }
    else
        return TBS_FAILED;

    return TBS_SUCCESS;
}


/*************************************************************************
功能:查询DHCP的当前信息
参数: 无
返回: TBS_SUCCESS :添加成功
      
备注: 查询返回的信息有
		MinAddress :
		MaxAddress:
		DHCPLeaseTime:
		DNSServer address:
       
*************************************************************************/
static ULONG CLI_DHCP_Show(void)
{
    int nIfNum = -1;
    char aszNextLevel[CLI_MAX_OBJ_NUM][CLI_LEAF__LEN];
    nIfNum = CLI_GetNextLevelName(CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH, aszNextLevel);
    if (nIfNum < 0)
    {
        IO_Print("\n No configured DHCPServer interface.");
        return TBS_SUCCESS;
    }
    
    char aszLeafName[5][CLI_LEAF__LEN] = {"DHCPServerEnable","MinAddress", "MaxAddress", "DHCPLeaseTime", "DNSServers"};
    char szObjName[256] = {0} ;
    ST_CLISTR_LINK *pStrList;
	  
    if(STB_StrLen(CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH) > sizeof(szObjName) - 1)
    {
        IO_Print("Leaf length larger than CLI_LEAF__LEN(512)\n");
        return TBS_FAILED;
    }
    sprintf(szObjName, "%s", CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH);
    pStrList = CLI_GetObject(szObjName, 5, aszLeafName);
    if (pStrList != NULL)
    {
        IO_Print("\n DHCPServerPath %s:"
             "\n     DHCPServerEnable : %s"
             "\n     MinAddress      : %s"
             "\n     MaxAddress    : %s"
             "\n     DHCPLeaseTime : %s"
             "\n     DNSServer: %s", szObjName , pStrList->pstrValue,
             pStrList->pNext->pstrValue, pStrList->pNext->pNext->pstrValue,pStrList->pNext->pNext->pNext->pstrValue,pStrList->pNext->pNext->pNext->pNext->pstrValue);
        CLI_DestroyStrLink(pStrList);
    }
    
    return TBS_SUCCESS;
}

/**********************************************************************************
功能:配置DHCP信息
参数: 无
返回: TBS_SUCCESS : 配置成功
	    : TBS_FAILED : 配置不成功
备注: 配置的信息有
	      MinAddress:
	      MaxAddress:
	      DHCPLeaseTime:
	      DNSServer address:
**********************************************************************************/
static ULONG CLI_DHCP_Config(void)
{
    char   *szStartIPAddress = NULL;
    char   *szEndIPAddress = NULL;
    char   *szLeaseTime= NULL;
    char   *szDnsServer = NULL;
    char szLanip[56] = {0};
    int a1,a2,a3,a4;
    int b1,b2,b3,b4;
    char aszName[6][CLI_NODE__LEN];
    char aszVal[6][CLI_NODE__LEN];
    szStartIPAddress = CLI_GetParamByName("start-IP-address");
    szEndIPAddress = CLI_GetParamByName("end-IP-address");
    szLeaseTime = CLI_GetParamByName("lease-time");
    szDnsServer = CLI_GetParamByName("dnsserver");

    if (szStartIPAddress == NULL || szEndIPAddress == NULL || szLeaseTime == NULL || szDnsServer == NULL)
    {
        IO_Print("Term Parameter error.");
        return TBS_FAILED;
    }

    sprintf(aszName[0], "%s%s", CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH, "MinAddress");
    sprintf(aszName[1], "%s%s", CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH, "MaxAddress");
    sprintf(aszName[2], "%s%s", CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH,  "DHCPLeaseTime");
    sprintf(aszName[3], "%s%s", CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH,  "DNSServers");
    sprintf(aszName[4], "%s%s", CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH,  "IPRouters");
    sprintf(aszName[5], "%s%s", CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH,  "IPInterface.1.IPInterfaceIPAddress");

    strcpy(aszVal[0], szStartIPAddress);
    strcpy(aszVal[1], szEndIPAddress);
    strcpy(aszVal[2], szLeaseTime);
    strcpy(aszVal[3], szDnsServer);
    /*获取lanIP地址*/
    CLI_GetLeaf(aszName[4], szLanip);
    if(strlen(szLanip)!=0)
    {
        sscanf(szLanip,"%d.%d.%d.%d",&a1,&a2,&a3,&a4);
    }
    sscanf(aszVal[0],"%d.%d.%d.%d",&b1,&b2,&b3,&b4);
    sprintf(aszVal[4],"%d.%d.%d.%d",b1,b2,b3,a4);
    sprintf(aszVal[5],"%d.%d.%d.%d",b1,b2,b3,a4);
    if (CLI_SetLeaf(6, aszName, aszVal)  != TBS_SUCCESS)
    {
        CLI_CommErrorPrint();
        return TBS_FAILED;
    }
    
    return TBS_SUCCESS;
}

/******************************************************************************************
功能: 使DHCP生效
参数: 无
返回: TBS_SUCCESS : 成功
           : TBS_FAILED : 不成功
备注: 无
*******************************************************************************************/
static ULONG CLI_DHCP_Enable(void)
{
    char aszName[1][CLI_NODE__LEN];
    char aszVal[1][CLI_NODE__LEN];

    sprintf(aszName[0],"%s%s",CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH,"DHCPServerEnable");
    strcpy(aszVal[0],CLI_LAN_DHCPSERVER_ENABLE);
    if(CLI_SetLeaf(1,aszName,aszVal))
    {
        CLI_CommErrorPrint();
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}

/******************************************************************************************
功能: 使DHCP生效
参数: 无
返回: TBS_SUCCESS : 成功
           : TBS_FAILED : 不成功
备注: 无
*******************************************************************************************/

static ULONG CLI_DHCP_Disable(void)
{
    char aszName[1][CLI_NODE__LEN];
    char aszVal[1][CLI_NODE__LEN];

    sprintf(aszName[0],"%s%s",CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH,"DHCPServerEnable");
    strcpy(aszVal[0],CLI_LAN_DHCPSERVER_DISABLE);
    if(CLI_SetLeaf(1,aszName,aszVal))
    {
        CLI_CommErrorPrint();
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}

/******************************************************************************************
功能: 使能DHCP 端口绑定
参数: 无
返回: TBS_SUCCESS : 成功
           : TBS_FAILED : 不成功
备注: 无
*******************************************************************************************/
static ULONG CLI_DHCP_Port_Lan(void)
{
    char *szInterface = NULL;
    char *szEnable = NULL;
    char aszName[1][CLI_NODE__LEN];
    char aszVal[1][CLI_NODE__LEN];    
    szInterface = CLI_GetParamByName("interface");
    szEnable = CLI_GetParamByName("enable-disable");
    
    switch (*szInterface)
    {
        case '1':
            sprintf(aszName[0], "%s", NODE_DHCP_PORT_LAN1);
            if(0 == strcmp(szEnable, "enable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_ENABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }
            else if(0 == strcmp(szEnable, "disable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_DISABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }            
            return TBS_SUCCESS;
        case '2':
            sprintf(aszName[0], "%s", NODE_DHCP_PORT_LAN2);
            if(0 == strcmp(szEnable, "enable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_ENABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }
            else if(0 == strcmp(szEnable, "disable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_DISABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }            
            return TBS_SUCCESS;
        case '3':
            sprintf(aszName[0], "%s", NODE_DHCP_PORT_LAN3);
            if(0 == strcmp(szEnable, "enable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_ENABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }
            else if(0 == strcmp(szEnable, "disable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_DISABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }            
            return TBS_SUCCESS;
       case '4':
            sprintf(aszName[0], "%s", NODE_DHCP_PORT_LAN4);
            if(0 == strcmp(szEnable, "enable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_ENABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }
            else if(0 == strcmp(szEnable, "disable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_DISABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }            
            return TBS_SUCCESS;        
    }
    return TBS_FAILED;
}

/******************************************************************************************
功能: 使能DHCP 端口绑定
参数: 无
返回: TBS_SUCCESS : 成功
           : TBS_FAILED : 不成功
备注: 无
*******************************************************************************************/
static ULONG CLI_DHCP_Port_Wlan(void)
{
    char *szInterface = NULL;
    char *szEnable = NULL;
    char aszName[1][CLI_NODE__LEN];
    char aszVal[1][CLI_NODE__LEN];    
    szInterface = CLI_GetParamByName("interface");
    szEnable = CLI_GetParamByName("enable-disable");
    
    switch (*szInterface)
    {
        case '1':
            sprintf(aszName[0], "%s", NODE_DHCP_PORT_WLAN1);
            if(0 == strcmp(szEnable, "enable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_ENABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }
            else if(0 == strcmp(szEnable, "disable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_DISABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }            
            return TBS_SUCCESS;
        case '2':
            sprintf(aszName[0], "%s", NODE_DHCP_PORT_WLAN2);
            if(0 == strcmp(szEnable, "enable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_ENABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }
            else if(0 == strcmp(szEnable, "disable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_DISABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }            
            return TBS_SUCCESS;
        case '3':
            sprintf(aszName[0], "%s", NODE_DHCP_PORT_WLAN3);
            if(0 == strcmp(szEnable, "enable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_ENABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }
            else if(0 == strcmp(szEnable, "disable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_DISABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }            
            return TBS_SUCCESS;
       case '4':
            sprintf(aszName[0], "%s", NODE_DHCP_PORT_WLAN4);
            if(0 == strcmp(szEnable, "enable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_ENABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }
            else if(0 == strcmp(szEnable, "disable"))
            {
                strcpy(aszVal[0], CLI_LAN_DHCPSERVER_DISABLE);
                if(CLI_SetLeaf(1,aszName,aszVal))
                {
                    CLI_CommErrorPrint();
                    return TBS_FAILED;
                }
            }            
            return TBS_SUCCESS;        
    }
    return TBS_FAILED;
}


/******************************************************************************************
功能: 指定端口使能DHCP功能
参数: 无
返回: TBS_SUCCESS : 成功
           : TBS_FAILED : 不成功
备注: 无
*******************************************************************************************/
static ULONG CLI_DHCP_Port(void)
{
    if(CLI_GetParamByName("lan") != NULL)
    {
        CLI_LAN_DHCP_OPERATION_CALL(CLI_DHCP_Port_Lan);
    }
    else if(CLI_GetParamByName("wlan") != NULL)
    {
        CLI_LAN_DHCP_OPERATION_CALL(CLI_DHCP_Port_Wlan);
    }
        
    return TBS_FAILED;
}


/***************************************************************************************************
功能: 显示IPInterface的信息
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 获得Interface失败
备注: 根据具体的参数如all, 1, 2来显示具体的IPInterface信息
****************************************************************************************************/

static ULONG CLI_IPInterface_Show(void)
{
    if(CLI_GetParamByName("interface") != NULL)  // show the exact interface information
    {
        char  *szInterfaceValue = NULL;
        szInterfaceValue = CLI_GetParamByName("interface");

        if(STB_StrCmp(szInterfaceValue,"all") == 0)
        {
            char szInferfaceExact[CLI_LAN_IPINTERFACE_SIZEBIT];
			
            sprintf(szInferfaceExact,"1");
            CLI_IPInterface_Show_ExactInterface(szInferfaceExact);

            sprintf(szInferfaceExact,"2");
            CLI_IPInterface_Show_ExactInterface(szInferfaceExact);
			
            return TBS_SUCCESS;
        }
        else 
        {
            CLI_IPInterface_Show_ExactInterface( szInterfaceValue);
        }
    }
    else
    {
        IO_Print("\n get interface error .");
        return TBS_FAILED;
    }
    
    return TBS_SUCCESS;
}


/*************************************************************************************************
功能: 显示具体的IPInterface的信息
参数: szInterface: 具体的IPInterface的值如"2"
返回: TBS_SUCCESS: 成功
	    : TBS_FAILED : 失败
备注: 无
**************************************************************************************************/
static ULONG CLI_IPInterface_Show_ExactInterface(char* szInterface)
{
    int nIfNum = -1;
    char aszNextLevel[CLI_MAX_OBJ_NUM][CLI_LEAF__LEN];
    char szInterfacePath[CLI_NODE__LEN];
    sprintf(szInterfacePath, "%s%s.",CLI_LAN_IPINTERFACE_PATH,szInterface);  // the true path of this interface
			
    nIfNum = CLI_GetNextLevelName(szInterfacePath, aszNextLevel);
    if (nIfNum < 0)
	  {
        IO_Print("\n No configured  IPinterface %s.",szInterfacePath);
        return TBS_SUCCESS;
    }

    char aszLeafName[2][CLI_LEAF__LEN] = {"IPInterfaceIPAddress", "IPInterfaceSubnetMask"};
    char szObjName[256] ;
    ST_CLISTR_LINK *pStrList = NULL;

    sprintf(szObjName, "%s%s.", CLI_LAN_IPINTERFACE_PATH, szInterface);
    pStrList = CLI_GetObject(szObjName, 2, aszLeafName);
    if (pStrList != NULL)
    {
        IO_Print("\n Interface %s:"
                "\n     IPInterfaceIPAddress      : %s"
                "\n     IPInterfaceSubnetMask    : %s",szInterfacePath, pStrList->pstrValue,
                pStrList->pNext->pstrValue);
        CLI_DestroyStrLink(pStrList);
    }
    return TBS_SUCCESS;
}


/***************************************************************************************************
功能: 配置IPInterface的信息
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
备注: 无
****************************************************************************************************/
static ULONG CLI_IPInterface_Config(void)
{
    char   *szInterface = NULL;
    char   *szIPAddress = NULL;
    char   *szMask = NULL;
    int  a1,a2,a3,a4;
    int b1,b2,b3,b4;
    int c1,c2,c3,c4;
    int d1,d2,d3,d4;
    char szIppoolStart[64]={0};
    char szIppoolEnd[64]={0};
    char szDns[64] = {0};
    int count;

    char aszName[5][CLI_NODE__LEN];
    char aszVal[5][CLI_NODE__LEN];

    szInterface = CLI_GetParamByName("interface");
    szIPAddress = CLI_GetParamByName("ip");
    szMask = CLI_GetParamByName("mask");

    if (szInterface == NULL || szIPAddress == NULL || szMask == NULL )
    {
        IO_Print("Term Parameter error.");
        return TBS_FAILED;
    }

    sprintf(aszName[0], "%s%s.%s", CLI_LAN_IPINTERFACE_PATH, szInterface,"IPInterfaceIPAddress");
    sprintf(aszName[1], "%s%s.%s", CLI_LAN_IPINTERFACE_PATH, szInterface,"IPInterfaceSubnetMask");
    sprintf(aszName[2], "%s%s", CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH,"MinAddress");
    sprintf(aszName[3], "%s%s", CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH,"MaxAddress");
    sprintf(aszName[4], "%s%s", CLI_LAN_LANHOSTCONFIGMANAGEMENT_PATH,"DNSServers");

    strcpy(aszVal[0], szIPAddress);
    strcpy(aszVal[1], szMask);
    count = 2;
    sscanf (aszVal[0],"%d.%d.%d.%d",&a1,&a2,&a3,&a4);
        
    CLI_GetLeaf(aszName[2], szIppoolStart);
    CLI_GetLeaf(aszName[3], szIppoolEnd);
    CLI_GetLeaf(aszName[4], szDns);
    if(strlen(szIppoolStart)!=0)
    {
        sscanf(szIppoolStart,"%d.%d.%d.%d",&b1,&b2,&b3,&b4);
        sscanf(szIppoolEnd,"%d.%d.%d.%d",&c1,&c2,&c3,&c4);
        sprintf(aszVal[2],"%d.%d.%d.%d",a1,a2,a3,b4);
        sprintf(aszVal[3],"%d.%d.%d.%d",a1,a2,a3,c4); 
        count =4;
    }
    if(strlen(szDns)!=0)
    {
        sscanf(szDns,"%d.%d.%d.%d",&d1,&d2,&d3,&d4);
        sprintf(aszVal[4],"%d.%d.%d.%d",a1,a2,a3,d4);
        count =5;
    }
 
    if (CLI_SetLeaf(count, aszName, aszVal))
    {
        CLI_CommErrorPrint();
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 设置使IPInterface有效
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注: 根据具体的参数如all, 1, 2来设置具体的IPInterface使有效
****************************************************************************************************/

static ULONG CLI_IPInterface_Enable(void)
{
    if(CLI_GetParamByName("interface") != NULL)  // show the exact interface information
    {
        char  *szInterfaceValue;
        szInterfaceValue = CLI_GetParamByName("interface");

        if(STB_StrCmp(szInterfaceValue,"all") == 0)
        {
            char szInferfaceExact[CLI_LAN_IPINTERFACE_SIZEBIT];
			
            sprintf(szInferfaceExact,"1");
            CLI_IPInterface_Enable_ExactInterface(szInferfaceExact,TRUE);

            sprintf(szInferfaceExact,"2");
            CLI_IPInterface_Enable_ExactInterface(szInferfaceExact,TRUE);
			
            return TBS_SUCCESS;
        }  
        else 
        {
            CLI_IPInterface_Enable_ExactInterface( szInterfaceValue,TRUE);
        }
    }
    else
    {
        IO_Print("\n get interface error .");
        return TBS_FAILED;
    }
    
    return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 设置使具体一个IPInterface有效或无效
参数: szInterface : 具体的IPInterface如"2"等
	    : bEnable : TRUE 使有效
	                     FALSE使无效
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注: 根据具体的参数如all, 1, 2来设置具体的IPInterface使有效或无效
****************************************************************************************************/

static ULONG CLI_IPInterface_Enable_ExactInterface(char* szInterface,BOOL bEnable)
{
    char aszName[1][CLI_NODE__LEN];
    char aszVal[1][CLI_NODE__LEN];

    sprintf(aszName[0],"%s%s.%s",CLI_LAN_IPINTERFACE_PATH,szInterface,"Enable");

    if(bEnable)
        strcpy(aszVal[0],CLI_LAN_IPINTERFACE_ENABLE);
    else
        strcpy(aszVal[0],CLI_LAN_IPINTERFACE_DISABLE);
    if(CLI_SetLeaf(1,aszName,aszVal))
    {
        CLI_CommErrorPrint();
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 使IPInterface无效
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注: 根据具体的参数如all, 1, 2来设置具体的IPInterface使无效
****************************************************************************************************/

static ULONG CLI_IPInterface_Disable(void)
{
    if(CLI_GetParamByName("interface") != NULL)  // show the exact interface information
    {
        char  *szInterfaceValue;
        szInterfaceValue = CLI_GetParamByName("interface");

        if(STB_StrCmp(szInterfaceValue,"all") == 0)
        {
            char szInferfaceExact[CLI_LAN_IPINTERFACE_SIZEBIT];
			
            sprintf(szInferfaceExact,"1");                                                     // for later  make it robust
            CLI_IPInterface_Enable_ExactInterface(szInferfaceExact,FALSE);

            sprintf(szInferfaceExact,"2");                                                    // for later  make it robust
            CLI_IPInterface_Enable_ExactInterface(szInferfaceExact,FALSE);
			
            return TBS_SUCCESS;
        }
        else 
        {
            CLI_IPInterface_Enable_ExactInterface( szInterfaceValue,FALSE);
        }
    }
    else
    {
        IO_Print("\n get interface error .");
        return TBS_FAILED;
    }
    
    return TBS_SUCCESS;
}


/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/
 
#ifdef __cplusplus
}
#endif

