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
#include "libcli.h"

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

#ifdef WIN32
typedef int socklen_t;

int winsock_init()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    // Start up sockets
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        return 0;
    }

    /*
     * Confirm that the WinSock DLL supports 2.2
     * Note that if the DLL supports versions greater than 2.2 in addition to
     * 2.2, it will still return 2.2 in wVersion since that is the version we
     * requested.
     * */
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        WSACleanup();
        return 0;
    }
    return 1;
}
#endif

int cmd_test(struct cli_def *cli, char *command, char *argv[], int argc)
{
    int i;
    cli_print(cli, "called %s with \"%s\"", __FUNCTION__, command);
    cli_print(cli, "%d arguments:", argc);
    for (i = 0; i < argc; i++)
        cli_print(cli, "        %s", argv[i]);

    return CLI_OK;
}
static int m_iEchoOff = 0;
static struct termios stored_settings;
/*********************************************************
函数: CLI_TermEchoOn
作用: 开启终端回显

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
函数: CLI_TermEchoOff
作用: 关闭终端回显

*********************************************************/
void  CLI_TermEchoOff()
{
    if (!m_iEchoOff)
    {
        struct termios new_settings;
        tcgetattr(0,&stored_settings);  //获取和保存终端状态
        new_settings = stored_settings;
        new_settings.c_lflag &= (~ECHO);  //关闭回显
        /* Disable canonical mode, and set buffer size to 1 byte */
        new_settings.c_lflag &= (~ICANON);  //关闭标准输入模式
        new_settings.c_cc[VTIME] = 0;  //非规范模式读取时的超时时间
        new_settings.c_cc[VMIN] = 1; //非规范模式读取时的最小字符数
        tcsetattr(0, TCSANOW, &new_settings);  //设置新的终端状态
        m_iEchoOff = 1;
    }
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
    /* 开启终端回显 */
    CLI_TermEchoOn();

    printf("\n");
    system("sh");

    /* 关闭终端回显 */
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
int cmd_iwconfig(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
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
int cmd_reboot(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
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

int cmd_brctl(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
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

int cmd_ated(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
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

int cmd_ping(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
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
int cmd_protest(struct cli_def *cli, UNUSED(char *command), char *argv[], int argc)
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
#if 1
    //cli_print(cli, "called %s with \"%s\"", __FUNCTION__, command);
    //cli_print(cli, "%d arguments:", argc);


	n += snprintf(cmd_buf + n, len - n, "%s ", command);
		
    for (i = 0; i < argc; i++){
        //cli_print(cli, "        %s", argv[i]);
		
		n += snprintf(cmd_buf + n, len - n, "%s ", argv[i]);
    }
    //cli_print(cli, "        %s", cmd_buf);
#endif
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

int main()
{
    struct cli_command *c;
    struct cli_def *cli;
    int s, x, r=STDIN_FILENO, w=STDOUT_FILENO;
    struct sockaddr_in addr;
    int on = 1;

#ifndef WIN32
    signal(SIGCHLD, SIG_IGN);
#endif
#ifdef WIN32
    if (!winsock_init()) {
        printf("Error initialising winsock\n");
        return 1;
    }
#endif

    cli = cli_init();
    cli_set_banner(cli, "libcli test environment");
    cli_set_hostname(cli, "router");
    cli_regular(cli, regular_callback);
    cli_regular_interval(cli, 5); // Defaults to 1 second
    cli_set_idle_timeout(cli, 300); // 60 second idle timeout

	
    cli_register_command(cli, NULL, "protest", cmd_protest, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "protest cmd");

    cli_register_command(cli, NULL, "iwpriv", cmd_iwpriv, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "iwpriv cmd");

    cli_register_command(cli, NULL, "ifconfig", cmd_ifconfig, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "ifconfig cmd");

    cli_register_command(cli, NULL, "iwconfig", cmd_iwconfig, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "iwconfig cmd");
	
    cli_register_command(cli, NULL, "reboot", cmd_reboot, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "reboot cmd");
	
    cli_register_command(cli, NULL, "brctl", cmd_brctl, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "brctl cmd");

	cli_register_command(cli, NULL, "ated", cmd_ated, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "ated cmd");
	
	cli_register_command(cli, NULL, "ping", cmd_ping, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "ping cmd");

    cli_register_command(cli, NULL, "sh", cmd_shell, PRIVILEGE_PRIVILEGED,
        MODE_EXEC, "sh cmd");
	
	#if 0
    cli_register_command(cli, NULL, "test", cmd_test, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, NULL);

	
    cli_register_command(cli, NULL, "simple", NULL, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, NULL);

    cli_register_command(cli, NULL, "simon", NULL, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, NULL);

    cli_register_command(cli, NULL, "set", cmd_set, PRIVILEGE_PRIVILEGED,
        MODE_EXEC, NULL);

    c = cli_register_command(cli, NULL, "show", NULL, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, NULL);

    cli_register_command(cli, c, "regular", cmd_show_regular, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "Show the how many times cli_regular has run");

    cli_register_command(cli, c, "counters", cmd_test, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "Show the counters that the system uses");

    cli_register_command(cli, c, "junk", cmd_test, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, NULL);

    cli_register_command(cli, NULL, "interface", cmd_config_int,
        PRIVILEGE_PRIVILEGED, MODE_CONFIG, "Configure an interface");

    cli_register_command(cli, NULL, "exit", cmd_config_int_exit,
        PRIVILEGE_PRIVILEGED, MODE_CONFIG_INT,
        "Exit from interface configuration");

    cli_register_command(cli, NULL, "address", cmd_test, PRIVILEGE_PRIVILEGED,
        MODE_CONFIG_INT, "Set IP address");

    c = cli_register_command(cli, NULL, "debug", NULL, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, NULL);

    cli_register_command(cli, c, "regular", cmd_debug_regular, PRIVILEGE_UNPRIVILEGED,
        MODE_EXEC, "Enable cli_regular() callback debugging");
#endif
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
#if 0
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return 1;
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(CLITEST_PORT);
    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    if (listen(s, 50) < 0)
    {
        perror("listen");
        return 1;
    }

    printf("Listening on port %d\n", CLITEST_PORT);
#endif
	
	CLI_TermEchoOff();
    //while ((x = accept(s, NULL, 0)))
    {
#ifndef WIN32
#if 0
        int pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return 1;
        }

        /* parent */
        if (pid > 0)
        {
            socklen_t len = sizeof(addr);
            if (getpeername(x, (struct sockaddr *) &addr, &len) >= 0)
                printf(" * accepted connection from %s\n", inet_ntoa(addr.sin_addr));

            close(x);
            continue;
        }

        /* child */
        close(s);
#endif
        cli_loop_cli(cli, r, w);
        exit(0);
#else
        cli_loop(cli, x);
        shutdown(x, SD_BOTH);
        close(x);
#endif
    }

    cli_done(cli);
    return 0;
}
