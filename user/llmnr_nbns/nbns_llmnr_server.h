#ifndef NBNS_LLMNR_SERVER_H_
#define NBNS_LLMNR_SERVER_H_

#define uint8							unsigned char
#define uint16							unsigned short
#define uint32							unsigned int
#define int8							char
#define int16							short
#define int32							int

struct llmnr_answer {
	uint8  len;
	char*	name;
	unsigned short type;
	unsigned short ClassType;
	unsigned int ttl;
	unsigned short datalen;
	unsigned short flags;
	unsigned char ip[4];
} __attribute__((__packed__));

struct llmnr_ans {
	unsigned short id;
	unsigned short flags;
	unsigned short question;
	unsigned short answer;
	unsigned short auth;
	unsigned short add;
	struct llmnr_answer answers;
} __attribute__((__packed__));

struct llmnr_query {
	uint8  len;
	char   *name;
	unsigned short type;
	unsigned short ClassType;
} __attribute__((__packed__));

struct llmnr_req {
	unsigned short id;
	unsigned short flags;
	unsigned short question;
	unsigned short answer;
	unsigned short auth;
	unsigned short add;
	struct llmnr_query queries;
} __attribute__((__packed__));

struct nbns_query {
	char blank;
	char name[33];
	unsigned short type;
	unsigned short ClassType;
} __attribute__((__packed__));

struct nbns_addrr {
    unsigned short rr_name;
    unsigned short type;
    unsigned short ClassType;
    unsigned int ttl;
    unsigned short datelen;
    unsigned short flags;
    unsigned int addr;
} __attribute__((__packed__));

struct nbns_answer {
	char blank;
	char name[33];
	unsigned short type;
	unsigned short ClassType;
	unsigned int ttl;
	unsigned short datalen;
	unsigned short flags;
	unsigned char ip[4];
} __attribute__((__packed__));

struct nbns_req {
	unsigned short id;
	unsigned short flags;
	unsigned short question;
	unsigned short answer;
	unsigned short auth;
	unsigned short add;
	struct nbns_query queries;
	struct nbns_addrr addrr;
} __attribute__((__packed__));

struct nbns_ans {
	unsigned short id;
	unsigned short flags;
	unsigned short question;
	unsigned short answer;
	unsigned short auth;
	unsigned short add;
	struct nbns_answer answers;
} __attribute__((__packed__));

struct nbname {
	char ascii_name[16] ;
	unsigned short rr_flags;
} __attribute__((__packed__));

typedef struct nbname_response_header {
	unsigned short transaction_id;
	unsigned short flags;
	unsigned short question_count;
	unsigned short answer_count;
	unsigned short name_service_count;
	unsigned short additional_record_count;
	char question_name[34];
	unsigned short question_type;
	unsigned short question_class;
	unsigned int ttl;
	unsigned short rdata_length;
	unsigned char number_of_names;
} __attribute__((__packed__)) nbname_response_header_t;

typedef struct nbname_response_footer {
	unsigned char adapter_address[6];
	unsigned char version_major;
	unsigned char version_minor;
	unsigned short duration;
	unsigned short frmps_received;
	unsigned short frmps_transmitted;
	unsigned short iframe_receive_errors;
	unsigned short transmit_aborts;
	unsigned int transmitted;
	unsigned int received;
	unsigned short iframe_transmit_errors;
	unsigned short no_receive_buffer;
	unsigned short tl_timeouts;
	unsigned short ti_timeouts;
	unsigned short free_ncbs;
	unsigned short ncbs;
	unsigned short max_ncbs;
	unsigned short no_transmit_buffers;
	unsigned short max_datagram;
	unsigned short pending_sessions;
	unsigned short max_sessions;
	unsigned short packet_sessions;
} __attribute__((__packed__)) nbname_response_footer_t;

struct nb_host_info {
	struct nbname_response_header* header;
	struct nbname* names;
	struct nbname_response_footer* footer;
	int is_broken;
} __attribute__((__packed__));

#define DATAMAX 128
#define MAX_BUF_LEN 2048


#define LLMNR_PORT (5355)
#define LLMNR_V4_MCAST_ADDR "224.0.0.252"
#define LLMNR_V6_MCAST_ADDR "ff02::1:3"

#define NBNS_PORT (137)

#define IPV6_ADDR_FILE_PATH "/proc/net/if_inet6"

#define NBNS_HOSTINFO_FILE_PATH "/tmp/nbns.hostinfo"

#endif




