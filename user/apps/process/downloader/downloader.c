/******************************************************************
 Copyright (c), 2013-2023, T&W ELECTRONICS(SHENTHEN) Co., Ltd.

 文件名称: downloader.c
 文件描述:

 修订记录:
        1. 作者: wuyouhui, wuyouhui@twsz.com
           日期: 2013年03月05日
           内容: Create

        2. 作者: linming@twsz.com
           日期: 2013.03.11
           内容: add function

******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <errno.h>

#include <bcmnvram.h>
#include <nvram_list.h>
#include "downloader.h"
#include "shutils.h"

#define BT 1
#define FTP 2
#define HTTP_HTTPS 3

#define HTTP_PREFIX_1 "http://"
#define HTTP_PREFIX_2 "https://"
#define HTTP_PREFIX_3 "HTTP://"
#define HTTP_PREFIX_4 "HTTPS://"

#define FTP_PREFIX_1  "ftp://"
#define FTP_PREFIX_2  "FTP://"

#define DWNLOADER_MAX_TASK	3
#define TASK_MAX_PORT 	"/var/tmp/down_task_max_port"
#define INIT_TASK_STATUS_FILE 	1

#define MNT_INFO_FILE 	"/var/tmp/mnttmp"
#define MNT_KEY 		"media"
#define PID_SUFFIX      ".tw_downloader.pid"
/******************************************************************************
*                               FUNCTION                                     *
******************************************************************************/

/*************************************************************************
功能: 转换种子名称为二进制字符串
参数: pSrc       -> 种子名称
	pDest	     ->输出二进制字符串
返回: 0
备注: 吴悠慧 2013-3月创建
*************************************************************************/
int tw_Downloader_String2HexStr(char *pSrc, char *pDest)
{
	char tmp[3]={0};
	int len=0,i=0;

	len=strlen(pSrc);
	pDest[0]=0;
	while(i<len){
		sprintf(tmp,"%02X",(unsigned char)pSrc[i]);
		strcat(pDest, tmp);
		i++;
	}
	return 0;
}

/*************************************************************************
功能: get the USB disk's size 
参数: NULL
返回: The avaliable size of the USB disk
备注: 吴悠慧 2013-3月创建
*************************************************************************/
static uint64_t tw_get_mnt_size()
{
	char mntpath[128] = {0};
	char devpath[128] = {0};
	char usePercent[8] = {0};
	char tmpbuf[512]  = {0};
	uint64_t isize = 0;
	uint64_t iused = 0;
	uint64_t iavailable = 0;
	FILE *fp = NULL;

	memset(mntpath, 0, sizeof(mntpath));
	memset(devpath, 0, sizeof(devpath));
	memset(usePercent, 0, sizeof(usePercent));

	/*use df command to get the disk's information*/
	sprintf(tmpbuf, "df > %s", MNT_INFO_FILE);
	system(tmpbuf);
	
	if ((fp = fopen(MNT_INFO_FILE, "r")) != NULL) {
			while (fgets(tmpbuf, sizeof(tmpbuf), fp) != NULL) {
				if (strstr(tmpbuf, MNT_KEY) != NULL) {
					sscanf(tmpbuf, "%s %llu %llu %llu %s %s", devpath, &isize, &iused, 
						&iavailable, usePercent, mntpath);
					break;
				}
			}
			fclose(fp);
		}
	
	sprintf(tmpbuf, "rm %s", MNT_INFO_FILE);
	system(tmpbuf);
	
	return iavailable;
}

/*************************************************************************
功能: get the USB disk's mounting path
参数: Output  mntpath   ->the path of the USB disk
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
int tw_Downloader_get_mnt_path(char *mntpath)
{
	char devpath[128] = {0};
	FILE *fp;
	char buf[256];
    int flag = 0;       //U盘是否存在
	memset(mntpath, 0, sizeof(mntpath));
	memset(devpath, 0, sizeof(devpath));

	if ((fp = fopen("/proc/mounts", "r")) != NULL) {
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			if (strstr(buf, MNT_KEY) != NULL) {
				sscanf(buf, "%s %s", devpath, mntpath);
                flag = 1;
				break;
			}
		}
		fclose(fp);
	}
    if( 1 == flag )
	    return 1;
    else 
        return 0;
}

/*************************************************************************
功能: to confirm is there enough size of USB disk for the new downloader task
参数: filesize   ->the size of the file while will be download
返回: 1: enough  0: not enough
备注: 吴悠慧 2013-3月创建
*************************************************************************/
static int tw_Downloader_Is_Enough_buffer(uint64 filesize)
{
	uint64 mntsize = 0;

	mntsize = tw_get_mnt_size();
	if (((filesize/1024)+1) > mntsize)
		return 0;
	return 1;
}

/*************************************************************************
功能: get a new port for the transmission-cli download task
参数: output the roecord of the port number, default is the max record
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
static int tw_Downloader_get_Max_port(char *maxport)
{
	FILE *fp = NULL;
	int port = 51314;
	char tmpAry[128] = {0};

	fp = fopen(TASK_MAX_PORT, "rt");
	if (fp)
	{
		if (fgets(tmpAry, sizeof(tmpAry), fp))
		{
			/*the port number increase 1 each time*/
			port = atoi(tmpAry) + 1;
		}
		fclose(fp);
	}
	sprintf(maxport, "%d", port);
	sprintf(tmpAry, "echo %d > "TASK_MAX_PORT, port);
	system(tmpAry);

	return 1;
}

/*************************************************************************
功能: get the status of the transmission-cli downloader progress 
参数: filePath  -> the root path of the downloader task, here is tw_downloader/
返回: The avaliable size of the USB disk
备注: 吴悠慧 2013-3月创建
*************************************************************************/
static int tw_Downloader_add_status_List(char *filePath)
{
    char fname[256] = {0};
	char tmpbuf[16] = {0};
	char strLine  [512] = {0};
	char statbuf[512*10] = {0};
	char finishbuf[128*10] = {0};
	char *record = NULL;
	FILE *fp = NULL;
	

	/*get all the downloading tasks' value from nvram,and save in statbuf*/
	record = tw_nvram_get_force_from_nvram("downloader_running_task");
	if (NULL != record && strstr(record,TASK_MARK))
	{
		sprintf(statbuf, "%s", record);	
	}
    
	/*get all the finished tasks' value from nvram,and save in statbuf*/
	record = tw_nvram_get_force_from_nvram("downloader_finish_task");	
	if (NULL != record && strstr(record,TASK_MARK))
	{
		sprintf(finishbuf, "%s", record); 
	}
    
	/*get torrent file's name in 0x mode*/
	record = strstr(filePath, DOWNLOADER_FOLDER"/"STATUS"/");
	sprintf(fname, "%s", record+strlen(DOWNLOADER_FOLDER"/"STATUS"/"));
	record = strtok(fname, ".");  
	sprintf(fname, "%s", record);
	if( NULL == (fp = fopen(filePath, "rt")) )
	{
        cprintf("[%s %d] open file fail!\n",__FUNCTION__,__LINE__);
        return -1;
	}

	while(fgets(strLine, sizeof(strLine), fp))
	{
		if (strstr(strLine, STATUS_FINISH_PERCENT))
		{
			sscanf(strLine, STATUS_FINISH_PERCENT"=%s\n", tmpbuf);

			/*if finish the downloading task, percent=100%, then save it into the finish nvram; 
				 * else save it into the downloading nvram*/
			if (strstr(tmpbuf, "100"))
			{
				sprintf(finishbuf+strlen(finishbuf), TASK_MARK"%s"TASK_VALUE_MARK, fname);
                while(fgets(strLine, sizeof(strLine), fp))
	            {
                     if( strstr(strLine, FILE_SIZE) )
                     {
                        sscanf(strLine, FILE_SIZE"=%s\n", tmpbuf);
                        sprintf(finishbuf+strlen(finishbuf), "%s", tmpbuf);
                     }
                     break;                  
                }
				break;
			}
			else
		    {
                sprintf(statbuf+strlen(statbuf), TASK_MARK"%s", fname);
			    sprintf(statbuf+strlen(statbuf), TASK_VALUE_MARK"%s", tmpbuf);
			}
            
		}
        else if( strstr(strLine, FILE_SIZE) )
        {
            sscanf(strLine, FILE_SIZE"=%s\n", tmpbuf);
            sprintf(statbuf+strlen(statbuf), TASK_VALUE_MARK"%s", tmpbuf);
           
        }
		else if (strstr(strLine, STATUS_DOWNLOAD_SPEED))
		{
			sscanf(strLine, STATUS_DOWNLOAD_SPEED"=%s\n", tmpbuf);
			sprintf(statbuf+strlen(statbuf), TASK_VALUE_MARK"%s", tmpbuf);
		}
		else if (strstr(strLine, STATUS_TASK_STATUS) ||(strstr(strLine, STATUS_SPEEDER_NUM)))
		{
			if (strstr(strLine, STATUS_TASK_STATUS) && check_pid_running(fname))
				//sscanf(strLine, STATUS_TASK_STATUS"=%s\n", tmpbuf);
				sprintf(tmpbuf, "%s" , "4");
			else
				sprintf(tmpbuf, "%s" , "0");
			sprintf(statbuf+strlen(statbuf), TASK_VALUE_MARK"%s", tmpbuf);
		}      
	}
    
	/*resave the running and finished download task's nvram param*/
	tw_nvram_unset_force("downloader_running_task");
	tw_nvram_set_force("downloader_running_task", statbuf);
   
	tw_nvram_unset_force("downloader_finish_task");
	tw_nvram_set_force("downloader_finish_task", finishbuf);
   
	fclose(fp);
    return 1;
}

/*************************************************************************
功能: delete information from nvram setting according to the operation of 
      each task
参数: nvName    -> the nvram setting param
      filename  -> the task's bt file's 0x name
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
static int  tw_Downloader_del_list(char *nvName, char *filename)
{
	char *tmpIndex = NULL;
	char *next = NULL;
	char tmpbuff[512*10] = {0};

	tmpIndex = tw_nvram_get_force_from_nvram(nvName);
	if (tmpIndex != NULL && strstr(tmpIndex,TASK_MARK))
	{
		sprintf(tmpbuff, "%s", tmpIndex);
		if ((tmpIndex = strstr(tmpbuff, filename)))
		{
			if ((next=strstr(tmpIndex, TASK_MARK)))
			{
				tmpIndex = tmpIndex - (strlen(TASK_MARK));
				tmpIndex[0] = '\0';
				
				sprintf(tmpbuff+strlen(tmpbuff), "%s", next);
			}
			else
			{
				tmpIndex = tmpIndex - (strlen(TASK_MARK));
				tmpIndex[0] = '\0';
			}
		}

		tw_nvram_unset_force(nvName);
		tw_nvram_set_force(nvName, tmpbuff);
	}
	return 1;
}

/*************************************************************************
功能: to confirm the task is in the downloader_running_task nvram setting
参数: Input  taskName  -> the name of the task
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
static int tw_Downloader_search_running(char *taskName)
{
	char *record = NULL;
	char *index = NULL;
	char *end = NULL;
    char hex_name[1024] = {0};
	int lenth = strlen(TASK_VALUE_MARK);

    tw_Downloader_String2HexStr(taskName, hex_name);

	record = tw_nvram_get_force_from_nvram("downloader_running_task");
	if (NULL != record && strstr(record,TASK_MARK))
	{
		if ((index = strstr(record, hex_name)))
		{
			int i = 0;
			end = strstr(index, TASK_VALUE_MARK);
			if (NULL != end)
			{
				while((i < 4) &&(end= strstr(end, TASK_VALUE_MARK)))
				{
                    char tmp[4] = {0};
					end += lenth;
					++ i ;
                    sprintf(tmp, "%c", end[0]);
					if ((i == 4) && (atoi(tmp) == 4))   
						return 1;
				}
			}
		}
	}
	return 0;
}

/*************************************************************************
功能: to confirm the task is in the downloader_waiting_task nvram setting
参数: Input  taskName  -> the name of the task
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
static int tw_Downloader_search_waiting(char *taskName)
{
	char *record = NULL;

	record = tw_nvram_get_force_from_nvram("downloader_waiting_task");
	if (NULL != record && strstr(record,TASK_MARK))
	{
		if (strstr(record, taskName))
			return 1;
	}
	return 0;
}

/*************************************************************************
功能: to assure that the new start task not in the running/waiting queue
参数: Input  inputTask  -> the name of the new task
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
static void tw_Downloader_Keep_New_Task(char *inputTask)
{
	char *index = NULL;
	char *end = NULL;
	char szname[512] = {0};
    char url[512] = {0};
	int lenth = strlen(TASK_MARK);
    
	if (NULL != inputTask)
	{
		index = strstr(inputTask , TASK_MARK);
		if (NULL != index)
		{
			while((index = strstr(index, TASK_MARK)))
			{
				int ret = 0;
				index += lenth;

				/*get the input task name*/
				if ((end= strstr(index, TASK_MARK)))
					snprintf(szname, (end-index+1), "%s", index);
				else
					sprintf(szname, "%s", index);

                 if( NULL != szname && ( strstr(szname,HTTP_PREFIX_1) || strstr(szname,HTTP_PREFIX_2) || strstr(szname,HTTP_PREFIX_3) || strstr(szname,HTTP_PREFIX_4) || strstr(szname,FTP_PREFIX_1) || strstr(szname,FTP_PREFIX_2)) )
                {
                    sprintf(url,"%s",szname);
                    get_http_ftp_file_name(url, szname);
                }

				/*to confirm that if the task is in running or waiting list*/
				if (tw_Downloader_search_running(szname))
					ret = 1;
				else if (tw_Downloader_search_waiting(szname))
					ret = 1;
				else
					ret = 0;
                
				/*omit the repeat tasks*/
				if (ret)
				{
					if(NULL != end)
					{
						index[0] = '\0';
						end += lenth;		
						sprintf(inputTask+strlen(inputTask), "%s", end);
                        index = inputTask;
					}
					else
					{
						(index-lenth)[0] = '\0';
					}
				}
			}
		}
	}	
}

/*************************************************************************
功能: to analy the bt file and get the downloader file name, totalsize
参数: Input  torrentPath   ->the path of the bt torrent file
      Output name -> the name of the download file
             size -> the totalsize of the download file
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
static int tw_Downloader_Get_BT_info(char *torrentPath, char *name, uint64 *size)
{
	FILE *fp = NULL;
	char *index = NULL;
	char *end = NULL;
	char szLine[1024*8] = {0};	

	/*open and analise the BT file*/
	fp = fopen(torrentPath, "rt");
	if (fp)
	{
		if(fgets(szLine, sizeof(szLine), fp))
		{		
			/*caculate the totalsize of the BT downloader file */
			remove_out_0x00_dat(szLine,sizeof(szLine));//去除szline中的00，方便进行字符串处理
			index = strstr(szLine, BT_TORRENT_LENGTH_START);
			while (index != NULL)
			{
				char subsize[16] = {0};
				index += (strlen(BT_TORRENT_LENGTH_START)+1);
				end = strstr(index, BT_TORRENT_LENGTH_END);
				snprintf(subsize, (end-index+1), "%s", index);
				*size += atoll(subsize);
				index = strstr(index, BT_TORRENT_LENGTH_START);
			}

			/*get the BT downloader file name*/
			index = strstr(szLine, BT_TORRENT_NAME);
			index += strlen(BT_TORRENT_NAME);
			while(index[0] != ':')
				index ++;
			end = index +1;
			while (end[0] != ':')
				end ++;

			/* end-2: because name end with 12, so we need to omit 2 charater */
			snprintf(name, ((end-2)-index), "%s", (index+1));				
			}
			fclose(fp);
		}
	return 1;
}

/*************************************************************************
功能: add the BT file's name and size information to the downloader_file_info
      nvram setting
参数: Output  mntpath   ->the path of the USB disk
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
int tw_Downloader_add_BT_info(char *torrentPath, char *savePath)
{
	char *index = NULL;
	char szCmd[1024] = {0};
	char infobuf[640*10] = {0};
	char tor_name[128] = {0};
	char hex_name[256] = {0};
	char name[256] = {0};
    char usb_path[128] = {0};
	uint64 size = 0;

	index = strstr(torrentPath, BT_DOWNLOADER_TORRENT_FOLDER);
	index +=strlen(BT_DOWNLOADER_TORRENT_FOLDER"/");
	snprintf(tor_name, sizeof(tor_name), "%s", index);
	tw_Downloader_String2HexStr(tor_name, hex_name);

	index = tw_nvram_get_force_from_nvram("downloader_file_info");
	if (NULL != index && strstr(index,TASK_MARK))
	{
		sprintf(infobuf, "%s", index);	
	}

	if ((NULL == index) || (!check_task_existent(index, hex_name,TASK_MARK)))
	{
		tw_Downloader_Get_BT_info(torrentPath, name, &size);
		/*if the buffer of usb disk is less than the downloading file size, then exit*/
		if (!tw_Downloader_Is_Enough_buffer(size))
		{
			sprintf(szCmd, "rm -rf %s", torrentPath);
			system(szCmd);
			cprintf("The usb disk Available buffer is less than the download file\n");
			tw_nvram_set_force("is_bigger", "1");
			return 0;
		}

		/*construct the task's infomation, add then add to the nvram setting*/
		sprintf(infobuf+strlen(infobuf), 
		    TASK_MARK"%s"TASK_VALUE_MARK"%s"TASK_VALUE_MARK"%llu"TASK_VALUE_MARK"%s"TASK_VALUE_MARK"%d"TASK_VALUE_MARK"%s", 
		    hex_name, name, size, DOWNLOADER_FILE_PATH,BT, tor_name);
        tw_nvram_unset_force("downloader_file_info");
		tw_nvram_set_force("downloader_file_info", infobuf);
	}
	else
	{
		cprintf("The adding file is exist!");
		return 0;
	}

    //创建该任务的记录文件
    tw_Downloader_get_mnt_path(usb_path);
    sprintf(szCmd,"%s/"DOWNLOADER_FOLDER"/%s/%s",usb_path,TASKS_FILE,hex_name);
    FILE * fp = NULL;
    if( NULL == ( fp = fopen(szCmd,"w+")) )
    {
        cprintf("[%s %d] open file fail!\n",__FUNCTION__,__LINE__);
        return -1;
    }
    fprintf(fp,FILE_NAME"=%s\n",name);
    fprintf(fp,FILE_SIZE"=%lld\n",size);
    fprintf(fp,SAVE_PATH"=%s\n",DOWNLOADER_FILE_PATH);
    fprintf(fp,DONWLOAD_TYPE"=%d\n",BT);
    fprintf(fp,DONWLOAD_TARGET"=%s\n",tor_name);
    fflush(fp);
    fclose(fp);
    
	return 1;
}

/*************************************************************************
功能: use to initial the nvram setting while the USB disk add/remove  
      nvram setting
参数: Output  tmpConfpath   ->the root folder of the download task, here is tw_downlader/
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/

int tw_Downloader_Init_File_list(char *downloader_path, int type)
{
	char strCmd[512] = {0};
    char confpath[256] = {0};
    char task_name[512] = {0};
	FILE *fp = NULL;

    if( NULL == downloader_path)
    {   
        tw_Downloader_get_mnt_path(confpath);//add by linming: 路径指定
        strcat(confpath,"/"DOWNLOADER_FOLDER);
    }
    else 
        sprintf(confpath,"%s",downloader_path);

    if( access(confpath,F_OK) )
    {
        return -3;
    }
   
    if( INIT_TASK_STATUS_FILE == type )
    {
        tw_nvram_unset_force("downloader_running_task");
        tw_nvram_unset_force("downloader_finish_task");
    }
    
    else
    {
        empty_downloader_data();
        sprintf(strCmd, "ls %s/%s -l > /var/tmp/tasks_file", confpath,TASKS_FILE);
        system(strCmd);
		/*initial the downloader_file_info nvram setting according to the torrent files*/
        if( NULL == (fp = fopen("/var/tmp/tasks_file", "rt")) )
        {
            cprintf("[%s %d] open file fail!\n",__FUNCTION__,__LINE__);
            return -2;
        }
        while(!feof(fp = get_file_name(fp, task_name)))
        {
            sprintf(strCmd,"%s/"TASKS_FILE"/%s",confpath,task_name);
            add_all_tasks_info(strCmd,task_name);
        }
        fclose(fp);
        //system("rm -rf /var/tmp/tasks_File");
    }

    sprintf(strCmd, "ls %s/%s -l > /var/tmp/status_file", confpath,STATUS);
	system(strCmd);
	/*initial the downloader_running_task/downlaoder_finish_task nvram setting according to the transmission-cli output file which save in USB disk*/
	fp = fopen("/var/tmp/status_file", "rt");
	if( NULL == fp )
    {
         cprintf("--- [%s %d] open file fail!\n",__FUNCTION__,__LINE__);
         return -1;
    }
	while(!feof(fp = get_file_name(fp, task_name)))
	{
	    if (0 != strlen(task_name))
		{
		    sprintf(strCmd, "%s/%s/%s", confpath, STATUS, task_name);	
		}
		if (strstr(strCmd, "status"))
			tw_Downloader_add_status_List(strCmd); 
	}	
    fclose(fp);
    return 1;
}


/*************************************************************************
功能: start the transmission-cli progress  
参数: Input  savePath   -> where to save the download file
             confPath   -> where to get the config file
             torrentPath -> the BT file will be downloaded
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
int tw_Downloader_start_Transmission(char* savePath, char* confPath, char* torrentPath, int speed)
{	
	char tmpCmd[512] = {0};
	char name[512] = {0};
	char maxport[16] = {0};
	char mntpath[1024] = {0};
	uint64 size = 0;

	tw_Downloader_get_mnt_path(mntpath);
	tw_Downloader_get_Max_port(maxport);

	/*to confirm the usb disk is large enough fo the new download task, if not exit*/
	sprintf(tmpCmd, "%s/%s", mntpath, torrentPath);
	tw_Downloader_Get_BT_info(tmpCmd, name, &size);
	if (!tw_Downloader_Is_Enough_buffer(size))
	{
		sprintf(name, "%s", tmpCmd);
		sprintf(tmpCmd, "rm -rf %s", name);
		system(tmpCmd);
		cprintf("The usb disk Available buffer is less than the download file\n");
		tw_nvram_set_force("is_bigger", "1");
		return 0;
	}
    #if 0
    if( 0 == speed )
	{
        sprintf(tmpCmd, "transmission-cli -w %s/%s -g %s/%s -p %s -d 200 -u 20 -f 1 %s/%s &", 
			mntpath, DOWNLOADER_FILE_PATH, mntpath,confPath, maxport, mntpath,torrentPath);
    }
    else
    {
        sprintf(tmpCmd, "transmission-cli -d %d -w %s/%s -g %s/%s -p %s -d 200 -u 20 -f 1 %s/%s &", 
			speed, mntpath,DOWNLOADER_FILE_PATH, mntpath,confPath, maxport, mntpath,torrentPath);
    }
    #endif
    if( 0 == speed )
	{
        sprintf(tmpCmd, "cd %s; transmission-cli -D -w %s -g %s -p %s -u 20 -f 1 %s &", 
			mntpath, DOWNLOADER_FILE_PATH, confPath, maxport, torrentPath);
    }
    else
    {
        sprintf(tmpCmd, "cd %s; transmission-cli -d %d -w %s -g %s -p %s -u 20 -f 1 %s &", 
			mntpath,speed, DOWNLOADER_FILE_PATH, confPath, maxport, torrentPath);
    }
	system(tmpCmd);
    cprintf("[%s %d] Cmd=%s\n",__FUNCTION__,__LINE__,tmpCmd);
	return 1;
}

/*************************************************************************
功能: use to pause the task which is running 
参数: Input  torrentFile   -> the name of the BT file which want to pause
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
int tw_Downloader_pause(char *task)
{
	FILE *fp = NULL;
	char tmpBuff[16] = { 0 };
	char tmparry[1024] = { 0 };

	/*get the running task's pid, then kill it by pid*/
	sprintf(tmparry, PID_DIR"/%s"PID_SUFFIX, task);
	fp = fopen(tmparry, "rt");
	if (fp)
	{
		fgets(tmpBuff, sizeof(tmpBuff), fp);
		fclose(fp);
	}

	sprintf(tmparry, "kill %s", tmpBuff);
	system(tmparry);

    sprintf(tmparry,"rm -rf /var/run/%s"PID_SUFFIX, task);
    system(tmparry);
    
    tw_Downloader_del_list("downloader_waiting_task", task);
	return 1;
}

/*************************************************************************
功能: use to remove the task from the running/waiting queue
参数: NULL
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
int tw_Downloader_delete(char* confPath, char *task)
{
	char tmpAry[1024] = { 0 };
	char tmpCmd[1024] = { 0 };
	char hex_name[1024] = {0};
   
	tw_Downloader_pause(task); 
	tw_Downloader_String2HexStr(task, hex_name);
	
	if(strlen(hex_name)>255)
    {
  		hex_name[255] = 0;
  	}

	/*wait for a while, then can run the following command normal*/
	sleep(2);
	
	/*remove info from var/run path*/
	sprintf(tmpCmd, "rm -rf "TASK_STATUS_FOLDER"/%s", hex_name);
	system(tmpCmd);

    tw_Downloader_get_mnt_path(tmpAry);
    
	/*remove status info from downloader path*/
	sprintf(tmpCmd, "rm -rf %s/"DOWNLOADER_FOLDER"/"STATUS"/%s."STATUS, tmpAry, hex_name);
	system(tmpCmd);

    /*remove torrent file*/
    sprintf(tmpCmd, "rm -rf %s/"BT_DOWNLOADER_TORRENT_PATH"/%s",tmpAry,task);
    system(tmpCmd);

    /*remove info file*/
    sprintf(tmpCmd, "rm -rf %s/"DOWNLOADER_FOLDER"/"TASKS_FILE"/%s", tmpAry, hex_name);
    system(tmpCmd);
    
	/*remove info from nvram param*/
	tw_Downloader_del_list("downloader_file_info", hex_name);
	tw_Downloader_del_list("downloader_running_task", hex_name);
	tw_Downloader_del_list("downloader_finish_task", hex_name);
    tw_Downloader_del_list("downloader_waiting_task", task);
	return 1;
}

/*************************************************************************
功能: use to clean the finish task records 
参数: NULL
返回: 1
备注: 吴悠慧 2013-3月创建
*************************************************************************/
#if 0
int tw_Downloader_clean_finish_task()
{
	char *finish = NULL;
	char *running = NULL;
	char *findex = NULL;
	char *rindex = NULL;
	char *fend = NULL;
	char *rend = NULL;
	char szfname[256] = {0};
	char szrname[256] = {0};
	char szCmd[512] = {0};
	char mntpath[1024] = {0};
	int lenth = strlen(TASK_MARK);

	finish = tw_nvram_get_force_from_nvram("downloader_finish_task");
	running = tw_nvram_get_force_from_nvram("downloader_file_info");

	findex = finish;
	rindex = running;
	while ( NULL != findex && (findex=strstr(findex, TASK_MARK)) && (fend=strstr(findex, TASK_VALUE_MARK)))
	{
		findex = findex + lenth;
		snprintf(szfname, (fend-findex+1), "%s", findex);

		/*get the 0X name of the BT torrent file which have been finished download*/
		while( NULL != rindex && (rindex=strstr(rindex, TASK_MARK)) && (rend=strstr(rindex, TASK_VALUE_MARK)))
		{
			rindex = rindex + lenth;
			snprintf(szrname, (rend-rindex+1), "%s", rindex);

			if (!strcmp(szfname, szrname))
				break;
		}

		/*remove the task from downloader_file_info, downloader_finish_task nvram setting*/
		tw_Downloader_del_list("downloader_file_info", szfname);
		tw_Downloader_del_list("downloader_finish_task", szfname);

		/*remove the task relate files which output by transmission-cli*/
		tw_Downloader_get_mnt_path(mntpath);
		sprintf(szCmd, "rm -rf %s/downloader/%s", mntpath, szfname);
//		printf("[%s:%d]:szCmd=%s\n", __FUNCTION__, __LINE__, szCmd);
		system(szCmd);
	}
	return 1;
}
#endif

/*************************************************************************
功能: 开始任务的入口函数 
参数: 
返回: 
备注: 吴悠慧、林明 2013-3月创建
*************************************************************************/
int tw_Downloader_Start_Manage_Task(char *tasks,int speed)
{
	char szname[512] = {0};
	char task_buff[144*20] = {0};
	char new_buff[144*20] = {0};
	char szpath[1024] = {0};
	char *record = NULL;
	char *n_record = NULL;
	char *start = NULL;
	char *end = NULL;
	int lenth = strlen(TASK_MARK);	
    
	/*Init the param*/
    if( NULL != tasks )
    {
	    sprintf(new_buff, "%s", tasks);
	    tw_Downloader_Keep_New_Task(new_buff); //omit the repeat tasks
	    if(strlen(new_buff))
        {
            n_record = new_buff;
	    }
    }
	record = tw_nvram_get_force_from_nvram("downloader_waiting_task");
    
	if(check_num_of_tw_downloader_pid())
	{ 
		/*if the running task's number is less than DWNLOADER_MAX_TASK, then active the waiting task*/
		while( ( ((NULL!= record) && strstr(record,TASK_MARK)) || ((NULL != n_record) && strstr(n_record,TASK_MARK)) ) && check_num_of_tw_downloader_pid())
		{	
			//memset(task_buff, 0, sizeof(task_buff));
			/*if the waiting nvram param has the value, then wakeup up the task from nvram*/
			if((NULL!= record) && (start=strstr(record, TASK_MARK)))
			{
				start += lenth;
				if ((end=strstr(start, TASK_MARK)))
					snprintf(szname, (end-start+1), "%s", start);
				else
					sprintf(szname, "%s", start);

				record = end;
				
                tw_Downloader_del_list("downloader_waiting_task", szname);
			}
			/*if the waiting nvram param is null, then wakeup the task from new input param from webpage setting*/
			else if((NULL != n_record) && (start=strstr(n_record, TASK_MARK)))
			{
				start += lenth;
				if ((end=strstr(start, TASK_MARK)))
					snprintf(szname, (end-start+1), "%s", start);
				else
					sprintf(szname, "%s", start);
				
				n_record = end;
			}	
            if( NULL != szname && ( strstr(szname,HTTP_PREFIX_1) || strstr(szname,HTTP_PREFIX_2) || strstr(szname,HTTP_PREFIX_3) || strstr(szname,HTTP_PREFIX_4)) )
            {
                 tw_Downloader_start_wget(NULL, szname, HTTP_HTTPS,speed);
            }
            else if( NULL != szname && ( strstr(szname,FTP_PREFIX_1) || strstr(szname,FTP_PREFIX_2)) )
            {    
                 tw_Downloader_start_wget(NULL, szname, FTP,speed);
            }
            else
			{
                sprintf(szpath, BT_DOWNLOADER_TORRENT_PATH"/%s", szname);
			    tw_Downloader_start_Transmission(NULL, BT_DOWNLOADER_CONF_PATH, szpath,speed);
            }
            
            cprintf("[%s %d] start=%s\n",__FUNCTION__,__LINE__,szname);
			sleep(5);
		}
	}
    
    /*according to the record's status to reset the nvram's setting*/
	if (NULL != record && strstr(record,TASK_MARK) && NULL != new_buff)
	{ 
        sprintf(task_buff, "%s", record);
		sprintf(task_buff+strlen(task_buff), "%s", new_buff);
       
	}
	else if (NULL != n_record && strstr(n_record,TASK_MARK))
	{
		sprintf(task_buff, "%s", n_record);
	}
	else
	{
        memset(task_buff, 0, sizeof(task_buff));
	}   
	
	tw_nvram_unset_force("downloader_waiting_task");
	if(strlen(task_buff))
	{
		tw_nvram_set_force("downloader_waiting_task", task_buff);
	}
	return 1;
	
}

/*************************************************************************
功能: 暂停任务的入口函数 
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
int tw_Downloader_Pause_Manage_Task(char *tasks)
{
    char task[512] = {0};
    char url[512] = {0};
    while( NULL != (tasks = separate_tasks(tasks,task)))
    {
        cprintf("[%s %d] pause=%s\n",__FUNCTION__,__LINE__,task);
        if( NULL != task && ( strstr(task,HTTP_PREFIX_1) || strstr(task,HTTP_PREFIX_2) || strstr(task,HTTP_PREFIX_3) || strstr(task,HTTP_PREFIX_4) || strstr(task,FTP_PREFIX_1) || strstr(task,FTP_PREFIX_2)) )
        {
                 sprintf(url,"%s",task);
                 get_http_ftp_file_name(url, task);
        }
        tw_Downloader_pause(task);
        sleep(1);
    }
    tw_Downloader_Init_File_list(NULL, 1);
    return 0;
}

/*************************************************************************
功能: 删除任务的入口函数 
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
int tw_Downloader_Delete_Manage_Task(char *tasks)
{
    char task[512] = {0};
    char url[512] = {0};
    while( NULL != (tasks = separate_tasks(tasks,task)))
    {
        cprintf("[%s %d] delete=%s\n",__FUNCTION__,__LINE__,task);
        if( NULL != task && ( strstr(task,HTTP_PREFIX_1) || strstr(task,HTTP_PREFIX_2) || strstr(task,HTTP_PREFIX_3) || strstr(task,HTTP_PREFIX_4) || strstr(task,FTP_PREFIX_1) || strstr(task,FTP_PREFIX_2)) )
        {
                 sprintf(url,"%s",task);
                 get_http_ftp_file_name(url, task);
        }
        tw_Downloader_delete(NULL,task);
    }
    tw_Downloader_Init_File_list(NULL, 1);
    return 0;
}

/*************************************************************************
功能: 从传入的一串种子文件名字中，逐一获取每个种子文件的名字
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
char * separate_tasks( char *tasks,char *task)
{   
    char *start = NULL;
    char *end = NULL;
    int len = strlen(TASK_MARK);
    start = strstr(tasks,TASK_MARK);
    memset(task, 0, strlen(task));
    if( NULL == start )
    {
        return NULL;
    }
    else
    {
        start = start+len;
        end = strstr(start,TASK_MARK);
        if( NULL == end )
        {
            strcpy(task,start);
            return start;
        }
        else
        {
            
            strncpy(task,start,end-start);
            task[end-start] = '\0';
            return end;
        }          
    } 
}

/*************************************************************************
功能: 从一个文件内容中，读取一个文件的名字
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
FILE * get_file_name(FILE *fp, char *name)
{
    char strLine[1024] = {0};
    
    fgets(strLine, sizeof(strLine), fp);
    while(!feof(fp))
    {
		 if (strLine[0] != 'd')
		 {
			char *token = NULL;
			int lenth = 0;

			token = strtok(strLine, " ");
			while (token != NULL)
			{
				sprintf(name, "%s", token);
				token = strtok(NULL, " ");
			}
            
			if (0 != strlen(name))
			{
				lenth = strlen(name);
				name[lenth-1] = '\0';
                break;
			}
		 }
         fgets(strLine, sizeof(strLine), fp);
    }
    return fp;
}

/*************************************************************************
功能: 检查正在运行的任务数是否超过 DWNLOADER_MAX_TASK 个
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
int check_num_of_tw_downloader_pid()
{
    FILE *fp = NULL;
    char pid_file_name[256] = {0};
    int number = 0;
    system("ls -l /var/run > /var/tmp/pid_file");

    fp = fopen("/var/tmp/pid_file", "rt");
    if (fp)
    {
        while(!feof(fp = get_file_name(fp, pid_file_name)))
        {
           if(strstr(pid_file_name,PID_SUFFIX))
           {
              number++;
           }
        }
        fclose(fp);
    }
    if( number >= DWNLOADER_MAX_TASK )
        return 0;
    else 
        return 1;
}

/*************************************************************************
功能: 清空nvram和任务运行产生的pid文件
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
void empty_downloader_data()
{
    tw_nvram_unset_force("downloader_file_info");
    tw_nvram_unset_force("downloader_running_task");
    tw_nvram_unset_force("downloader_waiting_task");
    tw_nvram_unset_force("downloader_finish_task");
    tw_nvram_unset_force("downloader_tasks");
    tw_nvram_unset_force("action_bt");
    tw_nvram_unset_force("is_bigger");
    tw_nvram_unset_force("downloader_buf_task");
    system("cd /var/run;rm -rf *"PID_SUFFIX);
    system("cd /var/tmp;rm -rf tasks_files status_file  pid_file  down_task_max_port *.status");     
}

/*************************************************************************
功能: 查看任务是否正在运行
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
int check_pid_running(char *name)
{
    FILE *fp = NULL;
    char pid_file_name[256] = {0};
    char hex_name[256] = {0};
    system("ls -l /var/run > /var/tmp/pid_file");

    fp = fopen("/var/tmp/pid_file", "rt");
    if (fp)
    {
        while(!feof(fp = get_file_name(fp, pid_file_name)))
        {
           tw_Downloader_String2HexStr(pid_file_name,hex_name);
           if(strstr(hex_name,name))
           {
			   fclose(fp);
                return 1;
           }
        }
        fclose(fp);
    }
    return 0;
}

/*************************************************************************
功能: 增加一个http/ftp任务
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
int tw_downloader_add_http_ftp_info(char *url)
{
    char name[256] = {0};
    char hex_name[1024] = {0};
    char usb_path[256] = {0};    
    char strCmd[512] = {0};
    char file_info[640*10] = {0};
    char *index = NULL;
    int download_type = 0;
    char *usrname = NULL;
    char *password = NULL;
    get_http_ftp_file_name(url,name);//取得下载文件名字
    tw_Downloader_String2HexStr(name, hex_name);

    //get usb disk path and init all folder
	tw_Downloader_get_mnt_path(usb_path);
    sprintf(strCmd,"%s/%s",usb_path,DOWNLOADER_FOLDER);
    if( access(strCmd,F_OK) )
    {
        init_folder(strCmd);
    }
    
    //判断下载协议类型
    if( 'h' == url[0] || 'H' == url[0] )
    {
        download_type = HTTP_HTTPS;
    }
    else
    {
        download_type = FTP;
    }

    //将任务加入nvram的downloader_file_info
    index = tw_nvram_get_force_from_nvram("downloader_file_info");
	if (NULL != index && strstr(index,TASK_MARK))
	{
		sprintf(file_info, "%s", index);	
	}
	if ((NULL == index) || (!check_task_existent(index, hex_name,TASK_MARK)))//需要优化
	{
		/*construct the task's infomation, add then add to the nvram setting*/
		sprintf(file_info+strlen(file_info), 
		    TASK_MARK"%s"TASK_VALUE_MARK"%s"TASK_VALUE_MARK"%s"TASK_VALUE_MARK"%s"TASK_VALUE_MARK"%d"TASK_VALUE_MARK"%s",
		    hex_name, name, "/", DOWNLOADER_FILE_PATH,download_type, url);
        tw_nvram_unset_force("downloader_file_info");
		tw_nvram_set_force("downloader_file_info", file_info);
	}
	else
	{
		cprintf("The adding file is exist!");
		return 0;
	}

    usrname = tw_nvram_get_force_from_nvram("usrname_downloader");
    password = tw_nvram_get_force_from_nvram("password_downloader");
    
    //创建该任务的记录文件
    sprintf(strCmd,"%s/"DOWNLOADER_FOLDER"/%s/%s",usb_path,TASKS_FILE,hex_name);
    FILE * fp = NULL;
    if( NULL == ( fp = fopen(strCmd,"w+")) )
    {
        cprintf("[%s %d] open file fail!\n",__FUNCTION__,__LINE__);
        return -1;
    }
    fprintf(fp,FILE_NAME"=%s\n",name);
    fprintf(fp,FILE_SIZE"=%s\n","/");
    fprintf(fp,SAVE_PATH"=%s\n",DOWNLOADER_FILE_PATH);
    fprintf(fp,DONWLOAD_TYPE"=%d\n",download_type);
    fprintf(fp,DONWLOAD_TARGET"=%s\n",url);
    fprintf(fp,USRNAME"=%s\n",usrname);
    fprintf(fp,PASSWORD"=%s\n",password);
    fflush(fp);
    fclose(fp);

	return 1;
    
}


/*************************************************************************
功能: 从url中获取文件名
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
void get_http_ftp_file_name(char *url, char *name)
{
    char *start = url;
    //取得下载文件名字
    while( NULL != start && strstr(start,"/"))
    {
        start = start+1;
    }
    if( NULL != start )
    {
        sprintf(name,"%s",start);
    }
}

/*************************************************************************
功能: 加载所有任务
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
int add_all_tasks_info(char *file_name, char *task_name)
{
    char *file_info = NULL;
    char infobuf[640*10] = {0};
    char tmpbuf[512] = {0};
    char strLine[512] = {0};
	FILE *fp = NULL;
	
    file_info = tw_nvram_get_force_from_nvram("downloader_file_info");
	if (NULL != file_info && strstr(file_info,TASK_MARK))
	{
		sprintf(infobuf, "%s", file_info);	
	}
    sprintf(infobuf+strlen(infobuf), TASK_MARK"%s", task_name);

    if( NULL == ( fp = fopen(file_name, "rt")) )
    {
        cprintf("[%s %d] open file fail!\n",__FUNCTION__,__LINE__);
        return -1;
    }
	while(fgets(strLine, sizeof(strLine), fp))
	{
		if (strstr(strLine, FILE_NAME))
		{
			sscanf(strLine, FILE_NAME"=%s\n", tmpbuf);
            sprintf(infobuf+strlen(infobuf), TASK_VALUE_MARK"%s", tmpbuf);   
		}
		else if (strstr(strLine, FILE_SIZE))
		{
			sscanf(strLine, FILE_SIZE"=%s\n", tmpbuf);
			sprintf(infobuf+strlen(infobuf), TASK_VALUE_MARK"%s", tmpbuf);
		}
		else if (strstr(strLine, SAVE_PATH))
		{
			sscanf(strLine, SAVE_PATH"=%s\n", tmpbuf);
			sprintf(infobuf+strlen(infobuf), TASK_VALUE_MARK"%s", tmpbuf);
		}
        else if (strstr(strLine, DONWLOAD_TYPE))
		{
			sscanf(strLine, DONWLOAD_TYPE"=%s\n", tmpbuf);
			sprintf(infobuf+strlen(infobuf), TASK_VALUE_MARK"%s", tmpbuf);
		}
        else if (strstr(strLine, DONWLOAD_TARGET))
		{
			sscanf(strLine, DONWLOAD_TARGET"=%s\n", tmpbuf);
			sprintf(infobuf+strlen(infobuf), TASK_VALUE_MARK"%s", tmpbuf);
		}
	}

	/*resave the running and finished download task's nvram param*/
	tw_nvram_unset_force("downloader_file_info");
	tw_nvram_set_force("downloader_file_info", infobuf);	

    fclose(fp);
    return 1;
}

/*************************************************************************
功能: 开始一个http/https/ftp任务
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
int tw_Downloader_start_wget(char* savePath, char* url, int type, int speed)
{
    char strCmd[1024] = {0};
    char usb_path[128] = {0};
    char name[256] = {0};
    char usrname[64] = {0};
    char password[128] = {0};
    char hex_task[1024] = {0};
    FILE *fp =  NULL;
    speed = speed * 1000;
    tw_Downloader_get_mnt_path(usb_path);
    get_http_ftp_file_name(url, name);
    tw_Downloader_String2HexStr(name,hex_task);
    sprintf(strCmd,"%s/"DOWNLOADER_FOLDER"/"TASKS_FILE"/%s",usb_path,hex_task);
    if( NULL == ( fp = fopen(strCmd,"rt")) )
    {
        cprintf("[%s %d] open file fail!\n",__FUNCTION__,__LINE__);
        return -1;
    }
    while(fgets(strCmd, sizeof(strCmd), fp))
	{
		if (strstr(strCmd, USRNAME))
		{
			sscanf(strCmd, USRNAME"=%s\n", usrname);  
		}
        if (strstr(strCmd, PASSWORD))
		{
			sscanf(strCmd, PASSWORD"=%s\n", password); 
		}
    }
    fclose(fp);
    
    switch(type)
    {
        case FTP:
            if( NULL != usrname && strlen(usrname) && NULL != password && strlen(password) )
            {   
                if( 0 == speed )
                {
                    sprintf(strCmd,"wget -c -nd  --restrict-file-names=nocontrol --ftp-user=%s --ftp-password=%s -P %s/%s %s &",usrname,password,usb_path,DOWNLOADER_FILE_PATH,url);
                }
                else
                {
                    sprintf(strCmd,"wget -c -nd  --limit-rate=%d --restrict-file-names=nocontrol --ftp-user=%s --ftp-password=%s -P %s/%s %s &",speed,usrname,password,usb_path,DOWNLOADER_FILE_PATH,url);
                }
            }
            else
            {
                if( 0 == speed )
                {
                    sprintf(strCmd,"wget -c -nd --restrict-file-names=nocontrol -P %s/%s %s &",usb_path,DOWNLOADER_FILE_PATH,url);
                }
                else
                {
                    sprintf(strCmd,"wget -c -nd --limit-rate=%d --restrict-file-names=nocontrol -P %s/%s %s &",speed,usb_path,DOWNLOADER_FILE_PATH,url);
                }
            }
            system(strCmd);
            
            break;
            
        case HTTP_HTTPS:
            if( (NULL != usrname && strlen(usrname)) || (NULL != password && strlen(password)) )
            {
                if( 0 == speed )
                {
                    sprintf(strCmd,"wget -c -nd --restrict-file-names=nocontrol --http-user=%s --http-password=%s --no-check-certificat -P %s/%s %s &",usrname,password,usb_path,DOWNLOADER_FILE_PATH,url);
                }
                else
                {
                    sprintf(strCmd,"wget -c -nd --limit-rate=%d --restrict-file-names=nocontrol --http-user=%s --http-password=%s --no-check-certificat -P %s/%s %s &",speed,usrname,password,usb_path,DOWNLOADER_FILE_PATH,url);
                }
            }
            else
            {
                if( 0 == speed )
                {
                    sprintf(strCmd,"wget -c -nd --restrict-file-names=nocontrol --no-check-certificat -P %s/%s %s &",usb_path,DOWNLOADER_FILE_PATH,url);
                }
                else
                {
                    sprintf(strCmd,"wget -c -nd --limit-rate=%d --restrict-file-names=nocontrol --no-check-certificat -P %s/%s %s &",speed,usb_path,DOWNLOADER_FILE_PATH,url);
                }
            }
            system(strCmd);
            cprintf("[%s %d] Cmd=%s\n",__FUNCTION__,__LINE__,strCmd);
            break;
            
        default:
            cprintf("downloader type unknowed!\n");
    }
    return 1;
}

/*************************************************************************
功能: 建立文件夹
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
void init_folder(char *pathname)
{
    char strCmd[256] = {0};
    sprintf(strCmd,"mkdir -p %s",pathname);//downloader
    system(strCmd);

    sprintf(strCmd,"mkdir -p %s/%s",pathname,DOWNLOADER_FILE_FOLDER);//downloader/download
    system(strCmd);

    sprintf(strCmd,"mkdir -p %s/%s",pathname,STATUS);//downloader/status
    system(strCmd);

    sprintf(strCmd,"mkdir -p %s/%s",pathname,TASKS_FILE);//downloader/tasks_file
    system(strCmd);

    sprintf(strCmd,"mkdir -p %s/%s",pathname,BT_DOWNLOADER_TORRENT_FOLDER);//downloader/torrent
    system(strCmd);

    sprintf(strCmd,"mkdir -p %s/%s",pathname,BT_DOWNLOADER_CONF_FOLDER);//downloader/conf 
    system(strCmd);
}

/*************************************************************************
功能: 检查任务是否存在
参数: 
返回: 
备注: 林明 2013-3月创建
*************************************************************************/
int check_task_existent(char *Pstr, char *Dstr, char *flag)
{
    char *index = NULL;
    unsigned long length = 0;
    char tmp[16] = {0};
    if( NULL != Pstr && (index = strstr(Pstr,Dstr)) && NULL != Dstr)
    {
        length = strlen(Dstr);
        index = index + length;
        if( NULL != flag )
        {
            length = strlen(flag);
            snprintf(tmp,length+1,"%s",index);
            if( !strcmp(tmp,flag))
            {
                return 1;
            }
        }
    }
    return 0;
}
/*************************************************************************
功能: 去掉使用fget得到的字符串中存在的0x00
参数: in_buff字符数组，data_len长度
返回: 
备注: 王新中 2014-9月创建
*************************************************************************/
void  remove_out_0x00_dat(char* in_buff,int data_len)
{
     int str_len=0;
      int i;
	for(i=0;i<data_len;i++)
	{
		if(in_buff[i]!=0x00)
	          {
	          	in_buff[str_len]=in_buff[i];//逐个拷贝
			str_len++;
	          }
	} 
 }

/*end by wuyouhui at XX-XX-XX*/

