/**********************************************************************
 Copyright (c), 1991-2010, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 File Name: cli_route.c
 File description: Route function for CLI
 Modify history:
        1. Author: Tangqiang
           Date: 2010-01-07
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
#define CLI_ROUTE_TRACE(x)  //IO_Print x

#define CLI_WAN_IP_CONNECTION_NAME      "string InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANIPConnection.%lu.Name=%s"
#define CLI_WAN_PPP_CONNECTION_NAME     "string InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANPPPConnection.%lu.Name=%s"
#define CLI_WAN_DEVICE_BASE             "InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice."
#define CLI_WAN_IP_CONNECTION           "WANIPConnection"
#define CLI_WAN_PPP_CONNECTION          "WANPPPConnection"
#define CLI_ROUTE_STATIC_BASE           "InternetGatewayDevice.Layer3Forwarding.Forwarding."

//This macro is used to get the string value
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


/******************************************************************************
 *                               GLOBAL VAR                                   *
 ******************************************************************************/


/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/
ULONG CLI_Route_Dynamic();
ULONG CLI_Route_Static();
ULONG CLI_Route_Show();

ULONG CLI_Route_Static_Add();
ULONG CLI_Route_Static_Del();
static ULONG CLI_GetPathByConnName(char *pszConnName, char *pszPath, int iLen);

//Main function
ULONG  CLI_Route()
{
    if (CLI_GetParamByName("dynamic") != NULL)
    {
        return CLI_Route_Dynamic();
    }
    else if (CLI_GetParamByName("static") != NULL)
    {
        return CLI_Route_Static();
    }
    else if (CLI_GetParamByName("show") != NULL)
    {
        return CLI_Route_Show();
    }

    return TBS_FAILED;
}


/*************************************************************
function  : Set the RIP feature on wan interface(connection)
parameters: none
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : use cli command such as:
            route dynamic set pppoe_0_32 RIPv1
*************************************************************/
ULONG CLI_Route_Dynamic()
{
    char *pszWanInterface = NULL;
    char *pszRipVersion   = NULL;
    char szPath[MAX_PATH_LEN] = {0};
    char aszPath[1][CLI_LEAF__LEN];
    char aszValue[1][CLI_LEAF__LEN];

    if (CLI_GetParamByName("set") != NULL)
    {
        //Check if get parameter of wan interface
        CLI_GET_PARAMETER_ASSERT(pszWanInterface, "wan");

        //Check if get parameter of rip version
        CLI_GET_PARAMETER_ASSERT(pszRipVersion, "rip-version");

        //Get the path of the wan interface node
        if (CLI_GetPathByConnName(pszWanInterface, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
        {
            return TBS_FAILED;
        }
        CLI_ROUTE_TRACE(("%s: Get the node path : %s\n", __FUNCTION__, szPath));

        //Fill the leaf of RIP
        sprintf(aszPath[0], "%sRouteProtocolRx", szPath);
        sprintf(aszValue[0], "%s", pszRipVersion);

        return CLI_SetLeaf(1, aszPath, aszValue);
    }

    return TBS_FAILED;
}

/*************************************************************
function  : Config the static route feature
parameters: none
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : 
*************************************************************/
ULONG CLI_Route_Static()
{
    if (CLI_GetParamByName("add") != NULL)
    {
        return CLI_Route_Static_Add();
    }
    else if (CLI_GetParamByName("del") != NULL)
    {
        return CLI_Route_Static_Del();
    }

    return TBS_FAILED;
}

/*************************************************************
function  : Add a static route rule
parameters: none
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : use cli command such as:
            route static add 10.10.0.0 255.255.0.0 10.10.10.1 pppoe_0_32
*************************************************************/
ULONG CLI_Route_Static_Add()
{
    ULONG ulInstIndex     = 0;
    char *pszDestNetwork  = NULL;
    char *pszSubnet       = NULL;
    char *pszGatewayIp    = NULL;
    char *pszWanInterface = NULL;
    ULONG ulForwardMetric = 0;
    BOOL  bEnable         = 0;

    char szPath[MAX_PATH_LEN] = {0};
    char aszPath[6][CLI_LEAF__LEN];
    char aszValue[6][CLI_LEAF__LEN];
    char *pTempPos = NULL;

    //Check if get the specific parameters
    CLI_GET_PARAMETER_ASSERT(pszDestNetwork, "dest-network");
    CLI_GET_PARAMETER_ASSERT(pszSubnet, "subnet");
    CLI_GET_PARAMETER_ASSERT(pszGatewayIp, "gateway-ip");
    CLI_GET_PARAMETER_ASSERT(pszWanInterface, "wan");

    //Get the path of the wan interface node
    if (CLI_GetPathByConnName(pszWanInterface, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    pTempPos = szPath + strlen(szPath) - 1;
    *pTempPos = '\0';   //The path is end with '.', remove the last '.'
    CLI_ROUTE_TRACE(("\n%s: Get the node path : %s\n", __FUNCTION__, szPath));

    if (CLI_AddObjectEx(CLI_ROUTE_STATIC_BASE, &ulInstIndex) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    CLI_ROUTE_TRACE(("%s Add new static route node %ul\n", __FUNCTION__, ulInstIndex));

    //Fill the leaf of static route node
    sprintf(aszPath[0], "%s%lu.DestIPAddress", CLI_ROUTE_STATIC_BASE, ulInstIndex);
    sprintf(aszValue[0], "%s", pszDestNetwork);

    sprintf(aszPath[1], "%s%lu.DestSubnetMask", CLI_ROUTE_STATIC_BASE, ulInstIndex);
    sprintf(aszValue[1], "%s", pszSubnet);

    sprintf(aszPath[2], "%s%lu.GatewayIPAddress", CLI_ROUTE_STATIC_BASE, ulInstIndex);
    sprintf(aszValue[2], "%s", pszGatewayIp);

    sprintf(aszPath[3], "%s%lu.Interface", CLI_ROUTE_STATIC_BASE, ulInstIndex);
    sprintf(aszValue[3], "%s", szPath);

    ulForwardMetric = 16;
    sprintf(aszPath[4], "%s%lu.ForwardingMetric", CLI_ROUTE_STATIC_BASE, ulInstIndex);
    sprintf(aszValue[4], "%lu", ulForwardMetric);

    bEnable = 1;
    sprintf(aszPath[5], "%s%lu.Enable", CLI_ROUTE_STATIC_BASE, ulInstIndex);
    sprintf(aszValue[5], "%d", bEnable);

    if (CLI_SetLeaf(6, aszPath, aszValue) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

/*************************************************************
function  : Del an exist static route rule
parameters: none
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : use cli command such as:
            route static del 10.10.0.0 255.255.0.0
*************************************************************/
ULONG CLI_Route_Static_Del()
{
    char *pszDestNetwork = NULL;
    char *pszSubnet      = NULL;
    ST_MSG *pstMsg       = NULL;
    ULONG ulCount        = 0;
    ULONG ulTemp         = 0;
    char *pCurPos        = NULL;
    char *pTempPos       = NULL;
    char szPath[MAX_PATH_LEN] = {0};
    UCHAR ucDestNetworkFound  = 0;
    UCHAR ucSubnetFound       = 0;

    //Check if get the specific parameters
    CLI_GET_PARAMETER_ASSERT(pszDestNetwork, "dest-network");
    CLI_GET_PARAMETER_ASSERT(pszSubnet, "subnet");

    //Send GET message to get all the leaves of the node which szPath specified
    pstMsg = CLI_GetNode(CLI_ROUTE_STATIC_BASE);
    if (pstMsg == NULL)
    {
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody+sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_ROUTE_TRACE(("%s :The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    for (; ulTemp < ulCount; ulTemp++)
    {
        if ((pTempPos = strstr(pCurPos, ".DestSubnetMask=")) != NULL)
        {
            if (strcmp(pszSubnet, pTempPos + strlen(".DestSubnetMask=")) == 0)
            {
                ucSubnetFound = 1;
            }
        }
        else if ((pTempPos = strstr(pCurPos, ".DestIPAddress=")) != NULL)
        {
            if (strcmp(pszDestNetwork, pTempPos + strlen(".DestIPAddress=")) == 0)
            {
                ucDestNetworkFound = 1;
            }
        }

        if ((ucSubnetFound == 1) && (ucDestNetworkFound == 1))
        {
            /*
             *Skip the beginning of "string ", it's 7 bytes
             *The struct of data which CMM returned is : string xxx.xxx.xxx.{i}.xxx=xxx
             */
            strncpy(szPath, pCurPos + 7, (pTempPos - pCurPos - 7 + 1));
            CLI_ROUTE_TRACE(("%s: Delete the node: %s\n", __FUNCTION__, szPath));
            return CLI_DelObject(szPath);
        }

        pCurPos += (strlen(pCurPos) + 1);
    }

    CLI_ROUTE_TRACE(("%s: Can not find the static route\n", __FUNCTION__));
    return TBS_FAILED;
}

/*************************************************************
function  : Print the current dynamic and static route stat
parameters: none
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : use cli command such as:
            route show
*************************************************************/
ULONG CLI_Route_Show()
{
    char szWanInterface[256] = {0};
    char szRipVersion[8]     = {0};
    char szDestNetwork[32]   = {0};
    char szDestSubnet[32]    = {0};
    char szGatewayIp[32]     = {0};
    char szPath[MAX_PATH_LEN]= {0};

    ST_MSG *pstMsg = NULL;
    ULONG ulCount  = 0;
    ULONG ulTemp   = 0;
    char *pCurPos  = NULL;
    char *pTempPos = NULL;
    ULONG  ulWanDevIndex = 1;

    while(1)
    {
        memset(szPath, 0 , MAX_PATH_LEN);
        sprintf(szPath, CLI_WAN_DEVICE_BASE, ulWanDevIndex);

        //Send GET message to get all the leaves of the node which szPath specified
        pstMsg = CLI_GetNode(szPath);
        if (pstMsg == NULL)
        {
            return TBS_FAILED;
        }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody+sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_ROUTE_TRACE(("%s :The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);
    
    //Print the dynamic route table
    IO_Print("\nDynamic Route:\n  Interface\t\tRIP version\n");

    for (ulTemp = 0; ulTemp < ulCount; ulTemp++)
    {
        pTempPos = strstr(pCurPos, ".RouteProtocolRx=");
        if (pTempPos != NULL)  //Found the interface
        {
            memset(szRipVersion, 0, sizeof(szRipVersion));
            strcpy(szRipVersion, pTempPos + strlen(".RouteProtocolRx="));
            if (strlen(szRipVersion) <= 0)
            {
                //RouteProtocolRx may be empty if RIP is disabled
                strcpy(szRipVersion, "Off");
            }
            
            pCurPos += (strlen(pCurPos) + 1);
            ulTemp++;
            for (; ulTemp < ulCount; ulTemp++)
            {
                pTempPos = strstr(pCurPos, ".Name=");
                if (pTempPos != NULL)  //Found the wan interface name
                {
                    memset(szWanInterface, 0, sizeof(szWanInterface));
                    strcpy(szWanInterface, pTempPos + strlen(".Name="));

                    //If it is routed PVC, print out
                    if ((strncmp(szWanInterface, "ppp", 3) == 0) || 
                        (strncmp(szWanInterface, "mer", 3) == 0) || 
                        (strncmp(szWanInterface, "ipo", 3) == 0))
                    {
                        IO_Print("  %s\t\t%s\n", szWanInterface, szRipVersion);
                    }
                    break;
                }

                pCurPos += (strlen(pCurPos) + 1);
            } //End of for loop
        }

            pCurPos += (strlen(pCurPos) + 1);
        } //End of for loop
        ulWanDevIndex++;
    }

    IO_Print("\n"); //End of print


    //Send GET message to get all the leaves of the node which szPath specified
    pstMsg = CLI_GetNode(CLI_ROUTE_STATIC_BASE);
    if (pstMsg == NULL)
    {
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody+sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_ROUTE_TRACE(("%s :The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    //Print the static route table
    IO_Print("Static Route:\n  Dest Network\tDest Subnet\tGateway IP\tWan Interface\n");

    for (ulTemp = 0; ulTemp < ulCount; ulTemp++)
    {
        pTempPos = strstr(pCurPos, ".Interface=");
        if (pTempPos != NULL)
        {
            memset(szPath, 0, sizeof(szPath));
            strcpy(szPath, pTempPos + strlen(".Interface="));

            //If the path is not end with '.', add the '.'
            pTempPos = szPath + strlen(szPath) - 1;
            if (*pTempPos != '.')
            {
                pTempPos++;
                *pTempPos = '.';
            }

            strcat(szPath, "Name.");
            if (CLI_GetLeaf(szPath, szWanInterface) != TBS_SUCCESS)
            {
                CLI_ROUTE_TRACE(("%s: Failed to the name of %s\n", __FUNCTION__, szPath));
                return TBS_FAILED;
            }

            pCurPos += (strlen(pCurPos) + 1);
            ulTemp++;
            for (; ulTemp < ulCount; ulTemp++)
            {
                if ((pTempPos = strstr(pCurPos, ".GatewayIPAddress=")) != NULL)
                {
                    memset(szGatewayIp, 0, sizeof(szGatewayIp));
                    strcpy(szGatewayIp, pTempPos + strlen(".GatewayIPAddress="));
                }
                else if ((pTempPos = strstr(pCurPos, ".DestSubnetMask=")) != NULL)
                {
                    memset(szDestSubnet, 0, sizeof(szDestSubnet));
                    strcpy(szDestSubnet, pTempPos + strlen(".DestSubnetMask="));
                }
                else if ((pTempPos = strstr(pCurPos, ".DestIPAddress=")) != NULL)
                {
                    memset(szDestNetwork, 0, sizeof(szDestNetwork));
                    strcpy(szDestNetwork, pTempPos + strlen(".DestIPAddress="));

                    //Print this node information
                    IO_Print("  %s\t%s\t%s\t%s\n", szDestNetwork, szDestSubnet, szGatewayIp, szWanInterface);
                    break;
                }

                pCurPos += (strlen(pCurPos) + 1);
            } //End of for loop
        }

        pCurPos += (strlen(pCurPos) + 1);
    } //End of for loop

    IO_Print("\n"); //End of print

    return TBS_SUCCESS;
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
        CLI_ROUTE_TRACE(("%s :The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

        if(ulCount == 0)
        {
            IO_Print("%s :The items is 0\n", __FUNCTION__);
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
                sscanf(pCurPos, CLI_WAN_IP_CONNECTION_NAME, &ulIndex1, &ulIndex2, &ulIndex3, szName);
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
                sscanf(pCurPos, CLI_WAN_PPP_CONNECTION_NAME, &ulIndex1, &ulIndex2, &ulIndex3, szName);
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
        ulWanDevIndex++;
    }

    IO_Print("\r\nconnection name:%s not exist!\n", pszConnName);
    return TBS_FAILED;
}
/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif
