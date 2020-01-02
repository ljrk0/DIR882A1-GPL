
/*
  * IPv6 CE-Router Test Debug:
  * 1. Add the codes of obtaining the result of DAD in kernel protocol stack.
  * 2017-08-24 --liushenghui
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <time.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
//#include <linux/ipv6.h>

#include "libnetlink.h"


#define _unused __attribute__((unused))

struct in6_ifreq {
	struct in6_addr ifr6_addr;
	u_int32_t ifr6_prefixlen;
	unsigned int ifr6_ifindex;
};

typedef struct match_cond_t
{
	struct in6_addr addr;
	unsigned int if_idx;
	int found;
}match_cond;

typedef enum
{
	IFADDRCONF_ADD,
	IFADDRCONF_DEL
} ifaddrconf_type_t;

//#define IFA_F_DADFAILED (0x08)

static int check_address_object(const struct sockaddr_nl *who ,
	struct nlmsghdr *n , void *arg);
static int check_address_status(const char *ifname ,
	const struct in6_addr *addr);

static int ifAddrConf(ifaddrconf_type_t iConfType,
			const char * pIfName,
			const struct in6_addr * pAddr,
			int iPrefixLen);

int ifAddrConf(ifaddrconf_type_t iConfType,
			const char * pIfName,
			const struct in6_addr * pAddr,
			int iPrefixLen)
{
	int iSockFd = -1;
	unsigned int iCmd;
	struct ifreq ifr;
	struct in6_ifreq req;

	if (!pIfName || !pAddr)
		return -1;

	switch (iConfType)
	{
		case IFADDRCONF_ADD:
			iCmd = SIOCSIFADDR;
			break;
		case IFADDRCONF_DEL:
			iCmd = SIOCDIFADDR;
			break;
		default:
			syslog(LOG_ERR, "Unknown config type!\n");
			return -1;
	}

	iSockFd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (iSockFd < 0)
		syslog(LOG_ERR, "socket fail: %s, errno=%d\n",
			strerror(errno), errno);

	memset(&ifr, '\0', sizeof (struct ifreq));
	snprintf(ifr.ifr_name, sizeof (ifr.ifr_name) - 1, "%s", pIfName);
	if (ioctl(iSockFd, SIOGIFINDEX, &ifr) < 0)
	{
		syslog(LOG_ERR, "ioctl fail: %s, errno=%d\n",
			strerror(errno), errno);
		close(iSockFd);
		return -1;
	}

	memcpy(&req.ifr6_addr, pAddr, sizeof (req.ifr6_addr));
	req.ifr6_prefixlen = iPrefixLen;
	req.ifr6_ifindex = ifr.ifr_ifindex;

	if (ioctl(iSockFd, iCmd, &req) < 0)
	{
		syslog(LOG_ERR, "ioctl fail: %s, errno=%d\n",
			strerror(errno), errno);
		close(iSockFd);
		return -1;
	}

	close(iSockFd);

	return 0;
}

/*
  * IPv6 CE-Router Test Debug:
  * 1. This function comes from wide-dhcpv6 at svn version 74760.
  * 2. To obtain the result of DAD in kernel protocol stack.
  * 2017-08-24 --liushenghui
*/
static int check_address_status(const char *ifname ,
							const struct in6_addr *addr)
{
	match_cond target;
	int result = 0;
	struct rtnl_handle rth;

	memset(&target , 0 , sizeof(target));
	//prepare our target object
	memcpy(target.addr.s6_addr , addr->s6_addr , 16);
	target.if_idx = if_nametoindex(ifname);
	if(target.if_idx == 0)
	{
		result = -ENODATA;
		goto out;
	}

	if((result = rtnl_open(&rth , 0)) < 0)
		goto out;

	if((result = rtnl_wilddump_request(&rth, AF_INET6, RTM_GETADDR)) < 0)
		goto out_close_connection;

	if((result = rtnl_dump_filter(&rth, check_address_object, &target, NULL, NULL)) < 0)
		goto out_close_connection;
	
	if(target.found == 0)
		result = -ENODATA;

out_close_connection:
	rtnl_close(&rth);
out:
	return result;
}

/*
  * IPv6 CE-Router Test Debug:
  * 1. This function comes from wide-dhcpv6 at svn version 74760.
  * 2. To obtain the result of DAD in kernel protocol stack.
  * 2017-08-24 --liushenghui
*/
static int check_address_object(_unused const struct sockaddr_nl *who,
						struct nlmsghdr *n , void *arg)
{
	match_cond *target = (match_cond*)arg;
	struct ifaddrmsg *ifa = NLMSG_DATA(n);
	struct rtattr * rta_tb[IFA_MAX+1];

	if(n->nlmsg_len < NLMSG_LENGTH(sizeof(ifa)))
		return 0; //skip this

	if(ifa->ifa_index != target->if_idx)
		return 0; //skip this

	if(ifa->ifa_family != AF_INET6)
		return 0; //skip this

	parse_rtattr(rta_tb, IFA_MAX, IFA_RTA(ifa), n->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa)));

	if(rta_tb[IFA_ADDRESS])
	{
		if(memcmp(target->addr.s6_addr , ((struct in6_addr*)RTA_DATA(rta_tb[IFA_ADDRESS]))->s6_addr , 16) == 0)
		{
			//is DAD ok?
			if((ifa->ifa_flags & IFA_F_DADFAILED) == 0)
				target->found = 1;
		}
	}

	return 0;
}

int dad(const char * pIfName,
		const struct in6_addr * pAddr,
		int iPrefixLen)
{
	int iRet = 0;
	struct timespec ts = {2, 0};

	if (!pIfName || !pAddr)
		return -1;

	if (ifAddrConf(IFADDRCONF_ADD, pIfName, pAddr, iPrefixLen) < 0)
		return -1;

	while (nanosleep(&ts, &ts) < 0 && errno == EINTR);

	iRet = check_address_status(pIfName, pAddr);

	/*
	  * IPv6 CE-Router Test Debug:
	  * 1. It don't need to delete the address if the DAD is OK.
	  * 2. And it can speed up the configuring process to do so.
	  * 2017-08-24 --liushenghui
	*/
	if (iRet < 0)
		ifAddrConf(IFADDRCONF_DEL, pIfName, pAddr, iPrefixLen);

	return iRet;
}

