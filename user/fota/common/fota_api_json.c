
/*
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include <json.h>
#include <fota_debug.h>
#include <fota_common.h>
#include <fota_api.h>

int get_http_error_message(int * pCode, char * * pMsg,
							struct http_chunk * pHttp_data)
{
	const char * pMessage = NULL;
	int iResult_code = -1;
	json_object * pRoot = NULL;
	//json_object * pError_msg = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(pHttp_data->pData, -1);

	if (0 != pHttp_data->iStatus_code)
	{
		DEBUG("Get http response fail, iStatus_code=%d.\n",
			pHttp_data->iStatus_code);
		return -1;
	}

	pRoot = json_tokener_parse(pHttp_data->pData);
	RETURN_IF_NULL(pRoot, -1);

	if (iVerbose)
	{
		DEBUG("http error message=%s.\n", (char *)pHttp_data->pData);
	}

	//pError_msg = json_object_object_get(pRoot, "error");
	//GOTO_IF_NULL(pError_msg, done);

	pItem = json_object_object_get(pRoot, "error_code");
	GOTO_IF_NULL(pItem, done);

	if (pCode)
	{
		*pCode = json_object_get_int(pItem);
	}

	pItem = json_object_object_get(pRoot, "error_msg");
	GOTO_IF_NULL(pItem, done);
	pMessage = json_object_get_string(pItem);
	GOTO_IF_NULL(pMessage, done);

	if (pMsg)
	{
		*pMsg = strdup(pMessage);
		if (NULL == *pMsg)
		{
			DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
			goto done;
		}
	}

	iResult_code = 0;

done:

	json_object_put(pRoot);

	return iResult_code;
}

int get_fota_register_parameters(
						struct fota_client_info * pDevice_info,
						char * * pParam)
{
	const char * pJson_string = NULL;
	int iResult_code = -1;
	json_object * pRoot = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pParam, -1);
	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pDevice_info->pClient_id, -1);
	RETURN_IF_NULL(pDevice_info->pClient_secret, -1);
	RETURN_IF_NULL(pDevice_info->password.pDevice_id, -1);

	pRoot = json_object_new_object();
	RETURN_IF_NULL(pRoot, -1);

	pItem = json_object_new_string(pDevice_info->pClient_id);
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "client_id", pItem);

	pItem = json_object_new_string("");
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "client_secret", pItem);

	pItem = json_object_new_string("device");
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "grant_type", pItem);

	pItem = json_object_new_string(pDevice_info->sUser_name);
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "username", pItem);

	pItem = json_object_new_string(pDevice_info->password.pDevice_id);
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "password", pItem);

	pJson_string = json_object_to_json_string(pRoot);
	GOTO_IF_NULL(pJson_string, done);

	*pParam = strdup(pJson_string);
	if (NULL == *pParam)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	if (iVerbose)
	{
		DEBUG("pParam=%s.\n", *pParam);
	}

	iResult_code = 0;

done:

	json_object_put(pRoot);

	return iResult_code;
}

int get_fota_register_response(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_data)
{
	const char * pJson_string = NULL;
	int iResult_code = -1;
	json_object * pRoot = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(pHttp_data->pData, -1);

	if (0 != pHttp_data->iStatus_code)
	{
		DEBUG("Get http response fail, iStatus_code=%d.\n",
			pHttp_data->iStatus_code);
		return -1;
	}

	FREE_POINTER(pDevice_info->pAccess_token);
	FREE_POINTER(pDevice_info->pRefresh_token);

	if (iVerbose)
	{
		DEBUG("http body=%s.\n", (char *)pHttp_data->pData);
	}

	pRoot = json_tokener_parse(pHttp_data->pData);
	RETURN_IF_NULL(pRoot, -1);

	pItem = json_object_object_get(pRoot, "access_token");
	GOTO_IF_NULL(pItem, done);
	pJson_string = json_object_get_string(pItem);
	GOTO_IF_NULL(pJson_string, done);

	pDevice_info->pAccess_token = strdup(pJson_string);
	if (!pDevice_info->pAccess_token)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	pItem = json_object_object_get(pRoot, "expires_in");
	GOTO_IF_NULL(pItem, done);
	pDevice_info->iLifetime_of_access_token = json_object_get_int(pItem);

	pItem = json_object_object_get(pRoot, "refresh_token");
	GOTO_IF_NULL(pItem, done);
	pJson_string = json_object_get_string(pItem);
	GOTO_IF_NULL(pJson_string, done);

	pDevice_info->pRefresh_token = strdup(pJson_string);
	if (!pDevice_info->pRefresh_token)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	iResult_code = 0;

done:

	json_object_put(pRoot);

	if (iResult_code != 0)
	{
		FREE_POINTER(pDevice_info->pAccess_token);
	}

	return iResult_code;
}

int get_fota_refresh_parameters(
						struct fota_client_info * pDevice_info,
						char * * pParam)
{
	const char * pJson_string = NULL;
	int iResult_code = -1;
	json_object * pRoot = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pParam, -1);
	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pDevice_info->pClient_id, -1);
	RETURN_IF_NULL(pDevice_info->pClient_secret, -1);
	RETURN_IF_NULL(pDevice_info->pRefresh_token, -1);

	pRoot = json_object_new_object();
	RETURN_IF_NULL(pRoot, -1);

	pItem = json_object_new_string(pDevice_info->pClient_id);
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "client_id", pItem);

	pItem = json_object_new_string("");
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "client_secret", pItem);

	pItem = json_object_new_string("refresh_token");
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "grant_type", pItem);

	pItem = json_object_new_string(pDevice_info->pRefresh_token);
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "refresh_token", pItem);

	pJson_string = json_object_to_json_string(pRoot);
	GOTO_IF_NULL(pJson_string, done);

	*pParam = strdup(pJson_string);
	if (NULL == *pParam)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	if (iVerbose)
	{
		DEBUG("pParam=%s.\n", *pParam);
	}

	iResult_code = 0;

done:

	json_object_put(pRoot);

	return iResult_code;
}

int get_fota_refresh_response(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_data)
{
	const char * pJson_string = NULL;
	int iResult_code = -1;
	json_object * pRoot = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(pHttp_data->pData, -1);

	if (0 != pHttp_data->iStatus_code)
	{
		DEBUG("Get http response fail, iStatus_code=%d.\n",
			pHttp_data->iStatus_code);
		return -1;
	}

	FREE_POINTER(pDevice_info->pAccess_token);

	if (iVerbose)
	{
		DEBUG("http body=%s.\n", (char *)pHttp_data->pData);
	}

	pRoot = json_tokener_parse(pHttp_data->pData);
	RETURN_IF_NULL(pRoot, -1);

	pItem = json_object_object_get(pRoot, "access_token");
	GOTO_IF_NULL(pItem, done);
	pJson_string = json_object_get_string(pItem);
	GOTO_IF_NULL(pJson_string, done);

	pDevice_info->pAccess_token = strdup(pJson_string);
	if (!pDevice_info->pAccess_token)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	pItem = json_object_object_get(pRoot, "expires_in");
	GOTO_IF_NULL(pItem, done);
	pDevice_info->iLifetime_of_access_token = json_object_get_int(pItem);

	iResult_code = 0;

done:

	json_object_put(pRoot);

	return iResult_code;
}

int get_fota_upload_parameters(
						struct fota_client_info * pDevice_info,
						char * * pParam)
{
	const char * pJson_string = NULL;
	int iResult_code = -1;
	json_object * pRoot = NULL;
	json_object * pVersion = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pParam, -1);
	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pDevice_info->pFw_variant, -1);

	pRoot = json_object_new_object();
	RETURN_IF_NULL(pRoot, -1);

	pItem = json_object_new_string(pDevice_info->pFw_variant);
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "fw_variant", pItem);

	pVersion = json_object_new_object();
	GOTO_IF_NULL(pVersion, done);
	json_object_object_add(pRoot, "fw_ver", pVersion);

	pItem = json_object_new_string(pDevice_info->fw_version.sMajor);
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pVersion, "major", pItem);

	pItem = json_object_new_string(pDevice_info->fw_version.sMinor);
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pVersion, "minor", pItem);

	pItem = json_object_new_string(pDevice_info->fw_version.sRevision);
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pVersion, "rev", pItem);

	pJson_string = json_object_to_json_string(pRoot);
	GOTO_IF_NULL(pJson_string, done);

	*pParam = strdup(pJson_string);
	if (NULL == *pParam)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	if (iVerbose)
	{
		DEBUG("pParam=%s.\n", *pParam);
	}

	iResult_code = 0;

done:

	json_object_put(pRoot);

	return iResult_code;
}

