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

#ifndef DNS_UTILS_H_
#define DNS_UTILS_H_

#include <inttypes.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "error_codes.h"

#ifndef IP_TRANSPARENT
#define IP_TRANSPARENT			19
#endif

#ifndef IP_RECVORIGDSTADDR
#define IP_RECVORIGDSTADDR		20
#endif

/* FIXME Including linux/in6.h throws redefinition errors and it looks it
 * is a known issue. Not sure if this is the best way */
#ifndef IPV6_RECVORIGDSTADDR
#define IPV6_RECVORIGDSTADDR		74
#endif

#ifndef IPV6_TRANSPARENT
#define IPV6_TRANSPARENT		75
#endif

/* TODO : More error codes need to be added */
/* Below policy values are in sync with the corresponding reason codes in cloud*/
#define POLICY_ALLOW		    0
#define POLICY_HOST_REPUTATION  1
#define POLICY_NETWORK_PAUSE	5
#define POLICY_DEVICE_BLOCK		6
#define POLICY_EULA_BLOCK		7
/*These are the additional policy types that router code uses internally*/
#define POLICY_WHITELIST		248
#define POLICY_ADD_HR_REDIRIP	249
#define POLICY_ADD_DB_REDIRIP	250
#define POLICY_ADD_EB_REDIRIP	251
#define POLICY_ADD_NP_REDIRIP	252
#define POLICY_DENY_RATING	    253
#define POLICY_UNAVAILABLE		254
#define POLICY_DECISION_PENDING 255

/* Size of various constant sized fields in DNS packet */
#define DNS_HEADER_LEN      12
#define DNS_RR_TYPE_LEN     2
#define DNS_RR_CLASS_LEN    2
#define DNS_RR_TTL_LEN      4
#define DNS_RR_RDATALEN_LEN 2

#define DENY_RESPONSE_SIZE  16

/* Standard types in RR */
#define DNS_TYPE_A          1
#define DNS_TYPE_AAAA 		28
#define DNS_TYPE_ANY        255

/* TODO Revisit the sizes */
#define MAX_PACKET_LENGTH		2048
#define MAX_CONTROL_LEN			128
#define	MAX_NAME_LEN			32

#define SHGW_IPv4				0
#define SHGW_IPv6				1

#define SHGW_IPv4_ADDR_STRLEN		INET_ADDRSTRLEN
#define SHGW_IPv6_ADDR_STRLEN		INET6_ADDRSTRLEN

#define SHGW_GET_SOCKET_DOMAIN(_addr) ((_addr->addr_type == SHGW_IPv6)? AF_INET6: AF_INET)
#define SHGW_GET_SOCKLEN(_addr) ((_addr->addr_type == SHGW_IPv6)? sizeof(struct sockaddr_in6): sizeof(struct sockaddr_in))
#define SHGW_GET_SOCKADDR(_addr) ((_addr->addr_type == SHGW_IPv6)? (struct sockaddr *) &(_addr->addr.addr6): (struct sockaddr *) &(_addr->addr.addr4))

typedef struct msg_buf
{
	char    buf[MAX_PACKET_LENGTH];
	char    cbuf[MAX_CONTROL_LEN];
	char    nbuf[MAX_NAME_LEN];
	struct  iovec iov;
	struct  msghdr msg;
	int     size;
}msg_buf_t;

/* Refactor to make it to contain rest of structures */
/*
 * TODO : Zero is not 3 bits long. add other fields (refactor the old code)
 */

typedef struct _dns_header
{
    uint16_t id;

#if BYTE_ORDER == BIG_ENDIAN
    unsigned qr:1;
    unsigned opcode:4;
    unsigned aa:1;
    unsigned tc:1;
    unsigned rd:1;
    unsigned ra:1;
    unsigned zero:3;
    unsigned rcode:4;
#endif

#if BYTE_ORDER == LITTLE_ENDIAN || BYTE_ORDER == PDP_ENDIAN
    unsigned rd :1;
	unsigned tc :1;
	unsigned aa :1;
	unsigned opcode :4;
	unsigned qr :1;
    unsigned rcode :4;
	unsigned zero:3;
	unsigned ra :1;
#endif

    uint16_t q_count;
    uint16_t ans_count;
    uint16_t auth_count;
    uint16_t add_count;
} dns_header_t;

typedef struct _dns_query_rec
{
    char            *name;
    uint16_t        qtype;
    uint16_t        qclass;
    /* Used to store the offset of the next rr/question */
    int             next_rr_offset;
} dns_query_rec_t;

typedef struct _res_record
{
    unsigned char *name;
    uint16_t      type;
    uint16_t      _class;
    uint32_t      ttl;
    uint16_t      data_len;
    unsigned char *rdata;
    /* Used to store the next rr section offset */
    int           next_rr_offset;
} res_record_t;

typedef struct __rr_data
{
	uint16_t	  answer_count;
	res_record_t *answer_records;
} rr_set_t;

typedef struct _shgw_sockaddr {
	union {
		struct sockaddr_in addr4;
		struct sockaddr_in6 addr6;
	} addr;
	unsigned int if_index;
	short addr_type;
} shgw_sockaddr_t;

typedef struct _shgw_ipset_data
{
	uint32_t			ttl;
	char 				*domain_name;
	struct in_addr 		src_ipv4;
	struct in_addr		**dst_ipv4;
	int					num_dst_ipv4;
/*
	struct in6_addr		**src_ipv6;
	struct in6_addr		**dst_ipv6;
	int					num_src_ipv6;
	int					num_dst_ipv6;
*/

} shgw_ipset_data_t;

typedef enum
{
	IPTABLE_INSERT,
	IPTABLE_DELETE
}iptable_op_type;

STATUS_CODE verify_dns_packet(char *dns_packet_buffer);
STATUS_CODE verify_policy_response(char *dns_packet_buffer);

void free_rr_set(res_record_t *answer_records, uint16_t answer_count);

STATUS_CODE parse_rr(char* dns_packet_buffer ,
                     int offset,
                     res_record_t *rr_data);

void free_rr_set(res_record_t *answer_records, uint16_t answer_count);

int get_answer_offset(char *dns_packet_buffer);

void modify_ttl_in_rr(char *dns_packet_bufer,
                      res_record_t *rr_data,
                      uint32_t ttl_to_be_set);


void update_ttl_if_required(
        char *dns_packet_buffer,
        res_record_t records[],
        uint32_t ttl_to_be_set);

/*
 * Parses the dns packet's answer RRs and stores it in answer_records
 */
STATUS_CODE parse_dns_response(char *dns_packet_buffer,
                               res_record_t *answer_records);

STATUS_CODE set_socket_options(int socket_fd, int domain);

STATUS_CODE send_dns(char *data_packet, int data_len, shgw_sockaddr_t *dst_addr, int *sock_fd);

uint16_t get_answer_count(char *dns_packet_buffer);

void set_dns_header_flags(char *dns_packet_buffer,uint16_t answer_count);

void set_dns_header_tid(char *dns_packet_buffer);

STATUS_CODE parse_policy_info(char *dns_packet_buffer, int *decision, int *dns_packet_buffer_len, res_record_t **parsed_records);

void get_policy_info(char *data_buffer, int *decision, int *data_buffer_length, res_record_t **parsed_records);

STATUS_CODE get_packet_details(msg_buf_t *msg_buf, shgw_sockaddr_t *src_addr, shgw_sockaddr_t *dst_addr, char **payload);

STATUS_CODE prepare_to_recvmsg(msg_buf_t **msg_buf);

STATUS_CODE receive_data(msg_buf_t **msg_buf, int fd);

uint16_t get_transaction_id(const char * dns_packet);
uint16_t get_request_type(const char* data_buffer);

#endif /* DNS_UTILS_H_ */
