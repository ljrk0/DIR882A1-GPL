/**********************************************************************
 Copyright (c), 1991-2010, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 File Name: cli_pmapping.c
 File description: wan function for CLI
 Modify history:
        1. Author: Wangjing
           Date: 2010-06-22
           Content: Create the file
**********************************************************************/

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


#define CLI_LAYER2_BRIDGE_BASE "InternetGatewayDevice.Layer2Bridging.Bridge."
#define CLI_BRIDGE_NAME     "BridgeName"
#define CLI_BRIDGE_VLANID   "VLANID"

#define CLI_PARA_LEN    256
#define CLI_NODE_TR069_BASIC    "InternetGatewayDevice.ManagementServer."
#define CLI_NODE_DEVICE_BASIC    "InternetGatewayDevice.DeviceInfo."

#define CLI_GET_PARAMETER_ASSERT(pPointer, name) do \
{ \
    (pPointer) = CLI_GetParamByName((name)); \
    if ((pPointer) == NULL) \
    { \
        return TBS_FAILED; \
    } \
}while(0);


/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/
ULONG CLI_Config_TR069();
ULONG CLI_Show_TR069();
ULONG CLI_CHG_Enable_to_STR();
ULONG CLI_Request_TR069();

ULONG CLI_TR069()
{
    if (CLI_GetParamByName("config") != NULL)
    {
        return CLI_Config_TR069();
    }
    else if (CLI_GetParamByName("show") != NULL)
    {
        return CLI_Show_TR069();
    }
    else if (CLI_GetParamByName("request") != NULL)
    {
        return CLI_Request_TR069();
    }    

    return TBS_FAILED;
}


ULONG CLI_Config_TR069()
{
    char szPath[6][CLI_NODE__LEN];
    char szValue[6][CLI_NODE__LEN];
    int ulConfigNum = 0;
    
    memset(szPath, 0x00, sizeof(szPath));
    memset(szValue, 0x00, sizeof(szValue));

     if(CLI_GetParamByName("status") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.EnableCWMP");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("Enable-value"));
            if(strcmp(szValue[ulConfigNum],"enable")==0)
            {
                memset(szValue[ulConfigNum],0,sizeof(szValue[ulConfigNum]));
                strcpy(szValue[ulConfigNum],"1");
            }
            else if(strcmp(szValue[ulConfigNum],"disable")==0)
            {
                 memset(szValue[ulConfigNum],0,sizeof(szValue[ulConfigNum]));
                 strcpy(szValue[ulConfigNum],"0");
            }
            else
            {
                IO_Print("\nfail:should be enable or disable\n");
                return TBS_FAILED;
            }
            ulConfigNum++;
     }
       if(CLI_GetParamByName("Inform") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.PeriodicInformInterval");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("Inform-Interval"));
            ulConfigNum++;
     }
          if(CLI_GetParamByName("URL") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.URL");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("URL-v"));
            ulConfigNum++;
     }

     if(CLI_GetParamByName("username") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.Username");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("username-v"));
            ulConfigNum++;
     }
    if(CLI_GetParamByName("password") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.Password");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("password-v"));
            ulConfigNum++;
     }
     if(CLI_GetParamByName("InfEnable") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.PeriodicInformEnable");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("InfEnable-v"));
            if(strcmp(szValue[ulConfigNum],"enable")==0)
            {
                memset(szValue[ulConfigNum],0,sizeof(szValue[ulConfigNum]));
                strcpy(szValue[ulConfigNum],"1");
            }
            else if(strcmp(szValue[ulConfigNum],"disable")==0)
            {
                 memset(szValue[ulConfigNum],0,sizeof(szValue[ulConfigNum]));
                 strcpy(szValue[ulConfigNum],"0");
            }
            else
            {
                IO_Print("\nfail:should be enable or disable\n");
                return TBS_FAILED;
            }
            ulConfigNum++;
     }
    //…Ë÷√
    if (CLI_SetLeaf(ulConfigNum, szPath, szValue) != TBS_SUCCESS)
    { 
        IO_Print("\nfail~~~\n");
        return TBS_FAILED;
    }
    IO_Print("\nsuccess!\n");   
    return TBS_SUCCESS;
}

ULONG CLI_Show_TR069()
{
    char szPath[CLI_NODE__LEN] = {0};
    char szTr069Enable[CLI_MAX_RWOBJ_NUM] = {0};
    char szInformEnable[CLI_MAX_RWOBJ_NUM] = {0};
    char szInformInterval[CLI_MAX_RWOBJ_NUM] = {0};
    char szURL[CLI_PARA_LEN] = {0};
    char szUsername[CLI_PARA_LEN] = {0};
    char szPassword[CLI_PARA_LEN] = {0};
    char szRequestEnable[CLI_MAX_RWOBJ_NUM] = {0};
    char szRequestUsername[CLI_PARA_LEN] = {0};
    char szRequestPassword[CLI_PARA_LEN] = {0};

    char szReqPath[CLI_PARA_LEN] = {0};
    char szReqPort[CLI_PARA_LEN] = {0};

    char szOui[CLI_PARA_LEN] = {0};
    char szProductclass[CLI_PARA_LEN] = {0};
	char szSn[CLI_PARA_LEN] = {0};
	char szManuf[CLI_PARA_LEN] = {0};
	char szModulName[CLI_PARA_LEN] = {0};
    
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "EnableCWMP");
    if (CLI_GetLeaf(szPath, szTr069Enable) != TBS_SUCCESS)
    {
        IO_Print("Tr069Enable Not available \n");
    }
    CLI_CHG_Enable_to_STR(szTr069Enable, CLI_MAX_RWOBJ_NUM);

    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "PeriodicInformEnable");
    if (CLI_GetLeaf(szPath, szInformEnable) != TBS_SUCCESS)
    {
        IO_Print("PeriodicInformEnable Not available \n");
    }
    CLI_CHG_Enable_to_STR(szInformEnable, CLI_MAX_RWOBJ_NUM);
    
    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "PeriodicInformInterval");    
    if (CLI_GetLeaf(szPath, szInformInterval) != TBS_SUCCESS)
    {
        IO_Print("PeriodicInformInterval Not available \n");
    }

    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "URL");    
    if (CLI_GetLeaf(szPath, szURL) != TBS_SUCCESS)
    {
        IO_Print("URL Not available \n");
    }
    
    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "Username");    
    if (CLI_GetLeaf(szPath, szUsername) != TBS_SUCCESS)
    {
        IO_Print("Username Not available \n");
    }
    
    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "Password");        
    if (CLI_GetLeaf(szPath, szPassword) != TBS_SUCCESS)
    {
        IO_Print("Password Not available \n");
    }
    
    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "ConnectionRequestEnable");    
    if (CLI_GetLeaf(szPath, szRequestEnable) != TBS_SUCCESS)
    {
        IO_Print("ConnectionRequestEnable Not available \n");
    }
    CLI_CHG_Enable_to_STR(szRequestEnable, CLI_MAX_RWOBJ_NUM);
    
    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "ConnectionRequestUsername");    
    if (CLI_GetLeaf(szPath, szRequestUsername) != TBS_SUCCESS)
    {
        IO_Print("ConnectionRequestUsername Not available \n");
    }
    
    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "ConnectionRequestPassword");    
    if (CLI_GetLeaf(szPath, szRequestPassword) != TBS_SUCCESS)
    {
        IO_Print("ConnectionRequestPassword Not available \n");
    }

    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "ConnectionRequestPath");    
    if (CLI_GetLeaf(szPath, szReqPath) != TBS_SUCCESS)
    {
        IO_Print("ConnectionRequestPath Not available \n");
    }
	
    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_TR069_BASIC, "ConnectionRequestPort");    
    if (CLI_GetLeaf(szPath, szReqPort) != TBS_SUCCESS)
    {
        IO_Print("ConnectionRequestPort Not available \n");
    }
  /*device info*/
     memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_DEVICE_BASIC, "ManufacturerOUI");    
    if (CLI_GetLeaf(szPath, szOui) != TBS_SUCCESS)
    {
        IO_Print("ManufacturerOUI Not available \n");
    }
	
    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_DEVICE_BASIC, "ProductClass");    
    if (CLI_GetLeaf(szPath, szProductclass) != TBS_SUCCESS)
    {
        IO_Print("ProductClass Not available \n");
    }
	  memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_DEVICE_BASIC, "SerialNumber");    
    if (CLI_GetLeaf(szPath, szSn) != TBS_SUCCESS)
    {
        IO_Print("SerialNumber Not available \n");
    }
	
    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_DEVICE_BASIC, "Manufacturer");    
    if (CLI_GetLeaf(szPath, szManuf) != TBS_SUCCESS)
    {
        IO_Print("Manufacturer Not available \n");
    }
    memset(szPath, 0x00, sizeof(szPath));
    sprintf(szPath, "%s%s", CLI_NODE_DEVICE_BASIC, "ModelName");    
    if (CLI_GetLeaf(szPath, szModulName) != TBS_SUCCESS)
    {
        IO_Print("ModelName Not available \n");
    }
	

	
    IO_Print("\n===================================================");    
    IO_Print("\n               Tr069 Enable: %s", szTr069Enable);
    IO_Print("\n              Inform Enable: %s", szInformEnable);
    IO_Print("\n            Inform Interval: %s", szInformInterval);
    IO_Print("\n                    ACS URL: %s", szURL);
    IO_Print("\n               ACS Username: %s", szUsername);
    IO_Print("\n               ACS Password: %s", "***********");
    IO_Print("\n  Connection Request Enable: %s", szRequestEnable);
    IO_Print("\nConnection Request Username: %s", szRequestUsername);
    IO_Print("\nConnection Request Password: %s", "***********");
    IO_Print("\n    Connection Request Path: %s", szReqPath);
    IO_Print("\n    Connection Request Port: %s", szReqPort);
    IO_Print("\n                    CPE OUI: %s", szOui);
    IO_Print("\n           CPE ProductClass: %s", szProductclass);
    IO_Print("\n           CPE SerialNumber: %s", szSn);
    IO_Print("\n            CPE Manufacture: %s", szManuf);
    IO_Print("\n              CPE ModelName: %s\n", szModulName);
    return TBS_SUCCESS;
}

ULONG CLI_Request_TR069()
{
    char szPath[6][CLI_NODE__LEN];
    char szValue[6][CLI_NODE__LEN];

    int ulConfigNum = 0;
    
    memset(szPath, 0x00, sizeof(szPath));
    memset(szValue, 0x00, sizeof(szValue));

     if(CLI_GetParamByName("RequestStatus") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.ConnectionRequestEnable");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("Enable-value"));
            if(strcmp(szValue[ulConfigNum],"enable")==0)
            {
                memset(szValue[ulConfigNum],0,sizeof(szValue[ulConfigNum]));
                strcpy(szValue[ulConfigNum],"1");
            }
            else if(strcmp(szValue[ulConfigNum],"disable")==0)
            {
                 memset(szValue[ulConfigNum],0,sizeof(szValue[ulConfigNum]));
                 strcpy(szValue[ulConfigNum],"0");
            }
            else
            {                IO_Print("\nfail:should be enable or disable\n");
                return TBS_FAILED;
            }
            ulConfigNum++;
     }
     if(CLI_GetParamByName("username") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.ConnectionRequestUsername");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("RequestUsername"));
            ulConfigNum++;
     }
    if(CLI_GetParamByName("password") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.ConnectionRequestPassword");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("RequestPassword"));
            ulConfigNum++;
     }
   if(CLI_GetParamByName("path") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.ConnectionRequestPath");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("RequestPath"));
            ulConfigNum++;
     }
        if(CLI_GetParamByName("port") != NULL )
     {
            STB_StrCpy(szPath[ulConfigNum], "InternetGatewayDevice.ManagementServer.ConnectionRequestPort");
            STB_StrCpy(szValue[ulConfigNum], CLI_GetParamByName("RequestPort"));
            ulConfigNum++;
     }
     
    //…Ë÷√
    if (CLI_SetLeaf(ulConfigNum, szPath, szValue) != TBS_SUCCESS)
    { 
        IO_Print("\nfail~~~\n");
        return TBS_FAILED;
    }
    IO_Print("\nsuccess!\n");      
    return TBS_SUCCESS;
}

ULONG CLI_CHG_Enable_to_STR(char *szEnable, int ulLEN)
{
    if(0 == strcmp(szEnable, "1"))
    {
        memset(szEnable, 0x00, ulLEN);
        strcpy(szEnable, "Enable");
        return TBS_SUCCESS;
    }
    else if(0 == strcmp(szEnable, "0"))
    {
        memset(szEnable, 0x00, ulLEN);
        strcpy(szEnable, "Disable");
        return TBS_SUCCESS;
    }
    return TBS_FAILED;
}

#ifdef __cplusplus
}
#endif
