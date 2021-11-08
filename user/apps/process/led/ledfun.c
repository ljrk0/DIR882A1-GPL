#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <memory.h>
#include <malloc.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include "libled.h"
#include "common.h"

extern unsigned int regular_count;
unsigned int gset_all_test = 0;
unsigned int g_print = 1;

#ifdef TRUE
#undef TRUE

#define	TRUE	(g_print)

#endif



int cmd_led_regular(struct cli_def *cli, char *command, char *argv[], int argc)
{
    int i;
    cli_print(cli, "called %s with \"%s\"", __FUNCTION__, command);
    cli_print(cli, "%d arguments:", argc);
    for (i = 0; i < argc; i++)
        cli_print(cli, "        %s", argv[i]);

    return CLI_OK;
}
struct cli_command *cmd_find_led(struct cli_def *cli, char *command, char *argv[], int argc)
{
	struct cli_command *commands = cli->commands;
	struct cli_command *c = NULL, *again = NULL;
	int i, l = 0;
	char suffcommands[256] = {0}, suffcmd[128] = {0};
	char *pc = suffcmd;
		
	for (i = 0; i < argc; i++)
	{
		sprintf(pc," %s",argv[i]);
		pc += strlen(suffcmd);
	}
	sprintf(suffcommands, "%s %s", command, suffcmd);

	if(strstr(suffcommands, "test"))
	{
		gset_all_test = 1;
	}

	if(strstr(suffcommands, "-s"))
	{
		g_print = 0;
	}
	
	for (c = commands; c; c = c->next)
    {
    	l = strlen(c->command);
		
        if (strncasecmp(c->command, "led", l) == 0)
		{
			//cli_print(cli, "c->command : %s", c->command);
			return c;
		}
    }
	
    return NULL;
}

int cmd_led_regular_on(struct cli_def *cli, char *command, char *argv[], int argc)
{
	struct cli_command *commands = cmd_find_led(cli, command, argv, argc);	
	struct cli_command *c = NULL, *again = NULL;
	int l = 0;
    char szcmd[128] = {0};
	//char *cmd = szcmd;

	if(!commands) return 0;
	
    if (strstr(command, "power"))
	{
		if(strstr(command, "high"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_ON_CMD, LED_POWER_NG);
			twsystem(szcmd,TRUE);
		}else{
			snprintf(szcmd,sizeof(szcmd), LED_ON_CMD, LED_POWER_OK);
			twsystem(szcmd,TRUE);
		}
	}
	else if (strstr(command, "internet"))
	{
		if(strstr(command, "high"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_ON_CMD, LED_INTERNET_NG);
			twsystem(szcmd,TRUE);
		}else{
			snprintf(szcmd,sizeof(szcmd), LED_ON_CMD, LED_INTERNET_OK);
			twsystem(szcmd,TRUE);
		}
	}
	else if (strstr(command, "wps"))
	{
		snprintf(szcmd,sizeof(szcmd), LED_WLAN_2G_ON_CMD);
		twsystem(szcmd,TRUE);
		snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_ON_CMD);
		twsystem(szcmd,TRUE);
#if defined(__THIRD_WIFI_CARD_SUPPORT__)
		snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_2_ON_CMD);
		twsystem(szcmd,TRUE);
#endif
	}
	else if (strstr(command, "wifi"))
	{
		if(strstr(command, "low"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_WLAN_2G_ON_CMD);
			twsystem(szcmd,TRUE);
		}
		else if(strstr(command, "high2"))
		{
#if defined(__THIRD_WIFI_CARD_SUPPORT__)
			snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_2_ON_CMD);
			twsystem(szcmd,TRUE);
#endif
		}
		else if(strstr(command, "high"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_ON_CMD);
			twsystem(szcmd,TRUE);
		}
	}
	else if (strstr(command, "lan"))
	{
		
	}
	else if (strstr(command, "usb2"))
	{
#ifndef NOT_SUPPERT_USB2
		snprintf(szcmd,sizeof(szcmd), LED_ON_CMD, LED_USB2);
		twsystem(szcmd,TRUE);
#endif
	}
	else if (strstr(command, "usb3"))
	{
		snprintf(szcmd,sizeof(szcmd), LED_ON_CMD, LED_USB3);
		twsystem(szcmd,TRUE);
	}

	//cli_print(cli, "cmd_led_regular_on cmd : %s", szcmd);
    return CLI_OK;
}

int cmd_led_regular_off(struct cli_def *cli, char *command, char *argv[], int argc)
{
	struct cli_command *commands = cmd_find_led(cli, command, argv, argc);
	struct cli_command *c = NULL, *again = NULL;
	int l = 0;
    char szcmd[128] = {0};
	
	if(!commands) return 0;
	
    if (strstr(command, "power"))
	{
		if(strstr(command, "high"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_POWER_NG);
			twsystem(szcmd,TRUE);
		}else{
			snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_POWER_OK);
			twsystem(szcmd,TRUE);
		}
	}
	else if (strstr(command, "internet"))
	{
		if(strstr(command, "high"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_INTERNET_NG);
			twsystem(szcmd,TRUE);
		}else{
			snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_INTERNET_OK);
			twsystem(szcmd,TRUE);
		}
	}
	else if (strstr(command, "wps"))
	{
		snprintf(szcmd,sizeof(szcmd), LED_WLAN_2G_OFF_CMD);
		twsystem(szcmd,TRUE);
		snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_OFF_CMD);
		twsystem(szcmd,TRUE);
#if defined(__THIRD_WIFI_CARD_SUPPORT__)
		snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_2_OFF_CMD);
		twsystem(szcmd,TRUE);
#endif
	}
	else if (strstr(command, "wifi"))
	{
		if(strstr(command, "low"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_WLAN_2G_OFF_CMD);
			twsystem(szcmd,TRUE);
		}
		else if(strstr(command, "high2"))
		{
#if defined(__THIRD_WIFI_CARD_SUPPORT__)
			snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_2_OFF_CMD);
			twsystem(szcmd,TRUE);
#endif
		}
		else if(strstr(command, "high"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_OFF_CMD);
			twsystem(szcmd,TRUE);
		}
	}
	else if (strstr(command, "lan"))
	{
		
	}
	else if (strstr(command, "usb2"))
	{
#ifndef NOT_SUPPERT_USB2
		snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_USB2);
		twsystem(szcmd,TRUE);
#endif
	}
	else if (strstr(command, "usb3"))
	{
		snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_USB3);
		twsystem(szcmd,TRUE);
	}

	//cli_print(cli, "cmd_led_regular_off cmd : %s", szcmd);
    return CLI_OK;
}

int cmd_led_regular_blink(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
{
	struct cli_command *commands = cmd_find_led(cli, command, argv, argc);
	struct cli_command *c = NULL, *again = NULL;
	int l = 0;
    char szcmd[128] = {0};
	//char *cmd = szcmd;

	if(!commands) return 0;
	
    if (strstr(command, "power"))
	{
		if(strstr(command, "high"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_BLINK_CMD, LED_POWER_NG);
			twsystem(szcmd,TRUE);
		}else{
			snprintf(szcmd,sizeof(szcmd), LED_BLINK_CMD, LED_POWER_OK);
			twsystem(szcmd,TRUE);
		}
	}
	else if (strstr(command, "internet"))
	{
		if(strstr(command, "high"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_BLINK_CMD, LED_INTERNET_NG);
			twsystem(szcmd,TRUE);
		}else{
			snprintf(szcmd,sizeof(szcmd), LED_BLINK_CMD, LED_INTERNET_OK);
			twsystem(szcmd,TRUE);	
		}
	}
	else if (strstr(command, "wps"))
	{
		snprintf(szcmd,sizeof(szcmd), LED_WLAN_2G_BLINK_CMD);
		twsystem(szcmd,TRUE);
		snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_BLINK_CMD);
		twsystem(szcmd,TRUE);
#if defined(__THIRD_WIFI_CARD_SUPPORT__)
		snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_2_BLINK_CMD);
		twsystem(szcmd,TRUE);
#endif
	}
	else if (strstr(command, "wifi"))
	{
		if(strstr(command, "low"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_WLAN_2G_BLINK_CMD);
			twsystem(szcmd,TRUE);
		}
		else if(strstr(command, "high2"))
		{
#if defined(__THIRD_WIFI_CARD_SUPPORT__)
			snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_2_BLINK_CMD);
			twsystem(szcmd,TRUE);
#endif
		}
		else if(strstr(command, "high"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_WLAN_5G_BLINK_CMD);
			twsystem(szcmd,TRUE);
		}
	}
	else if (strstr(command, "lan"))
	{
		
	}
	else if (strstr(command, "usb2"))
	{
#ifndef NOT_SUPPERT_USB2
		snprintf(szcmd,sizeof(szcmd), LED_BLINK_CMD, LED_USB2);
		twsystem(szcmd,TRUE);
#endif
	}
	else if (strstr(command, "usb3"))
	{
		snprintf(szcmd,sizeof(szcmd), LED_BLINK_CMD, LED_USB3);
		twsystem(szcmd,TRUE);
	}

	//cli_print(cli, "cmd_led_regular_blink cmd : %s", szcmd);
    return CLI_OK;
}

int cmd_led_regular_blinkfast(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
{
	struct cli_command *commands = cmd_find_led(cli, command, argv, argc);
	struct cli_command *c = NULL, *again = NULL;
	int l = 0;
    char szcmd[128] = {0};
	//char *cmd = szcmd;

	if(!commands) return 0;
	
    if (strstr(command, "power"))
	{
		
	}
	else if (strstr(command, "internet"))
	{
		if(strstr(command, "high"))
		{
			snprintf(szcmd,sizeof(szcmd), LED_BLINK_FAST_CMD, LED_INTERNET_NG);
			twsystem(szcmd,TRUE);
		}else{
			snprintf(szcmd,sizeof(szcmd), LED_BLINK_FAST_CMD, LED_INTERNET_OK);
			twsystem(szcmd,TRUE);	
		}
	}
	else if (strstr(command, "wps"))
	{
		
	}
	else if (strstr(command, "wifi"))
	{
		
	}
	else if (strstr(command, "lan"))
	{
		
	}
	else if (strstr(command, "usb2"))
	{
		
	}
	else if (strstr(command, "usb3"))
	{
		
	}

	//cli_print(cli, "cmd_led_regular_blinkfast cmd : %s", szcmd);
    return CLI_OK;
}

int cmd_led_all_on(struct cli_def *cli, char *command, char *argv[], int argc)
{
	struct cli_command *commands = cmd_find_led(cli, command, argv, argc);
	struct cli_command *c = NULL, *again = NULL;
	int l = 0;
    char szcmd[128] = {0};
	//char *cmd = szcmd;

	if(!commands) return 0;


		unlink("/tmp/ban_led");
		//power
		snprintf(szcmd,sizeof(szcmd), LED_ON_CMD, LED_POWER_OK);
		twsystem(szcmd,TRUE);
		snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_POWER_NG);
		twsystem(szcmd,TRUE);
				
		//Internet /tmp/ban_led,timer
		
		//lan (1 2 3 4) switch
		snprintf(szcmd,sizeof(szcmd), "switch reg w 7d00 77777;switch reg w 7d04 77777");
		twsystem(szcmd,TRUE);
		
		//usb3.0 /var/cn1_usbevent
		if( gset_all_test || access(USB_CN1_FLAG_PATH, F_OK) == 0)
		{
			snprintf(szcmd,sizeof(szcmd), LED_ON_CMD, LED_USB3);
			twsystem(szcmd,TRUE);
		}

		//usb2.0 /var/cn12_usbevent
		if( gset_all_test || access(USB_CN12_FLAG_PATH, F_OK) == 0)
		{
#ifndef NOT_SUPPERT_USB2
			snprintf(szcmd,sizeof(szcmd), LED_ON_CMD, LED_USB2);
			twsystem(szcmd,TRUE);
#endif
		}

		twsystem(LED_WLAN_2G_ON_CMD,TRUE);
		twsystem(LED_WLAN_5G_ON_CMD,TRUE);
#if defined(__THIRD_WIFI_CARD_SUPPORT__)
		twsystem(LED_WLAN_5G_2_ON_CMD,TRUE);
#endif		


	//cli_print(cli, "cmd_led_regular_on cmd : %s", szcmd);
    return CLI_OK;
}

int cmd_led_all_off(struct cli_def *cli, char *command, char *argv[], int argc)
{
	struct cli_command *commands = cmd_find_led(cli, command, argv, argc);
	struct cli_command *c = NULL, *again = NULL;
	int l = 0;
    char szcmd[128] = {0};
	//char *cmd = szcmd;

	if(!commands) return 0;
	

		// /tmp/ban_led
		//snprintf(szcmd,sizeof(szcmd), "touch /tmp/ban_led");
		//twsystem(szcmd,TRUE);
		
		//power on/off
		snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_POWER_OK);
		twsystem(szcmd,TRUE);
		snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_POWER_NG);
		twsystem(szcmd,TRUE);
		
		//Internet /tmp/ban_led,timer
		snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_INTERNET_OK);
		twsystem(szcmd,TRUE);
		snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_INTERNET_NG);
		twsystem(szcmd,TRUE);
		
		//wps (wlan driver)
		//(none)
		twsystem(LED_WLAN_2G_OFF_CMD,TRUE);
		twsystem(LED_WLAN_5G_OFF_CMD,TRUE);
#if defined(__THIRD_WIFI_CARD_SUPPORT__)
		twsystem(LED_WLAN_5G_2_OFF_CMD,TRUE);
#endif

		//lan (1 2 3 4) switch
		snprintf(szcmd,sizeof(szcmd), "switch reg w 7d00 11111;switch reg w 7d04 66666;switch reg w 7d10 11111;switch reg w 7d14 11111;switch reg w 7d18 10111");
		twsystem(szcmd,TRUE);
		
		//usb3.0 /var/cn1_usbevent
		if( gset_all_test || access(USB_CN1_FLAG_PATH, F_OK) == 0)
		{
			snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_USB3);
			twsystem(szcmd,TRUE);
		}		

		//usb2.0 /var/cn12_usbevent
		if( gset_all_test || access(USB_CN12_FLAG_PATH, F_OK) == 0)
		{
#ifndef NOT_SUPPERT_USB2
			snprintf(szcmd,sizeof(szcmd), LED_OFF_CMD, LED_USB2);
			twsystem(szcmd,TRUE);
#endif
		}


	//cli_print(cli, "cmd_led_all_off cmd : %s", szcmd);
    return CLI_OK;
}

int cmd_led_all_blink(struct cli_def *cli, char *command, char *argv[], int argc)
{
	struct cli_command *commands = cmd_find_led(cli, command, argv, argc);
	struct cli_command *c = NULL, *again = NULL;
	int l = 0;
    char szcmd[128] = {0};
	//char *cmd = szcmd;

	if(!commands) return 0;
	

	//cli_print(cli, "cmd_led_all_blink cmd : %s", szcmd);
    return CLI_OK;
}


int cli_register_led(struct cli_def *cli)
{
    struct cli_command *c;
    struct cli_command *o;
    struct cli_command *m, *n, *p;

	if(!cli) return;
	
  c = cli_register_command(cli, NULL, "led", NULL, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show led");

    o = cli_register_command(cli, c, "set", cmd_led_regular, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show set led");
    	m = cli_register_command(cli, o, "all", cmd_led_regular,PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show set all led");
		
	    cli_register_command(cli, m, "on", cmd_led_all_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show set all led start");
		cli_register_command(cli, m, "off", cmd_led_all_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show set all led stop");
	    cli_register_command(cli, m, "blink", cmd_led_all_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show set all led blink");

		n = cli_register_command(cli, m, "test", cmd_led_all_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show set all led test");

		    cli_register_command(cli, n, "on", cmd_led_all_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show set all led start");
			cli_register_command(cli, n, "off", cmd_led_all_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show set all led stop");
		    cli_register_command(cli, n, "blink", cmd_led_all_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show set all led blink");
		
    o = cli_register_command(cli, c, "power", cmd_led_regular, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show power led");
	
	    cli_register_command(cli, o, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show power led start");
		cli_register_command(cli, o, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show power led stop");
	    cli_register_command(cli, o, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show power led blink");

    	m = cli_register_command(cli, o, "high", cmd_led_regular,PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show power high led");
			
		    cli_register_command(cli, m, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show power high led start");
			cli_register_command(cli, m, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show power high led stop");
		    cli_register_command(cli, m, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show power high led blink");

    o = cli_register_command(cli, c, "internet", cmd_led_regular, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show internet led");

	    cli_register_command(cli, o, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show internet led start");
		cli_register_command(cli, o, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show internet led stop");
	    cli_register_command(cli, o, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show internet led blink");
	    cli_register_command(cli, o, "blinkfast", cmd_led_regular_blinkfast, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show internet led blinkfast");

    	m = cli_register_command(cli, o, "high", cmd_led_regular,PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show internet high led");
			
		    cli_register_command(cli, m, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show internet high led start");
			cli_register_command(cli, m, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show internet high led stop");
		    cli_register_command(cli, m, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show internet high led blink");
		    cli_register_command(cli, m, "blinkfast", cmd_led_regular_blinkfast, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show internet high led blinkfast");

    o = cli_register_command(cli, c, "wps", cmd_led_regular, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wps led");

	    cli_register_command(cli, o, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wps led start");
		cli_register_command(cli, o, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wps led stop");
	    cli_register_command(cli, o, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wps led blink");

    o = cli_register_command(cli, c, "wifi", cmd_led_regular,PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show wifi led");

    	m = cli_register_command(cli, o, "low", cmd_led_regular,PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show wifi low led (2.4G)");
    	n = cli_register_command(cli, o, "high", cmd_led_regular,PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show wifi high led (5G)");
    	p = cli_register_command(cli, o, "high2", cmd_led_regular,PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show wifi high2 led (5G_2)");

	    cli_register_command(cli, m, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wifi low led start (2.4G)");
		cli_register_command(cli, m, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wifi low led stop (2.4G)");
	    cli_register_command(cli, m, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wifi low led blink (2.4G)");

	    cli_register_command(cli, n, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wifi high led start (5G)");
		cli_register_command(cli, n, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wifi high led stop (5G)");
	    cli_register_command(cli, n, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wifi high led blink (5G)");

	    cli_register_command(cli, p, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wifi high2 led start (5G_2)");
		cli_register_command(cli, p, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wifi high2 led stop (5G_2)");
	    cli_register_command(cli, p, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show wifi high2 led blink (5G_2)");

    o = cli_register_command(cli, c, "lan", cmd_led_regular,PRIVILEGE_UNPRIVILEGED, MODE_EXEC,"Show lan led");

	    cli_register_command(cli, o, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show lan led start");
		cli_register_command(cli, o, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show lan led stop");
	    cli_register_command(cli, o, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show lan led blink");

    o = cli_register_command(cli, c, "usb2", cmd_led_regular, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show usb2 led");

	    cli_register_command(cli, o, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show usb2 led start");
		cli_register_command(cli, o, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show usb2 led stop");
	    cli_register_command(cli, o, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show usb2 led blink");

    o = cli_register_command(cli, c, "usb3", cmd_led_regular, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show usb3 led");

	    cli_register_command(cli, o, "on", cmd_led_regular_on, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show usb3 led start");
		cli_register_command(cli, o, "off", cmd_led_regular_off, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show usb3 led stop");
	    cli_register_command(cli, o, "blink", cmd_led_regular_blink, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, "Show usb3 led blink");

}




