#ifndef _IP6T_REJECT_H
#define _IP6T_REJECT_H

enum ip6t_reject_with {
	IP6T_ICMP6_NO_ROUTE,
	IP6T_ICMP6_ADM_PROHIBITED,
	IP6T_ICMP6_NOT_NEIGHBOUR,
	IP6T_ICMP6_ADDR_UNREACH,
	IP6T_ICMP6_PORT_UNREACH,
#ifdef __CONFIG_IPV6_CE_ROUTER_TEST_DEBUG__
	/*
	  * IPv6 CE-Router Test Debug:
	  * 1. Add the code value 5 "source address failed ingress/egress policy"
	  *   for ICMPv6 type 1 "Destination Unreachable".
	  * 2. It has been assigned in RFC4443 section 3.1.
	  * 2017-10-16 --liushenghui
	*/
	IP6T_ICMP6_SRCADDR_FAIL,
#endif
	IP6T_ICMP6_ECHOREPLY,
	IP6T_TCP_RESET
};

struct ip6t_reject_info {
	u_int32_t	with;	/* reject type */
};

#endif /*_IP6T_REJECT_H*/
