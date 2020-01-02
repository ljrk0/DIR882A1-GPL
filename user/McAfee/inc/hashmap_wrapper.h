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

#ifndef HASHMAP_WRAPPER_H
#define HASHMAP_WRAPPER_H

#include <json-c/linkhash.h>
#include "error_codes.h"

#define hm_foreach(map, entry, tmp) lh_foreach_safe(map->hashmap, entry, tmp)

typedef struct string_hashmap_wrap{
	lh_table *hashmap;
}strmap_t;

typedef struct ptr_hashmap_wrap{
	lh_table *hashmap;
}ptrmap_t;

typedef struct lh_entry hm_entry_t;

typedef void (hashmap_entry_free_fn)(hm_entry_t *entry);

void *hm_get_key_from_map_entry(hm_entry_t *entry);
void *hm_get_value_from_map_entry(hm_entry_t *entry);

ptrmap_t *hm_init_ptrmap(int size, hashmap_entry_free_fn *entry_free_fn);
STATUS_CODE hm_add_to_ptrmap(ptrmap_t *map,void *key,const void *value);
void *hm_get_from_ptrmap(ptrmap_t *map,const void *key);
STATUS_CODE hm_delete_from_ptrmap(ptrmap_t *map,const void *key);
void hm_destroy_ptrmap(ptrmap_t *map);
int hm_get_ptrmap_size(ptrmap_t *map);
int hm_get_ptrmap_count(ptrmap_t *map);

strmap_t *hm_init_strmap(int size, hashmap_entry_free_fn *entry_free_fn);
STATUS_CODE hm_add_to_strmap(strmap_t *map,char *key,const void *value);
STATUS_CODE hm_add_to_strmap_safe(strmap_t *map, char *key, const void *value);
void *hm_get_from_strmap(strmap_t *map,const char *key);
hm_entry_t* hm_get_entry_from_strmap(strmap_t *map, const char *key);
STATUS_CODE hm_delete_from_strmap(strmap_t *map,const char *key);
void hm_destroy_strmap(strmap_t *map);
int hm_get_strmap_size(strmap_t *map);
int hm_get_strmap_count(strmap_t *map);

#endif //HASHMAP_WRAPPER_H
