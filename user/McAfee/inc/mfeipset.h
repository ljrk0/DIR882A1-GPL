#pragma once
#include <arpa/inet.h>

/*
 * Author : Eswar Yaganti
 */

typedef enum mfeipset_set_family {
  MFEIPSET_HASH_IP,
  MFEIPSET_HASH_IP_PORT,
  MFEIPSET_HASH_IP_PORT_IP,
  MFEIPSET_HASH_NET,
  MFEIPSET_HASH_NET_NET
} mfeipset_set_family;

void *mfeipset_open();

int mfeipset_set_create(void *handle, const char *name, enum mfeipset_set_family family, uint32_t timeout);
int mfeipset_set_add(void *handle, const char *name, uint32_t timeout, struct in_addr *ip, uint16_t port, struct in_addr *ip2);
int mfeipset_set_delete(void *handle, const char *name, struct in_addr *ip, uint16_t port, struct in_addr *ip2);
int mfeipset_set_net_add(void *handle, const char *name, uint32_t timeout, struct in_addr *ip, uint8_t cidr, struct in_addr *ip2,uint8_t cidr2);
int mfeipset_set_net_delete(void *handle, const char *name, struct in_addr *ip, uint8_t cidr, struct in_addr *ip2, uint8_t cidr2);

int mfeipset_set_create6(void *handle, const char *name, enum mfeipset_set_family family, uint32_t timeout);
int mfeipset_set_add6(void *handle, const char *name, uint32_t timeout, struct in6_addr *ip, uint16_t port, struct in6_addr *ip2);
int mfeipset_set_delete6(void *handle, const char *name, struct in6_addr *ip, uint16_t port, struct in6_addr *ip2);
int mfeipset_set_net_add6(void *handle, const char *name, uint32_t timeout, struct in6_addr *ip, uint8_t cidr, struct in6_addr *ip2, uint8_t cidr2);
int mfeipset_set_net_delete6(void *handle, const char *name,struct in6_addr *ip, uint8_t cidr, struct in6_addr *ip2, uint8_t cidr2);

int mfeipset_set_flush(void *handle, const char *name);
int mfeipset_set_destroy(void *handle, const char *name);

void mfeipset_close(void *handle);
