/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 �ļ����� : cli_main.c
 �ļ����� :


 �����б� :


 �޶���¼ :
          1 ���� : liuzequn
            ���� : 2007-10-30
            ���� :

**********************************************************************/


/******************************************************************************
 *                                GLOBAL                                      *
 ******************************************************************************/

#include <stdio.h>
#include "cli/cli_term.h"
#include "../include/tbsutil.h"

/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/

extern void CLI_TermTaskProc();
extern ULONG  CLI_ProcessInit();
extern void CLI_ProcessDestory();

int main()
{
    if (CLI_FAILED(CLI_ProcessInit()))
    {
        printf("\n CLI Init Failed!!!");
        CLI_ProcessDestory();
        return 1;
    }
	#if 0
 	{
		pid_t pid;
		char szCmd[64];
		pid = getpid();
		sprintf(szCmd, "echo %d > /var/cmd_pid", pid);
		tbsSystem(szCmd,0);
    }
	#endif
    CLI_TermTask(0, 0);

    printf("\nCmd Exit!\n");
    CLI_ProcessDestory();
    return 0;
}



/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/



