
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include <curl/curl.h>

#include "tz.h"

extern struct tz_config tz_config_cmd;

int send_utcinfo_request(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data)
{
	char sUrl[512] = { 0 };
	int iResult_code = -1;
	CURL * pCurl = NULL;
	CURLcode res;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pDevice_info->pClient_id, -1);
	RETURN_IF_NULL(pHttp_header, -1);
	RETURN_IF_NULL(pHttp_data, -1);

	memset(pHttp_header, '\0', sizeof (struct http_chunk));
	memset(pHttp_data, '\0', sizeof (struct http_chunk));

	pHttp_header->iType = CHUNK_TYPE_MEM;
	pHttp_data->iType = CHUNK_TYPE_MEM;

	res = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (res != CURLE_OK)
	{
		DEBUG("curl_global_init fail.\n");
		return -1;
	}

	pCurl = curl_easy_init();
	GOTO_IF_NULL(pCurl, done);

	snprintf(sUrl, sizeof (sUrl), "https://%s%s?client_id=%s",
		pDevice_info->sServer_domain, UTC_INFO_URI,
		pDevice_info->pClient_id);

	res = curl_easy_setopt(pCurl, CURLOPT_URL, sUrl);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	/*
	  * 1. The utcinfo API does not require token authentication,
	  *   the server certificate validation SHOULD be skipped
	  *   when accessing this API.
	  * 2. And this behavior is required by the D-Link FOTA specification
	  *   <D-Link_FOTA_Service_Device_API_Reference_v1.0.0.pdf>.
	  * 2018-05-25 --liushenghui
	*/
	res = curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER,
		0L);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, "GET");

	res = curl_easy_setopt(pCurl, CURLOPT_HEADERFUNCTION,
		handle_recv_data);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	res = curl_easy_setopt(pCurl, CURLOPT_HEADERDATA,
		(void *)pHttp_header);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION,
		handle_recv_data);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	res = curl_easy_setopt(pCurl, CURLOPT_WRITEDATA,
		(void *)pHttp_data);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	if (tz_config_cmd.iVerbose)
	{
		curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);
		GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	}

	res = curl_easy_perform(pCurl);
	if (res != CURLE_OK)
	{
		DEBUG("curl_easy_perform() failed: %s.\n",
			curl_easy_strerror(res));
		goto done;
	}

	iResult_code = 0;

done:

	curl_easy_cleanup(pCurl);
	curl_global_cleanup();

	return iResult_code;
}

int send_tzinfo_request(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data)
{
	char sUrl[128] = { 0 };
	char * pAuth_header = NULL;
	int iResult_code = -1;
	int iAuth_header_len = 0;
	CURL * pCurl = NULL;
	CURLcode res;
	struct curl_slist * pChunk = NULL;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pDevice_info->pAccess_token, -1);
	RETURN_IF_NULL(pHttp_header, -1);
	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(tz_config_cmd.pCacert, -1);

	memset(pHttp_header, '\0', sizeof (struct http_chunk));
	memset(pHttp_data, '\0', sizeof (struct http_chunk));

	pHttp_header->iType = CHUNK_TYPE_MEM;
	pHttp_data->iType = CHUNK_TYPE_MEM;

	res = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (res != CURLE_OK)
	{
		DEBUG("curl_global_init fail.\n");
		return -1;
	}

	pCurl = curl_easy_init();
	GOTO_IF_NULL(pCurl, done);

	snprintf(sUrl, sizeof (sUrl), "https://%s%s?timezone=%s",
		pDevice_info->sServer_domain, TZ_INFO_URI,
		pDevice_info->sTZ_location);
	res = curl_easy_setopt(pCurl, CURLOPT_URL, sUrl);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	iAuth_header_len = strlen("Authorization: Bearer ")
		+ strlen(pDevice_info->pAccess_token) + 1;
	pAuth_header = malloc(iAuth_header_len);
	if (!pAuth_header)
	{
		DEBUG("malloc fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}
	snprintf(pAuth_header, iAuth_header_len, "%s%s",
		"Authorization: Bearer ", pDevice_info->pAccess_token);

	pChunk = curl_slist_append(pChunk, pAuth_header);
	GOTO_IF_NULL(pChunk, done);

	res = curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pChunk);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_CAINFO,
		tz_config_cmd.pCacert);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, "GET");

	res = curl_easy_setopt(pCurl, CURLOPT_HEADERFUNCTION,
		handle_recv_data);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	res = curl_easy_setopt(pCurl, CURLOPT_HEADERDATA,
		(void *)pHttp_header);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION,
		handle_recv_data);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	res = curl_easy_setopt(pCurl, CURLOPT_WRITEDATA,
		(void *)pHttp_data);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	if (tz_config_cmd.iVerbose)
	{
		curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);
		GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	}

	res = curl_easy_perform(pCurl);
	if (res != CURLE_OK)
	{
		DEBUG("curl_easy_perform() failed: %s.\n",
			curl_easy_strerror(res));
		goto done;
	}

	iResult_code = 0;

done:

	FREE_POINTER(pAuth_header);
	curl_easy_cleanup(pCurl);
	curl_slist_free_all(pChunk);
	curl_global_cleanup();

	return iResult_code;
}

