#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <linux/errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h> 
#include <errno.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/wireless.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <errno.h> 
//#include "flash_layout.h"
#include "../protest/protest.h"

#include <string.h>
#include <assert.h>
//#include <bcmnvram.h>
//#include <nvram_list.h>
//#include <wlif_utils.h>
//#include <shutils.h>

#if 0
#define SIOCGIWNAME				0x8B01		/* get name == wireless protocol */
#define SIOCGIWRTLSTAINFO       0x8B30
#define SIOCGIWRTLSTANUM        0x8B31
#define SIOCGIWRTLDRVVERSION    0x8B32
#define SIOCGIWRTLSCANREQ       0x8B33
#define SIOCGIWRTLGETBSSDB      0x8B34
#define SIOCGIWRTLJOINREQ       0x8B35
#define SIOCGIWRTLJOINREQSTATUS 0x8B36
#define SIOCGIWRTLGETBSSINFO    0x8B37
#define SIOCGIWRTLGETWDSINFO    0x8B38
#define SIOCGIWRTLGETRSSI          0x8B4A
#endif

#define MAX_RSSI_LEN  65
#define MAX_WLANDEV_LEN 16
#define MAX_MSG_SIZE 1024


struct serverip {
	int port;
	int wlandev;
	char ip[IP_ADDR_LENGTH];
};


int sockfd = 0;//save socket handle
char rssi_buf[MAX_MSG_SIZE];
struct serverip server_info;
struct sockaddr_in server_addr;

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/
int get_rssi_val(void)
{
	char wl_interface[MAX_RSSI_LEN] = {0}; 
	char rssi_info[MAX_RSSI_LEN] = {0};
	int rssi_val[3] = {0};
	int iResult = 0;
	char cmd[512] = {0};
	FILE *fp = NULL;

	if(0 == server_info.wlandev)//1//
	{
		memcpy(wl_interface, "eth1", MAX_RSSI_LEN);/*2.4G 接口*/
	}
	else if(1 == server_info.wlandev)//2//
	{
		memcpy(wl_interface, "eth2", MAX_RSSI_LEN);/*5G 接口*/
	}
	else if(3 == server_info.wlandev)
	{
		PTI_PRINTF("Error:Device Num");
		return -1;
	}

	//get results
	sprintf(cmd,"wl -i %s phy_rssi_ant",wl_interface);

	fp = popen(cmd, "r");

	sleep(1);
	
	if(fp)
	{
		while( fgets(rssi_info, sizeof(rssi_info),fp) )
		{
				PTI_PRINTF("%s\n",rssi_info);
				sscanf(rssi_info,"rssi[0] %u  rssi[1] %u  rssi[2] %u ",
						&rssi_val[0], &rssi_val[1], &rssi_val[2]);
				PTI_PRINTF("===rssi[%d:%d:%d]==",rssi_val[0],rssi_val[1],rssi_val[2]);
				break;
		}
		
		pclose(fp);
	}else
	{
		return -1;
	}
	
 	iResult = (rssi_val[0]+rssi_val[1]+rssi_val[2])/3 ;
	
	return iResult;
}



void  PrintMsg(int sig)
{
	int iret = 0;
    int iRssiResult = 0;
	int n; 

	struct timeval stTv;
	
	iRssiResult = get_rssi_val();
	
	if(iRssiResult == 0)
	{
		printf("Error:[%s]Get RSSI failed!\n",__FUNCTION__);
		return ;
	}
	
	printf("Info rssi = %d \n", iRssiResult);

	gettimeofday(&stTv, NULL);

	memset(rssi_buf,0,sizeof(rssi_buf));
	
	sprintf(rssi_buf, "%lld%lld#%d\n", (long long )stTv.tv_sec, (long long )stTv.tv_usec, iRssiResult); /* 暂时这样写，明确报文格式后修改*/
	 
	/*向服务器端发送数据报*/

	iret = sendto(sockfd, rssi_buf, strlen(rssi_buf), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)); 
	if(iret)
	{
		printf("Error:[%s]send failed!\n",__FUNCTION__);
	}
	
	return ;
}

int main(int argc, char **argv)
{
	struct   itimerval  tick;
	int ret = 0;


	if(4 != argc)
		return 0;

	memset(&server_info, 0, sizeof(struct serverip));

	memcpy(server_info.ip, argv[1], IP_ADDR_LENGTH);
	server_info.port = atoi(argv[2]);
	server_info.wlandev = atoi(argv[3]);


	/*创建套接字*/
	sockfd = socket(AF_INET,SOCK_DGRAM,0); 
	if(sockfd < 0) 
	{ 
		printf("Socket Error:%s\n",strerror(errno)); 
		exit(1); 
	}

	/* 填充服务端的资料 */ 
	bzero(&server_addr,sizeof(struct sockaddr_in)); 
	server_addr.sin_family=AF_INET; 
	server_addr.sin_port=htons(server_info.port); 
	server_addr.sin_addr.s_addr =inet_addr(server_info.ip);

	//注册SIGALRM信号调用PrintMsg
    signal(SIGALRM,  PrintMsg);

    tick.it_value.tv_sec  =   3 ;    // 十秒钟后将启动定时器 
    tick.it_value.tv_usec  =   0 ;
    tick.it_interval.tv_sec    = 0 ;  // 定时器启动后，每隔1秒将执行相应的函数 
    tick.it_interval.tv_usec  =  100000 ;

    // setitimer将触发SIGALRM信号
    ret = setitimer(ITIMER_REAL,  & tick,  NULL);

    if   (  ret  !=   0 )
    {
            printf( " Set  timer  error.  %s  \n " ,  strerror(errno)  );

            return   - 1 ;
    }

    while(1)
            pause();

	if(sockfd)
		close(sockfd); 
	
    return   0 ;

}


