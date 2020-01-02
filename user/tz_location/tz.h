
/*
*/

#ifndef __TZ_H__
#define __TZ_H__

#include <fota_debug.h>
#include <fota_common.h>
#include <fota_api.h>

#define TZ_VERSION "0.2.0"

#define TZ_INFO_URI "/device/v1/misc/tzinfo"
#define UTC_INFO_URI "/device/v1/misc/utcinfo"

#define TZ_CONFIG_FILE "/tmp/tz.cfg"

struct tz_config
{
	char * pConfig_file;
	char * pPid_file;
	char * pAction;
	char * pCacert;
	int iDaemonize;
	int iEnable_stdio;
	int iHelp;
	int iVerbose;
	struct fota_client_info client_info;
};

#endif

