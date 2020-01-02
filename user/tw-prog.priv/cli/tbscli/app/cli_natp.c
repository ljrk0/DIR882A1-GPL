/**********************************************************************
 Copyright (c), 1991-2010, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 File Name: cli_natp.c
 File description: natp function for CLI
 Modify history:
        1. Author: Tangqiang
           Date: 2010-01-05
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
#define CLI_NATP_TRACE(x) //IO_Print x

#define CLI_WAN_IP_CONNECTION_NAME      "string InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANIPConnection.%lu.Name=%s"
#define CLI_WAN_PPP_CONNECTION_NAME     "string InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANPPPConnection.%lu.Name=%s"
#define CLI_WAN_DEVICE_BASE             "InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice."
#define CLI_WAN_IP_CONNECTION           "WANIPConnection"
#define CLI_WAN_PPP_CONNECTION          "WANPPPConnection"
#define CLI_SCHEDULE_NODE               "InternetGatewayDevice.X_TWSZ-COM_SCHEDULES.SchedEntry."
#define CLI_SCHEDULE_NODE_SCHEDNAME     "string InternetGatewayDevice.X_TWSZ-COM_SCHEDULES.SchedEntry.%lu.SchedName=%s"

#define CLI_PORT_MAPPING_ENABLE    "1"
#define CLI_PORT_MAPPING_DISABLE    "0"
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
 

/******************************************************************************
 *                               GLOBAL VAR                                   *
 ******************************************************************************/


/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/
ULONG CLI_Natp_Add_Rule();
ULONG CLI_Natp_Delete_Rule();
ULONG CLI_Natp_Show();
static ULONG CLI_GetPathByConnName(char *pszConnName, char *pszPath, int iLen);
static ULONG CLI_GetPathByScheduleName(char *pszScheduleName, char *pszPath, int iLen);

//Main function of NATP command
ULONG CLI_Natp()
{
    if (CLI_GetParamByName("add") != NULL)
    {
        return CLI_Natp_Add_Rule();
    }
    else if (CLI_GetParamByName("del") != NULL)
    {
        return CLI_Natp_Delete_Rule();
    }
    else if (CLI_GetParamByName("show") != NULL)
    {
        return CLI_Natp_Show();
    }
    else
    {
    }

    return TBS_FAILED;
}

/*************************************************************
function  : Add a new NATP rule(Port forwarding rule)
parameters: none
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : use cli command such as:
            natp add tank pppoe_0_32 tcp 80 80 8080 8080 192.168.1.5
*************************************************************/
ULONG CLI_Natp_Add_Rule()
{
    ULONG ulInstIndex;
    char *pszServerName        = NULL;
    char *pszWanInterface      = NULL;
    char *pszProtocol          = NULL;
    ULONG ulExternalStartPort  = 0;
    ULONG ulExternalEndPort    = 0;
    ULONG ulInternalStartPort  = 0;
    //ULONG ulInternalEndPort    = 0;
    char *pszLocalIp           = NULL;
    char *pszScheduleName      = NULL;
    char *pszRemoteIp          = NULL;
    char szPath[MAX_PATH_LEN]  = {0};
    char szPath2[MAX_PATH_LEN] = {0};
    char szPath3[MAX_PATH_LEN] = {0};
    char aszPath[9][CLI_NODE__LEN];
    char aszValue[9][CLI_NODE__LEN];
    
    //Check if get parameter of server
    CLI_GET_PARAMETER_ASSERT(pszServerName, "server");

    //Check if get parameter of wan interface
    CLI_GET_PARAMETER_ASSERT(pszWanInterface, "wan");

    //Check if get parameter of protocol
    CLI_GET_PARAMETER_ASSERT(pszProtocol, "protocol");

    //Check if get the external start port number
    CLI_GET_ULONG_ASSERT(ulExternalStartPort, "ex-start");

    //Check if get the external end port number
    CLI_GET_ULONG_ASSERT(ulExternalEndPort, "ex-range");

    //Check end port&start port
    if(ulExternalEndPort < ulExternalStartPort)
    {
        IO_Print("\n  Ex-range value should be larger than or equal to the ex-start value.");
        return TBS_FAILED;
    }

    //Check if get the internal start port number
    CLI_GET_ULONG_ASSERT(ulInternalStartPort, "internalport");

    //Check if get the internal end port number
   // CLI_GET_ULONG_ASSERT(ulInternalEndPort, "in-end");

    //Check if get the parameter of local IP
    CLI_GET_PARAMETER_ASSERT(pszLocalIp, "local");

    //Check if get the parameter of schedule name
    if (CLI_GetParamByName("schedule") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszScheduleName, "name");
    }

    //Check if get the parameter of remote IP
    if (CLI_GetParamByName("remote") != NULL)
    {
        CLI_GET_PARAMETER_ASSERT(pszRemoteIp, "IP");
    }

    //Get the path of the wan interface node
    if (CLI_GetPathByConnName(pszWanInterface, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    CLI_NATP_TRACE(("Get the node path : %s\n", szPath));

    //Create the node for NATP rule
    strcat(szPath, "PortMapping.");
    if (CLI_AddObjectEx(szPath, &ulInstIndex) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }

    //fill the elements of the NATP rule
    sprintf(aszPath[0], "%s%lu.PortMappingDescription", szPath, ulInstIndex);
    sprintf(aszValue[0], "%s", pszServerName);

    sprintf(aszPath[1], "%s%lu.PortMappingProtocol", szPath, ulInstIndex);
    sprintf(aszValue[1], "%s", pszProtocol);

    sprintf(aszPath[2], "%s%lu.ExternalPort", szPath, ulInstIndex);
    sprintf(aszValue[2], "%lu", ulExternalStartPort);

    sprintf(aszPath[3], "%s%lu.ExternalPortEndRange", szPath, ulInstIndex);
    sprintf(aszValue[3], "%lu", ulExternalEndPort);

    sprintf(aszPath[4], "%s%lu.InternalPort", szPath, ulInstIndex);
    sprintf(aszValue[4], "%lu", ulInternalStartPort);

    //sprintf(aszPath[5], "%s%lu.InternalPortEndRange", szPath, ulInstIndex);
    //sprintf(aszValue[5], "%lu", ulInternalEndPort);

    sprintf(aszPath[5], "%s%lu.InternalClient", szPath, ulInstIndex);
    sprintf(aszValue[5], "%s", pszLocalIp);

    sprintf(aszPath[6], "%s%lu.PortMappingTime", szPath, ulInstIndex);
    if (pszScheduleName != NULL)
    {
        if ((strcasecmp(pszScheduleName, "Always") != 0) && (strcasecmp(pszScheduleName, "Never") != 0))
        {
            //Get the path of the schedule node
            if(CLI_GetPathByScheduleName(pszScheduleName, szPath2, MAX_PATH_LEN) != TBS_SUCCESS)
            {
                return TBS_FAILED;
            }
            CLI_NATP_TRACE(("Get the node path : %s\n", szPath2));
            
            sprintf(aszValue[6], "%s", szPath2);
        }
        else
        {
            if(0 == strcasecmp(pszScheduleName, "Always"))
            {
                strcpy(aszValue[6], "Always");
            }
            else if(0 == strcasecmp(pszScheduleName, "Never"))
            {
                strcpy(aszValue[6], "Never");
            }
        }
    }
    else
    {
        sprintf(aszValue[6], "%s", "Always");
    }

    sprintf(aszPath[7], "%s%lu.RemoteHost", szPath, ulInstIndex);
    if (pszRemoteIp != NULL)
    {
        sprintf(aszValue[7], "%s", pszRemoteIp);
    }
    else
    {
        strcpy(aszValue[7], "");
    }

    sprintf(aszPath[8], "%s%lu.PortMappingEnabled", szPath, ulInstIndex);
    sprintf(aszValue[8], "%s", CLI_PORT_MAPPING_ENABLE);

    //Begin set the values
    if (CLI_SetLeaf(9, aszPath, aszValue) != TBS_SUCCESS)
    {
       //如果添加叶子节点失败，需要删除已增加的实例对象，
       //否则会导致web页面port forwarding显示出错
       sprintf(szPath3, "%s%lu.", szPath, ulInstIndex);
	  if (CLI_DelObject(szPath3) != TBS_SUCCESS)
      {
            return TBS_FAILED;
      }
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}

/*************************************************************
function  : Delete an exist NATP rule(Port forwarding rule)
parameters: none
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : use cli command such as:
            natp del tank pppoe_0_32
*************************************************************/
ULONG CLI_Natp_Delete_Rule()
{
    char *pszServerName       = NULL;
    char *pszWanInterface     = NULL;
    char szPath[MAX_PATH_LEN] = {0};
    ST_MSG *pstMsg            = NULL;
    ULONG ulCount             = 0;
    ULONG ulTemp              = 0;
    char *pCurPos             = NULL;
    char *pTempPos            = NULL;

    //Check if get parameter of server
    CLI_GET_PARAMETER_ASSERT(pszServerName, "server");

    //Check if get parameter of wan interface
    CLI_GET_PARAMETER_ASSERT(pszWanInterface, "wan");

    //Get the path of the wan interface node
    if (CLI_GetPathByConnName(pszWanInterface, szPath, MAX_PATH_LEN) != TBS_SUCCESS)
    {
        return TBS_FAILED;
    }
    strcat(szPath, "PortMapping.");
    CLI_NATP_TRACE(("Get the node path : %s\n", szPath));

    /*
     *Get the Node such as InternetGatewayDevice.WANDevice.1.WANConnectionDevice.{i}.WANPPPConnection.{i}.PortMapping.{i}
     */
    if ((pstMsg = CLI_GetNode(szPath)) == NULL)
    {
        return TBS_FAILED;
    }

    ulCount = *(ULONG*)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        //The number of the items can not greater than 1024
        CLI_NATP_TRACE(("%s :The items is too big : %lu\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Set pCurPos point to the items
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);

    for (; ulTemp < ulCount; ulTemp++)
    {
        pTempPos = strstr(pCurPos, ".PortMappingDescription=");
        if (pTempPos != NULL)
        {
            if (strcmp(pszServerName, pTempPos+strlen(".PortMappingDescription=")) == 0)
            {
                memset(szPath, 0, MAX_PATH_LEN);
                /*
                 *Skip the beginning of "string ", it's 7 bytes
                 *The struct of data which CMM returned is : string xxx.xxx.xxx.{i}.xxx=xxx
                 */
                strncpy(szPath, pCurPos + 7, pTempPos-pCurPos - 7 + 1);

                /*
                 *It's not good here because NAT rule may have the same name.
                 *It will delete all the nodes which have this name
                 */
                if (CLI_DelObject(szPath) != TBS_SUCCESS)
                {
                    return TBS_FAILED;
                }
            }
        }

        //Move the pointer to the next item
        pCurPos += (strlen(pCurPos) + 1);
    }

    return TBS_SUCCESS;
}

/*************************************************************
function  : List the exist NATP rule(Port forwarding rule)
parameters: none
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : use cli command such as:
            natp show
*************************************************************/
ULONG CLI_Natp_Show()
{
    ST_MSG *pstMsg = NULL;
    ULONG ulCount  = 0;
    ULONG ulTemp   = 0;
    char *pCurPos  = NULL;
    char *pTempPos = NULL;
    
    //The elements of the NATP rule
    char szServerName[256]    = {0};
    char szWanInterface[256]  = {0};
    char szProtocol[16]       = {0};
    ULONG ulExternalStartPort = 0;
    ULONG ulExternalEndPort   = 0;
    ULONG ulInternalStartPort = 0;
    //ULONG ulInternalEndPort   = 0;
    char szLocalIp[32]        = {0};
    char szScheduleName[256]  = {0};
    char szRemoteIp[32]       = {0};
    char szPath[MAX_PATH_LEN] = {0};
    ULONG  ulWanDevIndex = 1;

    while(1)
    {
        memset(szPath, 0 , MAX_PATH_LEN);
        sprintf(szPath, CLI_WAN_DEVICE_BASE, ulWanDevIndex);
    
        //Get the node of the wan device
        pstMsg = CLI_GetNode(szPath);
        if (pstMsg == NULL)
        {
            if(ulWanDevIndex == 1)
            {
                return TBS_FAILED;
            }
            else
            {
                return TBS_SUCCESS;
            }
        }

        //Get the items
        ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
        if (ulCount > 1024)
        {
            if(ulWanDevIndex == 1)
            {
                return TBS_FAILED;
            }
            else
            {
                return TBS_SUCCESS;
            }
        }

        pCurPos = pstMsg->szMsgBody + sizeof(ULONG)*2;

        //Print the head at first
        if(ulWanDevIndex == 1)
        {
            IO_Print("\n%s   %s   %s   %s   %s   %s   %s   %s   %s   %s\n", "Server", "Wan interface", "Protocol", "External Port Start", "End",
                                                                            "Internal Port Start", "End", "Local IP", "Schedule", "Remote IP");
        }
        
        for (; ulTemp < ulCount; ulTemp++)
        {
            pTempPos = strstr(pCurPos, ".Name=");
            if (pTempPos != NULL)
            {
                UCHAR found_begin = 0;
                UCHAR found_end = 0;
                UCHAR interface_end_tag = 0;
                
                //Print the wan interface name
                sscanf(pTempPos, ".Name=%s", szWanInterface);
                //IO_Print("\n%s:\n", szWanInterface);

            //Move the pointer to the NATP item
            memset(szPath, 0, sizeof(szPath));
            strncpy(szPath, pCurPos + 7, (pTempPos - pCurPos - 7 + 1));
            do
            {
                ulTemp++;
                pCurPos += (strlen(pCurPos) + 1);

                if (strstr(pCurPos, szPath) == NULL)
                {
                    //Current wan interface doesn't have NATP rule
                    interface_end_tag = 1;
                    break;
                }
            }while(strstr(pCurPos, ".X_TWSZ-COM_Weight=") == NULL);

            if (interface_end_tag == 0)
            {
                for(; ulTemp < ulCount; ulTemp++)
                {
                    if(strstr(pCurPos, ".X_TWSZ-COM_Weight=") != NULL)
                    {
                        found_begin = 1;
                    }
                    else if((pTempPos = strstr(pCurPos, ".PortMappingTime=")) != NULL)
                    {
                        memset(szScheduleName, 0, sizeof(szScheduleName));
                        strcpy(szScheduleName, pTempPos+strlen(".PortMappingTime="));
                    }
                    else if((pTempPos = strstr(pCurPos, ".PortMappingDescription=")) != NULL)
                    {
                        memset(szServerName, 0, sizeof(szServerName));
                        strcpy(szServerName, pTempPos+strlen(".PortMappingDescription="));
                    }
                    else if((pTempPos = strstr(pCurPos, ".InternalClient=")) != NULL)
                    {
                        memset(szLocalIp, 0, sizeof(szLocalIp));
                        strcpy(szLocalIp, pTempPos+strlen(".InternalClient="));
                    }
                    else if((pTempPos = strstr(pCurPos, ".PortMappingProtocol=")) != NULL)
                    {
                        memset(szProtocol, 0, sizeof(szProtocol));
                        strcpy(szProtocol, pTempPos+strlen(".PortMappingProtocol="));
                    }
                    /*else if((pTempPos = strstr(pCurPos, ".InternalPortEndRange=")) != NULL)
                    {
                        pTempPos += strlen(".InternalPortEndRange=");
                        sscanf(pTempPos, "%lu", &ulInternalEndPort);
                    }*/
                    else if ((pTempPos = strstr(pCurPos, ".InternalPort=")) != NULL)
                    {
                        pTempPos += strlen(".InternalPort=");
                        sscanf(pTempPos, "%lu", &ulInternalStartPort);
                    }
                    else if ((pTempPos = strstr(pCurPos, ".ExternalPortEndRange=")) != NULL)
                    {
                        pTempPos += strlen(".ExternalPortEndRange=");
                        sscanf(pTempPos, "%lu", &ulExternalEndPort);
                        }
                    else if ((pTempPos = strstr(pCurPos, ".ExternalPort=")) != NULL)
                    {
                        pTempPos += strlen(".ExternalPort=");
                        sscanf(pTempPos, "%lu", &ulExternalStartPort);
                    }
                    else if ((pTempPos = strstr(pCurPos, ".RemoteHost=")) != NULL)
                    {
                        memset(szRemoteIp, 0, sizeof(szRemoteIp));
                        strcpy(szRemoteIp, pTempPos+strlen(".RemoteHost="));
                    }
                    else if (strstr(pCurPos, ".PortMappingEnabled=") != NULL)
                    {
                        found_end = 1;
                    }
                    else
                    {
                    }

                    if (found_begin == 0)
                    {
                        CLI_NATP_TRACE(("NATP rules have been output for wan interface :%s\n", szWanInterface));
                        break;
                    }
                    if (found_end == 1)
                    {
                        //Print the NATP rule
                        IO_Print("  %s\t%s\t%s\t%lu\t%lu\t%lu\t%lu\t%s\t%s\t%s\n", szServerName, szWanInterface, szProtocol,
                                                                                   ulExternalStartPort, ulExternalEndPort,
                                                                                   ulInternalStartPort, ulInternalStartPort+ulExternalEndPort-ulExternalStartPort,
                                                                                   szLocalIp, szScheduleName, szRemoteIp);
                        found_begin = 0;
                        found_end = 0;
                    }

                        pCurPos += (strlen(pCurPos) + 1);
                    } //End of for loop
                } //End of if (interface_end_tag == 0)
            } //End of if (pTempPos != NULL)

            pCurPos += (strlen(pCurPos) + 1);
        } //End of for loop
        
        ulWanDevIndex++;
    }

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
        CLI_NATP_TRACE(("%s :The items is too big : %d\n", __FUNCTION__, ulCount));
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

/*************************************************************
function  : Get the path of the specific schedule name
parameters: pszScheduleName : the name of the specific schedule
            pszPath         : output the path of the schedule
return    : TBS_SUCCESS if successed
            TBS_FAILED if failed
remark    : Do not try to release the pstMsg here, because it will point to the global
            variable :g_pstCliMsg, cli_comm.c will do the release.
*************************************************************/
static ULONG CLI_GetPathByScheduleName(char *pszScheduleName, char *pszPath, int iLen)
{
    ST_MSG *pstMsg = NULL;
    ULONG  ulCount = 0;
    ULONG  ulTemp  = 0;
    ULONG  ulIndex1= 0;
    char *pCurPos  = NULL;
    char *pTempPos = NULL;
    char szName[MAX_NODE_VALUE_LEN] = {0};

    if ((pszScheduleName == NULL)
        || (pszPath == NULL)
        || (iLen < MAX_PATH_LEN))
    {
        return TBS_FAILED;
    }
    
    //Send GET message to get all the leaves of the node which szPath specified
    pstMsg = CLI_GetNode(CLI_SCHEDULE_NODE);
    if (pstMsg == NULL)
    {
        return TBS_FAILED;
    }

    //Get item count, need to skip the result
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount > 1024)
    {
        CLI_NATP_TRACE(("%s :The items is too big : %d\n", __FUNCTION__, ulCount));
        return TBS_FAILED;
    }

    //Skip the result and item count words
    pCurPos = (pstMsg->szMsgBody + sizeof(ULONG)*2);
    memset(szName, 0, MAX_NODE_VALUE_LEN);    
    for (ulTemp = 0; ulTemp < ulCount; ulTemp++)
    {
        if ((pTempPos = strstr(pCurPos, ".SchedName=")) != NULL)
        {
            //found
            memset(szName, 0, sizeof(szName));
            sscanf(pCurPos, CLI_SCHEDULE_NODE_SCHEDNAME, &ulIndex1, szName);
            if (strcmp(szName, pszScheduleName) == 0)
            {
                /*
                 *Skip the beginning of "string ", it's 7 bytes
                 *The struct of data which CMM returned is : string xxx.xxx.xxx.{i}.xxx=xxx
                 */
                strncpy(pszPath, pCurPos + 7, (pTempPos - pCurPos - 7 + 1));
                return TBS_SUCCESS;
            }
        }
        if (pCurPos)
            pCurPos += (strlen(pCurPos) + 1);
    }

    IO_Print("\r\nSchedule name : %s is not exist\n", pszScheduleName);
    return TBS_FAILED;
}
/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif
