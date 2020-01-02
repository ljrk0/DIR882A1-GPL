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
 * Author : Eswar Yaganti
 */


#include <arpa/inet.h> /* for in_addr* types */
#include <stdlib.h>
#include "shgw_logger.h"
#include "mfeipset.h"
#include "shgw_common.h"

#ifndef SHGW_IPSET_H
#define SHGW_IPSET_H

#define WHITELIST_SET				"SHGW_WHITELIST"
#define WHITELIST_SET6				"SHGW6_WHITELIST"
#define NETWORK_PAUSE_SET			"SHGW_NETWORK_PAUSE"
#define NETWORK_PAUSE_SET6			"SHGW6_NETWORK_PAUSE"
#define DEVICE_BLOCK_SET			"SHGW_DEVICE_BLOCK"
#define DEVICE_BLOCK_SET6			"SHGW6_DEVICE_BLOCK"
#define EULA_BLOCK_SET				"SHGW_EULA_BLOCK"
#define EULA_BLOCK_SET6				"SHGW6_EULA_BLOCK"
#define HOST_REPUTATION_SET			"SHGW_HOST_REPUTATION"
#define HOST_REPUTATION_DST_SET		"SHGW_HOST_REPUTATION_DST"
#define HOST_REPUTATION_SET6 		"SHGW6_HOST_REPUTATION"

#define SHP_WAN_ALLOW_IPSET			"SHP_WAN_ALLOW_SET"
#define SHP_MGT_CONSOLE_IPSET		"SHP_MGT_CONSOLE_SET_A"
#define SHP_WAN_ALLOW_IPSET6		"SHP_WAN_ALLOW_SET_IP6"
#define SHP_MGT_CONSOLE_IPSET6		"SHP_MGT_CONSOLE_SET_A_IP6"

#define	_SRC							"src"
#define	_DST							"dst"
#define _SRC_DST						"src,dst"
#define _DST_DST						"dst,dst"
#define _SRC_DST_DST					"src,dst,dst"
#define WHITELIST_SET_TYPE				_DST
#define NETWORK_PAUSE_SET_TYPE			_SRC
#define DEVICE_BLOCK_SET_TYPE			_SRC
#define EULA_BLOCK_SET_TYPE				_SRC_DST
#define HOST_REPUTATION_SET_TYPE		_SRC_DST_DST


STATUS_CODE shgw_ipset_wd_init();
void shgw_ipset_wd_deinit();

STATUS_CODE shgw_ipset_dnsproxy_init();
void shgw_ipset_dnsproxy_deinit();

STATUS_CODE shgw_ipset_set_add(const char *set, uint32_t timeout, struct in_addr *ip, uint16_t port,  struct in_addr *ip2);
STATUS_CODE shgw_ipset_set_delete(const char *name, struct in_addr *ip, uint16_t port, struct in_addr *ip2);

STATUS_CODE shgw_ipset_set_add6(const char *set, uint32_t timeout, struct in6_addr *ip, uint16_t port, struct in6_addr *ip2);
STATUS_CODE shgw_ipset_set_delete6(const char *set, struct in6_addr *ip, uint16_t port, struct in6_addr *ip2);

#ifdef UNIT_TEST

PRIVATE void *handle; /* No one needs to know this except me */

#endif

#endif /* SHGW_IPSET_H */
