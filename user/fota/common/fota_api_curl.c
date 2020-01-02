
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include <curl/curl.h>
#include <fota_debug.h>
#include <fota_common.h>
#include <fota_api.h>

static size_t save_recv_data_to_memory(void * pPtr,
							size_t iSize,
							size_t iNmemb,
							void * pUser_info)
{
	size_t iWritten = 0;
	struct http_chunk * pData = NULL;

	RETURN_IF_NULL(pUser_info, 0);

	pData = (struct http_chunk *) pUser_info;
	pData->iStatus_code -= 1;

	RETURN_IF_NULL(pPtr, 0);

	//DEBUG("iSize=%lu, iNmemb=%lu.\n",
	//	(unsigned long)iSize, (unsigned long)iNmemb);

	if (CHUNK_TYPE_MEM != pData->iType)
	{
		DEBUG("Expect chunk type %d, input chunk type %d.\n",
			CHUNK_TYPE_MEM, pData->iType);
		return 0;
	}

	iWritten = iSize * iNmemb;
	pData->pData = realloc(pData->pData,
		pData->iSize + iWritten + 1);
	if (!pData->pData)
	{
		DEBUG("realloc fail, %s, errno=%d.\n", strerror(errno), errno);
		return 0;
	}

	memcpy((char *)pData->pData + pData->iSize, pPtr, iWritten);
	pData->iSize += iWritten;
	*((char *)pData->pData + pData->iSize) = '\0';

	pData->iStatus_code += 1;

	return iWritten;
}

static size_t save_recv_data_to_file(void * pPtr,
							size_t iSize,
							size_t iNmemb,
							void * pUser_info)
{
	size_t iWritten = 0;
	struct http_chunk * pData = NULL;
	FILE * fp = NULL;

	RETURN_IF_NULL(pUser_info, 0);

	pData = (struct http_chunk *) pUser_info;
	pData->iStatus_code -= 1;

	RETURN_IF_NULL(pData->pData, 0);
	RETURN_IF_NULL(pPtr, 0);

	//DEBUG("iSize=%lu, iNmemb=%lu.\n",
	//	(unsigned long)iSize, (unsigned long)iNmemb);

	if (CHUNK_TYPE_FILE != pData->iType)
	{
		DEBUG("Expect chunk type %d, input chunk type %d.\n",
			CHUNK_TYPE_FILE, pData->iType);
		return 0;
	}

	fp = (FILE *) pData->pData;
	iWritten = fwrite(pPtr, iSize, iNmemb, fp);
	if (iWritten != iNmemb)
	{
		DEBUG("fwrite fail, %s, errno=%d.\n", strerror(errno), errno);
		return 0;
	}

	pData->iStatus_code += 1;

	return iWritten;
}

size_t handle_recv_data(void * pPtr,
							size_t iSize,
							size_t iNmemb,
							void * pUser_info)
{
	struct http_chunk * pData = NULL;

	RETURN_IF_NULL(pPtr, 0);
	RETURN_IF_NULL(pUser_info, 0);

	pData = (struct http_chunk *) pUser_info;

	if (CHUNK_TYPE_MEM == pData->iType)
	{
		return save_recv_data_to_memory(pPtr, iSize,
			iNmemb, pUser_info);
	}
	else if (CHUNK_TYPE_FILE == pData->iType)
	{
		return save_recv_data_to_file(pPtr, iSize,
			iNmemb, pUser_info);
	}
	else
	{
		DEBUG("Unsupported chunk type: %d.\n", pData->iType);
	}

	return 0;
}

int get_http_response_code(int * pCode,
							struct http_chunk * pHttp_header)
{
	int iResult_code = -1;
	int iItem_num = 0;

	RETURN_IF_NULL(pCode, -1);
	RETURN_IF_NULL(pHttp_header, -1);
	RETURN_IF_NULL(pHttp_header->pData, -1);

	if (0 != pHttp_header->iStatus_code)
	{
		DEBUG("Get http response header fail, iStatus_code=%d.\n",
			pHttp_header->iStatus_code);
		return -1;
	}

	if (iVerbose)
	{
		DEBUG("http header=%s.\n", (char *)pHttp_header->pData);
	}

	iItem_num = sscanf(pHttp_header->pData, " %*s %d ", pCode);
	if (iItem_num != 1)
	{
		DEBUG("Get http response code fail, %d.\n", iItem_num);
		if (iItem_num < 0)
		{
			DEBUG("sscanf fail, %s, errno=%d.\n",
				strerror(errno), errno);
		}
		goto done;
	}

	iResult_code = 0;

done:

	DEBUG("iHttp_code=%d.\n", *pCode);
	return iResult_code;
}

int send_register_request(
						struct fota_client_info * pDevice_info,
						const char * pParameters,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data)
{
	char sUrl[128] = { 0 };
	int iResult_code = -1;
	CURL * pCurl = NULL;
	CURLcode res;
	struct curl_slist * pChunk = NULL;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pParameters, -1);
	RETURN_IF_NULL(pHttp_header, -1);
	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(pCacert, -1);

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

	snprintf(sUrl, sizeof (sUrl), "https://%s%s",
		pDevice_info->sServer_domain, REGISTER_URI);
	res = curl_easy_setopt(pCurl, CURLOPT_URL, sUrl);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	pChunk = curl_slist_append(pChunk, "Content-Type: application/json");
	GOTO_IF_NULL(pChunk, done);

	res = curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pChunk);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_CAINFO,
		pCacert);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, pParameters);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	res = curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE,
		(long)strlen(pParameters));
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

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
	res = curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT,
		8L);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	if (iVerbose)
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
	curl_slist_free_all(pChunk);
	curl_global_cleanup();

	return iResult_code;
}

int send_refresh_request(
						struct fota_client_info * pDevice_info,
						const char * pParameters,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data)
{
	char sUrl[128] = { 0 };
	int iResult_code = -1;
	CURL * pCurl = NULL;
	CURLcode res;
	struct curl_slist * pChunk = NULL;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pParameters, -1);
	RETURN_IF_NULL(pHttp_header, -1);
	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(pCacert, -1);

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

	snprintf(sUrl, sizeof (sUrl), "https://%s%s",
		pDevice_info->sServer_domain, REFRESH_TOKEN_URI);
	res = curl_easy_setopt(pCurl, CURLOPT_URL, sUrl);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	pChunk = curl_slist_append(pChunk, "Content-Type: application/json");
	GOTO_IF_NULL(pChunk, done);

	res = curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pChunk);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_CAINFO,
		pCacert);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, pParameters);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	res = curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE,
		(long)strlen(pParameters));
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

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

	res = curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT,
		8L);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	if (iVerbose)
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
	curl_slist_free_all(pChunk);
	curl_global_cleanup();

	return iResult_code;
}

int send_upload_request(
						struct fota_client_info * pDevice_info,
						const char * pParameters,
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
	RETURN_IF_NULL(pParameters, -1);
	RETURN_IF_NULL(pHttp_header, -1);
	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(pCacert, -1);

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

	snprintf(sUrl, sizeof (sUrl), "https://%s%s",
		pDevice_info->sServer_domain, UPDATE_DEVICE_INFO_URI);
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

	pChunk = curl_slist_append(pChunk, "Content-Type: application/json");
	GOTO_IF_NULL(pChunk, done);

	res = curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pChunk);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_CAINFO,
		pCacert);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, pParameters);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	res = curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE,
		(long)strlen(pParameters));
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, "PATCH");

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

	res = curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT,
		8L);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	if (iVerbose)
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

