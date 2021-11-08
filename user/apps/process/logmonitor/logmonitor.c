#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h> /* for open */
#include <string.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/utsname.h> /* for uname */
#include <net/if_arp.h>
#include <dirent.h>
//#include <bcmnvram.h>
//#include <epivers.h>
//#include <router_version.h>
//#include <shutils.h>
#include "logmonitor.h"
#include "nvram.h"


int s_urlblk=0;

/******************************************************************
功能: 访问受限网站需要发送日志
参数:
        sig: 信号量
返回:
备注:
******************************************************************/
void LOGGER_URLBLK(int sig)
{
	if(!strcmp(nvram_safe_get("mail_ Alertsend"),"1"))
	{
		s_urlblk = 1;
	}
}
/******************************************************************
功能: 发送日志
返回: TBS_SUCCESS: 成功
其它值: 失败
备注:heyanping add
******************************************************************/
int LOGGER_SEND(void)
{
	int iRet;
	char szCmd[256] = {0};
	system("cat /var/log/sysevent.txt > /var/log/log_email.txt");
	system("cat /var/log/firewall.txt >> /var/log/log_email.txt");
	sprintf(szCmd, "nbsmtp -c /var/nbsmtprc -T %s -a %s -A %s","11AC_LOG","/var/log/log_email.txt","/var/log/log_email.txt");
	iRet=system(szCmd);
//	printf("iRet=%d",iRet);
	return iRet;
}
void logger_Sig_send(int sig)
{
	LOGGER_SEND();
}

/******************************************************************
功能: 安全写文件函数
参数:
        write函数的参数
返回: 写入的字节数
备注:
******************************************************************/
ssize_t LOGGER_SafeWrite(int iFD, const void *pszBuf, size_t ulCount)
{
    ssize_t n;

    do {
        n = write(iFD, pszBuf, ulCount);
    } while (n < 0 && errno == EINTR);

    return n;
}
/******************************************************************
功能: Write all of the supplied buffer out to a file.
        This does multiple writes as necessary.
        Returns the amount written, or -1 on an error.
参数:
        write函数的参数
返回: 写入的字节数
备注:
******************************************************************/
ssize_t LOGGER_FullWrite(int iFD, const void *pszBuf, size_t ulLen)
{
    ssize_t lCC;
    ssize_t lTotal;

    lTotal = 0;

    while (ulLen) {
        lCC = LOGGER_SafeWrite(iFD, pszBuf, ulLen);

        if (lCC < 0)
            return lCC;    /* write() returns -1 on failure. */

        lTotal += lCC;
        pszBuf = ((const char *)pszBuf) + lCC;
        ulLen -= lCC;
    }

    return lTotal;
}
/******************************************************************
功能: 文件截断函数
参数:
        *pszFile: 需要被截断的函数      lSize: 截断大小
返回: TBS_SUCCESS: 成功       其它值: 失败
备注:
******************************************************************/
int LOGGER_FileLockTruncate(const char *pszFile, long lSize)
{
    int iLogFD, iRet;
    struct flock stFLock;
    struct stat stFStat;
    char *pszBuf = NULL;

    if (lSize < 0 || NULL == pszFile)
    {
        return -1;
    }


    if ((iLogFD = open(pszFile, O_RDWR)) < 0)
    {
        return -2;
    }

     
    /* 使用记录锁实现对文件的独占访问 */
    stFLock.l_whence = SEEK_SET;
    stFLock.l_start = 0;
    stFLock.l_len = 1;
    stFLock.l_type = F_WRLCK;

    /* LOGGER_TRACE_PRINTF("INFO: Try to lock\n"); */
    if (fcntl(iLogFD, F_SETLK, &stFLock) < 0)
    {
        iRet = -8;
        goto OUT;
    }

    /* LOGGER_TRACE_PRINTF("INFO: locked\n"); */

    if (0L == lSize)
    {
        ftruncate(iLogFD, 0);

        iRet = 0;

        goto OUT;
    }

    if(fstat(iLogFD, &stFStat) < 0)
    {
        /* LOGGER_TRACE_PRINTF("ERROR: stat\n"); */
        iRet = -3;
        goto OUT;
    }

    /* 当文件大小超过指定值的 120% 时，将文件修改成指定的大小 */
    if (stFStat.st_size > (long)(lSize * 1.2))
    {
        long n, lRead = 0, lWritten = 0;
        char *pszPos = NULL;

        pszBuf = (char *)malloc(lSize);
        if (NULL == pszBuf)
        {
            /* Just truncate it to lSize */
            ftruncate(iLogFD, lSize);
            iRet = -4;
            goto OUT;
        }

        if (lseek(iLogFD, lSize * (-1), SEEK_END) < 0)
        {
            iRet = -5;
            goto OUT;
        }

        /* 读入 lSize 的内容到缓冲区 */
        pszPos = pszBuf;

        while ((n = read(iLogFD, pszPos, lSize - lRead)) > 0)
        {
            lRead += n;
            pszPos += n;

            if (lRead >= lSize)
                break;
        }

        if (n < 0)
        {
            iRet = -6;
            goto OUT;
        }

        ftruncate(iLogFD, 0);

        if (lseek(iLogFD, 0, SEEK_SET) < 0)
        {
            iRet = -9;
            goto OUT;
        }

        lWritten = LOGGER_FullWrite(iLogFD, pszBuf, lSize);
        if (lSize != lWritten)
        {
            iRet = -7;
            goto OUT;
        }

        free(pszBuf);
        pszBuf = NULL;
    }

    stFLock.l_type = F_UNLCK;
    fcntl(iLogFD, F_SETLK, &stFLock);
    /* LOGGER_TRACE_PRINTF("INFO: unlocked\n"); */

    close(iLogFD);

    return 0;

OUT:
    if (pszBuf)
        free(pszBuf);

    stFLock.l_type = F_UNLCK;
    fcntl(iLogFD, F_SETLK, &stFLock);
    /* LOGGER_TRACE_PRINTF("INFO: unlocked\n"); */

    close(iLogFD);

    return iRet;
}
/* 
使用于strace跟踪记录文件的控制，主要是防止log过大，造成内存耗尽
*/
int trace_log(char * path)
{
	struct stat stFStat;
	int f = -1;

	if ((f = open(path, O_RDWR)) < 0)
    {
    	return -1;
    }
	if(fstat(f, &stFStat) < 0)
    {
		close(f);
		return -1;
    }
	/* 当文件大小超过指定值的 TRACE_LOG_SIZE 时，将文件清除，再记录 */
   if (stFStat.st_size > (long)TRACE_LOG_SIZE)
    {
		close(f);
		remove(path);
		return -1;
    }
	close(f);
	return 0;

}


/******************************************************************
功能: 根据条件决定是否发送日志记录
参数:
        sig: 信号量
返回:
备注:heyanping add
******************************************************************/
void LOGGER_DoRecord(int sig)
{
	int iRet;
	struct tm *newtime;
	time_t longtime;
	char *sendtype=NULL;
	char *clocktime=NULL;
	char *weektime=NULL;
	char *week[7]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
	int szClocktime=0;
	int minutes=0;
	int szWeektime=0;
	struct stat stFStat;
	int iSrcFD = -1;
	unsigned long s_ulUncompLen = LOGGER_MAX_UNCOMP_FILE_LEN;


	/* 监测log文件大小 */	
	trace_log(TRACE_HTTP_LOG_FILE);
	
	time(&longtime);
	newtime=localtime(&longtime);
	

	sendtype = nvram_safe_get("mail_sendtype");
	clocktime = nvram_safe_get("mail_Daytime");
	weektime = nvram_safe_get("mail_Weektime");

#if 0
	printf("%d  %d-%d-%d-%d:%d:%d\n",           newtime->tm_wday,
												newtime->tm_year  +  1900,
												newtime->tm_mon  +  1,
												newtime->tm_mday,
												newtime->tm_hour,
												newtime->tm_min,
												newtime->tm_sec);
#endif

	if(!strcmp(sendtype,"2"))    //every hour
		if(newtime->tm_min == 0)
		{
			LOGGER_SEND();
		}
	if(!strcmp(sendtype,"3"))    //every day
	{
		sscanf(clocktime, "%d:%d", &szClocktime, &minutes);
		if(newtime->tm_min == 0&&newtime->tm_hour == szClocktime)
		{
			LOGGER_SEND();
		}
	}
	if(!strcmp(sendtype,"4"))    //every week
	{
		sscanf(clocktime, "%d:%d", &szClocktime, &minutes);
		if(!strcmp(week[newtime->tm_wday-1],weektime) && newtime->tm_hour == szClocktime && newtime->tm_min == 0)
		{
			LOGGER_SEND();
		}
	}
	if ((iSrcFD = open(LOG_EMAIL_FILE, O_RDWR)) < 0)
    {
        goto OUT;
    }
	if(fstat(iSrcFD, &stFStat) < 0)
    {
		close(iSrcFD);
        goto OUT;
    }
	/* 当文件大小超过指定值的 120% 时，将文件修改成指定的大小 */
   if (stFStat.st_size > (long)(s_ulUncompLen * 1.2))
    {
		if(!strcmp(sendtype,"1"))    //when full
		{
			LOGGER_SEND();
		}
        LOGGER_FileLockTruncate(LOG_EMAIL_FILE, s_ulUncompLen);
 

    }

	

	close(iSrcFD);
OUT:
    alarm(60);

    return ;
}


/* 
功能:通过进程名称获取进程号
创建/日期: 谢奕勇/2013-06-21
*/
int find_pid_by_name( char* ProcName, int* foundpid)  
{  
        DIR             *dir;  
        struct dirent   *d;  
        int             pid, i;  
        //int             pid; 
        char            *s;  
        int pnlen;  
  
        i = 0;  
        foundpid[0] = 0;  
        pnlen = strlen(ProcName);  
  
        /* Open the /proc directory. */  
        dir = opendir("/proc");  
        if (!dir)  
        {  
                printf("cannot open /proc");  
                return -1;  
        }  
  
        /* Walk through the directory. */  
        while ((d = readdir(dir)) != NULL) {  
  
                char exe [PATH_MAX+1];  
                char path[PATH_MAX+1];  
                int len;  
                int namelen;  
  
                /* See if this is a process */  
                if ((pid = atoi(d->d_name)) == 0)       continue;  
  
                snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);  
                if ((len = readlink(exe, path, PATH_MAX)) < 0)  
                        continue;  
                path[len] = '\0';  
  
                /* Find ProcName */  
                s = strrchr(path, '/');  
                if(s == NULL) continue;  
                s++;  
  
                /* we don't need small name len */  
                namelen = strlen(s);  
                if(namelen < pnlen)     continue;  
  
                if(!strncmp(ProcName, s, pnlen)) {  
                        /* to avoid subname like search proc tao but proc taolinke matched */  
                        if(s[pnlen] == ' ' || s[pnlen] == '\0') {  
                                foundpid[i] = pid;  
                                i++;  
                                //foundpid = pid;
                        }  
                }  
        }  
  
        foundpid[i] = 0;  
        closedir(dir);  
  
        return  0;  
  
}  
  

/* 
用于控制httpd进程，当httpd进程发生异常时，及时截获其上下文异常信息，利于调试分析定位问题
创建/日期: 谢奕勇/2013-06-21
*/
start_trace()
{
	pid_t http_id;
	char cmd[256] = {0};
	int i, rv, pid_t[128]; 

    memset(&cmd,0,sizeof(0));
	rv = find_pid_by_name("httpd",&pid_t);
	if(!rv)
	{  
        for(i=0; pid_t[i] != 0; i++)
        {
            http_id = pid_t[i];  
        }	   
	} 

	sprintf(cmd,"strace -o %s -i -f -p %d & ",TRACE_HTTP_LOG_FILE,http_id);
	system(cmd);

}



/******************************************************************
功能: logmonitor主函数
参数:
        argc: 参数个数      *argv: 参数数组
返回: TBS_SUCCESS: 成功       其它值: 失败
备注:
******************************************************************/
int main(int argc, const char *argv[])
{

 	
    signal(SIGALRM, LOGGER_DoRecord);
	signal(33, LOGGER_URLBLK);
	signal(34, logger_Sig_send);
	pid_t pid = -1;
    int   ret = -1;

//	start_trace();//start strace process add by xieyiyong in 2013-06-21 
	pid = fork();    
	if(0 < pid)    
	{        
		exit(0);    
	}
	if(0 == pid)//child process    
	{		
		LOGGER_DoRecord(0);        
		while(1)        
		{
			
			sleep(2);
        }    
	}
    
	return ret;
}
