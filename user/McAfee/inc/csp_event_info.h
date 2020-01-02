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
 * CSP telemetry JSON structures.
 * */

#include "shgw_json.h"
#include "error_codes.h"

/*
{
    "header": {
        "clientid": "3f7d6cbd3d9f4537b95bb91158ae5e29-95353f7c5010",
        "collection_log_size": "0",
        "timestamp": "1450349438",
        "device_type": "pc",
        "event_log_size": "0",
        "device_os": "Windows",
        "app_id": "152c3290-c515-11e3-be43-ef8523d0c858",
        "affid": "0",
        "culture": "en-US",
        "hw_id": "b6b4717ad7d42494b7a1f6460623de12",
        "sw_id": "{03E46DC8-D8EC-4583-9E77-9CC97B4E9FF5}"
    },
    "event_list": [
        {
            "applicationid": "152c3290-c515-11e3-be43-ef8523d0c858",
            "eventtype": "exception",
            "timestamp": "1382942378",
            "emailid": "xyz@macfee.com",
            "profileid": "88888888",
            "profileuid": "fffd03db-49fa-4f6a-b2f8-6c4aed144200",
            "groupid": "123456",
            "event_data": {
                "message": "Objet Ref Doesnot Set",
                "component": "$$$$$$$$$$$$$",
                "methodname": "ExceptionTestMethod",
                "priority": "1",
                "severity": "Information",
                "additionalinfo": "key1=value1&key2=value2",
                "appdomainname": "McCSPClientDemo.apk",
                "applicationname": "McCSPClientDemo",
                "callduration": "123",
                "callingapplication": "YAP.Face.Recog",
                "createdby": "TestLogger",
                "datacenter": "TestDataCenter",
                "errorid": "900875",
                "logreferenceid": "1905363",
                "logtype": "Log",
                "machinename": "MyMachine",
                "requestcontext": "Some Request Context data",
                "responsecontext": "Response Context data",
                "stacktrace": "error in line 30",
                "statedata": "SateData",
                "url": "http://mcafee.com"
            }
        }
    ]
}
 * */

typedef struct _csp_event_header_t
{
	char* clientid;
	char* collection_log_size;
	char* timestamp;
	char* device_type;
	char* event_log_size;
	char* device_os;
	char* app_id;
	char* affid;
	char* culture;
	char* hw_id;
	char* sw_id;
} csp_event_header_t;

typedef struct _csp_event_data_t
{
	char* message;
	char* component;
	char* methodname;
#if 0
	char* priority;
	char* severity;
	char* additionalinfo;
	char* appdomainname;
	char* applicationname;
	char* callduration;
	char* callingapplication;
	char* createdby;
	char* datacenter;
	char* errorid;
	char* logreferenceid;
	char* logtype;
	char* machinename;
	char* requestcontext;
	char* responsecontext;
	char* stacktrace;
	char* statedata;
	char* url;
#endif
} csp_event_data_t;

typedef struct _csp_event_t
{
	char* applicationid;
	char* eventtype;
	char* timestamp;
#if 0
	char* emailid;
	char* profileid;
	char* profileuid;
	char* groupid;
#endif
	csp_event_data_t event_data;
} csp_event_t;

typedef struct _csp_event_info_t
{
	csp_event_header_t header;
	int nevents;
	csp_event_t* event_list;
} csp_event_info_t;

/* type prefixed functions,
 * prefix signifies they act only on prefixed type
 * sort of instance methods in C++
 * */

STATUS_CODE csp_event_header_tojson (csp_event_header_t* this_ptr, shgw_json_node** proot);
STATUS_CODE csp_event_header_isvalid(csp_event_header_t* this_ptr);
void        csp_event_header_free   (csp_event_header_t* this_ptr);

STATUS_CODE csp_event_data_tojson (csp_event_data_t* this_ptr, shgw_json_node** proot);
STATUS_CODE csp_event_data_isvalid(csp_event_data_t* this_ptr);
void        csp_event_data_free   (csp_event_data_t* this_ptr);

STATUS_CODE csp_event_tojson (csp_event_t* this_ptr, shgw_json_node** proot);
STATUS_CODE csp_event_isvalid(csp_event_t* this_ptr);
void        csp_event_free   (csp_event_t* this_ptr);

STATUS_CODE csp_event_info_tojson(csp_event_info_t* this_ptr, shgw_json_node** proot);
void        csp_event_info_free  (csp_event_info_t* this_ptr);
