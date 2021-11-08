/*
 *	Handle incoming frames
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/netfilter_bridge.h>
#include <linux/export.h>
#include <linux/rculist.h>
#include "br_private.h"

//add by taogan 2017-11-30
#if defined(CONFIG_DHCP_MON)
int br_dhcp_packet(struct sk_buff *skb, unsigned char *br_dest, unsigned char *dest);
#endif
// add end taogan 2017-11-30

/* Hook for brouter */
br_should_route_hook_t __rcu *br_should_route_hook __read_mostly;
EXPORT_SYMBOL(br_should_route_hook);
#ifdef CONFIG_BR_FILTER_DHCP

extern unsigned char uiBrFilterDhcpEnable[8];
extern unsigned char uiwanif[32];
extern unsigned char uilanif[64];

int br_filter_dhcp(struct sk_buff *skb)
{
	struct ethhdr                   *eth = (struct ethhdr *)skb->mac_header;
	struct iphdr                    *iph;
	struct udphdr                   *udph;

	//判断是否开启这个功能
	if (strcmp(uiBrFilterDhcpEnable,"1") != 0)
		return 1;

	//判断是否写入LAN WAN接口
	if (strlen(uiwanif) == 0 || strlen(uilanif) == 0)
		return 1;

	if (eth->h_proto != htons(ETH_P_IP))
		return 1;
	
	/* Check for possible (maliciously) malformed IP frame (thanks Dave) */
	iph = (struct iphdr *) skb->data;
	if(iph->protocol != 17)
		return 1;
	
	//udph = (struct udphdr *)(((unsigned char *)iph) + (iph->ihl<<2));
	udph = (struct udphdr *)&(skb->data[20]);

	//丢弃从LAN侧来的offer包
	if( udph->dest == htons(68) )// the packet is dhcp offer packet
	{
		//printk("receive offer dev = %s\n",skb->dev->name);
		if (strstr(uilanif, skb->dev->name) != NULL) {
			printk("reveive offer from lan device : %s, so drop it\n",skb->dev->name);
			return 0;
		}
	}
#if 0
	//丢弃从WAN口来的discover包
	if( udph->dest == htons(67) )// the packet is dhcp offer packet
	{
		//printk("receive discover dev = %s\n",skb->dev->name);
		if (strstr(uiwanif, skb->dev->name) != NULL) {
			printk("reveive discover from wan device : %s, so drop it\n",skb->dev->name);
			return 0;
		}	
	}
#endif
	return 1;
}

#endif

static int br_pass_frame_up(struct sk_buff *skb)
{
	struct net_device *indev, *brdev = BR_INPUT_SKB_CB(skb)->brdev;
	struct net_bridge *br = netdev_priv(brdev);
	struct br_cpu_netstats *brstats = this_cpu_ptr(br->stats);

	u64_stats_update_begin(&brstats->syncp);
	brstats->rx_packets++;
	brstats->rx_bytes += skb->len;
	u64_stats_update_end(&brstats->syncp);

	/* Bridge is just like any other port.  Make sure the
	 * packet is allowed except in promisc modue when someone
	 * may be running packet capture.
	 */
	if (!(brdev->flags & IFF_PROMISC) &&
	    !br_allowed_egress(br, br_get_vlan_info(br), skb)) {
		kfree_skb(skb);
		return NET_RX_DROP;
	}

	skb = br_handle_vlan(br, br_get_vlan_info(br), skb);
	if (!skb)
		return NET_RX_DROP;

	indev = skb->dev;
	skb->dev = brdev;

	return NF_HOOK(NFPROTO_BRIDGE, NF_BR_LOCAL_IN, skb, indev, NULL,
		       netif_receive_skb);
}

//add by taogan 2017-11-30
#ifdef CONFIG_DHCP_MON
#define OPT_CODE 0
#define OPT_LEN 1
#define OPT_DATA 2
#define OPTION_FIELD		0
#define FILE_FIELD		1
#define SNAME_FIELD		2
/* DHCP option codes (partial list) */
#define DHCP_PADDING		0x00
#define DHCP_SUBNET		0x01
#define DHCP_TIME_OFFSET	0x02
#define DHCP_ROUTER		0x03
#define DHCP_TIME_SERVER	0x04
#define DHCP_NAME_SERVER	0x05
#define DHCP_DNS_SERVER		0x06
#define DHCP_LOG_SERVER		0x07
#define DHCP_COOKIE_SERVER	0x08
#define DHCP_LPR_SERVER		0x09
#define DHCP_HOST_NAME		0x0c
#define DHCP_BOOT_SIZE		0x0d
#define DHCP_DOMAIN_NAME	0x0f
#define DHCP_SWAP_SERVER	0x10
#define DHCP_ROOT_PATH		0x11
#define DHCP_IP_TTL		0x17
#define DHCP_MTU		0x1a
#define DHCP_BROADCAST		0x1c
#define DHCP_STATIC_ROUTE	0x21 
#define DHCP_NTP_SERVER		0x2a
#define DHCP_WINS_SERVER	0x2c
#define DHCP_REQUESTED_IP	0x32
#define DHCP_LEASE_TIME		0x33
#define DHCP_OPTION_OVER	0x34
#define DHCP_MESSAGE_TYPE	0x35
#define DHCP_SERVER_ID		0x36
#define DHCP_PARAM_REQ		0x37
#define DHCP_MESSAGE		0x38
#define DHCP_MAX_SIZE		0x39
#define DHCP_T1			0x3a
#define DHCP_T2			0x3b
#define DHCP_VENDOR		0x3c
#define DHCP_CLIENT_ID		0x3d
#define DHCP_NETBIOS_NODETYPE 0x2e
#define DHCP_NETBIOS_SCOPE 0x2F
#define DHCP_END		0xFF
int br_filter_mangle_udp_packet(struct sk_buff **skb, unsigned int match_offset,unsigned int match_len,unsigned char *rep_buffer, unsigned int rep_len);
struct proc_dir_entry *filter_root = NULL;
static struct proc_dir_entry *res1 = NULL;
static struct proc_dir_entry *res2 = NULL;
int enable_filter = 0;

struct dhcpMessage {
	unsigned char op;
	unsigned char htype;
	unsigned char hlen;
	unsigned char hops;
	unsigned int xid;
	unsigned short secs;
	unsigned short flags;
	unsigned int ciaddr;
	unsigned int yiaddr;
	unsigned int siaddr;
	unsigned int giaddr;
	unsigned char chaddr[16];
	unsigned char sname[64];
	unsigned char file[128];
	unsigned int cookie;
	unsigned char options[308]; /* 312 - cookie */ 
};

struct udp_dhcp_packet {
	struct iphdr ip;
	struct udphdr udp;
	struct dhcpMessage data;
};

typedef struct device_list_table
{
	unsigned char ip[4];
	unsigned char mac[6];
	unsigned char hostname[64];
}device_list_table;

extern int g_dhcpmon_pid;
extern struct sock *netlink_dhcpmon_sock;
rwlock_t  br_netlink_lock;

unsigned char *br_get_dhcp_option(struct dhcpMessage *packet, int code, unsigned char *option_len)
{
	int i, length;
	unsigned char *optionptr = NULL;
	int over = 0, done = 0, curr = OPTION_FIELD;
	
	optionptr = packet->options;
	i = 0;
	length = 308;
	while (!done) 
    {
		if (i >= length) 
        {
			return NULL;
		}

		if (optionptr[i + OPT_CODE] == code) 
        {
			if (i + 1 + optionptr[i + OPT_LEN] >= length) 
            {
				return NULL;
			}
			*option_len = optionptr[i + OPT_LEN];
			return optionptr + i + 2;
		}			
		switch (optionptr[i + OPT_CODE]) 
        {
    		case DHCP_PADDING:
    			i++;
    			break;
    		case DHCP_OPTION_OVER:
    			if (i + 1 + optionptr[i + OPT_LEN] >= length) 
                {
    				return NULL;
    			}
    			over = optionptr[i + 3];
    			i += optionptr[OPT_LEN] + 2;
    			break;
    		case DHCP_END:
    			if (curr == OPTION_FIELD && over & FILE_FIELD) 
                {
    				optionptr = packet->file;
    				i = 0;
    				length = 128;
    				curr = FILE_FIELD;
    			} 
                else if (curr == FILE_FIELD && over & SNAME_FIELD) 
    		    {
    				optionptr = packet->sname;
    				i = 0;
    				length = 64;
    				curr = SNAME_FIELD;
    			} 
                else
                {
                    done = 1;
                }
    			break;
    		default:
    			i += optionptr[OPT_LEN + i] + 2;
		}
	}
	return NULL;
}

static void send_msg_to_user(struct device_list_table device_msg)
{
	int size = 0, ret = 0;
	struct device_list_table *s_device = NULL;
	unsigned char *old_tail = NULL;
	struct sk_buff *skb = NULL;
	struct nlmsghdr *nlh = NULL;

	if(g_dhcpmon_pid == 0)
	{
		printk("dhcpmon_pid's pid = %d.Please run dhcpmon_pid.\n", g_dhcpmon_pid);
		return;
	}

	size = NLMSG_SPACE(256);

	skb = alloc_skb(size, GFP_ATOMIC);
	old_tail = skb->tail;

	/* init msg header */
	nlh = nlmsg_put(skb, 0, 0, NETLINK_DHCP_TBS, size-sizeof(*nlh), 0);

	/* point to msg data area */
	s_device = NLMSG_DATA(nlh);

	/* fill data for sending */
	memset(s_device, 0, size);
	s_device->ip[0] = device_msg.ip[0];
	s_device->ip[1] = device_msg.ip[1];
	s_device->ip[2] = device_msg.ip[2];
	s_device->ip[3] = device_msg.ip[3];

	s_device->mac[0] = device_msg.mac[0];
	s_device->mac[1] = device_msg.mac[1];
	s_device->mac[2] = device_msg.mac[2];
	s_device->mac[3] = device_msg.mac[3];
	s_device->mac[4] = device_msg.mac[4];
	s_device->mac[5] = device_msg.mac[5];

	memcpy(s_device->hostname, device_msg.hostname, 64);
	
	/* get netlink msg length */
	nlh->nlmsg_len = skb->tail - old_tail;

	//NETLINK_CB(skb).pid = 0;
	NETLINK_CB(skb).dst_group = 0;
	//NETLINK_CB(skb).dst_pid = blp_cmm_pid;  //by ZhangYu,kernel 2.6.21 has no member dst_pid

	/* send msg */
	read_lock_bh(&br_netlink_lock);
	ret = netlink_unicast(netlink_dhcpmon_sock, skb, g_dhcpmon_pid, MSG_DONTWAIT);
	read_unlock_bh(&br_netlink_lock);

	//printk("message send! ret=%x \n",ret);

	return;
	
nlmsg_failure:
	printk("Fail to send netlink message.\n");

	if(skb)
		kfree_skb(skb);

	return;

}



/*
fuction: br_dhcp_packet

input: udp data, dhcp option code
return:code offset at skb
*/
int br_dhcp_packet(struct sk_buff *skb, unsigned char *br_dest, unsigned char *dest)
{
	struct iphdr *iph = NULL;
	struct udphdr *udph = NULL;
	struct udp_dhcp_packet *dhcp_packet = NULL;
	unsigned char option_len = 0, *temp = NULL;
#ifdef CONFIG_DHCP_MON
	struct device_list_table g_deviceListTable;
#endif
	
    if(NULL == skb)
		return 0;
	
	if(NULL == br_dest)
		return 0;

	if(NULL == dest)
		return 0;
	
    //get iph
    iph = (struct iphdr *)skb_network_header(skb);
	
	if(NULL == iph)
	    return 0;
	
    //get udph
	udph=(void *) iph + iph->ihl*4;
	if(NULL == udph)
	    return 0;

    //if dhcp server packet send to me
	if(iph->protocol == IPPROTO_UDP)
    {
		if(ntohs(udph->dest) == 67)
		{
			dhcp_packet =(struct udp_dhcp_packet *)skb->data;
			if(NULL == dhcp_packet)
				return 0;
			if(0x01 == dhcp_packet->data.op)
			{
	            if (NULL != (temp = br_get_dhcp_option(&(dhcp_packet->data), DHCP_HOST_NAME, &option_len))) 
	            {
	            	memset(&g_deviceListTable, 0, sizeof(struct device_list_table));
					
	            	memcpy(g_deviceListTable.mac, dhcp_packet->data.chaddr, 6);
					/*
					printk("mac=%02x:%02x:%02x:%02x:%02x:%02x", g_deviceListTable.mac[0], g_deviceListTable.mac[1],
						g_deviceListTable.mac[2], g_deviceListTable.mac[3], g_deviceListTable.mac[04], g_deviceListTable.mac[5]);
					*/
					if(option_len < 64)
					{
						memcpy(g_deviceListTable.hostname, temp, option_len);
						/*
						printk(" hostname=%s", g_deviceListTable.hostname);
						*/
					}

					if (NULL != (temp = br_get_dhcp_option(&(dhcp_packet->data), DHCP_REQUESTED_IP, &option_len))) 
					{
						memcpy(g_deviceListTable.ip, temp, 4);
						/*
						printk(" ip=%d.%d.%d.%d\n", g_deviceListTable.ip[0], g_deviceListTable.ip[1],
							g_deviceListTable.ip[2], g_deviceListTable.ip[3]);
						*/
					}
					send_msg_to_user(g_deviceListTable);
	            }
			}
		}
	}	
					
	return 0;
}


#endif
//add end taogan 2017-11-30

/* note: already called with rcu_read_lock */
int br_handle_frame_finish(struct sk_buff *skb)
{
	const unsigned char *dest = eth_hdr(skb)->h_dest;
	struct net_bridge_port *p = br_port_get_rcu(skb->dev);
	struct net_bridge *br;
	struct net_bridge_fdb_entry *dst;
	struct net_bridge_mdb_entry *mdst;
	struct sk_buff *skb2;
	u16 vid = 0;

	if (!p || p->state == BR_STATE_DISABLED)
		goto drop;

	if (!br_allowed_ingress(p->br, nbp_get_vlan_info(p), skb, &vid))
		goto drop;

	/* insert into forwarding database after filtering to avoid spoofing */
	br = p->br;
	br_fdb_update(br, p, eth_hdr(skb)->h_source, vid);

	if (!is_broadcast_ether_addr(dest) && is_multicast_ether_addr(dest) &&
	    br_multicast_rcv(br, p, skb))
		goto drop;

	if (p->state == BR_STATE_LEARNING)
		goto drop;
	//add by taogan 2017-11-30
#if defined(CONFIG_DHCP_MON)
		if(0 != g_dhcpmon_pid)
		{
			br_dhcp_packet(skb, p->br->dev->dev_addr, dest);
		}
#endif
	//add end by taogan 2017-11-30

	BR_INPUT_SKB_CB(skb)->brdev = br->dev;

	/* The packet skb2 goes to the local host (NULL to skip). */
	skb2 = NULL;

	if (br->dev->flags & IFF_PROMISC)
		skb2 = skb;

	dst = NULL;

	if (is_broadcast_ether_addr(dest))
		skb2 = skb;
	else if (is_multicast_ether_addr(dest)) {
		mdst = br_mdb_get(br, skb, vid);
		if (mdst || BR_INPUT_SKB_CB_MROUTERS_ONLY(skb)) {
			if ((mdst && mdst->mglist) ||
			    br_multicast_is_router(br))
				skb2 = skb;
			br_multicast_forward(mdst, skb, skb2);
			skb = NULL;
			if (!skb2)
				goto out;
		} else
			skb2 = skb;

		br->dev->stats.multicast++;
	} else if ((dst = __br_fdb_get(br, dest, vid)) &&
			dst->is_local) {
		skb2 = skb;
		/* Do not forward the packet since it's local. */
		skb = NULL;
	}

	if (skb) {
		if (dst) {
			dst->used = jiffies;
			br_forward(dst->dst, skb, skb2);
		} else
			br_flood_forward(br, skb, skb2);
	}

	if (skb2)
		return br_pass_frame_up(skb2);

out:
	return 0;
drop:
	kfree_skb(skb);
	goto out;
}

/* note: already called with rcu_read_lock */
static int br_handle_local_finish(struct sk_buff *skb)
{
	struct net_bridge_port *p = br_port_get_rcu(skb->dev);
	u16 vid = 0;

	br_vlan_get_tag(skb, &vid);
	br_fdb_update(p->br, p, eth_hdr(skb)->h_source, vid);
	return 0;	 /* process further */
}

/*
 * Return NULL if skb is handled
 * note: already called with rcu_read_lock
 */
rx_handler_result_t br_handle_frame(struct sk_buff **pskb)
{
	struct net_bridge_port *p;
	struct sk_buff *skb = *pskb;
	const unsigned char *dest = eth_hdr(skb)->h_dest;
	br_should_route_hook_t *rhook;

	if (unlikely(skb->pkt_type == PACKET_LOOPBACK))
		return RX_HANDLER_PASS;

	if (!is_valid_ether_addr(eth_hdr(skb)->h_source))
		goto drop;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		return RX_HANDLER_CONSUMED;

	p = br_port_get_rcu(skb->dev);

	if (unlikely(is_link_local_ether_addr(dest))) {
		/*
		 * See IEEE 802.1D Table 7-10 Reserved addresses
		 *
		 * Assignment		 		Value
		 * Bridge Group Address		01-80-C2-00-00-00
		 * (MAC Control) 802.3		01-80-C2-00-00-01
		 * (Link Aggregation) 802.3	01-80-C2-00-00-02
		 * 802.1X PAE address		01-80-C2-00-00-03
		 *
		 * 802.1AB LLDP 		01-80-C2-00-00-0E
		 *
		 * Others reserved for future standardization
		 */
		switch (dest[5]) {
		case 0x00:	/* Bridge Group Address */
			/* If STP is turned off,
			   then must forward to keep loop detection */
			if (p->br->stp_enabled == BR_NO_STP)
				goto forward;
			break;

		case 0x01:	/* IEEE MAC (Pause) */
			goto drop;

		default:
			/* Allow selective forwarding for most other protocols */
			if (p->br->group_fwd_mask & (1u << dest[5]))
				goto forward;
		}

		/* Deliver packet to local host only */
		if (NF_HOOK(NFPROTO_BRIDGE, NF_BR_LOCAL_IN, skb, skb->dev,
			    NULL, br_handle_local_finish)) {
			return RX_HANDLER_CONSUMED; /* consumed by filter */
		} else {
			*pskb = skb;
			return RX_HANDLER_PASS;	/* continue processing */
		}
	}

forward:
	switch (p->state) {
	case BR_STATE_FORWARDING:
#ifdef CONFIG_BR_FILTER_DHCP 
		if(br_filter_dhcp(skb) == 0)
		{
			goto drop;
		}
#endif
		rhook = rcu_dereference(br_should_route_hook);
		if (rhook) {
			if ((*rhook)(skb)) {
				*pskb = skb;
				return RX_HANDLER_PASS;
			}
			dest = eth_hdr(skb)->h_dest;
		}
		/* fall through */
	case BR_STATE_LEARNING:
		if (ether_addr_equal(p->br->dev->dev_addr, dest))
			skb->pkt_type = PACKET_HOST;

		NF_HOOK(NFPROTO_BRIDGE, NF_BR_PRE_ROUTING, skb, skb->dev, NULL,
			br_handle_frame_finish);
		break;
	default:
drop:
		kfree_skb(skb);
	}
	return RX_HANDLER_CONSUMED;
}
