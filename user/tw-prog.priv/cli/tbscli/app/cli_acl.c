
#include "../cli_pub.h"
#include "../cli/cli_term.h"
#include "../cli/cli_io.h"
#include "../cli/cli_interpret.h"
#include "cli_cmd.inc"
#include "common.h"
#include "../cli_comm.h"

#ifdef __cplusplus
extern "C" {
#endif



#define ARRAYLEN(array) ((sizeof(array))/(sizeof(array[0])))

typedef struct __port_name{
	char *optname;
	char *tr69name;
}port_name;

#define CLI_WAN_IP_CONNECTION_NAME      "string InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANIPConnection.%lu.Name=%s"
#define CLI_WAN_PPP_CONNECTION_NAME     "string InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice.%lu.WANPPPConnection.%lu.Name=%s"
#define CLI_WAN_DEVICE_BASE             "InternetGatewayDevice.WANDevice.%lu.WANConnectionDevice."
#define CLI_WAN_IP_CONNECTION           "WANIPConnection"
#define CLI_WAN_PPP_CONNECTION          "WANPPPConnection"
#define CLI_SCHEDULE_NODE               "InternetGatewayDevice.X_TWSZ-COM_SCHEDULES.SchedEntry."
#define CLI_SCHEDULE_NODE_SCHEDNAME     "string InternetGatewayDevice.X_TWSZ-COM_SCHEDULES.SchedEntry.%lu.SchedName=%s"

#define CLI_ACL_BASE "InternetGatewayDevice.X_TWSZ-COM_ACL."
#define CLI_ACL_RACL "RACL."
#define CLI_ACL_LACL "LACL."

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

ULONG CLI_Acl_Wan_Proc()
{
	char *p_conname = NULL;
	char *p_port = NULL;
	char *p_optname = NULL;
	port_name arr_port[]={
		{"ftp","FTP"},
		{"http","HTTP"},
		{"icmp","ICMP"},
		{"snmp","SNMP"},
		{"ssh","SSH"},
		{"telnet","TELNET"},
		{"tftp","TFTP"}
	};
	 //acl: <conname(s)> <contype(s)> <port(s)> <optname(s)>
	char *p0 = NULL;
	int i = 0;
	int objnumber = 0; 
	char aszPath[1][CLI_NODE__LEN];
    char aszValue[1][CLI_NODE__LEN];
	char tmppath[MAX_PATH_LEN] = {0};

	memset(aszPath,0,sizeof(aszPath));
	memset(aszValue,0,sizeof(aszValue));

	if ( (p_conname = CLI_GetParamByName("conname")) == NULL)
	{
		IO_Print("\r\nPlease Input conname\n");
		return TBS_FAILED;
	}

	if(CLI_GetPathByConnName(p_conname,tmppath,MAX_PATH_LEN) == TBS_FAILED)
	{
		IO_Print("\r\nThat no exsit a %s's connection\n",p_conname);
		return TBS_FAILED;
	}


	//get object id index
	p0 = tmppath;
	for(i=0;i<4;i++)
	{
		p0 = strstr(p0,".");
		p0++;
	}
	objnumber = atoi(p0);

	if ( (p_port = CLI_GetParamByName("port")) == NULL)
	{
		return TBS_FAILED;
	}

	for(i=0;i<ARRAYLEN(arr_port);i++)
	{
		if(strcmp(p_port,arr_port[i].optname) == 0)
		{
			break;
		}
	}

	if(i >= ARRAYLEN(arr_port))
	{
		IO_Print("That port name is only support ftp http icmp snmp ssh telnet tftp\n");
		return TBS_FAILED;
	}
	
	memset(tmppath,0,sizeof(tmppath));
	sprintf(tmppath,"%s%s%d.%s.%d.",CLI_ACL_BASE,CLI_ACL_RACL,objnumber,"Service",i+1);

	sprintf(aszPath[0],"%s%s",tmppath,"Enable");
	
	if ( (p_optname = CLI_GetParamByName("optname")) == NULL)
	{
		return TBS_FAILED;
	}
	
	if(strcmp(p_optname,"enable") == 0)
	{
		sprintf(aszValue[0],"%d",1);
		return CLI_SetLeaf(1,aszPath,aszValue);
	}
	else if(strcmp(p_optname,"disable") == 0)
	{
		sprintf(aszValue[0],"%d",0);
		return CLI_SetLeaf(1,aszPath,aszValue);
	}
	else if(strcmp(p_optname,"show") == 0)
	{
		int value = -1;
		
		if(CLI_GetLeaf(aszPath[0],aszValue[0]) != TBS_SUCCESS)
		{
			return TBS_FAILED;
		}
		value = atoi(aszValue[0]);
		if(value == 0)
		{
			IO_Print("\r\nWan Port:%s is %s\n",arr_port[i].optname,"Disable");
			return TBS_SUCCESS;
		}
		else if(value == 1)
		{
			IO_Print("\r\nWan Port:%s is %s\n",arr_port[i].optname,"Enable");
			return TBS_SUCCESS;
		}
		else
		{
			IO_Print("\r\nWan Port:%s is %s\n",arr_port[i].optname,"Error");
			return TBS_FAILED;
		}
	}
	else
	{
		IO_Print("\r\nThat optname is only support enable disable show\n");
		return TBS_FAILED;
	}
	return TBS_FAILED;
}



ULONG CLI_Acl_Lan_Proc()
{
	int i = 0;
	char *p_port = NULL;
	char *p_optname = NULL;
	port_name arr_port[]={
		{"ftp","FTP"},
		{"http","HTTP"},
		{"icmp","ICMP"},
		{"snmp","SNMP"},
		{"ssh","SSH"},
		{"telnet","TELNET"},
		{"tftp","TFTP"}
	};
	char aszPath[1][CLI_NODE__LEN];
    char aszValue[1][CLI_NODE__LEN];

	memset(aszPath,0,sizeof(aszPath));
	memset(aszValue,0,sizeof(aszValue));

	char tmppath[512] = {0};
	
	if ( (p_port = CLI_GetParamByName("port")) == NULL)
	{
		return TBS_FAILED;
	}

	for(i=0;i<ARRAYLEN(arr_port);i++)
	{
		if(strcmp(p_port,arr_port[i].optname) == 0)
		{
			break;
		}
	}

	if(i >= ARRAYLEN(arr_port))
	{
		IO_Print("That port name is only support ftp http icmp snmp ssh telnet tftp\n");
		return TBS_FAILED;
	}
	
	sprintf(tmppath,"%s%s%d.%s.%d.",CLI_ACL_BASE,CLI_ACL_LACL,1,"Service",i+1);
	sprintf(aszPath[0],"%s%s",tmppath,"Enable");
	
	if ( (p_optname = CLI_GetParamByName("optname")) == NULL)
	{
		return TBS_FAILED;
	}
	
	if(strcmp(p_optname,"enable") == 0)
	{
		sprintf(aszValue[0],"%d",1);
		return CLI_SetLeaf(1,aszPath,aszValue);
	}
	else if(strcmp(p_optname,"disable") == 0)
	{
		sprintf(aszValue[0],"%d",0);
		return CLI_SetLeaf(1,aszPath,aszValue);
	}
	else if(strcmp(p_optname,"show") == 0)
	{
		int value = -1;
		
		if(CLI_GetLeaf(aszPath[0],aszValue[0]) != TBS_SUCCESS)
		{
			return TBS_FAILED;
		}
		value = atoi(aszValue[0]);
		if(value == 0)
		{
			IO_Print("\r\nLan Port:%s is %s\n",arr_port[i].optname,"Disable");
			return TBS_SUCCESS;
		}
		else if(value == 1)
		{
			IO_Print("\r\nLan Port:%s is %s\n",arr_port[i].optname,"Enable");
			return TBS_SUCCESS;
		}
		else
		{
			IO_Print("\r\nLan Port:%s is %s\n",arr_port[i].optname,"Error");
			return TBS_FAILED;
		}
	}
	else
	{
		IO_Print("\r\nThat optname is only support enable disable show\n");
		return TBS_FAILED;
	}
	return TBS_FAILED;
}

ULONG CLI_Acl()
{
	char *p_contype = NULL;
	if ( (p_contype = CLI_GetParamByName("contype")) == NULL)
	{
		return TBS_FAILED;
	}

	if(strcmp(p_contype,"wan") == 0)
	{
		return CLI_Acl_Wan_Proc();
	}
	else if(strcmp(p_contype,"lan") == 0)
	{
		return CLI_Acl_Lan_Proc();
	}
	else
	{
		IO_Print("\r\nThat contype is only support wan or lan\n");
	}
	
	return TBS_FAILED;
}

#ifdef __cplusplus
}
#endif

