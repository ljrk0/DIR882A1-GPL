
/*
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include <json.h>
#include "fota.h"

extern struct fota_config fota_config_cmd;

static int get_version_info(json_object * pVer,
						struct version * pFw_version)
{
	const char * pJson_string = NULL;
	int iResult_code = -1;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pVer, -1);
	RETURN_IF_NULL(pFw_version, -1);

	pItem = json_object_object_get(pVer, "major");
	GOTO_IF_NULL(pItem, done);
	pJson_string = json_object_get_string(pItem);
	GOTO_IF_NULL(pJson_string, done);
	snprintf(pFw_version->sMajor,
		sizeof (pFw_version->sMajor), "%s", pJson_string);

	pItem = json_object_object_get(pVer, "minor");
	GOTO_IF_NULL(pItem, done);
	pJson_string = json_object_get_string(pItem);
	GOTO_IF_NULL(pJson_string, done);
	snprintf(pFw_version->sMinor,
		sizeof (pFw_version->sMinor), "%s", pJson_string);

	pItem = json_object_object_get(pVer, "rev");
	if (pItem)
	{
		pJson_string = json_object_get_string(pItem);
		if (pJson_string)
		{
			snprintf(pFw_version->sRevision,
				sizeof (pFw_version->sRevision), "%s", pJson_string);
		}
	}

	iResult_code = 0;

done:

	if (iResult_code != 0)
	{
		pFw_version->sMajor[0] = '\0';
		pFw_version->sMinor[0] = '\0';
		pFw_version->sRevision[0] = '\0';
	}

	return iResult_code;
}

static int get_fw_info(json_object * pFw, struct fota_fw_info * pFw_info)
{
	const char * pJson_string = NULL;
	int iResult_code = -1;
	json_object * pVersion = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pFw, -1);
	RETURN_IF_NULL(pFw_info, -1);

	FREE_POINTER(pFw_info->pRelease_note);
	FREE_POINTER(pFw_info->pUrl);

	pVersion = json_object_object_get(pFw, "ver");
	GOTO_IF_NULL(pVersion, done);

	if (get_version_info(pVersion, &pFw_info->fw_version) < 0)
	{
		goto done;
	}

	pItem = json_object_object_get(pFw, "ts");
	GOTO_IF_NULL(pItem, done);
	pFw_info->iSeconds = json_object_get_int(pItem);
	if (pFw_info->iSeconds <= INT32_MIN
		|| pFw_info->iSeconds >= INT32_MAX
	)
	{
		DEBUG("Invalid time value: %ld.\n", pFw_info->iSeconds);
		goto done;
	}

	pItem = json_object_object_get(pFw, "url");
	GOTO_IF_NULL(pItem, done);
	pJson_string = json_object_get_string(pItem);
	GOTO_IF_NULL(pJson_string, done);
	pFw_info->pUrl = strdup(pJson_string);
	if (!pFw_info->pUrl)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	pItem = json_object_object_get(pFw, "release_note");
	if (pItem)
	{
		pJson_string = json_object_get_string(pItem);
		if (pJson_string)
		{
			pFw_info->pRelease_note = strdup(pJson_string);
			if (!pFw_info->pRelease_note)
			{
				DEBUG("strdup fail, %s, errno=%d.\n",
					strerror(errno), errno);
				goto done;
			}
		}
	}

	pFw_info->iExist = 1;
	iResult_code = 0;

done:

	if (iResult_code != 0)
	{
		pFw_info->iExist = 0;
		pFw_info->fw_version.sMajor[0] = '\0';
		pFw_info->fw_version.sMinor[0] = '\0';
		pFw_info->fw_version.sRevision[0] = '\0';

		FREE_POINTER(pFw_info->pRelease_note);
		FREE_POINTER(pFw_info->pUrl);
	}

	return iResult_code;
}

int get_fota_query_response(
						struct fota_latest_fw_info * pLatest_fw_info,
						struct http_chunk * pHttp_data)
{
	int iResult_code = -1;
	json_object * pRoot = NULL;
	json_object * pVersion = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pLatest_fw_info, -1);

	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(pHttp_data->pData, -1);

	if (0 != pHttp_data->iStatus_code)
	{
		DEBUG("Get http response fail, iStatus_code=%d.\n",
			pHttp_data->iStatus_code);
		return -1;
	}

	if (fota_config_cmd.iVerbose)
	{
		DEBUG("http body=%s.\n", (char *)pHttp_data->pData);
	}

	pRoot = json_tokener_parse(pHttp_data->pData);
	RETURN_IF_NULL(pRoot, -1);

	pItem = json_object_object_get(pRoot, "stable");
	if (pItem)
	{
		if (get_fw_info(pItem, &pLatest_fw_info->stable) < 0)
		{
			goto done;
		}
	}

	pItem = json_object_object_get(pRoot, "beta");
	if (pItem)
	{
		get_fw_info(pItem, &pLatest_fw_info->beta);
	}

	pItem = json_object_object_get(pRoot, "intermediate");
	if (pItem)
	{
		get_fw_info(pItem, &pLatest_fw_info->intermediate);
	}

	pVersion = json_object_object_get(pRoot, "enforced_ver");
	if (pVersion)
	{
		get_version_info(pVersion, &pLatest_fw_info->enforced_version);
	}

	iResult_code = 0;

done:

	json_object_put(pRoot);

	return iResult_code;
}

int get_fota_event_parameters(
						struct fota_event * pEvent,
						char * * pParam)
{
	const char * pJson_string = NULL;
	int iResult_code = -1;
	int iIndex = 0;
	json_object * pRoot = NULL;
	json_object * pMsg = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pParam, -1);
	RETURN_IF_NULL(pEvent, -1);
	RETURN_IF_NULL(pEvent->pEvent_id, -1);

	pRoot = json_object_new_object();
	RETURN_IF_NULL(pRoot, -1);

	pItem = json_object_new_string(pEvent->pEvent_id);
	GOTO_IF_NULL(pItem, done);
	json_object_object_add(pRoot, "event_id", pItem);

	pMsg = json_object_new_array();
	GOTO_IF_NULL(pMsg, done);
	json_object_object_add(pRoot, "variables", pMsg);

	for (iIndex = 0 ;
		iIndex < sizeof (pEvent->pVariables) / sizeof (char *) ;
		++iIndex
	)
	{
		if (pEvent->pVariables[iIndex])
		{
			pItem = json_object_new_string(pEvent->pVariables[iIndex]);
			if (pItem)
			{
				if (json_object_array_add(pMsg, pItem) < 0)
				{
					DEBUG("json_object_array_add fail.\n");
				}
			}
			else
			{
				DEBUG("json_object_new_string fail.\n");
			}
		}
	}

	pJson_string = json_object_to_json_string(pRoot);
	GOTO_IF_NULL(pJson_string, done);

	*pParam = strdup(pJson_string);
	if (NULL == *pParam)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	if (fota_config_cmd.iVerbose)
	{
		DEBUG("pParam=%s.\n", *pParam);
	}

	iResult_code = 0;

done:

	json_object_put(pRoot);

	return iResult_code;
}

