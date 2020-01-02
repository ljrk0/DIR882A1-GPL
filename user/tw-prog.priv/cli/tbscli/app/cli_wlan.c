
/****************************************************************************
 文件名  : CLI_wlan.c
 作者    : Huang Mengxiang
 版本    :
 完成日期:
 文件描述: 
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


//#define CLI_WLAN_DEBUG 1

/*********************************************************************
*						MACRO										   *
**********************************************************************/
#define  CLI_WLAN_ENABLE       				 "1"
#define  CLI_WLAN_DISABLE      				 "0"

#define CLI_WLAN_ENABLE_NAME   			"Enable"
#define CLI_WLAN_ENABLE_VALUE   			"1"
#define CLI_WLAN_DISABLE_VALUE   		       "0"


#define  CLI_WLAN_PRINT                                  printf

#define CLI_WLANCONFIGURATION_PATH         "InternetGatewayDevice.LANDevice.1.WLANConfiguration."
#define CLI_WLAN_PUBLIC_PARAM_PATH         "InternetGatewayDevice.X_TWSZ-COM_WiFi.Radio."
#define CLI_MAC_PATH                                      "InternetGatewayDevice.X_TWSZ-COM_MAC_Filter."
#define CLI_MAC_PATH_BLACKLIST                   "InternetGatewayDevice.X_TWSZ-COM_MAC_Filter.BlackList."
#define CLI_WLAN_SWITCH                    "InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.Enable"

#define CLI_WIRELESS_MAC_PATH              "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.AllowedMACAddresses"

#define CLI_MAC_ALL_DAYS                              "Sun,Mon,Tue,Wed,Thu,Fri,Sat"

#define CLI_WLAN_SSID_NAME                         "SSID"
#define CLI_WLAN_CHANNEL_NAME                  "Channel"
#define CLI_WLAN_CHANEL_AUTO                   "AutoChannelEnable"
#define ClI_WLAN_WEP_KEY1                    "WEPKey.1.WEPKey"
#define ClI_WLAN_WEP_KEY2                    "WEPKey.2.WEPKey"
#define ClI_WLAN_WEP_KEY3                    "WEPKey.3.WEPKey"
#define ClI_WLAN_WEP_KEY4                    "WEPKey.4.WEPKey"

#define CLI_WLAN_WPA_KEY                     "KeyPassphrase"

#define CLI_WLAN_WEP_BEACONTYPE              "Basic"
#define CLI_WLAN_WPA1_BEACONTYPE            "WPA"
#define CLI_WLAN_WPA2_BEACONTYPE	            "11i"
#define CLI_WLAN_ENCRYPTION_NONE             "None"


#define CLI_WEEK_DAYS_NUM                           7
#define CLI_MAX_DAY_NUM                               4

#define  CLI_DAYS_STR_MAX_SIZE                    28
#define  MAX_PATH_LEN                                     257
#define  CLI_WLAN_PRESHAREKEY_MIN_LEN     8

#define CLI_Strstr strstr


#define CLI_WLAN_CHECK_LETTER_HEX(Ch) do \
{ \
	if( !( ('0' <=Ch && Ch <= '9') || ('a' <= Ch && Ch <= 'f') || ('A' <= Ch && Ch <'F')) )  \
	{ \
		IO_Print("error letter %c is not hex\n",Ch); \
		return TBS_FAILED; \
	} \
}while(0);


#define CLI_DAYS_SIZE_ASSERT(pPosCur,pEqlPos) do \
{ \
    if ((pEqlPos) - (pPosCur) != 3) \
    { \
    	IO_Print("Days Format error %s \n",pPosCur); \
        return TBS_FAILED; \
    } \
}while(0);

#define CLI_WLAN_ASSER_WPAVERSION(wpaVersion) do \
{ \
        if( !(STB_StrCmp(wpaVersion,"1")==0  || STB_StrCmp(wpaVersion,"2") == 0) ) \
        	{ \
        		IO_Print("Error wpaVersion is not 1 or 2 !\n"); \
        		return TBS_FAILED; \
        	} \
}while(0);



/**************************************************/
/* MACRO for WLAN                                                         */
/***************************************************/
#define CLI_WLAN_OPERATION_CALL(pFun) do \
{ \
    if (pFun() == TBS_SUCCESS) \
    { \
        return TBS_SUCCESS;\
    } \
}while(0);

/**************************************************/
/* MACRO for WLAN                                                         */
/***************************************************/
#define CLI_MAC_OPERATION_CALL(pFun) do \
{ \
    if (pFun() == TBS_SUCCESS) \
    { \
        return TBS_SUCCESS;\
    } \
}while(0);


/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/
typedef struct st_CLI_WLAN_WPAPsk
{
    char  *pszInterval;
    char  *pszPreSharedKey;
}ST_CLI_WLAN_WPAPsk;

typedef struct st_CLI_WLAN_WPAEnterprise
{
    char  *pszInterval;
    char   *pszServerIP;
    char    *pszPort;
    char  *pszSecret;

}ST_CLI_WLAN_WPAEnterprise;

/*********************************************************************/
/* STATIC CONST VALUE                    */
/*********************************************************************/
const static char days[][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};         // a week



/*********************************************************************
*						FUNCTION									   *
**********************************************************************/

static ULONG CLI_WLAN_Show(void);
static ULONG CLI_WLAN_Config(void);
static ULONG CLI_WLAN_Enable(void);
static ULONG CLI_WLAN_Disable(void);
static ULONG CLI_WLAN_No_Encryption(void);

static ULONG CLI_MAC_Add(void);
static ULONG CLI_MAC_Del(void);
static ULONG CLI_MAC_Show(void);
static ULONG CLI_MAC_Enable(void);
static ULONG CLI_MAC_Disable(void);

static ULONG CLI_Wireless_MAC_Add(void);
static ULONG CLI_Wireless_MAC_Del(void);
static ULONG CLI_Wireless_MAC_Show(void);
static ULONG CLI_Wireless_MAC_Enable(void);
static ULONG CLI_Wireless_MAC_Disable(void);

static ULONG CLI_WLAN_Show_Exact(char* szInterface, char* szLeaf);
static ULONG CLI_WLAN_Show_Pub_Exact(char* szInterface, char* szLeaf);
static ULONG CLI_WLAN_Config_Exact(char* szPath, char szLeaf[CLI_MAX_RWOBJ_NUM][CLI_LEAF__LEN] ,char  szLeafValue[CLI_MAX_RWOBJ_NUM][CLI_LEAF__LEN],ULONG ulCount);
static ULONG CLI_WLAN_Config_WEP(char* szInterface);
static ULONG CLI_WLAN_Config_WPA(char* szInterface);
static ULONG CLI_WLAN_Config_WPA_PSK(char *szInterfaceValue,char* szWpaVersion,ST_CLI_WLAN_WPAPsk *stWPAPsk, char* szWPA);
static ULONG CLI_WLAN_Config_WPA_Ent(char *szInterfaceValue,char* szWpaVersion,ST_CLI_WLAN_WPAEnterprise *stWPAEnt,char* szWPA);
static ULONG CLI_WLAN_CheckWEPKeyValue(char* WEPKeyValue,char *szWEPEncryptionLevel);


static int CLI_ParseDays(char* pszDaysValue, char    aszDays[CLI_WEEK_DAYS_NUM][CLI_MAX_DAY_NUM]) ;
static int CLI_DaysFormatCheck(char* pszDayTemp);
static ULONG CLI_FormatMac(char* pszMacAddress);
static BOOL CLI_WLAN_FindTargetStr(ULONG ulCount, char szMacInstPath[MAX_PATH_LEN],char* pCurPos, char* szTargetStr, char* TargetStrCur);
static int CLI_SplitString(char *str, char *substr[], const char delimit, const int max_count);


/*******************************************************************************/
/* 函数功能: 设置获取WLAN信息用                     */
/*******************************************************************************/

ULONG CLI_WLAN(void)
{
	if(CLI_GetParamByName("show") != NULL)
 	{
 		CLI_WLAN_OPERATION_CALL(CLI_WLAN_Show);
 	}
	else if(CLI_GetParamByName("config") != NULL)
	{
		CLI_WLAN_OPERATION_CALL(CLI_WLAN_Config);
	}
	else if(CLI_GetParamByName("enable") != NULL)
	{
		CLI_WLAN_OPERATION_CALL(CLI_WLAN_Enable);
	}
	else if(CLI_GetParamByName("disable") != NULL)
	{
		CLI_WLAN_OPERATION_CALL(CLI_WLAN_Disable);
	}
	else if(CLI_GetParamByName("noencryption") != NULL)
	{
		CLI_WLAN_OPERATION_CALL(CLI_WLAN_No_Encryption);
	}

	     return TBS_FAILED;

	return TBS_SUCCESS;
}

/**********************************************************************************/
/* 函数功能: 添加删除mac过滤用*/
/**********************************************************************************/

ULONG CLI_WLAN_MAC(void)
{

	IO_Print("enter cli_wlan_mac() \n");     // for debug
	
	if(CLI_GetParamByName("add") != NULL)
	{
		CLI_MAC_OPERATION_CALL(CLI_MAC_Add);
	}
	else if(CLI_GetParamByName("del") != NULL)
	{
		CLI_MAC_OPERATION_CALL(CLI_MAC_Del);
	}
	else if(CLI_GetParamByName("show") != NULL )
	{
		CLI_MAC_OPERATION_CALL(CLI_MAC_Show);
	}
	else if(CLI_GetParamByName("enable") != NULL)
	{
		CLI_MAC_OPERATION_CALL(CLI_MAC_Enable);
	}
	else if(CLI_GetParamByName("disable") != NULL)
	{
		CLI_MAC_OPERATION_CALL(CLI_MAC_Disable);
	}
	else 
		return TBS_FAILED;
	
	return TBS_SUCCESS;
}

/**********************************************************************************/
/* 函数功能: 添加删除wlan的mac过滤用*/
/**********************************************************************************/
ULONG CLI_WIRELESS_MAC(void)
{
    IO_Print("\n");
	
	if(CLI_GetParamByName("add") != NULL)
	{
		CLI_MAC_OPERATION_CALL(CLI_Wireless_MAC_Add);
	}
	else if(CLI_GetParamByName("del") != NULL)
	{
		CLI_MAC_OPERATION_CALL(CLI_Wireless_MAC_Del);
	}
	else if(CLI_GetParamByName("show") != NULL )
	{
		CLI_MAC_OPERATION_CALL(CLI_Wireless_MAC_Show);
	}
	else if(CLI_GetParamByName("enable") != NULL)
	{
		CLI_MAC_OPERATION_CALL(CLI_Wireless_MAC_Enable);
	}
	else if(CLI_GetParamByName("disable") != NULL)
	{
		CLI_MAC_OPERATION_CALL(CLI_Wireless_MAC_Disable);
	}
	else 
		return TBS_FAILED;
	
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 显示wlan的信息
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
备注: 根据具体的参数如ssid channel来调用CLI_WLAN_Show_Exact()显示具体的wlan信息
****************************************************************************************************/
static ULONG CLI_WLAN_Show(void)
{
    char* szLeaf 		 = NULL;
    char* szInterface = CLI_GetParamByName("interface");
    char* szName       = CLI_GetParamByName("name");
    int i = 1;
    if(NULL == szName || NULL == szInterface )
    {
        IO_Print("get parameter error !");
        return TBS_FAILED;
    }
    else if(0== STB_StrCmp(szName,"ssid") )
    {
        szLeaf = CLI_WLAN_SSID_NAME;
    }
    else if(0 == STB_StrCmp(szName,"channel") )
    {
        szLeaf = CLI_WLAN_CHANNEL_NAME;
        return CLI_WLAN_Show_Pub_Exact(szInterface, szLeaf);
    }
    else if(0 == STB_StrCmp(szName,"8211mode") )
    {
        szLeaf = "Standard";
        return CLI_WLAN_Show_Pub_Exact(szInterface, szLeaf);
    }
    else if(0 == STB_StrCmp(szName,"encrypt") )
    {
        szLeaf = "BeaconType";
    }
    else if(0 == STB_StrCmp(szName,"status") )
    {
        szLeaf = "Enable";
    }
    else if(0==STB_StrCmp(szName,"wep"))
    {
        szLeaf = malloc(32);
        for(i=1;i<5;i++)
        {
            sprintf(szLeaf,"WEPKey.%d.WEPKey",i);
            if(TBS_FAILED == CLI_WLAN_Show_Exact(szInterface,szLeaf))
            {
                return TBS_FAILED;
            }
        }
        return TBS_SUCCESS;
    }
    else if(0 == STB_StrCmp(szName,"wpa") )
    {
        szLeaf = "PreSharedKey.1."CLI_WLAN_WPA_KEY;
    }
    else
    {
        IO_Print("Please use : wlan show ssid or wlan show channel\n");
        return TBS_FAILED;
    }
    if(TBS_FAILED == CLI_WLAN_Show_Exact(szInterface,szLeaf))
    {
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 专用于telefonica测试，将ISO-8859-1编码范围为0x80~0xff的字符转换为CP850，供DOS终端显示
参数: aszValue 待转换的字符串
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
上层函数 : CLI_WLAN_Show_Exact    
备注: 
****************************************************************************************************/
static ULONG CLI_WLAN_Conv_Iso8859_Cp850(UCHAR *aszValue)
{
    UCHAR *ucTmp = NULL;

#ifndef TELEFONICA_SPAIN_TEST
    return TBS_SUCCESS;
#endif   

    if(aszValue == NULL)
    {
		IO_Print("input string is NULL\n");
        return TBS_FAILED;
    }

    //convert ISO-8859-1 char(0x80~0xFF) to Code page 850 char
    ucTmp = aszValue;
    while(*ucTmp != '\0')
    {
        if(*ucTmp >= SPECIAL_CHAR_BEGIN)
        {
		    //IO_Print("special char is 0x%x\n",*ucTmp);
            *ucTmp = CLI_MapIso8859ToCp850(*ucTmp);
		    //IO_Print("special char1 is 0x%x\n",*ucTmp);
        }
        ucTmp++;
    }
    
    return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 显示wlan的具体信息
参数: szInterface : 具体的interface如"1"
	    : szLeaf : 叶节点的path
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
上层函数 : CLI_WLAN_Show    
备注: 根据具体的参数如ssid channel来显示具体的wlan信息
****************************************************************************************************/
static ULONG CLI_WLAN_Show_Exact(char* szInterface, char* szLeaf)
{
    if(NULL == szLeaf)
    {
        IO_Print("leaf is NULL\n");
        return TBS_FAILED;
    }
    char aszValue[CLI_LEAF__LEN];
    char szPath[CLI_LEAF__LEN];
    char * szLeafValue = szLeaf;
    if((STB_StrLen(CLI_WLANCONFIGURATION_PATH) + STB_StrLen(szInterface) +STB_StrLen(szLeafValue)) > CLI_LEAF__LEN -1)
    {
        IO_Print("leaf length larger than the CLI_LEAF__LEN(512)\n");
        return TBS_FAILED;
    }
    sprintf(szPath,"%s%s.%s",CLI_WLANCONFIGURATION_PATH,szInterface,szLeafValue);

    if (TBS_SUCCESS!=CLI_GetLeaf(szPath,aszValue))
    {
        CLI_CommErrorPrint();
        return TBS_FAILED;
    }

    CLI_WLAN_Conv_Iso8859_Cp850((UCHAR *)aszValue);

    if(strcmp(szLeafValue,ClI_WLAN_WEP_KEY1)==0)
    {
        IO_Print("\n KEY1=%s\n",aszValue);
        return TBS_SUCCESS;
    }
    else if(strcmp(szLeafValue,ClI_WLAN_WEP_KEY2)==0)
    {
        IO_Print("\n KEY2=%s\n",aszValue);
        return TBS_SUCCESS;
    }
    else if(strcmp(szLeafValue,ClI_WLAN_WEP_KEY3)==0)
    {
        IO_Print("\n KEY3=%s\n",aszValue);
        return TBS_SUCCESS;
    }
    else if(strcmp(szLeafValue,ClI_WLAN_WEP_KEY4)==0)
    {
        IO_Print("\n KEY4=%s\n",aszValue);
        return TBS_SUCCESS;
    }
    else if(strcmp(szLeafValue,"BeaconType")==0)
    {
        if(strcmp(aszValue,"11i") ==0) 
        {
            memset(aszValue,0,sizeof(aszValue));
            strcpy(aszValue,"WPA2");
        }
        else if(strcmp(aszValue,"WPAand11i") ==0) 
        {
            memset(aszValue,0,sizeof(aszValue));
            strcpy(aszValue,"WPA/WPA2 AUTO");				
        }
        IO_Print("\n Security Mode=%s\n",aszValue);
        return TBS_SUCCESS;
    }
    else if(strcmp(szLeafValue,"Enable")==0)
    {
        if(strcmp(aszValue,"1")==0)
            IO_Print("\n Status=%s\n","Enable");
        else
            IO_Print("\n Status=%s\n","Disable");
        return TBS_SUCCESS;
    }
	
    IO_Print("\n %s=%s\n",szLeafValue,aszValue);
	
    return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 显示wlan公共配置参数的具体信息
参数: szInterface : 具体的interface如"1"
	    : szLeaf : 叶节点的path
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
上层函数 : CLI_WLAN_Show    
备注: 由于TBS_MTK的设计问题，无线的公共参数如Channel单独处理，与其他虚拟AP的
            参数分开处理。
****************************************************************************************************/
static ULONG CLI_WLAN_Show_Pub_Exact(char* szInterface, char* szLeaf)
{
    if(NULL == szLeaf)
    {
        IO_Print("leaf is NULL\n");
        return TBS_FAILED;
    }
    char aszValue[CLI_LEAF__LEN];
    char szPath[CLI_LEAF__LEN];

    sprintf(szPath,"%s%s.%s", CLI_WLAN_PUBLIC_PARAM_PATH, szInterface, szLeaf);

    if (TBS_SUCCESS != CLI_GetLeaf(szPath, aszValue))
    {
        CLI_CommErrorPrint();
        return TBS_FAILED;
    }

    //CLI_WLAN_Conv_Iso8859_Cp850((UCHAR *)aszValue);

    IO_Print("\n %s=%s\n", szLeaf, aszValue);
	
    return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 将字符串中特殊字符转换成空格或问号，
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
备注: telefonica西班牙测试时要求ssid和密码支持空格和问号
****************************************************************************************************/
static ULONG CLI_WLAN_Convert_Special_Char(const char *pcString)
{
    char *pcTmp = NULL;
    
#ifndef TELEFONICA_SPAIN_TEST
    return TBS_SUCCESS;
#endif

    if(pcString == NULL)
    {
		IO_Print("string to convert is null\n");
		return TBS_FAILED;
    }

    pcTmp = (char *)pcString;

    //如果起始字符为空格，非法
    if(pcTmp[0] == KEY_US)
    {
		return TBS_FAILED;
    }

    //如果末尾字符为空格，非法
    if(strlen(pcString) > 0
        && pcTmp[strlen(pcString) - 1] == KEY_US)
    {
		return TBS_FAILED;
    }

    while(*pcTmp != '\0')
    {
        if(*pcTmp == KEY_US)
        {
            *pcTmp = ' ';
        }
        else if(*pcTmp == KEY_RE)
        {
            *pcTmp = '?';
        }
        
        pcTmp++;
    }

    //连续出现空格，非法
    if(strlen(pcString) >= 2 &&
        strstr(pcString, "  ") != NULL)
    {
		return TBS_FAILED;
    }
    
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 配置wlan的信息
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
备注: 根据具体的参数如basic , encryption来配置具体的wlan信息
****************************************************************************************************/
static ULONG CLI_WLAN_Config(void)
{

    int iRet = TBS_FAILED;
    
    char* szInterface = CLI_GetParamByName("interface");

    if(NULL == szInterface )
    {
        IO_Print("get parameter error !");
        return TBS_FAILED;
    }
    if(CLI_GetParamByName("basic") != NULL)
    {
        ULONG  ulConfigNum                = 0;
        char   szLeafValue[CLI_MAX_RWOBJ_NUM][CLI_LEAF__LEN] ;
        char   szLeaf[CLI_MAX_RWOBJ_NUM][CLI_LEAF__LEN] ;
        char szPath[CLI_LEAF__LEN];
	
         if(CLI_GetParamByName("ssid") != NULL )
        {
            STB_StrCpy(szLeaf[ulConfigNum], CLI_WLAN_SSID_NAME);
            STB_StrCpy(szLeafValue[ulConfigNum] , CLI_GetParamByName("ssid-v"));
            iRet = CLI_WLAN_Convert_Special_Char(szLeafValue[ulConfigNum]);
            if(iRet != TBS_SUCCESS)
            {
                IO_Print("\nSsid set error !\n");
                return TBS_FAILED;
            }
            ulConfigNum++;
            /* SSID和CHANNEL在不同的节点路径下，分开来设置 */
            sprintf(szPath, "%s%s", CLI_WLANCONFIGURATION_PATH, szInterface);
            if(TBS_FAILED ==CLI_WLAN_Config_Exact(szPath, szLeaf, szLeafValue, ulConfigNum))
            {
                return TBS_FAILED;
            }
        }
        if(CLI_GetParamByName("channel") != NULL )
        {
            /* channel在IGD.X_TWSZ-COM_WiFi.Radio下面 */
            ulConfigNum = 0;
            STB_StrCpy(szLeaf[ulConfigNum], CLI_WLAN_CHANNEL_NAME);
            STB_StrCpy(szLeafValue[ulConfigNum], CLI_GetParamByName("channel-v"));
            ulConfigNum++;
            STB_StrCpy(szLeaf[ulConfigNum], CLI_WLAN_CHANEL_AUTO);
            if (strcmp("0", CLI_GetParamByName("channel-v")) == 0)
                STB_StrCpy(szLeafValue[ulConfigNum], "1");  //无线自动搜索信道
            else
                STB_StrCpy(szLeafValue[ulConfigNum], "0");  //手动指定信道
            ulConfigNum++;
            sprintf(szPath, "%s%s", CLI_WLAN_PUBLIC_PARAM_PATH, szInterface);
            if(TBS_FAILED ==CLI_WLAN_Config_Exact(szPath, szLeaf, szLeafValue, ulConfigNum))
            {
                return TBS_FAILED;
            }
        }
        if(CLI_GetParamByName("8211mode") != NULL )
        {
            /* channel在IGD.X_TWSZ-COM_WiFi.Radio下面 */
            ulConfigNum = 0;
            STB_StrCpy(szLeaf[ulConfigNum], "Standard");
            STB_StrCpy(szLeafValue[ulConfigNum], CLI_GetParamByName("8211mode-v"));
            if(!(strcmp(szLeafValue[ulConfigNum],"b")==0 || strcmp(szLeafValue[ulConfigNum],"g")==0 ||
                  strcmp(szLeafValue[ulConfigNum],"n")==0 ||  strcmp(szLeafValue[ulConfigNum],"bg")==0 ||
                  strcmp(szLeafValue[ulConfigNum],"ng")==0 ||strcmp(szLeafValue[ulConfigNum],"bgn")==0))
            {
                IO_Print("\n%s invalid \n",szLeafValue[ulConfigNum]);
                return TBS_SUCCESS;      
            }
            ulConfigNum++;
            sprintf(szPath, "%s%s", CLI_WLAN_PUBLIC_PARAM_PATH, szInterface);
            if(TBS_FAILED ==CLI_WLAN_Config_Exact(szPath, szLeaf, szLeafValue, ulConfigNum))
            {
                return TBS_FAILED;
            }
        }
        
    }
    else if(CLI_GetParamByName("encryption") != NULL)
    {
        if(CLI_GetParamByName("wep") != NULL)
        {
            return CLI_WLAN_Config_WEP(szInterface);
        }
        else if(CLI_GetParamByName("wpa") != NULL)
        {
            return CLI_WLAN_Config_WPA(szInterface);
        }
        else
        {
            IO_Print("Format error in config security!!!\n");
            return TBS_SUCCESS;
        }
    }
    else
    {
        IO_Print("Format error! \n");
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 配置wlan的具体的信息
参数: szPath : 具体的interface 如"1"所在的路径
	    : szLeaf : 要配置的叶子节点path的集合
	    : szLeafValue : 要配置的叶子节点value的集合
	    : ulCount : 要配置的叶子节点的个数
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
上层函数: CLI_WLAN_Config
备注: 根据具体的参数来配置具体的wlan信息
****************************************************************************************************/
static ULONG CLI_WLAN_Config_Exact(char* szPath, char szLeaf[CLI_MAX_RWOBJ_NUM][CLI_LEAF__LEN] ,char  szLeafValue[CLI_MAX_RWOBJ_NUM][CLI_LEAF__LEN],ULONG ulCount)
{
	int index = 0;
	if(ulCount <= 0)
	{
		IO_Print("ERROR! WLAN_Config_Exact parameter is NULL \n");
		return TBS_FAILED;
	}
	char aszName[CLI_MAX_RWOBJ_NUM][CLI_LEAF__LEN];
	char aszVal[CLI_MAX_RWOBJ_NUM][CLI_LEAF__LEN];

	for(index = 0; index < ulCount; ++index)
	{
		 sprintf(aszName[index],"%s.%s", szPath, szLeaf[index]);
		 strcpy(aszVal[index],szLeafValue[index]);
	}
		
	if(CLI_SetLeaf(ulCount,aszName,aszVal))
	{
	    CLI_CommErrorPrint();
	    return TBS_FAILED;
	}
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 配置wlan encryption wep 信息
参数: szInterface : 具体的interface如"1"
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
上层函数: CLI_WLAN_Config
备注: 根据具体的参数来配置wlan 的wep信息
****************************************************************************************************/
static ULONG CLI_WLAN_Config_WEP(char* szInterface)
{
	char    *szInterfaceValue = szInterface;
	int    iRet = TBS_FAILED;	
	char   *szBeaconType 				= NULL;           // Security Mode
	// char   *szWEPEncryption                       = NULL;
	char   *szWEPEncryptionLevel			= NULL;           //WEP Key Length      64bits or 128bits
	char   *szWEPKeyIndex				= NULL;		 //Choose WEP Key
	char   *szWEPKeyValue				= NULL;          // wep key
	char   *szBasicAuthenticationMode	= NULL;		//Authentication 
	 
	char aszName[6][CLI_NODE__LEN];
	char aszVal[6][CLI_NODE__LEN];

    szBeaconType = CLI_WLAN_WEP_BEACONTYPE;
    szWEPEncryptionLevel = CLI_GetParamByName("wepkeylength");
    szWEPKeyIndex = CLI_GetParamByName("wepkeyindex");
    szWEPKeyValue = CLI_GetParamByName("wepkeyvalue");
	szBasicAuthenticationMode = CLI_GetParamByName("authentication");
	
	if (szBeaconType == NULL || szWEPEncryptionLevel == NULL || szWEPKeyIndex == NULL || szWEPKeyValue == NULL ||szBasicAuthenticationMode ==NULL)
	{
		IO_Print("Term Parameter error.");
		return TBS_FAILED;
	}

    iRet = CLI_WLAN_Convert_Special_Char(szWEPKeyValue);
    if(iRet != TBS_SUCCESS)
    {
		IO_Print("\nWepkeyvalue set error!\n");
		return TBS_FAILED;
    }
    
	if( CLI_WLAN_CheckWEPKeyValue(szWEPKeyValue,szWEPEncryptionLevel) != TBS_SUCCESS)
	{
		IO_Print("wepKep value format error \n");
		return TBS_FAILED;
	}

	sprintf(aszName[0], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue,"BeaconType");
	sprintf(aszName[1], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue,"WEPEncryptionLevel");
	sprintf(aszName[2], "%s%s.%s", CLI_WLANCONFIGURATION_PATH,  szInterfaceValue,"WEPKeyIndex");
	sprintf(aszName[3], "%s%s.%s.%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "WEPKey",szWEPKeyIndex,"WEPKey");
	sprintf(aszName[4], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "BasicAuthenticationMode");	
	sprintf(aszName[5], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "BasicEncryptionModes");	

    strcpy(aszVal[0], szBeaconType);

	if(STB_StrCmp(szWEPEncryptionLevel,"64bits") == 0)
		strcpy(aszVal[1], "40-bit");
	else
	 	strcpy(aszVal[1], "104-bit");
	 
	strcpy(aszVal[2], szWEPKeyIndex);
	strcpy(aszVal[3], szWEPKeyValue);
	
	if(STB_StrCmp(szBasicAuthenticationMode,"open") == 0)
		strcpy(aszVal[4],"None");
	else
		strcpy(aszVal[4],"SharedAuthentication");

	strcpy(aszVal[5],"WEPEncryption");

    if (CLI_SetLeaf(6, aszName, aszVal)  != TBS_SUCCESS)
    {
       CLI_CommErrorPrint();
       return TBS_FAILED;
    }
        
    return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 配置wlan encryption wpa 信息
参数: szInterface : 具体的interface如"1"
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
上层函数: CLI_WLAN_Config
备注: 根据具体的参数来配置wlan 的wpa信息
****************************************************************************************************/
static ULONG CLI_WLAN_Config_WPA(char* szInterface)
{
	int    iRet = TBS_FAILED;	
	char  *szWpaVersion			= NULL;
	char   *szInterfaceValue          = szInterface;
	szWpaVersion = CLI_GetParamByName("wpa-version");
	if(szWpaVersion != NULL )
	{
		CLI_WLAN_ASSER_WPAVERSION(szWpaVersion);
	}
	else
	{
		IO_Print("Get szWpaVersion is NULL \n");
		return TBS_FAILED;
	}
	char *pszWPA= NULL;
	char szWPA[64] = {0};
	pszWPA= CLI_GetParamByName("wpa-encryption");
	if(pszWPA!= NULL)
	{
	/* cli命令配置无线的加密模式，按Tab键补全的字符为小写, 小写也要生效 */
		if (!strcasecmp(pszWPA, "TKIP"))
		{
		    strcpy(szWPA, "TKIPEncryption");
		}
		else if (!strcasecmp(pszWPA, "AES"))
		{
		    strcpy(szWPA, "AESEncryption");
		}
		else if (!strcasecmp(pszWPA, "TKIPAES"))
		{
		    strcpy(szWPA, "TKIPandAESEncryption");
		}
		else
		{
		    IO_Print("Get pszWPA is NULL \n");
		    return TBS_FAILED;
		}
	}
	else 
	{
	    IO_Print("Get szWPA is NULL \n");
		return TBS_FAILED;
	}
	
	
	if(CLI_GetParamByName("psk") != NULL) 	//config psk
	{
		ST_CLI_WLAN_WPAPsk stWPAPsk;
		stWPAPsk.pszInterval = CLI_GetParamByName("interval");
		stWPAPsk.pszPreSharedKey = CLI_GetParamByName("pre-shared-key");
		if(NULL== stWPAPsk.pszInterval  ||NULL== stWPAPsk.pszPreSharedKey)
		{
			IO_Print("Error ! Get parameter of stWPAPsk is NULL \n");
			return TBS_FAILED;
		}
		if(STB_StrLen(stWPAPsk.pszPreSharedKey) < CLI_WLAN_PRESHAREKEY_MIN_LEN)
		{
			IO_Print("Error ! PreSharedKey is less than CLI_WLAN_PRESHAREKEY_MIN_LEN(8) \n");
			return TBS_FAILED;
		}
        
        iRet = CLI_WLAN_Convert_Special_Char(stWPAPsk.pszPreSharedKey);
        if(iRet != TBS_SUCCESS)
        {
    		IO_Print("\nPre-shared-key set error!\n");
    		return TBS_FAILED;
        }
        

		return CLI_WLAN_Config_WPA_PSK(szInterfaceValue,szWpaVersion,&stWPAPsk, szWPA);
		
	}
	else if(CLI_GetParamByName("ent") != NULL) //config Enterprise
	{
		ST_CLI_WLAN_WPAEnterprise stWPAEnt;
		stWPAEnt.pszInterval = CLI_GetParamByName("interval");
		stWPAEnt.pszServerIP = CLI_GetParamByName("server-ip");
		stWPAEnt.pszPort= CLI_GetParamByName("port");
		stWPAEnt.pszSecret= CLI_GetParamByName("secret");
			
		if(NULL== stWPAEnt.pszInterval  ||NULL== stWPAEnt.pszServerIP || NULL == stWPAEnt.pszServerIP || NULL == stWPAEnt.pszPort || NULL == stWPAEnt.pszSecret)
		{
			IO_Print("Error ! Get parameter of stWPAPsk is NULL \n");
			return TBS_FAILED;
		}
        
        iRet = CLI_WLAN_Convert_Special_Char(stWPAEnt.pszSecret);
        if(iRet != TBS_SUCCESS)
        {
    		IO_Print("\nSecret set error!\n");
    		return TBS_FAILED;
        }

		return CLI_WLAN_Config_WPA_Ent(szInterfaceValue,szWpaVersion,&stWPAEnt, szWPA);

	}
	else
	{
		IO_Print("Error! please use right parameter format enter '?' for help!");
		return TBS_FAILED;
	}
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 配置wlan encryption wpa-psk信息
参数: szInterface : 具体的interface如"1"
	    : szWpaVersion : 具体的wpa的版本信息如"1" 或"2"
	    : stWPAPsk : 要配置的psk的具体信息
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
上层函数: CLI_WLAN_Config
备注: 根据具体的参数来配置wlan 的wpa-psk信息
****************************************************************************************************/
static ULONG CLI_WLAN_Config_WPA_PSK(char* szInterfaceValue,char* szWpaVersion,ST_CLI_WLAN_WPAPsk *stWPAPsk,char* szWPA )
{

	char * szInterfaceValueT = szInterfaceValue;
	char * szWpaVersionT = szWpaVersion;
	char * szIntervalT = stWPAPsk->pszInterval;
	char * szPreSharedKeyT =stWPAPsk->pszPreSharedKey;

	char * szWPAEncryptionModes = szWPA;
	
	if(NULL == szInterfaceValueT || NULL == szWpaVersionT || NULL == szIntervalT || NULL == szPreSharedKeyT)
	{
		IO_Print("Error ! Get parameter is NULL in CLI_WLAN_Config_WPA_PSK()\n");
		return TBS_FAILED;
	}
	char aszName[6][CLI_NODE__LEN];
    char aszVal[6][CLI_NODE__LEN];

	sprintf(aszName[0], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue,"BeaconType");   // Security Mode
	sprintf(aszName[3], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "X_TWSZ-COM_WPAGroupRekey"); //Group Key Update Interval 

	if(strlen(szPreSharedKeyT)<64)
    {
		sprintf(aszName[4], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "PreSharedKey.1.KeyPassphrase");	 
		sprintf(aszName[5], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "X_TWSZ-COM_PSKExpression");	
		strcpy(aszVal[5], "KeyPassphrase"); 
	}
	else
	{
		sprintf(aszName[4], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "PreSharedKey.1.PreSharedKey");	 // Pre-Shared Key
		sprintf(aszName[5], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "X_TWSZ-COM_PSKExpression");	
		strcpy(aszVal[5], "PreSharedKey"); 
	}
	
	if(STB_StrCmp(szWpaVersionT,"1") == 0)
	{
	   	 
		strcpy(aszVal[0], "11i");         
		sprintf(aszName[1], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue,"IEEE11iAuthenticationMode"); // value set EAPAuthentication for WPA_Ent
		sprintf(aszName[2], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "IEEE11iEncryptionModes");

	}
	else if(STB_StrCmp(szWpaVersionT,"2") == 0)
	{
	   	 
		strcpy(aszVal[0], "WPAand11i");         
        sprintf(aszName[1], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue,"IEEE11iAuthenticationMode"); //WPA Mode
        sprintf(aszName[2], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "IEEE11iEncryptionModes"); 	
	}
	else
	{
		IO_Print("config wpa version error !!! \n");	    
		return TBS_FAILED;
	}
	
	strcpy(aszVal[1], "PSKAuthentication"); 
	strcpy(aszVal[2],szWPAEncryptionModes);
	strcpy(aszVal[3], szIntervalT);
	strcpy(aszVal[4],szPreSharedKeyT);	

/*for debug
*/
/*
{
int i = 0;
for(i = 0; i<5; i++)
{
	IO_Print("path=%s\n",aszName[i]);
	IO_Print("path=%s\n",aszVal[i]);
}
}
*/
	if (CLI_SetLeaf(6, aszName, aszVal)  != TBS_SUCCESS)
	{
		CLI_CommErrorPrint();
		return TBS_FAILED;
	}    
	return TBS_SUCCESS;
   
}

/***************************************************************************************************
功能: 配置wlan encryption wpa-Ent(Enterprise) 信息
参数: szInterface : 具体的interface如"1"
	    : szWpaVersion : 具体的wpa的版本信息如"1" 或"2"
	    : stWPAEnt : 要配置的Ent的具体信息
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
上层函数: CLI_WLAN_Config
备注: 根据具体的参数来配置wlan 的wpa-ent信息
****************************************************************************************************/
static ULONG CLI_WLAN_Config_WPA_Ent(char *szInterfaceValue,char* szWpaVersion,ST_CLI_WLAN_WPAEnterprise *stWPAEnt,char* szWPA)
{

	char * szInterfaceValueT = szInterfaceValue;
	char * szWpaVersionT = szWpaVersion;
	char * szIntervalT = stWPAEnt->pszInterval;
	char * szServerIPT = stWPAEnt->pszServerIP;
	char * szPortT =stWPAEnt->pszPort;
	char * szSecretT= stWPAEnt->pszSecret;
	char * szWPAEncryptionModes = szWPA;
	
	if(NULL == szInterfaceValueT || NULL == szWpaVersionT || NULL == szIntervalT || NULL == szServerIPT || NULL == szPortT  || NULL ==szSecretT || NULL == szWPA)
	{
		IO_Print("Error ! Get parameter is NULL in CLI_WLAN_Config_WPA_PSK()\n");
		return TBS_FAILED;
	}
	char aszName[7][CLI_NODE__LEN];
	char aszVal[7][CLI_NODE__LEN];
	   
	sprintf(aszName[0], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue,"BeaconType");   // Security Mode
	sprintf(aszName[3], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "X_TWSZ-COM_WPAGroupRekey"); //Group Key Update Interval :     
	sprintf(aszName[4], "%s%s.%s.%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "X_TWSZ-COM_RadiusServer","1","X_TWSZ-COM_IPAddress");	 // 
	sprintf(aszName[5], "%s%s.%s.%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "X_TWSZ-COM_RadiusServer","1","X_TWSZ-COM_Port");	 // 
	sprintf(aszName[6], "%s%s.%s.%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "X_TWSZ-COM_RadiusServer","1","X_TWSZ-COM_ShareKey");//
	
	if(STB_StrCmp(szWpaVersionT,"1") == 0)
	{
		strcpy(aszVal[0], CLI_WLAN_WPA2_BEACONTYPE);     
		sprintf(aszName[1], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue,"IEEE11iAuthenticationMode"); // value set EAPAuthentication for WPA_Ent
		sprintf(aszName[2], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "IEEE11iEncryptionModes");  
	}
	else if(STB_StrCmp(szWpaVersionT,"2") == 0)
	{
		strcpy(aszVal[0], "WPAand11i");        
		sprintf(aszName[1], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue,"IEEE11iAuthenticationMode"); //WPA Mode
		sprintf(aszName[2], "%s%s.%s", CLI_WLANCONFIGURATION_PATH, szInterfaceValue, "IEEE11iEncryptionModes"); 
	}
	else
	{
		IO_Print("-----config wpa version error !!! \n");	    
		return TBS_FAILED;
	}
	strcpy(aszVal[1], "EAPAuthentication"); // MACRO LATER
	strcpy(aszVal[2],szWPAEncryptionModes);	
	strcpy(aszVal[3], szIntervalT);
 	strcpy(aszVal[4],szServerIPT);	
	strcpy(aszVal[5],szPortT);	
	strcpy(aszVal[6],szSecretT);	

#ifdef CLI_WLAN_DEBUG
	int i =0;
	for( i = 0; i < 7; i++)
	{
		printf("path=%s---value=%s\n",aszName[i],aszVal[i]);
	}
#endif
	if (CLI_SetLeaf(7, aszName, aszVal)  != TBS_SUCCESS)
	{
		CLI_CommErrorPrint();
		return TBS_FAILED;
	}
        
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 设置使wlan有效
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注: 根据具体的参数如1, 2来设置具体的wlan使有效
****************************************************************************************************/
static ULONG CLI_WLAN_Enable(void)
{
	char* szInterface = CLI_GetParamByName("interface");
	char aszName[1][CLI_LEAF__LEN];
	char aszVal[1][CLI_LEAF__LEN];
        char *szInerfaceValue = NULL;

        memset(aszName, 0x00, sizeof(aszName));
        memset(aszVal, 0x00, sizeof(aszVal));
    
	if(NULL == szInterface)
	{
	        strcpy(aszName[0], CLI_WLAN_SWITCH);
                strcpy(aszVal[0], CLI_WLAN_ENABLE_VALUE);
                
        	if(CLI_SetLeaf(1, aszName,aszVal))
        	{
        	      CLI_CommErrorPrint();
        	      return TBS_FAILED;
        	}
                
		return TBS_SUCCESS;
	}

        szInerfaceValue = CLI_GetParamByName("interface-value");
        if(NULL == szInerfaceValue)
        {
		IO_Print("get parameter error !");
		return TBS_FAILED;                
        }
        
	if(STB_StrLen(CLI_WLANCONFIGURATION_PATH) + STB_StrLen(szInerfaceValue) > CLI_LEAF__LEN - 1)
	{
		IO_Print("ERROR! WLAN_Enable parameter is larger than CLI_LEAF__LEN(512) \n");
		return TBS_FAILED;
	}

	 sprintf(aszName[0],"%s%s.%s",CLI_WLANCONFIGURATION_PATH,szInerfaceValue,CLI_WLAN_ENABLE_NAME);
	 strcpy(aszVal[0],CLI_WLAN_ENABLE_VALUE);

	 if(CLI_SetLeaf(1, aszName,aszVal))
	 {
	 	CLI_CommErrorPrint();
	       return TBS_FAILED;
	 }
	
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 设置使wlan无效
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注: 根据具体的参数如1, 2来设置具体的wlan使无效
****************************************************************************************************/
static ULONG CLI_WLAN_Disable(void)
{
	char* szInterface = CLI_GetParamByName("interface");
	char aszName[1][CLI_LEAF__LEN];
	char aszVal[1][CLI_LEAF__LEN];
        char *szInerfaceValue = NULL;

        memset(aszName, 0x00, sizeof(aszName));
        memset(aszVal, 0x00, sizeof(aszVal));
    
	if(NULL == szInterface)
	{
	        strcpy(aszName[0], CLI_WLAN_SWITCH);
                strcpy(aszVal[0], CLI_WLAN_DISABLE_VALUE);
                
        	if(CLI_SetLeaf(1, aszName,aszVal))
        	{
        	      CLI_CommErrorPrint();
        	      return TBS_FAILED;
        	}
                
		return TBS_SUCCESS;
	}
	
        szInerfaceValue = CLI_GetParamByName("interface-value");
        if(NULL == szInerfaceValue)
        {
		IO_Print("get parameter error !");
		return TBS_FAILED;                
        }
        
	if(STB_StrLen(CLI_WLANCONFIGURATION_PATH) + STB_StrLen(szInerfaceValue) > CLI_LEAF__LEN - 1)
	{
		IO_Print("ERROR! WLAN_Enable parameter is larger than CLI_LEAF__LEN(512) \n");
		return TBS_FAILED;
	}

	 sprintf(aszName[0],"%s%s.%s",CLI_WLANCONFIGURATION_PATH,szInerfaceValue,CLI_WLAN_ENABLE_NAME);
	 strcpy(aszVal[0],CLI_WLAN_DISABLE_VALUE);

	 if(CLI_SetLeaf(1, aszName,aszVal))
	 {
	 	CLI_CommErrorPrint();
	       return TBS_FAILED;
	 }
	
	return TBS_SUCCESS;

}

/***************************************************************************************************
功能: 设置使wlan encryption无效
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注: 根据具体的参数如1, 2来设置具体的wlan encryption使无效
****************************************************************************************************/
static ULONG CLI_WLAN_No_Encryption(void)
{
	char* szInterface = CLI_GetParamByName("interface");
	if(NULL == szInterface)
	{
		IO_Print("get parameter error !");
		return TBS_FAILED;
	}
	
	char aszName[2][CLI_LEAF__LEN];
	char aszVal[2][CLI_LEAF__LEN];

	if(STB_StrLen(CLI_WLANCONFIGURATION_PATH) + STB_StrLen(szInterface) > CLI_LEAF__LEN - 1)
	{
		IO_Print("ERROR! WLAN_Enable parameter is larger than CLI_LEAF__LEN(512) \n");
		return TBS_FAILED;
	}

	 sprintf(aszName[0],"%s%s.%s",CLI_WLANCONFIGURATION_PATH,szInterface,"BeaconType");
	 strcpy(aszVal[0],CLI_WLAN_ENCRYPTION_NONE);

	 sprintf(aszName[1],"%s%s.%s",CLI_WLANCONFIGURATION_PATH,szInterface,"BasicEncryptionModes");
	 strcpy(aszVal[1],CLI_WLAN_ENCRYPTION_NONE);


	 if(CLI_SetLeaf(2, aszName,aszVal))
	 {
	 	CLI_CommErrorPrint();
	       return TBS_FAILED;
	 }
	
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 添加wlan的mac black list 的mac值
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注: 无
****************************************************************************************************/
static ULONG CLI_MAC_Add(void)
{

	int        i                                               = 0;
	ULONG ulInstIndex                             = 0; //实例号
	int        nDays                                      = 0;
	char     aszDays[CLI_WEEK_DAYS_NUM][CLI_MAX_DAY_NUM];
	char   *pszMacAddress                         = NULL;
	char   *pszDescribetion                        =NULL;     //user name
	char   *pszStartTime                            = NULL;
	char   *pszEndTime                              = NULL;
	char   *pszDaysToken                          = NULL;
	char   *pszDaysValue                           = NULL;
	char   *pszEnableAlldays                     = NULL;
//	char   *pszEnableAllTimes = NULL;                                                     // 根据注册命令添加
//	char   *pszSchedulePath =NULL;                                                        // 根据注册命令添加
//	char   *pszScheduleMode = NULL;									// 根据注册命令添加

	char aszPath[9][CLI_NODE__LEN] ;
       char aszValue[9][CLI_NODE__LEN];

	pszMacAddress = CLI_GetParamByName("mac-v");
	pszDescribetion = CLI_GetParamByName("description");
	pszStartTime = CLI_GetParamByName("starttime");
	pszEndTime = CLI_GetParamByName("endtime");

	if(NULL == pszMacAddress || NULL == pszDescribetion || NULL == pszStartTime || NULL == pszEndTime)
	{
		IO_Print("ERROR! Get parameters NULL in MAC_ADD() \n");
		return TBS_FAILED;
	}

	if ( (pszDaysToken = CLI_GetParamByName("alldays")) != NULL) 
	{

		pszEnableAlldays = CLI_WLAN_ENABLE_VALUE;
		STB_StrCpy(aszValue[3],CLI_MAC_ALL_DAYS);                  //Days
				
	}
	else if( (pszDaysToken = CLI_GetParamByName("days")) != NULL)
	{
		pszDaysValue = CLI_GetParamByName("days-v");
		if(NULL == pszDaysValue)
		{
			IO_Print("ERROR! Get parameter days-v is NULL in MAC_ADD() \n");
			return TBS_FAILED;
		}
		else
		{

			nDays = CLI_ParseDays(pszDaysValue,aszDays);

			if ( nDays == TBS_FAILED) // 还没考虑输入重复的天
			{
				return TBS_FAILED;
			}
			else if(nDays == CLI_WEEK_DAYS_NUM)
			{
				pszEnableAlldays = CLI_WLAN_ENABLE_VALUE;
				STB_StrCpy(aszValue[3],CLI_MAC_ALL_DAYS);                  //Days
			}
			else
			{
				pszEnableAlldays = CLI_WLAN_DISABLE_VALUE;
				for(i = 0; i < nDays; ++i)
				{
					STB_StrCat(aszValue[3],aszDays[i]);
					STB_StrCat(aszValue[3],",");	
				}			
			}
		
		}
	}
	else
	{
		IO_Print("ERROR! Not Get parameter alldays or days is NULL in MAC_ADD() \n");
		return TBS_FAILED;
	}

    if(CLI_AddObjectEx(CLI_MAC_PATH_BLACKLIST, &ulInstIndex) != TBS_SUCCESS)
    {

        return TBS_FAILED;
    }

	if(CLI_FormatMac(pszMacAddress) == TBS_FAILED)
		return TBS_FAILED;

	*(pszStartTime + 5) = '\0';
	*(pszEndTime + 5)  = '\0';        // format for set

	sprintf(aszPath[0],"%s%lu.EndTime",CLI_MAC_PATH_BLACKLIST,ulInstIndex);
	sprintf(aszValue[0],"%s",pszEndTime);

	sprintf(aszPath[1],"%s%lu.StartTime",CLI_MAC_PATH_BLACKLIST,ulInstIndex);
	sprintf(aszValue[1],"%s",pszStartTime);

	sprintf(aszPath[2],"%s%lu.EnableAllTimes",CLI_MAC_PATH_BLACKLIST,ulInstIndex);
	sprintf(aszValue[2],"%s",CLI_WLAN_DISABLE_VALUE);                                                         // 设为"0" 根据之后的注册命令再改

	sprintf(aszPath[3],"%s%lu.Days",CLI_MAC_PATH_BLACKLIST,ulInstIndex);

	sprintf(aszPath[4],"%s%lu.EnableAllDays",CLI_MAC_PATH_BLACKLIST,ulInstIndex);
	sprintf(aszValue[4],"%s",pszEnableAlldays);

	sprintf(aszPath[5],"%s%lu.SchedulePath",CLI_MAC_PATH_BLACKLIST,ulInstIndex);		// 设为"Never" 根据之后的注册命令再改
	sprintf(aszValue[5],"%s","Always");
	
	sprintf(aszPath[6],"%s%lu.ScheduleMode",CLI_MAC_PATH_BLACKLIST,ulInstIndex);              // 设为"Manual" 根据之后的注册命令再改
	sprintf(aszValue[6],"%s","Manual");

	sprintf(aszPath[7],"%s%lu.MACAddress",CLI_MAC_PATH_BLACKLIST,ulInstIndex);
	sprintf(aszValue[7],"%s",pszMacAddress);

	sprintf(aszPath[8],"%s%lu.Description",CLI_MAC_PATH_BLACKLIST,ulInstIndex);
	sprintf(aszValue[8],"%s",pszDescribetion);

	/* for debug
	*/
/*
{
	for(i = 0; i < 9; i++)
	{
	printf("%d:path=%s+++value=%s\n",i,aszPath[i],aszValue[i]);
		}
}
*/

    if (CLI_SetLeaf(9, aszPath, aszValue) != TBS_SUCCESS)
    {  
       IO_Print("CLI_SetLeaf     failed ---  CLI_MAC_Add() \n"); // for debug 
        return TBS_FAILED;
    }
	
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 删除mac的mac black list的mac
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注: 根据具体的参数mac的值来删除mac
****************************************************************************************************/
static ULONG CLI_MAC_Del(void)
{
	ST_MSG *pstMsg                               = NULL;
	ULONG    ulCount                               = 0;
	ULONG    ulTemp                               = 0;
	char      *pMacName                          = NULL;
       char *pCurPos                                   = NULL;
       char *pTempPos                                = NULL;
	char szMacInstPath[MAX_PATH_LEN]  = {0};

	if(NULL == (pMacName = CLI_GetParamByName("mac-v")) )
	{
		IO_Print("ERROR! Get parameter mac-v is NULL in CLI_MAC_Del()!\n");
		return TBS_FAILED;
	}
	CLI_FormatMac(pMacName);

	if(NULL == (pstMsg = CLI_GetNode(CLI_MAC_PATH_BLACKLIST)) )
	{
		IO_Print("ERROR! Get ST_MSG is NULL in CLI_MAC_Del()!\n");
		return TBS_FAILED;
	}
	  /*
     *从消息中解析出路径，如:
     *InternetGatewayDevice.X_TWSZ-COM_MAC_Filter.BlackList.1.
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
        
        pTempPos = strstr(pCurPos, "MACAddress=");
        if (pTempPos != NULL)
        {
            char *pPos = strchr(pCurPos, '=');
            if (pPos != NULL)
            {
                //判断pMacName值是否相同
                if (strcmp(pMacName, pPos+1) == 0)
                {
                    /*
                     *跳过前面的"string "，共7个字节
                     *因为CMM返回的一个item类似这样的:
                     *"string InternetGatewayDevice.X_TWSZ-COM_DDNS.1.MACAddress=test.tzo.com"
                     */
                    strncpy(szMacInstPath, pCurPos+7, pTempPos-(pCurPos+7));

                    break;
                }
            }
        }

        //指向下一个item
        pCurPos += strlen(pCurPos) + 1;
    }

    //如果找不到
    if (strlen(szMacInstPath) == 0)
    {
        return TBS_FAILED;
    }
    //找到了，删除之
    else if (CLI_DelObject(szMacInstPath) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
		
	return TBS_SUCCESS;
}
/***************************************************************************************************
功能: 显示mac的mac black list的mac值
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注: 根据具体的参数mac来显示具体的mac信息或all来显示全部mac信息
****************************************************************************************************/
static ULONG CLI_MAC_Show(void)
{

		 ST_MSG  *pstMsg					= NULL;
		 char *pTempPos						= NULL;
		 char *pCurPos 						= NULL;
		 char *pCurPosSrc 					=NULL;
		 char szTemp[MAX_PATH_LEN]   			= {0};
		 char szMacInstPath[MAX_PATH_LEN]	={0};
		 ULONG ulCount  						= 0;
		 ULONG ulTemp   						= 0;
		 ULONG ulTempID 					= 0;
		 BOOL bAllMac = FALSE;


		char* pMacName = CLI_GetParamByName("mac-v");

		if(NULL  == pMacName)
		{
			IO_Print("ERROR! Get parameter mac-v is NULL in CLI_MAC_Show!\n");
			return TBS_FAILED;
		}
		if(STB_StrCmp(pMacName,"all") == 0 )
		{
			bAllMac = TRUE;
		}
		else
		{
			if(CLI_FormatMac(pMacName) == TBS_FAILED )
			{
				IO_Print("CLI_FormatMac ERROR! In CLI_MAC_Show()\n" );
				return TBS_FAILED;
			}
		}

	    //获取CLI_MAC_PATH_BLACKLIST 下的实例
	    pstMsg = CLI_GetNode(CLI_MAC_PATH_BLACKLIST);
	    if (pstMsg == NULL)
	    {
	    	IO_Print("CLI_GetNode InternetGatewayDevice.X_TWSZ-COM_MAC_Filter.BlackList. Msg is NULL \n");
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

	    pCurPosSrc = pCurPos;

	    if(TRUE == bAllMac)
	    {
			for (ulTemp=0; ulTemp<ulCount; ulTemp++)
			{
				pTempPos = strstr(pCurPos,CLI_MAC_PATH_BLACKLIST);
				if(pTempPos != NULL)
				{
					sscanf(pTempPos + STB_StrLen(CLI_MAC_PATH_BLACKLIST), "%lu.%s", &ulTempID, szTemp);
					IO_Print("%lu : %s\n",ulTempID, szTemp);
				}
				pCurPos += strlen(pCurPos) + 1;
			}
	     }
	    else
	    {
	    /*
	    	   for (ulTemp=0; ulTemp<ulCount; ulTemp++)
		   {
			    pTempPos = strstr(pCurPos, "MACAddress=");
			    if (pTempPos != NULL)
			     {
			             char *pPos = strchr(pCurPos, '=');
			             if (pPos != NULL)
			             {
			               //判断pMacName值是否相同
			             		  if (strcmp(pMacName, pPos+1) == 0)
			               	 {
			                    
			                     //跳过前面的"string "，共7个字节
			                     //因为CMM返回的一个item类似这样的:
			                     //"string InternetGatewayDevice.X_TWSZ-COM_DDNS.1.MACAddress=test.tzo.com"
			                     
			                    		strncpy(szMacInstPath, pCurPos+7, pTempPos-(pCurPos+7));
							bFindMac = TRUE;
			                    		break;
			                	}
			            	}
			        }

			        //指向下一个item
			        pCurPos += STB_StrLen(pCurPos) + 1;
		}
		if(FALSE == bFindMac)
		{
			IO_Print("Do not find mac %s \n",pMacName);
			return TBS_FAILED;
		}
		*/
		if(FALSE ==  CLI_WLAN_FindTargetStr(ulCount,szMacInstPath,pCurPos,pMacName,"MACAddress=") )
		{
			IO_Print("Do not find mac %s \n",pMacName);
			return TBS_FAILED;
		}
	IO_Print("szMacInstPath =%s\n",szMacInstPath); //ffor debug
		pCurPos = pCurPosSrc;
		for (ulTemp=0; ulTemp<ulCount; ulTemp++)
		{
			pTempPos = strstr(pCurPos,szMacInstPath);
			if(pTempPos  != NULL)
			{
				sscanf(pTempPos + STB_StrLen(CLI_MAC_PATH_BLACKLIST), "%lu.%s", &ulTempID, szTemp);
				IO_Print("%s\n",szTemp);
			}

			pCurPos += strlen(pCurPos) + 1;
		}
				
	}
	
	return TBS_SUCCESS;
}

static BOOL CLI_WLAN_FindTargetStr(ULONG ulCount, char szMacInstPath[MAX_PATH_LEN],char* pCurPos, char* szTargetStr, char* TargetStrCur)
{
	int ulTemp = 0;
	BOOL bFind = FALSE;
	if(NULL == szMacInstPath || NULL == pCurPos ||  NULL==szTargetStr  || NULL == TargetStrCur )
	{
		IO_Print("Error ! parameter NULL in CLI_WLAN_FindTargetStr()!\n");
		return bFind;
	}
	for (ulTemp=0; ulTemp<ulCount; ulTemp++)
	 {

		char *pTempPos = strstr(pCurPos, TargetStrCur);
		if (pTempPos != NULL)
		{
			 char *pPos = strchr(pCurPos, '=');
			  if (pPos != NULL)
			  {
			   //判断pMacName值是否相同
			              if (strcmp(szTargetStr, pPos+1) == 0)
			               {
			                    /*
			                     *跳过前面的"string "，共7个字节
			                     *因为CMM返回的一个item类似这样的:
			                     *"string InternetGatewayDevice.X_TWSZ-COM_DDNS.1.MACAddress=test.tzo.com"
			                     */
			                  strncpy(szMacInstPath, pCurPos+7, pTempPos-(pCurPos+7));
						bFind = TRUE;
			                    	break;
			                }
			      }
		}

			        //指向下一个item
		pCurPos += STB_StrLen(pCurPos) + 1;
	}

	return bFind;
}

/***************************************************************************************************
功能: 设置使wlan的mac black list有效
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注:无
****************************************************************************************************/
static ULONG CLI_MAC_Enable(void)
{
	char aszName[1][CLI_LEAF__LEN];
	char aszVal[1][CLI_LEAF__LEN];

	if(STB_StrLen(CLI_MAC_PATH) > CLI_LEAF__LEN - 1)
	{
		IO_Print("ERROR! CLI_MAC_Enable path is larger than CLI_LEAF__LEN(512) \n");
		return TBS_FAILED;
	}

	 sprintf(aszName[0],"%s%s",CLI_MAC_PATH,CLI_WLAN_ENABLE_NAME);
	 strcpy(aszVal[0],CLI_WLAN_ENABLE_VALUE);

	 if(CLI_SetLeaf(1, aszName,aszVal))
	 {
	 	CLI_CommErrorPrint();
	       return TBS_FAILED;
	 }
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 设置使wlan的mac black list无效
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注:无
****************************************************************************************************/
static ULONG CLI_MAC_Disable(void)
{
	char aszName[1][CLI_LEAF__LEN];
	char aszVal[1][CLI_LEAF__LEN];

	if(STB_StrLen(CLI_MAC_PATH) > CLI_LEAF__LEN - 1)
	{
		IO_Print("ERROR! CLI_MAC_Enable path is larger than CLI_LEAF__LEN(512) \n");
		return TBS_FAILED;
	}

	 sprintf(aszName[0],"%s%s",CLI_MAC_PATH,CLI_WLAN_ENABLE_NAME);
	 strcpy(aszVal[0],CLI_WLAN_DISABLE_VALUE);

	 if(CLI_SetLeaf(1, aszName,aszVal))
	 {
	 	CLI_CommErrorPrint();
	       return TBS_FAILED;
	 }
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 显示wlan的mac 过滤信息
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注:无
****************************************************************************************************/
#define MAX_WIRELESS_MAC_NUN 32  //MAC列表中MAC的最大数目
#define MAX_WIRELESS_LAN_NUM 4   //Wireless Lan的数目
static ULONG CLI_Wireless_MAC_Show(void)
{
    ULONG   ulCount  	= 0;
    char    szValue[CLI_LEAF__LEN];
    char    *pAllowedMac[MAX_WIRELESS_MAC_NUN]={NULL};
    ULONG   ulIndex = 0;
    char    szPath[MAX_PATH_LEN];


    //IO_Print("CLI_Wireless_MAC_Show: ulCount = %lu \n", ulCount);

    //显示wireless LAN的MACAddressControlEnabled
    for (ulIndex=0; ulIndex < MAX_WIRELESS_LAN_NUM; ulIndex++)
    {
        sprintf(szPath, "%s%lu.%s", CLI_WLANCONFIGURATION_PATH, ulIndex+1, "MACAddressControlEnabled");
        if(TBS_SUCCESS != CLI_GetLeaf(szPath, szValue))
        {
            IO_Print("\nWireless get MAC enable flag error \n");
            return TBS_FAILED;
        }
        IO_Print("\nWLAN MAC control %s \n", strcmp(szValue, "1") ? "disable" : "enable");
        break;
    }

    //获取AllowedMACAddresses
	 if (TBS_SUCCESS != CLI_GetLeaf(CLI_WIRELESS_MAC_PATH, szValue))
	{
        CLI_CommErrorPrint();
        return TBS_FAILED;
	}

    //IO_Print("Wireless Mac value: %s \n", szValue);

    //显示MAC地址列表
    ulCount = CLI_SplitString(szValue, pAllowedMac, ',', MAX_WIRELESS_MAC_NUN);
    if(ulCount > MAX_WIRELESS_MAC_NUN)
    {
        IO_Print("\nError: Mac addr count error\n");
        return TBS_FAILED;
    }
    else if(ulCount == 0)
    {
        IO_Print("\nThe MAC list is empty\n");
    }
    else
    {
        //IO_Print("Wireless Mac count is %lu\n", ulCount);
        
        //逐行显示MAC列表
        IO_Print("\nThe MAC list: \n");
        for(ulIndex = 0; ulIndex < ulCount; ulIndex++)
        {
            IO_Print("    %s \n", pAllowedMac[ulIndex]);
        }
    }
    
	
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 设置wlan的mac 过滤功能为enable
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注:无
****************************************************************************************************/
static ULONG CLI_Wireless_MAC_Enable(void)
{
    ULONG  ulIndex = 0;
    char   aszPath[1][CLI_LEAF__LEN];
	char   aszVal[1][CLI_LEAF__LEN];

    
    strcpy(aszVal[0], CLI_WLAN_ENABLE_VALUE);

    //分别设置每个wireless LAN对象对应的MACAddressControlEnabled为enable
    for (ulIndex = 0; ulIndex < MAX_WIRELESS_LAN_NUM; ulIndex++)
    {
        sprintf(aszPath[0], "%s%lu.%s", CLI_WLANCONFIGURATION_PATH, ulIndex+1, "MACAddressControlEnabled");
        if(CLI_SetLeaf(1, aszPath, aszVal))
        {
            IO_Print("\nWireless set MAC enable flag error \n");
            return TBS_FAILED;
        }
    }

	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 设置wlan的mac 过滤功能为disable
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注:无
****************************************************************************************************/
static ULONG CLI_Wireless_MAC_Disable(void)
{
    ULONG  ulIndex = 0;
    char   aszPath[1][CLI_LEAF__LEN];
	char   aszVal[1][CLI_LEAF__LEN];

    //分别设置每个wireless LAN对象对应的MACAddressControlEnabled为enable
    strcpy(aszVal[0], CLI_WLAN_DISABLE_VALUE);
    for (ulIndex = 0; ulIndex < MAX_WIRELESS_LAN_NUM; ulIndex++)
    {
        sprintf(aszPath[0], "%s%lu.%s", CLI_WLANCONFIGURATION_PATH, ulIndex+1, "MACAddressControlEnabled");
        if(CLI_SetLeaf(1, aszPath, aszVal))
        {
            IO_Print("\nWireless set MAC disable flag error, index=%d\n", ulIndex);
            return TBS_FAILED;
        }
    }

	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 从wlan的mac 过滤表中删除匹配的MAC
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注:无
****************************************************************************************************/
static ULONG CLI_Wireless_MAC_Del(void)
{
    char    szValue[CLI_LEAF__LEN];
    char    aszPath[1][CLI_LEAF__LEN];
	char    aszVal[1][CLI_LEAF__LEN];
	char    *pMacName   = NULL;
    char    *pTemp = NULL;

	if(NULL == (pMacName = CLI_GetParamByName("mac-v")) )
	{
		IO_Print("ERROR! Get parameter mac-v is NULL in CLI_Wireless_MAC_Del()!\n");
		return TBS_FAILED;
	}
	CLI_FormatMac(pMacName);


    //获取AllowedMACAddresses
	 if (TBS_SUCCESS != CLI_GetLeaf(CLI_WIRELESS_MAC_PATH, szValue))
	{
        CLI_CommErrorPrint();
        return TBS_FAILED;
	}

    if(strlen(szValue) > 0)
    {
        //找到后删除对应的MAC
        if((pTemp = strstr(szValue, pMacName)) != NULL)
        {
            //MAC位置可能出现在列表头、中、尾，各种情况都要考虑到。
            if(strlen(pTemp) > strlen(pMacName))
            {
                strcpy(pTemp, pTemp + strlen(pMacName) + 1);
            }
            else if(pTemp != szValue)
            {
                *(pTemp - 1) = '\0'; 
            }
            else
            {
                *pTemp = '\0';
            }
            sprintf(aszPath[0], "%s", CLI_WIRELESS_MAC_PATH);
            sprintf(aszVal[0], "%s", szValue);
            if(CLI_SetLeaf(1, aszPath, aszVal))
            {
                IO_Print("\nWireless set MAC filter failed \n");
                return TBS_FAILED;
            }
        }
        else
        {
            IO_Print("\nNo MAC matched! \n");
        }
    }
    else
    {
        IO_Print("\nWireless filtered MAC list is empty \n");
    }
	
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 向wlan的mac 过滤表中增加MAC
参数: 无
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 设置失败
备注:无
****************************************************************************************************/
static ULONG CLI_Wireless_MAC_Add(void)
{
    char    szValue[CLI_LEAF__LEN];
    char    aszPath[1][CLI_LEAF__LEN];
	char    aszVal[1][CLI_LEAF__LEN];
	char    *pMacName   = NULL;

	if(NULL == (pMacName = CLI_GetParamByName("mac-v")) )
	{
		IO_Print("\nERROR! Get parameter mac-v is NULL in CLI_Wireless_MAC_Add()!\n");
		return TBS_FAILED;
	}
	CLI_FormatMac(pMacName);


    //获取AllowedMACAddresses
	 if (TBS_SUCCESS != CLI_GetLeaf(CLI_WIRELESS_MAC_PATH, szValue))
	{
        CLI_CommErrorPrint();
        return TBS_FAILED;
	}

    if(strlen(szValue) > 0)
    {
        //MAC列表不为空时，且新MAC在MAC列表中不存在时，添加
        if(strstr(szValue, pMacName) == NULL)
        {
            sprintf(aszPath[0], "%s", CLI_WIRELESS_MAC_PATH);
            sprintf(aszVal[0], "%s,%s", szValue, pMacName);
            if(CLI_SetLeaf(1, aszPath, aszVal))
            {
                IO_Print("\nWireless set MAC filter failed \n");
                return TBS_FAILED;
            }
        }
        else
        {
            IO_Print("\nThe MAC already in the list! \n");
        }
    }
    //如果MAC列表为空，直接添加
    else
    {
        sprintf(aszPath[0], "%s", CLI_WIRELESS_MAC_PATH);
        sprintf(aszVal[0], "%s", pMacName);
        if(CLI_SetLeaf(1, aszPath, aszVal))
        {
            IO_Print("\nWireless add MAC failed \n");
            return TBS_FAILED;
        }
    }
	
	return TBS_SUCCESS;
}


/***************************************************************************************************
功能: 对输入的days格式进行解析
参数: 输入: pszDayValue : 要解析的days字串
	    : 输出: aszDays : 解析的结果
返回: nDays: Days格式合格总天数
           : TBS_FAILED :格式不合格
备注: 输入的合格days需形如sun-mon-tue , tue-thu等及取每天的前三个字符代表該天用"-"来连接
****************************************************************************************************/
static int CLI_ParseDays(char* pszDaysValue, char   aszDays[CLI_WEEK_DAYS_NUM][CLI_MAX_DAY_NUM]) 
{
	int nDays = 0, i = 0;

	char  pszDayTemp[4] = {0};
	if(NULL == pszDaysValue || STB_StrLen(pszDaysValue) >CLI_DAYS_STR_MAX_SIZE -1 )
	{
		IO_Print("ERROR! pszDaysValue is NULL or size too larger in CLI_ParseDays() \n");
		return TBS_FAILED;
	}
	else
	{	
		char * pPosCur = NULL;
		char*  pEqlPos  = NULL;

		pPosCur = pszDaysValue;

		if(*pPosCur == '-')
		{
				IO_Print("warning do not start with '-' please!!!");
				return TBS_FAILED;
		}

		while(*pPosCur)
		{
			pEqlPos = strchr(pPosCur + 1, '-');


			if(pEqlPos != NULL)
			{
		
				CLI_DAYS_SIZE_ASSERT(pPosCur,pEqlPos);
				for(i = 0; i < 3; ++i)
				{
					pszDayTemp[i] = *(pPosCur + i);
				}
				if((i = CLI_DaysFormatCheck(pszDayTemp) ) != TBS_FAILED)
				{	
					STB_StrCpy(aszDays[nDays],days[i]);
					nDays ++;
				}
				else
				{
					IO_Print("Days: %s format error\n",pszDayTemp);
					return TBS_FAILED;
				}
			}
			else
			{
				if(STB_StrLen(pPosCur) == 3)
				{
					for(i = 0; i < 3; ++i)
					{
						pszDayTemp[i] = *(pPosCur + i);
					}
					if((i = CLI_DaysFormatCheck(pszDayTemp) ) != TBS_FAILED)
					{
						STB_StrCpy(aszDays[nDays],days[i]);
						nDays ++;
					}
					else
					{
						IO_Print("Days: %s format error\n",pszDayTemp);
						return TBS_FAILED;
					}
				}
				else
				{
					IO_Print("Days: %s format error\n",pPosCur);
					return TBS_FAILED;
				}
			}

			pPosCur = pPosCur + CLI_MAX_DAY_NUM;
			
		}
		
	}
	return nDays;
}

/***************************************************************************************************
功能: 检查days的字串格式是否合格
参数: pszDayTemp : 待解析的字串
返回: i: 表示在全局与之匹配的位置
           : TBS_FAILED : 设置失败
备注:无
****************************************************************************************************/
static int CLI_DaysFormatCheck(char* pszDayTemp)
{
	int i = 0;
	for(i = 0; i < CLI_WEEK_DAYS_NUM; i++)
	{
		if(STB_StrNiCmp(days[i],pszDayTemp,3) == 0)
		{
			return i;
		}
	}
	return TBS_FAILED;
}
/***************************************************************************************************
功能: 检查mac的字串格式并将输入的格式改成配置能识别的格式
参数: pszMacAddress : 输入输出的mac字串
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
备注:输入形式如00-00-00-11-11-11输出为00:00:00:11:11:11
****************************************************************************************************/
static ULONG CLI_FormatMac(char* pszMacAddress)
{
	if(NULL == pszMacAddress)
	{
		IO_Print("macAddress is NULL in CLI_FormatMac\n");
		return TBS_FAILED;
	}
	while(*pszMacAddress)
	{
		if(*pszMacAddress == '-')
		{
			*pszMacAddress = ':';
			pszMacAddress++;
		}
		else
			pszMacAddress++;
			
	}
	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 检查wepKey字串格式是否符合配置要求
参数: WEPKeyValue : 输入的wepKey字串
	    : szWEPEncryptionLevel : 指出wepKey的类别
返回: TBS_SUCCESS: 成功
           : TBS_FAILED : 失败
备注:输入的字串可以为指定长度的字符字串或16进度数据字串
****************************************************************************************************/
static ULONG CLI_WLAN_CheckWEPKeyValue(char * WEPKeyValue, char * szWEPEncryptionLevel)
{
	int length 	= 0;

	char * szKeyTemp = WEPKeyValue;

	if(STB_StrCmp(szWEPEncryptionLevel,"64bits") == 0)
	{
		length = 10;
	}
	else if(STB_StrCmp(szWEPEncryptionLevel,"128bits") == 0)
	{
		length = 26;
	}

	if(length > 0)
	{
		
		if(STB_StrLen(WEPKeyValue) == length)      // 十六进制数据的key
		{
			while(*szKeyTemp)
			{
				CLI_WLAN_CHECK_LETTER_HEX(*szKeyTemp);
				szKeyTemp++;
			}
		}
		else if(STB_StrLen(WEPKeyValue) != length/2) 
		{
			IO_Print("WEP Key  length Error !");
			return TBS_FAILED;
		}
		
	}
	else
	{
		IO_Print("_atol  return value not right  ! \n");
		return TBS_FAILED;
	}

	return TBS_SUCCESS;
}

/***************************************************************************************************
功能: 字符串分割，将分割结果保存在二维数组中
参数: 无
返回: 小于max_count为正常
           等于大于max_count为失败
备注:无
****************************************************************************************************/
static int CLI_SplitString(char *str, char *substr[], const char delimit, const int max_count)
{
	int count = 0;

	if ( str == NULL || substr == NULL )
		return max_count;

    if(strlen(str) == 0)
    {
        return 0;
    }

	if ( 0 < max_count )
			substr[0] = str;

	count = 1;
	while ( *str )
	{

		while ( *str != delimit && *str != '\0' )
			str++;

		if ( *str == delimit )
		{
			*str++ = '\0';
			if(count < max_count)
				substr[count] = str;
			count++;
		}
	}

	return count;
}

/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/
 
#ifdef __cplusplus
}
#endif



