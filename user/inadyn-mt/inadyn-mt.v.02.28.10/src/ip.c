/*
Copyright (C) 2003-2004 Narcis Ilisei
Modifications by Bryan Hoover (bhoover@wecs.com)
Copyright (C) 2009 Bryan Hoover (bhoover@wecs.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
	Dyn Dns update main implementation file
	Author: narcis Ilisei
	Date: May 2003

	History:
      Sept. 2009
	  - Windows socket layer shutdown per startup
	  - Oct. 2009
	  - ip_initialize async
      Dec. 2010
	  - Added IPv6, IPv4 address parsing
*/
#define MODULE_TAG "IP: "

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <Iphlpapi.h>

#include "os.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "debug_if.h"
#include "ip.h"
#include "safe_mem.h"
#include "unicode_util.h"

#ifdef USE_THREADS

#include "threads_wrapper.h"

#ifdef HAVE_FUNC_GETHOSTBYNAME_R_NOT

#ifndef ASYNC_LOOKUP

static mutex_t	gethostname_mutex=MUTEX_T_INIT;

#endif

#endif

#endif

typedef struct
{
	int		fam_val;
	const char	*p_name;
} ADDR_FAMILY_NAME;

static const ADDR_FAMILY_NAME global_addrfamily_table[]  =
{
	{AF_INET,"AF_INET"},
	{AF_INET6,"AF_INET6"},

	{0,NULL}
};

static const char* unknown_addr_fam = "address family not supported";

/*
	 basic resource allocations for the ip object
*/
RC_TYPE ip_construct(IP_SOCKET *p_self)
{
        if (p_self == NULL)
        {
                return RC_INVALID_POINTER;
        }

        if (p_self->is_constructed)
        {
                return RC_OK;
        }

        memset(p_self, 0, sizeof(IP_SOCKET));

        p_self->initialized = FALSE;
        p_self->socket = safe_malloc(sizeof(SOCKET));
        p_self->socket[0]=INVALID_SOCKET;
        p_self->addr_ar = safe_malloc(sizeof(struct addrinfo *));
        p_self->addr_ar[0]=NULL;
        p_self->s_src_addr = safe_malloc(sizeof(char *));
	p_self->s_src_addr[0]=NULL;
        p_self->p_net_addr = safe_malloc(sizeof(struct sockaddr_storage *));
        p_self->p_net_addr[0]=NULL;
        p_self->server_socket_count=1;
        memset( &p_self->remote_addr, 0,sizeof(p_self->remote_addr));
        p_self->timeout = IP_DEFAULT_TIMEOUT;

        p_self->is_constructed=TRUE;


        return RC_OK;
}

/*
	This is not a "complete" clone.  Take care to see what is/isn't coppied.
*/
RC_TYPE ip_clone(IP_SOCKET *p_self_dest,IP_SOCKET *p_self_src)
{

	if (!(p_self_dest))

		return RC_INVALID_POINTER;

	if (!(p_self_src))

		return RC_INVALID_POINTER;

	ip_construct(p_self_dest);

	p_self_dest->p_remote_host_name=p_self_src->p_remote_host_name;
	p_self_dest->port=p_self_src->port;
	p_self_dest->timeout=p_self_src->timeout;
	p_self_dest->initialized=0;


	return RC_OK;
}

/*
	Resource free.
*/
RC_TYPE ip_destruct(IP_SOCKET *p_self)
{

	if (p_self == NULL)
	{
		return RC_OK;
	}

	if (!(p_self->is_constructed))
	{
		return RC_OK;
	}

	ip_shutdown(p_self);

	free(p_self->socket);
	free(p_self->addr_ar);

        free(p_self->p_net_addr);
        free(p_self->s_src_addr);

        if (p_self->if_name) {

                free(p_self->if_name);
                p_self->if_name=NULL;
        }

        memset(&p_self->addr_pref,0,sizeof(IPV6_PRIV_EXT));

	memset(p_self,0,sizeof(IP_SOCKET));


	return RC_OK;
}

#ifdef _WIN32

static int get_adapters_addresses_cb(FARPROC pf_GetAdaptersAddresses,PIP_ADAPTER_ADDRESSES adapterAddresses,int *outBufLen)
{

	return pf_GetAdaptersAddresses(AF_UNSPEC,GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER |
					GAA_FLAG_SKIP_MULTICAST,NULL,adapterAddresses,outBufLen);
}

int get_adapters_addresses(PIP_ADAPTER_ADDRESSES *adapterAddresses)
{

	return get_win_api_data("iphlpapi","GetAdaptersAddresses",get_adapters_addresses_cb,adapterAddresses);
}

static int get_adapters_info_cb(FARPROC pf_GetAdaptersInfo,PIP_ADAPTER_INFO adapterInfo,int *outBufLen)
{

	return pf_GetAdaptersInfo(adapterInfo,outBufLen);
}

int get_adapters_info(PIP_ADAPTER_INFO *adapterInfo)
{

	return get_win_api_data("iphlpapi","GetAdaptersInfo",get_adapters_info_cb,adapterInfo);
}

#endif

/*wrap getifaddrs for Windows, earlier Solaris.
*/
int getifaddrs_w(struct ifaddrs_w** in_ifaddrs)
{

#if defined(HAVE_IFADDRS_H) && !(defined(__sun) && defined(__SVR4))

	return getifaddrs(in_ifaddrs);
#else
	*in_ifaddrs=NULL;

  #ifndef _WIN32

	errno=ENOSYS;

	return -1;

  #else
	/*address family AF_UNSPEC or AF_INET6 needs at LEAST Windows XP sp1 (use isWinNT, isWinXPsp1)*/

	/*not at least windows xp sp1?*/
	if (!(isWinXPsp1())) {

		PIP_ADAPTER_INFO	adapterInfo=NULL;
		PIP_ADAPTER_INFO	pAdapterInfo;
		struct ifaddrs_w	*p_ifaddrs;
		IP_ADDR_STRING		*p_IpAddressList;

		if (!(0==get_adapters_info(&adapterInfo)))

			return -1;

		if (!(adapterInfo))

			return 0;

		*in_ifaddrs=safe_malloc(sizeof(ifaddrs_w));

		p_ifaddrs=*in_ifaddrs;

		for (pAdapterInfo=adapterInfo;pAdapterInfo;pAdapterInfo=pAdapterInfo->Next) {

			p_IpAddressList=&pAdapterInfo->IpAddressList;

			do {
				p_ifaddrs->ifa_name=safe_malloc(strlen(pAdapterInfo->Description)+1);
				p_ifaddrs->ifa_name=strcpy(p_ifaddrs->ifa_name,pAdapterInfo->Description);

				p_ifaddrs->ifa_addr=safe_malloc(sizeof(struct sockaddr_in));
				p_ifaddrs->ifa_addr->sa_family=AF_INET;
				inet_pton_w(AF_INET,(const char *) p_IpAddressList->IpAddress.String,&((struct sockaddr_in *) p_ifaddrs->ifa_addr)->sin_addr);

				if (!(p_IpAddressList->Next || pAdapterInfo->Next)) {

					break;
				}
				else {
					p_ifaddrs->ifa_next=safe_malloc(sizeof(ifaddrs_w));
					p_ifaddrs=p_ifaddrs->ifa_next;

					p_IpAddressList=p_IpAddressList->Next;
				}
			}
			while (p_IpAddressList);
		}

		return 0;
	}
	else {
		PIP_ADAPTER_ADDRESSES			adapterAddresses=NULL;
		PIP_ADAPTER_ADDRESSES			pAdapterAddresses;
		IP_ADAPTER_UNICAST_ADDRESS		*pUnicastAddress;
		struct ifaddrs_w			*p_ifaddrs;


		if (!(0==get_adapters_addresses(&adapterAddresses)))

			return -1;

		if (!(adapterAddresses))

			return 0;

		*in_ifaddrs=safe_malloc(sizeof(ifaddrs_w));

		p_ifaddrs=*in_ifaddrs;

		for (pAdapterAddresses=adapterAddresses;pAdapterAddresses;pAdapterAddresses=pAdapterAddresses->Next) {

			pUnicastAddress=pAdapterAddresses->FirstUnicastAddress;

			do {
				if (pUnicastAddress) {

					p_ifaddrs->ifa_addr=safe_malloc(sizeof(struct sockaddr_storage));

					if (!(AF_INET6==pUnicastAddress->Address.lpSockaddr->sa_family))

						memcpy(p_ifaddrs->ifa_addr,pUnicastAddress->Address.lpSockaddr,sizeof(struct sockaddr_in));
					else
						memcpy(p_ifaddrs->ifa_addr,pUnicastAddress->Address.lpSockaddr,sizeof(struct sockaddr_in6));

					p_ifaddrs->ifa_name=utf_16_to_8(utf_malloc_16_to_8(&p_ifaddrs->ifa_name,pAdapterAddresses->FriendlyName),pAdapterAddresses->FriendlyName);


					pUnicastAddress=pUnicastAddress->Next;
				}

				if (!(pAdapterAddresses->Next || pUnicastAddress))

					break;

				if (!(pUnicastAddress))

					if (!(pAdapterAddresses->Next->FirstUnicastAddress))

						break;

				p_ifaddrs->ifa_next=safe_malloc(sizeof(ifaddrs_w));
				p_ifaddrs=p_ifaddrs->ifa_next;
			}
			while (pUnicastAddress);
		}

		free(adapterAddresses);

		if (!((*in_ifaddrs))->ifa_name) {

			freeifaddrs_w(*in_ifaddrs);

			*in_ifaddrs=NULL;
		}

		return 0;
	}

  #endif
#endif

}

int freeifaddrs_w(struct ifaddrs_w* in_ifaddrs)
{

#if defined(HAVE_IFADDRS_H) && !(defined(__sun) && defined(__SVR4))

	freeifaddrs(in_ifaddrs);
#else
  #ifndef _WIN32

	errno=ENOSYS;

	return -1;
  #else

	if (in_ifaddrs) {

		struct ifaddrs_w	*p_in_ifaddrs=in_ifaddrs;

		do {

			free(p_in_ifaddrs->ifa_name);
			free(p_in_ifaddrs->ifa_addr);

			p_in_ifaddrs=p_in_ifaddrs->ifa_next;
		}
		while (p_in_ifaddrs);

		free(in_ifaddrs);
	}
  #endif
#endif

	return 0;
}

/*roll your own inet_ntop for legacy Windows.  DOES NOT double colon compress leading zeros -- excepting ip4 mapped addresses.

 Note, turns out, Windows 2000 (earliest ipv6 support) has equivalent functions, InetNtop, InetPton.  Perhaps I'll implement ipv6 on Win98.
 :) -- well, maybe just for SE.

 Be sure and stay tuned for more great function comments, anecdotes, and various and sundry explanations regarding input/output parameters!
*/
const char* inet_ntop_w(int addr_family,const void* net_addr_in,char* s_out_addr,int buff_len)
{
	unsigned char*	addr8_bytes=(unsigned char*) net_addr_in;
	unsigned short*	addr16_bytes=(unsigned short*) net_addr_in;
	int		ret=-1;
	int		i=0;


	if (addr_family==AF_INET)

		ret=sprintf(s_out_addr,"%i.%i.%i.%i",addr8_bytes[0],addr8_bytes[1],addr8_bytes[2],addr8_bytes[3]);

	if (addr_family==AF_INET6) {

		while (!(addr8_bytes[++i]));

		/*ip4 mapped addresses*/
		if (i==12)

			ret=sprintf(s_out_addr,"::%i.%i.%i.%i",addr8_bytes[12],addr8_bytes[13],addr8_bytes[14],addr8_bytes[15]);
		else

			if (i==10)

				if (addr8_bytes[10]==0xff && addr8_bytes[11]==0xff) {

					ret=sprintf(s_out_addr,"::%x%x:%i.%i.%i.%i",addr8_bytes[10],addr8_bytes[11],addr8_bytes[12],
						addr8_bytes[13],addr8_bytes[14],addr8_bytes[15]);
				}

		/*normal ip6 address*/
		if (ret<0)

			ret=sprintf(s_out_addr,"%x:%x:%x:%x:%x:%x:%x:%x",ntohs(addr16_bytes[0]),ntohs(addr16_bytes[1]),ntohs(addr16_bytes[2]),
				ntohs(addr16_bytes[3]),ntohs(addr16_bytes[4]),ntohs(addr16_bytes[5]),ntohs(addr16_bytes[6]),
				ntohs(addr16_bytes[7]));

	}

	if (ret<0)

		return NULL;

	return s_out_addr;
}

/*inet_pton_w aux function.
*/
static void fix_up_leading_zeros(int family,unsigned char* addr8_bytes,int zeros_index,int out_index)
{

        int     move_len;
        int     src_pos;
        int     dest_pos=15;
        int     zeros_end;
	int	i;

        src_pos=out_index;
        move_len=15-out_index;
        zeros_end=zeros_index+move_len;

        while (dest_pos>=zeros_end) {

                addr8_bytes[dest_pos]=addr8_bytes[src_pos];

                dest_pos--;
                src_pos--;
        }

	for (i=zeros_index;i<zeros_end;i++)

		addr8_bytes[i]=0;
}


/*Can't have one without the other.
*/
int inet_pton_w(int family, const char *strptr, void *addrptr)
{

	unsigned char*	addr8_bytes=((unsigned char*) addrptr);
	unsigned char	c_hex;
	int		exp=10;
	int		in_index=0;
	int		out_index=0;
	int		is_first_nibble=0;
	int		absent_zeros=0;
	int		is_first_digit=1;
	char*		strpbrk_in;
	char*		p_strpbrk_in;
	char*		hex_delim;
	char*		ip4_mapped_pos=0;
	int		zeros_index=0;


	if (!(strptr) || !(addrptr)) {

		errno=EINVAL;

		return -1;
	}

	strpbrk_in=safe_malloc(strlen(strptr)+1);
	strcpy(strpbrk_in,strptr);
	p_strpbrk_in=strpbrk_in;

	if (family==AF_INET) {

		memset(addrptr,0,sizeof(struct in_addr));
	}
	else {
		memset(addrptr,0,sizeof(struct in6_addr));

		/*account for absent leading zeros*/
		hex_delim=strpbrk(p_strpbrk_in,":");
		absent_zeros=4-(hex_delim-strpbrk_in);
		p_strpbrk_in=hex_delim+1;

		exp=16;

		/*detect ip4 mapped address*/
		ip4_mapped_pos=strpbrk(p_strpbrk_in,".");
	}

	while (strptr[in_index]) {

		if (strptr[in_index]=='.' || strptr[in_index]==':') {

			if (strptr[in_index]==':') {

				if (ip4_mapped_pos)

					/*switch to ipv4 mode*/
					if (ip4_mapped_pos-3<=p_strpbrk_in) {

						family=AF_INET;
						exp=10;

						out_index--;
					}
			}

			if (!(hex_delim=strpbrk(p_strpbrk_in,":"))) {

				absent_zeros=4-strlen(p_strpbrk_in);
			}
			else {

				absent_zeros=4-(hex_delim-p_strpbrk_in);
				p_strpbrk_in=hex_delim+1;

				/*double colon*/
				if (absent_zeros==4)

					/*ipv4 mapped, or address with leading zeros*/
					if (out_index==0)

						/*actually 0, but need to flag it*/
						zeros_index=1;

					else
						zeros_index=out_index;
			}

			in_index++;
			is_first_digit=1;

			if (family==AF_INET)

				out_index++;

			continue;
		}

		if (is_first_digit) {

			if (family==AF_INET)

				is_first_digit=0;
		}
		else
			addr8_bytes[out_index]*=exp;


		if (family==AF_INET) {

			addr8_bytes[out_index]=addr8_bytes[out_index]+strptr[in_index]-'0';
		}
		else {

			if (absent_zeros) {

				if (absent_zeros==1)

					is_first_nibble=1;

				if (absent_zeros==2)

					out_index+=1;

				if (absent_zeros==3) {

					out_index+=1;
					is_first_nibble=1;
				}

				absent_zeros=0;
			}

			c_hex=strptr[in_index];

			if (strptr[in_index]>'9')

				c_hex=tolower(strptr[in_index]);

			if (c_hex>='a') {

				addr8_bytes[out_index]=addr8_bytes[out_index]+c_hex-87;
			}
			else {

				addr8_bytes[out_index]=addr8_bytes[out_index]+c_hex-'0';
			}

			if (!(is_first_nibble)) {

				is_first_nibble=1;
				is_first_digit=0;
			}
			else {

				is_first_nibble=0;
				is_first_digit=1;
				out_index++;
			}
		}

		in_index++;
	}

	free(strpbrk_in);

	if (zeros_index) {

		/*just needed > 0 to flag leading zeros*/
		if (zeros_index==1)

			zeros_index=0;

		if (family==AF_INET6)

			out_index--;	/*'cause get's incremented, before loop exit test*/

		fix_up_leading_zeros(family,addr8_bytes,zeros_index,out_index);
	}

	return 0;
}

/*return true if is a global address.
*/
int is_ip6_global(struct in6_addr* s_address)
{

	unsigned char	ch_test=((char*) s_address)[0];


	return ((ch_test>>4)==2);
}

int is_ip6_tmp(struct in6_addr* s_address)
{

	unsigned char	ch_test=((char*) s_address)[8];


	return !((ch_test & 2)==2);
}

/*return true if is a global address, and it's public (priv extentions address
  preference parlance) - assumes MAC address, fffe type format.
*/
int is_ip6_public(struct in6_addr* s_address)
{

	if (!(is_ip6_global(s_address)))

		return 0;

	return !(is_ip6_tmp(s_address));
}

/*return true if s_address is in accordance with addr_pref
*/
int is_addr_pref(struct in6_addr* s_address,unsigned addr_pref)
{

	/*no addr pref specified -- short of
	  system policy, return global temp
	*/
	if (!(addr_pref))

		return (is_ip6_global(s_address) && is_ip6_tmp(s_address));

	if (addr_pref==IPV6_PREFER_SRC_PUBLIC)

		return is_ip6_public(s_address);

	if (addr_pref==IPV6_PREFER_SRC_PUBTMP_DEFAULT || addr_pref==IPV6_PREFER_SRC_TMP)

		return (is_ip6_global(s_address) && is_ip6_tmp(s_address));

/*
	if (addr_pref==IPV6_PREFER_SRC_PUBTMP_DEFAULT)

		return (is_ip6_global(s_address) && is_ip6_tmp(s_address));
*/
	return 0;
}

static struct sockaddr_storage* do_if_address(char* if_name,int addr_family,struct IPV6_PRIV_EXT* addr_pref)
{
	struct ifaddrs_w	*ifaddr;
	struct sockaddr_storage	*ret_sockaddrs=NULL;


	if (getifaddrs_w(&ifaddr)) {

		log_errno(errno,LOG_ERR, "E:" MODULE_TAG "getifaddrs returned error in if_address:  %s...\n");
	}
	else {

		struct ifaddrs_w	*ifa;
		int			family;

		for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {

			if (ifa->ifa_addr == NULL)
				continue;

			family = ifa->ifa_addr->sa_family;

			/*if (family == AF_INET || family == AF_INET6) {*/
			if (family == addr_family) {

				int	sock_size=sizeof(struct sockaddr_in);
				int	is_select_addr;

				if (!(is_select_addr=(!(if_name))))

					is_select_addr=(0==strcmp(ifa->ifa_name,if_name));

				if (is_select_addr) {

					int	is_sellection=1;

					if (family==AF_INET6) {

						if (is_sellection=is_addr_pref((&(((struct sockaddr_in6*) ifa->ifa_addr)->sin6_addr)),addr_pref->flags)) {

							sock_size=sizeof(struct sockaddr_in6);

							if (!(addr_pref->flags))

								DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "addr pref,'policy' specified.  Assuming addr pref, global temporary in if_addr...\n"));

						}
					}

					if (is_sellection) {

						ret_sockaddrs=safe_malloc(sock_size);
						memcpy(ret_sockaddrs,ifa->ifa_addr,sock_size);

						break;
					}
				}
			}
		}

		freeifaddrs_w(ifaddr);
	}

	if (!(ret_sockaddrs))

		if (addr_family==AF_INET6)

			if (addr_pref->flags)

				if (!(if_name))

					DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "priv ext address selection '%s',  either not found, or yet supported...\n",addr_pref->addr_pref));

				else
					DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Specific device, '%s', priv ext address selection '%s',  either not found, or yet supported...\n",if_name,addr_pref->addr_pref));

	return ret_sockaddrs;
}

struct sockaddr_storage* if_address(char* if_name,int addr_family,struct IPV6_PRIV_EXT* addr_pref)
{

	if (!(if_name)) {

		DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "if_name null, in if_address...\n"));

		return NULL;
	}

	return do_if_address(if_name,addr_family,addr_pref);
}

static struct sockaddr_storage* if_address2(char* if_name,int addr_family,struct IPV6_PRIV_EXT* addr_pref)
{

#ifndef _WIN32

	if (!(if_name)) {
#else
	/*windows ip6 - if no if_name, bind to first
	  address corresponding to addr_pref.
	*/
	if (!(if_name || AF_INET6==addr_family)) {
#endif
		DBG_PRINTF((LOG_DEBUG,"D:" MODULE_TAG "if_name null, in if_address...\n"));

		return NULL;
	}

	return do_if_address(if_name,addr_family,addr_pref);
}

static int do_do_if_bind(struct sockaddr_storage *p_bind_in,SOCKET socket,struct sockaddr_storage **p_net_addr,char **s_src_addr,char *if_name,int ai_family,IPV6_PRIV_EXT addr_pref)
{
	int				sock_size=sizeof(struct sockaddr_in);
	int				addr_family;
	char				s_binded_addr[INET6_ADDRSTRLEN];
	void*				in_addr;
	int				bind_ret;

	addr_family=ai_family;

	memset(s_binded_addr,0,INET6_ADDRSTRLEN);


	if (addr_family==AF_INET) {

		in_addr=&(((struct sockaddr_in*) p_bind_in)->sin_addr);
	}
	else {

		in_addr=&(((struct sockaddr_in6*) p_bind_in)->sin6_addr);

		sock_size=sizeof(struct sockaddr_in6);
	}

	if (!(inet_ntop_w(addr_family,in_addr,s_binded_addr,INET6_ADDRSTRLEN)))

		log_errno(errno,LOG_ERR,"E:" MODULE_TAG "inet_ntop returned error:  %s, in if_bind...\n");

	if ((0==(bind_ret=bind(socket,(struct sockaddr*) p_bind_in,sock_size)))) {

		*p_net_addr=p_bind_in;

		*s_src_addr=safe_malloc(strlen(s_binded_addr)+1);
		strcpy(*s_src_addr,s_binded_addr);
	}

	return bind_ret;
}

static log_bind_result(int bind_ret,char *if_name,char *s_binded_addr)
{

	if (0==bind_ret) {

		if (!(if_name))

			DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "Binded socket to local interface:  %s, in if_bind...\n",s_binded_addr));
		else
			DBG_PRINTF((LOG_NOTICE,"N:" MODULE_TAG "Binded socket to local interface:  %s, address:  %s, in if_bind...\n",if_name,s_binded_addr));
	}
	else {
		char*	s_err=str_errno(errno);

		if (!(if_name))

			DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Bind returned error:  %s, in if_bind...\n",s_err));
		else
			DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Bind returned error:  %s, in if_bind, attempting bind to interface:  %s...\n",s_err,if_name));

		free(s_err);
	}
}

static RC_TYPE do_if_bind(SOCKET socket,struct sockaddr_storage **p_net_addr,char **s_src_addr,char *if_name,int ai_family,IPV6_PRIV_EXT addr_pref)
{
	struct sockaddr_storage*	p_bind_in;
	int				bind_ret;

	if (!(p_bind_in=if_address(if_name,ai_family,&addr_pref))) {

		return RC_ERROR;
	}
	else {
		bind_ret=do_do_if_bind(p_bind_in,socket,p_net_addr,s_src_addr,if_name,ai_family,addr_pref);

		log_bind_result(bind_ret,if_name,*s_src_addr);
	}

	return RC_OK;
}

static RC_TYPE do_if_bind2(SOCKET socket,char *if_name,int ai_family,IPV6_PRIV_EXT addr_pref)
{
	struct sockaddr_storage*	p_bind_in;
	int				bind_ret;
	char				*s_src_addr=NULL;
	struct sockaddr_storage		*p_net_addr_dummy=NULL;


	if (!(p_bind_in=if_address2(if_name,ai_family,&addr_pref))) {

		return RC_ERROR;
	}
	else {
		bind_ret=do_do_if_bind(p_bind_in,socket,&p_net_addr_dummy,&s_src_addr,if_name,ai_family,addr_pref);

		log_bind_result(bind_ret,if_name,s_src_addr);

		free(s_src_addr);
		free(p_net_addr_dummy);
	}

	return RC_OK;
}

RC_TYPE if_addr_pref_bind(SOCKET socket,char *if_name,IPV6_PRIV_EXT addr_pref)
{
	RC_TYPE			rc;

	rc=do_if_bind2(socket,if_name,AF_INET6,addr_pref);

	return rc;
}

RC_TYPE if_bind(IP_SOCKET *p_self,int socket_index)
{

	return do_if_bind(p_self->socket[socket_index],&p_self->p_net_addr[socket_index],&p_self->s_src_addr[socket_index],p_self->if_name,
				p_self->addr_ar[socket_index]->ai_family,p_self->addr_pref);
}

RC_TYPE do_ip_set_addr_pref(SOCKET socket,char *if_name,IPV6_PRIV_EXT addr_pref)
{
	RC_TYPE	rc;

#ifdef _WIN32

	rc=if_addr_pref_bind(socket,if_name,addr_pref);

#else
	if (0==setsockopt(socket,IPPROTO_IPV6,IPV6_ADDR_PREFERENCES,(const char *) &addr_pref.flags,sizeof(unsigned)))

		rc=RC_OK;
	else
		rc=RC_ERROR;
#endif
	if (!(RC_OK==rc))

		log_errno(errno,LOG_ERR, "E:" MODULE_TAG "Error setting ipv6 address preference in do_ip_set_sock_addr_pref:  %s...\n");

	return rc;
}

RC_TYPE ip_set_addr_pref(IP_SOCKET *p_self)
{

	return do_ip_set_addr_pref(p_self->socket[p_self->sock_index],p_self->if_name,p_self->addr_pref);
}

RC_TYPE ip_initialize(IP_SOCKET *p_self)
{
	RC_TYPE	rc=RC_OK;
	struct 	addrinfo *result=NULL;
	struct	addrinfo hints;
	int	resolv_ret=0;
	char	port_str[128];
	BOOL	is_ip_support_ok;


	if (p_self->initialized==TRUE) {

		return RC_OK;
	}

	do
	{
		if (!(is_ip_support_ok=(RC_OK==(rc=os_ip_support_startup()))))

			break;


		memset(&hints, 0,sizeof(struct addrinfo));

		if (!(p_self->is_ipv4 || p_self->is_ipv6) || (p_self->is_ipv4 && p_self->is_ipv6))
		{
			//hints.ai_family=AF_UNSPEC;
			hints.ai_family=AF_INET;
		}
		else
		{
			if (p_self->is_ipv4)
			{
				hints.ai_family=AF_INET;
			}
			else
			{
				hints.ai_family=AF_INET6;
			}
		}
		hints.ai_socktype=SOCK_STREAM;
		hints.ai_protocol=IPPROTO_TCP;
		hints.ai_flags=AI_CANONNAME;


		/*remote addres */
		if (p_self->p_remote_host_name==NULL) {

			rc=RC_INVALID_POINTER;
		}
		else {

#ifndef _WIN32
			snprintf(port_str,sizeof(port_str),"%d",p_self->port);
#else
			_snprintf(port_str,sizeof(port_str),"%d",p_self->port);
#endif

			if (!(resolv_ret=getaddrinfo(p_self->p_remote_host_name,port_str,&hints,&result))) {

				p_self->addr=result;
			}
			else {

				DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Error 0x%x resolving host name '%s'\n",
				            os_get_socket_error(),p_self->p_remote_host_name));

				rc=RC_IP_INVALID_REMOTE_ADDR;

				break;
			}
		}
	}
	while(0);

	if (rc == RC_OK) {

		p_self->initialized=TRUE;
	}
	else {
		if (is_ip_support_ok)

			os_ip_support_cleanup();

		ip_shutdown(p_self);
	}

	return rc;
}

/*	Disconnect and some other clean up.
*/
RC_TYPE ip_shutdown(IP_SOCKET *p_self)
{
	int	i;


	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (!p_self->initialized)
	{
		return RC_OK;
	}

	/*
		close them, don't dealloc; socket are created
		with realloc so no mem leak, and must always
		have at least one allocated -- implemetation
		migrated to socket list from single static
	*/
	for (i=0;i<p_self->server_socket_count;i++) {

		if (!(p_self->socket[i]==INVALID_SOCKET)) {

			if (!(closesocket(p_self->socket[i])==0)) {

				DBG_PRINTF((LOG_CRIT,"C:" MODULE_TAG "closesocket returned error 0x%x in ip_shutdown...\n", os_get_socket_error()));
			}

			free(p_self->p_net_addr[i]);
			p_self->p_net_addr[i]=NULL;
			free(p_self->s_src_addr[i]);
			p_self->s_src_addr[i]=NULL;
		}
	}

	p_self->socket=safe_realloc(p_self->socket,sizeof(SOCKET));
	p_self->socket[0]=INVALID_SOCKET;

	p_self->server_socket_count=1;
	p_self->sock_index=0;

	if (p_self->addr) {

		freeaddrinfo(p_self->addr);

		p_self->addr=NULL;
	}

	os_ip_support_cleanup();

	p_self->initialized = FALSE;

	return RC_OK;
}

RC_TYPE ip_send(IP_SOCKET *p_self, const char *p_buf, int len)
{
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (!p_self->initialized)
	{
		return RC_IP_OBJECT_NOT_INITIALIZED;
	}

	if ((p_self->socket[p_self->sock_index]==INVALID_SOCKET))
	{
		return RC_IP_INVALID_SOCKET;
	}

	if( send(p_self->socket[p_self->sock_index], (char*) p_buf, len, 0) == SOCKET_ERROR )
	{
		DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Error 0x%x in send()\n", os_get_socket_error()));
		return RC_IP_SEND_ERROR;
	}
	return RC_OK;
}

/*
	Receive data into user's buffer.
	return
		if the max len has been received
		if a timeout occures
	In p_recv_len the total number of bytes are returned.
	Note:
		if the recv_len is bigger than 0, no error is returned.
*/
RC_TYPE ip_recv(IP_SOCKET *p_self, char *p_buf, int max_recv_len, int *p_recv_len)
{
	RC_TYPE	rc = RC_OK;
	int	remaining_buf_len = max_recv_len;
	int	total_recv_len = 0;
	int	recv_len = 0;

	if (p_self == NULL || p_buf == NULL || p_recv_len == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (!p_self->initialized)
	{
		return RC_IP_OBJECT_NOT_INITIALIZED;
	}

	if ((p_self->socket[p_self->sock_index]==INVALID_SOCKET))
	{
		return RC_IP_INVALID_SOCKET;
	}

	while (remaining_buf_len > 0)
	{
		int chunk_size = remaining_buf_len > IP_DEFAULT_READ_CHUNK_SIZE ?
		                 IP_DEFAULT_READ_CHUNK_SIZE : remaining_buf_len;

		recv_len = recv(p_self->socket[p_self->sock_index], p_buf + total_recv_len, chunk_size, 0);

		if (recv_len < 0)
		{

			{
				DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Error 0x%x in recv()\n", os_get_socket_error()));
				rc = RC_IP_RECV_ERROR;
			}
			break;
		}

		if (recv_len == 0)
		{
			if (total_recv_len == 0)
			{
				rc = RC_IP_RECV_ERROR;
			}
			break;
		}

		total_recv_len += recv_len;
		remaining_buf_len = max_recv_len - total_recv_len;
	}


	*p_recv_len = total_recv_len;
	return rc;
}


/*Accessors */
RC_TYPE ip_set_if_name(IP_SOCKET *p_self,char *if_name)
{

	if (!(p_self))

		return RC_INVALID_POINTER;

	if (!(if_name))

		return RC_INVALID_POINTER;

	p_self->if_name=safe_realloc(p_self->if_name,strlen(if_name)+1);
	strcpy(p_self->if_name,if_name);


	return RC_OK;
}

char* ip_get_if_name(IP_SOCKET *p_self,char **if_name)
{

	if (!(p_self))

		return NULL;

	if (!(*if_name))

		return NULL;

	*if_name=safe_malloc(strlen(p_self->if_name)+1);
	strcpy(*if_name,p_self->if_name);

	return *if_name;
}

RC_TYPE ip_set_priv_ext(IP_SOCKET *p_self,IPV6_PRIV_EXT addr_pref)
{

        if (!(p_self))

                return RC_INVALID_POINTER;

	memcpy(&p_self->addr_pref,&addr_pref,sizeof(IPV6_PRIV_EXT));

	return RC_OK;
}

IPV6_PRIV_EXT* ip_get_priv_ext(IP_SOCKET *p_self,IPV6_PRIV_EXT *addr_pref)
{

        if (p_self)

		memcpy(addr_pref,&p_self->addr_pref,sizeof(IPV6_PRIV_EXT));
	else

		DBG_PRINTF((LOG_WARNING, "W:" MODULE_TAG "p_self is null in ip_get_priv_ext.  Returning untouched IPV6_PRIV_EXT param...\n"));

	return addr_pref;
}

RC_TYPE ip_set_port(IP_SOCKET *p_self, int p)
{
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	if (p < 0 || p > IP_SOCKET_MAX_PORT)
	{
		return RC_IP_BAD_PARAMETER;
	}
	p_self->port = p;
	return RC_OK;
}

RC_TYPE ip_set_remote_name(IP_SOCKET *p_self, const char* p)
{
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}
	p_self->p_remote_host_name = p;
	return RC_OK;
}

RC_TYPE ip_set_remote_timeout(IP_SOCKET *p_self, int t)
{
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}
	p_self->timeout = t;
	return RC_OK;
}

RC_TYPE ip_set_is_ipv4(IP_SOCKET *p_self, BOOL is_ipv4)
{
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	p_self->is_ipv4=is_ipv4;

	return RC_OK;
}

RC_TYPE ip_set_is_ipv6(IP_SOCKET *p_self, BOOL is_ipv6)
{
        if (p_self == NULL)
        {
                return RC_INVALID_POINTER;
        }

        p_self->is_ipv6=is_ipv6;

        return RC_OK;
}

RC_TYPE ip_get_port(IP_SOCKET *p_self, int *p_port)
{
	if (p_self == NULL || p_port == NULL)
	{
		return RC_INVALID_POINTER;
	}
	*p_port = p_self->port;
	return RC_OK;
}

RC_TYPE ip_get_remote_name(IP_SOCKET *p_self, const char* *p)
{
	if (p_self == NULL || p == NULL)
	{
		return RC_INVALID_POINTER;
	}
	*p = p_self->p_remote_host_name;
	return RC_OK;
}

RC_TYPE ip_get_remote_timeout(IP_SOCKET *p_self, int *p)
{
	if (p_self == NULL || p == NULL)
	{
		return RC_INVALID_POINTER;
	}
	*p = p_self->timeout;
	return RC_OK;
}

RC_TYPE ip_get_is_ipv4(IP_SOCKET *p_self, BOOL *is_ipv4)
{
	if (p_self == NULL)
	{
		return RC_INVALID_POINTER;
	}

	*is_ipv4=p_self->is_ipv4;

	return RC_OK;
}

RC_TYPE ip_get_is_ipv6(IP_SOCKET *p_self, BOOL *is_ipv6)
{
        if (p_self == NULL)
        {
                return RC_INVALID_POINTER;
        }

        *is_ipv6=p_self->is_ipv6;

        return RC_OK;
}

const char* addr_family_get_name(int in_val)
{
	const ADDR_FAMILY_NAME *it = global_addrfamily_table;

	while (it->p_name != NULL)
	{
		if (it->fam_val == in_val)
		{
			return it->p_name;
		}
		++it;
	}

	return unknown_addr_fam;
}

static RC_TYPE push_in_buffer(char* p_src, int src_len, char *p_buffer, int* p_act_len, int max_len)
{
	if (*p_act_len + src_len > max_len)
	{
		return RC_FILE_IO_OUT_OF_BUFFER;
	}
	memcpy(p_buffer + *p_act_len,p_src, src_len);
	*p_act_len += src_len;
	return RC_OK;
}

RC_TYPE ip_parser_init(IP_FILE_PARSER *p_cfg, FILE *p_file)
{
	memset(p_cfg, 0, sizeof(*p_cfg));

	p_cfg->state = NEW_LINE_IP;
	p_cfg->p_file = p_file;

	return RC_OK;
}

/*
	IP string parse, sans regex.

    Not fully error checked (for instance, could miss hex digits in IPv4 format, and there is no
	range checking), but shouldn't be a problem with reasonably sane input (I don't imagine ip servers
	are trying to trick clients :-)).
*/
RC_TYPE ip_parser_read(IP_FILE_PARSER *p_cfg,char *p_buffer,int maxlen,char *ch,int *count,int *parse_end)
{

	#define HEX_DIGITS	"0123456789aAbBcCdDeEfF"
	#define DEC_DIGITS	"0123456789"

	RC_TYPE rc = RC_OK;


	switch (p_cfg->state)
	{
	case NEW_LINE_IP:

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=NEWLINE_COLON;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=NUMERIC;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case NEWLINE_COLON:

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=DBL_COLON_DETECT_A;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case NUMERIC:

		if (strpbrk(ch, HEX_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,".")))
		{
			p_cfg->state=DOT_1;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=COLON_DETECT_1;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case DOT_1:

		if (strpbrk(ch, DEC_DIGITS))
		{
			p_cfg->state=IPV4_NUMERIC_2;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case DOT_2:

		if (strpbrk(ch, DEC_DIGITS))
		{
			p_cfg->state=IPV4_NUMERIC_3;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case DOT_3:

		if (strpbrk(ch, DEC_DIGITS))
		{
			p_cfg->state=IPV4_NUMERIC_4;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV4_NUMERIC_2:

		if (strpbrk(ch, DEC_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,".")))
		{
			p_cfg->state=DOT_2;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV4_NUMERIC_3:

		if (strpbrk(ch, DEC_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,".")))
		{
			p_cfg->state=DOT_3;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV4_NUMERIC_4:

		if (strpbrk(ch, DEC_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		*parse_end=1;

		return rc;

	case COLON_DETECT_1:

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=IPV6_NUMERIC_2;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=DBL_COLON_DETECT_B;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case COLON_DETECT_2:

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=IPV6_NUMERIC_3;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=DBL_COLON_DETECT_B;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case COLON_DETECT_3:

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=IPV6_NUMERIC_4;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=DBL_COLON_DETECT_B;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case COLON_DETECT_4:

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=IPV6_NUMERIC_5;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=DBL_COLON_DETECT_B;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case COLON_DETECT_5:

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=IPV6_NUMERIC_6;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=DBL_COLON_DETECT_B;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case COLON_DETECT_6:

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=IPV6_NUMERIC_7;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=DBL_COLON_DETECT_B;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case COLON_DETECT_7:

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=IPV6_NUMERIC_8;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			*parse_end=1;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV6_NUMERIC_2:

		if (strpbrk(ch, HEX_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=COLON_DETECT_2;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV6_NUMERIC_3:

		if (strpbrk(ch, HEX_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=COLON_DETECT_3;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV6_NUMERIC_4:

		if (strpbrk(ch, HEX_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=COLON_DETECT_4;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV6_NUMERIC_5:

		if (strpbrk(ch, HEX_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=COLON_DETECT_5;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV6_NUMERIC_6:

		if (strpbrk(ch, HEX_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=COLON_DETECT_6;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV6_NUMERIC_7:

		if (strpbrk(ch, HEX_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=COLON_DETECT_7;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,".")))
		{
			p_cfg->state=DOT_1;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV6_NUMERIC_8:

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=IPV6_FINAL;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		return RC_IP_PARSER_INVALID_IP;

	case IPV6_FINAL:

		if (strpbrk(ch, HEX_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		*parse_end=1;

		return rc;

	case DBL_COLON_A:

		if (strpbrk(ch, HEX_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=DBL_COLON_DETECT_A;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,".")))
		{
			p_cfg->state=DOT_1;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		*parse_end=1;

		return rc;

	case DBL_COLON_B:

		if (strpbrk(ch, HEX_DIGITS))
		{
			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			p_cfg->state=DBL_COLON_DETECT_B;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,".")))
		{
			return RC_IP_PARSER_INVALID_IP;
		}

		*parse_end=1;

		return rc;

	case DBL_COLON_DETECT_A:

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=DBL_COLON_A;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			return RC_IP_PARSER_INVALID_IP;
		}

		*parse_end=1;

		return rc;

	case DBL_COLON_DETECT_B:

		if (strpbrk(ch, HEX_DIGITS))
		{
			p_cfg->state=DBL_COLON_B;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}

		if (!(strcmp(ch,":")))
		{
			return RC_IP_PARSER_INVALID_IP;
		}

		*parse_end=1;

		return rc;

	default:

		return rc;
	}
}

/*	assume ip is in str_ip, and that any errors are because have yet
	to parse past preliminary characters.
*/
RC_TYPE parse_ip_address(char **str_ip,char *in_str)
{
	RC_TYPE			rc = RC_OK;
	int			parse_end = 0;
	int			count = 0;
	char			ch[2];
	IP_FILE_PARSER		p_cfg;
	char			*p_buffer;
	int			maxlen;

	/*	TODO:	range check IPv4 digit groups
	*/

	if (!(in_str))

		return RC_INVALID_POINTER;

	*str_ip=NULL;

	maxlen=strlen(in_str);

	p_buffer=safe_malloc(maxlen+1);

	*p_buffer='\0';

	ip_parser_init(&p_cfg,NULL);


	while(!(parse_end) && *in_str) {

		memset(ch,0,2);

		strncpy(ch,in_str++,1);

		if (!((rc=ip_parser_read(&p_cfg,p_buffer,maxlen,ch,&count,&parse_end))==RC_OK)) {

			memset(p_buffer,0,maxlen+1);

			count=0;

			ip_parser_init(&p_cfg,NULL);
		}
	}

	if (rc==RC_OK) {

		push_in_buffer("\0",1,p_buffer,&count,maxlen);

		*str_ip=safe_malloc(strlen(p_buffer)+1);

		strcpy(*str_ip,p_buffer);

		if (!(strcmp(*str_ip,"")))

			rc=RC_IP_PARSER_INVALID_IP;
	}

	free(p_buffer);

	return rc;
}

