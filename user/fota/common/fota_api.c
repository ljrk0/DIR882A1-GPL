
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>

#include <fota_debug.h>
#include <fota_common.h>
#include <fota_api.h>

int iVerbose;
char * pCacert;

struct fota_message fota_error_msg[] =
{
	//{"100", "Invalid request", "Bad or missing parameters"},
	{"101", "Invalid grant", "Bad grant_type or invalid refresh token"},
	{"102", "Invalid scope", "Invalid scope"},
	{"110", "Unauthorized", "Unauthorized access or bad username/password"},
	{"111", "Invalid access token", "Bad or expired access token"},
	{"112", "Invalid client", "Bad client ID/secret"},
	{"140", "Resource not found", "Bad URI"},
	{NULL}
};

const struct fota_message * get_error_msg(const char * pMsg_code);

int get_fota_register_parameters(
						struct fota_client_info * pDevice_info,
						char * * pParam);
int send_register_request(
						struct fota_client_info * pDevice_info,
						const char * pParameters,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data);
int get_fota_register_response(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_data);

int get_fota_refresh_parameters(
						struct fota_client_info * pDevice_info,
						char * * pParam);
int send_refresh_request(
						struct fota_client_info * pDevice_info,
						const char * pParameters,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data);
int get_fota_refresh_response(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_data);

int get_fota_upload_parameters(
						struct fota_client_info * pDevice_info,
						char * * pParam);
int send_upload_request(
						struct fota_client_info * pDevice_info,
						const char * pParameters,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data);

enum fota_request_type action_of_error_code(
						enum fota_error_code iError_code)
{
	enum fota_request_type iType = 0;

	if (CODE_INVALID_REFRESH_TOKEN == iError_code)
	{
		iType = TYPE_REGISTER;
	}
	else if (CODE_INVALID_ACCESS_TOKEN == iError_code)
	{
		iType = TYPE_REFRESH_TOKEN;
	}
	else
	{
		iType = TYPE_REGISTER;
	}

	return iType;
}

int get_registration_status(struct fota_client_info * pDevice_info)
{
	RETURN_IF_NULL(pDevice_info, -1);

	if (pDevice_info->pAccess_token
		&& 0 != strcmp(pDevice_info->pAccess_token, "")
	)
	{
		return REGISTERED;
	}

	return UNREGISTERED;
}

/*
  * Register the device to D-Link FOTA server.
  * Input:
  *   Client ID.
  *   Client secret: not used, leave an empty field.
  *   Grant type.
  *   User name: use the uppercase MAC address labeled on the device.
  *   Password: device ID.
  * Output:
  *   Access token: API access token.
  *   Valid lifetime: how many seconds the access token is valid for, e.g., 3600.
  *   Refresh token.
  * 2018-01-25 --liushenghui
*/
int register_to_fota_server(struct fota_client_info * pDevice_info)
{
	char * pParameters = NULL;
	char * pMsg = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int iHttp_code = 0;
	int iMsg_code = 0;
	struct http_chunk http_header;
	struct http_chunk http_data;

	DEBUG("Now REGISTER the device to the server.\n");

	RETURN_IF_NULL(pDevice_info, -1);

	memset(&http_header, '\0', sizeof (http_header));
	memset(&http_data, '\0', sizeof (http_data));

	iRet = get_fota_register_parameters(pDevice_info, &pParameters);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = send_register_request(pDevice_info, pParameters,
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

	iRet = get_fota_register_response(pDevice_info, &http_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	DEBUG("REGISTER the device to the server success.\n");

	iRet = setenv("FOTA_ACCESS_TOKEN",
		pDevice_info->pAccess_token, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = setenv("FOTA_REFRESH_TOKEN",
		pDevice_info->pRefresh_token, 1);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = setenv("FOTA_REGISTER_STATUS", "1", 1);
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

/*
  * Update access token.
  * Input:
  *   Client ID.
  *   Client secret: not used, leave an empty field.
  *   Grant type.
  *   Refresh token.
  * Output:
  *   Access token: API access token.
  *   Valid lifetime: how many seconds the access token is valid for, e.g., 3600.
  * 2018-01-26 --liushenghui
*/
int refresh_access_token(struct fota_client_info * pDevice_info)
{
	char * pParameters = NULL;
	char * pMsg = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int iHttp_code = 0;
	int iMsg_code = 0;
	struct http_chunk http_header;
	struct http_chunk http_data;

	DEBUG("Now REFRESH the access token.\n");

	RETURN_IF_NULL(pDevice_info, -1);

	memset(&http_header, '\0', sizeof (http_header));
	memset(&http_data, '\0', sizeof (http_data));

	iRet = get_fota_refresh_parameters(pDevice_info, &pParameters);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = send_refresh_request(pDevice_info, pParameters,
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

	iRet = get_fota_refresh_response(pDevice_info, &http_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	DEBUG("REFRESH the access token success.\n");

	iRet = setenv("FOTA_ACCESS_TOKEN",
		pDevice_info->pAccess_token, 1);
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

/*
  * Update device information.
  * Input:
  *   Variant: Firmware variant, which identifies a special firmware build for
  *      a certain project or region-specific use, Optional.
  *   Version: current firmware version, Optional.
  *   Access token.
  * Output:
  *   None.
  * 2018-01-26 --liushenghui
*/
int upload_device_info(struct fota_client_info * pDevice_info)
{
	char * pParameters = NULL;
	char * pMsg = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int iHttp_code = 0;
	int iMsg_code = 0;
	struct http_chunk http_header;
	struct http_chunk http_data;

	DEBUG("Now UPLOAD the device information to the server.\n");

	RETURN_IF_NULL(pDevice_info, -1);

	memset(&http_header, '\0', sizeof (http_header));
	memset(&http_data, '\0', sizeof (http_data));

	iRet = get_fota_upload_parameters(pDevice_info, &pParameters);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = send_upload_request(pDevice_info, pParameters,
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

	DEBUG("UPLOAD the device information to the server success.\n");

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

int fota_handle_token_expire(
						enum fota_request_type iType,
						struct fota_client_info * pDevice_info)
{
	static int iRetrys_after_fail = 2;
	int iRet = 0;
	int iResult_code = -1;

	RETURN_IF_NULL(pDevice_info, -1);

	if (iRetrys_after_fail <= 0)
	{
		goto done;
	}

	--iRetrys_after_fail;

	if (TYPE_REFRESH_TOKEN == iType)
	{
		FREE_POINTER(pDevice_info->pAccess_token);

		iRet = refresh_access_token(pDevice_info);
		if (iRet != 0)
		{
			DEBUG("Refresh the access token fail.\n");
			iType = TYPE_REGISTER;
		}
	}

	if (TYPE_REGISTER == iType)
	{
		FREE_POINTER(pDevice_info->pAccess_token);
		FREE_POINTER(pDevice_info->pRefresh_token);

		iRet = register_to_fota_server(pDevice_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);

		iRet = upload_device_info(pDevice_info);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}

	iResult_code = 0;

done:

	return iResult_code;
}

static int skip_end_of_line(FILE * fp)
{
	char sLine[256] = { 0 };

	RETURN_IF_NULL(fp, -1);

	while (fgets(sLine, sizeof (sLine), fp))
	{
		if ('\n' == sLine[strlen(sLine) - 1])
		{
			return 1;
		}
	}

	return 0;
}

static int fota_set_config_option(struct fota_option * pOption,
								const char * pConfig_name,
								const char * pConfig_value)
{
	int iIndex = 0;

	RETURN_IF_NULL(pOption, -1);
	RETURN_IF_NULL(pConfig_name, -1);
	RETURN_IF_NULL(pConfig_value, -1);

	while (pOption[iIndex].pName)
	{
		if (!strcmp(pOption[iIndex].pName, pConfig_name))
		{
			break;
		}

		++iIndex;
	}

	if (!pOption[iIndex].pName)
	{
		return 0;
	}

	if (OPTION_TYPE_STRING == pOption[iIndex].iType)
	{
		RETURN_IF_NULL(pOption[iIndex].pAddr, -1);

		snprintf((char *)pOption[iIndex].pAddr,
			pOption[iIndex].iBuf_len,
			"%s", pConfig_value);
	}
	else if (OPTION_TYPE_INT == pOption[iIndex].iType)
	{
		RETURN_IF_NULL(pOption[iIndex].pAddr, -1);

		*(int *)pOption[iIndex].pAddr = atoi(pConfig_value);
	}
	else if (OPTION_TYPE_STRING2 == pOption[iIndex].iType)
	{
		RETURN_IF_NULL(pOption[iIndex].pAddr2, -1);

		*pOption[iIndex].pAddr2 = strdup(pConfig_value);
		if (!*pOption[iIndex].pAddr2)
		{
			DEBUG("strdup fail, %s, errno=%d.\n",
				strerror(errno), errno);
			return -1;
		}
	}
	else if (OPTION_TYPE_FILE == pOption[iIndex].iType)
	{
		RETURN_IF_NULL(pOption[iIndex].pAddr2, -1);

		if (fota_get_file_content(pConfig_value,
			pOption[iIndex].pAddr2, NULL) < 0)
		{
			DEBUG("fota_get_file_content fail.\n");
			unlink(pConfig_value);
			return -1;
		}

		unlink(pConfig_value);
	}

	return 0;
}

int fota_parse_config_file(
						const char * pConfig_file,
						struct fota_option * pConfig)
{
	char sLine[256] = { 0 };
	char sConfig_name[64] = { 0 };
	char sConfig_value[128] = { 0 };
	int iResult_code = -1;
	int iItem_num = 0;
	FILE * fp = NULL;

	RETURN_IF_NULL(pConfig_file, -1);
	RETURN_IF_NULL(pConfig, -1);

	fp = fopen(pConfig_file, "r");
	if (!fp)
	{
		DEBUG("fopen fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	while (fgets(sLine, sizeof (sLine), fp))
	{
		if ('#' == *sLine)
		{
			goto skip_line;
		}

		*sConfig_name = '\0';
		*sConfig_value = '\0';
		iItem_num = sscanf(sLine, "%63[^=]=%127[^;];",
			sConfig_name, sConfig_value);
		GOTO_IF_NOT_EQUAL(iItem_num, 2, skip_line);

		if (fota_set_config_option(pConfig, sConfig_name,
			sConfig_value) < 0)
		{
			DEBUG("fota_set_config_option fail.\n");
			goto done;
		}

skip_line:

		if ('\n' != sLine[strlen(sLine) - 1])
		{
			skip_end_of_line(fp);
		}
	}

	if (ferror(fp))
	{
		DEBUG("Parsing %s encountered problems, %s, errno=%d.\n",
			pConfig_file, strerror(errno), errno);
		goto done;
	}

	iResult_code = 0;

done:

	if (fp && fclose(fp) < 0)
	{
		DEBUG("fclose fail, %s, errno=%d.\n", strerror(errno), errno);
	}

	return iResult_code;
}

int free_device_info(struct fota_client_info * pDevice_info)
{
	RETURN_IF_NULL(pDevice_info, -1);

	FREE_POINTER(pDevice_info->pClient_id);
	FREE_POINTER(pDevice_info->pClient_secret);
	FREE_POINTER(pDevice_info->pRefresh_token);
	FREE_POINTER(pDevice_info->pAccess_token);
	FREE_POINTER(pDevice_info->pFw_variant);
	FREE_POINTER(pDevice_info->password.pDevice_id);

	return 0;
}

