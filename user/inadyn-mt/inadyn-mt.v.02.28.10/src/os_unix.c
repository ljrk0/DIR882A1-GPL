/*
Copyright (C) 2003-2004 Narcis Ilisei
Modifications by Steve Horbachuk
Copyright (C) 2006 Steve Horbachuk

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#define MODULE_TAG "OS_UNIX: "

#ifndef EXCLUDE_CONFIG_H

#include "config.h"

#endif

#include "debug_if.h"

#include "os.h"
#include "dyndns.h"
#include "lang.h"
#include "events.h"

#ifdef UNIX_OS
#include <unistd.h>
#include <sys/stat.h>
#include <locale.h>
#include <signal.h>

#include "debug_if.h"

void os_sleep_ms(int ms)
{
	usleep(ms*1000);
}

int  os_get_socket_error (void)
{
	return errno;
}

RC_TYPE os_ip_support_startup(void)
{
	return RC_OK;
}

RC_TYPE os_ip_support_cleanup(void)
{
	return RC_OK;
}

RC_TYPE os_shell_execute(char * p_cmd)
{
	RC_TYPE	rc = RC_OK;
	int	kid;
	switch((kid=vfork()))
	{
	case 0:
		/* child */
		execl("/bin/sh", "sh", "-c", p_cmd, (char *) 0);
		exit(1);

		break;
	case -1:
		rc = RC_OS_FORK_FAILURE;
		break;
	default:
		/* parent */
		break;
	}
	return rc;
}

/**
	install handler for SIGALRM and HUP, INT, QUIT.
	avoid receiving HIP,INT, QUIT during ALRM and
*/
RC_TYPE os_install_signal_handler(void *p_dyndns)
{
	RC_TYPE rc;
	struct sigaction    newact;

	/*point to handler in events.c*/
	newact.sa_handler = unix_signal_handler;
	newact.sa_flags   = 0;

	rc = sigemptyset(&newact.sa_mask)    ||

	sigaddset(&newact.sa_mask, SIGHUP)   ||
	sigaddset(&newact.sa_mask, SIGINT)   ||
	sigaddset(&newact.sa_mask, SIGQUIT)  ||

	sigaddset(&newact.sa_mask, SIGUSR1)  ||
	sigaddset(&newact.sa_mask, SIGPIPE)  ||

	sigaction(SIGALRM, &newact, NULL)    ||

	sigemptyset(&newact.sa_mask)         ||

	sigaddset(&newact.sa_mask, SIGALRM)  ||
	sigaction(SIGHUP, &newact, NULL)     ||
	sigaction(SIGINT, &newact, NULL)     ||
	sigaction(SIGQUIT, &newact, NULL)    ||

	sigaction(SIGUSR1, &newact, NULL);
	sigaction(SIGPIPE, &newact, NULL);

	if (rc != RC_OK)
	{
		DBG_PRINTF((LOG_WARNING,"DYNDNS: Error '%s' (0x%x) installing OS signal handler\n", rc));
	}

	return rc;
}

/*
  closes current console

  July 5th, 2004 - Krev
  ***
  This function is used to close the console window to start the
  software as a service on Windows. On Unix, closing the console window
  isn't used for a daemon, but rather it forks. Modified this function
  to fork into a daemon mode under Unix-compatible systems.

  Actions:
    - for child:
        - close in and err console
        - become session leader
        - change working directory
        - clear the file mode creation mask
    - for parent
        just exit
*/
RC_TYPE close_console_window(void)
{
	pid_t	pid = fork();
	int	chdir_ret;

	if(pid < 0)
	{
		return RC_OS_FORK_FAILURE;
	}

	if (pid == 0)
	{ /* child */
		fclose(stdin);
		fclose(stderr);
		setsid();
		chdir_ret=chdir("/");
		umask(0);
		return RC_OK;
	}
	else
	{
		exit(0);
	}
	return RC_OK;
}

/* MAIN - Dyn DNS update entry point.*/
int main(int argc, char* argv[])
{

	int retVal=0;


	init_lang_strings(NULL,setlocale(LC_ALL, ""));


	retVal=inadyn_main(argc, argv);


	dealloc_lang_strings();


	return retVal;
}

RC_TYPE os_syslog_open(const char *p_prg_name)
{
	openlog(LOG_DDNS, 0 , LOG_USER);
	return RC_OK;
}

RC_TYPE os_syslog_close(void)
{
	closelog();
	return RC_OK;
}

RC_TYPE os_change_persona(OS_USER_INFO *p_usr_info)
{
	int rc;
	do
	{
		if (p_usr_info->gid != -1)
		{
			if ((rc = setgid(p_usr_info->gid)) != 0)
			{
				break;
			}
		}

		if (p_usr_info->uid != -1)
		{
			if ((rc = setuid(p_usr_info->uid)) != 0)
			{
				break;
			}
		}
	}
	while(0);
	if (rc != 0)
	{
		DBG_PRINTF((LOG_WARNING,"E:" MODULE_TAG "Error changing uid/gid. OS err=0x%x\n",errno));
		return RC_OS_CHANGE_PERSONA_FAILURE;
	}
	return RC_OK;
}
#endif
