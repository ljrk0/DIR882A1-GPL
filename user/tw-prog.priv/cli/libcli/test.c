#include <stdio.h>
#include <sys/types.h>
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <memory.h>
#include <malloc.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>

#include "libcli.h"




#define        max(a,b)        ((a) > (b) ? (a) : (b))
static int m_iEchoOff = 0;
static struct termios stored_settings;
/*********************************************************
����: CLI_TermEchoOn
����: �����ն˻���

*********************************************************/
void  CLI_TermEchoOn()
{
    if (m_iEchoOff)
    {
        tcsetattr(0, TCSANOW, &stored_settings);
        m_iEchoOff = 0;
    }
}

/*********************************************************
����: CLI_TermEchoOff
����: �ر��ն˻���

*********************************************************/
void  CLI_TermEchoOff()
{
    if (!m_iEchoOff)
    {
        struct termios new_settings;
        tcgetattr(0,&stored_settings);  //��ȡ�ͱ����ն�״̬
        new_settings = stored_settings;
        new_settings.c_lflag &= (~ECHO);  //�رջ���
        /* Disable canonical mode, and set buffer size to 1 byte */
        new_settings.c_lflag &= (~ICANON);  //�رձ�׼����ģʽ
        new_settings.c_cc[VTIME] = 0;  //�ǹ淶ģʽ��ȡʱ�ĳ�ʱʱ��
        new_settings.c_cc[VMIN] = 1; //�ǹ淶ģʽ��ȡʱ����С�ַ���
        tcsetattr(0, TCSANOW, &new_settings);  //�����µ��ն�״̬
        m_iEchoOff = 1;
    }
}


int main()
{
    int sr;
    fd_set rset;
	int n, l, c;
	int maxfd, rsockfd = 0;

    struct timeval tm;
	
    // Default to 5 second timeout intervals
    tm.tv_sec = 5;
    tm.tv_usec = 0;
	
    FD_ZERO(&rset);
    FD_SET(rsockfd, &rset);
    //FD_SET(5, &rset);
	CLI_MSG("rsockfd=%d ",rsockfd);

	maxfd = max(5, rsockfd) + 1;

	CLI_TermEchoOff();
	
	#if 0
	while (1)
	{
	    if ((sr = select(maxfd, &rset, NULL, NULL, &tm)) < 0)
	    {
	        /* select error */
	        if (errno == EINTR)
	            continue;

	        perror("select");
	        l = -1;
	        break;
	    }
		if (FD_ISSET(rsockfd, &rset)) 
		{ 
		    if ((n = read(rsockfd, &c, 1)) < 0)
		    {
		        if (errno == EINTR)
		            continue;

		        perror("read");
		        l = -1;
		        break;
		    }
			{
				CLI_MSG("0x%2x ",c);
			}
		}
		if (FD_ISSET(5, &rset)) 
		{
			
		    if ((n = read(5, &c, 1)) < 0)
		    {
		        if (errno == EINTR)
		            continue;

		        perror("read");
		        l = -1;
		        break;
		    }
			{
				CLI_MSG("0x%2x ",c);
			}
		}
		{
			CLI_MSG("0x%2x ",c);
		}
	}
	#else 
	while(1)
	{

		c = getchar();
		CLI_MSG("0x%2x ",c);
	}
	

	#endif
}



