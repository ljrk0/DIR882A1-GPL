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

#ifndef GENERATE_ROUTER_ID_H
#define GENERATE_ROUTER_ID_H

#include "openssl/sha.h"
#include "sqlite_wrapper.h"
#include "shgw_utils.h"
#include "shgw_digest.h"

#define MAC_ADDRESS_SIZE 				12
#define NIC_IDENTIFIER_LENGTH 			6
#define MAC_LENGTH_WITH_COLONS			18

STATUS_CODE get_router_mac(char *mac_id);

void get_mac_without_colons(char *mac_id, char *mac_without_colons);

STATUS_CODE get_router_id(char *mac_id, char **router_id);

#endif
