/* Kernel module to match the port-ranges, trigger related port-ranges,
 * and alters the destination to a local IP address.
 *
 * Copyright (C) 2003, CyberTAN Corporation
 * All Rights Reserved.
 *
 * Description:
 *   This is kernel module for port-triggering.
 *
 *   The module follows the Netfilter framework, called extended packet
 *   matching modules.
 */

//#define DEBUG 1
#include <linux/version.h>
#include <linux/autoconf.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/if.h>
#include <linux/inetdevice.h>
#include <net/protocol.h>
#include <net/checksum.h>

#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_TRIGGER.h>

#ifdef CONFIG_NF_CONNTRACK
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_nat.h>
#else
#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>
#include <linux/netfilter_ipv4/ip_conntrack_tuple.h>
#include <linux/netfilter_ipv4/ip_autofw.h>
#include <linux/netfilter_ipv4/lockhelp.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#include <linux/netfilter_ipv4/ip_nat.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("iptables trigger target module");

#ifdef CONFIG_NF_CONNTRACK


/* This rwlock protects the main hash table, protocol/helper/expected
 *    registrations, conntrack timers*/
//#define ASSERT_READ_LOCK(x) MUST_BE_READ_LOCKED(&ip_conntrack_lock)
//#define ASSERT_WRITE_LOCK(x) MUST_BE_WRITE_LOCKED(&ip_conntrack_lock)
#define ASSERT_READ_LOCK(x)
#define ASSERT_WRITE_LOCK(x)
#include <linux/netfilter_ipv4/listhelp.h>

/* 调试开关 */
//#define DEBUG_TRIGGER

#ifdef DEBUG_TRIGGER
#define DEBUGP(fmt, args...) printk("%20s/%04d: "fmt, __FUNCTION__,  __LINE__, ## args);
#else
#define DEBUGP(fmt, args...)
#endif

struct ipt_trigger {
    struct list_head list;		/* Trigger list */
    struct timer_list timeout;	/* Timer for list destroying */
    u_int32_t srcip;		/* Outgoing source address */
    u_int32_t dstip;		/* Outgoing destination address */
    u_int16_t mproto;		/* Trigger protocol */
    u_int16_t rproto;		/* Related protocol */
    struct ipt_trigger_ports ports;	/* Trigger and related ports */
    u_int8_t reply;			/* Confirm a reply connection */
};


/* Return pointer to first true entry, if any, or NULL.  A macro
   required to allow inlining of cmpfn. */
#define LIST_FIND(head, cmpfn, type, args...)           \
({                                                      \
        const struct list_head *__i, *__j = NULL;       \
                                                        \
        ASSERT_READ_LOCK(head);                         \
        list_for_each(__i, (head))                      \
                if (cmpfn((const type)__i , ## args)) { \
                        __j = __i;                      \
                        break;                          \
                }                                       \
        (type)__j;                                      \
})

#define LIST_FIND_W(head, cmpfn, type, args...)         \
({                                                      \
        const struct list_head *__i, *__j = NULL;       \
                                                        \
        ASSERT_WRITE_LOCK(head);                        \
        list_for_each(__i, (head))                      \
                if (cmpfn((type)__i , ## args)) {       \
                        __j = __i;                      \
                        break;                          \
                }                                       \
        (type)__j;                                      \
})

/* Just like LIST_FIND but we search backwards */
#define LIST_FIND_B(head, cmpfn, type, args...)         \
({                                                      \
        const struct list_head *__i, *__j = NULL;       \
                                                        \
        ASSERT_READ_LOCK(head);                         \
        list_for_each_prev(__i, (head))                 \
                if (cmpfn((const type)__i , ## args)) { \
                        __j = __i;                      \
                        break;                          \
                }                                       \
        (type)__j;                                      \
})

/* IP Hooks */
/* After promisc drops, checksum checks. */
#ifndef NF_IP_PRE_ROUTING
#define NF_IP_PRE_ROUTING	0
#endif

/* If the packet is destined for another interface. */
#ifndef NF_IP_FORWARD
#define NF_IP_FORWARD		2
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
#define IPT_CONTINUE XT_CONTINUE
#define ipt_target xt_target
#endif

LIST_HEAD(trigger_list);
//DECLARE_LOCK(ip_trigger_lock);
static unsigned int trigger_ip_timeout = 600;	/* 600s */

//static unsigned int trigger_ip_timeout = 5;	/* 5s */


static void trigger_refresh(struct ipt_trigger *trig, unsigned long extra_jiffies)
{
    DEBUGP("%s: \n", __FUNCTION__);
    NF_CT_ASSERT(trig);
    //write_lock_bh(&nf_conntrack_lock);
    spin_lock_bh(&nf_conntrack_lock);
    /* Need del_timer for race avoidance (may already be dying). */
    if (del_timer(&trig->timeout)) {
        trig->timeout.expires = jiffies + extra_jiffies;
        add_timer(&trig->timeout);
    }

    //write_unlock_bh(&nf_conntrack_lock);
    spin_unlock_bh(&nf_conntrack_lock);	    
}

static void __del_trigger(struct ipt_trigger *trig)
{
    DEBUGP("%s: \n", __FUNCTION__);
    NF_CT_ASSERT(trig);
    ASSERT_WRITE_LOCK(&nf_conntrack_lock);

    DEBUGP("%s: \n", __FUNCTION__);
    /* delete from 'trigger_list' */
    list_del(&trig->list);
    kfree(trig);
}

static void trigger_timeout(unsigned long ul_trig)
{
    struct ipt_trigger *trig= (void *) ul_trig;

    DEBUGP("trigger list %p timed out\n", trig);
    //write_lock_bh(&nf_conntrack_lock);
    spin_lock_bh(&nf_conntrack_lock);    
    __del_trigger(trig);
    //write_unlock_bh(&nf_conntrack_lock);
    spin_unlock_bh(&nf_conntrack_lock);	    
}

static unsigned int
add_new_trigger(struct ipt_trigger *trig)
{
    struct ipt_trigger *new;

    DEBUGP("!!!!!!!!!!!! %s !!!!!!!!!!!\n", __FUNCTION__);
    //write_lock_bh(&nf_conntrack_lock);
    spin_lock_bh(&nf_conntrack_lock);
    new = (struct ipt_trigger *)kmalloc(sizeof(struct ipt_trigger), GFP_ATOMIC);
    if (!new) {

    spin_unlock_bh(&nf_conntrack_lock);    
        //write_unlock_bh(&nf_conntrack_lock);
        DEBUGP("%s: OOM allocating trigger list\n", __FUNCTION__);
        return -ENOMEM;
    }
    memset(new, 0, sizeof(*trig));
    INIT_LIST_HEAD(&new->list);
    memcpy(new, trig, sizeof(*trig));

    /* add to global table of trigger */
    list_prepend(&trigger_list, &new->list);
    /* add and start timer if required */
    init_timer(&new->timeout);
  
    new->timeout.data = (unsigned long)new;
    new->timeout.function = trigger_timeout;
    new->timeout.expires = jiffies + (trigger_ip_timeout * HZ);
    add_timer(&new->timeout);

    spin_unlock_bh(&nf_conntrack_lock);
    //write_unlock_bh(&nf_conntrack_lock);

    return 0;
}

static inline int trigger_out_matched(const struct ipt_trigger *i,
        const u_int16_t proto, const u_int16_t dport, const struct ipt_trigger_info *info)
{
    DEBUGP("i=%p, proto=%d, dport=%d, rproto=%d, rport[0..1]=%d-%d.\n",
        i, proto, dport, info->rproto, info->ports.rport[0], info->ports.rport[1]);
    DEBUGP("Got one, mproto= %d, mport[0..1]=%d-%d.\n",
        i->mproto, i->ports.mport[0], i->ports.mport[1]);

    return ((i->mproto == 0 || i->mproto == proto)
        && ((i->ports.mport[0] <= dport) && (i->ports.mport[1] >= dport))
        && (i->rproto == info->rproto)
        && ((i->ports.rport[0] == info->ports.rport[0]) && (i->ports.rport[1] == info->ports.rport[1]))
    );
}

static unsigned int
trigger_out(struct sk_buff **pskb,
        unsigned int hooknum,
        const struct net_device *in,
        const struct net_device *out,
        const void *targinfo)
{
    const struct ipt_trigger_info *info = targinfo;
    struct ipt_trigger trig, *found;
    //const struct iphdr *iph = (*pskb)->nh.iph;
    const struct iphdr *iph = ip_hdr(*pskb);
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */

    DEBUGP("############# %s ############\n", __FUNCTION__);
    /* Check if the trigger range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_out_matched, struct ipt_trigger *,
        iph->protocol, ntohs(tcph->dest), info);

    if (found) {
        /* Yeah, it exists. We need to update(delay) the destroying timer. */
        trigger_refresh(found, trigger_ip_timeout * HZ);
        /* In order to allow multiple hosts use the same port range, we update
           the 'saddr' after previous trigger has a reply connection. */
        if (found->reply)
            found->srcip = iph->saddr;
    }
    else {
        /* Create new trigger */
        memset(&trig, 0, sizeof(trig));
        trig.srcip = iph->saddr;
        trig.mproto = info->mproto;
        trig.rproto = info->rproto;
        memcpy(&trig.ports, &info->ports, sizeof(struct ipt_trigger_ports));
        add_new_trigger(&trig);	/* Add the new 'trig' to list 'trigger_list'. */
        DEBUGP("new trigger:srcip=%d.%d.%d.%d mproto=%d rproto=%d mport[%d-%d] rport[%d-%d]\n",
                NIPQUAD(trig.srcip),trig.mproto,trig.rproto,trig.ports.mport[0],trig.ports.mport[1],
                trig.ports.rport[0],trig.ports.rport[1]);
    }

    return IPT_CONTINUE;	/* We don't block any packet. */
}

static inline int trigger_in_matched(const struct ipt_trigger *i,
        const u_int16_t proto, const u_int16_t dport)
{
    DEBUGP("i=%p, proto= %d, dport=%d.\n", i, proto, dport);
    DEBUGP("Got one, rproto= %d, rport[0..1]=%d-%d.\n",
            i->rproto, i->ports.rport[0], i->ports.rport[1]);

    return ((i->rproto ==0 || i->rproto == proto)
        && ((i->ports.rport[0] <= dport) && (i->ports.rport[1] >= dport))
    );
}

static unsigned int
trigger_in(struct sk_buff **pskb,
        unsigned int hooknum,
        const struct net_device *in,
        const struct net_device *out,
        const void *targinfo)
{
    struct ipt_trigger *found;
    //const struct ipt_trigger_info *info = targinfo;
    //const struct iphdr *iph = (*pskb)->nh.iph;
    const struct iphdr *iph = ip_hdr(*pskb);
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */

    /* Check if the trigger-ed range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_in_matched,
            struct ipt_trigger *, iph->protocol, ntohs(tcph->dest));
    if (found) {
        DEBUGP("############# %s ############\n", __FUNCTION__);
        /* Yeah, it exists. We need to update(delay) the destroying timer. */
        trigger_refresh(found, trigger_ip_timeout * HZ);
        return NF_ACCEPT;	/* Accept it, or the incoming packet could be
                               dropped in the FORWARD chain */
    }

    return IPT_CONTINUE;	/* Our job is the interception. */
}

static unsigned int
trigger_dnat(struct sk_buff **pskb,
        unsigned int hooknum,
        const struct net_device *in,
        const struct net_device *out,
        const void *targinfo)
{
    struct ipt_trigger *found;
    //const struct ipt_trigger_info *info = targinfo;
    //const struct iphdr *iph = (*pskb)->nh.iph;
    const struct iphdr *iph = ip_hdr(*pskb);
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */
    struct nf_conn *ct;
    enum ip_conntrack_info ctinfo;
    struct nf_nat_range newrange;
	enum nf_nat_manip_type maniptype;

    NF_CT_ASSERT(hooknum == NF_INET_PRE_ROUTING);

    /* Check if the trigger-ed range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_in_matched,
            struct ipt_trigger *, iph->protocol, ntohs(tcph->dest));

    if (!found || !found->srcip)
    {
        DEBUGP("############# %s ############\n", __FUNCTION__);
        return IPT_CONTINUE;	/* We don't block any packet. */
    }

    DEBUGP("############# %s ############\n", __FUNCTION__);
    found->reply = 1;	/* Confirm there has been a reply connection. */
    ct = nf_ct_get(*pskb, &ctinfo);
    NF_CT_ASSERT(ct && (ctinfo == IP_CT_NEW));

    DEBUGP("%s: hooknum=%d got ", __FUNCTION__,hooknum);
    //NF_CT_DUMP_TUPLE(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
	memset(&newrange, 0, sizeof(struct nf_nat_range));
    /* Alter the destination of imcoming packet. */
#if 1
	newrange.flags = NF_NAT_RANGE_MAP_IPS;
	newrange.min_addr.ip = found->srcip;
	newrange.max_addr.ip = found->srcip;
#else	
    newrange = ((struct nf_nat_range)
            {NF_NAT_RANGE_MAP_IPS,
            found->srcip, found->srcip,
            { 0 }, { 0 }
            });
#endif
    //add by ramen to let the packet through our modem 2008/03/06
    (*pskb)->nfctinfo=IP_CT_RELATED;
    /* Hand modified range to generic setup. */
    maniptype = HOOK2MANIP(hooknum);
    //return nf_nat_setup_info(ct, &newrange, hooknum);
    return nf_nat_setup_info(ct, &newrange, maniptype);
}

static unsigned int
/*
trigger_target(struct sk_buff **pskb,
        const struct net_device *in,
        const struct net_device *out,
        unsigned int hooknum,
        const struct xt_target *target,
        const void *targinfo)
*/
trigger_target(struct sk_buff *skb, const struct xt_action_param *par)
{
    //const struct ipt_trigger_info *info = targinfo;
    const struct ipt_trigger_info *info = par->targinfo;
    //const struct iphdr *iph = (*pskb)->nh.iph;
    const struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */

    /* The Port-trigger only supports TCP and UDP. */
    if ((iph->protocol != IPPROTO_TCP) && (iph->protocol != IPPROTO_UDP))
        return IPT_CONTINUE;

    DEBUGP("type=%s, protocol=%s, dport=%d\n",
        (info->type == IPT_TRIGGER_DNAT) ? "dnat"
            :(info->type == IPT_TRIGGER_IN) ? "in" : "out",
        (iph->protocol == IPPROTO_TCP) ? "tcp" : "udp",
        ntohs(tcph->dest));

    if (info->type == IPT_TRIGGER_OUT)
    {
        /* Check if the protocol equal to trigger protocol
           and if the destport is in trigger port range
        */
        if ((info->mproto != 0 && info->mproto != iph->protocol)
            || (ntohs(tcph->dest) < info->ports.mport[0] || ntohs(tcph->dest) > info->ports.mport[1])
        )
            return IPT_CONTINUE;
            
        return trigger_out(&skb, par->hooknum, par->in, par->out, par->targinfo);
    }
    else
    {
        /* Check if the protocol equal to related protocol
           and if the destport is in related port range
        */
        DEBUGP("%d %s %d\n", info->ports.rport[0], __FUNCTION__, __LINE__);
        if (info->ports.rport[0])
        {
            
            DEBUGP("info->rproto:%d info->ports.rport[0]:%d info->ports.rport[1]:%d iph->protocol:%dtcph->dest:%d \n", info->rproto, info->ports.rport[0], info->ports.rport[1],
                    iph->protocol, ntohs(tcph->dest));
            if ((info->rproto != 0 && info->rproto != iph->protocol)
                || (ntohs(tcph->dest) < info->ports.rport[0] || ntohs(tcph->dest) > info->ports.rport[1])
            )
                return IPT_CONTINUE;
        }

        if (info->type == IPT_TRIGGER_IN)
        {
            return trigger_in(&skb, par->hooknum, par->in, par->out, par->targinfo);
        }
        else if (info->type == IPT_TRIGGER_DNAT)
        {
            return trigger_dnat(&skb, par->hooknum, par->in, par->out, par->targinfo);
        }
    }

    return IPT_CONTINUE;
}

static int
trigger_check(const struct xt_tgchk_param *par)
/*
trigger_check(const char *tablename,
        const void *e,
        const struct xt_target *target,
        void *targinfo,
        unsigned int hook_mask)
*/
{
    //const struct ipt_trigger_info *info = targinfo;
    const struct ipt_trigger_info *info = par;
    struct list_head *cur_item, *tmp_item;

    if ((strcmp(par->table, "mangle") == 0)) {
        DEBUGP("bad table `%s'.\n", par->table);
        return 0;
    }
    
    if (par->hook_mask & ~((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD))) {
        DEBUGP("bad hooks %x, %x.\n", par->hook_mask, ~((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD)));
        return 0;
    }

    if (info->mproto) {
        if (info->mproto != IPPROTO_TCP && info->mproto != IPPROTO_UDP) {
            DEBUGP("bad mproto %d.\n", info->mproto);
            return 0;
        }
    }
    if (info->rproto) {
        if (info->rproto != IPPROTO_TCP && info->rproto != IPPROTO_UDP) {
            DEBUGP("bad rproto %d.\n", info->rproto);
            return 0;
        }
    }
    if (info->type == IPT_TRIGGER_OUT) {
        if (!info->ports.mport[0] || !info->ports.rport[0]) {
            DEBUGP("Try 'iptables -j TRIGGER -h' for help.\n");
            return 0;
        }
    }
    else
    {
        if (info->mproto || info->ports.mport[0]) {
            DEBUGP("When trigger-type is 'in' or 'dnat', mproto and mport is not needed\n");
            return 0;
        }
    }

    /* Empty the 'trigger_list' */
    list_for_each_safe(cur_item, tmp_item, &trigger_list) {
        struct ipt_trigger *trig = (void *)cur_item;

        DEBUGP("%s: list_for_each_safe(): %p.\n", __FUNCTION__, trig);
        del_timer(&trig->timeout);
        __del_trigger(trig);
    }

    return 1;
}

static struct proc_dir_entry *trigger_proc = NULL;
static int proc_trigger_read(struct seq_file *seq, void *v)
{
    int i = 0;
    struct list_head *cur_item, *tmp_item;

    seq_printf(seq, "timeout: %d\n", trigger_ip_timeout/60);
    seq_printf(seq, "trigger list:\n");
    
    /* Empty the 'trigger_list' */
    list_for_each_safe(cur_item, tmp_item, &trigger_list) {
        struct ipt_trigger *trig = (void *)cur_item;
        seq_printf(seq, "#%d srcip= %pI4, dstip= %pI4, mproto= %d, mport[0..1]=%d-%d, rproto= %d, rport[0..1]=%d-%d.\n",
                    ++i, &(trig->srcip), &(trig->dstip),
                    trig->mproto, trig->ports.mport[0], trig->ports.mport[1],
                    trig->rproto, trig->ports.rport[0], trig->ports.rport[1]);
    }
	
	return 0;
}

static ssize_t proc_trigger_write(struct file *file, const char __user *buf, 
		      size_t len, loff_t *data)
{
	int ret;
	char str_buf[256];
    char action[20] = {0};
	int val = 0;
    int port_start = 0;
    int port_end = 0;
    int mproto = 0;
    struct list_head *cur_item, *tmp_item;

	if(len > 255)
	{
		printk("Error: the value must be between 10-3600\n");
		return len;
	}

	copy_from_user(str_buf, buf, len);
	str_buf[len] = '\0';

	ret = sscanf(str_buf, "%s %d %d %d", action, (int*)&val, (int *)&port_end, (int *)&mproto);
    if((ret != 2 && ret != 4) || val < 0)
	{
		printk("Error.Sample: echo debug 1 > /proc/debug \n");
		return len;
	}
	if (strcmp(action, "timeout") == 0)
	{
        if(val < 1 || val > 9999)
    	{
    		printk("Error: the value must be between 1-9999 minutes\n");
    		return len;
    	}
    	trigger_ip_timeout = val * 60;
	}
    else if (strcmp(action, "flush") == 0)
    {
        pr_debug("Flush trigger list.\n");
    
        /* Empty the 'trigger_list' */
        list_for_each_safe(cur_item, tmp_item, &trigger_list) {
            struct ipt_trigger *trig = (void *)cur_item;
            del_timer(&trig->timeout);
            __del_trigger(trig);
        }
    }
    else if (strcmp(action, "delete") == 0 && ret == 4)
    {
        port_start = val;
        
        list_for_each_safe(cur_item, tmp_item, &trigger_list)
        {
            struct ipt_trigger *trig = (void *)cur_item;
            if (trig->mproto == (u_int16_t)mproto && trig->ports.mport[0] == (u_int16_t)port_start &&
                trig->ports.mport[1] == (u_int16_t)port_end)
            {
                del_timer(&trig->timeout);
                 __del_trigger(trig);
            }
        }
    }
    else
    {
        printk("Error: Unkown command.\n");
    }

	return len;
}

static struct ipt_target trigger_reg = {
    .name		= "TRIGGER",
    .family         = NFPROTO_IPV4,
    .target		= trigger_target,
    .targetsize	= sizeof(struct ipt_trigger_info),
    //.table		= "mangle",
    //.hooks		= (1 << NF_INET_PRE_ROUTING) | (1 << NF_IP_LOCAL_OUT),
    //.hooks		= (1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD),
    .checkentry	= trigger_check,
    .me		= THIS_MODULE,
};

static int IntPortTriggerOpen(struct inode *inode, struct file *file)
{
	return single_open(file, proc_trigger_read, NULL);
}

#else

/* This rwlock protects the main hash table, protocol/helper/expected
 *    registrations, conntrack timers*/
//#define ASSERT_READ_LOCK(x) MUST_BE_READ_LOCKED(&ip_conntrack_lock)
//#define ASSERT_WRITE_LOCK(x) MUST_BE_WRITE_LOCKED(&ip_conntrack_lock)
#define ASSERT_READ_LOCK(x)
#define ASSERT_WRITE_LOCK(x)
#include <linux/netfilter_ipv4/listhelp.h>

/* 调试开关 */
//#define DEBUG_TRIGGER

#ifdef DEBUG_TRIGGER
#define DEBUGP(fmt, args...) printk("%20s/%04d: "fmt, __FUNCTION__,  __LINE__, ## args);
#else
#define DEBUGP(fmt, args...)
#endif

struct ipt_trigger {
    struct list_head list;		/* Trigger list */
    struct timer_list timeout;	/* Timer for list destroying */
    u_int32_t srcip;		/* Outgoing source address */
    u_int32_t dstip;		/* Outgoing destination address */
    u_int16_t mproto;		/* Trigger protocol */
    u_int16_t rproto;		/* Related protocol */
    struct ipt_trigger_ports ports;	/* Trigger and related ports */
    u_int8_t reply;			/* Confirm a reply connection */
};

LIST_HEAD(trigger_list);
//DECLARE_LOCK(ip_trigger_lock);

static void trigger_refresh(struct ipt_trigger *trig, unsigned long extra_jiffies)
{
    DEBUGP("%s: \n", __FUNCTION__);
    IP_NF_ASSERT(trig);
    //write_lock_bh(&ip_conntrack_lock);
    spin_lock_bh(&nf_conntrack_lock);

    /* Need del_timer for race avoidance (may already be dying). */
    if (del_timer(&trig->timeout)) {
        trig->timeout.expires = jiffies + extra_jiffies;
        add_timer(&trig->timeout);
    }

    //write_unlock_bh(&ip_conntrack_lock);
    spin_unlock_bh(&nf_conntrack_lock);	
}

static void __del_trigger(struct ipt_trigger *trig)
{
    DEBUGP("%s: \n", __FUNCTION__);
    IP_NF_ASSERT(trig);
    ASSERT_WRITE_LOCK(&ip_conntrack_lock);

    /* delete from 'trigger_list' */
    list_del(&trig->list);
    kfree(trig);
}

static void trigger_timeout(unsigned long ul_trig)
{
    struct ipt_trigger *trig= (void *) ul_trig;

    DEBUGP("trigger list %p timed out\n", trig);
    //write_lock_bh(&ip_conntrack_lock);
    spin_lock_bh(&ip_conntrack_lock);    
    __del_trigger(trig);
    //write_unlock_bh(&ip_conntrack_lock);
    spin_unlock_bh(&ip_conntrack_lock);	    
}

static unsigned int
add_new_trigger(struct ipt_trigger *trig)
{
    struct ipt_trigger *new;

    DEBUGP("!!!!!!!!!!!! %s !!!!!!!!!!!\n", __FUNCTION__);
    //write_lock_bh(&ip_conntrack_lock);
    spin_lock_bh(&ip_conntrack_lock);    
    new = (struct ipt_trigger *)kmalloc(sizeof(struct ipt_trigger), GFP_ATOMIC);
    if (!new) {
        //write_unlock_bh(&ip_conntrack_lock);
        spin_unlock_bh(&ip_conntrack_lock);	        
        DEBUGP("%s: OOM allocating trigger list\n", __FUNCTION__);
        return -ENOMEM;
    }

    memset(new, 0, sizeof(*trig));
    INIT_LIST_HEAD(&new->list);
    memcpy(new, trig, sizeof(*trig));

    /* add to global table of trigger */
    list_prepend(&trigger_list, &new->list);
    /* add and start timer if required */
    init_timer(&new->timeout);
    new->timeout.data = (unsigned long)new;
    new->timeout.function = trigger_timeout;
    new->timeout.expires = jiffies + (trigger_ip_timeout * HZ);
    add_timer(&new->timeout);

    //write_unlock_bh(&ip_conntrack_lock);
    spin_unlock_bh(&ip_conntrack_lock);	    

    return 0;
}

static inline int trigger_out_matched(const struct ipt_trigger *i,
        const u_int16_t proto, const u_int16_t dport, const struct ipt_trigger_info *info)
{
    DEBUGP("i=%p, proto=%d, dport=%d, rproto=%d, rport[0..1]=%d-%d.\n",
        i, proto, dport, info->rproto, info->ports.rport[0], info->ports.rport[1]);
    DEBUGP("Got one, mproto= %d, mport[0..1]=%d-%d.\n",
        i->mproto, i->ports.mport[0], i->ports.mport[1]);

    return ((i->mproto == 0 || i->mproto == proto)
        && ((i->ports.mport[0] <= dport) && (i->ports.mport[1] >= dport))
        && (i->rproto == info->rproto)
        && ((i->ports.rport[0] == info->ports.rport[0]) && (i->ports.rport[1] == info->ports.rport[1]))
    );
}

static unsigned int
trigger_out(struct sk_buff **pskb,
        unsigned int hooknum,
        const struct net_device *in,
        const struct net_device *out,
        const void *targinfo)
{
    const struct ipt_trigger_info *info = targinfo;
    struct ipt_trigger trig, *found;
    const struct iphdr *iph = (*pskb)->nh.iph;
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */

    DEBUGP("############# %s ############\n", __FUNCTION__);

    /* Check if the trigger range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_out_matched, struct ipt_trigger *,
        iph->protocol, ntohs(tcph->dest), info);

    if (found) {
        /* Yeah, it exists. We need to update(delay) the destroying timer. */
        trigger_refresh(found, trigger_ip_timeout * HZ);
        /* In order to allow multiple hosts use the same port range, we update
           the 'saddr' after previous trigger has a reply connection. */
        if (found->reply)
            found->srcip = iph->saddr;
    }
    else {

        /* Create new trigger */
        memset(&trig, 0, sizeof(trig));
        trig.srcip = iph->saddr;
        trig.mproto = info->mproto;
        trig.rproto = info->rproto;
        memcpy(&trig.ports, &info->ports, sizeof(struct ipt_trigger_ports));
        add_new_trigger(&trig);	/* Add the new 'trig' to list 'trigger_list'. */
        DEBUGP("new trigger:srcip=%d.%d.%d.%d mproto=%d rproto=%d mport[%d-%d] rport[%d-%d]\n",
                NIPQUAD(trig.srcip),trig.mproto,trig.rproto,trig.ports.mport[0],trig.ports.mport[1],
                trig.ports.rport[0],trig.ports.rport[1]);
    }

    return IPT_CONTINUE;	/* We don't block any packet. */
}

static inline int trigger_in_matched(const struct ipt_trigger *i,
        const u_int16_t proto, const u_int16_t dport)
{
    DEBUGP("i=%p, proto= %d, dport=%d.\n", i, proto, dport);
    DEBUGP("Got one, rproto= %d, rport[0..1]=%d-%d.\n",
            i->rproto, i->ports.rport[0], i->ports.rport[1]);

    return ((i->rproto ==0 || i->rproto == proto)
        && ((i->ports.rport[0] <= dport) && (i->ports.rport[1] >= dport))
    );
}

static unsigned int
trigger_in(struct sk_buff **pskb,
        unsigned int hooknum,
        const struct net_device *in,
        const struct net_device *out,
        const void *targinfo)
{
    struct ipt_trigger *found;
    //const struct ipt_trigger_info *info = targinfo;
    const struct iphdr *iph = (*pskb)->nh.iph;
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */

    /* Check if the trigger-ed range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_in_matched,
            struct ipt_trigger *, iph->protocol, ntohs(tcph->dest));
    if (found) {
        DEBUGP("############# %s ############\n", __FUNCTION__);
        /* Yeah, it exists. We need to update(delay) the destroying timer. */
        trigger_refresh(found, trigger_ip_timeout * HZ);
        return NF_ACCEPT;	/* Accept it, or the incoming packet could be
                               dropped in the FORWARD chain */
    }

    return IPT_CONTINUE;	/* Our job is the interception. */
}

static unsigned int
trigger_dnat(struct sk_buff **pskb,
        unsigned int hooknum,
        const struct net_device *in,
        const struct net_device *out,
        const void *targinfo)
{
    struct ipt_trigger *found;
    //const struct ipt_trigger_info *info = targinfo;
    const struct iphdr *iph = (*pskb)->nh.iph;
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */
    struct ip_conntrack *ct;
    enum ip_conntrack_info ctinfo;
    struct ip_nat_range newrange;

    IP_NF_ASSERT(hooknum == NF_INET_PRE_ROUTING);
    /* Check if the trigger-ed range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_in_matched,
            struct ipt_trigger *, iph->protocol, ntohs(tcph->dest));

    if (!found || !found->srcip)
        return IPT_CONTINUE;	/* We don't block any packet. */

    DEBUGP("############# %s ############\n", __FUNCTION__);
    found->reply = 1;	/* Confirm there has been a reply connection. */
    ct = ip_conntrack_get(*pskb, &ctinfo);
    IP_NF_ASSERT(ct && (ctinfo == IP_CT_NEW));

    DEBUGP("%s: hooknum=%d got ", __FUNCTION__,hooknum);
    DUMP_TUPLE(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);

    /* Alter the destination of imcoming packet. */
    newrange = ((struct ip_nat_range)
            {IP_NAT_RANGE_MAP_IPS,
            found->srcip, found->srcip,
            { 0 }, { 0 }
            });
    //add by ramen to let the packet through our modem 2008/03/06
    (*pskb)->nfctinfo=IP_CT_RELATED;
    /* Hand modified range to generic setup. */
    return ip_nat_setup_info(ct, &newrange, hooknum);
}

static unsigned int
trigger_target(struct sk_buff **pskb,
        const struct net_device *in,
        const struct net_device *out,
        unsigned int hooknum,
        const struct xt_target *target,
        const void *targinfo)
{
    const struct ipt_trigger_info *info = targinfo;
    const struct iphdr *iph = (*pskb)->nh.iph;
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */

    /* The Port-trigger only supports TCP and UDP. */
    if ((iph->protocol != IPPROTO_TCP) && (iph->protocol != IPPROTO_UDP))
        return IPT_CONTINUE;

    DEBUGP("type=%s, protocol=%s, dport=%d\n",
        (info->type == IPT_TRIGGER_DNAT) ? "dnat"
            :(info->type == IPT_TRIGGER_IN) ? "in" : "out",
        (iph->protocol == IPPROTO_TCP) ? "tcp" : "udp",
        ntohs(tcph->dest));

    if (info->type == IPT_TRIGGER_OUT)
    {
        /* Check if the protocol equal to trigger protocol
           and if the destport is in trigger port range
        */
        if ((info->mproto != 0 && info->mproto != iph->protocol)
            || (ntohs(tcph->dest) < info->ports.mport[0] || ntohs(tcph->dest) > info->ports.mport[1])
        )
            return IPT_CONTINUE;

        //return trigger_out(pskb, hooknum, in, out, targinfo);
    return IPT_CONTINUE;
    }
    else
    {
        /* Check if the protocol equal to related protocol
           and if the destport is in related port range
        */
        if (info->ports.rport[0])
        {
            if ((info->rproto != 0 && info->rproto != iph->protocol)
                || (ntohs(tcph->dest) < info->ports.rport[0] || ntohs(tcph->dest) > info->ports.rport[1])
            )
                return IPT_CONTINUE;
        }

        if (info->type == IPT_TRIGGER_IN)
        {
           //return trigger_in(pskb, hooknum, in, out, targinfo);
    return IPT_CONTINUE;
        }
        else if (info->type == IPT_TRIGGER_DNAT)
        {
    return IPT_CONTINUE;
            //return trigger_dnat(pskb, hooknum, in, out, targinfo);
        }
    }

    return IPT_CONTINUE;
}

static int
//trigger_check(const char *tablename,
//        const void *e,
//        const struct xt_target *target,
//        void *targinfo,
//        unsigned int hook_mask)
trigger_check(const struct xt_tgchk_param *par)
{
    //const struct ipt_trigger_info *info = targinfo;
    const struct ipt_trigger_info *info = par;
    struct list_head *cur_item, *tmp_item;
/*
    if ((strcmp(tablename, "mangle") == 0)) {
        DEBUGP("bad table `%s'.\n", tablename);
        return 0;
    }
    if (hook_mask & ~((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD))) {
        DEBUGP("bad hooks %x, %x.\n", hook_mask, ~((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD)));
        return 0;
    }
*/
    if (info->mproto) {
        if (info->mproto != IPPROTO_TCP && info->mproto != IPPROTO_UDP) {
            DEBUGP("bad mproto %d.\n", info->mproto);
            return 0;
        }
    }
    if (info->rproto) {
        if (info->rproto != IPPROTO_TCP && info->rproto != IPPROTO_UDP) {
            DEBUGP("bad rproto %d.\n", info->rproto);
            return 0;
        }
    }
    if (info->type == IPT_TRIGGER_OUT) {
        if (!info->ports.mport[0] || !info->ports.rport[0]) {
            DEBUGP("Try 'iptables -j TRIGGER -h' for help.\n");
            return 0;
        }
    }
    else
    {
        if (info->mproto || info->ports.mport[0]) {
            DEBUGP("When trigger-type is 'in' or 'dnat', mproto and mport is not needed\n");
            return 0;
        }
    }

    /* Empty the 'trigger_list' */
    list_for_each_safe(cur_item, tmp_item, &trigger_list) {
        struct ipt_trigger *trig = (void *)cur_item;

        DEBUGP("%s: list_for_each_safe(): %p.\n", __FUNCTION__, trig);
        del_timer(&trig->timeout);
        __del_trigger(trig);
    }

    return 1;
}

static struct ipt_target trigger_reg = {
    .name		= "TRIGGER",
    .family         = NFPROTO_IPV4,
    //.target		= trigger_target,
    .targetsize	= sizeof(struct ipt_trigger_info),
    .table		= "mangle",
//    .hooks		= (1 << NF_INET_PRE_ROUTING) | (1 << NF_IP_LOCAL_OUT),
    .hooks		= (1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD),
    .checkentry	= trigger_check,
    .me		= THIS_MODULE,
};

#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
static const struct file_operations port_trigger_fops = {
	.owner 		= THIS_MODULE,
	.open       = IntPortTriggerOpen,
	.read	 	= seq_read,
	.write 		= proc_trigger_write
};
#endif
static int __init ipt_trigger_init(void)
{
	
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
    trigger_proc = create_proc_entry("porttrigger", 0666, NULL);
    if (trigger_proc) {
    	trigger_proc->read_proc = proc_trigger_read;
    	trigger_proc->write_proc = proc_trigger_write;
    }
#else
	trigger_proc = proc_create("porttrigger", 0, NULL, &port_trigger_fops);
#endif
    trigger_reg.family = AF_INET;
    return xt_register_target(&trigger_reg);
}

static void __exit ipt_trigger_fini(void)
{
    if (trigger_proc) {
        remove_proc_entry("porttrigger", NULL);
    	trigger_proc = NULL;
    }
    
    xt_unregister_target(&trigger_reg);
}

module_init(ipt_trigger_init);
module_exit(ipt_trigger_fini);


