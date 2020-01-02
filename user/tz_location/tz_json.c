
/*
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include <json.h>
#include "tz.h"

int get_utcinfo_response(
						struct http_chunk * pHttp_data,
						char * * pUtc_time)
{
	const char * pJson_string = NULL;
	int iResult_code = -1;
	json_object * pRoot = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(pHttp_data->pData, -1);
	RETURN_IF_NULL(pUtc_time, -1);

	if (0 != pHttp_data->iStatus_code)
	{
		DEBUG("Get http response fail, iStatus_code=%d.\n",
			pHttp_data->iStatus_code);
		return -1;
	}

	if (iVerbose)
	{
		DEBUG("http body=%s.\n", (char *)pHttp_data->pData);
	}

	pRoot = json_tokener_parse(pHttp_data->pData);
	RETURN_IF_NULL(pRoot, -1);

	pItem = json_object_object_get(pRoot, "utcinfo");
	GOTO_IF_NULL(pItem, done);
	pJson_string = json_object_get_string(pItem);
	GOTO_IF_NULL(pJson_string, done);

	*pUtc_time = strdup(pJson_string);
	if (!*pUtc_time)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	iResult_code = 0;

done:

	json_object_put(pRoot);

	return iResult_code;
}

int get_tzinfo_response(
						struct http_chunk * pHttp_data,
						char * * pTZ)
{
	const char * pJson_string = NULL;
	int iResult_code = -1;
	json_object * pRoot = NULL;
	json_object * pItem = NULL;

	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(pHttp_data->pData, -1);
	RETURN_IF_NULL(pTZ, -1);

	if (0 != pHttp_data->iStatus_code)
	{
		DEBUG("Get http response fail, iStatus_code=%d.\n",
			pHttp_data->iStatus_code);
		return -1;
	}

	if (iVerbose)
	{
		DEBUG("http body=%s.\n", (char *)pHttp_data->pData);
	}

	pRoot = json_tokener_parse(pHttp_data->pData);
	RETURN_IF_NULL(pRoot, -1);

	pItem = json_object_object_get(pRoot, "TZ");
	GOTO_IF_NULL(pItem, done);
	pJson_string = json_object_get_string(pItem);
	GOTO_IF_NULL(pJson_string, done);

	*pTZ = strdup(pJson_string);
	if (!*pTZ)
	{
		DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	iResult_code = 0;

done:

	json_object_put(pRoot);

	return iResult_code;
}

