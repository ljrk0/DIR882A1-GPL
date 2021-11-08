/**********************************************************************
 * Copyright (c), 1991-2009, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 * 文件名称 : dhcpmon.c
 * 文件描述 : 本文件主要实现内核与应用层的通讯 
 * 修订记录 :
 *       1 创建 : 	taogan
 *         日期 : 	2015-09-06
 *         描述 :	使用NETLINK机制实现内核与应用层通讯
 **********************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#if 0
    #define DEBUG_TRACE(format, ...) do {printf("%s:%u - " format "\n" , __func__, __LINE__,  ##__VA_ARGS__ );} while(0)
#else
    #define DEBUG_TRACE(format, ...)
#endif

#define IPC_MAX_MSGSIZE	    			256				/* max received message size */

/*****************************************************************************
*                                STRUCT                                      *
******************************************************************************/
typedef struct netlink_dhcp_msg_payload {	/* valid message body struct */
	unsigned char ip[4];
	unsigned char mac[6];
	unsigned char hostname[64];
} DHCPMON_MSG_PAYLOAD, *PDHCPMON_MSG_PAYLOAD;


/******************************************************************************
 *                               LOCAL VAR                                    *
 ******************************************************************************/
static int s_iDHCPSock = 0;		/* netlink socket unique identifier */
static struct nlmsghdr *s_nlhdr = NULL;

/******************************************************************************
 *                               GLOBAL VAR                                    *
 ******************************************************************************/
static char *ether_etoa(unsigned char *e, char *a)
{
	static char hexbuf[] = "0123456789abcdef";
	
	int i, k;

	for (k = 0, i = 0; i < 6; i++) {
		a[k++] = hexbuf[(e[i] >> 4) & 0xF];
		a[k++] = hexbuf[(e[i]) & 0xF];
		a[k++]=':';
	}
	
	a[--k] = 0;
	
	return a;
}

/********************************************************************
 * Desc: query the valid socket id
 *
 * @return: unique socket id
 * @author: rongwei
 * @data:	2014/05/16
 *******************************************************************/
int DHCP_GetSocket( void )
{
	return s_iDHCPSock;
}
#define DHCP_MON_LIST_PATH "/tmp/dhcp_mon_list"
#define DHCP_MON_LIST_TMP_PATH "/tmp/dhcp_mon_list.tmp"

int DHCP_add_device(PDHCPMON_MSG_PAYLOAD pst_nlmsg_payload)
{
	FILE *fp = NULL;
	FILE *tmp_fp = NULL;
	char buf[256] = { 0 };
	char szMac[18] = { 0 }, szNewMac[18] = { 0 };
	int i_find = 0;

	if(-1 == access(DHCP_MON_LIST_PATH, F_OK))
	{
		fp = fopen(DHCP_MON_LIST_PATH, "w+");
	}
	else
	{
		fp = fopen(DHCP_MON_LIST_PATH, "r+");
	}

	if(NULL == fp)
	{
		printf("open %s fail\n", DHCP_MON_LIST_PATH);
		return -1;
	}

	tmp_fp = fopen(DHCP_MON_LIST_TMP_PATH, "w+");

	if(NULL == tmp_fp)
	{
		printf("open %s fail\n", DHCP_MON_LIST_TMP_PATH);
		fclose(fp);
		return -1;
	}

	while(NULL != fgets(buf, sizeof(buf), fp))
	{
		memset(szMac, 0, sizeof(szMac));
		if(TW_get_rule_safe(0, buf, ',', szMac, sizeof(szMac)) == -1)
		{
			continue;		
		}

		if( !strncasecmp(szMac, ether_etoa(pst_nlmsg_payload->mac, szNewMac), sizeof(szMac)) )
		{
			fprintf(tmp_fp, "%s,%d.%d.%d.%d,%s\n", szNewMac, pst_nlmsg_payload->ip[0], pst_nlmsg_payload->ip[1],
				pst_nlmsg_payload->ip[2], pst_nlmsg_payload->ip[3], pst_nlmsg_payload->hostname);
			i_find = 1;
			continue;
		}
		fprintf(tmp_fp, "%s", buf);
	}

	if( !i_find )
	{
		fprintf(tmp_fp, "%s,%d.%d.%d.%d,%s\n", ether_etoa(pst_nlmsg_payload->mac, szNewMac), pst_nlmsg_payload->ip[0], pst_nlmsg_payload->ip[1],
				pst_nlmsg_payload->ip[2], pst_nlmsg_payload->ip[3], pst_nlmsg_payload->hostname);
	}
	
	fclose(fp);
	fclose(tmp_fp);

	rename(DHCP_MON_LIST_TMP_PATH, DHCP_MON_LIST_PATH);

	return 0;

}
int DHCPMON_MainProc(void)
{
    int iRet;
	PDHCPMON_MSG_PAYLOAD pst_nlmsg_payload = NULL;
	char szMac[32] = { 0 };

    static struct sockaddr_nl daddr;
	static struct msghdr msg;
	static struct iovec iov;

    memset(&daddr, 0, sizeof(daddr));
    iov.iov_base = (void *)s_nlhdr;
    iov.iov_len = NLMSG_SPACE(IPC_MAX_MSGSIZE);

    msg.msg_name = (void *)&daddr;
    msg.msg_namelen = sizeof(daddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    /* 接收NETLINK消息并处理 */
    do
    {
        DEBUG_TRACE("OUT: receive msg.\n");

        /* 清空消息缓冲 */
        memset(s_nlhdr, 0, NLMSG_SPACE(IPC_MAX_MSGSIZE));

        /* 接收到消息 */
        iRet = recvmsg(s_iDHCPSock, &msg, 0);
        if (iRet == 0)
        {
            DEBUG_TRACE("OUT: Exit.\n");
            break;
        }
        else if (iRet == -1)
        {
            DEBUG_TRACE("OUT: Fail to receive msg\n");
            break;
        }

		pst_nlmsg_payload = NLMSG_DATA(s_nlhdr);

		if( NULL == pst_nlmsg_payload)
		{
			DEBUG_TRACE( "[%s(%d)] fail to receive message!\n", __FUNCTION__, __LINE__ );
			return -1;
		}
/*
		printf("DhcpMon ADD %d.%d.%d.%d %s %s\n",
		pst_nlmsg_payload->ip[0], pst_nlmsg_payload->ip[1],
		pst_nlmsg_payload->ip[2], pst_nlmsg_payload->ip[3],
		ether_etoa(pst_nlmsg_payload->mac, szMac), pst_nlmsg_payload->hostname);
*/
		DHCP_add_device(pst_nlmsg_payload);
		
    }while(1); /* while(0) 大循环*/

    return 0;
}

/********************************************************************
 * Desc: receive message from kernel and distribute it to corresponding module to process
 *
 * @cmd: operation command
 * @content: valid message content
 * 
 * @return: 0(success), -1(Fail)
 * @author: rongwei
 * @data:	2014/05/16
 *******************************************************************/
int IPC_SendMsg( int cmd, char *content )
{
	return 1;
}

/********************************************************************
 * Desc: this routine is mainly response to set the pid of current proccess
 * to proc entry
 *
 * @return: 0(success), -1(Fail)
 * @author: rongwei
 * @data:	2014/05/16
 *******************************************************************/
static int IPC_set_sock_pid( void )
{
	FILE *fp = NULL;
	char pid[ 8 ] = { 0 };			// pid of current proccess
	char entry_name[ 32 ] = { 0 };	// proc entry path

	/* open the target proc entry of pid for 2.4G wlan module */
	sprintf( entry_name, "%s", "/proc/dhcpmon_pid");
	fp = fopen( entry_name, "w" );
	if( !fp )
    {
        DEBUG_TRACE( "[%s(%d)] Fail to open proc entry(%s)!\n", __FUNCTION__, __LINE__, entry_name );
        return -1;
    }
	/* put the pid to proc entry of pid */
	sprintf( pid, "%d", getpid() );
    if( 0 == fputs( pid, fp ) )
    {
        DEBUG_TRACE( "[%s(%d)] Fail to write to proc entry(%s)!\n", __FUNCTION__, __LINE__, entry_name );
		fclose(fp);
        return -1;
    }
	
    fclose(fp);

	return 0;
}

/********************************************************************
 * Desc: this routine is mainly response to create socket
 *
 * @return: 0(success), -1(Fail)
 * @author: rongwei
 * @data:	2014/05/16
 *******************************************************************/
int main( void )
{
	struct sockaddr_nl sock_addr;

	/* put pid to proc entry in order to read by kernel */
	if( IPC_set_sock_pid() < 0 )
	{
		return -1;
	}

	/* create netlink socket with private netlink protocol */
	s_iDHCPSock = socket( AF_NETLINK, SOCK_RAW, NETLINK_DHCP_TBS );
	if( s_iDHCPSock < 0 )
	{
		DEBUG_TRACE("[%s(%d)] Fail to create socket(protocol=%d)! err=%s %d\n", __FUNCTION__, __LINE__, NETLINK_DHCP_TBS, strerror(errno), errno );
		return -1;
	}

	/* bind netlink socket with local address */
	memset( &sock_addr, 0, sizeof( sock_addr ) );
	sock_addr.nl_family = AF_NETLINK;
	sock_addr.nl_pid = getpid();
	sock_addr.nl_groups = 0;
	if( bind( s_iDHCPSock, (struct sockaddr*)&sock_addr, sizeof( sock_addr ) ) != 0 )
	{
		DEBUG_TRACE("[%s(%d)] Fail to bind socket!\n", __FUNCTION__, __LINE__ );
		close( s_iDHCPSock );
		return -1;
	}
	
	s_nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(IPC_MAX_MSGSIZE));
    if(s_nlhdr == NULL)
    {
	   DEBUG_TRACE("[%s(%d)] Fail to malloc s_nlhdr\n", __FUNCTION__, __LINE__ );
	   close(s_iDHCPSock);
	   return -1;
    }

	DHCPMON_MainProc();

	system("echo 0 > /proc/dhcpmon_pid");  /* set to 0 when exit. */

    if (s_nlhdr != NULL)
    {
        free(s_nlhdr);
        s_nlhdr = NULL;
    }

    if (s_iDHCPSock > 0)
    {
        close(s_iDHCPSock);
        s_iDHCPSock = -1;
    }

	return 0;
}

