
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <sys/socket.h>

#include "tz.h"

#define NTP_SYNC_TIME_EXEC_PATH "/tmp/ntp_sync_event"
#define NTP_CLIENT_PATH "/bin/ntpclient"
#define NTP_SYNC_RETRY_TIMES "5"
#define NTP_SYNC_RETRY_INTERVAL "1"
#define UTC_INFO_RETRY_INTERVAL 60
#define TZ_INFO_RETRY_INTERVAL 60
#define PERIODIC_TIME_SYNC_INTERVAL (3600 * 24 * 7)
#define MAX_NTP_RETRY_INTERVAL 600
#define MAX_FOTA_RETRY_INTERVAL 300

struct tz_config tz_config_cmd;

int tz_init_config(int argc, char * * argv,
	struct tz_config * pConfig);
int tz_free_config(struct tz_config * pConfig);

int send_utcinfo_request(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data);
int get_utcinfo_response(
						struct http_chunk * pHttp_data,
						char * * pUtc_time);

int send_tzinfo_request(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data);
int get_tzinfo_response(
						struct http_chunk * pHttp_data,
						char * * pTZ);

static int start_scheduler(void)
{
	system(NTP_SYNC_TIME_EXEC_PATH);

	return 0;
}

/*
  * 1. Execute time synchronization.
  * 2. If fail, retry for 5 times with 1 second intervals.
*/
static int execute_ntp_request(char * pNtp_server)
{
	char * const argv[] =
		{
			NTP_CLIENT_PATH,
			"-h",
			pNtp_server,
			"-i",
			NTP_SYNC_RETRY_INTERVAL,
			"-c",
			NTP_SYNC_RETRY_TIMES,
			"-s",
			"-q",
			"-d",
			NULL
		};
	int iResult_code = -1;

	RETURN_IF_NULL(pNtp_server, -1);

	if (execute_action(NTP_CLIENT_PATH, argv,
		&iResult_code) < 0)
	{
		DEBUG("execute %s fail!\n", NTP_CLIENT_PATH);
		return -1;
	}

	if (0 != iResult_code)
	{
		DEBUG("Child fail! Exit code %d.\n", iResult_code);
		return -1;
	}

	return 0;
}

static int execute_request_utcinfo(
					struct fota_client_info * pDevice_info,
					char * * pUtc_time)
{
	char * pMsg = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int iHttp_code = 0;
	int iMsg_code = 0;
	struct http_chunk http_header;
	struct http_chunk http_data;

	DEBUG("Now request UTCinfo from fota server.\n");

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pUtc_time, -1);

	memset(&http_header, '\0', sizeof (http_header));
	memset(&http_data, '\0', sizeof (http_data));

	iRet = send_utcinfo_request(pDevice_info,
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

		DEBUG("Error code: %d, error message: %s.\n",
			iMsg_code, pMsg);

		goto done;
	}

	iRet = get_utcinfo_response(&http_data, pUtc_time);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	DEBUG("Request UTCinfo from fota server success.\n");

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
  * 1. If time synchronization fail, go to utcinfo service for
  *   the current UTC timestamp.
  * 2. If fail, keep trying the utcinfo service for every 1 minutes.
*/
static int request_utcinfo(struct fota_client_info * pDevice_info,
					char * * pUtc_time)
{
	int iRet = 0;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pUtc_time, -1);

	iRet = execute_request_utcinfo(pDevice_info, pUtc_time);

	while (0 != iRet)
	{
		execute_pause(UTC_INFO_RETRY_INTERVAL);

		iRet = execute_request_utcinfo(pDevice_info, pUtc_time);
	}

	DEBUG("Request utcinfo success.\n");

	return 0;
}

static int execute_request_tzinfo(
				struct fota_client_info * pDevice_info,
				char * * pTZ)
{
	char * pMsg = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int iHttp_code = 0;
	int iMsg_code = 0;
	struct http_chunk http_header;
	struct http_chunk http_data;

	DEBUG("Now request TZinfo from fota server.\n");

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pTZ, -1);

	memset(&http_header, '\0', sizeof (http_header));
	memset(&http_data, '\0', sizeof (http_data));

	iRet = send_tzinfo_request(pDevice_info,
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

	iRet = get_tzinfo_response(&http_data, pTZ);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	DEBUG("Request TZinfo from fota server success.\n");

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

static int fota_tzinfo_action(
				struct fota_client_info * pDevice_info,
				char * * pTZ)
{
	int iRet = 0;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pTZ, -1);

retry:

	iRet = execute_request_tzinfo(pDevice_info, pTZ);
	if (iRet > 0)
	{
		iRet = fota_handle_token_expire(iRet, pDevice_info);
		if (0 == iRet)
		{
			FREE_POINTER(*pTZ);
			goto retry;
		}
		else
		{
			return -1;
		}
	}
	else if (iRet < 0)
	{
		DEBUG("execute_request_tzinfo fail.\n");
		return -1;
	}

	return 0;
}

/*
  * 1. If fails with tzinfo, keep trying for every 1 minutes.
*/
static int request_tzinfo(
				struct fota_client_info * pDevice_info,
				char * * pTZ)
{
	int iRet = 0;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pTZ, -1);

	iRet = fota_tzinfo_action(pDevice_info, pTZ);

	while (0 != iRet)
	{
		execute_pause(TZ_INFO_RETRY_INTERVAL);

		iRet = fota_tzinfo_action(pDevice_info, pTZ);
	}

	DEBUG("Request tzinfo success.\n");

	return 0;
}

static int sync_utc_time(struct fota_client_info * pDevice_info)
{
	char sCmd_buf[128] = { 0 };
	char * pUtc_time = NULL;
	int iSync_time_done = 0;
	int iRet = 0;
	int iResult_code = -1;

	RETURN_IF_NULL(pDevice_info, -1);

	if (pDevice_info->iNtp_enable)
	{
		DEBUG("Start ntp time synchronization.\n");

		iRet = execute_ntp_request(pDevice_info->sNtp_server);
		if (0 == iRet)
		{
			DEBUG("The ntp time synchronization is successful.\n");

			iSync_time_done = 1;
		}
	}

	if (1 != iSync_time_done)
	{
		DEBUG("Go to utcinfo service for the current UTC timestamp.\n");

		iRet = request_utcinfo(pDevice_info, &pUtc_time);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);

		snprintf(sCmd_buf, sizeof (sCmd_buf),
			"date -u %s", pUtc_time);
		system(sCmd_buf);

		DEBUG("Set system time<%s>.\n", pUtc_time);
	}

	DEBUG("The time synchronization is successful.\n");

	iResult_code = 0;

done:

	FREE_POINTER(pUtc_time);

	return iResult_code;
}

static int sync_time_zone(struct fota_client_info * pDevice_info)
{
	char sCmd_buf[128] = { 0 };
	char * pTZ = NULL;
	int iRet = 0;
	int iResult_code = -1;

	RETURN_IF_NULL(pDevice_info, -1);

	DEBUG("Start the request of tzinfo for time zone update.\n");

	iRet = request_tzinfo(pDevice_info, &pTZ);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	snprintf(sCmd_buf, sizeof (sCmd_buf), "echo '%s' > /etc/TZ",
		pTZ);
	system(sCmd_buf);

	DEBUG("Set local time zone<%s>.\n", pTZ);

	iResult_code = 0;

done:

	FREE_POINTER(pTZ);

	return iResult_code;
}

static int wait_for_next_sync(void)
{
	int iInterval = PERIODIC_TIME_SYNC_INTERVAL;

	execute_pause(iInterval);

	return 0;
}

static int periodic_time_sync(
				struct fota_client_info * pDevice_info)
{
	int iRet = 0;
	int iCount = 0;
	int iNtp_retry_interval = 1;

	RETURN_IF_NULL(pDevice_info, -1);

	while (1)
	{
		DEBUG("Now wait for the next synchronization.\n");
		wait_for_next_sync();

		DEBUG("Start the periodic NTP time synchronization %d.\n",
			++iCount);

retry:

		iRet = execute_ntp_request(pDevice_info->sNtp_server);
		if (0 == iRet)
		{
			DEBUG("The time synchronization is successful. "
				"Start scheduler.\n");

			iNtp_retry_interval = 1;
			start_scheduler();
		}
		else
		{
			iNtp_retry_interval = (2 * iNtp_retry_interval)
				% MAX_NTP_RETRY_INTERVAL;
			DEBUG("Ntp fail! Wait %d seconds and try again.\n",
				iNtp_retry_interval);

			execute_pause(iNtp_retry_interval);

			goto retry;
		}
	}

	return -1;
}

static int handle_fota_register(
					struct fota_client_info * pDevice_info)
{
	int iRet = 0;
	int iFota_retry_interval = 1;

	RETURN_IF_NULL(pDevice_info, -1);

	iRet = get_registration_status(pDevice_info);
	if (REGISTERED == iRet)
	{
		return 0;
	}

	while (1)
	{
		iRet = register_to_fota_server(
			pDevice_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, retry);

		iRet = upload_device_info(pDevice_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, retry);

		break;

retry:

		iFota_retry_interval = (2 * iFota_retry_interval)
			% MAX_FOTA_RETRY_INTERVAL;
		DEBUG("Fota register fail! Wait %d seconds and try again.\n",
				iFota_retry_interval);

		execute_pause(iFota_retry_interval);
	}

	return 0;
}

static int time_sync_output_action(void)
{
	char * const argv[] =
		{
			FOTA_OUTPUT_TOOL_PATH,
			"fw-encrypt",
			NULL
		};
	int iResult_code = -1;

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

	openlog("TIME", LOG_PID, LOG_DAEMON);

	iRet = tz_init_config(argc, argv, &tz_config_cmd);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = wait_network_accessible(
		tz_config_cmd.client_info.sServer_domain,
		AF_UNSPEC);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = setenv("FOTA_ACTION", "time_sync", 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	DEBUG("Execute ACTION: %s.\n", tz_config_cmd.pAction);

	iRet = sync_utc_time(&tz_config_cmd.client_info);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	/*
	  * 1. The FOTA API request needs to verify the server's certificate.
	  * 2. The server's certificate has two time parameter, a time when
	  *   it begin taking effect and a time when it stop taking effect.
	  * 3. So we send the FOTA API requests after the time
	  *   synchronization is successful.
	  * 2018-05-25 --liushenghui
	*/
	iRet = handle_fota_register(&tz_config_cmd.client_info);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = sync_time_zone(&tz_config_cmd.client_info);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = setenv("TIME_SYNC_DONE", "1", 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = setenv("FOTA_ACTION_RESULT", "OK", 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = time_sync_output_action();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	start_scheduler();

	if (!strcmp(tz_config_cmd.pAction, "daemon"))
	{
		iRet = periodic_time_sync(&tz_config_cmd.client_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}

	iResult_code = 0;

done:

	tz_free_config(&tz_config_cmd);
	closelog();

	raise(SIGTERM);

	return iResult_code;
}

