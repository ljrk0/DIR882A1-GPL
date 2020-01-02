#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
//#include <signal.h>
#include <ctype.h>
//#include <linux/udp.h>
//#include <netinet/ip.h>
//#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <net/if.h>
//#include <linux/if_ether.h>
#include <netinet/in.h>
//#include <netpacket/packet.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include "nbns_llmnr_server.h"

//#define DEBUG 1
#ifdef DEBUG
#define NLS_DEBUG(fmt, args...)  printf("[%s:%d] "fmt, __func__, __LINE__, ##args)
#else
#define NLS_DEBUG(fmt, args...)
#endif

#define NLS_ERROR(fmt, args...)  printf("[%s:%d] "fmt, __func__, __LINE__, ##args)

#define MAX_MATCH_STR_NUM 4
static int match_str_num = 0;

char manageip[16] = {0};
char manageipv6[48] = {0};
char match_str[MAX_MATCH_STR_NUM][DATAMAX] = {{0}};
char nbns_llmnr_buf[MAX_BUF_LEN] = {0};
char dev_interface[8] = {0};

unsigned int get32(void* data) {
	union {
		char bytes[4];
		unsigned int all;
	} x;

	memcpy(x.bytes, data, 4);
	return(ntohl(x.all));
};

unsigned short get16(void* data) {
        union {
                char bytes[2];
                unsigned short all;
        } x;

        memcpy(x.bytes, data, 2);
        return(ntohs(x.all));
};

struct nb_host_info* parse_response(char* buff, int buffsize) 
{
	struct nb_host_info* hostinfo = NULL;
    nbname_response_footer_t* response_footer;
	nbname_response_header_t* response_header;
	int name_table_size;
	int offset = 0;

	if((response_header = malloc(sizeof(nbname_response_header_t))) == NULL) return NULL;
	if((response_footer = malloc(sizeof(nbname_response_footer_t))) == NULL) return NULL;
	bzero(response_header, sizeof(nbname_response_header_t));
	bzero(response_footer, sizeof(nbname_response_footer_t));
	
	if((hostinfo = malloc(sizeof(struct nb_host_info))) == NULL) return NULL;
	hostinfo->header = NULL;
    hostinfo->names = NULL;
	hostinfo->footer = NULL;
	hostinfo->is_broken = -1;

	/* Parsing received packet */
	/* Start with header */
	if( offset + sizeof(response_header->transaction_id) >= buffsize) goto broken_packet;
	response_header->transaction_id = get16(buff+offset); 
	//Move pointer to the next structure field
	offset+=sizeof(response_header->transaction_id);
        hostinfo->header = response_header;

	// Check if there is room for next field in buffer
	if( offset+sizeof(response_header->flags) >= buffsize) goto broken_packet; 
	response_header->flags = get16(buff+offset);
        offset+=sizeof(response_header->flags);
	
	if( offset+sizeof(response_header->question_count) >= buffsize) goto broken_packet;
	response_header->question_count = get16(buff+offset);
        offset+=sizeof(response_header->question_count);
        
	if( offset+sizeof(response_header->answer_count) >= buffsize) goto broken_packet;
	response_header->answer_count = get16(buff+offset);
        offset+=sizeof(response_header->answer_count);
        
	if( offset+sizeof(response_header->name_service_count) >= buffsize) goto broken_packet;
	response_header->name_service_count = get16(buff+offset);
        offset+=sizeof(response_header->name_service_count);
        
	if( offset+sizeof(response_header->additional_record_count) >= buffsize) goto broken_packet;
	response_header->additional_record_count = get16(buff+offset);
        offset+=sizeof(response_header->additional_record_count);
        
	if( offset+sizeof(response_header->question_name) >= buffsize) goto broken_packet;
	strncpy(response_header->question_name, buff+offset, sizeof(response_header->question_name));
        offset+=sizeof(response_header->question_name);
        
	if( offset+sizeof(response_header->question_type) >= buffsize) goto broken_packet;
	response_header->question_type = get16(buff+offset);
        offset+=sizeof(response_header->question_type);
        
	if( offset+sizeof(response_header->question_class) >= buffsize) goto broken_packet;
	response_header->question_class = get16(buff+offset);
        offset+=sizeof(response_header->question_class);
        
	if( offset+sizeof(response_header->ttl) >= buffsize) goto broken_packet;
        response_header->ttl = get32(buff+offset);
        offset+=sizeof(response_header->ttl);
        
	if( offset+sizeof(response_header->rdata_length) >= buffsize) goto broken_packet;
        response_header->rdata_length = get16(buff+offset);
        offset+=sizeof(response_header->rdata_length);
        
	if( offset+sizeof(response_header->number_of_names) >= buffsize) goto broken_packet;
	response_header->number_of_names = *(typeof(response_header->number_of_names)*)(buff+offset);
        offset+=sizeof(response_header->number_of_names);

	/* Done with packet header - it is okay */
	
	name_table_size = (response_header->number_of_names) * (sizeof(struct nbname));
	if( offset+name_table_size >= buffsize) goto broken_packet;
	
	if((hostinfo->names = malloc(name_table_size))==NULL) return NULL;
	memcpy(hostinfo->names, buff + offset, name_table_size);
	
	offset+=name_table_size;

	/* Done with name table - it is okay */ 
 
        /* Now parse response footer */

	if( offset+sizeof(response_footer->adapter_address) >= buffsize) goto broken_packet;	
	memcpy(response_footer->adapter_address, 
	       (buff+offset), 
	       sizeof(response_footer->adapter_address));
	offset+=sizeof(response_footer->adapter_address);

	hostinfo->footer=response_footer;	

#if 0
	if( offset+sizeof(response_footer->version_major) >= buffsize) goto broken_packet;
	response_footer->version_major = *(typeof(response_footer->version_major)*)(buff+offset);
	offset+=sizeof(response_footer->version_major);
	
	if( offset+sizeof(response_footer->version_minor) >= buffsize) goto broken_packet;
	response_footer->version_minor = *(typeof(response_footer->version_minor)*)(buff+offset);
	offset+=sizeof(response_footer->version_minor);
	
	if( offset+sizeof(response_footer->duration) >= buffsize) goto broken_packet;
	response_footer->duration = get16(buff+offset);
	offset+=sizeof(response_footer->duration);
	
	if( offset+sizeof(response_footer->frmps_received) >= buffsize) goto broken_packet;
	response_footer->frmps_received= get16(buff+offset);
	offset+=sizeof(response_footer->frmps_received);
	
	if( offset+sizeof(response_footer->frmps_transmitted) >= buffsize) goto broken_packet;
	response_footer->frmps_transmitted = get16(buff+offset);
	offset+=sizeof(response_footer->frmps_transmitted);
	
	if( offset+sizeof(response_footer->iframe_receive_errors) >= buffsize) goto broken_packet;
	response_footer->iframe_receive_errors = get16(buff+offset);
	offset+=sizeof(response_footer->iframe_receive_errors);
	
	if( offset+sizeof(response_footer->transmit_aborts) >= buffsize) goto broken_packet;
	response_footer->transmit_aborts =  get16(buff+offset);
	offset+=sizeof(response_footer->transmit_aborts);
	
	if( offset+sizeof(response_footer->transmitted) >= buffsize) goto broken_packet;
	response_footer->transmitted = get32(buff+offset);
	offset+=sizeof(response_footer->transmitted);
	
	if( offset+sizeof(response_footer->received) >= buffsize) goto broken_packet;
	response_footer->received = get32(buff+offset);
	offset+=sizeof(response_footer->received);
	
	if( offset+sizeof(response_footer->iframe_transmit_errors) >= buffsize) goto broken_packet;
	response_footer->iframe_transmit_errors = get16(buff+offset);
	offset+=sizeof(response_footer->iframe_transmit_errors);
	
	if( offset+sizeof(response_footer->no_receive_buffer) >= buffsize) goto broken_packet;
	response_footer->no_receive_buffer = get16(buff+offset);
	offset+=sizeof(response_footer->no_receive_buffer);
	
	if( offset+sizeof(response_footer->tl_timeouts) >= buffsize) goto broken_packet;
	response_footer->tl_timeouts = get16(buff+offset);
	offset+=sizeof(response_footer->tl_timeouts);
	
	if( offset+sizeof(response_footer->ti_timeouts) >= buffsize) goto broken_packet;
	response_footer->ti_timeouts = get16(buff+offset);
	offset+=sizeof(response_footer->ti_timeouts);
	
	if( offset+sizeof(response_footer->free_ncbs) >= buffsize) goto broken_packet;
	response_footer->free_ncbs = get16(buff+offset);
	offset+=sizeof(response_footer->free_ncbs);
	
	if( offset+sizeof(response_footer->ncbs) >= buffsize) goto broken_packet;
	response_footer->ncbs = get16(buff+offset);
	offset+=sizeof(response_footer->ncbs);
	
	if( offset+sizeof(response_footer->max_ncbs) >= buffsize) goto broken_packet;
	response_footer->max_ncbs = get16(buff+offset);
	offset+=sizeof(response_footer->max_ncbs);
	
	if( offset+sizeof(response_footer->no_transmit_buffers) >= buffsize) goto broken_packet;
	response_footer->no_transmit_buffers = get16(buff+offset);
	offset+=sizeof(response_footer->no_transmit_buffers);
	
	if( offset+sizeof(response_footer->max_datagram) >= buffsize) goto broken_packet;
	response_footer->max_datagram = get16(buff+offset);
	offset+=sizeof(response_footer->max_datagram);
	
	if( offset+sizeof(response_footer->pending_sessions) >= buffsize) goto broken_packet;
	response_footer->pending_sessions = get16(buff+offset);
	offset+=sizeof(response_footer->pending_sessions);
	
	if( offset+sizeof(response_footer->max_sessions) >= buffsize) goto broken_packet;
	response_footer->max_sessions = get16(buff+offset);
	offset+=sizeof(response_footer->max_sessions);
	
	if( offset+sizeof(response_footer->packet_sessions) >= buffsize) goto broken_packet;
	response_footer->packet_sessions = get16(buff+offset);
	offset+=sizeof(response_footer->packet_sessions);
#endif
	/* Done with packet footer and the whole packet */

	return hostinfo;
	
	broken_packet: 
		hostinfo->is_broken = offset;
		return hostinfo;	
};

void save_hostinfo(struct in_addr addr, struct nb_host_info* hostinfo)
{
    int i;
    unsigned char service; 
    char comp_name[16] = {0};
    char mac[32] = {0};
    int unique;
    int find=0;
    //FILE *fp = NULL;

    strncpy(comp_name,"<unknown>",15);
    if(hostinfo->header && hostinfo->names) {
        for(i=0; i< hostinfo->header->number_of_names; i++) {
            service = hostinfo->names[i].ascii_name[15];
            unique = ! (hostinfo->names[i].rr_flags & 0x0080);
            if(service == 0 && unique ) {
                /* Unique name, workstation service - this is computer name */ 
                strncpy(comp_name, hostinfo->names[i].ascii_name, 15);
                comp_name[15] = 0;
                find = 1;
                break;
            }
        }
    }

    if(hostinfo->footer) {
        snprintf(mac, sizeof(mac),
        "%02X:%02X:%02X:%02X:%02X:%02X",
        hostinfo->footer->adapter_address[0], hostinfo->footer->adapter_address[1],
        hostinfo->footer->adapter_address[2], hostinfo->footer->adapter_address[3],
        hostinfo->footer->adapter_address[4], hostinfo->footer->adapter_address[5]);
    }  

    if(!find || !mac[0]) {
        NLS_ERROR("name or mac is null.\n");
        return;
    }
    
    //TODO: save host info to file
    

    return;
}

void get_host_info(struct in_addr addr, char* buff, int buffsize)
{
    struct nb_host_info* hostinfo;

	hostinfo = (struct nb_host_info*)parse_response(buff, buffsize);
	if(!hostinfo || hostinfo->is_broken >= 0) {
	    NLS_ERROR("parse_response returned NULL");
	    return;
	};

	save_hostinfo(addr, hostinfo);
    return;
}

void write_hostinfo_to_file(struct in_addr addr, char *hostname)
{
    char cmd[128] = {0}, cnt[8] = {0}, buf[64] = {0}, tmpbuf[64] = {0};
    FILE *fp_hostinfo = NULL, *fp_cnt = NULL;
    char *ip = NULL;
    char *hi_buf = NULL;
    int n = 0, hi_buf_len = 64 * 64;
    int find = 0;

    if(access(NBNS_HOSTINFO_FILE_PATH, F_OK) != 0) {
        fp_hostinfo = fopen(NBNS_HOSTINFO_FILE_PATH, "w+");
        if(!fp_hostinfo) {
            return;
        }
        fprintf(fp_hostinfo, "%s %s\n", inet_ntoa(addr), hostname);
        fflush(fp_hostinfo);
        fclose(fp_hostinfo);
    } else {
        snprintf(cmd, sizeof(cmd), "cat %s | wc -l", NBNS_HOSTINFO_FILE_PATH);
        fp_cnt = popen(cmd, "r");
        if(!fp_cnt) {
            return;
        }

        fgets(cnt, sizeof(cnt), fp_cnt);
        pclose(fp_cnt);
        if(atoi(cnt) >= 64) {
            fp_hostinfo = fopen(NBNS_HOSTINFO_FILE_PATH, "w+");
            if(!fp_hostinfo) {
                return;
            }
            fprintf(fp_hostinfo, "%s %s\n", inet_ntoa(addr), hostname);
            fflush(fp_hostinfo);
            fclose(fp_hostinfo);
        } else {
            fp_hostinfo = fopen(NBNS_HOSTINFO_FILE_PATH, "r");
            if(!fp_hostinfo) {
                return;
            }

            hi_buf = malloc(hi_buf_len);
            if(!hi_buf) {
                NLS_ERROR("malloc fail.\n");
                fclose(fp_hostinfo);
                return;
            }

            memset(hi_buf, 0x0, hi_buf_len);
            
            while(fgets(buf, sizeof(buf), fp_hostinfo)) {
                strcpy(tmpbuf, buf);
                ip = strtok(tmpbuf, " ");
                if(ip) {
                    if(strcmp(ip, inet_ntoa(addr)) == 0) {
                        n += snprintf(hi_buf + n , hi_buf_len - n, "%s %s\n", inet_ntoa(addr), hostname);
                        find = 1;
                    } else {
                        n += snprintf(hi_buf + n , hi_buf_len - n, "%s", buf);
                    }
                }
                memset(buf, 0x0, sizeof(buf));
                memset(tmpbuf, 0x0, sizeof(tmpbuf));
            }

            fclose(fp_hostinfo);

            if(!find) {
                n += snprintf(hi_buf + n , hi_buf_len - n, "%s %s\n", inet_ntoa(addr), hostname);
            }            

            fp_hostinfo = fopen(NBNS_HOSTINFO_FILE_PATH, "w+");
            if(!fp_hostinfo) {
                free(hi_buf);
                return;
            }

            fprintf(fp_hostinfo, "%s", hi_buf);
            fflush(fp_hostinfo);
            fclose(fp_hostinfo);
            free(hi_buf);
        }
        
    }

    return;
}

void toUpper(char *s) {
    if(!s) {
        return;
    }
    
    while(*s) {
        if((*s >= 'a') && (*s <= 'z')) {
            *s += 'A' - 'a';
        }
        ++s;
    }
    return;
}

void toLower(char *s) {
    if(!s) {
        return;
    }
    
    while(*s) {
        if((*s >= 'A') && (*s <= 'Z')) {
            *s -= 'A' - 'a';
        }
        ++s;
    }
    return;
}

int isDomainNameMatch(char pDomainName[][DATAMAX], char *pRequestName)
{
    int i = 0;

    if(pDomainName == NULL || pRequestName == NULL) {
        return 0;
    }

    for(i = 0; i < MAX_MATCH_STR_NUM; i++) {
        NLS_DEBUG("pDomainName:%s pRequestName:%s\n", pDomainName[i], pRequestName);
        
        if(strlen(pDomainName[i]) != 0 && strlen(pRequestName) != 0) {
            if(strlen(pDomainName[i]) == strlen(pRequestName) && strncmp(pDomainName[i], pRequestName, strlen(pDomainName[i])) == 0) {
                return 1;
            }            
            
            if(strlen(pDomainName[i]) == strlen(pRequestName) - 4 && strncmp(pDomainName[i], &pRequestName[4], strlen(pDomainName[i])) == 0) {                
                return 1;
            }            
        }

    }

    return 0;        
}

int get_options(int argc, char *argv[])
{
    int c = 0;
    while((c = getopt(argc, argv, "s:i:")) != -1) {
        switch(c) {
        case 's':
            if(match_str_num >= MAX_MATCH_STR_NUM) {
                NLS_ERROR("max match num is %d\n", MAX_MATCH_STR_NUM);
                return -1;
            } else {
                strncpy(match_str[match_str_num], optarg, strlen(optarg));
                NLS_DEBUG("hijack str:%s\n", match_str[match_str_num]);
                match_str_num++;
            }
    		break;

    	case 'i':
            strncpy(dev_interface, optarg, strlen(optarg));
            NLS_DEBUG("bind interface:%s\n", optarg);
    	    break;
#if 0    		
        case 'i':
    		memcpy(manageip, optarg, strlen(optarg));

    		//memcpy(manageipBak, optarg, strlen(optarg));
    		
    		NLS_DEBUG("ipv4:%s\n", manageip);
    		break;
    		
        case 'k':
    		memcpy(manageipv6, optarg, strlen(optarg));
    		NLS_DEBUG("ipv6:%s\n", manageipv6);
    		break;	
#endif    		
    		
        default:                
            break;
        }
    }
    
    return 0;
}

int getInterfaceAddr(const char *ifname, char *ifaddr)
{
    struct ifreq ifr;
    struct sockaddr_in *addr = NULL;
    int sockFd = -1;
    int iRet = -1;

    if(ifname == NULL || ifaddr == NULL) {
        return -1;
    }

    sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockFd < 0) {
        NLS_ERROR("socket \n");
        return -1;
    }
    
    memset(&ifr, 0, sizeof(ifr)); 
	strcpy(ifr.ifr_name, ifname);
	
	iRet = ioctl(sockFd, SIOCGIFADDR, &ifr);
	if(iRet < 0) {
	    NLS_ERROR("ioctl \n");
	    close(sockFd);
        return -1;
	} 
	
	addr = (struct sockaddr_in *)&ifr.ifr_addr;
	NLS_DEBUG("****addr:%s****\n", (char*)inet_ntoa(addr->sin_addr));
	
    strcpy(ifaddr, (char*)inet_ntoa(addr->sin_addr));
    close(sockFd);

    return 0;
}

unsigned char charToValue(char c)
{
    unsigned char tmp = 0;
    
    if(c >= '0' && c <= '9') {
        tmp = c - '0';
    }
    
    if(c >= 'A' && c <= 'F') {
        tmp = c - 'A' + 10;
    }
    
    if(c >= 'a' && c <= 'f') {
        tmp = c - 'a' + 10;
    }

    return tmp;
}

int getInterfaceAddrV6(const char *ifname, struct in6_addr *sin6_addr)
{
    FILE *fp = NULL;
    char addrBuf[256] = {0};
    char *saveptr = NULL;
    char *item = NULL;
    char *p = NULL;
    int index = 0;
    int find = 0;
    unsigned char low = 0;
    unsigned char high = 0;

    enum {
        IF_ADDR,
        IF_INDEX,
        PREFIX_LEN,
        SCOPE_VAL,
        IF_FLAGS,
        IF_NAME,
        MAX_INDEX,
    };

    char* if_info[MAX_INDEX] = {NULL};

    if((fp = fopen(IPV6_ADDR_FILE_PATH, "r")) == NULL) {
        NLS_ERROR("open file fail \n");
        goto done;
    }

    while(fgets(addrBuf, sizeof(addrBuf), fp)) {
        for(item = strtok_r(addrBuf, " ", &saveptr); item != NULL && index < MAX_INDEX; item = strtok_r(NULL, " ", &saveptr)) {
            if_info[index++] = item;
        }

        index = 0;
        if_info[IF_NAME][strlen(if_info[IF_NAME]) - 1] = '\0';
        if(strcmp(if_info[IF_NAME], ifname) != 0) {
            continue;
        }

        p = if_info[IF_ADDR];
        NLS_DEBUG("****addr:%s****\n", p);
        if(strlen(p) != 32) {
            NLS_DEBUG("Addr len error \n");
            continue;
        }

        toLower(p);
        if(strncmp(p, "fe80", 4) != 0) {
            NLS_DEBUG("Not link local addr \n");
            continue;
        }
        
        while(*p != '\0') {
            high = charToValue(*p++);
            low = charToValue(*p++);
            sin6_addr->s6_addr[index++] = ((high & 0x0f) << 4) | (low & 0x0f);
        }
        #if 0
        for(i = 0; i < 16; i++) {
            printf("0x%02X ", sin6_addr->s6_addr[i]);
        }
        
        printf("\n");
        #endif
        find = 1;
        break;
    }

done:
    if(fp) {
        fclose(fp);
    }
    
    return find;
}

int openNbnsRecvSocket(const char *bind_addr)
{
	int s;
	int i = 1;
	struct sockaddr_in sockname;
	
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if(s < 0) {
		NLS_ERROR("socket: %s\n", strerror(errno));
		return -1;
	}	

	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0) {
		NLS_ERROR("setsockopt(SO_REUSEADDR): %s\n", strerror(errno));
		close(s);
		return -1;
	}	

	memset(&sockname, 0, sizeof(struct sockaddr_in));
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(NBNS_PORT);
	sockname.sin_addr.s_addr= htonl(INADDR_ANY);

	if(bind(s, (struct sockaddr *)&sockname, sizeof(struct sockaddr_in)) < 0) {
		NLS_ERROR("bind: %s\n", strerror(errno));
		close(s);
		return -1;
	}

	return s;
}

int openLlmnrRecvSocketV6(const char *bind_addr)
{
	int s = -1;
	int i = 1;
	struct sockaddr_in6 sockname;
	struct ipv6_mreq mreq6;
	int if_index = 0;
	struct sockaddr_in6 addr6;
	
	s = socket(AF_INET6, SOCK_DGRAM, 0);
	if(s < 0) {
		NLS_ERROR("socket: %s\n", strerror(errno));
		return -1;
	}	

	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0) {
		NLS_ERROR("setsockopt(SO_REUSEADDR): %s\n", strerror(errno));
		close(s);
		return -1;
	}	
	
    i = 1;
    if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &i, sizeof(i)) < 0) {
        NLS_ERROR("setsockopt(IPV6_V6ONLY): %s\n", strerror(errno));
        close(s);
        return -1;
    }

    memset(&sockname, 0, sizeof(struct sockaddr_in6));
    sockname.sin6_family = AF_INET6;
	sockname.sin6_port = htons(LLMNR_PORT);

	if(bind(s, (struct sockaddr *)&sockname, sizeof(struct sockaddr_in6)) < 0) {
		NLS_ERROR("bind: %s\n", strerror(errno));
		close(s);
		return -1;
	}

	if(inet_pton(AF_INET6, LLMNR_V6_MCAST_ADDR, &addr6.sin6_addr) <= 0) {
	    NLS_ERROR("inet_pton \n");
	    close(s);
        return -1;
	}

	memset(&mreq6, 0, sizeof(mreq6));
    mreq6.ipv6mr_multiaddr = addr6.sin6_addr;
    if_index = if_nametoindex("br0");
    NLS_DEBUG("*****if_index:%d*****\n", if_index);
    mreq6.ipv6mr_interface = if_index > 0 ? if_index : 0;	
    #if 0
    for(i = 0; i < 16; i++) {
        printf("0x%02X ", mreq6.ipv6mr_multiaddr.s6_addr[i]);
    }
    printf("\n");
    #endif
    if (setsockopt(s, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mreq6, sizeof(mreq6)) < 0) {
        NLS_ERROR("setsockopt(IPV6_ADD_MEMBERSHIP): %s\n", strerror(errno));
        close(s);
        return -1;
    }
    
	return s;
}

int openLlmnrRecvSocket(const char *bind_addr)
{
	int s;
	int i = 1;
	struct sockaddr_in sockname;
	struct ip_mreq imr;
	
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if(s < 0) {
		NLS_ERROR("socket: %s\n", strerror(errno));
		return -1;
	}	

	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0) {
		NLS_ERROR("setsockopt(SO_REUSEADDR): %s\n", strerror(errno));
		close(s);
		return -1;
	}	

	memset(&sockname, 0, sizeof(struct sockaddr_in));
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(LLMNR_PORT);
	sockname.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(s, (struct sockaddr *)&sockname, sizeof(struct sockaddr_in)) < 0) {
		NLS_ERROR("bind: %s\n", strerror(errno));
		close(s);
		return -1;
	}

	memset(&imr, 0x0, sizeof(imr));
	imr.imr_multiaddr.s_addr = inet_addr(LLMNR_V4_MCAST_ADDR);
	imr.imr_interface.s_addr = inet_addr(bind_addr);
    
	if(setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&imr, sizeof(imr)) < 0) {
		NLS_ERROR("setsockopt(IP_ADD_MEMBERSHIP): %s\n", strerror(errno));
		close(s);
		return -1;
	}	

	return s;
}

int DecodeName(char *in, char *out, int *type)
{
		char tmpbuf[34] = {0};
		int i = 0;
		char p, q;
		int len;
		
		for(i = 0; i < 16; i++) {	
			p = *(in + 2 * i);
			q = *(in + 2 * i + 1);
			if(p >= 'A' && q >= 'A') {
			    tmpbuf[i] = (((p - 'A') & 0x0f) << 4) + ((q -'A') & 0x0f);
			} else
			    break;
		}

		*type = tmpbuf[15];
		tmpbuf[15] = '\0';

        len = strlen(tmpbuf);

        while(len--) {
            if(tmpbuf[len - 1] == ' ')
                tmpbuf[len - 1] = '\0';
            else
                break;
        }

		memcpy(out, tmpbuf, strlen(tmpbuf));
		return 0;	
}

int nbnsPacketProcess(int sockFd, struct sockaddr_in *pClientAddr, size_t client_addr_len, int recv_len)
{
	struct nbns_req *req;
	struct nbns_ans *ans;
    char namebuf[DATAMAX] = {0};
    unsigned int ipnum[4] = {0};	
    int type = 0;

    char *pRecvBuf = nbns_llmnr_buf;

    req = (struct nbns_req*)pRecvBuf;
#if 0   //parse response after using nbtscan utils, this feature not test!!!
    if(htons(req->flags) & 1 << 15) {   //response
        get_host_info(pClientAddr->sin_addr, pRecvBuf, recv_len);
        return 0;
    }
#endif
    NLS_DEBUG("id:0x%04x flag:0x%04x\n", 
            htons(req->id), 
            htons(req->flags));

    if((htons(req->flags) >> 15) & 0x1) {   //response
        return 0;
    }

    if(((htons(req->flags) >> 11) & 0xf) == 0x5 
        && htons(req->question) == 0x1
        && htons(req->answer) == 0x0
        && htons(req->auth) == 0x0
        && htons(req->add) == 0x1) {
        DecodeName(req->queries.name, namebuf, &type);
        NLS_DEBUG("type111:0x%x name:%s\n", type, namebuf);
        if(type != 0x0) {
            NLS_DEBUG("drop111->id:0x%04x\n", htons(req->id));
            return 0;
        }

        if((htons(req->addrr.flags) >> 15) & 0x1) {
            NLS_DEBUG("drop222->id:0x%04x\n", htons(req->id));
            return 0;            
        }

        write_hostinfo_to_file(pClientAddr->sin_addr, namebuf);

    } else if(((htons(req->flags) >> 11) & 0xf) == 0x0) {

        DecodeName(req->queries.name, namebuf, &type);
        NLS_DEBUG("type222:0x%x name:%s\n", type, namebuf);
        //toUpper(match_str);

    	if(!isDomainNameMatch(match_str, namebuf)) 
            return -1;

    	sscanf(manageip, "%u.%u.%u.%u", &ipnum[0], &ipnum[1], &ipnum[2], &ipnum[3]);

        ans = malloc(sizeof(struct nbns_ans));
        if(ans == NULL) {
            NLS_ERROR("malloc fail\n");
            return -1;
        }	

        memset(ans, 0x0, sizeof(struct nbns_ans));

        //设置用户数据包层
    	ans->id 	  = req->id;
    	ans->flags 	  = htons(0x8400); //应答包
    	ans->question = htons(0x00);   //询问包个数
    	ans->answer	  = htons(0x01);   //应答包个数
    	ans->auth	  = htons(0x0);    //无认证
    	ans->add	  = htons(0x0);    //附加信息   

        ans->answers.blank = req->queries.blank;

        memcpy(ans->answers.name, req->queries.name, sizeof(req->queries.name));
    	ans->answers.type = req->queries.type;
    	ans->answers.ClassType = req->queries.ClassType;
    	ans->answers.ttl	= htonl(0x00000e10);
    	ans->answers.datalen = htons(0x06);
    	ans->answers.flags = htons(0x4000);
    	ans->answers.ip[0] = (unsigned char)ipnum[0];
    	ans->answers.ip[1] = (unsigned char)ipnum[1];
    	ans->answers.ip[2] = (unsigned char)ipnum[2];
    	ans->answers.ip[3] = (unsigned char)ipnum[3];    

    	sendto(sockFd, ans, sizeof(struct nbns_ans), 0, (struct sockaddr*)pClientAddr, client_addr_len);

    	if(ans != NULL) {
            free(ans);
    	}
	}
			
	return 0;
}


int llmnrPacketProcess(int sockFd, struct sockaddr_in6 *pClientAddr, socklen_t client_addr_len)
{
    struct llmnr_req *req;
	struct llmnr_ans *ans;
    char namebuf[DATAMAX] = {0};
    unsigned int ipnum[4] = {0};
    char dataSentbuf[DATAMAX * 2] = {0};
    int dataLen = 0;
    struct in6_addr ipv6addr;
    int i = 0;
    int queryType = 0;
    int queryClass = 0;

    char *pRecvBuf = nbns_llmnr_buf;

    req = (struct llmnr_req*)pRecvBuf;
    strncpy(namebuf, pRecvBuf + 12 + 1, req->queries.len);
    queryType = ntohs(*(unsigned short *)(pRecvBuf + 12 + 1 + req->queries.len + 1));
	queryClass = ntohs(*(unsigned short *)(pRecvBuf + 12 + 1 + req->queries.len + 1 + 2));

	toUpper(namebuf);
	//toUpper(match_str);

	if(!isDomainNameMatch(match_str, namebuf)) 
        return -1;  

	sscanf(manageip, "%u.%u.%u.%u", &ipnum[0], &ipnum[1], &ipnum[2], &ipnum[3]);

    if(!getInterfaceAddrV6(dev_interface, &ipv6addr)) {
        NLS_ERROR("getInterfaceAddrV6 \n");
        return -1;
    }

    ans = malloc(sizeof(struct llmnr_ans));
    if(ans == NULL) {
        NLS_ERROR("malloc fail \n");
        return -1;
    }

    memset(ans, 0x0, sizeof(struct llmnr_ans));
    
    //设置用户数据包层
	ans->id 	    = req->id;
	ans->flags 	    = htons(0x8000); //应答包
	ans->question   = htons(0x01);   //询问包个数
	if( (queryType == 0x001C) || (queryType == 0x0001) ) {
	    ans->answer	= htons(0x01);   //应答包个数
	} else {
        ans->answer	  = htons(0x00);   //应答包个数
	}
	ans->auth	    = htons(0x0);    //无认证
	ans->add	    = htons(0x0);    //附加信息
	
    memcpy(&dataSentbuf[dataLen], ans, 12);
    dataLen += 12;
    dataSentbuf[dataLen++] = req->queries.len;
    memcpy(&dataSentbuf[dataLen], pRecvBuf + 12 + 1, req->queries.len);
    dataLen += req->queries.len + 1;
    memcpy(&dataSentbuf[dataLen], pRecvBuf + 14 + req->queries.len, 2);
    dataLen += 2;
    memcpy(&dataSentbuf[dataLen], pRecvBuf + 16 + req->queries.len, 2);
    dataLen += 2;

	if(queryType == 0x0001) 
	{   //Type A
        dataSentbuf[dataLen++] = req->queries.len;
        memcpy(&dataSentbuf[dataLen], pRecvBuf + 12 + 1, req->queries.len);
        dataLen += req->queries.len + 1;
        memcpy(&dataSentbuf[dataLen], pRecvBuf + 14 + req->queries.len, 2);
        dataLen += 2;
        memcpy(&dataSentbuf[dataLen], pRecvBuf + 16 + req->queries.len, 2);
        dataLen += 2;     
    	// time to live
    	dataSentbuf[dataLen++] = 0x00;
    	dataSentbuf[dataLen++] = 0x00;
    	dataSentbuf[dataLen++] = 0x00;
    	dataSentbuf[dataLen++] = 0x1e;
    	// ip address len
    	dataSentbuf[dataLen++] = 0x00;
    	dataSentbuf[dataLen++] = 0x04;
    	// ip address
    	dataSentbuf[dataLen++] = (unsigned char)ipnum[0];
    	dataSentbuf[dataLen++] = (unsigned char)ipnum[1];
    	dataSentbuf[dataLen++] = (unsigned char)ipnum[2];
    	dataSentbuf[dataLen++] = (unsigned char)ipnum[3];        
    }
    else if(queryType == 0x001C) {   //Type AAAA
        dataSentbuf[dataLen++] = req->queries.len;
        memcpy(&dataSentbuf[dataLen], pRecvBuf + 12 + 1, req->queries.len);
        dataLen += req->queries.len + 1;
        memcpy(&dataSentbuf[dataLen], pRecvBuf + 14 + req->queries.len, 2);
        dataLen += 2;
        memcpy(&dataSentbuf[dataLen], pRecvBuf + 16 + req->queries.len, 2);
        dataLen += 2;     
    	// time to live
    	dataSentbuf[dataLen++] = 0x00;
    	dataSentbuf[dataLen++] = 0x00;
    	dataSentbuf[dataLen++] = 0x00;
    	dataSentbuf[dataLen++] = 0x1e;
    	// ip address len
    	dataSentbuf[dataLen++] = 0x00;
    	dataSentbuf[dataLen++] = 0x10;
    	// ip address
    	for(i = 0; i < 16; i++) {
            dataSentbuf[dataLen++] = ipv6addr.s6_addr[i];
    	}
    }

    
    sendto(sockFd, dataSentbuf, dataLen, 0, (struct sockaddr*)pClientAddr, client_addr_len);
    
	if(ans != NULL) {
        free(ans);
	}
			
	return 0;
}

int main(int argc, char *argv[])
{
    pid_t pid;
    fd_set readfds;
    int nReadBytes = -1;
    struct timeval tm;
    int nbns_sock = -1;
    int llmnr_sock = -1;
    int llmnr_sock_v6 = -1;
    char ifaddr[16] = {0};
    int maxFd = -1;
    int retSelect = -1;
    int i = 0;

    printf("nbns and llmnr server starting...\n");

	if(get_options(argc, argv) < 0 ) {
        return -1;
    }

    if(getInterfaceAddr(dev_interface, ifaddr) != 0) {
        NLS_ERROR("getInterfaceAddr \n");
        return -1;
    }

    memset(manageip, 0x0, sizeof(manageip));
    strcpy(manageip, ifaddr);

    for(i = 0; i < MAX_MATCH_STR_NUM; i++) {
        toUpper(match_str[i]);
    }

    llmnr_sock = openLlmnrRecvSocket(ifaddr);
    if(llmnr_sock < 0) {
        return -1;
    }

    nbns_sock = openNbnsRecvSocket(ifaddr);
    if(nbns_sock < 0) {
        return -1;
    }

    llmnr_sock_v6 = openLlmnrRecvSocketV6(ifaddr);
    if(llmnr_sock_v6 < 0) {
        return -1;
    }

    pid = fork();
    if(pid > 0) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = 0;

        if(llmnr_sock_v6 >= 0) {
            close(llmnr_sock_v6);
        }
    
        for(;;) { 
            memset(&tm, 0x0, sizeof(tm));
            tm.tv_sec = 1;
            tm.tv_usec = 1000;
            FD_ZERO(&readfds);    
            FD_SET(llmnr_sock, &readfds);
            FD_SET(nbns_sock, &readfds);
            maxFd = llmnr_sock > nbns_sock ? llmnr_sock : nbns_sock;
            retSelect = select(maxFd + 1, &readfds, NULL, NULL, &tm);
            if(retSelect <= 0) {
                continue;
            }

            if(FD_ISSET(llmnr_sock, &readfds)) {
                memset(nbns_llmnr_buf, 0x0, sizeof(nbns_llmnr_buf));
                memset(&client_addr, 0x0, sizeof(client_addr));
                client_addr_len = sizeof(client_addr);
                nReadBytes = recvfrom(llmnr_sock, nbns_llmnr_buf, sizeof(nbns_llmnr_buf), 
                                      0, (struct sockaddr*)&client_addr, &client_addr_len);

                if(nReadBytes > 0) {
                    llmnrPacketProcess(llmnr_sock, &client_addr, client_addr_len);
                }
            }

            if(FD_ISSET(nbns_sock, &readfds)) {
                memset(nbns_llmnr_buf, 0x0, sizeof(nbns_llmnr_buf));
                memset(&client_addr, 0x0, sizeof(client_addr));
                client_addr_len = sizeof(client_addr);
                nReadBytes = recvfrom(nbns_sock, nbns_llmnr_buf, sizeof(nbns_llmnr_buf), 
                                      0, (struct sockaddr*)&client_addr, &client_addr_len);

                if(nReadBytes > 0) {
                    nbnsPacketProcess(nbns_sock, &client_addr, client_addr_len, nReadBytes);
                }                

            }
            

        }
    } else if(pid == 0) {
        struct sockaddr_in6 client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        
        if(llmnr_sock >= 0) {
            close(llmnr_sock);
        }

        if(nbns_sock >= 0) {
            close(nbns_sock);
        }

        for(;;) {
            memset(&tm, 0x0, sizeof(tm));
            tm.tv_sec = 1;
            tm.tv_usec = 1000;
            FD_ZERO(&readfds);    
            FD_SET(llmnr_sock_v6, &readfds);
            retSelect = select(llmnr_sock_v6 + 1, &readfds, NULL, NULL, &tm);
            if(retSelect <= 0) {
                continue;
            }

            if(FD_ISSET(llmnr_sock_v6, &readfds)) {
                memset(nbns_llmnr_buf, 0x0, sizeof(nbns_llmnr_buf));
                nReadBytes = recvfrom(llmnr_sock_v6, nbns_llmnr_buf, sizeof(nbns_llmnr_buf), 
                                      0, (struct sockaddr*)&client_addr, &client_addr_len);

                if(nReadBytes > 0) {
                    llmnrPacketProcess(llmnr_sock_v6, &client_addr, client_addr_len);
                }
            }            
        }
        
    } else {
        NLS_ERROR("fork error!\n");
    }

    return 0;
}
