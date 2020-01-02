/****************************************************************************
 *
 * INTEL CONFIDENTIAL
 * Copyright (c) 2016 Intel Corporation All Rights Reserved
 *
 * The source code contained or described herein and all documents related
 * to the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors. Title to the Material remains with Intel
 * Corporation or its suppliers and licensors. The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 *
****************************************************************************/

/* 
 * shgw_http_request.h
 */

#pragma once

#include <curl/curl.h>
#include <event2/event.h>
#include <sys/stat.h>
#include "shgw_common.h"

#include "error_codes.h"

STATUS_CODE shgw_http_init(const char* cert_path);
void shgw_http_deinit();

/* except url, everything else can be NULL */
STATUS_CODE shgw_http_GET   (const char *url, const char *req_data, struct curl_slist* headers, int* status_code, char** resp_data);
STATUS_CODE shgw_http_POST  (const char *url, const char *req_data, struct curl_slist* headers, int* status_code, char** resp_data);
STATUS_CODE shgw_http_DELETE(const char *url, const char *req_data, struct curl_slist* headers, int* status_code, char** resp_data);

STATUS_CODE basic_http_request(const char* verb, const char *url, const char *req_data, struct curl_slist* headers,
		int* http_status_code, char** resp_data);

/*
 * Asynchronous HTTP
 * */

/* occasionaly only HTTP response code is of interest and not server response body.
 * In such case, NO_ALLOC will avoid memory allocation but will invoke user
 * callback with size set to -1 and response buffer to NULL.
 * */
typedef enum __SHGW_HTTP_RESPOPT {
	SHGW_HTTP_RESPOPT_ALLOC			= 	(1 << 0),
	SHGW_HTTP_RESPOPT_NO_ALLOC		= 	(1 << 1),
	SHGW_RR_HTTP_RESPOPT_ALLOC		= 	(1 << 2),
	SHGW_HTTP_RESPOPT_HDR_ALLOC		= 	(1 << 3),
	SHGW_HTTP_RESPOPT_FILE_UPLOAD		=	(1 << 4),
	SHGW_HTTP_RESPOPT_ERRORCODE_UPLOAD	=	(1 << 5),
} SHGW_HTTP_RESPOPT;

/* type of callback expected from users willing to receive data asynchronously */
typedef void (*shgw_async_http_cb_t)(int http_status_code, int resp_size, char* resp_data, void* userp, char *header_data);

STATUS_CODE shgw_http_async_init(struct event_base* base_event, const char* cert_path);
void shgw_http_async_deinit();

STATUS_CODE shgw_http_async_GET   (const char *url, struct curl_slist* headers, char *req_data,
		SHGW_HTTP_RESPOPT resp_opt, shgw_async_http_cb_t user_cb, void* userp);
STATUS_CODE shgw_http_async_POST  (const char *url, struct curl_slist* headers, char *req_data,
		SHGW_HTTP_RESPOPT resp_opt, shgw_async_http_cb_t user_cb, void* userp);
STATUS_CODE shgw_http_async_DELETE(const char *url, struct curl_slist* headers, char *req_data,
		SHGW_HTTP_RESPOPT resp_opt, shgw_async_http_cb_t user_cb, void* userp);
STATUS_CODE shgw_http_async_UPLOAD(const char *url, struct curl_slist* headers,
	char *req_data, SHGW_HTTP_RESPOPT resp_opt, shgw_async_http_cb_t user_cb,
	void* userp);

STATUS_CODE shgw_http_parse_header(char *header_data, char **data_buffer, const char *field);


#ifdef UNIT_TEST
PRIVATE STATUS_CODE _do_http_async(const char* verb, const char *url,
	struct curl_slist* headers, char *req_data, SHGW_HTTP_RESPOPT resp_opt,
	shgw_async_http_cb_t user_cb, void* userp);

PRIVATE STATUS_CODE _setup_easy_curl(const char* verb, const char* url,
		char *req_data, const char* cert_path, struct curl_slist* headers,
		SHGW_HTTP_RESPOPT resp_opt, shgw_async_http_cb_t user_cb, void* userp,
		CURL** phcurl);
PRIVATE void _ev_timeout_cb(evutil_socket_t fd, short event, void *arg);
PRIVATE void _curl_timeout_cb(CURLM *multi, long timeout_ms, void *userp);
PRIVATE void _on_curl_perform_cb(evutil_socket_t fd, short event, void* arg);
PRIVATE size_t _curl_write_cb(void *data, size_t size, size_t nmemb,void *userdata);
PRIVATE void _read_curl_socket(CURLM* hmcurl);
PRIVATE int _curl_socket_activity_cb(CURL *easy, curl_socket_t sockfd,
        int action, void *userp, void *socketp);
PRIVATE size_t _curl_hdr_cb(void *data, size_t size, size_t nmemb, void *userdata);
PRIVATE size_t _curl_cb(void *data, size_t size, size_t nmemb, void *userdata);

PRIVATE STATUS_CODE _shgw_http_request(const char* verb, const char *url,
        const char *req_data, struct curl_slist* headers, int* http_status_code,
        char** resp_data);
#endif





