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

#include "error_codes.h"
#include <sys/socket.h>			/* Provides socket related functionality */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include "shgw_http_request.h"
#include "shgw_query_sqlite.h"
#include <mfeipset.h>
#include "shgw_json.h"

int __wrap_socket(int domain, int type, int protocol);

int __wrap_connect(int socket, const struct sockaddr *address,
	       socklen_t address_len);

int __wrap_bind(int socket, const struct sockaddr *address,
	       socklen_t address_len);

int __wrap_listen(int socket, int backlog);

int __wrap_accept(int socket, struct sockaddr *address,
	       socklen_t *address_len);

void* __wrap_shgw_malloc(size_t size);

struct curl_slist *__wrap_curl_slist_append(struct curl_slist *list,
						const char *string);

int __wrap_curl_global_init(long flags);

CURL *__wrap_curl_easy_init();

CURLcode __wrap_curl_easy_setopt(CURL *handle, CURLoption option,
					void *parameter);

CURLcode __wrap_curl_easy_getinfo(CURL *easy_handle, CURLINFO info,
					void **parameter);

void __wrap_curl_easy_cleanup(CURL *easy_handle);

void __wrap_shgw_http_deinit(CURL *easy_handle);

void* __wrap_shgw_realloc(void *ptr, size_t size);

int __wrap_shgw_warning(int id, int confidence_level, const char* fmt, ...);

int __wrap_event_free();
//void __wrap_event_free(struct event* ev);

int __wrap_curl_multi_add_handle();
void* __wrap_shgw_calloc(size_t, size_t);
CURLMcode __wrap_curl_multi_socket_action(CURLM *multi_handle, curl_socket_t s,
                                   int ev_bitmask, int *running_handles);

CURLMsg * __wrap_curl_multi_info_read(CURLM *multi_handle, int *msgs_in_queue);
CURLMcode __wrap_curl_multi_assign(CURLM *, curl_socket_t, void *);

ssize_t __wrap_send(int sockfd, const void *buf, size_t len, int flags);
STATUS_CODE __wrap_shgw_get_router_id(char **router_id);
STATUS_CODE __wrap_shgw_query_sqlite(const char* db_path, char* query, query_type type,
        shgw_sqlite_cb_t callback, char **cb_data);

char * __wrap_strdup(char *dups);
//size_t __wrap_strlen(char *s);

//int __wrap_fnctl(int fd, int cmd, ...);

//int __wrap_open(const char *pathname, int flags, mode_t mode);

ssize_t __wrap_read(int fd, void *buf, size_t count);
void* __wrap_mfeipset_open();
void __wrap_mfeipset_close(void *handle);
int __wrap_mfeipset_set_create(void *handle, uint8_t family, const char *name, mfeipset_set_family type, uint32_t timeout);
int __wrap_mfeipset_set_delete6(void *handle, const char *name, struct in6_addr *ip, uint16_t port, struct in6_addr *ip2);
int __wrap_mfeipset_set_add6(void *handle, const char *name, uint32_t timeout, struct in6_addr *ip, uint16_t port, struct in6_addr *ip2);
int __wrap_mfeipset_set_add(void *handle, const char *name, uint32_t timeout, struct in_addr *ip, uint16_t port,  struct in_addr *ip2);
int __wrap_mfeipset_set_delete(void *handle, const char *name, struct in_addr *ip, uint16_t port, struct in_addr *ip2);
int __wrap_mfeipset_set_destroy(void *handle, const char *name);

shgw_json_node* __wrap_shgw_json_parse_string(const char* str);
SHGW_JSON_TYPE __wrap_shgw_json_type(shgw_json_node* jnode);
int __wrap_shgw_json_length(shgw_json_node* jnode);
shgw_json_node* __wrap_shgw_json_array_get_at(shgw_json_node* jnode, int idx);
int __wrap_shgw_json_free(shgw_json_node* jnode);
STATUS_CODE __wrap_shgw_get_secret_key_bits(int **index_arr, int *arr_len);
void __wrap_curl_slist_free_all(struct curl_slist *list);
STATUS_CODE __wrap_shgw_get_access_token(char **access_token_t);

