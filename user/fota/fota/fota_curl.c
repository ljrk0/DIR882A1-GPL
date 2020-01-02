
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include <curl/curl.h>

#include "fota.h"

extern struct fota_config fota_config_cmd;

int send_query_request(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data)
{
	char sUrl[128] = { 0 };
	char sQuery_string[128] = { 0 };
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
	RETURN_IF_NULL(fota_config_cmd.pCacert, -1);

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

	if (0 != strcmp(pDevice_info->sLanguage, ""))
	{
		snprintf(sQuery_string, sizeof (sQuery_string), "?lang=%s",
			pDevice_info->sLanguage);
	}

	snprintf(sUrl, sizeof (sUrl), "https://%s%s%s",
		pDevice_info->sServer_domain, FIRMWARE_QUERY_URI,
		sQuery_string);
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
		fota_config_cmd.pCacert);
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

	res = curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT,
		8L);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	if (fota_config_cmd.iVerbose)
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

/*
*/
int send_query_request_2(
						struct fota_client_info * pDevice_info,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data)
{
	char sUrl[256] = { 0 };
	char sLanguage[64] = { 0 };
	int iResult_code = -1;
	CURL * pCurl = NULL;
	CURLcode res;

	RETURN_IF_NULL(pDevice_info, -1);
	RETURN_IF_NULL(pHttp_header, -1);
	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(fota_config_cmd.pCacert, -1);

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

	if (0 != strcmp(pDevice_info->sLanguage, ""))
	{
		snprintf(sLanguage, sizeof (sLanguage), "&lang=%s",
			pDevice_info->sLanguage);
	}

	snprintf(sUrl, sizeof (sUrl), "https://%s%s"
		"?series=%s&model=%s&hw_ver=%s&variant=%s%s",
		pDevice_info->sServer_domain, FIRMWARE_QUERY_URI_2,
		pDevice_info->sSeries_name,
		pDevice_info->sModel_name,
		pDevice_info->sHardware_version,
		pDevice_info->pFw_variant,
		sLanguage);

	res = curl_easy_setopt(pCurl, CURLOPT_URL, sUrl);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_CAINFO,
		fota_config_cmd.pCacert);
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

	res = curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT,
		8L);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	if (fota_config_cmd.iVerbose)
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

int send_event_request(
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
	RETURN_IF_NULL(fota_config_cmd.pCacert, -1);

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
		pDevice_info->sServer_domain, EVENT_NOTIFICATION_URI);
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
		fota_config_cmd.pCacert);
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
	if (fota_config_cmd.iVerbose)
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

int send_download_firmware_request(
						struct fota_fw_info * pFw,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data)
{
	int iResult_code = -1;
	CURL * pCurl = NULL;
	CURLcode res;
	FILE * fp = NULL;

	RETURN_IF_NULL(pFw, -1);
	RETURN_IF_NULL(pFw->pUrl, -1);
	RETURN_IF_NULL(pHttp_header, -1);
	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(fota_config_cmd.pCacert, -1);
	RETURN_IF_NULL(fota_config_cmd.pFw_path, -1);

	fp = fopen(fota_config_cmd.pFw_path, "wb");
	if (!fp)
	{
		DEBUG("fopen fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	memset(pHttp_header, '\0', sizeof (struct http_chunk));
	memset(pHttp_data, '\0', sizeof (struct http_chunk));

	pHttp_header->iType = CHUNK_TYPE_MEM;
	pHttp_data->iType = CHUNK_TYPE_FILE;
	pHttp_data->pData = (void *)fp;

	res = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (res != CURLE_OK)
	{
		DEBUG("curl_global_init fail.\n");
		goto done;
	}

	pCurl = curl_easy_init();
	GOTO_IF_NULL(pCurl, done);

	res = curl_easy_setopt(pCurl, CURLOPT_URL, pFw->pUrl);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_CAINFO,
		fota_config_cmd.pCacert);
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

	if (fota_config_cmd.iVerbose)
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

	if (ferror(fp))
	{
		DEBUG("Write %s encountered problems, %s, errno=%d.\n",
			fota_config_cmd.pFw_path, strerror(errno), errno);
		goto done;
	}

	iResult_code = 0;

done:

	if (fclose(fp) < 0)
	{
		DEBUG("fclose fail, %s, errno=%d.\n", strerror(errno), errno);
	}

	curl_easy_cleanup(pCurl);
	curl_global_cleanup();

	return iResult_code;
}

static int update_progress_by_percent(
				struct fota_progress_measure * pProgress,
				curl_off_t dl_total, curl_off_t dl_now,
				curl_off_t ul_total, curl_off_t ul_now)
{
	static int iLast_percent = 0;
	int iNow_percent = 0;

	RETURN_IF_NULL(pProgress, 0);
	RETURN_IF_NULL(pProgress->pProgress, 0);

	if (PROGRESS_TYPE_PERCENT != pProgress->iType)
	{
		DEBUG("Expect progress type %d, input progress type %d.\n",
			PROGRESS_TYPE_PERCENT, pProgress->iType);
		return 0;
	}

	if (pProgress->iPercent < 1 || pProgress->iPercent > 100)
	{
		DEBUG("Invalid percent value: %d, range: 1-100.\n",
			pProgress->iPercent);
		return 0;
	}

	if (0 == dl_total)
	{
		return 0;
	}

	iNow_percent = (int)(100 * dl_now / dl_total);

	if (iNow_percent - iLast_percent >= pProgress->iPercent
		|| 100 == iNow_percent)
	{
		if (fseek(pProgress->pProgress, 0, SEEK_SET) < 0)
		{
			DEBUG("fseek fail, %s, errno=%d.\n", strerror(errno), errno);
			return 0;
		}

		fprintf(pProgress->pProgress, "%d%%", iNow_percent);

		if (fflush(pProgress->pProgress) < 0)
		{
			DEBUG("fflush fail, %s, errno=%d.\n", strerror(errno), errno);
			return 0;
		}

		iLast_percent = iNow_percent;
	}

	return 0;
}

static int update_progress_by_time(
				struct fota_progress_measure * pProgress,
				curl_off_t dl_total, curl_off_t dl_now,
				curl_off_t ul_total, curl_off_t ul_now)
{
	static time_t iLast = 0;
	time_t iNow = 0;

	RETURN_IF_NULL(pProgress, 0);
	RETURN_IF_NULL(pProgress->pProgress, 0);

	if (PROGRESS_TYPE_TIME != pProgress->iType)
	{
		DEBUG("Expect progress type %d, input progress type %d.\n",
			PROGRESS_TYPE_TIME, pProgress->iType);
		return 0;
	}

	if (pProgress->iInterval < 0)
	{
		DEBUG("Invalid time value: %d, must be greater than 0.\n",
			pProgress->iInterval);
		return 0;
	}

	if (0 == dl_total)
	{
		return 0;
	}

	iNow = time(NULL);
	if (0 == iLast || iNow - iLast >= pProgress->iInterval)
	{
		if (fseek(pProgress->pProgress, 0, SEEK_SET) < 0)
		{
			DEBUG("fseek fail, %s, errno=%d.\n", strerror(errno), errno);
			return 0;
		}

		fprintf(pProgress->pProgress,
			"%"CURL_FORMAT_CURL_OFF_T"%%",
			(100 * dl_now / dl_total));

		if (fflush(pProgress->pProgress) < 0)
		{
			DEBUG("fflush fail, %s, errno=%d.\n", strerror(errno), errno);
			return 0;
		}

		iLast = iNow;
	}
	else if (dl_now == dl_total)
	{
		if (fseek(pProgress->pProgress, 0, SEEK_SET) < 0)
		{
			DEBUG("fseek fail, %s, errno=%d.\n", strerror(errno), errno);
			return 0;
		}

		fprintf(pProgress->pProgress, "100%%");

		if (fflush(pProgress->pProgress) < 0)
		{
			DEBUG("fflush fail, %s, errno=%d.\n", strerror(errno), errno);
			return 0;
		}
	}

	return 0;
}

/* this is how the CURLOPT_XFERINFOFUNCTION callback works */
static int xferinfo(void * p,
				curl_off_t dl_total, curl_off_t dl_now,
				curl_off_t ul_total, curl_off_t ul_now)
{
	struct fota_progress_measure * pProgress;

	RETURN_IF_NULL(p, 0);

	pProgress = (struct fota_progress_measure * )p;

	if (PROGRESS_TYPE_PERCENT == pProgress->iType)
	{
		update_progress_by_percent(pProgress,
			dl_total, dl_now, ul_total, ul_now);
	}
	else if (PROGRESS_TYPE_TIME == pProgress->iType)
	{
		update_progress_by_time(pProgress,
			dl_total, dl_now, ul_total, ul_now);
	}
	else
	{
		DEBUG("Unsupported progress type: %d.\n", pProgress->iType);
	}

	return 0;
}

/* for libcurl older than 7.32.0 (CURLOPT_PROGRESSFUNCTION) */
static int older_progress(void * p,
				double dl_total, double dl_now,
				double ul_total, double ul_now)
{
	return xferinfo(p,
			(curl_off_t)dl_total,
			(curl_off_t)dl_now,
			(curl_off_t)ul_total,
			(curl_off_t)ul_now);
}

static int init_progress_module(CURL * pCurl, FILE * * pProgress)
{
	int iResult_code = -1;
	CURLcode res;
	struct fota_progress_measure progress;
	struct fota_progress_config * pProgress_config = NULL;

	RETURN_IF_NULL(pCurl, -1);
	RETURN_IF_NULL(pProgress, -1);

	pProgress_config = &fota_config_cmd.progress_config;
	if (!pProgress_config->pProgress_file)
	{
		return 0;
	}

	*pProgress = fopen(pProgress_config->pProgress_file, "w");
	if (!*pProgress)
	{
		DEBUG("fopen fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	memset(&progress, '\0', sizeof (progress));
	progress.iType = pProgress_config->iProgress_type;
	if (PROGRESS_TYPE_PERCENT == progress.iType)
	{
		progress.iPercent = pProgress_config->iProgress_percent;
	}
	else if (PROGRESS_TYPE_TIME == progress.iType)
	{
		progress.iInterval = pProgress_config->iProgress_time;
	}
	else
	{
		DEBUG("Unknown progress type: %d.\n", progress.iType);
		return -1;
	}
	progress.pProgress = *pProgress;

	res = curl_easy_setopt(pCurl, CURLOPT_PROGRESSFUNCTION,
		older_progress);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	res = curl_easy_setopt(pCurl, CURLOPT_PROGRESSDATA,
		(void *)&progress);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

#if LIBCURL_VERSION_NUM >= 0x072000

	res = curl_easy_setopt(pCurl, CURLOPT_XFERINFOFUNCTION,
		xferinfo);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	res = curl_easy_setopt(pCurl, CURLOPT_XFERINFODATA,
		(void *)&progress);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

#endif

	res = curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 0L);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	iResult_code = 0;

done:

	return iResult_code;
}

int send_download_firmware_request_manual(
						struct fota_fw_info * pFw,
						struct http_chunk * pHttp_header,
						struct http_chunk * pHttp_data)
{
	int iResult_code = -1;
	CURL * pCurl = NULL;
	CURLcode res;
	FILE * fp = NULL;
	FILE * fp_progress = NULL;

	RETURN_IF_NULL(pFw, -1);
	RETURN_IF_NULL(pFw->pUrl, -1);
	RETURN_IF_NULL(pHttp_header, -1);
	RETURN_IF_NULL(pHttp_data, -1);
	RETURN_IF_NULL(fota_config_cmd.pCacert, -1);
	RETURN_IF_NULL(fota_config_cmd.pFw_path, -1);

	fp = fopen(fota_config_cmd.pFw_path, "wb");
	if (!fp)
	{
		DEBUG("fopen fail, %s, errno=%d.\n", strerror(errno), errno);
		goto done;
	}

	memset(pHttp_header, '\0', sizeof (struct http_chunk));
	memset(pHttp_data, '\0', sizeof (struct http_chunk));

	pHttp_header->iType = CHUNK_TYPE_MEM;
	pHttp_data->iType = CHUNK_TYPE_FILE;
	pHttp_data->pData = (void *)fp;

	res = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (res != CURLE_OK)
	{
		DEBUG("curl_global_init fail.\n");
		goto done;
	}

	pCurl = curl_easy_init();
	GOTO_IF_NULL(pCurl, done);

	res = curl_easy_setopt(pCurl, CURLOPT_URL, pFw->pUrl);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	res = curl_easy_setopt(pCurl, CURLOPT_CAINFO,
		fota_config_cmd.pCacert);
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
	res = curl_easy_setopt(pCurl, CURLOPT_TIMEOUT,
		170L);
	GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);

	if (fota_config_cmd.iVerbose)
	{
		curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);
		GOTO_IF_NOT_EQUAL(res, CURLE_OK, done);
	}

	if (init_progress_module(pCurl, &fp_progress) < 0)
	{
		DEBUG("init_progress_module fail.\n");
		goto done;
	}

	res = curl_easy_perform(pCurl);
	if (res != CURLE_OK)
	{
		DEBUG("curl_easy_perform() failed: %s.\n",
			curl_easy_strerror(res));
		goto done;
	}

	if (ferror(fp))
	{
		DEBUG("Write %s encountered problems, %s, errno=%d.\n",
			fota_config_cmd.pFw_path, strerror(errno), errno);
		goto done;
	}

	if (fp_progress && ferror(fp_progress))
	{
		DEBUG("Write %s encountered problems, %s, errno=%d.\n",
			fota_config_cmd.progress_config.pProgress_file,
			strerror(errno), errno);
		goto done;
	}

	iResult_code = 0;

done:

	if (fp && fclose(fp) < 0)
	{
		DEBUG("fclose fail, %s, errno=%d.\n", strerror(errno), errno);
	}

	if (fp_progress && fclose(fp_progress) < 0)
	{
		DEBUG("fclose fail, %s, errno=%d.\n", strerror(errno), errno);
	}

	curl_easy_cleanup(pCurl);
	curl_global_cleanup();

	return iResult_code;
}

