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

/*
 * Header file that defines common shgw utils
 *
 */

#ifndef SHGW_UTILS_H
#define SHGW_UTILS_H

#include <stdlib.h>
#include "error_codes.h"
#include <event2/event.h>
#include "shgw_common.h"

#define     SHGW_FREE       free

#ifndef SHGW_PRODUCTION_RELEASE
#define     SHGW_MALLOC         shgw_malloc
#define     SHGW_CALLOC         shgw_calloc
#define     SHGW_REALLOC        shgw_realloc
#else
#define     SHGW_MALLOC         malloc
#define     SHGW_CALLOC         calloc
#define     SHGW_REALLOC        realloc
#endif

#define SHGW_MAC_ADDRESS_LEN 				12
#define SHGW_NIC_IDENTIFIER_LENGTH 			6
#define SHGW_MAC_LENGTH_WITH_COLONS			18
#define ROUTER_ID_LENGTH					74
#define DEVICE_ID_LENGTH					74

#ifndef SHGW_PRODUCTION_RELEASE
void *shgw_malloc(size_t size);
void shgw_free(void *ptr);
void *shgw_calloc(size_t nelem, size_t elsize);
void *shgw_realloc(void *ptr, size_t size);
#endif

typedef void (*callback_func)(evutil_socket_t , short , void* );

#ifdef UNIT_TEST

PRIVATE int sqlite_callback(void *data, int argc, char **argv, char **azColName);

#endif

STATUS_CODE shgw_get_time_since_epoch(long *epoch_time);
double shgw_get_precision_time();
STATUS_CODE shgw_check_version(const char* version);
STATUS_CODE shgw_check_ip_for_all_zeros(unsigned char *ip);
STATUS_CODE retrieve_IP_of_local_device(const char* dev, unsigned char *ip);
STATUS_CODE retrieve_MAC_of_local_device(const char* dev, unsigned char* mac);
char* shgw_calc_hashed_mac(const char* mac_id);
void shgw_to_upper(char* str);
struct event *add_timer(struct event_base* _base_event, short flags, struct timeval* edns_cm_ping_interval, callback_func, void *data);
void shgw_tolower(char* str);
char* __get_mac_without_colon(const char* mac_id);
char* shgw_calc_hashed_mac(const char* mac_id);
char *get_csp_clientid_from_db();
STATUS_CODE on_shp_url_change();
#endif /* SHGW_UTILS_H */








