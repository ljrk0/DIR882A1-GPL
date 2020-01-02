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

#ifndef SHGW_RPC_SERVER
#define SHGW_RPC_SERVER

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <event2/event.h>
#include "shgw_common.h"
#include "shgw_utils.h"
#include "shgw_logger.h"
#include "error_codes.h"

#define SHGW_UNIX_PATH_MAX 108

/*
 * Struct Members
 * sockfd - fd returned by socket()
 */

/* FIXME Can we use a more specific typedef struct name ?? */
typedef struct _context
{
	int sockfd;
	struct event* accept_watcher;
	struct event_base* base_event;
	STATUS_CODE (*callback) (int, int);
}g_context_t;


/*
 * This function creates the listen socket for communication with server.
 */
#ifdef UNIT_TEST
PRIVATE STATUS_CODE shgw_uds_server_init(int *sockfd, struct sockaddr_un *addr, const char *filename);
#endif

/*
 * This is the interface for the modules. The filename is the path of the file for UDS.
 * callback function is the final function that has to be called when data is received
 * from the client.
 */
STATUS_CODE shgw_uds_server_init_reg_event(struct event_base* base_event,
	const char *filename, STATUS_CODE (callback)(int, int));

/*
 * This callback is called by the libev watcher when a connection is accepted.
 * Registers for the callback with the new fd for data read from client.
 */
void shgw_accept_cb(evutil_socket_t fd, short event, void *arg);

/*
 * Registers for the callback with the new fd for data read from client.
 */
STATUS_CODE register_to_recv_cmd_data(int client_fd);

/*Is the function that calls the passed function pointer. Unpacks data */
void shgw_rpc_stub_cb(evutil_socket_t fd, short event, void *arg);

STATUS_CODE shgw_rpc_read_cmd_data(int client_fd, int* data_size, void **cmd_data);

/* Writes back the status code of the RPC back to client */
STATUS_CODE write_rpc_cmd_status(int sockfd, int status);

void clear_rpc_resources();

#endif
