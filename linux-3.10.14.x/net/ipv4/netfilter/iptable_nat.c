/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2006 Netfilter Core Team <coreteam@netfilter.org>
 * (C) 2011 Patrick McHardy <kaber@trash.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/ip.h>
#include <net/ip.h>

#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_nat_core.h>
#include <net/netfilter/nf_nat_l3proto.h>
#include <linux/inetdevice.h>

static const struct xt_table nf_nat_ipv4_table = {
	.name		= "nat",
	.valid_hooks	= (1 << NF_INET_PRE_ROUTING) |
			  (1 << NF_INET_POST_ROUTING) |
			  (1 << NF_INET_LOCAL_OUT) |
			  (1 << NF_INET_LOCAL_IN),
	.me		= THIS_MODULE,
	.af		= NFPROTO_IPV4,
};


unsigned int G_ibr0_ip=0x100a8c0;
unsigned int G_ibr0_mask=0xffffff;
struct proc_dir_entry *ipinfo_root=NULL;
static struct proc_dir_entry *res1=NULL;
static struct proc_dir_entry *res2=NULL;
unsigned int ipaddrinfo = 0xc0a80001;
unsigned int netmaskinfo = 0xffffff00;


static int ipaddr_proc_write(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
      char tmpbuf[80];
			char *token=NULL; 
			char *s = NULL;
			unsigned char szip[5] = {0};
			char delim[] = "."; 
			unsigned int i=0;
			char   *stop  =NULL ; 

      if (count < 2)
	    return -EFAULT;
	    
	if (buffer && !copy_from_user(tmpbuf, buffer, 80))  {
 			s = tmpbuf;
    for(token = strsep(&s, delim); token != NULL; token = strsep(&s, delim)) {    	
    	szip[i++] = simple_strtoul(token,&stop,10);    	
    } 
   	ipaddrinfo = ((unsigned int)szip[0]<<24) + ((unsigned int)szip[1]<<16) + ((unsigned int)szip[2]<<8) + (unsigned int)szip[3];  
    G_ibr0_ip = ((unsigned int)szip[3]<<24) + ((unsigned int)szip[2]<<16) + ((unsigned int)szip[1]<<8) + (unsigned int)szip[0];

		return count;
	}	
      return -EFAULT;
}


static int netmask_proc_write(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
      char tmpbuf[80];
			char *token=NULL; 
			char *s = NULL;
			unsigned char sznetmask[5] = {0};
			char delim[] = "."; 
			unsigned int i=0;
			char   *stop  =NULL ; 

      if (count < 2)
	    return -EFAULT;   
	if (buffer && !copy_from_user(tmpbuf, buffer, 80))  {
 			s = tmpbuf;
    for(token = strsep(&s, delim); token != NULL; token = strsep(&s, delim)) {
    	sznetmask[i++] = simple_strtoul(token,&stop,10);
    } 
   
   netmaskinfo = ((unsigned int)sznetmask[0]<<24) + ((unsigned int)sznetmask[1]<<16) + ((unsigned int)sznetmask[2]<<8) + (unsigned int)sznetmask[3];  
    G_ibr0_mask = ((unsigned int)sznetmask[3]<<24) + ((unsigned int)sznetmask[2]<<16) + ((unsigned int)sznetmask[1]<<8) + (unsigned int)sznetmask[0];   		

		return count;
	}	
      return -EFAULT;
}

void ipaddr_show()
{
		return;
}

void netmask_show()
{
		return;
}

static int ipaddr_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file,ipaddr_show,NULL);
}

static int netmask_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file,netmask_show,NULL);
}

static const struct file_operations ipaddr_proc_fops = {
	.owner = THIS_MODULE,
	.open = ipaddr_seq_open,
 	.read  = seq_read,
 	.write  = ipaddr_proc_write,
 	.llseek = seq_lseek,  
 	.release = single_release,  
};

static const struct file_operations netmask_proc_fops = {
	.owner = THIS_MODULE,
	.open = netmask_seq_open,
 	.read  = seq_read,
 	.write  = netmask_proc_write,
  .llseek = seq_lseek,  
  .release = single_release,  
};

static unsigned int alloc_null_binding(struct nf_conn *ct, unsigned int hooknum)
{
	/* Force range to this IP; let proto decide mapping for
	 * per-proto parts (hence not IP_NAT_RANGE_PROTO_SPECIFIED).
	 */
	struct nf_nat_range range;

	range.flags = 0;
	pr_debug("Allocating NULL binding for %p (%pI4)\n", ct,
		 HOOK2MANIP(hooknum) == NF_NAT_MANIP_SRC ?
		 &ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip :
		 &ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip);

	return nf_nat_setup_info(ct, &range, HOOK2MANIP(hooknum));
}

static unsigned int nf_nat_rule_find(struct sk_buff *skb, unsigned int hooknum,
				     const struct net_device *in,
				     const struct net_device *out,
				     struct nf_conn *ct)
{
	struct net *net = nf_ct_net(ct);
	unsigned int ret;

	ret = ipt_do_table(skb, hooknum, in, out, net->ipv4.nat_table);
	if (ret == NF_ACCEPT) {
		if (!nf_nat_initialized(ct, HOOK2MANIP(hooknum)))
			ret = alloc_null_binding(ct, hooknum);
	}
	return ret;
}

#if 0
static void TW_get_br0_ip_mask(void)
{
	struct in_device *in_dev;	
	struct net_device *landev;
	struct in_ifaddr *ifap = NULL;
	G_ibr0_ip = 0;
	G_ibr0_mask = 0;
      	if ((landev = __dev_get_by_name(&init_net, "br0")) != NULL){
		in_dev=(struct net_device *)(landev->ip_ptr);
		if (in_dev != NULL) {
			for (ifap=in_dev->ifa_list; ifap != NULL; ifap=ifap->ifa_next) {
				if (strcmp("br0", ifap->ifa_label) == 0){
					G_ibr0_ip = ifap->ifa_address;
					G_ibr0_mask = ifap->ifa_mask;
					return; 
				}
			}
			
		}
	}	
}
#endif

static unsigned int
nf_nat_ipv4_fn(unsigned int hooknum,
	       struct sk_buff *skb,
	       const struct net_device *in,
	       const struct net_device *out,
	       int (*okfn)(struct sk_buff *))
{
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	struct nf_conn_nat *nat;
	/* maniptype == SRC for postrouting. */
	enum nf_nat_manip_type maniptype = HOOK2MANIP(hooknum);

	/* We never see fragments: conntrack defrags on pre-routing
	 * and local-out, and nf_nat_out protects post-routing.
	 */
	NF_CT_ASSERT(!ip_is_fragment(ip_hdr(skb)));

	ct = nf_ct_get(skb, &ctinfo);
	/* Can't track?  It's not due to stress, or conntrack would
	 * have dropped it.  Hence it's the user's responsibilty to
	 * packet filter it out, or implement conntrack/NAT for that
	 * protocol. 8) --RR
	 */
	if (!ct)
		{
		 /*songyongbo modify:2012-12-01
		    dlink 发现从wan口抓包偶尔能抓到源IP是lan 测网段的包
		    修改此处丢弃此类包*/
		struct iphdr *iph;	
		iph = ip_hdr(skb);
		//TW_get_br0_ip_mask();		
		if((0!=G_ibr0_ip)&&(0!=G_ibr0_mask))
		{
		    
			if((G_ibr0_ip & G_ibr0_mask) == (iph->saddr & G_ibr0_mask))
			{
               //printk("--------nf_nat_fn-----NF_DROP----\n");
			   //printk("br0 IP:0x%x--->br0 mask:0x%x\n",G_ibr0_ip,G_ibr0_mask);
			   //printk("Src IP:0x%x--->Dst IP:0x%x\n",iph->saddr,iph->daddr);
			   //printk("@@:0x%x--->@@:--->0x%x\n",(G_ibr0_ip & G_ibr0_mask),(iph->saddr & G_ibr0_mask));
			   return NF_DROP;
			}
			else
			{
			   return NF_ACCEPT;
			}
		}
     else       	
		   	return NF_ACCEPT;
	}

	/* Don't try to NAT if this packet is not conntracked */
	if (nf_ct_is_untracked(ct))
		return NF_ACCEPT;

	nat = nfct_nat(ct);
	if (!nat) {
		/* NAT module was loaded late. */
		if (nf_ct_is_confirmed(ct))
			return NF_ACCEPT;
		nat = nf_ct_ext_add(ct, NF_CT_EXT_NAT, GFP_ATOMIC);
		if (nat == NULL) {
			pr_debug("failed to add NAT extension\n");
			return NF_ACCEPT;
		}
	}

	switch (ctinfo) {
	case IP_CT_RELATED:
	case IP_CT_RELATED_REPLY:
		if (ip_hdr(skb)->protocol == IPPROTO_ICMP) {
			if (!nf_nat_icmp_reply_translation(skb, ct, ctinfo,
							   hooknum))
				return NF_DROP;
			else
				return NF_ACCEPT;
		}
		/* Fall thru... (Only ICMPs can be IP_CT_IS_REPLY) */
	case IP_CT_NEW:
		/* Seen it before?  This can happen for loopback, retrans,
		 * or local packets.
		 */
		if (!nf_nat_initialized(ct, maniptype)) {
			unsigned int ret;

			ret = nf_nat_rule_find(skb, hooknum, in, out, ct);
			if (ret != NF_ACCEPT)
				return ret;
		} else {
			pr_debug("Already setup manip %s for ct %p\n",
				 maniptype == NF_NAT_MANIP_SRC ? "SRC" : "DST",
				 ct);
			if (nf_nat_oif_changed(hooknum, ctinfo, nat, out))
				goto oif_changed;
		}
		break;

	default:
		/* ESTABLISHED */
		NF_CT_ASSERT(ctinfo == IP_CT_ESTABLISHED ||
			     ctinfo == IP_CT_ESTABLISHED_REPLY);
		if (nf_nat_oif_changed(hooknum, ctinfo, nat, out))
			goto oif_changed;
	}

	return nf_nat_packet(ct, ctinfo, hooknum, skb);

oif_changed:
	nf_ct_kill_acct(ct, ctinfo, skb);
	return NF_DROP;
}

static unsigned int
nf_nat_ipv4_in(unsigned int hooknum,
	       struct sk_buff *skb,
	       const struct net_device *in,
	       const struct net_device *out,
	       int (*okfn)(struct sk_buff *))
{
	unsigned int ret;
	__be32 daddr = ip_hdr(skb)->daddr;

	ret = nf_nat_ipv4_fn(hooknum, skb, in, out, okfn);
	if (ret != NF_DROP && ret != NF_STOLEN &&
	    daddr != ip_hdr(skb)->daddr)
		skb_dst_drop(skb);

	return ret;
}

static unsigned int
nf_nat_ipv4_out(unsigned int hooknum,
		struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out,
		int (*okfn)(struct sk_buff *))
{
#ifdef CONFIG_XFRM
	const struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	int err;
#endif
	unsigned int ret;

	/* root is playing with raw sockets. */
	if (skb->len < sizeof(struct iphdr) ||
	    ip_hdrlen(skb) < sizeof(struct iphdr))
		return NF_ACCEPT;

	ret = nf_nat_ipv4_fn(hooknum, skb, in, out, okfn);
#ifdef CONFIG_XFRM
	if (ret != NF_DROP && ret != NF_STOLEN &&
	    !(IPCB(skb)->flags & IPSKB_XFRM_TRANSFORMED) &&
	    (ct = nf_ct_get(skb, &ctinfo)) != NULL) {
		enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);

		if ((ct->tuplehash[dir].tuple.src.u3.ip !=
		     ct->tuplehash[!dir].tuple.dst.u3.ip) ||
		    (ct->tuplehash[dir].tuple.dst.protonum != IPPROTO_ICMP &&
		     ct->tuplehash[dir].tuple.src.u.all !=
		     ct->tuplehash[!dir].tuple.dst.u.all)) {
			err = nf_xfrm_me_harder(skb, AF_INET);
			if (err < 0)
				ret = NF_DROP_ERR(err);
		}
	}
#endif
	return ret;
}

static unsigned int
nf_nat_ipv4_local_fn(unsigned int hooknum,
		     struct sk_buff *skb,
		     const struct net_device *in,
		     const struct net_device *out,
		     int (*okfn)(struct sk_buff *))
{
	const struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	unsigned int ret;
	int err;

	/* root is playing with raw sockets. */
	if (skb->len < sizeof(struct iphdr) ||
	    ip_hdrlen(skb) < sizeof(struct iphdr))
		return NF_ACCEPT;

	ret = nf_nat_ipv4_fn(hooknum, skb, in, out, okfn);
	if (ret != NF_DROP && ret != NF_STOLEN &&
	    (ct = nf_ct_get(skb, &ctinfo)) != NULL) {
		enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);

		if (ct->tuplehash[dir].tuple.dst.u3.ip !=
		    ct->tuplehash[!dir].tuple.src.u3.ip) {
			err = ip_route_me_harder(skb, RTN_UNSPEC);
			if (err < 0)
				ret = NF_DROP_ERR(err);
		}
#ifdef CONFIG_XFRM
		else if (!(IPCB(skb)->flags & IPSKB_XFRM_TRANSFORMED) &&
			 ct->tuplehash[dir].tuple.dst.protonum != IPPROTO_ICMP &&
			 ct->tuplehash[dir].tuple.dst.u.all !=
			 ct->tuplehash[!dir].tuple.src.u.all) {
			err = nf_xfrm_me_harder(skb, AF_INET);
			if (err < 0)
				ret = NF_DROP_ERR(err);
		}
#endif
	}
	return ret;
}

static struct nf_hook_ops nf_nat_ipv4_ops[] __read_mostly = {
	/* Before packet filtering, change destination */
	{
		.hook		= nf_nat_ipv4_in,
		.owner		= THIS_MODULE,
		.pf		= NFPROTO_IPV4,
		.hooknum	= NF_INET_PRE_ROUTING,
#ifdef CONFIG_NETFILTER_RALINK_SWQOS_SUPPORT
		.priority	= NF_IP_PRI_NAT_DST_RALINK_QOS,	// Ralink: 
													// Raise priority to make nat table has higher
													// priority than mangle table.
													// In other words, now NAT happenes before QoS marking.
#else
		.priority	= NF_IP_PRI_NAT_DST, 
#endif
	},
	/* After packet filtering, change source */
	{
		.hook		= nf_nat_ipv4_out,
		.owner		= THIS_MODULE,
		.pf		= NFPROTO_IPV4,
		.hooknum	= NF_INET_POST_ROUTING,
		.priority	= NF_IP_PRI_NAT_SRC,
	},
	/* Before packet filtering, change destination */
	{
		.hook		= nf_nat_ipv4_local_fn,
		.owner		= THIS_MODULE,
		.pf		= NFPROTO_IPV4,
		.hooknum	= NF_INET_LOCAL_OUT,
		.priority	= NF_IP_PRI_NAT_DST,
	},
	/* After packet filtering, change source */
	{
		.hook		= nf_nat_ipv4_fn,
		.owner		= THIS_MODULE,
		.pf		= NFPROTO_IPV4,
		.hooknum	= NF_INET_LOCAL_IN,
		.priority	= NF_IP_PRI_NAT_SRC,
	},
};

static int __net_init iptable_nat_net_init(struct net *net)
{
	struct ipt_replace *repl;

	repl = ipt_alloc_initial_table(&nf_nat_ipv4_table);
	if (repl == NULL)
		return -ENOMEM;
	net->ipv4.nat_table = ipt_register_table(net, &nf_nat_ipv4_table, repl);
	kfree(repl);
	return PTR_RET(net->ipv4.nat_table);
}

static void __net_exit iptable_nat_net_exit(struct net *net)
{
	ipt_unregister_table(net, net->ipv4.nat_table);
}

static struct pernet_operations iptable_nat_net_ops = {
	.init	= iptable_nat_net_init,
	.exit	= iptable_nat_net_exit,
};


static int __init iptable_nat_init(void)
{
	int err;

	err = register_pernet_subsys(&iptable_nat_net_ops);
	if (err < 0)
		goto err1;

	err = nf_register_hooks(nf_nat_ipv4_ops, ARRAY_SIZE(nf_nat_ipv4_ops));
	if (err < 0)
		goto err2;
		
		ipinfo_root = proc_mkdir("ipinfo",NULL);
		if (!ipinfo_root){
			printk("create folder fail\n");
			return -ENOMEM;
		}
		res1 = proc_create("ip_addr", 0x0644,  ipinfo_root, &ipaddr_proc_fops);
		if (NULL==res1) {
				return -ENOMEM;
		}
		
		res2 = proc_create("net_mask", 0x0644,  ipinfo_root, &netmask_proc_fops);
		if (NULL==res2) {
			return -ENOMEM;
		}	
	return 0;

err2:
	unregister_pernet_subsys(&iptable_nat_net_ops);
err1:
	return err;
}

static void __exit iptable_nat_exit(void)
{
	nf_unregister_hooks(nf_nat_ipv4_ops, ARRAY_SIZE(nf_nat_ipv4_ops));
	unregister_pernet_subsys(&iptable_nat_net_ops);
}

module_init(iptable_nat_init);
module_exit(iptable_nat_exit);

MODULE_LICENSE("GPL");
