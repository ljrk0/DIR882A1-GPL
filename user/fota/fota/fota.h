
/*
*/

#ifndef __FOTA_H__
#define __FOTA_H__

#include <fota_debug.h>
#include <fota_common.h>
#include <fota_api.h>
#include "shutils.h"

#define FIRMWARE_QUERY_URI "/device/v1/me/latest_fw"
#define FIRMWARE_QUERY_URI_2 "/mobile/v1/misc/latest_fw"
#define EVENT_NOTIFICATION_URI "/device/v1/me/event"

#define MD5_CHECKSUM_HEADER "x-amz-meta-md5chksum"

#define EVENT_ID_FW_UPDATE_START "0101001"
#define EVENT_ID_FW_UPDATE_COMPLETE "0101002"

#define FOTA_CONFIG_FILE "/tmp/fota.cfg"
#define  TMP_FW "/tmp/fota_firmware.img"

struct fota_event
{
	char * pEvent_id;
	char * pVariables[5];
};

enum fota_progress_type
{
	/* Update the progress by percentage */
	PROGRESS_TYPE_PERCENT = 1,
	/* Update the progress by time */
	PROGRESS_TYPE_TIME
};

struct fota_progress_measure
{
	enum fota_progress_type iType;
	/* 
	  * Update the progress by per 'iPercent' percentage,
	  * used by 'percentage' method
	*/
	int iPercent;
	/*
	  * Update the progress by per 'iInterval' seconds,
	  * used by 'time' method
	*/
	int iInterval;
	/* Real-time progress output */
	FILE * pProgress;
};

struct fota_progress_config
{
	char * pProgress_file;
	int iProgress_type;
	int iProgress_percent;
	int iProgress_time;
};

struct fota_fw_info
{
	char * pUrl;
	char * pChecksum;
	char * pRelease_note;
	/*
	  * Unix timestamp: the number of seconds since the Epoch,
	  * 1970-01-01 00:00:00 +0000 (UTC).
	*/
	long iSeconds;
	int iExist;
	struct version fw_version;
};

struct fota_latest_fw_info
{
	struct fota_fw_info stable;
	struct fota_fw_info beta;
	struct fota_fw_info intermediate;
	struct version enforced_version;
};

struct fota_upgrade_config
{
	int iEvent_notification_enable;
	int iUpgrade_notification_enable;
	int iAuto_query_enable;
	int iAuto_download_enable;
	int iAuto_update_enable;
	int iBeta_fw_enable;
};

struct fota_config
{
	char * pConfig_file;
	char * pPid_file;
	char * pAction;
	char * pCacert;
	char * pFw_url;
	char * pFw_path;
	int iFw_encrypt_flag;
	int iDaemonize;
	int iEnable_stdio;
	int iHelp;
	int iVerbose;
	struct fota_progress_config progress_config;
	struct fota_upgrade_config upgrade_config;
	struct fota_client_info client_info;
	//struct fota_client_info_2 client_info_2;
};

#define CONSTRUCT_FOTA_UPGRADE_OPTION(config_option, pUpgrade_config) \
struct fota_option config_option[] = \
{ \
	/* configuration of upgrade */ \
\
	{"event-notification-enable", OPTION_TYPE_INT, \
		&pUpgrade_config->iEvent_notification_enable, \
		NULL}, \
	{"auto-update-notification-enable", OPTION_TYPE_INT, \
		&pUpgrade_config->iUpgrade_notification_enable, \
		NULL}, \
	{"auto-query-enable", OPTION_TYPE_INT, \
		&pUpgrade_config->iAuto_query_enable, \
		NULL}, \
	{"auto-download-enable", OPTION_TYPE_INT, \
		&pUpgrade_config->iAuto_download_enable, \
		NULL}, \
	{"auto-update-enable", OPTION_TYPE_INT, \
		&pUpgrade_config->iAuto_update_enable, \
		NULL}, \
	{"beta-firmware-enable", OPTION_TYPE_INT, \
		&pUpgrade_config->iBeta_fw_enable, \
		NULL}, \
	{NULL, 0, NULL, NULL, 0} \
}

#endif

