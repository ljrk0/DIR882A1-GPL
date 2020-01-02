/**********************************************************************
 Copyright (c), 1991-2010, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 File Name: cli_wan.c
 File description: wan function for CLI
 Modify history:
        1. Author: Tangqiang
           Date: 2010-01-13
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

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/
#define CLI_WAN_TRACE(x)  IO_Print x
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define CLI_WAN_IP_CONNECTION_NAME      "string InternetGatewayDevice.WANDevice.1.WANConnectionDevice.%lu.WANIPConnection.%lu.Name=%516s"
#define CLI_WAN_PPP_CONNECTION_NAME     "string InternetGatewayDevice.WANDevice.1.WANConnectionDevice.%lu.WANPPPConnection.%lu.Name=%516s"
#define CLI_WAN_IP_CONNECTION_NAME_ETH      "string InternetGatewayDevice.WANDevice.2.WANConnectionDevice.%lu.WANIPConnection.%lu.Name=%516s"
#define CLI_WAN_PPP_CONNECTION_NAME_ETH     "string InternetGatewayDevice.WANDevice.2.WANConnectionDevice.%lu.WANPPPConnection.%lu.Name=%516s"

#define CLI_WAN_DEVICE_BASE             "InternetGatewayDevice.WANDevice.1.WANConnectionDevice."
#define CLI_WAN_DEVICE_BASE_ETH             "InternetGatewayDevice.WANDevice.2.WANConnectionDevice."
#define CLI_WAN_IP_CONNECTION           "WANIPConnection"
#define CLI_WAN_PPP_CONNECTION          "WANPPPConnection"
#define CLI_WAN_CONNECTION_DEVICE       "WANConnectionDevice"

//This macro is used to get the string value
#define CLI_GET_PARAMETER_ASSERT(pPointer, name) do \
{ \
    (pPointer) = CLI_GetParamByName((name)); \
    if ((pPointer) == NULL) \
    { \
        return TBS_FAILED; \
    } \
}while(0);

//This macro is used to get the unsiged long port number
#define CLI_GET_ULONG_ASSERT(ulValue, name) do \
{ \
    if (CLI_GetUlongByName(name, &ulValue) != TBS_SUCCESS) \
    { \
        return TBS_FAILED; \
    } \
}while(0);
		

/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/
enum{
  PPPOE_CONNECTION  = 1,
  PPPOA_CONNECTION  = 2,
  MER_CONNECTION    = 3,
  IPOA_CONNECTION   = 4,
  BRIDGE_CONNECTION = 5
};

enum{
  CLI_WAN_CONN_NAME = 0,
  CLI_WAN_CONN_PROTOCOL,
//  CLI_WAN_CONN_VPI,
//  CLI_WAN_CONN_VCI,
  CLI_WAN_CONN_ENCAPSULATE,
  CLI_WAN_CONN_NAT,
  CLI_WAN_CONN_USERNAME,
  CLI_WAN_CONN_PASSWORD,
  CLI_WAN_CONN_DIALUP,
  CLI_WAN_CONN_PPPTIMEOUT,
  CLI_WAN_CONN_PPPMRU,
  CLI_WAN_CONN_SERVICE,
  CLI_WAN_CONN_ADDRESS_TYPE,
  CLI_WAN_CONN_IPADDR,
  CLI_WAN_CONN_NETMASK,
  CLI_WAN_CONN_REMOTEIP,

  CLI_WAN_CONN_STATUS,  //yuan
  CLI_WAN_CONN_Authentication_Method,  //yuan  

  CLI_WAN_CONN_DSLTYPE,
  CLI_WAN_CONN_PVC,
  CLI_WAN_CONN_VLANID,

  CLI_WAN_CONN_END
};

enum{
  CLI_PPP_CONN_NAME = 0,
  CLI_PPP_CONN_MODE,
  CLI_PPP_CONN_PVC,
  CLI_PPP_CONN_ENCAP,
  CLI_PPP_CONN_ATMQOS,
  CLI_PPP_CONN_USERNAME,
  CLI_PPP_CONN_PASSWORD,
  CLI_PPP_CONN_ADDRESS_TYPE,
  CLI_PPP_CONN_IPADDR,
  CLI_PPP_CONN_NETMASK,
  CLI_PPP_CONN_REMOTEIP,
  CLI_PPP_CONN_STATUS,  //yuan
  CLI_PPP_CONN_END
};

/******************************************************************************
 *                               GLOBAL VAR                                   *
 ******************************************************************************/


/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/

ULONG CLI_Wan_Add_Connection();
ULONG CLI_Wan_Modify_Connection();
ULONG CLI_Wan_Del_Connection();
ULONG CLI_Wan_Show();
ULONG CLI_PPP_GET();
ULONG CLI_PPP_SET();

ULONG CLI_CheckWanInterfaceExist(ULONG ulVpi, ULONG ulVci);
ULONG CLI_CheckWanInterfaceExist2(ULONG ulVpi, ULONG ulVci,char *pszprotel);
static ULONG CLI_GetPathByConnName(char *pszConnName, char *pszPath, int iLen);
ULONG CLI_SearchTotalNumberOfPvc(ULONG *pNum);

//main function
ULONG CLI_Wan()
{
    if (CLI_GetParamByName("add") != NULL)
    {
        return CLI_Wan_Add_Connection();
    }
    else if (CLI_GetParamByName("del") != NULL)
    {
        return CLI_Wan_Del_Connection();
    }
    else if (CLI_GetParamByName("modify") != NULL)
    {
        return CLI_Wan_Modify_Connection();
    }
    
    else if (CLI_GetParamByName("show") != NULL)
    {
        return CLI_Wan_Show();
    }
    else if (CLI_GetParamByName("get") != NULL)
    {
        return CLI_PPP_GET();
    }
    else if (CLI_GetParamByName("set") != NULL)
    {
        return CLI_PPP_SET();
    }
    
    return TBS_SUCCESS;
}

/*************************************************************************
function  : Add a new wan connection as giving parameters
parameters: none
return    : TBS_SUCCESS if success
            TBS_FAILED if not success
remark    : Use CLI command such as:
            wan add pppoe 0 32 LLC nat enable username aaa password aaa
*************************************************************************/
ULONG CLI_Wan_Add_Connection()
{
    ULONG ulWanIndex   = 0;
    ULONG ulPppIpIndex = 0;
    char *pszProtocol  = NULL;
    ULONG ulVpi        = 0;
    ULONG ulVci        = 32;
    ULONG ulVlan        = 0;
    ULONG ulVlanpri        = 0;
    char *pszEncap     = NULL;
    char *pszNat       = NULL;
    char *pszUsername  = NULL;
    char *pszPassword  = NULL;
    char *pszDialup    = NULL;
    ULONG ulPppTimeout = 0;
    ULONG ulPppMru     = 0;
    char *pszService   = NULL;
    char *pszIpaddr    = NULL;
    char *pszNetmask   = NULL;
    char *pszRemoteIp  = NULL;
    char *pszDnsIp = NULL;
    char *pszMode  = NULL;
    char *pszServ  = "UBRPlus";

    char szPath[MAX_PATH_LEN]    = {0};
    char szPathSub[MAX_PATH_LEN] = {0};
    ULONG ulPvcNumber            = 0;
    UCHAR ucIpPppType            = 0;
    ULONG result                 = 0;

    /* 分配数组超过最大参数个数，保证不会出错 */
    char aszPath[18][CLI_NODE__LEN];  //Increase this range if more items will be set
    char aszValue[18][CLI_NODE__LEN];
    //char *ppszPath     = NULL;
    //char *ppszValue    = NULL;
    ULONG ulParamsCount = 0;

    //Get the parameters of the wan connection
    CLI_GET_PARAMETER_ASSERT(pszProtocol, "protocol");
    CLI_GET_PARAMETER_ASSERT(pszMode, "mode-v");
   
    
    if (CLI_GetParamByName("vpi") != NULL)
    {
        CLI_GET_ULONG_ASSERT(ulVpi, "vpi-v");
    }
    if (CLI_GetParamByName("vci") != NULL)
    {
       CLI_GET_ULONG_ASSERT(ulVci, "vci-v");
    }
     if (CLI_GetParamByName("vlan") != NULL)
    {
        CLI_GET_ULONG_ASSERT(ulVlan, "vlan-v");
    }
    if (CLI_GetParamByName("vlanpri") != NULL)
    {
       CLI_GET_ULONG_ASSERT(ulVlanpri, "vlanpri-v");
    }
    if (CLI_GetParamByName("encap") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszEncap, "encap-value");
    }
    if (CLI_GetParamByName("nat") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszNat, "nat-value");
    }
    if (CLI_GetParamByName("username") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszUsername, "username-value");
    }
    if (CLI_GetParamByName("password") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszPassword, "password-value");
    }
    if (CLI_GetParamByName("dialup") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszDialup, "dialup-value");
    }
    if (CLI_GetParamByName("pppidletimeout") != NULL)
    {
        CLI_GET_ULONG_ASSERT(ulPppTimeout, "pppidletimeout-value");
    }
    if (CLI_GetParamByName("pppmru") != NULL)
    {
        CLI_GET_ULONG_ASSERT(ulPppMru, "pppmru-value");
    }
    if (CLI_GetParamByName("service") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszService, "service-name");
    }
    if (CLI_GetParamByName("ipaddr") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszIpaddr, "ipaddr-value");
    }
    if (CLI_GetParamByName("netmask") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszNetmask, "netmask-value");
    }
    if (CLI_GetParamByName("remoteip") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszRemoteIp, "remoteip-value");
    }
    if (CLI_GetParamByName("dnsip") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszDnsIp, "dnsip-value");
    }
    if (CLI_GetParamByName("serv") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszServ, "serv-v");
    }

    //Check if the PVC(ulVpi,ulVci) is exist.
    if(strcmp(pszMode,"ATM") ==0)
    {
        if(pszEncap == NULL)   //默认为空，在ATM模式下会报错
        {
            pszEncap = "LLC";
        }
        if( (strcmp(pszProtocol, "pppoe") == 0) ||(strcmp(pszProtocol, "bridge") == 0) ||(strcmp(pszProtocol, "mer") == 0))
        {
            result = CLI_CheckWanInterfaceExist2(ulVpi, ulVci,pszProtocol);
        }
        else
        {
            result = CLI_CheckWanInterfaceExist(ulVpi, ulVci);
        }
        if (result == TRUE)
        {
            IO_Print("Wan Interface Exist! Please delete the exist PVC first!");
            return TBS_FAILED;
        }
        else if (result == TBS_FAILED)
        {
            return TBS_FAILED;
        }
  
        //Search out the total number of PVCs which exist
        if (CLI_SearchTotalNumberOfPvc(&ulPvcNumber) != TBS_SUCCESS)
        {
            CLI_WAN_TRACE(("%s: Failed to get the total number of pvc\n", __FUNCTION__));
            return TBS_FAILED;
        }
    }
    if(strcmp(pszMode,"PTM") ==0)
    {
        if( (strcmp(pszProtocol, "pppoa") == 0) ||(strcmp(pszProtocol, "ipoa") == 0) )
        {
            IO_Print("\n  Input error: PTM Protocol don't have 'pppoa' or 'ipoa', please choose 'pppoe' 'mer' or 'bridge' to input again!");
            return TBS_FAILED;
        }
        if(pszEncap != NULL)
        {
            IO_Print("\n  Input error: PTM don't have encap, please don't input encap!");
            return TBS_FAILED;
        }
    }
    /*
     * Note: Please follow the steps: Add WANConnectionDevice node, then set params of this node; Add 
     * IPConnection node or PPPConnection node, then set params of this node. The actions of setting
     * params must be seperated, otherwise the PVC doesn't work.
     */
    /*****************InternetGatewayDevice.WANDevice.1.WANConnectionDevice.********************/
    if(strcmp(pszMode,"ATM") ==0 || strcmp(pszMode,"PTM") ==0 )
    {
        if (CLI_AddObjectEx(CLI_WAN_DEVICE_BASE, &ulWanIndex) != TBS_SUCCESS)
        {
            CLI_WAN_TRACE(("%s: Failed to add new node: %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE));
            return TBS_FAILED;
        }
            sprintf(szPath, "%s%lu.", CLI_WAN_DEVICE_BASE, ulWanIndex);
    }
    else
    {
        if( (strcmp(pszProtocol, "pppoa") == 0) ||(strcmp(pszProtocol, "ipoa") == 0) )
        {
            IO_Print("\n  Input error: ETHERNET Protocol don't have 'pppoa' or 'ipoa', please choose 'pppoe' 'mer' or 'bridge' to input again!");
	     return TBS_FAILED;
        }
        if(pszEncap != NULL)
        {
            IO_Print("\n  Input error: ETHERNET don't have encap, please don't input encap!");
	     return TBS_FAILED;
        }
		
        if (CLI_AddObjectEx(CLI_WAN_DEVICE_BASE_ETH, &ulWanIndex) != TBS_SUCCESS)
        {
            CLI_WAN_TRACE(("%s: Failed to add new node: %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE));
            return TBS_FAILED;
        }
        sprintf(szPath, "%s%lu.", CLI_WAN_DEVICE_BASE_ETH, ulWanIndex);
	
    }
 
    if (strcmp(pszProtocol, "pppoa") == 0)
    {
        ucIpPppType = PPPOA_CONNECTION;
    }
    else if (strcmp(pszProtocol, "pppoe") == 0)
    {
        ucIpPppType = PPPOE_CONNECTION;
    }
    else if (strcmp(pszProtocol, "mer") == 0)
    {
        ucIpPppType = MER_CONNECTION;
    }
    else if (strcmp(pszProtocol, "ipoa") == 0)
    {
        ucIpPppType = IPOA_CONNECTION;
    }
    else if (strcmp(pszProtocol, "bridge") == 0)
    {
        ucIpPppType = BRIDGE_CONNECTION;
    }

    if(strcmp(pszMode,"ATM") ==0 )
    {
		if ((ucIpPppType == PPPOE_CONNECTION) || (ucIpPppType == MER_CONNECTION) || (ucIpPppType == BRIDGE_CONNECTION))
		{
			sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "LinkType");
			sprintf(aszValue[ulParamsCount], "%s", "EoA");
			ulParamsCount++;
		}
		else if (ucIpPppType == PPPOA_CONNECTION)
		{
			sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "LinkType");
			sprintf(aszValue[ulParamsCount], "%s", "PPPoA");
			ulParamsCount++;
		}
		else if (ucIpPppType == IPOA_CONNECTION)
		{
			sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "LinkType");
			sprintf(aszValue[ulParamsCount], "%s", "IPoA");
			ulParamsCount++;
		}
		else
		{
			//Unknown pvc type
			return TBS_FAILED;
		}
	 
		sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "ATMEncapsulation");
		if(0 == strcasecmp(pszEncap, "LLC"))
		{
			strcpy(aszValue[ulParamsCount], "LLC");
		}
		else if(0 == strcasecmp(pszEncap, "VCMUX"))
		{
			strcpy(aszValue[ulParamsCount], "VCMUX");
		}
		ulParamsCount++;

		sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "DestinationAddress");
		sprintf(aszValue[ulParamsCount], "PVC:%lu/%lu", ulVpi, ulVci);
		ulParamsCount++;

		sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "ATMQoS");
		if(strcasecmp(pszServ,"UBRPlus")==0)
		{
			strcpy(aszValue[ulParamsCount], "UBR+");
		}
		else if(strcasecmp(pszServ,"UBR")==0 )
		{
			strcpy(aszValue[ulParamsCount], "UBR");
		}
		else if(strcasecmp(pszServ,"CBR")==0 )
		{
			strcpy(aszValue[ulParamsCount], "CBR");
		}
		else if(strcasecmp(pszServ,"VBR-nrt")==0 )
		{
			strcpy(aszValue[ulParamsCount], "VBR-nrt");
		}
		  else if(strcasecmp(pszServ,"VBR-rt")==0 )
		{
			strcpy(aszValue[ulParamsCount], "VBR-rt");
		}
		else
		{
		   strcpy(aszValue[ulParamsCount], "UBR+");
		}
		ulParamsCount++;
    }
  
    if(strcmp(pszMode,"ATM") ==0 || strcmp(pszMode,"PTM") ==0 )
    {
        sprintf(aszPath[ulParamsCount], "%s%s", szPath, "X_TWSZ-COM_DSLType");
        strcpy(aszValue[ulParamsCount],pszMode);
        ulParamsCount++;
    }
  
    sprintf(aszPath[ulParamsCount], "%s%s", szPath, "X_TWSZ-COM_VLANID");
    sprintf(aszValue[ulParamsCount], "%lu",ulVlan);
    ulParamsCount++;
    sprintf(aszPath[ulParamsCount], "%s%s", szPath, "X_TWSZ-COM_VLANPriority");
    sprintf(aszValue[ulParamsCount], "%lu",ulVlanpri);
    ulParamsCount++;

    //Begin set the values
    if (CLI_SetLeaf(ulParamsCount, aszPath, aszValue) != TBS_SUCCESS)
    {
        CLI_DelObject(szPath);
        return TBS_FAILED;
    }
    ulParamsCount = 0;

    /*******************WANIPConnection or WANPPPConnection*******************/
    //Add WANPPPConnection or WANIPConnection as specified
    if ((ucIpPppType == PPPOE_CONNECTION) || (ucIpPppType == PPPOA_CONNECTION))
    {
        sprintf(szPathSub, "%s%s.", szPath, CLI_WAN_PPP_CONNECTION);
    }
    else if ((ucIpPppType == MER_CONNECTION) || (ucIpPppType == IPOA_CONNECTION) || (ucIpPppType == BRIDGE_CONNECTION))
    {
        sprintf(szPathSub, "%s%s.", szPath, CLI_WAN_IP_CONNECTION);
    }
    else
    {
        CLI_DelObject(szPath);
        //Unknown type
        return TBS_FAILED;
    }

    if (CLI_AddObjectEx(szPathSub, &ulPppIpIndex) != TBS_SUCCESS)
    {
        CLI_WAN_TRACE(("%s: Failed to add new node: %s\n", __FUNCTION__, szPathSub));
        CLI_DelObject(szPath);
        return TBS_FAILED;
    }

    CLI_WAN_TRACE(("%s: Begin setting the params of wan connection: %d\n", __FUNCTION__, ucIpPppType));
    //Fill the parameters for PPPoE
    if (ucIpPppType == PPPOE_CONNECTION)
    {
        if(strcmp(pszMode,"ATM") ==0 || strcmp(pszMode,"PTM") ==0)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
            sprintf(aszValue[ulParamsCount], "%s%lu", "D_PPPoE_0_",ulWanIndex);
            ulParamsCount++;
        }
        else
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
            sprintf(aszValue[ulParamsCount], "%s%lu", "E_PPPoE_0_",ulWanIndex);
            ulParamsCount++; 
        }
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionType");
        sprintf(aszValue[ulParamsCount], "%s", "IP_Routed");
        ulParamsCount++;
        if (pszNat != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "NATEnabled");
            if (strcmp(pszNat, "enable") == 0)
            {
                sprintf(aszValue[ulParamsCount], "%d", 1);
            }
            else
            {
                sprintf(aszValue[ulParamsCount], "%d", 0);
            }
            ulParamsCount++;
        }

        if (pszUsername != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Username");
            sprintf(aszValue[ulParamsCount], "%s", pszUsername);
            ulParamsCount++;
        }

        if (pszPassword != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Password");
            sprintf(aszValue[ulParamsCount], "%s", pszPassword);
            ulParamsCount++;
        }

        if (pszDialup != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionTrigger");
            if(0 == strcasecmp(pszDialup, "AlwaysOn"))
            {
                strcpy(aszValue[ulParamsCount], "AlwaysOn");
            }
            else if(0 == strcasecmp(pszDialup, "OnDemand"))
            {
                strcpy(aszValue[ulParamsCount], "OnDemand");
            }
            else if(0 == strcasecmp(pszDialup, "Manual"))
            {
                strcpy(aszValue[ulParamsCount], "Manual");
            }
            ulParamsCount++;
        }

        if (ulPppTimeout != 0)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "IdleDisconnectTime");
            sprintf(aszValue[ulParamsCount], "%lu", ulPppTimeout);
            ulParamsCount++;
        }

        if (ulPppMru != 0)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "MaxMRUSize");
            sprintf(aszValue[ulParamsCount], "%lu", ulPppMru);
            ulParamsCount++;
        }

        if (pszService != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "PPPoEServiceName");
            sprintf(aszValue[ulParamsCount], "%s", pszService);
            ulParamsCount++;
        }
    }  //End of PPPOE_CONNECTION
    else if (ucIpPppType == PPPOA_CONNECTION)
    {
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
        sprintf(aszValue[ulParamsCount], "D_PPPoA_0_%lu", ulWanIndex);
        ulParamsCount++;

        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionType");
        sprintf(aszValue[ulParamsCount], "%s", "IP_Routed");
        ulParamsCount++;

        if (pszNat != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "NATEnabled");
            if (strcmp(pszNat, "enable") == 0)
            {
                sprintf(aszValue[ulParamsCount], "%d", 1);
            }
            else
            {
                sprintf(aszValue[ulParamsCount], "%d", 0);
            }
            ulParamsCount++;
        }

        if (pszUsername != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Username");
            sprintf(aszValue[ulParamsCount], "%s", pszUsername);
            ulParamsCount++;
        }

        if (pszPassword != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Password");
            sprintf(aszValue[ulParamsCount], "%s", pszPassword);
            ulParamsCount++;
        }

        if (pszDialup != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionTrigger");
            if (strcasecmp(pszDialup, "Manual") == 0)
            {
                strcpy(aszValue[ulParamsCount], "Manual");
            }
            else if (strcasecmp(pszDialup, "AlwaysOn") == 0)
            {
                strcpy(aszValue[ulParamsCount], "AlwaysOn");
            }
            else if (strcasecmp(pszDialup, "OnDemand") == 0)
            {
                strcpy(aszValue[ulParamsCount], "OnDemand");
            }
            ulParamsCount++;
        }

        if (ulPppTimeout != 0)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "IdleDisconnectTime");
            sprintf(aszValue[ulParamsCount], "%lu", ulPppTimeout);
            ulParamsCount++;
        }

        if (ulPppMru != 0)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "MaxMRUSize");
            sprintf(aszValue[ulParamsCount], "%lu", ulPppMru);
            ulParamsCount++;
        }
    }  //End of PPPOA_CONNECTION
    else if (ucIpPppType == MER_CONNECTION)
    {
		if(strcmp(pszMode,"ATM") ==0 || strcmp(pszMode,"PTM") ==0 )
		{
			sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
			sprintf(aszValue[ulParamsCount], "D_MER_0_%lu", ulWanIndex);
			ulParamsCount++;
		}
		else
		{
			sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
			sprintf(aszValue[ulParamsCount], "E_MER_0_%lu", ulWanIndex);
			ulParamsCount++;
		}
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionType");
        sprintf(aszValue[ulParamsCount], "%s", "IP_Routed");
        ulParamsCount++;

        if (pszNat != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "NATEnabled");
            if (strcmp(pszNat, "enable") == 0)
            {
                sprintf(aszValue[ulParamsCount], "%d", 1);
            }
            else
            {
                sprintf(aszValue[ulParamsCount], "%d", 0);
            }
            ulParamsCount++;
        }

        if (pszIpaddr != NULL) //Static
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "AddressingType");
            sprintf(aszValue[ulParamsCount], "%s", "Static");
            ulParamsCount++;

            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ExternalIPAddress");
            sprintf(aszValue[ulParamsCount], "%s", pszIpaddr);
            ulParamsCount++;

            if (pszNetmask != NULL)
            {
                sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "SubnetMask");
                sprintf(aszValue[ulParamsCount], "%s", pszNetmask);
                ulParamsCount++;
            }

            if (pszRemoteIp != NULL)
            {
                sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "DefaultGateway");
                sprintf(aszValue[ulParamsCount], "%s", pszRemoteIp);
                ulParamsCount++;
            }
	        if(pszDnsIp != NULL)
            {
                sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "DNSServers");
                sprintf(aszValue[ulParamsCount], "%s", pszDnsIp);
                ulParamsCount++;
            }
        }
        else                   //DHCP
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "AddressingType");
            sprintf(aszValue[ulParamsCount], "%s", "DHCP");
            ulParamsCount++;
        }
    }  //End of MER_CONNECTION
    else if (ucIpPppType == IPOA_CONNECTION)
    {
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
        sprintf(aszValue[ulParamsCount], "D_IPoA_0_%lu", ulWanIndex);
        ulParamsCount++;

        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionType");
        sprintf(aszValue[ulParamsCount], "%s", "IP_Routed");
        ulParamsCount++;

        if (pszNat != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "NATEnabled");
            if (strcmp(pszNat, "enable") == 0)
            {
                sprintf(aszValue[ulParamsCount], "%d", 1);
            }
            else
            {
                sprintf(aszValue[ulParamsCount], "%d", 0);
            }
            ulParamsCount++;
        }
        
        if (pszIpaddr != NULL) //Static
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "AddressingType");
            sprintf(aszValue[ulParamsCount], "%s", "Static");
            ulParamsCount++;

            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ExternalIPAddress");
            sprintf(aszValue[ulParamsCount], "%s", pszIpaddr);
            ulParamsCount++;

            if (pszNetmask != NULL)
            {
                sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "SubnetMask");
                sprintf(aszValue[ulParamsCount], "%s", pszNetmask);
                ulParamsCount++;
            }

            if (pszRemoteIp != NULL)
            {
                sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "DefaultGateway");
                sprintf(aszValue[ulParamsCount], "%s", pszRemoteIp);
                ulParamsCount++;
            }

	       if(pszDnsIp != NULL)
            {
                sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "DNSServers");
                sprintf(aszValue[ulParamsCount], "%s", pszDnsIp);
                ulParamsCount++;
            }
        }
        else                   //DHCP
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "AddressingType");
            sprintf(aszValue[ulParamsCount], "%s", "DHCP");
            ulParamsCount++;
        }
    }  //End of IPOA_CONNECTION
    else if (ucIpPppType == BRIDGE_CONNECTION)
    {
		if(strcmp(pszMode,"ATM") ==0 || strcmp(pszMode,"PTM") ==0)
		{
			sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
			sprintf(aszValue[ulParamsCount], "D_Bridging_0_%lu", ulWanIndex);
			ulParamsCount++;
		}
		else
		{
			sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
			sprintf(aszValue[ulParamsCount], "E_Bridging_0_%lu", ulWanIndex);
			ulParamsCount++;
		}
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionType");
        sprintf(aszValue[ulParamsCount], "%s", "IP_Bridged");
        ulParamsCount++;
    }  //End of BRIDGE_CONNECTION
    else
    {
        //Unknown pvc type
        CLI_WAN_TRACE(("%s: Unknow pvc type\n", __FUNCTION__));
        return TBS_FAILED;
    }

    if ((ucIpPppType == PPPOE_CONNECTION) || (ucIpPppType == PPPOA_CONNECTION))
    {
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "PPPLCPEchoRetry");
        sprintf(aszValue[ulParamsCount], "0");
        ulParamsCount++;
    }
    
    sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Enable");
    sprintf(aszValue[ulParamsCount], "%d", 1);
    ulParamsCount++;
    
    CLI_WAN_TRACE(("%s: Set %lu items\n", __FUNCTION__, ulParamsCount));

    //Begin set the values
    if (CLI_SetLeaf(ulParamsCount, aszPath, aszValue) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}

#if 0
/*************************************************************************
function  : Add a new wan connection as giving parameters
parameters: none
return    : TBS_SUCCESS if success
            TBS_FAILED if not success
remark    : Use CLI command such as:
            wan add pppoe 0 32 LLC nat enable username aaa password aaa
*************************************************************************/

ULONG CLI_Wan_Modify_Connection()
{
    ST_MSG *pstMsg = NULL;
    char *pCurPos  = NULL;
    char *pTempPos = NULL;
    ULONG ulCount  = 0;
    ULONG ulTemp   = 0;

    char *szName     = NULL;
    ULONG ulWanIndex   = 0;
    ULONG ulPppIpIndex = 0;
    char *pszProtocol  = NULL;
    ULONG ulVpi        = 0;
    ULONG ulVci        = 0;
    char *pszEncap     = NULL;
    char *pszNat       = NULL;
    char *pszUsername  = NULL;
    char *pszPassword  = NULL;
    char *pszDialup    = NULL;
    ULONG ulPppTimeout = 0;
    ULONG ulPppMru     = 0;
    char *pszService   = NULL;
    char *pszIpaddr    = NULL;
    char *pszNetmask   = NULL;
    char *pszRemoteIp  = NULL;

    char szPath[MAX_PATH_LEN]    = {0};
    char szPathSub[MAX_PATH_LEN] = {0};
    ULONG ulPvcNumber            = 0;
    UCHAR ucIpPppType            = 0;
    ULONG result                 = 0;

    char aszPath[10][CLI_NODE__LEN];  //Increase this range if more items will be set
    char aszValue[10][CLI_NODE__LEN];
   // char *ppszPath     = NULL;
   // char *ppszValue    = NULL;
    ULONG ulParamsCount = 0;

    /*********************************************************************
    delete the existed connection first
    **********************************************************************/
    CLI_GET_PARAMETER_ASSERT(szName, "pvc");

    pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE);
    if (pstMsg == NULL)
    {
        CLI_WAN_TRACE(("\n%s: Failed to the node of %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE));
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_WAN_TRACE(("\n%s: The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    for(; ulTemp < ulCount; ulTemp++)
    {
        pTempPos = strstr(pCurPos, ".Name=");
        if (pTempPos != NULL)
        {
            if (strcmp(szName, (pTempPos + strlen(".Name="))) == 0)
            {
                //Point to the back of the InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.
                pTempPos = strstr(pCurPos, CLI_WAN_CONNECTION_DEVICE);
                pTempPos += (strlen(CLI_WAN_CONNECTION_DEVICE) + 2);
                
                //skip the "string " of the pCurPos, its structure is "string XX.XX.XX=XX"
                strncpy(szPath, pCurPos + 7, (pTempPos - pCurPos - 7 + 1));

                if (CLI_DelObject(szPath) != TBS_SUCCESS)
                {
                    CLI_WAN_TRACE(("\n%s: Delete object %s failed\n", __FUNCTION__, szPath));
                    return TBS_FAILED;
                }
                break;
            }
        }

        pCurPos += (strlen(pCurPos) + 1);
    }
    pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE_ETH);
    if (pstMsg == NULL)
    {
        CLI_WAN_TRACE(("\n%s: Failed to the node of %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE));
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_WAN_TRACE(("\n%s: The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    for(; ulTemp < ulCount; ulTemp++)
    {
        pTempPos = strstr(pCurPos, ".Name=");
        if (pTempPos != NULL)
        {
            if (strcmp(szName, (pTempPos + strlen(".Name="))) == 0)
            {
                //Point to the back of the InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.
                pTempPos = strstr(pCurPos, CLI_WAN_CONNECTION_DEVICE);
                pTempPos += (strlen(CLI_WAN_CONNECTION_DEVICE) + 2);
                
                //skip the "string " of the pCurPos, its structure is "string XX.XX.XX=XX"
                strncpy(szPath, pCurPos + 7, (pTempPos - pCurPos - 7 + 1));

                if (CLI_DelObject(szPath) != TBS_SUCCESS)
                {
                    CLI_WAN_TRACE(("\n%s: Delete object %s failed\n", __FUNCTION__, szPath));
                    return TBS_FAILED;
                }
                break;
            }
        }

        pCurPos += (strlen(pCurPos) + 1);
    }


    CLI_WAN_TRACE(("%s: Connection:%s has been deleted successfully.\n", __FUNCTION__, szName));    

    /*********************************************************************
    add the new connection then
    **********************************************************************/
    CLI_GET_PARAMETER_ASSERT(pszProtocol, "protocol");
    CLI_GET_ULONG_ASSERT(ulVpi, "vpi");
    CLI_GET_ULONG_ASSERT(ulVci, "vci");
    CLI_GET_PARAMETER_ASSERT(pszEncap, "encap");
    if (CLI_GetParamByName("nat") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszNat, "nat-value");
    }
    if (CLI_GetParamByName("username") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszUsername, "username-value");
    }
    if (CLI_GetParamByName("password") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszPassword, "password-value");
    }
    if (CLI_GetParamByName("dialup") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszDialup, "dialup-value");
    }
    if (CLI_GetParamByName("pppidletimeout") != NULL)
    {
        CLI_GET_ULONG_ASSERT(ulPppTimeout, "pppidletimeout-value");
    }
    if (CLI_GetParamByName("pppmru") != NULL)
    {
        CLI_GET_ULONG_ASSERT(ulPppMru, "pppmru-value");
    }
    if (CLI_GetParamByName("service") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszService, "service-name");
    }
    if (CLI_GetParamByName("ipaddr") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszIpaddr, "ipaddr-value");
    }
    if (CLI_GetParamByName("netmask") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszNetmask, "netmask-value");
    }
    if (CLI_GetParamByName("remoteip") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszRemoteIp, "remoteip-value");
    }

    //Check if the PVC(ulVpi,ulVci) is exist. 
   result = CLI_CheckWanInterfaceExist(ulVpi, ulVci);
    if (result == TRUE)
    {
        IO_Print("Wan Interface Exist! Please delete the exist PVC first!");
        return TBS_FAILED;
    }
    else if (result == TBS_FAILED)
    {
        return TBS_FAILED;
    }

    //Search out the total number of PVCs which exist
    if (CLI_SearchTotalNumberOfPvc(&ulPvcNumber) != TBS_SUCCESS)
    {
        CLI_WAN_TRACE(("%s: Failed to get the total number of pvc\n", __FUNCTION__));
        return TBS_FAILED;
    }

    /*
     * Note: Please follow the steps: Add WANConnectionDevice node, then set params of this node; Add 
     * IPConnection node or PPPConnection node, then set params of this node. The actions of setting
     * params must be seperated, otherwise the PVC doesn't work.
     */
    /*****************InternetGatewayDevice.WANDevice.1.WANConnectionDevice.********************/
    if (CLI_AddObjectEx(CLI_WAN_DEVICE_BASE, &ulWanIndex) != TBS_SUCCESS)
    {
        CLI_WAN_TRACE(("%s: Failed to add new node: %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE));
        return TBS_FAILED;
    }
    sprintf(szPath, "%s%lu.", CLI_WAN_DEVICE_BASE, ulWanIndex);

    if (strcmp(pszProtocol, "pppoa") == 0)
    {
        ucIpPppType = PPPOA_CONNECTION;
    }
    else if (strcmp(pszProtocol, "pppoe") == 0)
    {
        ucIpPppType = PPPOE_CONNECTION;
    }
    else if (strcmp(pszProtocol, "mer") == 0)
    {
        ucIpPppType = MER_CONNECTION;
    }
    else if (strcmp(pszProtocol, "ipoa") == 0)
    {
        ucIpPppType = IPOA_CONNECTION;
    }
    else if (strcmp(pszProtocol, "bridge") == 0)
    {
        ucIpPppType = BRIDGE_CONNECTION;
    }

    if ((ucIpPppType == PPPOE_CONNECTION) || (ucIpPppType == MER_CONNECTION) || (ucIpPppType == BRIDGE_CONNECTION))
    {
        sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "LinkType");
        sprintf(aszValue[ulParamsCount], "%s", "EoA");
        ulParamsCount++;
    }
    else if (ucIpPppType == PPPOA_CONNECTION)
    {
        sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "LinkType");
        sprintf(aszValue[ulParamsCount], "%s", "PPPoA");
        ulParamsCount++;
    }
    else if (ucIpPppType == IPOA_CONNECTION)
    {
        sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "LinkType");
        sprintf(aszValue[ulParamsCount], "%s", "IPoA");
        ulParamsCount++;
    }
    else
    {
        //Unknown pvc type
        return TBS_FAILED;
    }

    sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "ATMEncapsulation");
    if(0 == strcasecmp(pszEncap, "LLC"))
    {
        strcpy(aszValue[ulParamsCount], "LLC");
    }
    else if(0 == strcasecmp(pszEncap, "VCMUX"))
    {
        strcpy(aszValue[ulParamsCount], "VCMUX");
    }
    ulParamsCount++;
        
    sprintf(aszPath[ulParamsCount], "%s%s.%s", szPath, "WANDSLLinkConfig", "DestinationAddress");
    sprintf(aszValue[ulParamsCount], "PVC:%lu/%lu", ulVpi, ulVci);
    ulParamsCount++;

    sprintf(aszPath[ulParamsCount], "%s%s", szPath, "X_TWSZ-COM_VLANID");
    sprintf(aszValue[ulParamsCount], "0");
    ulParamsCount++;
   
    //Begin set the values
    if (CLI_SetLeaf(ulParamsCount, aszPath, aszValue) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    ulParamsCount = 0;

    /*******************WANIPConnection or WANPPPConnection*******************/
    //Add WANPPPConnection or WANIPConnection as specified
    if ((ucIpPppType == PPPOE_CONNECTION) || (ucIpPppType == PPPOA_CONNECTION))
    {
        sprintf(szPathSub, "%s%s.", szPath, CLI_WAN_PPP_CONNECTION);
    }
    else if ((ucIpPppType == MER_CONNECTION) || (ucIpPppType == IPOA_CONNECTION) || (ucIpPppType == BRIDGE_CONNECTION))
    {
        sprintf(szPathSub, "%s%s.", szPath, CLI_WAN_IP_CONNECTION);
    }
    else
    {
        //Unknown type
        return TBS_FAILED;
    }

    if (CLI_AddObjectEx(szPathSub, &ulPppIpIndex) != TBS_SUCCESS)
    {
        CLI_WAN_TRACE(("%s: Failed to add new node: %s\n", __FUNCTION__, szPathSub));
        return TBS_FAILED;
    }

    CLI_WAN_TRACE(("%s: Begin setting the params of wan connection: %d\n", __FUNCTION__, ucIpPppType));
    //Fill the parameters for PPPoE
    if (ucIpPppType == PPPOE_CONNECTION)
    {
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
        sprintf(aszValue[ulParamsCount], "PVC:%lu/%lu", ulVpi, ulVci);
        ulParamsCount++;

        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionType");
        sprintf(aszValue[ulParamsCount], "%s", "IP_Routed");
        ulParamsCount++;

        if (pszNat != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "NATEnabled");
            if (strcmp(pszNat, "enable") == 0)
            {
                sprintf(aszValue[ulParamsCount], "%d", 1);
            }
            else
            {
                sprintf(aszValue[ulParamsCount], "%d", 0);
            }
            ulParamsCount++;
        }

        if (pszUsername != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Username");
            sprintf(aszValue[ulParamsCount], "%s", pszUsername);
            ulParamsCount++;
        }

        if (pszPassword != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Password");
            sprintf(aszValue[ulParamsCount], "%s", pszPassword);
            ulParamsCount++;
        }

        if (pszDialup != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionTrigger");
            if(0 == strcasecmp(pszDialup, "AlwaysOn"))
            {
                strcpy(aszValue[ulParamsCount], "AlwaysOn");
            }
            else if(0 == strcasecmp(pszDialup, "OnDemand"))
            {
                strcpy(aszValue[ulParamsCount], "OnDemand");
            }
            else if(0 == strcasecmp(pszDialup, "Manual"))
            {
                strcpy(aszValue[ulParamsCount], "Manual");
            }
            ulParamsCount++;
        }

        if (ulPppTimeout != 0)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "IdleDisconnectTime");
            sprintf(aszValue[ulParamsCount], "%lu", ulPppTimeout);
            ulParamsCount++;
        }

        if (ulPppMru != 0)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "MaxMRUSize");
            sprintf(aszValue[ulParamsCount], "%lu", ulPppMru);
            ulParamsCount++;
        }

        if (pszService != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "PPPoEServiceName");
            sprintf(aszValue[ulParamsCount], "%s", pszService);
            ulParamsCount++;
        }
    }  //End of PPPOE_CONNECTION
    else if (ucIpPppType == PPPOA_CONNECTION)
    {
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
        sprintf(aszValue[ulParamsCount], "PVC:%lu/%lu", ulVpi, ulVci);
        ulParamsCount++;

        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionType");
        sprintf(aszValue[ulParamsCount], "%s", "IP_Routed");
        ulParamsCount++;

        if (pszNat != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "NATEnabled");
            if (strcmp(pszNat, "enable") == 0)
            {
                sprintf(aszValue[ulParamsCount], "%d", 1);
            }
            else
            {
                sprintf(aszValue[ulParamsCount], "%d", 0);
            }
            ulParamsCount++;
        }

        if (pszUsername != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Username");
            sprintf(aszValue[ulParamsCount], "%s", pszUsername);
            ulParamsCount++;
        }

        if (pszPassword != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Password");
            sprintf(aszValue[ulParamsCount], "%s", pszPassword);
            ulParamsCount++;
        }

        if (pszDialup != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionTrigger");
            sprintf(aszValue[ulParamsCount], "%s", pszDialup);
            ulParamsCount++;
        }

        if (ulPppTimeout != 0)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "IdleDisconnectTime");
            sprintf(aszValue[ulParamsCount], "%lu", ulPppTimeout);
            ulParamsCount++;
        }

        if (ulPppMru != 0)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "MaxMRUSize");
            sprintf(aszValue[ulParamsCount], "%lu", ulPppMru);
            ulParamsCount++;
        }
    }  //End of PPPOA_CONNECTION
    else if (ucIpPppType == MER_CONNECTION)
    {
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
        sprintf(aszValue[ulParamsCount], "PVC:%lu/%lu", ulVpi, ulVci);
        ulParamsCount++;

        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionType");
        sprintf(aszValue[ulParamsCount], "%s", "IP_Routed");
        ulParamsCount++;

        if (pszNat != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "NATEnabled");
            if (strcmp(pszNat, "enable") == 0)
            {
                sprintf(aszValue[ulParamsCount], "%d", 1);
            }
            else
            {
                sprintf(aszValue[ulParamsCount], "%d", 0);
            }
            ulParamsCount++;
        }

        if (pszIpaddr != NULL) //Static
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "AddressingType");
            sprintf(aszValue[ulParamsCount], "%s", "Static");
            ulParamsCount++;

            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ExternalIPAddress");
            sprintf(aszValue[ulParamsCount], "%s", pszIpaddr);
            ulParamsCount++;

            if (pszNetmask != NULL)
            {
                sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "SubnetMask");
                sprintf(aszValue[ulParamsCount], "%s", pszNetmask);
                ulParamsCount++;
            }

            if (pszRemoteIp != NULL)
            {
                sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "DefaultGateway");
                sprintf(aszValue[ulParamsCount], "%s", pszRemoteIp);
                ulParamsCount++;
            }
        }
        else                   //DHCP
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "AddressingType");
            sprintf(aszValue[ulParamsCount], "%s", "DHCP");
            ulParamsCount++;
        }
    }  //End of MER_CONNECTION
    else if (ucIpPppType == IPOA_CONNECTION)
    {
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
        sprintf(aszValue[ulParamsCount], "PVC:%lu/%lu", ulVpi, ulVci);
        ulParamsCount++;

        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionType");
        sprintf(aszValue[ulParamsCount], "%s", "IP_Routed");
        ulParamsCount++;

        if (pszNat != NULL)
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "NATEnabled");
            if (strcmp(pszNat, "enable") == 0)
            {
                sprintf(aszValue[ulParamsCount], "%d", 1);
            }
            else
            {
                sprintf(aszValue[ulParamsCount], "%d", 0);
            }
            ulParamsCount++;
        }
        
        if (pszIpaddr != NULL) //Static
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "AddressingType");
            sprintf(aszValue[ulParamsCount], "%s", "Static");
            ulParamsCount++;

            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ExternalIPAddress");
            sprintf(aszValue[ulParamsCount], "%s", pszIpaddr);
            ulParamsCount++;

            if (pszNetmask != NULL)
            {
                sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "SubnetMask");
                sprintf(aszValue[ulParamsCount], "%s", pszNetmask);
                ulParamsCount++;
            }

            if (pszRemoteIp != NULL)
            {
                sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "DefaultGateway");
                sprintf(aszValue[ulParamsCount], "%s", pszRemoteIp);
                ulParamsCount++;
            }
        }
        else                   //DHCP
        {
            sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "AddressingType");
            sprintf(aszValue[ulParamsCount], "%s", "DHCP");
            ulParamsCount++;
        }
    }  //End of IPOA_CONNECTION
    else if (ucIpPppType == BRIDGE_CONNECTION)
    {
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Name");
        sprintf(aszValue[ulParamsCount], "PVC:%lu/%lu", ulVpi, ulVci);
        ulParamsCount++;

        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "ConnectionType");
        sprintf(aszValue[ulParamsCount], "%s", "IP_Bridged");
        ulParamsCount++;
    }  //End of BRIDGE_CONNECTION
    else
    {
        //Unknown pvc type
        CLI_WAN_TRACE(("%s: Unknow pvc type\n", __FUNCTION__));
        return TBS_FAILED;
    }

    if ((ucIpPppType == PPPOE_CONNECTION) || (ucIpPppType == PPPOA_CONNECTION))
    {
        sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "PPPLCPEchoRetry");
        sprintf(aszValue[ulParamsCount], "0");
        ulParamsCount++;
    }
    
    sprintf(aszPath[ulParamsCount], "%s%lu.%s", szPathSub, ulPppIpIndex, "Enable");
    sprintf(aszValue[ulParamsCount], "%d", 1);
    ulParamsCount++;
    
    CLI_WAN_TRACE(("%s: Set %lu items\n", __FUNCTION__, ulParamsCount));

    //Begin set the values
    if (CLI_SetLeaf(ulParamsCount, aszPath, aszValue) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    
    return TBS_SUCCESS;
}
#endif
/*************************************************************************
function  : Delete the specific PVC
parameters: none
return    : TBS_SUCCESS if success
            TBS_FAILED if not success
remark    : Use CLI command such as:
            wan del pppoe_0_32
*************************************************************************/
ULONG CLI_Wan_Del_Connection()
{
    ST_MSG *pstMsg = NULL;
    char *pCurPos  = NULL;
    char *pTempPos = NULL;
    ULONG ulCount  = 0;
    ULONG ulTemp   = 0;

    char *szName     = NULL;
    char szPath[256] = {0};

    //Get Parameter of PVC name
    CLI_GET_PARAMETER_ASSERT(szName, "pvc");

    pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE);
    if (pstMsg == NULL)
    {
        goto LABLE;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_WAN_TRACE(("\n%s: The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    for(; ulTemp < ulCount; ulTemp++)
    {
        pTempPos = strstr(pCurPos, ".Name=");
        if (pTempPos != NULL)
        {
            if (strcmp(szName, (pTempPos + strlen(".Name="))) == 0)
            {
                //Point to the back of the InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.
                pTempPos = strstr(pCurPos, CLI_WAN_CONNECTION_DEVICE);
                pTempPos += (strlen(CLI_WAN_CONNECTION_DEVICE) + 2);
                /* 上面的处理只考虑了wan的index是0-9的情况，如果超过9就不能只偏移2位 */
                while (*pTempPos != '.')
                {
                    ++pTempPos;
                }   
                
                //skip the "string " of the pCurPos, its structure is "string XX.XX.XX=XX"
                strncpy(szPath, pCurPos + 7, (pTempPos - pCurPos - 7 + 1));

                if (CLI_DelObject(szPath) != TBS_SUCCESS)
                {
                    CLI_WAN_TRACE(("\n%s: Delete object %s failed\n", __FUNCTION__, szPath));
                    return TBS_FAILED;
                }
                return TBS_SUCCESS;
            }
        }

        pCurPos += (strlen(pCurPos) + 1);
    }
LABLE:
    pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE_ETH);
    if (pstMsg == NULL)
    {
        CLI_WAN_TRACE(("\n%s: Failed to the node of %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE));
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_WAN_TRACE(("\n%s: The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    for(; ulTemp < ulCount; ulTemp++)
    {
        pTempPos = strstr(pCurPos, ".Name=");
        if (pTempPos != NULL)
        {
            if (strcmp(szName, (pTempPos + strlen(".Name="))) == 0)
            {
                //Point to the back of the InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.
                pTempPos = strstr(pCurPos, CLI_WAN_CONNECTION_DEVICE);
                pTempPos += (strlen(CLI_WAN_CONNECTION_DEVICE) + 2);
                /* 上面的处理只考虑了wan的index是0-9的情况，如果超过9就不能只偏移2位 */
                while (*pTempPos != '.')
                {
                    ++pTempPos;
                }   
                
                //skip the "string " of the pCurPos, its structure is "string XX.XX.XX=XX"
                strncpy(szPath, pCurPos + 7, (pTempPos - pCurPos - 7 + 1));

                if (CLI_DelObject(szPath) != TBS_SUCCESS)
                {
                    CLI_WAN_TRACE(("\n%s: Delete object %s failed\n", __FUNCTION__, szPath));
                    return TBS_FAILED;
                }
                return TBS_SUCCESS;
            }
        }

        pCurPos += (strlen(pCurPos) + 1);
    }

    CLI_WAN_TRACE(("%s: Can't find the pvc: %s\n", __FUNCTION__, szName));
    return TBS_FAILED;
}
/*************************************************************************
function  : Add a new wan connection as giving parameters
parameters: none
return    : TBS_SUCCESS if success
            TBS_FAILED if not success
remark    : Use CLI command such as:
            wan add pppoe 0 32 LLC nat enable username aaa password aaa
*************************************************************************/
ULONG CLI_Wan_Modify_Connection()
{
    int iRet = TBS_SUCCESS;
    iRet = CLI_Wan_Del_Connection();
    if(iRet == TBS_SUCCESS)
		return CLI_Wan_Add_Connection();
    else
        return TBS_FAILED;
		
}


/*************************************************************************
function  : List the PVCs or show the specific PVC
parameters: none
return    : TBS_SUCCESS if success
            TBS_FAILED if not success
remark    : Use CLI command such as:
            wan show
            wan show conn pppoe_0_32
*************************************************************************/
#define GET_LEFT_TO_VAL(tmpPath, formal, basePath, Node, Val) do \
	{ \
		sprintf((tmpPath),(formal), (basePath), (Node));\
		if( CLI_GetLeaf((tmpPath), (Val)) != TBS_SUCCESS) \
		{ \
			return TBS_FAILED; \
		} \
	}while(0);


ULONG CLI_Wan_Show()
{
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

    if (CLI_GetParamByName("conn") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszName, "pvc");
    }
    if (pszName != NULL)  //Show the detail of the specific PVC
    {
        
        ULONG ulTempSub = 0;
        UCHAR ucPvcType = 0;

        IO_Print("\n");
        
        //Initialize the array
        for(; ulTempSub < CLI_WAN_CONN_END; ulTempSub++)
        {
            memset(aszPvcData[ulTempSub], 0, 256);
        }
        /*
         * This is not the best way to get the PVC type(protocol), but it's a fast way
        */
        if (strstr(pszName, "PPPoE") !=NULL)
        {
            ucPvcType = PPPOE_CONNECTION;
        }
        else if (strstr(pszName, "PPPoA") != NULL)
        {
            ucPvcType = PPPOA_CONNECTION;
        }
        else if (strstr(pszName, "MER") !=NULL)
        {
            ucPvcType = MER_CONNECTION;
        }
        else if (strstr(pszName, "IPoA") != NULL)
        {
            ucPvcType = IPOA_CONNECTION;
        }
        else if (strstr(pszName, "Bridging") != NULL)
        {
            ucPvcType = BRIDGE_CONNECTION;
        }

        if (CLI_GetPathByConnName(pszName, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
        {
            CLI_WAN_TRACE(("\n%s: Failed to get the path of : %s\n", __FUNCTION__, pszName));
            return TBS_FAILED;
        }

        //Get the leaf of the wan connection
        GET_LEFT_TO_VAL(szPathSub, "%s%s", szPath, "Name", szValue);
        sprintf(aszPvcData[CLI_WAN_CONN_NAME], "%s\t\t%s\n", "Name", szValue);
		
        if(szValue[0]=='D')
	    {
			memset(szPathSub, 0, MAX_PATH_LEN);
			pCurPos = szPath;
			pTempPos = strstr(szPath, "WANConnectionDevice");
			pTempPos += (strlen("WANConnectionDevice")+2);  //point at the back of www.WANConnectionDevice.1.
			pTempPos = strstr(pTempPos,".");
			strncpy(szPathSub, pCurPos, (pTempPos - pCurPos + 1));
			strcat(szPathSub, "WANDSLLinkConfig.ATMEncapsulation"); 
			if (CLI_GetLeaf(szPathSub, szValue) != TBS_SUCCESS)
			{
				return TBS_FAILED;
			}
			sprintf(aszPvcData[CLI_WAN_CONN_ENCAPSULATE], "%s\t%s\n", "Encapsulate", szValue);
	    }
        if (ucPvcType != BRIDGE_CONNECTION)
        {
            GET_LEFT_TO_VAL(szPathSub, "%s%s", szPath, "NATEnabled",szValue );
            if (strcmp(szValue, "1") == 0)
            {
                sprintf(aszPvcData[CLI_WAN_CONN_NAT], "%s\t\t%s\n", "NAT", "enable");
            }
            else
            {
                sprintf(aszPvcData[CLI_WAN_CONN_NAT], "%s\t\t%s\n", "NAT", "disable");
            }
        }

        if ((ucPvcType == PPPOE_CONNECTION) || (ucPvcType == PPPOA_CONNECTION))
        {
			GET_LEFT_TO_VAL(szPathSub,"%s%s",  szPath, "Username", szValue);
            sprintf(aszPvcData[CLI_WAN_CONN_USERNAME], "%s\t%s\n", "ppp user", szValue);

            GET_LEFT_TO_VAL(szPathSub,"%s%s", szPath, "Password", szValue);
            sprintf(aszPvcData[CLI_WAN_CONN_PASSWORD], "%s\t\t%s\n", "password", szValue);

            GET_LEFT_TO_VAL(szPathSub,"%s%s", szPath, "ConnectionTrigger", szValue);
            sprintf(aszPvcData[CLI_WAN_CONN_DIALUP], "%s\t\t%s\n", "dialup", szValue);

            if (strcmp(szValue, "OnDemand") == 0)
            {
                GET_LEFT_TO_VAL(szPathSub,"%s%s", szPath, "IdleDisconnectTime", szValue);      
                sprintf(aszPvcData[CLI_WAN_CONN_PPPTIMEOUT], "%s\t%s\n", "ppp timeout", szValue);
            }

            GET_LEFT_TO_VAL(szPathSub,"%s%s", szPath, "MaxMRUSize", szValue);
            sprintf(aszPvcData[CLI_WAN_CONN_PPPMRU], "%s\t\t%s\n", "ppp mru", szValue);
        }

        if (ucPvcType == PPPOE_CONNECTION)
        {
            GET_LEFT_TO_VAL(szPathSub, "%s%s", szPath, "PPPoEServiceName", szValue);
            sprintf(aszPvcData[CLI_WAN_CONN_SERVICE], "%s\t\t%s\n", "service", szValue);

            GET_LEFT_TO_VAL(szPathSub, "%s%s", szPath, "ConnectionStatus", szValue);
            sprintf(aszPvcData[CLI_WAN_CONN_STATUS], "%s\t\t%s\n", "Status", szValue);	

            GET_LEFT_TO_VAL(szPathSub, "%s%s", szPath, "PPPAuthenticationProtocol", szValue);
            sprintf(aszPvcData[CLI_WAN_CONN_Authentication_Method], "%s\t%s\n", "Authentication", szValue);				
        }

        if ((ucPvcType == MER_CONNECTION) || (ucPvcType == IPOA_CONNECTION))
        {
            GET_LEFT_TO_VAL(szPathSub, "%s%s", szPath, "AddressingType", szValue);
            sprintf(aszPvcData[CLI_WAN_CONN_ADDRESS_TYPE], "%s\t\t%s\n", "IP type", szValue);

            if (strcmp(szValue, "Static") == 0)
            {
                GET_LEFT_TO_VAL(szPathSub, "%s%s", szPath, "ExternalIPAddress", szValue);
                sprintf(aszPvcData[CLI_WAN_CONN_IPADDR], "%s\t%s\n", "IP address", szValue);

                GET_LEFT_TO_VAL(szPathSub, "%s%s", szPath, "SubnetMask", szValue);
                sprintf(aszPvcData[CLI_WAN_CONN_NETMASK], "%s\t\t%s\n", "netmask", szValue);

                GET_LEFT_TO_VAL(szPathSub, "%s%s", szPath, "DefaultGateway", szValue);
                sprintf(aszPvcData[CLI_WAN_CONN_REMOTEIP], "%s\t%s\n", "remote ip", szValue);
            }
        }
        //Print out the information of the wan connection
        for (ulTemp = 0; ulTemp < CLI_WAN_CONN_END; ulTemp++)
        {
            if (aszPvcData[ulTemp] != NULL)
            {
                IO_Print(aszPvcData[ulTemp]);
            }
        }
        
    }
    else  //Show the PVC list
    {
    	char  tmp_wanName[16];
		char  tmp_Path[MAX_PATH_LEN];
		char * pTmpPathPos = NULL;
		char * pTmpPathCurPos = NULL;

		
        pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE);
        if (pstMsg == NULL)
        {
            CLI_WAN_TRACE(("%s: Failed to the node of %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE));
            return TBS_FAILED;
        }

        //Get item count, need to skip the result
        ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
        if (ulCount > 1024)
        {
            CLI_WAN_TRACE(("%s: The items is too big : %d\n", __FUNCTION__, ulCount));
            return TBS_FAILED;
        }

        //Skip the result and item count words
        pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

        IO_Print("\nDSL list:\n");
		IO_Print("%15s%15s%15s%15s%15s\n", "Name", "PVC", "VLanId","Protocol","Status");
        for(; ulTemp < ulCount; ulTemp++)
        {
            pTempPos = strstr(pCurPos, ".Name=");
            if (pTempPos != NULL)
            {
             //   IO_Print("  %s\n", pTempPos + strlen(".Name="));
             	memset(tmp_wanName, 0, sizeof(tmp_wanName));
			 	memset(szPath, 0 , MAX_PATH_LEN);
				memset(szPathSub, 0, MAX_PATH_LEN);
				memset(tmp_Path, 0, MAX_PATH_LEN);
				memset(aszPvcData, 0, CLI_WAN_CONN_END * 256);
				
			 	safe_strncpy( aszPvcData[CLI_WAN_CONN_NAME], pTempPos + strlen(".Name="),256);
				
				if (CLI_GetPathByConnName(aszPvcData[CLI_WAN_CONN_NAME], szPath, MAX_PATH_LEN) != TBS_SUCCESS)
		        {
        		    CLI_WAN_TRACE(("\n%s: Failed to get the path of : %s\n", __FUNCTION__, pszName));
		            return TBS_FAILED;
		        }
				// szPath is InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection/WANPPPConnection
				// back to  InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.
				pTmpPathCurPos = szPath;
				pTmpPathPos = strstr(szPath, "WANConnectionDevice");
				pTmpPathPos += (strlen("WANConnectionDevice")+2);  //point at the back of WANConnectionDevice.1.
				
				pTmpPathPos = strstr(pTmpPathPos,".");
				strncpy(szPathSub, pTmpPathCurPos, (pTmpPathPos - pTmpPathCurPos + 1));

				GET_LEFT_TO_VAL(tmp_Path, "%s%s", szPath, "ConnectionStatus", szValue);
				if( safe_strcmp(szValue, "Connected") != 0)
				{
					sprintf(aszPvcData[CLI_WAN_CONN_STATUS], "%s", "Disconnect");
				}else{	
	            	sprintf(aszPvcData[CLI_WAN_CONN_STATUS], "%s", szValue);
				}

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

				if (strstr(aszPvcData[CLI_WAN_CONN_NAME], "PPPoE") !=NULL)
     		   	{
		            sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "PPPoE");
        		}
		        else if (strstr(aszPvcData[CLI_WAN_CONN_NAME], "PPPoA") != NULL)
        		{
		            sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "PPPoA");
        		}
		        else if (strstr(aszPvcData[CLI_WAN_CONN_NAME], "MER") !=NULL)
        		{
		            sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "MER");
        		}
		        else if (strstr(aszPvcData[CLI_WAN_CONN_NAME], "IPoA") != NULL)
		        {
        		    sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "IPoA");
		        }
        		else if (strstr(aszPvcData[CLI_WAN_CONN_NAME], "Bridging") != NULL)
		        {
        		    sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "Bridge");
		        }else{
		        	sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "unknown");
		        }
//				IO_Print("%15s%15s%15s%15s%15s\n", "Name", "PVC", "VLanId","Protocol","Status");	
				IO_Print("%15s%15s%15s%15s%15s\n", aszPvcData[CLI_WAN_CONN_NAME], 
												  aszPvcData[CLI_WAN_CONN_PVC], 
												  aszPvcData[CLI_WAN_CONN_VLANID],
												  aszPvcData[CLI_WAN_CONN_PROTOCOL],
												  aszPvcData[CLI_WAN_CONN_STATUS]);	
            }

            pCurPos += (strlen(pCurPos) + 1);
        }

#ifdef CONFIG_APPS_LOGIC_ETHWAN

		ulTemp = 0;
		ulCount = 0;
		
		pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE_ETH);
        if (pstMsg == NULL)
        {
            CLI_WAN_TRACE(("%s: Failed to the node of %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE_ETH));
            return TBS_FAILED;
        }

        //Get item count, need to skip the result
        ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
        if (ulCount > 1024)
        {
            CLI_WAN_TRACE(("%s: The items is too big : %d\n", __FUNCTION__, ulCount));
            return TBS_FAILED;
        }

        //Skip the result and item count words
        pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

        IO_Print("\nEthernet list:\n");
		IO_Print("%15s%15s%15s%15s\n", "Name", "VLanId","Protocol","Status");
        for(; ulTemp < ulCount; ulTemp++)
        {
            pTempPos = strstr(pCurPos, ".Name=");
            if (pTempPos != NULL)
            {
             //   IO_Print("  %s\n", pTempPos + strlen(".Name="));
             	memset(tmp_wanName, 0, sizeof(tmp_wanName));
			 	memset(szPath, 0 , MAX_PATH_LEN);
				memset(szPathSub, 0, MAX_PATH_LEN);
				memset(tmp_Path, 0, MAX_PATH_LEN);
				memset(aszPvcData, 0, CLI_WAN_CONN_END * 256);
				
			 	safe_strncpy( aszPvcData[CLI_WAN_CONN_NAME], pTempPos + strlen(".Name="),256);
				
				if (CLI_GetPathByConnName(aszPvcData[CLI_WAN_CONN_NAME], szPath, MAX_PATH_LEN) != TBS_SUCCESS)
		        {
        		    CLI_WAN_TRACE(("\n%s: Failed to get the path of : %s\n", __FUNCTION__, pszName));
		            return TBS_FAILED;
		        }
				// szPath is InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection/WANPPPConnection
				// back to  InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.
				pTmpPathCurPos = szPath;
				pTmpPathPos = strstr(szPath, "WANConnectionDevice");
				pTmpPathPos += (strlen("WANConnectionDevice")+2);  //point at the back of WANConnectionDevice.1.
				
				pTmpPathPos = strstr(pTmpPathPos,".");
				strncpy(szPathSub, pTmpPathCurPos, (pTmpPathPos - pTmpPathCurPos + 1));

				GET_LEFT_TO_VAL(tmp_Path, "%s%s", szPath, "ConnectionStatus", szValue);
				if( safe_strcmp(szValue, "Connected") != 0)
				{
					sprintf(aszPvcData[CLI_WAN_CONN_STATUS], "%s", "Disconnect");
				}else{	
	            	sprintf(aszPvcData[CLI_WAN_CONN_STATUS], "%s", szValue);
				}


				GET_LEFT_TO_VAL(tmp_Path, "%s%s", szPathSub, "X_TWSZ-COM_VLANID", szValue);
				sprintf(aszPvcData[CLI_WAN_CONN_VLANID], "%s", szValue);
				

				if (strstr(aszPvcData[CLI_WAN_CONN_NAME], "PPPoE") !=NULL)
     		   	{
		            sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "PPPoE");
        		}
		        else if (strstr(aszPvcData[CLI_WAN_CONN_NAME], "PPPoA") != NULL)
        		{
		            sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "PPPoA");
        		}
		        else if (strstr(aszPvcData[CLI_WAN_CONN_NAME], "MER") !=NULL)
        		{
		            sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "MER");
        		}
		        else if (strstr(aszPvcData[CLI_WAN_CONN_NAME], "IPoA") != NULL)
		        {
        		    sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "IPoA");
		        }
        		else if (strstr(aszPvcData[CLI_WAN_CONN_NAME], "Bridging") != NULL)
		        {
        		    sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "Bridge");
		        }else{
		        	sprintf(aszPvcData[CLI_WAN_CONN_PROTOCOL], "%s", "unknown");
		        }
//				IO_Print("%15s%15s%15s%15s%15s\n", "Name", "PVC", "VLanId","Protocol","Status");	
				IO_Print("%15s%15s%15s%15s\n", aszPvcData[CLI_WAN_CONN_NAME], 
												  aszPvcData[CLI_WAN_CONN_VLANID],
												  aszPvcData[CLI_WAN_CONN_PROTOCOL],
												  aszPvcData[CLI_WAN_CONN_STATUS]);	
            }

            pCurPos += (strlen(pCurPos) + 1);
        }
#endif		
    }

    return TBS_SUCCESS;
}
/*************************************************************************
function  : set pppoe username and password
parameters: none
return    : TBS_SUCCESS if success
            TBS_FAILED if not success
remark    : Use CLI command such as:
            wan ppp get  PPPoE_33_1
*************************************************************************/
ULONG CLI_PPP_SET()
{

    ULONG ulCount  = 0;
    char *pszName    = NULL;
    char *pszuser    = NULL;
    char *pszpass    = NULL;
    char szPath[MAX_PATH_LEN] = {0};
    ULONG ulVpi        = 0;
    ULONG ulVci        = 32;
    char aszPath[4][CLI_NODE__LEN];  //Increase this range if more items will be set
    char aszValue[4][CLI_NODE__LEN];
    unsigned long wanindex1,wanindex2,wanindex3=0;
    int flag =0;
 
    if (CLI_GetParamByName("ppp") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszName, "pvc");
    }
    if (pszName != NULL)  //Show the detail of the specific PVC
    {
        char aszPvcData[CLI_PPP_CONN_END][256];
        ULONG ulTempSub = 0;

        IO_Print("\n");
        
        //Initialize the array
        for(; ulTempSub < CLI_PPP_CONN_END; ulTempSub++)
        {
            memset(aszPvcData[ulTempSub], 0, 256);
        }
  
         if (CLI_GetPathByConnName(pszName, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
        {
            IO_Print("\npvc %s error\n", pszName);
            return TBS_SUCCESS;
        }
        if(strstr(szPath,"WANPPPConnection") ==NULL)
        {
            IO_Print("\n Error ,pvc %s is not a pppoe or pppoa\n", pszName);
            return TBS_SUCCESS;
        }
       
        if (CLI_GetParamByName("username") != NULL)
        {
            CLI_GET_PARAMETER_ASSERT(pszuser, "username-value");
            sprintf(aszPath[ulCount],"%s%s",szPath,"Username");
            sprintf(aszValue[ulCount],"%s",pszuser);
            ulCount++;
        }
        if (CLI_GetParamByName("password") != NULL)
        {
            CLI_GET_PARAMETER_ASSERT(pszpass, "password-value");
            sprintf(aszPath[ulCount],"%s%s",szPath,"Password");
            sprintf(aszValue[ulCount],"%s",pszpass);
            ulCount++;
        }

         if (CLI_GetParamByName("vpi") != NULL)
        {
            CLI_GetUlongByName("vpi-v", &ulVpi);
            flag =1;
        }

        if (CLI_GetParamByName("vci") != NULL)
        {
            CLI_GetUlongByName("vci-v", &ulVci);
            flag =1;
        }
        if(flag == 1)
        {
            sscanf(szPath,"InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANPPPConnection.%lu.",&wanindex1,&wanindex2,&wanindex3);
            sprintf(aszPath[ulCount],"InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANDSLLinkConfig.DestinationAddress",wanindex1,wanindex2);
            sprintf(aszValue[ulCount], "PVC:%lu/%lu", ulVpi, ulVci);
              ulCount++;
        }
        if (CLI_SetLeaf(ulCount, aszPath, aszValue) != TBS_SUCCESS)
      {
            return TBS_FAILED;
      }
      }
    return TBS_SUCCESS;
}

/*************************************************************************
function  : get pppoe information
parameters: none
return    : TBS_SUCCESS if success
            TBS_FAILED if not success
remark    : Use CLI command such as:
            wan ppp get  PPPoE_33_1
*************************************************************************/
ULONG CLI_PPP_GET()
{
    char *pCurPos  = NULL;
    char *pTempPos = NULL;
    ULONG ulTemp   = 0;

    char *pszName    = NULL;
    char szPath[MAX_PATH_LEN] = {0};
    char szPathSub[256] = {0};
    char szValue[256] = {0};
    if (CLI_GetParamByName("ppp") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszName, "pvc");
    }
    if (pszName != NULL)  //Show the detail of the specific PVC
    {
        char aszPvcData[CLI_PPP_CONN_END][256];
        ULONG ulTempSub = 0;

        IO_Print("\n");
        
        //Initialize the array
        for(; ulTempSub < CLI_PPP_CONN_END; ulTempSub++)
        {
            memset(aszPvcData[ulTempSub], 0, 256);
        }
  
        if (CLI_GetPathByConnName(pszName, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
        {
            IO_Print("\npvc %s error\n", pszName);
            return TBS_SUCCESS;
        }
        if(strstr(szPath,"WANPPPConnection") ==NULL)
        {
             IO_Print("\n Error ,pvc %s is not a pppoe or pppoa\n", pszName);
             return TBS_SUCCESS;
        }
  
        //Get the leaf of the wan connection
        sprintf(szPathSub, "%s%s", szPath, "Name");
        if (CLI_GetLeaf(szPathSub, szValue) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
        sprintf(aszPvcData[CLI_PPP_CONN_NAME], "%s\t\t\t%s\n", "Name", szValue);
 
        memset(szPathSub, 0, 256);
        pCurPos = szPath;
        pTempPos = strstr(szPath, "WANConnectionDevice");
        pTempPos += (strlen("WANConnectionDevice") + 2);  //point at the back of www.WANConnectionDevice.1.
        pTempPos = strstr(pTempPos,".");
        strncpy(szPathSub, pCurPos, (pTempPos - pCurPos + 1));
        strcat(szPathSub, "X_TWSZ-COM_DSLType");
        if (CLI_GetLeaf(szPathSub, szValue) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
        sprintf(aszPvcData[CLI_PPP_CONN_PVC], "%s\t\t%s\n", "TYPE", strlen(szValue)==0? "-":szValue);
        if(pszName[0] == 'D')
        {
        memset(szPathSub, 0, 256);
        pCurPos = szPath;
        pTempPos = strstr(szPath, "WANConnectionDevice");
        pTempPos += (strlen("WANConnectionDevice") + 2);  //point at the back of www.WANConnectionDevice.1.
        strncpy(szPathSub, pCurPos, (pTempPos - pCurPos + 1));
        strcat(szPathSub, "WANDSLLinkConfig.");  //Make it like as www.WANConnectionDevice.1.WANDSLLinkConfig.
        strcat(szPathSub, "DestinationAddress");
        if (CLI_GetLeaf(szPathSub, szValue) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
        sprintf(aszPvcData[CLI_PPP_CONN_PVC], "%s\t\t\t%s\n", "PVC", strlen(szValue)==0? "-":szValue);

        memset(szPathSub, 0, 256);
        pCurPos = szPath;
        pTempPos = strstr(szPath, "WANConnectionDevice");
        pTempPos += (strlen("WANConnectionDevice") + 2);  //point at the back of www.WANConnectionDevice.1.
        strncpy(szPathSub, pCurPos, (pTempPos - pCurPos + 1));
        strcat(szPathSub, "WANDSLLinkConfig.");  //Make it like as www.WANConnectionDevice.1.WANDSLLinkConfig.
        strcat(szPathSub, "ATMEncapsulation");
        if (CLI_GetLeaf(szPathSub, szValue) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
        sprintf(aszPvcData[CLI_PPP_CONN_ENCAP], "Encapsulation Modet\t%s\n", szValue);

        memset(szPathSub, 0, 256);
        pCurPos = szPath;
        pTempPos = strstr(szPath, "WANConnectionDevice");
        pTempPos += (strlen("WANConnectionDevice") + 2);  //point at the back of www.WANConnectionDevice.1.
        strncpy(szPathSub, pCurPos, (pTempPos - pCurPos + 1));
        strcat(szPathSub, "WANDSLLinkConfig.");  //Make it like as www.WANConnectionDevice.1.WANDSLLinkConfig.
        strcat(szPathSub, "ATMQoS");
        if (CLI_GetLeaf(szPathSub, szValue) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
        if(strcmp(szValue,"UBR+")==0)
            sprintf(aszPvcData[CLI_PPP_CONN_ATMQOS], "Service Categoryt\t%s\n", "UBR Without PCR");
        else if(strcmp(szValue,"UBR")==0)
            sprintf(aszPvcData[CLI_PPP_CONN_ATMQOS], "Service Categoryt\t%s\n", "UBR With PCR");
        else if(strcmp(szValue,"VBR-nrt")==0)
            sprintf(aszPvcData[CLI_PPP_CONN_ATMQOS], "Service Categoryt\t%s\n", "CBR");
        else if(strcmp(szValue,"VBR-nrt")==0)
            sprintf(aszPvcData[CLI_PPP_CONN_ATMQOS], "Service Categoryt\t%s\n", "Non Realtime VB");
        else 
            sprintf(aszPvcData[CLI_PPP_CONN_ATMQOS], "Service Categoryt\t%s\n", "Realtime VBR");
        }
        sprintf(szPathSub, "%s%s", szPath, "Username");
        if (CLI_GetLeaf(szPathSub, szValue) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
        sprintf(aszPvcData[CLI_PPP_CONN_USERNAME], "%s\t\t%s\n", "Username", szValue);

        sprintf(szPathSub, "%s%s", szPath, "Password");
        if (CLI_GetLeaf(szPathSub, szValue) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
        sprintf(aszPvcData[CLI_PPP_CONN_PASSWORD], "%s\t\t%s\n", "Password", "****");
    /*
           sprintf(szPathSub, "%s%s", szPath, "ConnectionStatus");
            if (CLI_GetLeaf(szPathSub, szValue) != TBS_SUCCESS)
            {
                return TBS_FAILED;
            }
            sprintf(aszPvcData[CLI_WAN_CONN_STATUS], "%s\t\t%s\n", "Status", szValue);	
*/
     
        //Print out the information of the wan connection
        for (ulTemp = 0; ulTemp < CLI_PPP_CONN_END; ulTemp++)
        {
            if (aszPvcData[ulTemp] != NULL)
            {
                IO_Print(aszPvcData[ulTemp]);
            }
        }
        IO_Print("Protocol  \t\tPPPoE\n");
        
    }


    return TBS_SUCCESS;
}

/*************************************************************************
function  : Check if exist PVC is same as specific PVC
parameters: ulVpi : VPI of specific PVC
            ulVci : VCI of specific PVC
return    : TRUE if found
            FALSE if not found
remark    : FALSE is not represented to error
*************************************************************************/
ULONG CLI_CheckWanInterfaceExist(ULONG ulVpi, ULONG ulVci)
{
    char szPvc[16]            = {0};
    char szValue[MAX_PATH_LEN] = {0};
    ST_MSG *pstMsg            = NULL;
    ULONG ulCount             = 0;
    ULONG ulTemp              = 0;
    char *pCurPos             = NULL;
    char *pTempPos            = NULL;

    //Send message to get parameters of the node
    pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE);
    if (pstMsg == NULL)
    {
        CLI_WAN_TRACE(("%s: Failed to the node of %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE));
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_WAN_TRACE(("%s: The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    memset(szPvc, 0, sizeof(szPvc));
    sprintf(szPvc, "%lu/%lu", ulVpi, ulVci);
    for(; ulTemp < ulCount; ulTemp++)
    {
        //Check if the item is PVC value
        pTempPos = strstr(pCurPos, ".Name=");
        if (pTempPos != NULL)
        {
            strcpy(szValue, pTempPos + strlen(".Name=PVC:"));
            if (strcmp(szPvc, szValue) == 0)
            {
                return TRUE;
            }
        }

        pCurPos += (strlen(pCurPos) + 1);
    }

    return FALSE;
}

ULONG CLI_CheckWanInterfaceExist2(ULONG ulVpi, ULONG ulVci,char *pszprotel)
{
    char szPvc[16]            = {0};
    char szValue[MAX_PATH_LEN] = {0};
    char szValue2[MAX_PATH_LEN] = {0};
    ST_MSG *pstMsg            = NULL;
    ULONG ulCount             = 0;
    ULONG ulTemp              = 0;
    ULONG ulPvcFind              = 0;
    char *pCurPos             = NULL;
    char *pTempPos            = NULL;
    char *pProPos             = NULL;
    //char *pTempPos2            = NULL;
    ULONG ulFlag             = 0;

    pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE);
    if (pstMsg == NULL)
    {
        CLI_WAN_TRACE(("%s: Failed to the node of %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE));
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_WAN_TRACE(("%s: The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);
    memset(szPvc, 0, sizeof(szPvc));
    sprintf(szPvc, "%lu/%lu", ulVpi, ulVci);

    for(; ulTemp < ulCount; ulTemp++)
    {
        //Check if the item is PVC value
        pProPos = strstr(pCurPos, ".ConnectionType=");
        pTempPos = strstr(pCurPos, ".Name=");
        //pTempPos2 = strstr(pCurPos, ".Name=");
        if (pTempPos != NULL )
        {
            strcpy(szValue, pTempPos + strlen(".Name=PVC:"));
            if (strcmp(szPvc, szValue) == 0 )
            {  
                ulPvcFind=1;
                ulFlag++;
                //only can surport two connection with the same PVC one is for bridge or mer,another is for pppoe
                if(ulFlag >=3)
                {
                    return TRUE;
                }
            }
        }
            //get the connection name 
        if(pProPos != NULL)
        {
            if (ulPvcFind == 1 )
            {   
                if((strcmp(pszprotel, "bridge") == 0) || (strcmp(pszprotel, "mer") == 0)) 
    			{
                    strcpy(szValue2, pProPos + strlen(".ConnectionType="));
                    if(strcmp(pszprotel, "bridge") == 0)
                    {
                        if((strcmp(szValue2, "IP_Bridged") == 0) && (strstr(pCurPos, "WANIPConnection") != NULL))
                        {
                            return TRUE;
                        }
                    }
                    else if(strcmp(pszprotel, "mer") == 0)
                    {
                        if((strcmp(szValue2, "IP_Routed") == 0) && (strstr(pCurPos, "WANIPConnection") != NULL))
                        {
                            return TRUE;
                        }
                    }
            	}
                else
    		  	{
                    if(strstr(pCurPos, "WANPPPConnection") != NULL ) 
                    {      
                        return TRUE;
                    }
    		  	}					
            }            
        }
		#if 0
        if ( pTempPos2 != NULL)
        {

        }
        #endif
        pCurPos += (strlen(pCurPos) + 1);
        if(strstr(pCurPos, "X_TWSZ-COM_VLANPriority=") != NULL ) 
            ulPvcFind = 0;
    }
    return FALSE;
}


/*************************************************************************
function  : Get the path through wan interface name, such as:
            InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.
parameters: pszConnName: the name of the wan interface
            pszPath    : output the path of the wan interface
            iLen       : the max length of the pszPath
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : Do not try to release the pstMsg here, because it will point to the global
            variable :g_pstCliMsg, cli_comm.c will do the release.
*************************************************************************/
static ULONG CLI_GetPathByConnName(char *pszConnName, char *pszPath, int iLen)
{
    ST_MSG *pstMsg = NULL;
    ULONG  ulCount = 0;
    ULONG  ulTemp  = 0;
    ULONG  ulIndex1= 0;
    ULONG  ulIndex2= 0;
    char *pCurPos  = NULL;
    char szName[MAX_NODE_VALUE_LEN] = {0};
    char szPath[]  = CLI_WAN_DEVICE_BASE;

    if ((pszConnName == NULL)
        || (pszPath == NULL)
        || (iLen < MAX_PATH_LEN))
    {
        return TBS_FAILED;
    }

    //Send GET message to get all the leaves of the node which szPath specified
    pstMsg = CLI_GetNode(szPath);
    if (pstMsg == NULL)
    {
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_WAN_TRACE(("%s :The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);
    memset(szName, 0, MAX_NODE_VALUE_LEN);
    
    for (ulTemp = 0; ulTemp < ulCount; ulTemp++)
    {
        /*
         *Just search ".WANIPConnection." and ".WANPPPConnection." in the path
         */
         
        if (strstr(pCurPos, "."CLI_WAN_IP_CONNECTION".") != NULL)
        {
            memset(szName, 0, sizeof(szName));
            sscanf(pCurPos, CLI_WAN_IP_CONNECTION_NAME, &ulIndex1, &ulIndex2, szName);
            if (strcmp(szName, pszConnName) == 0)
            {
                //found
                char *pPos = strstr(pCurPos, ".Name=");
                /*
                 *Skip the beginning of "string ", it's 7 bytes
                 *The struct of data which CMM returned is : string xxx.xxx.xxx.{i}.xxx=xxx
                 */
                strncpy(pszPath, pCurPos + 7, (pPos - pCurPos - 7 + 1));

                return TBS_SUCCESS;
            }
        }
        else if (strstr(pCurPos, "."CLI_WAN_PPP_CONNECTION".") != NULL)
        {
            memset(szName, 0, sizeof(szName));
            sscanf(pCurPos, CLI_WAN_PPP_CONNECTION_NAME, &ulIndex1, &ulIndex2, szName);
            if (strcmp(szName, pszConnName) == 0)
            {
                //found
                char *pPos = strstr(pCurPos, ".Name=");
                /*
                 *Skip the beginning of "string ", it's 7 bytes
                 *The struct of data which CMM returned is : string xxx.xxx.xxx.{i}.xxx=xxx
                 */
                strncpy(pszPath, pCurPos + 7, (pPos - pCurPos - 7 + 1));

                return TBS_SUCCESS;
            }
        }

        //指向下一个item
        if (pCurPos)
            pCurPos += (strlen(pCurPos) + 1);
            
    }
 //Send GET message to get all the leaves of the node which szPath specified
    memset(szPath,0,sizeof(szPath));
    strcpy(szPath,CLI_WAN_DEVICE_BASE_ETH);
    pstMsg = CLI_GetNode(szPath);
    if (pstMsg == NULL)
    {
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_WAN_TRACE(("%s :The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);
    memset(szName, 0, MAX_NODE_VALUE_LEN);
    
    for (ulTemp = 0; ulTemp < ulCount; ulTemp++)
    {
        /*
         *Just search ".WANIPConnection." and ".WANPPPConnection." in the path
         */
         
        if (strstr(pCurPos, "."CLI_WAN_IP_CONNECTION".") != NULL)
        {
            memset(szName, 0, sizeof(szName));
            sscanf(pCurPos, CLI_WAN_IP_CONNECTION_NAME_ETH, &ulIndex1, &ulIndex2, szName);
            if (strcmp(szName, pszConnName) == 0)
            {
                //found
                char *pPos = strstr(pCurPos, ".Name=");
                /*
                 *Skip the beginning of "string ", it's 7 bytes
                 *The struct of data which CMM returned is : string xxx.xxx.xxx.{i}.xxx=xxx
                 */
                strncpy(pszPath, pCurPos + 7, (pPos - pCurPos - 7 + 1));

                return TBS_SUCCESS;
            }
        }
        else if (strstr(pCurPos, "."CLI_WAN_PPP_CONNECTION".") != NULL)
        {
            memset(szName, 0, sizeof(szName));
            sscanf(pCurPos, CLI_WAN_PPP_CONNECTION_NAME_ETH, &ulIndex1, &ulIndex2, szName);
            if (strcmp(szName, pszConnName) == 0)
            {
                //found
                char *pPos = strstr(pCurPos, ".Name=");
                /*
                 *Skip the beginning of "string ", it's 7 bytes
                 *The struct of data which CMM returned is : string xxx.xxx.xxx.{i}.xxx=xxx
                 */
                strncpy(pszPath, pCurPos + 7, (pPos - pCurPos - 7 + 1));

                return TBS_SUCCESS;
            }
        }

        //指向下一个item
        if (pCurPos)
            pCurPos += (strlen(pCurPos) + 1);
            
    }

    IO_Print("\r\nconnection name:%s not exist!\n", pszConnName);
    return TBS_FAILED;
}


/*************************************************************************
function  : Search WANConnectionDevice node to get the total pvc number
parameters: *pNum : return the exist pvc number
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : This function may not be necessary
*************************************************************************/
ULONG CLI_SearchTotalNumberOfPvc(ULONG *pNum)
{
    ST_MSG *pstMsg   = NULL;
    char *pCurPos    = NULL;
    ULONG ulCount    = 0;
    ULONG ulTemp     = 0;

    pstMsg = CLI_GetNode(CLI_WAN_DEVICE_BASE);
    if (pstMsg == NULL)
    {
        CLI_WAN_TRACE(("\n%s: Failed to get node of: %s\n", __FUNCTION__, CLI_WAN_DEVICE_BASE));
        return TBS_FAILED;
    }

    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_WAN_TRACE(("\n%s: The items is too big: %lu\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    for(; ulTemp < ulCount; ulTemp++)
    {
        if (strstr(pCurPos, "DestinationAddress") != NULL)
        {
            (*pNum)++;
        }
        pCurPos += (strlen(pCurPos) + 1);
    }

    CLI_WAN_TRACE(("%s: total pvc number: %lu\n", __FUNCTION__, *pNum));
    return TBS_SUCCESS;
}
/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif
