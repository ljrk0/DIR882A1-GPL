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
#include <errno.h>
#include <stdarg.h>
#include <memory.h>
#include <malloc.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include "libled.h"

#define CLITEST_PORT                8000
//#define CLITEST_PORT                23
#define MODE_CONFIG_INT             10

#ifdef __GNUC__
# define UNUSED(d) d __attribute__ ((unused))
#else
# define UNUSED(d) d
#endif

unsigned int regular_count = 0;
unsigned int debug_regular = 0;


int cmd_test(struct cli_def *cli, char *command, char *argv[], int argc)
{
    int i;
    cli_print(cli, "called %s with \"%s\"", __FUNCTION__, command);
    cli_print(cli, "%d arguments:", argc);
    for (i = 0; i < argc; i++)
        cli_print(cli, "        %s", argv[i]);

    return CLI_OK;
}

int systemCmd(struct cli_def *cli, char *pcmd)
{
    char pbuf[256] = {0};
    char *pstr;
	int iStatus = 0;
	
    if(! pcmd)
		return -1;
	
    FILE *pFile = popen(pcmd, "r");
    if(pFile != NULL)
	{
		//fgets(pbuf, 255, pFile);
		while (NULL != fgets(pbuf, sizeof(pbuf), pFile))
		{
			cli_bufprint(cli, "%s", pbuf);
			//cli_print(cli, "%s", pbuf);
		}
		cli_print(cli, "  ");
		pclose(pFile);
		return 0;
	}
#if 0
	iStatus = pclose(pFile); 
	if (!WIFEXITED(iStatus) || 0 != WEXITSTATUS(iStatus))
	{
		cli_print(cli, "        %d", iStatus);
	}
#endif
	return -1;
}
int CLI_Shell()
{
    /* ¿ªÆôÖÕ¶Ë»ØÏÔ */
    CLI_TermEchoOn();

    printf("\n");
    system("sh");

    /* ¹Ø±ÕÖÕ¶Ë»ØÏÔ */
    CLI_TermEchoOff();

    return 0;
}
int cmd_shell(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
{
	char cmd_buf[128]={0};
    int n = 0,len = sizeof(cmd_buf);
	
    int i;

	n += snprintf(cmd_buf + n, len - n, "%s ", command);
		
    for (i = 0; i < argc; i++)
	{
		n += snprintf(cmd_buf + n, len - n, "%s ", argv[i]);
    }

	cli_print(cli, "        %s", cmd_buf);
	
	CLI_Shell();

    return CLI_OK;
}
int cmd_ifconfig(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
{
	char cmd_buf[128]={0};
    int n = 0,len = sizeof(cmd_buf);
	
    int i;

	n += snprintf(cmd_buf + n, len - n, "%s ", command);
		
    for (i = 0; i < argc; i++)
	{
		n += snprintf(cmd_buf + n, len - n, "%s ", argv[i]);
    }
	
	systemCmd(cli,cmd_buf);

    return CLI_OK;
}

int cmd_iwpriv(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
{
	char cmd_buf[128]={0};
    int n = 0,len = sizeof(cmd_buf);
	
    int i;
	
	n += snprintf(cmd_buf + n, len - n, "%s ", command);
		
    for (i = 0; i < argc; i++)
	{
		
		n += snprintf(cmd_buf + n, len - n, "%s ", argv[i]);
    }
	systemCmd(cli,cmd_buf);

    return CLI_OK;
}
#define BIGBUF_SIZE (8192)
int cmd_led(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
{
	char *cmd_buf = NULL;
    int n = 0,len = BIGBUF_SIZE;
	
    int i;
	
    if ((cmd_buf = malloc(BIGBUF_SIZE)) == NULL)
	{
		cli_print(cli, "error %d arguments:", argc);
        return CLI_ERROR;
    }
	memset(cmd_buf,0,BIGBUF_SIZE);

	cli_print(cli, "argc %d arguments: %s", argc,argv[0]);
	
	n += snprintf(cmd_buf + n, len - n, "%s ", command);
		
    for (i = 0; i < argc; i++)
	{
		n += snprintf(cmd_buf + n, len - n, "%s ", argv[i]);
    }

	cli_print(cli, "argc %d cmd_buf: %s", argc,cmd_buf);
	
	systemCmd(cli,cmd_buf);

	free_z(cmd_buf);	
    return CLI_OK;
}

int cmd_set(struct cli_def *cli, UNUSED(char *command), char *argv[],
    int argc)
{
    if (argc < 2 || strcmp(argv[0], "?") == 0)
    {
        cli_print(cli, "Specify a variable to set");
        return CLI_OK;
    }

    if (strcmp(argv[1], "?") == 0)
    {
        cli_print(cli, "Specify a value");
        return CLI_OK;
    }

    if (strcmp(argv[0], "regular_interval") == 0)
    {
        unsigned int sec = 0;
        if (!argv[1] && !&argv[1])
        {
            cli_print(cli, "Specify a regular callback interval in seconds");
            return CLI_OK;
        }
        sscanf(argv[1], "%d", &sec);
        if (sec < 1)
        {
            cli_print(cli, "Specify a regular callback interval in seconds");
            return CLI_OK;
        }
        cli->timeout_tm.tv_sec = sec;
        cli->timeout_tm.tv_usec = 0;
        cli_print(cli, "Regular callback interval is now %d seconds", sec);
        return CLI_OK;
    }

    cli_print(cli, "Setting \"%s\" to \"%s\"", argv[0], argv[1]);
    return CLI_OK;
}

int cmd_config_int(struct cli_def *cli, UNUSED(char *command), char *argv[],
    int argc)
{
    if (argc < 1)
    {
        cli_print(cli, "Specify an interface to configure");
        return CLI_OK;
    }

    if (strcmp(argv[0], "?") == 0)
        cli_print(cli, "  test0/0");

    else if (strcasecmp(argv[0], "test0/0") == 0)
        cli_set_configmode(cli, MODE_CONFIG_INT, "test");
    else
        cli_print(cli, "Unknown interface %s", argv[0]);

    return CLI_OK;
}

int cmd_config_int_exit(struct cli_def *cli, UNUSED(char *command),
    UNUSED(char *argv[]), UNUSED(int argc))
{
    cli_set_configmode(cli, MODE_CONFIG, NULL);
    return CLI_OK;
}

int cmd_show_regular(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
{
    cli_print(cli, "cli_regular() has run %u times", regular_count);
    return CLI_OK;
}

int cmd_show_regular_start(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
{
    cli_print(cli, "cli_regular_start() has run %u times", regular_count);
    return CLI_OK;
}

int cmd_show_regular_stop(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
{
    cli_print(cli, "cli_regular_stop() has run %u times", regular_count);
    return CLI_OK;
}

int cmd_debug_regular(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
{
    debug_regular = !debug_regular;
    cli_print(cli, "cli_regular() debugging is %s", debug_regular ? "enabled" : "disabled");
    return CLI_OK;
}

int check_auth(char *username, char *password)
{
    if (strcasecmp(username, "root") != 0)
        return CLI_ERROR;
    if (strcasecmp(password, "root") != 0)
        return CLI_ERROR;
    return CLI_OK;
}

int cli_set_privilege_passwd(struct cli_def *cli)
{
	char *passwd = "admin";
	
	cli_allow_enable(cli, passwd);
	
	return 1;
}

int regular_callback(struct cli_def *cli)
{
    regular_count++;
    if (debug_regular)
    {
        cli_print(cli, "Regular callback - %u times so far", regular_count);
        cli_reprompt(cli);
    }
    return CLI_OK;
}

int check_enable(char *password)
{
    return !strcasecmp(password, "topsecret");
}

void pc(UNUSED(struct cli_def *cli), char *string)
{
    printf("%s\n", string);
}

int cli_register_led(struct cli_command *cli)
{
    struct cli_command *c;
    struct cli_command *o;

	if(!cli) return;
	
    c = cli_register_command(cli, NULL, "led", NULL, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, 
    	NULL);

    o = cli_register_command(cli, c, "power", NULL, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, 
		"Show power led");
	
    cli_register_command(cli, o, "start", cmd_show_regular_start, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, 
		"Show power led start");
	
    cli_register_command(cli, o, "stop", cmd_show_regular_stop, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, 
		"Show power led stop");

    cli_register_command(cli, c, "internet", cmd_test, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, 
		"Show internet led");

    cli_register_command(cli, c, "wps", cmd_test, PRIVILEGE_UNPRIVILEGED,MODE_EXEC, 
		"Show wps led");

    cli_register_command(cli, NULL, "wifi", cmd_config_int,PRIVILEGE_PRIVILEGED, MODE_CONFIG, 
		"Show wifi led");

    cli_register_command(cli, NULL, "lan", cmd_config_int_exit,PRIVILEGE_PRIVILEGED, MODE_CONFIG_INT,
        "Show lan led");

    cli_register_command(cli, NULL, "usb", cmd_test, PRIVILEGE_PRIVILEGED,MODE_CONFIG_INT, 
		"Show usb led");
}

int main(int argc, char *argv[])
{
    struct cli_command *c;
    struct cli_command *o;
    struct cli_def *cli;
    int s, x, r=STDIN_FILENO, w=STDOUT_FILENO;
    struct sockaddr_in addr;
    int on = 1;


	char cli_cmd[128] = {0};
	char *p = cli_cmd;
	int i = 0;
	for(i=0;i<argc;i++)
	{
		sprintf(p,"%s ",argv[i]);
		p += strlen(p);
	}
	CLI_MSG("cli_cmd:%s ",cli_cmd);

    cli = cli_init();
    cli_set_banner(cli, "libcli test environment");
    cli_set_hostname(cli, "router");
    cli_regular(cli, regular_callback);
    cli_regular_interval(cli, 5); // Defaults to 1 second
    cli_set_idle_timeout(cli, 300); // 60 second idle timeout

	#if 0
    cli_register_command(cli, NULL, "led", cmd_led, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "protest cmd");

    cli_register_command(cli, NULL, "iwpriv", cmd_iwpriv, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "iwpriv cmd");

    cli_register_command(cli, NULL, "ifconfig", cmd_ifconfig, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "ifconfig cmd");
	
    cli_register_command(cli, NULL, "sh", cmd_shell, PRIVILEGE_PRIVILEGED,
        MODE_EXEC, "sh cmd");
	#endif

	cli_register_led(cli);


    //cli_set_auth_callback(cli, check_auth); // no auth required
    cli_set_enable_callback(cli, check_enable);
	cli_set_privilege_passwd(cli);
    // Test reading from a file
    {
        FILE *fh;

        if ((fh = fopen("clitest.txt", "r")))
        {
            // This sets a callback which just displays the cli_print() text to stdout
            cli_print_callback(cli, pc);
            cli_file(cli, fh, PRIVILEGE_UNPRIVILEGED, MODE_EXEC);
            cli_print_callback(cli, NULL);
            fclose(fh);
        }
    }

	
    {

        cli_loop_cli(cli, r, w, cli_cmd);
        exit(0);

    }

    cli_done(cli);
    return 0;
}
