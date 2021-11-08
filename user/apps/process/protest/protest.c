
/**********************************************************************
 Copyright (c), 1991-2012, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称: protest.c
 文件描述: 实现产测统一接口功能
 修订记录:
        1. 作者: 苏邓海
           日期: 
           内容: 创建文件
**********************************************************************/
//#include <typedefs.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include "../button/btn_gpio.h"
//#include <bcmgpio.h>
#include "protest.h"
#include "pti_product.h"
//#include <bcmconfig.h>
#include <linux/sockios.h>
#include <linux/types.h>
#include <linux/ethtool.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <ctype.h>
#include <linux/autoconf.h>
#include <regex.h>
#include <netdb.h>
#include <resolv.h>
#include <signal.h>
#include "nvram.h"
#include "shutils.h"
#include "version.h"


static int setLanMac(char *pstr);
int PTI_GetMacAddr(char *pszMac);

 /******************************************************************************
*                               GLOBAL VAR                                   *
******************************************************************************/
static btn_t *btn_chain[BCMGPIO_MAXPINS] = {NULL};
static btn_t wl_btn;
static btn_t rs_btn;
static btn_t wps_btn;

void loadDefaultMacAddr()
{
    char mac_buf[18] = { 0};
	char tmp_mac_buf[18] = { 0};
	int i = 0, j = 0;
	
	if (-1 == PTI_GetMacAddr(mac_buf))
	{
		return -1;
	}

	for(i=0; i < sizeof(mac_buf); i ++)
	{
		if(':' != mac_buf[i])
		{
			tmp_mac_buf[j] = mac_buf[i];
			j++;
		}
	}
	setLanMac(tmp_mac_buf);
}

//for mydlink uid by wwzh
#define KEY_SIZE 16
#define KEY_RANGE 62
#include <stdlib.h>
#include <sys/time.h>

int gen_mydlink_uid(char *uniq_val, unsigned int mac_addr_last_4bytes)
{
 	struct timeval tv;
	int i, v;
	
 	/* necessary to randomize the start value */
 	gettimeofday(&tv, NULL);
 	srand(tv.tv_usec<<12 | mac_addr_last_4bytes);
	 /* key set: 09,
	 az,
	 AZ
	 */
 	for (i = 0; i < KEY_SIZE; i++) 
	{
 		v = rand() % KEY_RANGE;
 		uniq_val[i] = (v > 35) ? (v - 36 + 'A') : (v > 9) ? (v - 10 + 'a') : (v + '0');
 	}
 	uniq_val[KEY_SIZE] = 0;
 	printf("%s", uniq_val);
 	return 0;
 }

 
 static int setLanMac(char *pstr)
 {
	 char buf[256] ={ 0 };
	 unsigned char mac_addr[6];
	 int i = 0;
	 /***
	 wan为基mac;ra0=lan+1;rai0=lan+2, wan=lan+3
	 **/
	 char				 * byte;
	 unsigned int		 mac_value = 0;
 	 struct ifreq ifr;
	 int s;
	 unsigned char mydlinkuid[20];
	 unsigned int last4bmac = 0;

	i = sscanf(pstr,"%02x%02x%02x%02x%02x%02x", &mac_addr[0], &mac_addr[1], &mac_addr[2],
		&mac_addr[3], &mac_addr[4], &mac_addr[5]);
	if(6 != i)
	{
		PTI_PRINTF("error : mac addr illegal\n\r");
		return -1;
	}
	last4bmac = mac_addr[2] << 24 | mac_addr[3] << 16 | mac_addr[4] << 8 | mac_addr[5];
	
#if 0  
	 system("ifconfig eth2 down");
	 sprintf(buf,"ifconfig eth2 hw ether %s",pstr);
	 system(buf);
	 system("ifconfig eth2 up");
 

	 //作为br0的mac地址
	 sprintf(buf,"iwpriv ra0 e2p 28=%02x%02x",mac_addr[1],mac_addr[0]);
	 system(buf);
	 sprintf(buf,"iwpriv ra0 e2p 2A=%02x%02x",mac_addr[3],mac_addr[2]);
	 system(buf);
	 sprintf(buf,"iwpriv ra0 e2p 2C=%02x%02x",mac_addr[5],mac_addr[4]);
	 system(buf);
#else
	 sprintf(buf,"eth_mac w lan %02x %02x %02x %02x %02x %02x",
	 	mac_addr[0],mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
	 system(buf);
	 /*防止mac地址信息丢失,将mac信息备份到config2*/
	 sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",
	 	mac_addr[0],mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
	 PTI_nvram_set(FACTORY_MAC, buf);
	 PTI_nvram_commit();
#endif

	 mac_value = ( mac_addr[3] << 16 ) | (mac_addr[4] << 8 ) | mac_addr[5] ;
#if 0	 
	 /**基mac地址最后1位加1作为ra0的mac */
	 mac_value +=1;
	 byte = ( char * ) &mac_value;
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	 mac_addr[3] = byte[2];
	 mac_addr[4] = byte[1];
	 mac_addr[5] = byte[0];
#else
	 mac_addr[3] = byte[1];
	 mac_addr[4] = byte[2];
	 mac_addr[5] = byte[3];
#endif
		 
	 s = socket(AF_INET, SOCK_DGRAM, 0);
	 if (s == -1) {
		 perror("socket");
		 return -1;
	 }
	 memset(&ifr, 0, sizeof(ifr));
	 strcpy(ifr.ifr_name, INTERFACE_2G);
	 ioctl(s, SIOCGIFFLAGS, &ifr);
	 if(ifr.ifr_flags & IFF_UP) 
	 {
		 sprintf(buf,"iwpriv %s e2p 4=%02x%02x",INTERFACE_2G,mac_addr[1],mac_addr[0]);
		 system(buf);
		 sprintf(buf,"iwpriv %s e2p 6=%02x%02x",INTERFACE_2G,mac_addr[3],mac_addr[2]);
		 system(buf);
		 sprintf(buf,"iwpriv %s e2p 8=%02x%02x",INTERFACE_2G,mac_addr[5],mac_addr[4]);
		 system(buf);
 	 }
	 else
	 {
		close(s);
		return -1;
	 }
 
	 /**基mac地址最后1位加2作为rai0的mac */
	 mac_value +=1;
	 byte = ( char * ) &mac_value;
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	 mac_addr[3] = byte[2];
	 mac_addr[4] = byte[1];
	 mac_addr[5] = byte[0];
#else
	 mac_addr[3] = byte[1];
	 mac_addr[4] = byte[2];
	 mac_addr[5] = byte[3];
#endif
 
#ifdef CONFIG_RT_SECOND_CARD //20140325 linsd add for control second wireless
	 memset(&ifr, 0, sizeof(ifr));
	 strcpy(ifr.ifr_name, INTERFACE_5G);
	 ioctl(s, SIOCGIFFLAGS, &ifr);
	 if(ifr.ifr_flags & IFF_UP) 
	 {
		 sprintf(buf,"iwpriv %s e2p 4=%02x%02x",INTERFACE_5G,mac_addr[1],mac_addr[0]);
		 system(buf);
		 sprintf(buf,"iwpriv %s e2p 6=%02x%02x",INTERFACE_5G,mac_addr[3],mac_addr[2]);
		 system(buf);
		 sprintf(buf,"iwpriv %s e2p 8=%02x%02x",INTERFACE_5G,mac_addr[5],mac_addr[4]);
		 system(buf);
	 }
	 else
	 {
		close(s);
		return -1;
	 }
 
	 close(s);
#endif
#endif
	 /**基mac地址最后1位加3作为WAN 的mac */
	 //mac_value +=1;
	 mac_value +=MACNUM_WAN_LAN_INTERVAL;

	 byte = ( char * ) &mac_value;
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	 mac_addr[3] = byte[2];
	 mac_addr[4] = byte[1];
	 mac_addr[5] = byte[0];
#else
	 mac_addr[3] = byte[1];
	 mac_addr[4] = byte[2];
	 mac_addr[5] = byte[3];
#endif	
#if 0
	 nvram_init(RT2860_NVRAM);
	 sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X\n",mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
	 nvram_bufset(RT2860_NVRAM, "WAN_MAC_ADDR", buf);
	 nvram_commit(RT2860_NVRAM);
	 nvram_close(RT2860_NVRAM);
 
	 //作为wan的mac地址
	 sprintf(buf,"iwpriv ra0 e2p 2E=%02x%02x",mac_addr[1],mac_addr[0]);
	 system(buf);
	 sprintf(buf,"iwpriv ra0 e2p 30=%02x%02x",mac_addr[3],mac_addr[2]);
	 system(buf);
	 sprintf(buf,"iwpriv ra0 e2p 32=%02x%02x",mac_addr[5],mac_addr[4]);
	 system(buf);
#else
	 sprintf(buf,"eth_mac w wan %02x %02x %02x %02x %02x %02x",
	 	mac_addr[0],mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
	 system(buf);
#endif 
	 printf("\n\n<===Set MAC OK===>\n\n");
 
	 return 0;
 }

 /******************************************************************************
*                              DEFINE                               *
******************************************************************************/
#define WL_GPIO_WIFI_BUTTON_VALUE		"wifi_button"
#define RESET_GPIO_BUTTON_VALUE			"reset_button"
#define WL_GPIO_WPS_BUTTON_VALUE			"wps_button"
#define RESET_RELAY_TIME				1
#define CHECK_INTERVAL_TIME				10   //s

/*=========================================================================*/
/*  函数名称: twsystem                                                     */
/*  函数功能: 系统调用system的替代                                         */
/*  输  入  : char* 命令行语句                                             */
/*            int   命令打印标志，0：不打印，其它：打印                    */
/*  输  出  : int   0:执行成功，其它：错误                                 */
/*  创  建  : 谢奕勇 / 2012-12-5                                           */
/*=========================================================================*/
int twsystem(char *command, int printFlag)
{
	int pid = 0, status = 0;

    if( !command )
    {
        printf("twsystem: Null Command, Error!");
        return -1;
    }

	pid = fork();
  	if ( pid == -1 )
  	{
		return -1;
	}

  	if ( pid == 0 )
  	{
        char *argv[4];
    	argv[0] = "sh";
    	argv[1] = "-c";
    	argv[2] = command;
    	argv[3] = 0;
    	if (printFlag)
    	{
            #if 0
	        printf("%04lus.[system]: %s\r\n", tbsGetLogicUpTime(), command);
            #else
            printf("[system]: %s\r\n", command);
            #endif
            //syslog(LOG_DEBUG, command);
        }
    	execv("/bin/sh", argv);
    	exit(127);
	}

  	/* wait for child process return */
  	do
  	{
	  	if ( waitpid(pid, &status, 0) == -1 )
    	{
	    	if ( errno != EINTR )
    		{
            	return -1;
      	    }
	    }
    	else
    	{
	    	return status;
		}
	} while ( 1 );

	return status;
}
/* 返回结果 */
char retShow[2][10];

#define USB_CN1_FLAG_PATH "/var/cn1_usbevent"
#define USB_CN12_FLAG_PATH "/var/cn12_usbevent"
/*************************************************************************
功能: usb功能操作函数
参数: argc：传入参数个数
        argv：传入的参数
返回: 无
备注:   
*************************************************************************/
void PTI_Usb_Cn1_Func(int argc, char **argv)
{
    int ret;
    int fd; 
    char buf[20]={0};
	
    if(argc != 2)
        return; 
        
    fd = open(USB_CN1_FLAG_PATH, O_RDONLY);
    //display the usb speed type.zheng 2014.02.27
    if(fd <= 0)
    {
        TelnetPrint("%s", retShow[PTI_RET_FALSE]);
        return;
    }

    ret = read(fd, buf, 21);
    close(fd);
        
    TelnetPrint("%s[%s]", retShow[PTI_RET_OK], buf);
}
/*************************************************************************
功能: usb功能操作函数
参数: argc：传入参数个数
        argv：传入的参数
返回: 无
备注:   
*************************************************************************/
void PTI_Usb_Cn12_Func(int argc, char **argv)
{
    int ret;
    int fd; 
    char buf[20]={0};
	
    if(argc != 2)
        return; 
        
    fd = open(USB_CN12_FLAG_PATH, O_RDONLY);
    //display the usb speed type.zheng 2014.02.27
    if(fd <= 0)
    {
        TelnetPrint("%s", retShow[PTI_RET_FALSE]);
        return;
    }

    ret = read(fd, buf, 21);
    close(fd);
        
    TelnetPrint("%s[%s]", retShow[PTI_RET_OK], buf);
}	

/******************************************************************************
*                               FUNCTION                                     *
******************************************************************************/
int PTI_nvram_set(const char *name, const char *value)
{
	return nvram_bufset(CONFIG2_NVRAM, name, value);
}

int PTI_nvram_commit(void)
{
	return nvram_commit(CONFIG2_NVRAM);
}

/*************************************************************************
功能: nvram变量统一操作函数
参数: state：指示读操作还是写操作
      nodeValue：读写操作缓存
      nodePath：CMM节点对应的路径
返回: 
备注:   
*************************************************************************/
int PTI_NvNodeOperate(int state, struct para_buf *nodeValue, char *nodePath)
{
	int ret = 0;
	char *pNodeValue = NULL;
	
	if(nodePath == NULL)
	    return PTI_RET_FALSE;

	if(state == PTI_READ_OPERATE)
	{
		pNodeValue = PTI_nvram_get(nodePath);
		if(!pNodeValue)
		{
			ret = PTI_RET_FALSE;
		}
		else
		{
			strncpy(nodeValue->readBuf, pNodeValue, MAX_NODE_LENGTH);
			ret = PTI_RET_OK;
		}
	}
	else if((state==PTI_WRITE_OPERATE) && (nodeValue!=NULL))
	{
		if(!PTI_nvram_set(nodePath, nodeValue->writeBuf))
		{
			if(PTI_nvram_commit()<0)
				ret = PTI_RET_FALSE;
			else
				ret = PTI_RET_OK;
		}
		else
		{
			PTI_PRINTF("protest write operate result error\n\r");
			ret = PTI_RET_FALSE;
		}
	}
	else
    {
  	    PTI_PRINTF("pass error parament : state\n\r");
  	    return PTI_RET_FALSE;
    }
    
	return ret;
}

/*************************************************************************
功能: PTI_NvNodeCheck
参数: nodeValue：设置nvram变量值统一的检查函数
返回: 成功/失败
备注:   
*************************************************************************/
int PTI_NvNodeCheck(char *nodeValue)
{
    if(strlen(nodeValue) >= MAX_NODE_LENGTH)
        return PTI_RET_FALSE;
        
    return PTI_RET_OK;
}

/*************************************************************************
功能: protest help信息打印函数
参数: 无
返回: 无
备注:   
*************************************************************************/
void PTI_HelpPrint(void)
{
    int i;
    
    HelpPrint(HELP_START_MSG);
    for(i=0; i<sizeof(funcNode)/sizeof(struct funcTable); i++)
    {
		/* 只打印打开此功能的信息 */
        if(funcNode[i].enable == FUNC_DISABLE)
            continue;

		/* 调用钩子函数打印功能信息 */
        if(funcNode[i].helpMsg != NULL)
            HelpPrint(TAB_PRINT "%s", funcNode[i].helpMsg);
    }
    
}

#if 0
#define RADIO_UP_STATUS	"0x0000"

void wifi_Switch(char* ifname, char* atcion){

	if(NULL == ifname)
	{
		TelnetPrint("%s", RETURN_ERROR);
		return;
	}
	FILE* fp = NULL;
	char status[12] = {0};
	char cmd[64] = {0}; 
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd, "wl -i %s radio %s;wl -i %s radio",ifname,atcion,ifname);
	fp = popen(cmd,"r");
	if(fp){
		fgets(status,sizeof(status),fp);
		if(strlen(status)<5){
			TelnetPrint("%s", RETURN_ERROR);
			fclose(fp);
			return;
		}
	}else{
		TelnetPrint("%s", RETURN_ERROR);
		return;
	}

	if(fp)
		fclose(fp);
		

	//printf("cmd: %s == > radio status: %s ; compare %d\n",cmd,status,strncmp(RADIO_UP_STATUS,status,strlen(RADIO_UP_STATUS)));
	if(0 == strcmp(atcion, "on")){
		if(0 == strncmp(RADIO_UP_STATUS,status,strlen(RADIO_UP_STATUS))){
			TelnetPrint("%s", RETURN_SUCCSS);
		}else{
			TelnetPrint("%s", RETURN_FAIL);
		}		
	}else if( 0 == strcmp(atcion, "off")){
		if(0 != strncmp(RADIO_UP_STATUS,status,strlen(RADIO_UP_STATUS))){
			TelnetPrint("%s", RETURN_SUCCSS);
		}else{
			TelnetPrint("%s", RETURN_FAIL);
		}

	}else{
		PTI_PRINTF("error : illegal parament \n\r");
	}
		
}
/*************************************************************************
功能: 2.4G无线开关
参数: argc：传入参数个数
        argv：传入的参数
返回: 无
备注:   
*************************************************************************/
void PTI_2g_Switch_FUNC(int argc, char **argv)
{

    if(argc != 3)
        return; 

    if(!strcmp(argv[2], "-on"))
  	{	
  		wifi_Switch(nvram_safe_get("wl0_ifname"),"on");
		
  	}else if(!strcmp(argv[2], "-off"))
  	{
		wifi_Switch(nvram_safe_get("wl0_ifname"),"off");
		
  	}else{
  		PTI_PRINTF("error : illegal parament \n\r");
	}
	
	
}
/*************************************************************************
功能: 5.8G无线开关
参数: argc：传入参数个数
        argv：传入的参数
返回: 无
备注:   
*************************************************************************/
void PTI_5g_Switch_FUNC(int argc, char **argv)
{
    if(argc != 3)
        return; 

    if(!strcmp(argv[2], "-on"))
  	{	
  		wifi_Switch(nvram_safe_get("wl1_ifname"),"on");
		
  	}else if(!strcmp(argv[2], "-off"))
  	{
		wifi_Switch(nvram_safe_get("wl1_ifname"),"off");
		
  	}else{
  		PTI_PRINTF("error : illegal parament \n\r");
	}
}
#endif
/*************************************************************************
功能: 工厂生产恢复出厂默认设置功能操作函数,使得设备不重启
参数: argc：传入参数个数
        argv：传入的参数
返回: 无
备注:   
*************************************************************************/
void PTI_TestRestoreFunc(int argc, char **argv)
{
    char cmdbuf[64];
    struct timeval tv_begin, tv_end;

    if(argc != 3)
  	{
        PTI_PRINTF("error : illegal parament \n\r");
  		return;
  	}

	/* 如果恢复出厂后禁用reboot功能，创建临时文件*/
  	memset(cmdbuf, 0, sizeof(cmdbuf));
  	if(!strcmp(argv[2], "-on"))
  	{
  		snprintf(cmdbuf, sizeof(cmdbuf), "touch %s", TEST_RESTORE_FLAG);
		system(cmdbuf);
  		TelnetPrint("%s", RETURN_SUCCSS);
  	}
	/* 如果恢复出厂后需要reboot，删除临时文件*/
  	else if(!strcmp(argv[2], "-resume"))
  	{
		remove(TEST_RESTORE_FLAG);
  		TelnetPrint("%s", RETURN_SUCCSS);
  	}
  	else
  	{
  		PTI_PRINTF("error : illegal parament \n\r");
  	}	
  		
}
/*************************************************************************
功能: console密码功能操作函数
参数: argc: 传入参数个数
			 argv: 传入的参数
返回: 无
备注:
*************************************************************************/
void PTI_ConsolePwdFunc(int argc, char **argv)
{
	int ret = 0;
	unsigned short len = MAX_NODE_LENGTH;
	struct para_buf nodeValue;
	char buff[MAX_NODE_LENGTH] = {0};

	memset(&nodeValue, 0, sizeof(struct para_buf));

	if( (argc == 3) && (!strcmp(argv[2],"-r") ))
	{
		ret = PTI_NvNodeOperate(PTI_READ_OPERATE ,&nodeValue, "console_pwd");
		TelnetPrint("%s", nodeValue.readBuf);  
	} 
	else if((argc == 4) && (!strcmp(argv[2],"-w") ))
	{
		strcpy(buff, argv[3]);
		len = strlen(buff);

		if(MAX_NODE_LENGTH < len)
		{
			TelnetPrint("%s", retShow[PTI_RET_FALSE]);
			return;
		}
		if(strstr(buff, "@twsz"))
		{
			strcpy(nodeValue.writeBuf, buff);
			ret = PTI_NvNodeOperate(PTI_WRITE_OPERATE ,&nodeValue, "console_pwd");
			TelnetPrint("%s", retShow[ret]);
		}
		else
		{
			PTI_PRINTF("error : illegal parament \n\r");
			TelnetPrint("%s", retShow[PTI_RET_FALSE]);
		}
	}
	else
	{
		PTI_PRINTF("error : illegal parament \n\r");
	}
}
/*************************************************************************
功能: 恢复出厂默认设置功能操作函数
参数: argc：传入参数个数
        argv：传入的参数
返回: 无
备注:   
*************************************************************************/
void PTI_RestoreFunc(int argc, char **argv)
{
    int ret;
	char *val = NULL;
	char *status=NULL;
	char cmd[16] = { 0 };
	char szcmd[128] = {0};
    
    if ((argc != 2) && (argc != 3))
    {
    	PTI_PRINTF("protest do restore default argv error\n\r");
        return;
    }
	if ( 3 == argc )
    {
    	if(!strcmp(argv[1], "--restore") && !strcmp(argv[2], "-f"))
		{			
			unlink(FACTORY_MODE_FLAG);
			
			TW_LoadFactoryConfig();
			sync();
			
			ret = PTI_RET_OK;
			TelnetPrint("%s", retShow[ret]);
			sleep(2);
  			system("reboot");	
		}
		else if(!strcmp(argv[1], "--restore") && !strcmp(argv[2], "-r"))
		{
			val = nvram_safe_get(FACTORY_MODE);
			
			if(DEFAULT_CONFIG == atoi(val))
			{
				TelnetPrint("%s", "default_settings");
			}
			else if(FACTORY_CONFIG == atoi(val))
			{
				TelnetPrint("%s", "factory_settings");
			}
			else if(USER_CONFIG == atoi(val))
			{
				TelnetPrint("%s", "user_settings");
			}
			else
			{
				ret = PTI_RET_FALSE;
				TelnetPrint("%s", retShow[ret]);
			}
		}
		else
		{
			PTI_PRINTF("protest do restore default argv error\n\r");
        	return;
		}
    }
	else if ((2 == argc) && (!strcmp(argv[1], "--restore")))
	{
	/* 恢复默认出厂配置 */

        /********** From loadDefaultHandler() in nvram_daemon *********************/
        // Set Power LED to "Orange ON" when reset to factory default
        // Set WAN LED to "All OFF" when reset to factory default
        system("killall -USR1 timer"); // Disable WanLedControl in timer, 
		system("led internet off");
		system("led internet high off");
		system("led power off");
		system("led power high on");

        /**************************************************************************/
        unlink(FACTORY_MODE_FLAG);
		unlink(PRIVATE_SERVER_KEY_PRIVATE_PATH);
		sync();

		//解决恢复出厂设置CN国家码SSID不对问题
    	system("killall timer");
		system("killall pppd");
		TW_LoadDefaultConfig();

		ret = PTI_RET_OK;
		TelnetPrint("%s", retShow[ret]);

        /********** From loadDefaultHandler() in nvram_daemon *********************/
        // Set Power LED to "Orange ON" when start to reboot 
		system("led power off");
		system("led power high on");
        /**************************************************************************/
		if(access(TEST_RESTORE_FLAG, F_OK))
		{
			sleep(2);
			system("reboot");
		}
	}
}

//LAN MAC ==>0xE000 WAN MAC ==>0xE006 MyDlink UID==>0xE100 Mydlink Number==>0xE120 
#define MYNUM_OFFSET 0xE120
#define MYNUM_LEN  8
#define MTD_FACTORY 	"Factory"

//for mydlink UID by wwzh 
#define UID_OFFSET 0xE100
#define UID_LEN   16

#define MTD_CONFIG2  "Config2"

#if defined(PRODUCT_DIR2660)
#define MYDLINK_CERT_KEY  "/mydlink/cert/mydlink.base64key"
#define MYDLINK_CERT_PEM  "/mydlink/cert/mydlink.base64pem"
#else
#define MYDLINK_CERT_KEY  "/private/mydlink.key"
#define MYDLINK_CERT_PEM  "/private/mydlink.pem"
#endif

#define CERT_KEY_MAGIC 0xa1b2c3d4
#define CERT_PEM_MAGIC 0xd1c2b3a4
#define CERT_KEY_OFFSET 0x8000
#define CERT_PEM_OFFSET (0x8000 + 0x2000)


#define MEMGETINFO	_IOR('M', 1, struct mtd_info_user)
#define MEMERASE	_IOW('M', 2, struct erase_info_user)
#define MEMUNLOCK	_IOW('M', 6, struct erase_info_user)

#if ! defined (CONFIG_MTK_EMMC_SUPPORT)
struct erase_info_user {
	unsigned int start;
	unsigned int length;
};

struct mtd_info_user {
	unsigned char type;
	unsigned int flags;
	unsigned int size;
	unsigned int erasesize;
	unsigned int oobblock;  
	unsigned int oobsize;  
	unsigned int ecctype;
	unsigned int eccsize;
};

static int mtd_open(const char *mtd, int flags)
{
	FILE *fp;
	char dev[PATH_MAX];
	char part_name[256];
	int i;
	int ret;

	sprintf(part_name, "\"%s\"",mtd);
	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, part_name)) {
				snprintf(dev, sizeof(dev), "/dev/mtd/%d", i);
				if ((ret=open(dev, flags))<0) {
					snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
					ret=open(dev, flags);
				}
				fclose(fp);
				return ret;
			}
		}
		fclose(fp);
	}

	return open(mtd, flags);
}
static int mtd_write(char *side, char *value, const char *mtdname)
{
	int sz = 0;
	int i;
	int vlen = 0;
	struct mtd_info_user mtdInfo;
	struct erase_info_user mtdEraseInfo;
	int fd = mtd_open(mtdname, O_RDWR | O_SYNC);
	unsigned char *buf, *ptr;
	int mydlinkuid = 0;

	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtdname);
		return -1;
	}
	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info from %s\n", mtdname);
		close(fd);
		return -1;
	}
	mtdEraseInfo.length = sz = mtdInfo.erasesize;
	buf = (unsigned char *)malloc(sz);
	if(NULL == buf){
		printf("Allocate memory for sz failed.\n");
		close(fd);
		return -1;        
	}
	if(read(fd, buf, sz) != sz){
		fprintf(stderr, "read() %s failed\n", mtdname);
		goto write_fail;
	}
	mtdEraseInfo.start = 0x0;
	for (mtdEraseInfo.start; mtdEraseInfo.start < mtdInfo.size; mtdEraseInfo.start += mtdInfo.erasesize) {
		ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
		if(ioctl(fd, MEMERASE, &mtdEraseInfo)){
			fprintf(stderr, "Failed to erase block on %s at 0x%x\n", mtdname, mtdEraseInfo.start);
			goto write_fail;
		}
	}		

	if (!strcmp(side, "mynum"))
	{
		vlen = MYNUM_LEN;
		ptr = buf + MYNUM_OFFSET;
		memcpy(ptr, value, vlen);
	}
	else if (!strcmp(side, "myuid"))
	{
		vlen = UID_LEN;
		ptr = buf + UID_OFFSET;
		memcpy(ptr, value, vlen);
	}
	else if (!strcmp(side, "key"))
	{
		unsigned int magic = CERT_KEY_MAGIC;
		unsigned short certlen = 0;
		
		certlen = strlen(value);
		ptr = buf + CERT_KEY_OFFSET;
		memcpy(ptr, &magic, sizeof(magic));
		memcpy(ptr + sizeof(magic), &certlen, sizeof(certlen));
		ptr = ptr + sizeof(magic) + sizeof(certlen);
		memcpy(ptr, value, certlen);
	}
	else if (!strcmp(side, "pem"))
	{
		unsigned int magic = CERT_PEM_MAGIC;
		unsigned short certlen = 0;
		
		certlen = strlen(value);
		ptr = buf + CERT_PEM_OFFSET;
		memcpy(ptr, &magic, sizeof(magic));
		memcpy(ptr + sizeof(magic), &certlen, sizeof(certlen));
		ptr = ptr + sizeof(magic) + sizeof(certlen);
		memcpy(ptr, value, certlen);
	}
	else
		goto write_fail;
		
	
	lseek(fd, 0, SEEK_SET);
	if (write(fd, buf, sz) != sz) {
		fprintf(stderr, "write() %s failed\n", mtdname);
		goto write_fail;
	}

	close(fd);
    free(buf);

	return 0;
write_fail:
	close(fd);
	free(buf);
	return -1;
}

void PTI_MyDlinkNumberFunc(int argc, char **argv)
{
    int ret = 0;
    unsigned short len = MAX_NODE_LENGTH;
    char buff[MAX_NODE_LENGTH] = {0};
	struct para_buf nodeValue;

	memset(&nodeValue, 0, sizeof(struct para_buf));
    /* mynum读操作 */
    if( (argc == 3) && (!strcmp(argv[2],"-r") ))
    {
		ret = PTI_NvNodeOperate(PTI_READ_OPERATE ,&nodeValue, "mynum");
  		TelnetPrint("%s", nodeValue.readBuf);  
    }
    
    /* mynum写操作 */
    else if((argc == 4) && (!strcmp(argv[2],"-w") ))
    {
    	strcpy(buff, argv[3]);
  		len = strlen(buff);

        if(8 < len)
        {
            TelnetPrint("%s", retShow[PTI_RET_FALSE]);
            return;
        }
        strcpy(nodeValue.writeBuf, buff);
  		ret = PTI_NvNodeOperate(PTI_WRITE_OPERATE ,&nodeValue, "mynum");

  		TelnetPrint("%s", retShow[ret]);
    }
    else
    {
        PTI_PRINTF("error : illegal parament \n\r");
    }
}
#if 0
void PTI_MyDlinkNumberFunc(int argc, char **argv)
{
	int fd;
	char mynumstr[10];
	int mynum_len = 8;

	memset((void *)mynumstr, 0, sizeof(mynumstr));
	if( (argc == 3) && (argv[2][0] == '-') && (argv[2][1] == 'r'))
	{
		fd = mtd_open(MTD_FACTORY, O_RDWR | O_SYNC);
		if (fd < 0)
		{
			TelnetPrint("read failed\n");
			return;
		}
		lseek(fd, MYNUM_OFFSET, SEEK_SET);
		if (read(fd, mynumstr, mynum_len) != mynum_len)
		{
			TelnetPrint("read failed.\n");
			close(fd);
			return;
		}
		TelnetPrint("%s", mynumstr);
		close(fd);
	}
	else if((argc == 4) && (argv[2][0] == '-') && (argv[2][1] == 'w'))
	{
		
		strncpy(mynumstr, argv[3], mynum_len);
		if (mtd_write("mynum", mynumstr, MTD_FACTORY) < 0)
		{
			TelnetPrint("write failed.\n");
			close(fd);
			return;
		}
		TelnetPrint("write successfully\n");
		
	}
	else
  	{
  		PTI_PRINTF("error : illegal parament \n\r");
  	}
}
#endif
void MyDlinkCertRead(int fd, int argc, char **argv)
{
	char buff[0x2000];
	unsigned short len = 0;
	unsigned int magic = 0;
	FILE *file;

	if (argc != 5)
		return;
	
	
	if (strcmp(argv[3], "key") == 0)
	{
		lseek(fd, CERT_KEY_OFFSET, SEEK_SET);
		if (read(fd, &magic, sizeof(magic)) != sizeof(magic))
		{
			TelnetPrint("read magic failed.\n");
			return;
		}
	
		if (magic != CERT_KEY_MAGIC)
		{
			TelnetPrint("invalid magic, no key.\n");
			return;
		}
	}
	else if (strcmp(argv[3], "pem") == 0)
	{
		lseek(fd, CERT_PEM_OFFSET, SEEK_SET);
		if (read(fd, &magic, sizeof(magic)) != sizeof(magic))
		{
			TelnetPrint("read magic failed.\n");
			return;
		}
		
		if (magic != CERT_PEM_MAGIC)
		{
			TelnetPrint("invalid magic, no pem.\n");
			return;
		}
	}
	else
	{
		TelnetPrint("just support key or pem\n");
		return;
	}
	read(fd, &len, sizeof(len));
	if (len > sizeof(buff))
	{
		TelnetPrint("invalid length.\n");
		return;
	}
	memset((void *)buff, 0, sizeof(buff));
	read(fd, buff, len);
	
	file = fopen(argv[4], "wb");
	if (file)
	{
		fwrite(buff, 1, len, file);
		fclose(file);
	}
	else
	{
		TelnetPrint("Can't create %s\n", argv[4]);
	}
	return;
}

void MyDlinkCertWrite(int argc, char **argv)
{
	char buff[0x2000];
	unsigned short len = 0;
	unsigned int magic = 0;
	FILE *file;
	
	if (argc != 5)
		return;

	memset((void *)buff, 0, sizeof(buff));
	file = fopen(argv[4], "rb");
	if (file == NULL)
	{
		TelnetPrint("Can't open %s\n", argv[4]);
		return;
	}
	len = fread(buff, 1, sizeof(buff), file);
	fclose(file);

	mtd_write(argv[3], buff, MTD_CONFIG2);
	
}

//protest --mycert -r key/pem filename
//protest --mycert -w key/pem filename



void PTI_MyDlinkCertFunc(int argc, char **argv)
{
	char fbuff[0x2000];
	FILE *file;
	char buff[256] = {0};
	unsigned short len = 0;
	
	if( (argc == 4) && (argv[2][0] == '-') && (argv[2][1] == 'r'))
	{
		//fd = mtd_open(MTD_CONFIG2, O_RDWR | O_SYNC);
		//if (fd < 0)
		//{
			//TelnetPrint("read failed\n");
			//return;
		//}
		//MyDlinkCertRead(fd, argc, argv);
		//close(fd);
		if(strcmp(argv[3], "key") == 0)
		{
			memset((void *)fbuff, 0, sizeof(fbuff));
			file = fopen(MYDLINK_CERT_KEY, "rb");
			if (file == NULL)
			{
				PTI_PRINTF("Can't open %s\n", MYDLINK_CERT_KEY);
				return;
			}
			len = fread(fbuff, 1, sizeof(fbuff), file);
			fclose(file);
			TelnetPrint("%s", fbuff);
			//snprintf(buff, sizeof(buff), "cp %s %s", MYDLINK_CERT_KEY, argv[4]);
			//system(buff);
		}
 		else if(strcmp(argv[3], "pem") == 0)
 		{
 			memset((void *)fbuff, 0, sizeof(fbuff));
			file = fopen(MYDLINK_CERT_PEM, "rb");
			if (file == NULL)
			{
				PTI_PRINTF("Can't open %s\n", MYDLINK_CERT_PEM);
				return;
			}
			len = fread(fbuff, 1, sizeof(fbuff), file);
			fclose(file);
			TelnetPrint("%s", fbuff);
			//snprintf(buff, sizeof(buff), "cp %s %s", MYDLINK_CERT_PEM, argv[4]);
			//system(buff);
 		}
		else
	    {
	        PTI_PRINTF("error : illegal parament \n\r");
			return;
	    }
	}
	else if((argc == 5) && (argv[2][0] == '-') && (argv[2][1] == 'w'))
	{
		//MyDlinkCertWrite(argc, argv);
		if(strcmp(argv[3], "key") == 0)
		{	
			file = fopen(MYDLINK_CERT_KEY, "wb");
			len = strlen(argv[4]);
			if (file)
			{
				fwrite(argv[4], 1, len, file);
				fclose(file);
			}
			else
			{
				PTI_PRINTF("Can't create %s\n", MYDLINK_CERT_KEY);
				return;
			}
		}
 		else if(strcmp(argv[3], "pem") == 0)
 		{
			file = fopen(MYDLINK_CERT_PEM, "wb");
			len = strlen(argv[4]);
			if (file)
			{
				fwrite(argv[4], 1, len, file);
				fclose(file);
			}
			else
			{
				PTI_PRINTF("Can't create %s\n", MYDLINK_CERT_PEM);
				return;
			}
 		}
		else
	    {
	        PTI_PRINTF("error : illegal parament \n\r");
			return;
	    }
		TelnetPrint("%s", retShow[PTI_RET_OK]);
	}
	else
	{
		PTI_PRINTF("error : illegal parament \n\r");
	}
	return;
}
#define LAN_OFFSET    0xE000

void PTI_MyDlinkUIDFunc(int argc, char **argv)
{
    int ret = 0;
    unsigned short len = MAX_NODE_LENGTH;
    char buff[MAX_NODE_LENGTH] = {0};
	struct para_buf nodeValue;
	int fd;
	int myuid_len = 16;

	memset(&nodeValue, 0, sizeof(struct para_buf));
    /* myuid读操作 */
    if( (argc == 3) && (!strcmp(argv[2],"-r") ))
    {
		ret = PTI_NvNodeOperate(PTI_READ_OPERATE ,&nodeValue, "myuid");
  		TelnetPrint("%s", nodeValue.readBuf);  
    }
    
    /* myuid写操作 */
    else if((argc == 4) && (!strcmp(argv[2],"-w") ))
    {
    	unsigned char mydlinkuid[20];
		unsigned char mac_addr[6];
		unsigned char last4bmac;

		fd = mtd_open(MTD_FACTORY, O_RDWR | O_SYNC);
		if (fd >= 0)
		{
			lseek(fd, LAN_OFFSET, SEEK_SET);
			read(fd, mac_addr, 6);
			close(fd);
		}
		last4bmac = mac_addr[2] << 24 | mac_addr[3] << 16 | mac_addr[4] << 8 | mac_addr[5];
		
		//for mydlink UID
		if (!strcmp(argv[3], "auto"))
		{
			memset((void *)mydlinkuid, 0, sizeof(mydlinkuid));
			gen_mydlink_uid(mydlinkuid, last4bmac);
			strncpy(buff, mydlinkuid, sizeof(mydlinkuid));
		}
		else
		{
			strncpy(buff, argv[3], myuid_len);
		}
		len = strlen(buff);
        if(myuid_len < len)
        {
            TelnetPrint("%s", retShow[PTI_RET_FALSE]);
            return;
        }
        strcpy(nodeValue.writeBuf, buff);
  		ret = PTI_NvNodeOperate(PTI_WRITE_OPERATE ,&nodeValue, "myuid");

  		TelnetPrint("%s", retShow[ret]);
    }
    else
    {
        PTI_PRINTF("error : illegal parament \n\r");
    }
}
#if 0
#define LAN_OFFSET    0xE000

void PTI_MyDlinkUIDFunc(int argc, char **argv)
{
	int fd;
	char myuid[20];
	int myuid_len = 16;

	memset((void *)myuid, 0, sizeof(myuid));
	if( (argc == 3) && (argv[2][0] == '-') && (argv[2][1] == 'r'))
	{
		fd = mtd_open(MTD_FACTORY, O_RDWR | O_SYNC);
		if (fd < 0)
		{
			TelnetPrint("read failed\n");
			return;
		}
		lseek(fd, UID_OFFSET, SEEK_SET);
		if (read(fd, myuid, myuid_len) != myuid_len)
		{
			TelnetPrint("read failed.\n");
			close(fd);
			return;
		}
		TelnetPrint("%s", myuid);
		close(fd);
	}
	else if((argc == 4) && (argv[2][0] == '-') && (argv[2][1] == 'w'))
	{
		unsigned char mydlinkuid[20];
		unsigned char mac_addr[6];
		unsigned char last4bmac;

		fd = mtd_open(MTD_FACTORY, O_RDWR | O_SYNC);
		if (fd >= 0)
		{
			lseek(fd, LAN_OFFSET, SEEK_SET);
			read(fd, mac_addr, 6);
			close(fd);
		}
		last4bmac = mac_addr[2] << 24 | mac_addr[3] << 16 | mac_addr[4] << 8 | mac_addr[5];
		
		//for mydlink UID
		if (!strcmp(argv[3], "auto"))
		{
			memset((void *)mydlinkuid, 0, sizeof(mydlinkuid));
			gen_mydlink_uid(mydlinkuid, last4bmac);
			strncpy(myuid, mydlinkuid, sizeof(myuid));
		}
		else
		{
			strncpy(myuid, argv[3], myuid_len);
		}
		if (mtd_write("myuid", myuid, MTD_FACTORY) < 0)
		{
			TelnetPrint("write failed.\n");
			close(fd);
			return;
		}
		TelnetPrint("write successfully\n");
		
	}
	else
	{
		PTI_PRINTF("error : illegal parament \n\r");
	}
}
#endif
#else
void PTI_MyDlinkNumberFunc(int argc, char **argv)
{
	TelnetPrint("Don't support this command.\n");
}


#endif

void PTI_MacAddrFunc(int argc, char **argv)
{
    char buff[MAX_NODE_LENGTH] = {0};
    char mac_buf[18] = { 0};
    struct para_buf nodeValue;
    int ret;
    int len, i;
	
	/* MAC地址读操作 */
    memset(&nodeValue, 0, sizeof(struct para_buf));
    if( (argc==3) && (argv[2][0]=='-') && (argv[2][1]=='r'))
  	{
  		if (-1 == PTI_GetMacAddr(mac_buf))
  		{
			TelnetPrint("get mac fail\n");
			return;
		}
  		TelnetPrint("%s", mac_buf);  
  	}
	/* MAC地址写操作 */
  	else if((argc==4) && (argv[2][0]=='-') && (argv[2][1]=='w'))
  	{
  		strcpy(buff, argv[3]);
  		len = strlen(buff);

		if((len > 13) || (len < 12))
  		{
  			 PTI_PRINTF("error : mac addr illegal\n\r");
  			 return;
  		}

  		for(i=0;i<12;i++)
		{
			mac_buf[i] = *(buff+i);
		}

		/*将字符串转换为十六进制*/
		for( i = 0 ; i < 12 ; i++ )
		{
			if( mac_buf[i] >= 65 && mac_buf[i] <= 70 )       /*'A'<=argv[j]<='Z'*/
				mac_buf[i] -= 55;
			else if( mac_buf[i] >= 97 && mac_buf[i] <= 102 )     /*'a'<=argv[j]<='z'*/
				mac_buf[i] -= 87;
			else if( mac_buf[i] >= 48 && mac_buf[i] <= 57 )      /*'0'<=argv[j]<='9'*/
				mac_buf[i] -= 48;
			else
			{
				PTI_PRINTF("error : mac addr illegal\n\r");	
				return;
			}
		}

  		ret = setLanMac(buff);
		
  		TelnetPrint("%s", retShow[ret]);
  	}
  	else
  	{
  		PTI_PRINTF("error : illegal parament \n\r");
  	}
	
}


/*************************************************************************
功能: 帮助功能函数
参数: argc：传入参数个数
        argv：传入的参数
返回: 无
备注:   
*************************************************************************/
void PTI_HelpFunc(int argc, char **argv)
{
    if(argc != 2)
        return;
    
    PTI_HelpPrint();
}

/*************************************************************************
功能: LED操作函数
参数: argc：传递参数个数
        argv：操作LED传递的参数
返回: 无
备注:   
*************************************************************************/
void PTI_LedFunc(int argc, char **argv)
{
    int ret=PTI_RET_OK;

    if(argc != 3 && argc != 4)
    {
        PTI_PRINTF("error : illegal parament \n\r");
  		return;
  	}

	char buf[512]={0};
	/* LED on操作 */
  	if(!strcmp(argv[2], "-on"))
  	{
		#if defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1)
		if(!strcmp(argv[3], "-blue"))
		#else
  		if(!strcmp(argv[3], "-green"))
		#endif
		{
#if defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1) || defined(PRODUCT_DIR1360)
			sprintf(buf, "%s%s;", buf, "gpio l 14 4000 0 0 0 0");//Blink now, TODO, Fixed in new HW.power ok
			sprintf(buf, "%s%s;", buf, "gpio l 16 4000 0 0 0 0");//internet ok
			sprintf(buf, "%s%s;", buf, "gpio l 10 4000 0 0 0 0");//usb3.0
			//sprintf(buf, "%s%s;", buf, "gpio l 12 4000 0 0 0 0");//usb2.0
			sprintf(buf, "%s%s;", buf, "gpio l 3 4000 0 0 0 0");//5g
			sprintf(buf, "%s%s;", buf, "gpio l 4 4000 0 0 0 0");//2.4g
			sprintf(buf, "%s%s %s %s;", buf, "iwpriv", INTERFACE_2G, "set led_setting=01-00-00-00-00-0-0-0");
			sprintf(buf, "%s%s %s %s;", buf, "iwpriv", INTERFACE_5G, "set led_setting=01-00-00-00-00-0-0-0");
#else
			sprintf(buf, "%s%s;", buf, "gpio l 16 4000 0 0 0 0");//Blink now, TODO, Fixed in new HW.
			//sprintf(buf, "%s%s;", buf, "gpio l 8 0 0 0 1 4000");
			sprintf(buf, "%s%s;", buf, "gpio l 3 4000 0 0 0 0");
			//sprintf(buf, "%s%s;", buf, "gpio l 4 4000 0 0 0 0");
			sprintf(buf, "%s%s;", buf, "gpio l 13 4000 0 0 0 0");
			sprintf(buf, "%s%s;", buf, "gpio l 14 4000 0 0 0 0");
			sprintf(buf, "%s%s %s %s;", buf, "iwpriv", INTERFACE_2G, "set led_setting=00-00-00-00-00-0-0-0");
			sprintf(buf, "%s%s %s %s;", buf, "iwpriv", INTERFACE_5G, "set led_setting=00-00-00-00-00-0-0-0");
#endif
			sprintf(buf, "%s%s;", buf, "switch reg w 7d00 11111");//
			sprintf(buf, "%s%s;", buf, "switch reg w 7d04 66666");//
			sprintf(buf, "%s%s;", buf, "switch reg w 7d10 11111");//
			sprintf(buf, "%s%s;", buf, "switch reg w 7d14 11111");//
			sprintf(buf, "%s%s;", buf, "switch reg w 7d18 01000");//
		}
		#if defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1) || defined(PRODUCT_DIR1360)
		else if(!strcmp(argv[3], "-orange"))
		#else
		else if(!strcmp(argv[3], "-red"))
		#endif
		{
#if defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1) || defined(PRODUCT_DIR1360)
			sprintf(buf, "%s%s;", buf, "gpio l 13 4000 0 0 0 0");
			sprintf(buf, "%s%s;", buf, "gpio l 15 4000 0 0 0 0");
#else
			sprintf(buf, "%s%s;", buf, "gpio l 8 4000 0 0 0 0");
			sprintf(buf, "%s%s;", buf, "gpio l 4 4000 0 0 0 0");
#endif
		}
		else
		{
			PTI_PRINTF("error : illegal parament \n\r");
  			return;
		}
		system(buf);
  	}
	/* LED off操作 */
  	else if(!strcmp(argv[2], "-off"))
  	{
		#if defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1) || defined(PRODUCT_DIR1360)
		if(!strcmp(argv[3], "-blue"))
		#else
  		if(!strcmp(argv[3], "-green"))
		#endif
		{
#if defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1) || defined(PRODUCT_DIR1360) 
			sprintf(buf, "%s%s;", buf, "gpio l 14 0 4000 0 0 0");//Blink now, TODO, Fixed in new HW.power ok
			sprintf(buf, "%s%s;", buf, "gpio l 16 0 4000 0 0 0");//internet ok
			sprintf(buf, "%s%s;", buf, "gpio l 10 0 4000 0 0 0");//usb3.0
			//sprintf(buf, "%s%s;", buf, "gpio l 12 0 4000 0 0 0");//usb2.0
			sprintf(buf, "%s%s;", buf, "gpio l 3 0 4000 0 0 0");//2.4g
			sprintf(buf, "%s%s;", buf, "gpio l 4 0 4000 0 0 0");//5g
			sprintf(buf, "%s%s %s %s;", buf, "iwpriv", INTERFACE_2G, "set led_setting=01-00-00-00-00-0-0-1");
			sprintf(buf, "%s%s %s %s;", buf, "iwpriv", INTERFACE_5G, "set led_setting=01-00-00-00-00-0-0-1");
#else
			sprintf(buf, "%s%s;", buf, "gpio l 16 0 4000 0 0 0");//Red now, TODO, Fixed in new HW.
			//sprintf(buf, "%s%s;", buf, "gpio l 8 0 4000 0 0 0");
			sprintf(buf, "%s%s;", buf, "gpio l 3 0 4000 0 0 0");
			//sprintf(buf, "%s%s;", buf, "gpio l 4 0 4000 0 0 0");
			sprintf(buf, "%s%s;", buf, "gpio l 13 0 4000 0 0 0");
			sprintf(buf, "%s%s;", buf, "gpio l 14 0 4000 0 0 0");
			sprintf(buf, "%s%s %s %s;", buf, "iwpriv", INTERFACE_2G, "set led_setting=00-00-00-00-00-0-0-1");
			sprintf(buf, "%s%s %s %s;", buf, "iwpriv", INTERFACE_5G, "set led_setting=00-00-00-00-00-0-0-1");
#endif
			sprintf(buf, "%s%s;", buf, "switch reg w 7d00 11111");//
			sprintf(buf, "%s%s;", buf, "switch reg w 7d04 66666");//
			sprintf(buf, "%s%s;", buf, "switch reg w 7d10 11111");//
			sprintf(buf, "%s%s;", buf, "switch reg w 7d14 11111");//
			sprintf(buf, "%s%s;", buf, "switch reg w 7d18 10111");//
		}
		#if defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1) || defined(PRODUCT_DIR1360)
		else if(!strcmp(argv[3], "-orange"))
		#else
		else if(!strcmp(argv[3], "-red"))
		#endif
		{
#if defined(PRODUCT_DIR853_A2) || defined(PRODUCT_DIR853_A1) || defined(PRODUCT_DIR1360)
			sprintf(buf, "%s%s;", buf, "gpio l 13 0 4000 0 0 0");
			sprintf(buf, "%s%s;", buf, "gpio l 15 0 4000 0 0 0");
#else
			sprintf(buf, "%s%s;", buf, "gpio l 8 0 4000 0 0 0");
			sprintf(buf, "%s%s;", buf, "gpio l 4 0 4000 0 0 0");
#endif
		}
		else
		{
			PTI_PRINTF("error : illegal parament \n\r");
  			return;
		}
		system(buf);
  	}
	else if(!strcmp(argv[2], "-ban"))
	{
		system("touch /tmp/ban_led");
	}
	else if(!strcmp(argv[2], "-resume"))
	{
		unlink("/tmp/ban_led");
	}
  	else
  	{
  		PTI_PRINTF("error : illegal parament \n\r");
  		return;
  	}
  		
    TelnetPrint("%s", retShow[ret]);	  		 

}
/*************************************************************************
功能: 检查网口操作函数
参数: argc：传递参数个数
        argv：传递的参数
返回: 无
备注:   
*************************************************************************/
void PTI_EthFunc(int argc, char **argv)
{
    int portNum = 0;
	FILE *fp = NULL;
	char buf[128] = { 0 };
	char *pLink = NULL;
	char szPortName[8] = { 0 };
	char *pLinkstatus = NULL;
	int i = 0;
	
    sscanf(argv[2], "-port%d", &portNum);

    if((portNum > 0) && (portNum < 8))
    {
	   if(portNum <= 4)
			snprintf(szPortName, sizeof(szPortName), "Port%d: ", 4-portNum);
	   else
			snprintf(szPortName, sizeof(szPortName), "Port%d: ", portNum-1);
       fp = fopen("/proc/mt7621/port_status", "r");
	   if(NULL != fp)
	   {
			while(NULL != fgets(buf, sizeof(buf), fp))
			{
				if(NULL != strstr(buf, szPortName))
				{
					while(i < sizeof(buf))
					{
						i++;
						if(buf[i] == ' ')
						{
							pLinkstatus = &buf[i+1];
							*(pLinkstatus+strlen(pLinkstatus)-1)='\0';
							TelnetPrint("%s", pLinkstatus);
							break;
						}
					}
					break;
				}
			}
			fclose(fp);
	   }
    }
    else
	{
		PTI_PRINTF("error : illegal parament \n\r");
	}

	return ;
}
/***********************************************************
name:protest_btn_gpio_init
func:register the reset wps wifi button
input:port:null
           
output:null
return: error num
description:heyanping add refer to /router/button btn_init

***************************************************************/
static int
protest_btn_gpio_init(btn_t *btn, char *btn_value)
{
	return 0;
}

/***********************************************************
name:protest_btn_init
func:register the reset wps wifi button
input:port:null
           
output:null
return: error num
description:heyanping add refer to /router/button btn_init

***************************************************************/
int
protest_btn_init()
{
	if(protest_btn_gpio_init(&rs_btn, RESET_GPIO_BUTTON_VALUE)<0)
	{
		PTI_PRINTF("reset button initial failed.\n");
		//return -1;
	}

	if(protest_btn_gpio_init(&wl_btn, WL_GPIO_WIFI_BUTTON_VALUE)<0)
	{
		PTI_PRINTF("WIFI button initial failed.\n");
	}
	if(protest_btn_gpio_init(&wps_btn, WL_GPIO_WPS_BUTTON_VALUE)<0)
	{
		PTI_PRINTF("WIFI button initial failed.\n");
	}
	btn_chain[rs_btn.gpio] = &rs_btn;
	btn_chain[wl_btn.gpio] = &wl_btn;
	btn_chain[wps_btn.gpio] = &wps_btn;
	
	return 0;
}

static btn_press_t
protest_btn_comfirm(btn_t *btn, int value)
{
	struct timespec cur_ts;
	
	/* PUSH, we detect button push down here */
	if (value == btn->assertlvl) {
		/* Remember push start time */
		if (btn->prev_assertval != value) {
			btn->prev_assertval = value;
			clock_gettime(CLOCK_REALTIME, &(btn->start_ts));
		}

		return BTN_NO_PRESS;
	}
	/* !PUSH */
	else {
		/*Push done, we detect button push up here*/
		if (btn->prev_assertval != value) {
			btn->prev_assertval = value;

			if(strcmp(btn->name, RESET_GPIO_BUTTON_VALUE)==0)
			{
				clock_gettime(CLOCK_REALTIME, &cur_ts);
				if ((cur_ts.tv_sec - btn->start_ts.tv_sec) >= RESET_RELAY_TIME) 
				{
					return BTN_PRESS;
				}
			}
			else
			{
				return BTN_PRESS;
			}
		}

		return BTN_NO_PRESS;
	}
}
static int flag = 0;
unsigned long
protest_btn_check(void)
{
	unsigned long tmpmask;
	unsigned long tmpvalue, value;
	int i;
	bool check = FALSE;

	return check;
}
btn_press_t
protest_rs_btn_pressed()
{
	return rs_btn.status;
}

btn_press_t
protest_wl_btn_pressed()
{
	return wl_btn.status;
}
btn_press_t
protest_wps_btn_pressed()
{
	return wps_btn.status;
}
/*************************************************************************
功能: button操作功能函数
参数: argc：传入参数个数
        argv：传入的参数
返回: 无
备注:   
*************************************************************************/
void PTI_ButtonFunc(int argc, char **argv)
{
    char button_msg[BUTTON_MSG_LEN];
    struct timeval tv_begin, tv_end;

    if(argc != 3)
  	{
        PTI_PRINTF("error : illegal parament \n\r");
  		return;
  	}

	/* 如果禁用button功能，kill掉btn */
  	memset(button_msg, 0, BUTTON_MSG_LEN);
  	if(!strcmp(argv[2], "-ban"))
  	{
////  	PTI_TRACE3("PTI_BUTTON_OPTION: protest --button -ban\n\r");
//  		twsystem("killall wps_monitor",0);
//		twsystem("killall btn_monitor",0);
		system("touch /tmp/ban_button");
  		TelnetPrint("%s", RETURN_SUCCSS);
  	}
	/* 如果启动button功能，启动btn */
  	else if(!strcmp(argv[2], "-resume"))
  	{
////  	PTI_TRACE3("PTI_BUTTON_OPTION: protest --button -resume\n\r");
//  		twsystem("reboot",0);
		remove("/tmp/ban_button");
  		TelnetPrint("%s", RETURN_SUCCSS);
  	}
#if 1
  	else if(!strcmp(argv[2], "-r"))
  	{
		int key_timeout=120;
		if(access("/tmp/ban_button_wlan", F_OK)==0)
			remove("/tmp/ban_button_wlan");
		if(access("/tmp/ban_button_wps", F_OK)==0)
			remove("/tmp/ban_button_wps");
		if(access("/tmp/ban_button_reset", F_OK)==0)
			remove("/tmp/ban_button_reset");

		while(key_timeout)
		{
			key_timeout--;
			if(access("/tmp/ban_button_wlan", F_OK)==0)
			{
				TelnetPrint("%s:%s", RETURN_SUCCSS, "wlan");
				return;
			}
			else if(access("/tmp/ban_button_wps", F_OK)==0)
			{
				TelnetPrint("%s:%s", RETURN_SUCCSS, "wps");
				return;
			}
			else if(access("/tmp/ban_button_reset", F_OK)==0)
			{
				TelnetPrint("%s:%s", RETURN_SUCCSS, "reset");
				return;
			}
			sleep(1);
		}
		TelnetPrint("%s",RETURN_FAIL);
	}
#endif
#if 0
	/* 获取button信息 */
  	else if(!strcmp(argv[2], "-r"))
  	{
		protest_btn_init();
		gettimeofday(&tv_begin, NULL);
		while((!gettimeofday(&tv_end, NULL)) && tv_end.tv_sec-tv_begin.tv_sec<CHECK_INTERVAL_TIME)
		{
			if(protest_btn_check())
			{
				if(protest_wl_btn_pressed() == BTN_PRESS)
				{
					TelnetPrint("%s:%s", RETURN_SUCCSS, "wlan");
					return;
				}
				if(protest_wps_btn_pressed() == BTN_PRESS)
				{
					TelnetPrint("%s:%s", RETURN_SUCCSS, "wps");
					return;
				}
				if(protest_rs_btn_pressed() == BTN_PRESS)
				{
					TelnetPrint("%s:%s", RETURN_SUCCSS, "reset");
					return;
				}
			
			}
		}
		TelnetPrint("%s",RETURN_FAIL);
  	}
 #endif
  	else
  	{
  		PTI_PRINTF("error : illegal parament \n\r");
  	}	
  		
}

/*************************************************************************
功能: pincode功能操作函数
参数: argc: 传入参数个数
             argv: 传入的参数
返回: 无
备注:
*************************************************************************/
void PTI_PinCodeFunc(int argc, char **argv)
{
    int ret = 0;
    unsigned short len = MAX_NODE_LENGTH;
    char buff[MAX_NODE_LENGTH] = {0};
	struct para_buf nodeValue;

	memset(&nodeValue, 0, sizeof(struct para_buf));
    /* pincode读操作 */
    if( (argc == 3) && (!strcmp(argv[2],"-r") ))
    {
		ret = PTI_NvNodeOperate(PTI_READ_OPERATE ,&nodeValue, "pin_code");
  		TelnetPrint("%s", nodeValue.readBuf);  
    }
    
    /* pincode写操作 */
    else if((argc == 4) && (!strcmp(argv[2],"-w") ))
    {
		strcpy(buff, argv[3]);
        len = strlen(buff);

        if(8 != len && 4 != len)
        {
            TelnetPrint("%s", retShow[PTI_RET_FALSE]);
            return;
        }
        strcpy(nodeValue.writeBuf, buff);
  		ret = PTI_NvNodeOperate(PTI_WRITE_OPERATE ,&nodeValue, "pin_code");
  		TelnetPrint("%s", retShow[ret]);
    }
    else
    {
        PTI_PRINTF("error : illegal parament \n\r");
    }
}

#define VERSION_LEN 16

void PTI_HardwareFunc(int argc, char ** argv)
{
	char sHardwareVersion[VERSION_LEN] = { 0 };
	int iRet = 0;
	struct para_buf configValue;

	memset(&configValue, '\0', sizeof (configValue));

	if (3 == argc && !strcmp(argv[2], "-r"))
	{
		iRet = PTI_NvNodeOperate(PTI_READ_OPERATE,
			&configValue, "hw_version");

		if (PTI_RET_OK == iRet)
		{
			TelnetPrint("%s", configValue.readBuf);
		}
		else
		{
			TelnetPrint("%s", retShow[PTI_RET_FALSE]);
			PTI_PRINTF("Get Hardware version fail!\n\r");
		}
	}
	else if (4 == argc && !strcmp(argv[2], "-w"))
	{
		if (strlen(argv[3]) >= VERSION_LEN)
		{
			TelnetPrint("%s", retShow[PTI_RET_FALSE]);
			PTI_PRINTF("The length of Hardware version can not "
				"exceed %d.\n\r", (VERSION_LEN) - 1);
			return ;
		}

		snprintf(configValue.writeBuf, sizeof (configValue.writeBuf),
			"%s", argv[3]);

		iRet = PTI_NvNodeOperate(PTI_WRITE_OPERATE,
			&configValue, "hw_version");

		if (PTI_RET_OK == iRet)
		{
			TelnetPrint("%s", retShow[PTI_RET_OK]);
		}
		else
		{
			TelnetPrint("%s", retShow[PTI_RET_FALSE]);
			PTI_PRINTF("Set Hardware version<%s> fail!\n\r",
				argv[3]);
		}
	}
	else
	{
		TelnetPrint("%s", retShow[PTI_RET_FALSE]);
		PTI_PRINTF("Please enter valid parameter!\n\r");
	}
}

void PTI_SoftwareFunc(int argc, char **argv)
{
    int ret = 0;

	if( (argc == 3) && (!strcmp(argv[2],"-r") ))
    {
  		TelnetPrint("%s", TW_SW_VERSION);  
    }

	return;
}

void PTI_SoftwareExtFunc(int argc, char **argv)
{
    int ret = 0;

	if( (argc == 3) && (!strcmp(argv[2],"-r") ))
    {
  		TelnetPrint("%s", TW_SW_VERSION_EXT);  
    }

	return;
}
/*************************************************************************
功能: region功能操作函数
参数: argc: 传入参数个数
             argv: 传入的参数
返回: 无
备注: http://www.iso.org/iso/en/prods-services/iso3166ma/02iso-3166-code-lists/list-en1.html
*************************************************************************/
void PTI_RegionFunc(int argc, char **argv)
{
    int ret = 0;
    unsigned short len = MAX_NODE_LENGTH;
    char buff[MAX_NODE_LENGTH] = {0};
	struct para_buf nodeValue;
	int i = 0;

	memset(&nodeValue, 0, sizeof(struct para_buf));
    /* region读操作 */
    if( (argc == 3) && (!strcmp(argv[2],"-r") ))
    {
		ret = PTI_NvNodeOperate(PTI_READ_OPERATE ,&nodeValue, "region_code");
  		TelnetPrint("%s", nodeValue.readBuf);  
    }
    /* region写操作 */
    else if((argc == 4) &&  (!strcmp(argv[2],"-w")))
    {
		strcpy(buff, argv[3]);
        len = strlen(buff);

        if(2 != len)
        {
            TelnetPrint("%s", retShow[PTI_RET_FALSE]);
            return;
        }
		
        strcpy(nodeValue.writeBuf, buff);
  		ret = PTI_NvNodeOperate(PTI_WRITE_OPERATE ,&nodeValue, "region_code");
  		TelnetPrint("%s", retShow[ret]);
    }
    else
    {
        PTI_PRINTF("error : illegal parament \n\r");
    }
}
/*************************************************************************
功能: country功能操作函数
参数: argc: 传入参数个数
             argv: 传入的参数
返回: 无
备注: http://www.iso.org/iso/en/prods-services/iso3166ma/02iso-3166-code-lists/list-en1.html
*************************************************************************/
void PTI_CountryCodeFunc(int argc, char **argv)
{
    int ret = 0;
    unsigned short len = MAX_NODE_LENGTH;
    char buff[MAX_NODE_LENGTH] = {0};
	struct para_buf nodeValue;
	int i = 0;

	memset(&nodeValue, 0, sizeof(struct para_buf));
    /* region读操作 */
    if( (argc == 4) && (!strcmp(argv[2],"-2G")) && (!strcmp(argv[3],"-r") ))
    {
		ret = PTI_NvNodeOperate(PTI_READ_OPERATE ,&nodeValue, "country_code_2g");
  		TelnetPrint("%s", nodeValue.readBuf);  
    }
	else if( (argc == 4) && (!strcmp(argv[2],"-5G")) && (!strcmp(argv[3],"-r") ))
    {
		ret = PTI_NvNodeOperate(PTI_READ_OPERATE ,&nodeValue, "country_code_5g");
  		TelnetPrint("%s", nodeValue.readBuf);  
    }
    else if((argc == 5) && (!strcmp(argv[2],"-2G")) && (!strcmp(argv[3],"-w")))
    {
		strcpy(buff, argv[4]);
        len = strlen(buff);

        if(2 != len)
        {
            TelnetPrint("%s", retShow[PTI_RET_FALSE]);
            return;
        }
		
		for(i = 0;strlen(CountryRegionCode2g[i].customCountRegion) != 0; i++)
		{
			if(!strncmp(CountryRegionCode2g[i].customCountRegion, buff, 2))
			{
				break;
			}
		}

		if(strlen(CountryRegionCode2g[i].customCountRegion) == 0)
        {
            TelnetPrint("%s", retShow[PTI_RET_FALSE]);
            return;
        }
		
        strcpy(nodeValue.writeBuf, buff);
  		ret = PTI_NvNodeOperate(PTI_WRITE_OPERATE ,&nodeValue, "country_code_2g");
  		TelnetPrint("%s", retShow[ret]);
    }
	else if((argc == 5) && (!strcmp(argv[2],"-5G")) && (!strcmp(argv[3],"-w")))
    {
		strcpy(buff, argv[4]);
        len = strlen(buff);

        if(2 != len)
        {
            TelnetPrint("%s", retShow[PTI_RET_FALSE]);
            return;
        }
		
		for(i = 0; strlen(CountryRegionCode5g[i].customCountRegion) != 0; i++)
		{
			if(!strncmp(CountryRegionCode5g[i].customCountRegion, buff, 2))
			{
				break;
			}
		}

		if(strlen(CountryRegionCode5g[i].customCountRegion) == 0)
        {
            TelnetPrint("%s", retShow[PTI_RET_FALSE]);
            return;
        }
		
        strcpy(nodeValue.writeBuf, buff);
  		ret = PTI_NvNodeOperate(PTI_WRITE_OPERATE ,&nodeValue, "country_code_5g");
  		TelnetPrint("%s", retShow[ret]);
    }
	else if( (argc == 3) && (!strcmp(argv[2],"-help") ))
	{
		HelpPrint("2g region list :\n");
		for(i = 0; strlen(CountryRegionCode2g[i].customCountRegion) != 0; i++)
		{
			HelpPrint("\r%s\n", CountryRegionCode2g[i].customCountRegion);
		}
		HelpPrint("\n5g region list :\n");
		for(i = 0; strlen(CountryRegionCode5g[i].customCountRegion) != 0; i++)
		{
			HelpPrint("\r%s\n", CountryRegionCode5g[i].customCountRegion);
		}
	}
    else
    {
        PTI_PRINTF("error : illegal parament \n\r");
    }
}

/*************************************************************************
功能: serial number功能操作函数
参数: argc: 传入参数个数
             argv: 传入的参数
返回: 无
备注:
*************************************************************************/
void PTI_SnFunc(int argc, char **argv)
{
    int ret = 0;
    unsigned short len = MAX_NODE_LENGTH;
    char buff[MAX_NODE_LENGTH] = {0};
	struct para_buf nodeValue;

	memset(&nodeValue, 0, sizeof(struct para_buf));
    /* SN读操作 */
    if( (argc == 3) && (!strcmp(argv[2],"-r") ))
    {
		ret = PTI_NvNodeOperate(PTI_READ_OPERATE ,&nodeValue, "factory_sn");
  		TelnetPrint("%s", nodeValue.readBuf);  
    }
    
    /* SN写操作 */
    else if((argc == 4) && (!strcmp(argv[2],"-w") ))
    {
		strcpy(buff, argv[3]);
        len = strlen(buff);

        if(MAX_NODE_LENGTH < len)
        {
            TelnetPrint("%s", retShow[PTI_RET_FALSE]);
            return;
        }
        strcpy(nodeValue.writeBuf, buff);
  		ret = PTI_NvNodeOperate(PTI_WRITE_OPERATE ,&nodeValue, "factory_sn");
#if 0 //WPS PIN depend by sn
		if(ret == PTI_RET_OK)
			loadDefaultWpsPin(ALL_NVRAM);
#endif
  		TelnetPrint("%s", retShow[ret]);
    }
    else
    {
        PTI_PRINTF("error : illegal parament \n\r");
    }
}
/*************************************************************************
功能: wlan密码功能操作函数
参数: argc: 传入参数个数
             argv: 传入的参数
返回: 无
备注:
*************************************************************************/
void PTI_WlanPwdFunc(int argc, char **argv)
{
	int ret = 0;
    unsigned short len = MAX_NODE_LENGTH;
	struct para_buf nodeValue;
    char buff[MAX_NODE_LENGTH] = {0};

	memset(&nodeValue, 0, sizeof(struct para_buf));

	if( (argc == 3) && (!strcmp(argv[2],"-r") ))
    {
		ret = PTI_NvNodeOperate(PTI_READ_OPERATE ,&nodeValue, "wlan_pwd");
  		TelnetPrint("%s", nodeValue.readBuf);  
    } 
	else if((argc == 4) && (!strcmp(argv[2],"-w") ))
    {
		strcpy(buff, argv[3]);
     		 len = strlen(buff);

     		 if(MAX_NODE_LENGTH < len)
    		  {
       			  TelnetPrint("%s", retShow[PTI_RET_FALSE]);
         			return;
     		 }
		if(tbsCheckPasswd(buff)==TRUE)
		{
 	    		strcpy(nodeValue.writeBuf, buff);
	 		ret = PTI_NvNodeOperate(PTI_WRITE_OPERATE ,&nodeValue, "wlan_pwd");
 			TelnetPrint("%s", retShow[ret]);
		}
		else
		{
			PTI_PRINTF("error : illegal parament \n\r");
			TelnetPrint("%s", retShow[PTI_RET_FALSE]);
		}
    }
    else
    {
        PTI_PRINTF("error : illegal parament \n\r");
    }
}

#define FOTA_TMP_FILE "/tmp/fota_tmp"

static int get_fota_credential(const char * pCmd,
							char * pCredential,
							size_t iLen)
{
	char sCredential[512] = { 0 };
	int iRet = -1;
	int iResult_code = -1;
	FILE * fp = NULL;

	if (!pCmd || !pCredential)
	{
		PTI_PRINTF("Null pointer param.\n");
		return -1;
	}

	iRet = system(pCmd);
	if (0 == WIFEXITED(iRet) || 0 != WEXITSTATUS(iRet))
	{
		PTI_PRINTF("system fail, %s, errno=%d.\n", strerror(errno), errno);
		unlink(FOTA_TMP_FILE);
		return -1;
	}

	fp = fopen(FOTA_TMP_FILE, "r");
	unlink(FOTA_TMP_FILE);
	if (!fp)
	{
		PTI_PRINTF("fopen fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	if (!fgets(sCredential, sizeof (sCredential), fp))
	{
		PTI_PRINTF("fgets fail, %s, errno=%d.\n",
			strerror(errno), errno);
		goto done;
	}

	snprintf(pCredential, iLen, "%s", sCredential);

	iResult_code = 0;

done:

	if (fp && fclose(fp) < 0)
	{
		PTI_PRINTF("fclose fail, %s, errno=%d.\n", strerror(errno), errno);
	}

	return iResult_code;
}

void PTI_fota_func(int argc, char * * argv)
{
	char sCmd_buf[512] = { 0 };
	char sCredential[512] = { 0 };
	const char * pParam_name = NULL;
	const char * pAction = NULL;
	const char * pParam_value = NULL;
	int iRet = -1;

	if (4 == argc)
	{
		pParam_name = argv[2];
		pAction = argv[3];

		if (!strcmp(pParam_name, "-client_id")
			&& !strcmp(pAction, "-r"))
		{
			snprintf(sCmd_buf, sizeof (sCmd_buf),
				"fota_config get client_id %s", FOTA_TMP_FILE);
		}
		else if (!strcmp(pParam_name, "-client_secret")
			&& !strcmp(pAction, "-r"))
		{
			snprintf(sCmd_buf, sizeof (sCmd_buf),
				"fota_config get client_secret %s", FOTA_TMP_FILE);
		}
		else if (!strcmp(pParam_name, "-device_id")
			&& !strcmp(pAction, "-r"))
		{
			snprintf(sCmd_buf, sizeof (sCmd_buf),
				"fota_config get device_id %s", FOTA_TMP_FILE);
		}
		else if (!strcmp(pParam_name, "-device_id")
			&& !strcmp(pAction, "-generate"))
		{
			snprintf(sCmd_buf, sizeof (sCmd_buf),
				"fota_config generate device_id");

			iRet = system(sCmd_buf);
			if (0 != WIFEXITED(iRet) && 0 == WEXITSTATUS(iRet))
			{
				TelnetPrint("%s", retShow[PTI_RET_OK]);
			}
			else
			{
				TelnetPrint("%s", retShow[PTI_RET_FALSE]);
				PTI_PRINTF("Generate fota device id fail!\n\r");
			}
		}
		else
		{
			PTI_PRINTF("error : illegal parament<%s %s>\n\r",
				pParam_name, pAction);
			return ;
		}

		if (0 != strcmp(pAction, "-generate"))
		{
			iRet = get_fota_credential(sCmd_buf, sCredential,
				sizeof (sCredential));
			if (0 == iRet)
			{
				TelnetPrint("%s", sCredential);
			}
			else
			{
				TelnetPrint("%s", retShow[PTI_RET_FALSE]);
				PTI_PRINTF("Get %s fail!\n\r", pParam_name);
			}
		}
	}
	else if (5 == argc)
	{
		pParam_name = argv[2];
		pAction = argv[3];
		pParam_value = argv[4];

		if (!strcmp(pParam_name, "-client_id")
			&& !strcmp(pAction, "-w"))
		{
			snprintf(sCmd_buf, sizeof (sCmd_buf),
				"fota_config set client_id %s", pParam_value);

			iRet = system(sCmd_buf);
			if (0 != WIFEXITED(iRet) && 0 == WEXITSTATUS(iRet))
			{
				TelnetPrint("%s", retShow[PTI_RET_OK]);
			}
			else
			{
				TelnetPrint("%s", retShow[PTI_RET_FALSE]);
				PTI_PRINTF("Set fota client id fail!\n\r");
			}
		}
		else if (!strcmp(pParam_name, "-client_secret")
			&& !strcmp(pAction, "-w"))
		{
			snprintf(sCmd_buf, sizeof (sCmd_buf),
				"fota_config set client_secret %s", pParam_value);

			iRet = system(sCmd_buf);
			if (0 != WIFEXITED(iRet) && 0 == WEXITSTATUS(iRet))
			{
				TelnetPrint("%s", retShow[PTI_RET_OK]);
			}
			else
			{
				TelnetPrint("%s", retShow[PTI_RET_FALSE]);
				PTI_PRINTF("Set fota client secret fail!\n\r");
			}
		}
		else if (!strcmp(pParam_name, "-device_id")
			&& !strcmp(pAction, "-w"))
		{
			snprintf(sCmd_buf, sizeof (sCmd_buf),
				"fota_config set device_id %s", pParam_value);

			iRet = system(sCmd_buf);
			if (0 != WIFEXITED(iRet) && 0 == WEXITSTATUS(iRet))
			{
				TelnetPrint("%s", retShow[PTI_RET_OK]);
			}
			else
			{
				TelnetPrint("%s", retShow[PTI_RET_FALSE]);
				PTI_PRINTF("Set fota device_id fail!\n\r");
			}
		}
		else
		{
			PTI_PRINTF("error : illegal parament<%s %s>\n\r",
				pParam_name, pAction);
			return ;
		}
	}
	else
	{
		PTI_PRINTF("error : illegal parament \n\r");
		return ;
	}
}

/*************************************************************************
功能: main函数
参数: argc：传入参数个数
        argv：传入的参数
返回: 成功/失败
备注:   
*************************************************************************/
int main(int argc, char **argv)
{
    int ret, fun_code, iNvNode;

    /* 初始化返回结果 */
	strcpy(retShow[PTI_RET_OK], RETURN_SUCCSS);
	strcpy(retShow[PTI_RET_FALSE], RETURN_FAIL);
    
    /* 简单的参数效俭，并且取出功能号 */
    ret = PTI_ParaPick(argc, argv, &fun_code);
	if(ret != PTI_RET_OK)
	{
		PTI_PRINTF("PTI_ParaPick error \n\r");
		goto PTI_EXIT;
	}
    
    /* 非nvram变量，直接调用相应的功能函数 */
    if(funcNode[fun_code].func != NULL)
    {
        funcNode[fun_code].func(argc, argv);
    }
    else
    {
        PTI_PRINTF("Error: configuraiton error \n\r");
    }
    
PTI_EXIT:
    return PTI_RET_OK;    
}

/*************************************************************************
功能: 根据传入的参数，选择使用的功能
参数: argc：传入参数个数
        argv：传入的参数
        fun_code：选择的功能
返回: 成功/失败
备注:   
*************************************************************************/
int PTI_ParaPick(int argc, char **argv, int *fun_code)
{
	int i;
	int func_num = sizeof(funcNode)/sizeof(struct funcTable);
        
	if(argc < 2)
	{
		PTI_PRINTF("error : illegal parament \n\r");
		return PTI_RET_FALSE;
	}
    
	/* 在功能列表里查找对应的功能号 */
	for(i=0; i< func_num; i++)
	{
		if(!strcmp(argv[1], funcNode[i].function))
		{	
			/* 此功能可用 */
            if(funcNode[i].enable == FUNC_DISABLE)
            {
			    PTI_PRINTF("Error: not support this function \n\r");
                return PTI_RET_FALSE;
	        }
	        else
	        {
	            break;
	        }
	    }
	}

	/* 查找失败 */
	if(i >= func_num)
	{
		PTI_PRINTF("error : illegal parament \n\r");
		return PTI_RET_FALSE;
	}
    
    *fun_code = i;
    return PTI_RET_OK;
}
/******************************************************************************
*                                 END                                        *
******************************************************************************/
