/*****************************************************************************
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

#ifndef __WRAPPER_H_
#define __WRAPPER_H_

#include <string.h>
#include <signal.h>				/* Used for signal handling */
#include <sys/epoll.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include "openssl/sha.h"
#include "curl/curl.h"
#include <stdlib.h>
#include "sqlite3.h"
#include "error_codes.h"
#include "shgw_query_sqlite.h"
#include "router_registration_store.h"
#include "shgw_json.h"
#include "shgw_http_request.h"
#include "router_registration.h"

/* Mocking log functions */
void __wrap_shgw_critical(int id, int confidence_level, const char* fmt, ...);
void __wrap_shgw_info(int id, int confidence_level, const char* fmt, ...);
void __wrap_shgw_debug(int id, int confidence_level, const char* fmt, ...);
void __wrap_shgw_warning(int id, int confidence_level, const char* fmt, ...);
void __wrap_shgw_error(int id, int confidence_level, const char* fmt, ...);

/* Mocking functions corresponding to file operations */
char *__wrap_fopen(const char *filename, const char *mode);
char *__wrap_fgets(char *str, int n, FILE *stream);


/*Mocking curl related functions */
CURL *__wrap_curl_easy_init();
struct curl_slist *__wrap_curl_slist_append(struct curl_slist *list,
		const char *string);
CURLcode __wrap_curl_easy_setopt(CURL *handle, CURLoption option,
		void *parameter);
void __wrap_curl_easy_cleanup(CURL *easy_handle);
void __wrap_curl_slist_free_all(CURL *easy_handle);
CURLcode __wrap_curl_easy_perform(CURL *easy_handle);
CURLcode __wrap_curl_easy_getinfo(CURL *easy_handle, CURLINFO info,
		void *parameter);
CURL_EXTERN CURLcode __wrap_curl_global_init(long flags);
CURL_EXTERN const char *__wrap_curl_easy_strerror(CURLcode);
CURL_EXTERN void __wrap_curl_global_cleanup(void);

/* Mocking shgw utils functions */
STATUS_CODE __wrap_shgw_http_init(const char* cert_path);
STATUS_CODE __wrap_shgw_http_POST(const char *url, const char *req_data, struct curl_slist* headers, int* http_status_code, char** resp_data);
void __wrap_shgw_http_deinit();
STATUS_CODE __wrap_shgw_digest_sha256(const char *indata, int indata_size, unsigned char** outdata);
void __wrap_shgw_get_secret_key_bits(int **index_arr, int *arr_len);
STATUS_CODE __wrap_shgw_calc_partner_secret(char **partner_secret, char **nonce);
char* __wrap_shgw_calc_hashed_mac(const char* mac_id);


/* Mocking sqlite related functions */
int __wrap_sqlite3_open(const char *filename, sqlite3 **ppDb);
int __wrap_sqlite3_exec(sqlite3* db, const char *sql,
		int (*callback)(void* data, int argc, char** argv, char** azColName),
		void *callback_data, char **errmsg);
const char *__wrap_sqlite3_errmsg(sqlite3* db);
int __wrap_sqlite3_close(sqlite3* db);
void __wrap_sqlite3_free(void* data);
char *__wrap_sqlite3_mprintf(const char* data, ...);
STATUS_CODE __wrap_shgw_query_sqlite(const char* db_path, char* query, query_type type, shgw_sqlite_cb_t callback, char** cb_data);

STATUS_CODE __wrap_router_registration_store_init(const char *filename, router_registration_data_t *pdata);
int __wrap_shgw_http_async_init(const char *cert_path);
char* __wrap_shgw_json_object_get_string(shgw_json_node* jnode, const char* key);
char* __wrap_shgw_json_object_get_str_duplicate(shgw_json_node * resp_data, const char *key);
char* __wrap_shgw_json_parse_string(char* data);
int __wrap_shgw_json_object_get_int(shgw_json_node* jnode, const char* key);
char* __wrap_shgw_json_object_create();
char* __wrap_shgw_json_print_duplicated(shgw_json_node * json_req);
void __wrap_shgw_json_object_insert_string(shgw_json_node* jnode, const char* name, const char* value);
void __wrap_shgw_json_free(shgw_json_node* jnode);
int __wrap_shgw_http_async_POST(const char *url, struct curl_slist* headers,
		char *req_data, SHGW_HTTP_RESPOPT resp_opt, shgw_async_http_cb_t user_cb,
		void* userp);
char* __wrap_strdup(const char *str);
void __wrap_shgw_http_async_deinit();
int __wrap_router_registration_store_deinit(router_registration_data_t* pdata);
int __wrap_shgw_get_access_token_expiry(char **expiry);
int __wrap_shgw_get_router_id(char **id);
struct event* __wrap_event_new(struct event_base*, evutil_socket_t, short, event_callback_fn, void *);
int __wrap_event_add(struct event *, const struct timeval *);
void __wrap_event_base_dispatch(struct event_base*);
void __wrap_event_free(struct event*);
STATUS_CODE __wrap_shgw_get_refresh_token(char **refresh_token);
STATUS_CODE __wrap_form_and_send_rr_request_to_cloud(char *data, char *param_name, char *url);
STATUS_CODE __wrap_do_rr(RR_RUN_MODE run_mode);
struct event * __wrap_add_timer(struct event_base* _base_event, short flags, struct timeval *timeout, callback_func _cb, void *data);
long __wrap_atol(const char *nptr);
//STATUS_CODE __wrap_prepare_request_data(char *data, char *param_name, char **request_data);
//char * __wrap_get_csp_clientid_from_db();

#endif
