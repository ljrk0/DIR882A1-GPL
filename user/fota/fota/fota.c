
/*
  * fota concept design
  * 2018-01-25 --liushenghui
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>

#include "fota.h"
#include "fota_platform_dependence.h"

struct fota_config fota_config_cmd;

int fota_init_config(int argc, char * * argv,
	struct fota_config * pConfig);
int fota_free_config(struct fota_config * pConfig);

int send_query_request(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data);
int send_query_request_2(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data);
int get_fota_query_response(
						struct fota_latest_fw_info * pLatest_fw_info,
						struct http_chunk * pHttp_data);

int send_event_request(
						struct fota_client_info * pDevice_info,
						const char * pParameters,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data);
int get_fota_event_parameters(
						struct fota_event * pEvent,
						char * * pParam);

int send_download_firmware_request(
						struct fota_fw_info * pFw,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data);

int send_download_firmware_request_manual(
						struct fota_fw_info * pFw,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data);

/*
  * Query the latest firmware information.
  * Input:
  *   Language: Language preference, e.g., "en_US", Optional.
  *   Access token.
  * Output:
  *   Stable Version: stable firmware information.
  *   Beta Version: beta firmware information, Optional.
  *   Intermediate: current firmware version below this version must
  *      update to this firmware first before updating to any newer firmware
  *      version, Optional.
  *   Enforce: current firmware version below this version should be
  *      enforced to update to a newer firmware version, Optional.
  * 2018-01-26 --liushenghui
*/
static int query_latest_firmware_info(
						struct fota_client_info * pDevice_info,
						struct fota_latest_fw_info * pLatest_fw_info)
{
	char * pMsg = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int iHttp_code = 0;
	int iMsg_code = 0;
	struct http_chunk http_header;
	struct http_chunk http_data;

	DEBUG("Now check with the server for the latest firmware information.\n");

	RETURN_IF_NULL(pDevice_info, -1);

	memset(&http_header, '\0', sizeof (http_header));
	memset(&http_data, '\0', sizeof (http_data));

	iRet = send_query_request(pDevice_info,
		&http_header, &http_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = get_http_response_code(&iHttp_code, &http_header);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	if (200 != iHttp_code)
	{
		iRet = get_http_error_message(&iMsg_code, &pMsg, &http_data);
		if (iRet < 0 || !pMsg)
		{
			goto done;
		}

		DEBUG("Http code: %d, Error code: %d, error message: %s.\n",
			iHttp_code, iMsg_code, pMsg);

		iResult_code = action_of_error_code(iMsg_code);

		goto done;
	}

	iRet = get_fota_query_response(pLatest_fw_info, &http_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	DEBUG("QUERY the latest firmware information success.\n");

	iResult_code = 0;

done:

	FREE_POINTER(pMsg);
	if (CHUNK_TYPE_MEM == http_header.iType)
	{
		FREE_POINTER(http_header.pData);
	}
	if (CHUNK_TYPE_MEM == http_data.iType)
	{
		FREE_POINTER(http_data.pData);
	}

	return iResult_code;
}

/*
  * Query the latest firmware information for those non-encrypted firmware.
  * Input:
  *   Series: series name, e.g., "COVR"
  *   Model: model name, e.g., "COVR-3902"
  *   Hardware version: hardware version, e.g., "A1"
  *   Variant: Firmware variant, which identifies a special firmware build for
  *      a certain project or region-specific use, Optional.
  *   Language: Language preference, e.g., "en_US", Optional.
  * Output:
  *   Stable Version: stable firmware information.
  *   Beta Version: beta firmware information, Optional.
  *   Intermediate: current firmware version below this version must
  *      update to this firmware first before updating to any newer firmware
  *      version, Optional.
  *   Enforce: current firmware version below this version should be
  *      enforced to update to a newer firmware version, Optional.
  * 2018-01-26 --liushenghui
*/
static int query_latest_firmware_info_2(
						struct fota_client_info * pDevice_info,
						struct fota_latest_fw_info * pLatest_fw_info)
{
	char * pMsg = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int iHttp_code = 0;
	int iMsg_code = 0;
	struct http_chunk http_header;
	struct http_chunk http_data;

	DEBUG("Now check with the server for the latest firmware information.\n");

	RETURN_IF_NULL(pDevice_info, -1);

	memset(&http_header, '\0', sizeof (http_header));
	memset(&http_data, '\0', sizeof (http_data));

	iRet = send_query_request_2(pDevice_info,
		&http_header, &http_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = get_http_response_code(&iHttp_code, &http_header);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	if (200 != iHttp_code)
	{
		iRet = get_http_error_message(&iMsg_code, &pMsg, &http_data);
		if (iRet < 0 || !pMsg)
		{
			goto done;
		}

		DEBUG("Http code: %d, Error code: %d, error message: %s.\n",
			iHttp_code, iMsg_code, pMsg);

		goto done;
	}

	iRet = get_fota_query_response(pLatest_fw_info, &http_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	DEBUG("QUERY the latest firmware information success.\n");

	iResult_code = 0;

done:

	FREE_POINTER(pMsg);
	if (CHUNK_TYPE_MEM == http_header.iType)
	{
		FREE_POINTER(http_header.pData);
	}
	if (CHUNK_TYPE_MEM == http_data.iType)
	{
		FREE_POINTER(http_data.pData);
	}

	return iResult_code;
}

/*
  * Update device information.
  * Input:
  *   Event ID.
  *   Variables: the event messages may contain variables, for example:
  *      variable[0] = "My Device's Name", Optional.
  *   Access token.
  * Output:
  *   None.
  * 2018-01-29 --liushenghui
*/
static int send_event_notification_to_server(
							struct fota_client_info * pDevice_info,
							struct fota_event * pEvent)
{
	char * pParameters = NULL;
	char * pMsg = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int iHttp_code = 0;
	int iMsg_code = 0;
	struct http_chunk http_header;
	struct http_chunk http_data;

	DEBUG("Now send EVENT notification to the server.\n");

	RETURN_IF_NULL(pDevice_info, -1);

	memset(&http_header, '\0', sizeof (http_header));
	memset(&http_data, '\0', sizeof (http_data));

	iRet = get_fota_event_parameters(pEvent, &pParameters);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = send_event_request(pDevice_info, pParameters,
		&http_header, &http_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = get_http_response_code(&iHttp_code, &http_header);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	if (200 != iHttp_code)
	{
		iRet = get_http_error_message(&iMsg_code, &pMsg, &http_data);
		if (iRet < 0 || !pMsg)
		{
			goto done;
		}

		DEBUG("Http code: %d, Error code: %d, error message: %s.\n",
			iHttp_code, iMsg_code, pMsg);

		iResult_code = action_of_error_code(iMsg_code);

		goto done;
	}

	DEBUG("Send EVENT notification to the server success.\n");

	iRet = setenv("FOTA_UPDATE_DONE_NOTIFY", "0", 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	FREE_POINTER(pParameters);
	FREE_POINTER(pMsg);
	if (CHUNK_TYPE_MEM == http_header.iType)
	{
		FREE_POINTER(http_header.pData);
	}
	if (CHUNK_TYPE_MEM == http_data.iType)
	{
		FREE_POINTER(http_data.pData);
	}

	return iResult_code;
}

static int compare_fw_version(struct version * pVersion1,
						struct version * pVersion2)
{
	RETURN_IF_NULL(pVersion1, -2);
	RETURN_IF_NULL(pVersion2, -2);

	if (atoi(pVersion1->sMajor) == atoi(pVersion2->sMajor))
	{
		if (atoi(pVersion1->sMinor) == atoi(pVersion2->sMinor))
		{
			if (!strcmp(pVersion1->sRevision, "")
				|| !strcmp(pVersion2->sRevision, ""))
			{
				return 0;
			}

			if (atoi(pVersion1->sRevision) == atoi(pVersion2->sRevision))
			{
				return 0;
			}
			else
			{
				if ((atoi(pVersion1->sRevision) > atoi(pVersion2->sRevision)))
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
		}
		else
		{
			if ((atoi(pVersion1->sMinor) > atoi(pVersion2->sMinor)))
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
	}
	else
	{
		if ((atoi(pVersion1->sMajor) > atoi(pVersion2->sMajor)))
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}

	return -2;
}

static int get_firmware_md5_checksum(const char * pHeader,
								char * * pChecksum)
{
	char sChecksum[128] = { 0 };
	char * pChecksum_header = NULL;
	int iResult_code = -1;
	int iItem_num = 0;

	RETURN_IF_NULL(pHeader, -1);
	RETURN_IF_NULL(pChecksum, -1);

	pChecksum_header = strstr(pHeader, MD5_CHECKSUM_HEADER":");
	if (!pChecksum_header)
	{
		DEBUG("Checksum header(%s) of firmware is not found.\n",
			MD5_CHECKSUM_HEADER);
		return -1;
	}

	iItem_num = sscanf(pChecksum_header,
				MD5_CHECKSUM_HEADER": %128[^\r\n] ",
				sChecksum);
	if (iItem_num != 1)
	{
		DEBUG("Get firmware Checksum fail, %d.\n", iItem_num);
		if (iItem_num < 0)
		{
			DEBUG("sscanf fail, %s, errno=%d.\n",
				strerror(errno), errno);
		}
		goto done;
	}
	else
	{
		*pChecksum = strdup(sChecksum);
		if (!*pChecksum)
		{
			DEBUG("strdup fail, %s, errno=%d.\n",
				strerror(errno), errno);
			goto done;
		}
	}

	iResult_code = 0;

done:

	return iResult_code;
}

static int check_enforced_version(
					struct version * pVersion_info,
					struct fota_latest_fw_info * pLatest_fw_info)
{
	const char * pEnforced_exist = NULL;

	DEBUG("Now check enforced version.\n");

	RETURN_IF_NULL(pVersion_info, -1);
	RETURN_IF_NULL(pLatest_fw_info, -1);

	if (0 != strcmp(pLatest_fw_info->enforced_version.sMajor, ""))
	{
		if (-1 == compare_fw_version(pVersion_info,
			&pLatest_fw_info->enforced_version))
		{
			pEnforced_exist = "1";
		}
	}

	RETURN_IF_NULL(pEnforced_exist, 0);

	if (setenv("FOTA_ENFORCED_VERSION_EXIST",
		pEnforced_exist, 1) < 0)
	{
		DEBUG("setenv fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	return 0;
}

static int select_firmware_version(struct version * pVersion_info,
					struct fota_latest_fw_info * pLatest_fw_info,
					struct fota_upgrade_config * pUpgrade_config,
					struct fota_fw_info * * pFw)
{
	DEBUG("Now CHOOSE a viable firmware version.\n");

	RETURN_IF_NULL(pVersion_info, -1);
	RETURN_IF_NULL(pLatest_fw_info, -1);
	RETURN_IF_NULL(pUpgrade_config, -1);
	RETURN_IF_NULL(pFw, -1);

	check_enforced_version(pVersion_info, pLatest_fw_info);

	if (1 == pLatest_fw_info->intermediate.iExist)
	{
		if (-1 == compare_fw_version(pVersion_info,
				&pLatest_fw_info->intermediate.fw_version)
		)
		{
			DEBUG("Need upgrade intermediate firmware first.\n");
			*pFw = &pLatest_fw_info->intermediate;
			goto done;
		}
	}

	if (1 == pUpgrade_config->iBeta_fw_enable)
	{
		if (1 == pLatest_fw_info->beta.iExist)
		{
			if (1 == pLatest_fw_info->stable.iExist)
			{
				if (-1 == compare_fw_version(
					&pLatest_fw_info->beta.fw_version,
					&pLatest_fw_info->stable.fw_version)
				)
				{
					*pFw = &pLatest_fw_info->stable;
					goto done;
				}
			}

			DEBUG("Select upgrade beta firmware.\n");

			*pFw = &pLatest_fw_info->beta;
			goto done;
		}
	}

	if (1 == pLatest_fw_info->stable.iExist)
	{
		DEBUG("Select upgrade stable firmware.\n");

		*pFw = &pLatest_fw_info->stable;
		goto done;
	}

	DEBUG("No newer firmware is available.\n");
	return 0;

done:

	if (1 != compare_fw_version(&(*pFw)->fw_version,
		pVersion_info))
	{
		DEBUG("No newer firmware is available.\n");
		*pFw = NULL;
		return 0;
	}

	if (!(*pFw)->pUrl)
	{
		DEBUG("Fatal error, no url exists.\n");
		*pFw = NULL;
		return -1;
	}

	DEBUG("Local firmware version: major=%s, minor=%s, revision=%s.\n",
		pVersion_info->sMajor,
		pVersion_info->sMinor,
		pVersion_info->sRevision
		);

	DEBUG("The firmware version to be upgraded:"
		" major=%s, minor=%s, revision=%s, url=%s, time=%ld.\n",
		(*pFw)->fw_version.sMajor,
		(*pFw)->fw_version.sMinor,
		(*pFw)->fw_version.sRevision,
		(*pFw)->pUrl,
		(*pFw)->iSeconds
		);

	return 1;
}

static int download_firmware(struct fota_fw_info * pFw)
{
	int iRet = -1;
	int iResult_code = -1;
	int iHttp_code = 0;
	struct http_chunk http_header;
	struct http_chunk http_data;

	RETURN_IF_NULL(pFw, -1);

	memset(&http_header, '\0', sizeof (http_header));
	memset(&http_data, '\0', sizeof (http_data));

	FREE_POINTER(pFw->pChecksum);

	DEBUG("Start DOWNLOADing firmware now.\n");

	iRet = send_download_firmware_request(pFw,
		&http_header, &http_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = get_http_response_code(&iHttp_code, &http_header);
	if (iRet < 0 || iHttp_code != 200)
	{
		DEBUG("get_http_response_code fail!\n");
		goto done;
	}

	iRet = get_firmware_md5_checksum(http_header.pData,
		&pFw->pChecksum);
	if (iRet < 0 || !pFw->pChecksum)
	{
		DEBUG("Get firmware Checksum fail.\n");
		goto done;
	}

	GOTO_IF_NOT_EQUAL(http_data.iStatus_code, 0, done);

	DEBUG("Firmware DOWNLOAD completed.\n");

	iResult_code = 0;

done:

	if (CHUNK_TYPE_MEM == http_header.iType)
	{
		FREE_POINTER(http_header.pData);
	}
	if (CHUNK_TYPE_MEM == http_data.iType)
	{
		FREE_POINTER(http_data.pData);
	}

	return iResult_code;
}

static int download_firmware_manual(struct fota_fw_info * pFw)
{
	int iRet = -1;
	int iResult_code = -1;
	int iHttp_code = 0;
	struct http_chunk http_header;
	struct http_chunk http_data;

	RETURN_IF_NULL(pFw, -1);

	memset(&http_header, '\0', sizeof (http_header));
	memset(&http_data, '\0', sizeof (http_data));

	FREE_POINTER(pFw->pChecksum);

	DEBUG("Start DOWNLOADing firmware now.\n");

	iRet = send_download_firmware_request_manual(pFw,
		&http_header, &http_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = get_http_response_code(&iHttp_code, &http_header);
	if (iRet < 0 || iHttp_code != 200)
	{
		DEBUG("get_http_response_code fail!\n");
		goto done;
	}

	iRet = get_firmware_md5_checksum(http_header.pData,
		&pFw->pChecksum);
	if (iRet < 0 || !pFw->pChecksum)
	{
		DEBUG("Get firmware Checksum fail.\n");
		goto done;
	}

	GOTO_IF_NOT_EQUAL(http_data.iStatus_code, 0, done);

	DEBUG("Firmware DOWNLOAD completed.\n");

	iResult_code = 0;

done:

	if (CHUNK_TYPE_MEM == http_header.iType)
	{
		FREE_POINTER(http_header.pData);
	}
	if (CHUNK_TYPE_MEM == http_data.iType)
	{
		FREE_POINTER(http_data.pData);
	}

	return iResult_code;
}

static int free_latest_fw_info(struct fota_latest_fw_info * pFw_info)
{
	RETURN_IF_NULL(pFw_info, -1);

	FREE_POINTER(pFw_info->stable.pChecksum);
	FREE_POINTER(pFw_info->stable.pRelease_note);
	FREE_POINTER(pFw_info->stable.pUrl);

	FREE_POINTER(pFw_info->beta.pChecksum);
	FREE_POINTER(pFw_info->beta.pRelease_note);
	FREE_POINTER(pFw_info->beta.pUrl);

	FREE_POINTER(pFw_info->intermediate.pChecksum);
	FREE_POINTER(pFw_info->intermediate.pRelease_note);
	FREE_POINTER(pFw_info->intermediate.pUrl);

	return 0;
}

static int free_fw_info(struct fota_fw_info * pFw_info)
{
	RETURN_IF_NULL(pFw_info, -1);

	FREE_POINTER(pFw_info->pChecksum);
	FREE_POINTER(pFw_info->pRelease_note);
	FREE_POINTER(pFw_info->pUrl);

	return 0;
}

static int add_event_message(struct fota_event * pEvent,
						const char * pEvent_id,
						const char * pMsg)
{
	int iIndex = 0;

	RETURN_IF_NULL(pEvent, -1);

	if (pEvent_id)
	{
		FREE_POINTER(pEvent->pEvent_id);

		pEvent->pEvent_id = strdup(pEvent_id);
		if (!pEvent->pEvent_id)
		{
			DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
			return -1;
		}
	}

	if (pMsg)
	{
		for (iIndex = 0 ;
			iIndex < sizeof (pEvent->pVariables) / sizeof (char *) ;
			++iIndex
		)
		{
			if (!pEvent->pVariables[iIndex])
			{
				pEvent->pVariables[iIndex] = strdup(pMsg);
				if (!pEvent->pEvent_id)
				{
					DEBUG("strdup fail, %s, errno=%d.\n",
						strerror(errno), errno);
					return -1;
				}

				break;
			}
		}
	}

	return 0;
}

static int free_event_message(struct fota_event * pEvent)
{
	int iIndex = 0;

	RETURN_IF_NULL(pEvent, -1);

	FREE_POINTER(pEvent->pEvent_id);

	for (iIndex = 0 ;
		iIndex < sizeof (pEvent->pVariables) / sizeof (char *) ;
		++iIndex
	)
	{
		FREE_POINTER(pEvent->pVariables[iIndex]);
	}

	return 0;
}

static int seconds_to_date(time_t iSeconds, char * pDate, size_t iLen)
{
	struct tm fw_time;

	RETURN_IF_NULL(pDate, -1);

	if (NULL == gmtime_r(&iSeconds, &fw_time))
	{
		DEBUG("gmtime_r fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	snprintf(pDate, iLen, "%4d-%02d-%02d",
		fw_time.tm_year + 1900,
		fw_time.tm_mon + 1,
		fw_time.tm_mday);

	return 0;
}

static int fota_query_action(struct fota_client_info * pDevice_info,
					struct fota_latest_fw_info * pFw_info)
{
	int iRet = 0;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pFw_info, -1);

	memset(pFw_info, '\0', sizeof (struct fota_latest_fw_info));

query:

	iRet = query_latest_firmware_info(pDevice_info, pFw_info);
	if (iRet > 0)
	{
		iRet = fota_handle_token_expire(iRet, pDevice_info);
		if (0 == iRet)
		{
			free_latest_fw_info(pFw_info);
			goto query;
		}
		else
		{
			return -1;
		}
	}
	else if (iRet < 0)
	{
		DEBUG("query_latest_firmware_info fail.\n");
		return -1;
	}

	return 0;
}

static int fota_notify_action(struct fota_client_info * pDevice_info,
					struct fota_event * pEvent)
{
	int iRet = 0;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pEvent, -1);

notify:

	iRet = send_event_notification_to_server(pDevice_info, pEvent);
	if (iRet > 0)
	{
		iRet = fota_handle_token_expire(iRet, pDevice_info);
		if (0 == iRet)
		{
			goto notify;
		}
		else
		{
			return -1;
		}
	}
	else if (iRet < 0)
	{
		DEBUG("send_event_notification_to_server fail.\n");
		return -1;
	}

	return 0;
}

static int automatic_firmware_update(
					struct fota_upgrade_config * pUpgrade_config,
					struct fota_client_info * pDevice_info)
{
	char sEvent_msg[128] = { 0 };
	int iRet = 0;
	int iResult_code = -1;
	struct fota_latest_fw_info fw_info;
	struct fota_event event;
	struct fota_fw_info * pFw = NULL;

	RETURN_IF_NULL(pUpgrade_config, -1);
	RETURN_IF_NULL(pDevice_info, -1);

	memset(&fw_info, '\0', sizeof (fw_info));
	memset(&event, '\0', sizeof (event));

	if (1 != pUpgrade_config->iAuto_query_enable)
	{
		DEBUG("Automatic firmware query is not enable.\n");
		return 0;
	}

	iRet = get_registration_status(pDevice_info);
	if (REGISTERED != iRet)
	{
		iRet = register_to_fota_server(pDevice_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, exit);

		iRet = upload_device_info(pDevice_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, exit);
	}

	iRet = fota_query_action(pDevice_info, &fw_info);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	if (1 != pUpgrade_config->iAuto_download_enable)
	{
		DEBUG("Automatic firmware download is not enable.\n");
		iResult_code = 0;
		goto exit;
	}

	if (1 == pUpgrade_config->iEvent_notification_enable
		&& 1 == pUpgrade_config->iUpgrade_notification_enable)
	{
		snprintf(sEvent_msg, sizeof (sEvent_msg),
			"%s",
			pDevice_info->sModel_name);
		iRet = add_event_message(&event,
						EVENT_ID_FW_UPDATE_START,
						sEvent_msg);
		GOTO_IF_NOT_EQUAL(iRet, 0, exit);

		iRet = fota_notify_action(pDevice_info, &event);
		GOTO_IF_NOT_EQUAL(iRet, 0, exit);
	}

	iRet = select_firmware_version(&pDevice_info->fw_version,
					&fw_info, pUpgrade_config, &pFw);
	if (iRet < 0)
	{
		DEBUG("select_firmware_version fail.\n");
		goto exit;
	}
	else if (iRet != 1)
	{
		goto exit;
	}

	//DEBUG("Wait for 5 minutes to download firmware.\n");
	//execute_pause(5 * 60);

	iRet = download_firmware(pFw);
	if (0 != iRet)
	{
		DEBUG("download_firmware fail.\n");
		clear_firmware(fota_config_cmd.pFw_path);
		goto exit;
	}

	iRet = verify_firmware(fota_config_cmd.pFw_path, pFw);
	if (0 != iRet)
	{
		DEBUG("verify_firmware fail.\n");
		clear_firmware(fota_config_cmd.pFw_path);
		goto exit;
	}

	if (1 != pUpgrade_config->iAuto_update_enable)
	{
		DEBUG("Automatic firmware update is not enable.\n");
		iResult_code = 0;
		goto exit;
	}

	change_led_color();

	DEBUG("Wait for 10 seconds to save firmware to flash.\n");
	execute_pause(10);

	iRet = save_firmware_to_flash(fota_config_cmd.pFw_path);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = setenv("FOTA_UPDATE_DONE_NOTIFY", "1", 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iResult_code = 0;

exit:

	free_latest_fw_info(&fw_info);
	free_event_message(&event);

	return iResult_code;
}

/*
*/
static int automatic_firmware_update_2(
					struct fota_upgrade_config * pUpgrade_config,
					struct fota_client_info * pDevice_info)
{
	int iRet = 0;
	int iResult_code = -1;
	//struct fota_client_info_2 device_info;
	struct fota_latest_fw_info fw_info;
	struct fota_fw_info * pFw = NULL;

	RETURN_IF_NULL(pUpgrade_config, -1);
	RETURN_IF_NULL(pDevice_info, -1);

	memset(&fw_info, '\0', sizeof (fw_info));

	if (1 != pUpgrade_config->iAuto_query_enable)
	{
		DEBUG("Automatic firmware query is not enable.\n");
		return 0;
	}

	iRet = query_latest_firmware_info_2(pDevice_info, &fw_info);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	if (1 != pUpgrade_config->iAuto_download_enable)
	{
		DEBUG("Automatic firmware download is not enable.\n");
		iResult_code = 0;
		goto exit;
	}

	iRet = select_firmware_version(&pDevice_info->fw_version,
					&fw_info, pUpgrade_config, &pFw);
	if (iRet < 0)
	{
		DEBUG("select_firmware_version fail.\n");
		goto exit;
	}
	else if (iRet != 1)
	{
		goto exit;
	}

	//DEBUG("Wait for 5 minutes to download firmware.\n");
	//execute_pause(5 * 60);

	iRet = download_firmware(pFw);
	if (0 != iRet)
	{
		DEBUG("download_firmware fail.\n");
		clear_firmware(fota_config_cmd.pFw_path);
		goto exit;
	}

	iRet = verify_firmware(fota_config_cmd.pFw_path, pFw);
	if (0 != iRet)
	{
		DEBUG("verify_firmware fail.\n");
		clear_firmware(fota_config_cmd.pFw_path);
		goto exit;
	}

	if (1 != pUpgrade_config->iAuto_update_enable)
	{
		DEBUG("Automatic firmware update is not enable.\n");
		iResult_code = 0;
		goto exit;
	}

	change_led_color();

	DEBUG("Wait for 10 seconds to save firmware to flash.\n");
	execute_pause(10);

	iRet = save_firmware_to_flash(fota_config_cmd.pFw_path);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iResult_code = 0;

exit:

	free_latest_fw_info(&fw_info);

	return iResult_code;
}

static int fota_send_update_done_notification(
					struct fota_upgrade_config * pUpgrade_config,
					struct fota_client_info * pDevice_info)
{
	char sEvent_msg[128] = { 0 };
	int iRet = 0;
	int iResult_code = -1;
	struct fota_event event;

	RETURN_IF_NULL(pUpgrade_config, -1);
	RETURN_IF_NULL(pDevice_info, -1);

	memset(&event, '\0', sizeof (event));

	if (1 != pUpgrade_config->iAuto_query_enable)
	{
		DEBUG("Automatic firmware query is not enable.\n");
		return 0;
	}

	iRet = get_registration_status(pDevice_info);
	if (REGISTERED != iRet)
	{
		iRet = register_to_fota_server(pDevice_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, exit);
	}

	iRet = upload_device_info(pDevice_info);
	if (iRet > 0)
	{
		iRet = fota_handle_token_expire(iRet, pDevice_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, exit);
	}
	else if (iRet < 0)
	{
		DEBUG("upload_device_info fail.\n");
		goto exit;
	}

	if (1 == pUpgrade_config->iEvent_notification_enable
		&& 1 == pUpgrade_config->iUpgrade_notification_enable)
	{
		snprintf(sEvent_msg, sizeof (sEvent_msg),
			"%s",
			pDevice_info->sModel_name);
		iRet = add_event_message(&event,
						EVENT_ID_FW_UPDATE_COMPLETE,
						sEvent_msg);
		GOTO_IF_NOT_EQUAL(iRet, 0, exit);

		iRet = fota_notify_action(pDevice_info, &event);
		GOTO_IF_NOT_EQUAL(iRet, 0, exit);
	}

	iResult_code = 0;

exit:

	free_event_message(&event);

	return iResult_code;
}

static int manual_query_firmware_info(
					struct fota_upgrade_config * pUpgrade_config,
					struct fota_client_info * pDevice_info)
{
	char sDate[64] = { 0 };
	int iRet = 0;
	int iResult_code = -1;
	struct fota_latest_fw_info fw_info;
	struct fota_fw_info * pFw = NULL;

	RETURN_IF_NULL(pUpgrade_config, -1);
	RETURN_IF_NULL(pDevice_info, -1);

	memset(&fw_info, '\0', sizeof (fw_info));

	iRet = get_registration_status(pDevice_info);
	if (REGISTERED != iRet)
	{
		iRet = register_to_fota_server(pDevice_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, exit);

		iRet = upload_device_info(pDevice_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, exit);
	}

	iRet = fota_query_action(pDevice_info, &fw_info);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = select_firmware_version(&pDevice_info->fw_version,
					&fw_info, pUpgrade_config, &pFw);
	if (iRet < 0)
	{
		DEBUG("select_firmware_version fail.\n");
		goto exit;
	}
	else if (iRet != 1)
	{
		goto exit;
	}

	iRet = setenv("FOTA_MANUAL_FW_VERSION_MAJOR",
		pFw->fw_version.sMajor, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = setenv("FOTA_MANUAL_FW_VERSION_MINOR",
		pFw->fw_version.sMinor, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = setenv("FOTA_MANUAL_FW_VERSION_REVISION",
		pFw->fw_version.sRevision, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = setenv("FOTA_MANUAL_FW_URL", pFw->pUrl, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = seconds_to_date(pFw->iSeconds, sDate, sizeof (sDate));
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = setenv("FOTA_MANUAL_FW_DATE", sDate, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iResult_code = 0;

exit:

	free_latest_fw_info(&fw_info);

	return iResult_code;
}

/*
*/
static int manual_query_firmware_info_2(
					struct fota_upgrade_config * pUpgrade_config,
					struct fota_client_info * pDevice_info)
{
	char sDate[64] = { 0 };
	int iRet = 0;
	int iResult_code = -1;
	struct fota_latest_fw_info fw_info;
	struct fota_fw_info * pFw = NULL;

	RETURN_IF_NULL(pUpgrade_config, -1);
	RETURN_IF_NULL(pDevice_info, -1);

	memset(&fw_info, '\0', sizeof (fw_info));

	iRet = query_latest_firmware_info_2(pDevice_info, &fw_info);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = select_firmware_version(&pDevice_info->fw_version,
					&fw_info, pUpgrade_config, &pFw);
	if (iRet < 0)
	{
		DEBUG("select_firmware_version fail.\n");
		goto exit;
	}
	else if (iRet != 1)
	{
		goto exit;
	}

	iRet = setenv("FOTA_MANUAL_FW_VERSION_MAJOR",
		pFw->fw_version.sMajor, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = setenv("FOTA_MANUAL_FW_VERSION_MINOR",
		pFw->fw_version.sMinor, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = setenv("FOTA_MANUAL_FW_VERSION_REVISION",
		pFw->fw_version.sRevision, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = setenv("FOTA_MANUAL_FW_URL", pFw->pUrl, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = seconds_to_date(pFw->iSeconds, sDate, sizeof (sDate));
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iRet = setenv("FOTA_MANUAL_FW_DATE", sDate, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, exit);

	iResult_code = 0;

exit:

	free_latest_fw_info(&fw_info);

	return iResult_code;
}

static int manual_download_firmware(void)
{
	int iRet = 0;
	int iResult_code = -1;
	struct fota_fw_info fw_info;

	RETURN_IF_NULL(fota_config_cmd.pFw_url, -1);
	RETURN_IF_NULL(fota_config_cmd.pFw_path, -1);

	memset(&fw_info, '\0', sizeof (fw_info));
	fw_info.pUrl = strdup(fota_config_cmd.pFw_url);
	if (!fw_info.pUrl)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto exit;
	}

	iRet = download_firmware_manual(&fw_info);
	if (0 != iRet)
	{
		DEBUG("download_firmware fail.\n");
		clear_firmware(fota_config_cmd.pFw_path);
		goto exit;
	}

	iRet = md5_check(fota_config_cmd.pFw_path, fw_info.pChecksum);
	if (iRet < 0)
	{
		DEBUG("Check md5 checksum of firmware fail.\n");
		unlink(fota_config_cmd.pFw_path);
		goto exit;
	}

	iResult_code = 0;

exit:

	free_fw_info(&fw_info);

	if (0 != iResult_code)
	{
		fota_output_to_file(
			fota_config_cmd.progress_config.pProgress_file,
			"-1", strlen("-1"));
	}

	return iResult_code;
}

static int fota_output_action(const char * pAction,
							int iFw_encrypt_flag)
{
	char * argv[] =
		{
			FOTA_OUTPUT_TOOL_PATH,
			NULL,
			NULL
		};
	int iResult_code = -1;

	RETURN_IF_NULL(pAction, -1);

	if (1 == iFw_encrypt_flag)
	{
		if (0 != strcmp(pAction, "auto")
			&& 0 != strcmp(pAction, "notify")
			&& 0 != strcmp(pAction, "query")
		)
		{
			DEBUG("No information need to be output for action %s.\n",
				pAction);
			return 0;
		}

		argv[1] = "fw-encrypt";
	}
	else
	{
		if (0 != strcmp(pAction, "query")
		)
		{
			DEBUG("No information need to be output for action %s.\n",
				pAction);
			return 0;
		}

		argv[1] = "fw-unencrypt";
	}

	if (execute_action(FOTA_OUTPUT_TOOL_PATH, argv,
		&iResult_code) < 0)
	{
		DEBUG("execute %s fail!\n", FOTA_OUTPUT_TOOL_PATH);
		return -1;
	}

	if (0 != iResult_code)
	{
		DEBUG("Child fail! Exit code %d.\n", iResult_code);
		return -1;
	}

	return 0;
}

int main(int argc, char * * argv)
{
	int iRet = 0;
	int iResult_code = -1;

	openlog("FOTA", LOG_PID, LOG_DAEMON);

	if (detect_process_num("fota") > 1
		|| detect_process_num("mtd_write") > 0
		|| detect_process_num("/bin/imgdecrypt") > 0
	)
	{
		DEBUG("The firmware update process is already in progress!\n");
		goto done;
	}

	iRet = fota_init_config(argc, argv, &fota_config_cmd);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	//iRet = check_network_status(
	//	fota_config_cmd.client_info.sServer_domain,
	//	AF_UNSPEC);
	//GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = setenv("FOTA_ACTION", fota_config_cmd.pAction, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	DEBUG("Execute ACTION: %s.\n", fota_config_cmd.pAction);

	if (!strcmp(fota_config_cmd.pAction, "auto"))
	{
		if (1 == fota_config_cmd.iFw_encrypt_flag)
		{
			iRet = automatic_firmware_update(
				&fota_config_cmd.upgrade_config,
				&fota_config_cmd.client_info);
			GOTO_IF_NOT_EQUAL(iRet, 0, done);
		}
		else
		{
			iRet = automatic_firmware_update_2(
				&fota_config_cmd.upgrade_config,
				&fota_config_cmd.client_info);
			GOTO_IF_NOT_EQUAL(iRet, 0, done);
		}
	}
	else if (!strcmp(fota_config_cmd.pAction, "notify"))
	{
		if (1 == fota_config_cmd.iFw_encrypt_flag)
		{
			iRet = fota_send_update_done_notification(
				&fota_config_cmd.upgrade_config,
				&fota_config_cmd.client_info);
			GOTO_IF_NOT_EQUAL(iRet, 0, done);
		}
	}
	else if (!strcmp(fota_config_cmd.pAction, "query"))
	{
		if (1 == fota_config_cmd.iFw_encrypt_flag)
		{
			iRet = manual_query_firmware_info(
				&fota_config_cmd.upgrade_config,
				&fota_config_cmd.client_info);
			GOTO_IF_NOT_EQUAL(iRet, 0, done);
		}
		else
		{
			iRet = manual_query_firmware_info_2(
				&fota_config_cmd.upgrade_config,
				&fota_config_cmd.client_info);
			GOTO_IF_NOT_EQUAL(iRet, 0, done);
		}
	}
	else if (!strcmp(fota_config_cmd.pAction, "download"))
	{
		iRet = manual_download_firmware();
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}
	else
	{
		DEBUG("Unknown action.\n");
		goto done;
	}

	iRet = setenv("FOTA_ACTION_RESULT", "OK", 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = fota_output_action(fota_config_cmd.pAction,
		fota_config_cmd.iFw_encrypt_flag);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	DEBUG("Done.\n");

	iResult_code = 0;

done:

	fota_free_config(&fota_config_cmd);
	closelog();

	raise(SIGTERM);

	return iResult_code;
}

