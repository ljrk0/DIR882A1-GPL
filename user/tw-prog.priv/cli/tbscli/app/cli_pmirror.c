
#include "../cli_pub.h"
#include "../cli/cli_term.h"
#include "../cli/cli_io.h"
#include "../cli/cli_interpret.h"
#include "cli_cmd.inc"

#include "../cli_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLI_GET_ULONG_ASSERT(ulValue, name) do \
{ \
    if (CLI_GetUlongByName(name, &ulValue) != TBS_SUCCESS) \
    { \
        return TBS_FAILED; \
    } \
}while(0);


ULONG CLI_PortMirror()
{
	char  *p_vpi = NULL;
	char  *p_vci = NULL;
	char  *p_mirror_flag = NULL;
	char  cmdline[256] = {"sarctl mirror "};

	if (CLI_GetParamByName("vpi") != NULL)
	{
		p_vpi = CLI_GetParamByName("vpival");
		if(p_vpi == NULL)
		{
			return TBS_FAILED;
		}
		strcat ( cmdline, p_vpi);
		strcat ( cmdline, " " );
	}

	if ( CLI_GetParamByName ( "vci" ) != NULL )
	{
		p_vci = CLI_GetParamByName ( "vcival" );
		if ( p_vci == NULL )
		{
			return TBS_FAILED;
		}
		strcat ( cmdline, p_vci);
		strcat ( cmdline, " " );
	}

	if ( CLI_GetParamByName ("flags") == NULL )
	{
		return TBS_FAILED;
	}

	p_mirror_flag = CLI_GetParamByName ( "flagsval" );
	strcat ( cmdline, p_mirror_flag );
	system(cmdline);
	return TBS_SUCCESS;
}

#ifdef __cplusplus
}
#endif

