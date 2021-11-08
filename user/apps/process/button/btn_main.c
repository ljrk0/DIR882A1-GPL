/*
 * button main (platform dependent portion)
 * 
 * writen by T&W sudh
 * 
 * $Id: btn_main.c 2012-12-22 10:31:40Z $
 */
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
//#include <bcmnvram.h>
#include <signal.h>
//#include <shutils.h>
#include "btn_gpio.h"
#include "common.h"
#include "nvram.h"

#define BTN_FLAG_SHUTDOWN		1
#define BTN_FLAG_RESTART_RC		2
#define BTN_FLAG_RESTART_SYS	4


#define BUTTONM_PID_FILE_PATH	"/tmp/btn_monitor.pid"

static int btn_flags;

/* termination handler, call by main function */
void
btn_stophandler_signed(int sig)
{
	btn_flags |= BTN_FLAG_SHUTDOWN;
	return;
}

/* restart rc */
void
btn_restart_rc_signed()
{
	btn_flags |= BTN_FLAG_SHUTDOWN | BTN_FLAG_RESTART_RC;
	return;
}

/* restart system */
void
btn_restart_sys_signed()
{
	btn_flags |= BTN_FLAG_SHUTDOWN | BTN_FLAG_RESTART_SYS;
	return;
}


void btn_do_restart_rc()
{
	char typeBuf[8] = { 0 };
	/* restart rc*/
	sprintf(typeBuf, "%d", ALL_TYPE);
	nvram_safe_set("RC_ModeType",typeBuf);//rc mode restart,lan mode.
	system("kill -1 1");
}

void btn_do_restart_sys()
{
	/* restart system*/
	system("reboot");
}


/*
 * Name        : main
 * Description : Main entry point for the wl button monitor
 * Arguments   : int argc, char *argv[] - command line parameters
 * Return type : int
 */
int
main(int argc, char* argv[])
{
	FILE *pidfile;
	char *wifi_enable = NULL;

	/* Show usage */
	if (argc > 1) {
		fprintf(stderr, "Usage: btn_monitor\n");
		return -1;
	}

	/*
	 * Check whether this process is running
	 */
	if ((pidfile = fopen(BUTTONM_PID_FILE_PATH, "r"))) {
		fprintf(stderr, "%s: wl_btn_monitor has been started\n", __FILE__);

		fclose(pidfile);
		return -1;
	}

	/* Cread pid file */
	if (pidfile = fopen(BUTTONM_PID_FILE_PATH, "w")) {
		fprintf(pidfile, "%d\n", getpid());
		fclose(pidfile);
	}
	else {
		perror("pidfile");
		exit(errno);
	}

	/* establish a handler to handle SIGTERM. */
	signal(SIGTERM, btn_stophandler_signed);

	btn_init();

	while (1) {
		if(btn_check())
		{
			printf("===> Hello button down!!! \n");
			if(wl_btn_pressed() == BTN_PRESS)
			{
				wifi_enable = nvram_safe_get("wifi_hardware_radio_enable");
				if(wifi_enable && !strcmp(wifi_enable, "0"))
				{
					nvram_safe_set("wifi_hardware_radio_enable", "1");
					nvram_safe_set("wl0_radio", "1");
					nvram_safe_set("wl1_radio", "1");
					#if 0
					wl_btn_dualband_wifi_on();
					#endif
				}
				else if(wifi_enable && !strcmp(wifi_enable, "1"))
				{
					nvram_safe_set("wifi_hardware_radio_enable", "0");
					nvram_safe_set("wl0_radio", "0");
					nvram_safe_set("wl1_radio", "0");
					#if 0
					wl_btn_dualband_wifi_off();
					#endif
				}

				nvram_commit(RT2860_NVRAM);
				btn_restart_rc_signed();
			}

			//reset action at last
			if(rs_btn_pressed() == BTN_PRESS)
			{
				nvram_safe_set("restore_defaults", "1");
				if(nvram_safe_get("vlan1portsbackup") && nvram_safe_get("vlan2portsbackup"))
				{
					//使用之前备份的vlanports的定义
					nvram_safe_set("vlan1ports",nvram_safe_get("vlan1portsbackup"));
					nvram_safe_set("vlan2ports",nvram_safe_get("vlan2portsbackup"));
				}
				nvram_commit(RT2860_NVRAM);
				btn_restart_sys_signed();
			}
		}

		/* check flag for shutdown */
		if (btn_flags & BTN_FLAG_SHUTDOWN) {
			btn_clean();
			break;
		}

		/*sleep some time (500ms) is need*/
		usleep(500*1000);
	}

	/* destroy pid file */
	unlink(BUTTONM_PID_FILE_PATH);

	/* do restart system first */
	if (btn_flags & BTN_FLAG_RESTART_SYS)
		btn_do_restart_sys();

	/* so restart rc*/
	if (btn_flags & BTN_FLAG_RESTART_RC)
		btn_do_restart_rc();

	return 0;
}
