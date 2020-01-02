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

#pragma once

#include <json-c/json.h>

#define SHGW_JSON_TYPE		json_type
#define SHGW_JSON_NULL		json_type_null
#define SHGW_JSON_BOOLEAN	json_type_boolean
#define SHGW_JSON_DOUBLE	json_type_double
#define SHGW_JSON_INT		json_type_int
#define SHGW_JSON_OBJECT	json_type_object
#define SHGW_JSON_ARRAY		json_type_array
#define SHGW_JSON_STRING	json_type_string

typedef struct json_object shgw_json_node;

/* parsing routines */
shgw_json_node* shgw_json_parse_file(const char* filename);
shgw_json_node* shgw_json_parse_string(const char* str);
int shgw_json_free(shgw_json_node* jnode);
SHGW_JSON_TYPE shgw_json_type(shgw_json_node* jnode);

/* iterator object */
#define shgw_json_object_foreach(jnode, key, value) 	json_object_object_foreach(jnode, key, value)

/* common methods, sensible only for string, array or object */
int shgw_json_length(shgw_json_node* jnode);
const char* shgw_json_print(shgw_json_node* jnode);
char *shgw_json_print_duplicated(shgw_json_node* node);
const char* shgw_json_get_string(shgw_json_node* node);
char* shgw_json_get_string_duplicate(shgw_json_node* node);

/* object routines */
shgw_json_node* shgw_json_object_create();
int shgw_json_object_insert_object(shgw_json_node* jnode, const char* name, shgw_json_node* value);
int shgw_json_object_insert_int(shgw_json_node* jnode, const char* name, int value);
int shgw_json_object_insert_double(shgw_json_node* jnode, const char* name, double value);
int shgw_json_object_insert_long(shgw_json_node* jnode, const char* name, long value);
int shgw_json_object_insert_string(shgw_json_node* jnode, const char* name, const char* value);
shgw_json_node* shgw_json_object_get_object(shgw_json_node* jnode, const char* key);
int shgw_json_object_get_int(shgw_json_node* jnode, const char* key);
int shgw_json_object_get_value_int(shgw_json_node* jnode);
const char* shgw_json_object_get_string(shgw_json_node* jnode, const char* key);

/* array routines */
shgw_json_node* shgw_json_array_create();
int shgw_json_array_insert_object(shgw_json_node* jnode, shgw_json_node* value);
int shgw_json_array_insert_int(shgw_json_node* jnode, int value);
int shgw_json_array_insert_string(shgw_json_node* jnode, const char* value);

/* indexed array access */
int shgw_json_array_set_at(shgw_json_node* jnode, int idx, shgw_json_node* value);
shgw_json_node* shgw_json_array_get_at(shgw_json_node* jnode, int idx);

char* shgw_json_object_get_str_duplicate(shgw_json_node * resp_data, const char *key);
