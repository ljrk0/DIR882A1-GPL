#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h> 
#include <sys/time.h> 

#include <bcmnvram.h>
#include <nvram_list.h>
#include "downloader.h"

#define PID_SUFFIX      ".tw_downloader.pid"

/*************************************************************************
功能: 独立进程退出前的处理
参数: 
返回: 
备注: 林明 2013-4月创建
*************************************************************************/
void kill_downloader()
{
		tw_nvram_set_force("usb_existent","0");
		system("killall -9 transmission-cli");
		system("killall -9 wget");
		empty_downloader_data();
		system("killall -9 downloader");
}


static int downloader_speed_peak = 0; 
/*************************************************************************
功能: 计算下载速度的峰值，或者取得当前下载任务的状态信息
参数: type
      1:计算下载速度的峰值
      2:取得当前下载任务的状态信息(正在下载的任务和等待中的任务)
返回: 
备注: 林明 2013-4月创建
*************************************************************************/
void process_speed_or_targets(int type) 
{ 
		FILE *fp = NULL;
		char pid_file_name[256] = {0};
		char hex_name[256] = {0};
		char file[256] = {0};
		int len = 0;
		char strLine[256] = {0};
		char tmpbuf[256] = {0};
		int flag = 0;
		char usb_path[16] = {0};
		char speed[8] = {0};
		char target[512*10] = {0};
		char *wait = NULL;
		system("ls -l /var/run > /var/tmp/pid_file");
		tw_Downloader_get_mnt_path(usb_path);
		len = strlen(PID_SUFFIX);
		wait = tw_nvram_get_force_from_nvram("downloader_waiting_task");
		fp = fopen("/var/tmp/pid_file", "rt");
		if (fp)
		{
			downloader_speed_peak = 0;
			while(!feof(fp = get_file_name(fp, pid_file_name)))
			{
				if( strstr(pid_file_name,PID_SUFFIX) )
				{
					pid_file_name[strlen(pid_file_name) - len] = '\0';
					tw_Downloader_String2HexStr(pid_file_name,hex_name);
					FILE *fp2 = NULL;
					if( 1 == type )//获取速度
					{
						sprintf(file,"%s/%s/%s/%s.%s",usb_path,DOWNLOADER_FOLDER,STATUS,hex_name,STATUS);
						fp2 = fopen(file,"rt");
						if(fp2)
						{
							while(fgets(strLine, sizeof(strLine), fp2))
							{
								if (NULL != strLine && strstr(strLine, STATUS_DOWNLOAD_SPEED))
								{
									sscanf(strLine, STATUS_DOWNLOAD_SPEED"=%s\n", tmpbuf);
									if( NULL != tmpbuf )
									{
										downloader_speed_peak += atoi(tmpbuf);
									}
								}
							}
						fclose(fp2);
						}
						else
						{
							if(NULL != fp);//zhouxc add 返回前需要关闭handler
								fclose(fp);
							return;
						}
					}
					if( 2 == type )//保存当前任务状态:下载中和等待的任务
					{
						sprintf(file,"%s/%s/%s/%s",usb_path,DOWNLOADER_FOLDER,TASKS_FILE,hex_name);
						fp2 = fopen(file,"rt");
						if(fp2)
						{
							while(fgets(strLine, sizeof(strLine), fp2))
							{
								if (NULL != strLine && strstr(strLine, DONWLOAD_TARGET))
								{
									sscanf(strLine, DONWLOAD_TARGET"=%s\n", tmpbuf);
									if( NULL != tmpbuf )
									{
										sprintf(target+strlen(target),"%s%s",TASK_MARK,tmpbuf);
									}
								}
							}
						fclose(fp2);
						}
						else
						{
							if(NULL != fp);//zhouxc add 返回前需要关闭handler
								fclose(fp);
							return;
						}
					}

				}
			}
			fclose(fp);
			if( 2 == type )
			{
				if(NULL != wait && strlen(wait) )
				{
					sprintf(target+strlen(target),"%s",wait);
				}
				if( strlen(target) )
				{
					tw_nvram_set_force("downloader_buf_task",target);
				}
			}
		}
    
} 

/*************************************************************************
功能: 更新nvram中的速度峰值
参数: 
返回: 
备注: 林明 2013-4月创建
*************************************************************************/
int update_speed_peak()
{
		char *nvram_speed_peak = NULL;
		char speed[8] = {0};
		nvram_speed_peak = tw_nvram_get_force_from_nvram("downloader_speed_peak");
		if(NULL != nvram_speed_peak && (125 >= (atoi(nvram_speed_peak) - downloader_speed_peak) ||  125 >= (downloader_speed_peak- atoi(nvram_speed_peak))))
		{
			sprintf(speed,"%d",downloader_speed_peak);
			if( strcmp(speed,"0") )
			{
				tw_nvram_set_force("downloader_speed_peak",speed);
				return 1;
			}    
		}
		return 0;
}

static int busy_flag = 0;
static time_t free_start;
static time_t free_end;
static long last_packets = 0;
/*************************************************************************
功能: 限速的实现
参数: 
返回: 
备注: 林明 2013-4月创建
*************************************************************************/
void ctl_speed()
{
		FILE *fp = NULL;
		char packet[16] = {0};
		char tmp[16] = {0};
		char strLine[256] = {0};
		long current_packets = 0;
		long ctf_packets = 0;
		char *Ptr_packet = NULL;
		char *tasks = NULL;
		int max_speed = 0;
		char *peak_speed = NULL;
		char *Ptr_wan_ip_address = NULL;
		char wan_ip_address[16] = {0};

		system("ifconfig > /var/tmp/ifconfig_file");
		system("et dump ctf > /var/tmp/ctf_packets");
		Ptr_wan_ip_address = tw_nvram_get_force_from_nvram("wan0_ipaddr");
		sprintf(wan_ip_address,Ptr_wan_ip_address);

		if(NULL == (fp = fopen("/var/tmp/ifconfig_file","rt")))
		{
			cprintf("[%s %d] open file fail\n",__FUNCTION__,__LINE__);
			return;
		}
		while(fgets(strLine,sizeof(strLine),fp))
		{
			if(strstr(strLine,"eth1") || strstr(strLine,"eth2") || strstr(strLine,"vlan1"))
			{
				while(fgets(strLine,sizeof(strLine),fp))
				{
					if(strstr(strLine,"RX") && strstr(strLine,"packets"))
					{
						memset(packet, 0, sizeof(packet));
						memset(tmp, 0, sizeof(tmp));
						sscanf(strLine,"%s %s",tmp,packet);
						Ptr_packet = strstr(packet,":") + 1;
						current_packets += atoll(Ptr_packet);
						break;
					}
				}
			} 
		}
		fclose(fp);

		if(NULL == (fp = fopen("/var/tmp/ctf_packets","rt")))
		{
			return;
		}
		fgets(strLine,sizeof(strLine),fp);
		fgets(strLine,sizeof(strLine),fp);
		while(fgets(strLine,sizeof(strLine),fp))
		{
			if(strstr(strLine,"Bridge cache"))
				break;
			if(!strstr(strLine,wan_ip_address))
			{
				strtok(strLine," ");
				while(Ptr_packet = strtok(NULL," "))
				{
					sprintf(tmp,"%s",Ptr_packet);
				}
				ctf_packets += atoll(tmp);
			}
		}				
		fclose(fp);
		if(30000 <= (current_packets - last_packets + ctf_packets) )
		{
			if( 4 != busy_flag )
			{
				process_speed_or_targets(2);
				sleep(1);
				tasks = tw_nvram_get_force_from_nvram("downloader_buf_task");
				if( NULL != tasks && strstr(tasks,TASK_MARK) )
				{
					tw_Downloader_Pause_Manage_Task(tasks);
					sleep(3);
					peak_speed = tw_nvram_get_force_from_nvram("downloader_speed_peak");
					if( NULL != peak_speed && strlen(peak_speed) )
					{
						max_speed = atoi(peak_speed)/18;  //1/6
					}
					tw_Downloader_Start_Manage_Task(tasks,max_speed);
				}
				tw_nvram_unset_force("downloader_buf_task");
			}
			free_start = time(NULL);
			busy_flag = 4;	   
		}
		else if(10000 <= (current_packets - last_packets + ctf_packets) )
		{
			if( 3 != busy_flag )
			{
				process_speed_or_targets(2);
				sleep(1);
				tasks = tw_nvram_get_force_from_nvram("downloader_buf_task");
				if( NULL != tasks && strstr(tasks,TASK_MARK) )
				{
					tw_Downloader_Pause_Manage_Task(tasks);
					sleep(3);
					peak_speed = tw_nvram_get_force_from_nvram("downloader_speed_peak");
					if( NULL != peak_speed && strlen(peak_speed) )
					{
						max_speed = atoi(peak_speed)/12;   //1/4
					}
					tw_Downloader_Start_Manage_Task(tasks,max_speed);
				}
				tw_nvram_unset_force("downloader_buf_task");
			}
			free_start = time(NULL);
			busy_flag = 3;	   
		}
		else if(5000 <= (current_packets - last_packets + ctf_packets))
		{
			if( 2 != busy_flag )
			{
				process_speed_or_targets(2);
				sleep(1);
				tasks = tw_nvram_get_force_from_nvram("downloader_buf_task");
				if( NULL != tasks && strstr(tasks,TASK_MARK) )
				{
					tw_Downloader_Pause_Manage_Task(tasks);
					sleep(3);
					peak_speed = tw_nvram_get_force_from_nvram("downloader_speed_peak");
					if( NULL != peak_speed && strlen(peak_speed) )
					{
						max_speed = atoi(peak_speed)/9;   //1/3
					}
					tw_Downloader_Start_Manage_Task(tasks,max_speed);
				}
				tw_nvram_unset_force("downloader_buf_task");
			}
			free_start = time(NULL);
			busy_flag = 2;	   
		}
		else if(600 <= (current_packets - last_packets + ctf_packets))
		{
			if( 1 != busy_flag )
			{
				process_speed_or_targets(2);
				sleep(1);
				tasks = tw_nvram_get_force_from_nvram("downloader_buf_task");
				if( NULL != tasks && strstr(tasks,TASK_MARK) )
				{
					tw_Downloader_Pause_Manage_Task(tasks);
					sleep(3);
					peak_speed = tw_nvram_get_force_from_nvram("downloader_speed_peak");

					if( NULL != peak_speed && strlen(peak_speed) )
					{
						max_speed = atoi(peak_speed)/9*2;   //2/3
					}
					tw_Downloader_Start_Manage_Task(tasks,max_speed);
				}
				tw_nvram_unset_force("downloader_buf_task");
			}
			free_start = time(NULL);
			busy_flag = 1;	   
		}
		else//闲时
		{
			if( 0 != busy_flag )//从忙切换到闲
			{
				process_speed_or_targets(2);
				sleep(1);
				tasks = tw_nvram_get_force_from_nvram("downloader_buf_task");
				if( NULL != tasks && strstr(tasks,TASK_MARK) )
				{
					tw_Downloader_Pause_Manage_Task(tasks);
					sleep(2);
					tw_Downloader_Start_Manage_Task(tasks,0);
				}
				tw_nvram_unset_force("downloader_buf_task");
			}
			busy_flag = 0;
		}
		last_packets = current_packets;

}

static int count = 0;
void timer_process()
{
		count ++;
		tw_Downloader_Init_File_list(NULL,1);

		if( 0 == count % 5 )//更新峰值
		{
			if( 0 == busy_flag )
			{
				free_end = time(NULL);
				if( 300 <= (free_end - free_start) )//30 minitus
				{
					process_speed_or_targets(1);
					update_speed_peak();
				}
			}
		}
		if( 0 == count % 2 )
		{
			char *smart_internet = NULL; 
			smart_internet = tw_nvram_get_force_from_nvram("Smart_Internet_enable");
			if( NULL != smart_internet && !strcmp(smart_internet,"1"))
			{
				ctl_speed();
			}
		}
}

// 建立信号处理机制 
void init_sigaction(void) 
{ 
		struct sigaction tact; 
		/*信号到了要执行的任务处理函数为timer_process*/ 
		tact.sa_handler = timer_process; 
		tact.sa_flags = 0; 
		/*初始化信号集*/ 
		sigemptyset(&tact.sa_mask); 
		/*建立信号处理机制*/ 
		sigaction(SIGALRM, &tact, NULL); 
} 
void init_time() 
{ 
		struct itimerval value; 
		/*设定执行任务的时间间隔为60秒0微秒*/ 
		value.it_value.tv_sec = 60; 
		value.it_value.tv_usec = 0; 
		/*设定初始时间计数也为60秒0微秒*/ 
		value.it_interval = value.it_value; 
		/*设置计时器ITIMER_REAL*/ 
		setitimer(ITIMER_REAL, &value, NULL); 
} 

/******************************************************************
功能: downloader主函数
参数: argc: 参数个数      *argv: 参数数组
返回: 
备注:
******************************************************************/
int main(int argc, const char *argv[])
{
		init_sigaction(); 
		init_time(); 
		signal(SIGTERM,kill_downloader);
		char *downloader_tasks = NULL;
		char *action_flag = NULL;
		char *wait_record = NULL;
		char *buf_tasks = NULL;
		static int flag = 0;
		char *last_smart_internet = NULL;
		char *current_smart_internet = NULL;
		char last_smart_internet_tmp[8] = {0};

		last_smart_internet = tw_nvram_get_force_from_nvram("Smart_Internet_enable");
		sprintf(last_smart_internet_tmp,"%s",last_smart_internet);
		ctl_speed();

		while(1)
		{
			sleep(3);
			if( !strcmp(tw_nvram_get_force_from_nvram("usb_existent"),"0") )
			continue;

			downloader_tasks = tw_nvram_get_force_from_nvram("downloader_tasks");
			action_flag = tw_nvram_get_force_from_nvram("action_bt");
			wait_record = tw_nvram_get_force_from_nvram("downloader_waiting_task");	

			if( NULL != action_flag && !strcmp(action_flag,"0") )
			{
				if( NULL != downloader_tasks && strstr(downloader_tasks, "#task#"))
				{
					tw_Downloader_Start_Manage_Task(downloader_tasks,0);
				}
				flag = 1;
			}
			else if( NULL != action_flag && !strcmp(action_flag,"1") )
			{
				if( NULL != downloader_tasks && strstr(downloader_tasks, "#task#"))
				{
					tw_Downloader_Pause_Manage_Task(downloader_tasks);
				}
				flag = 1;
			}
			else if( NULL != action_flag && !strcmp(action_flag,"2"))
			{
				if( NULL != downloader_tasks && strstr(downloader_tasks, "#task#"))
				{
					tw_Downloader_Delete_Manage_Task(downloader_tasks);
				}
				flag = 1;
			}
			else if( NULL != wait_record && strstr(wait_record,TASK_MARK) && check_num_of_tw_downloader_pid())
			{
				sleep(3);
				tw_Downloader_Start_Manage_Task(NULL,0); 
			}   

			current_smart_internet = tw_nvram_get_force_from_nvram("Smart_Internet_enable");
			if( NULL != current_smart_internet && strcmp(last_smart_internet_tmp,current_smart_internet))
			{
				if(!strcmp(last_smart_internet_tmp,"1"))
				{
					process_speed_or_targets(2);
					sleep(1);
					buf_tasks = tw_nvram_get_force_from_nvram("downloader_buf_task");
					if( NULL != buf_tasks && strstr(buf_tasks,TASK_MARK) )
					{
						tw_Downloader_Pause_Manage_Task(buf_tasks);
						sleep(2);
						tw_Downloader_Start_Manage_Task(buf_tasks,0);
					}
					tw_nvram_unset_force("downloader_buf_task");
					busy_flag = 0;
				}
				last_smart_internet = tw_nvram_get_force_from_nvram("Smart_Internet_enable");
				sprintf(last_smart_internet_tmp,"%s",last_smart_internet);
			}

			if( 1 == flag )
			{
				flag = 0;
				tw_nvram_unset_force("downloader_tasks");
				tw_nvram_unset_force("action_bt");
			}
		}

		return 1;
}

