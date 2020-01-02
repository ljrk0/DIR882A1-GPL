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

#ifndef SHGW_RPC_CLIENT
#define SHGW_RPC_CLIENT



#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <event2/event.h>
#include <curl/curl.h>

#include "shgw_common.h"
#include "error_codes.h"
#include "shgw_utils.h"
#include "shgw_logger.h"

#define SHGW_UNIX_PATH_MAX 108

/* max wait time in seconds for RPC to return status */
#define	SHGW_RPC_MAX_WAIT 	5

typedef void (*shgw_rpc_status_cb_t)(STATUS_CODE status, void* arg);

/*
 * This function creates a UDS as a client and transmits the cmd_data to the
 * path specified by sock_path. fid specifies the task to be performed at the
 * server end.
 * If return code is SUCCESS, callback is guaranteed to be invoked.
 * */
STATUS_CODE shgw_send_data_to_peer(
	char *sock_path,
	int fid, int data_size, const void *cmd_data,
	struct event_base* base_event,
	shgw_rpc_status_cb_t status_cb, void* cb_arg);

STATUS_CODE get_rpc_return_status(int sockfd,
	struct event_base* base_event,
	shgw_rpc_status_cb_t status_cb, void* cb_arg);


#ifdef UNIT_TEST

PRIVATE STATUS_CODE shgw_uds_init(int *sockfd, char *sock_path);

#endif

#endif

