


#include <stdio.h>

#include "cli/cli_term.h"

extern void CLI_TermTaskProc();
extern ULONG  CLI_ProcessInit();

int main()
{
    if (RET_FAILED(CLI_ProcessInit()))
    {
        printf("\nCLI Init Failed!");
        getchar();
        return 1;
    }

    CLI_TermTask(0, 0);

    return 0;
}