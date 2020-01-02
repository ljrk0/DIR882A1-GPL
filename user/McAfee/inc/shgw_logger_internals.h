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
 * Author : Eswar
 */

#ifndef LOGGER_LOGGER_PRIVATE_H
#define LOGGER_LOGGER_PRIVATE_H

#define FILE_OPEN_MODE       "a"

#define MAX_LOG_LEVEL_NAME_LENGTH          10
#define MAX_CONFIDENCE_LEVEL_NAME_LENGTH   10

#define CONF_HIGH            "H"
#define CONF_MED             "M"
#define CONF_LOW             "L"

#define LOG_CRITICAL         "C"
#define LOG_ERROR            "E"
#define LOG_WARNING          "W"
#define LOG_INFO             "I"
#define LOG_DEBUG            "D"

#define MAX_MODULE_NAME_SIZE        "10"
#define TIME_FORMAT                 "\n%ld "
#define MODULE_DESC                 "%." MAX_MODULE_NAME_SIZE "s %d "
#define LOG_DESCRIPTION             "%.8s %s %d "

#define PREFIX                      TIME_FORMAT MODULE_DESC LOG_DESCRIPTION
#define FORMAT_STRING_MAX_LENGTH    140

#endif //LOGGER_LOGGER_PRIVATE_H
