/*
 *	Generic parts
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/llc.h>
#include <net/llc.h>
#include <net/stp.h>
#include <linux/version.h>
#include <linux/proc_fs.h>

#include "br_private.h"
//add by taogan 2017-11-30
#ifdef CONFIG_DHCP_MON
int g_dhcpmon_pid = 0;
struct sock *netlink_dhcpmon_sock;
#endif
//add end by taogan 2017-11-30

static const struct stp_proto br_stp_proto = {
	.rcv	= br_stp_rcv,
};

static struct pernet_operations br_net_ops = {
	.exit	= br_net_exit,
};
//add by taogan 2017-11-30
#ifdef CONFIG_DHCP_MON
static int dhcpmon_pid_write_proc( struct file *filp, const char __user *buf, unsigned long len, loff_t *data )
{
	int ret;
	
	if (copy_from_user(buf, buf, len))
		return -EFAULT;
	
	ret = sscanf(buf, "%d", &g_dhcpmon_pid);
	
	return len;
}

static int dhcpmon_pid_read_proc(struct file *file, char __user *buf,
						unsigned long len, loff_t *data)
{
	int iLen = 0;
	
	iLen = snprintf(buf, len, "%d", g_dhcpmon_pid);
	if (iLen <= *data)
	{
		return 0;
	}
	iLen  -= *data;
	
	*data += iLen;
	
	return iLen;
}

static struct file_operations dhcpmon_pid_fops = {
	.owner 	 = THIS_MODULE,
	.read	 = dhcpmon_pid_read_proc,
	.write	 = dhcpmon_pid_write_proc
};

#endif
//add end by taogan 2017-11-30

static int __init br_init(void)
{
	int err;
	//add by taogan 2017-11-30	
#ifdef CONFIG_DHCP_MON
	struct proc_dir_entry *proc_pid_dhcpmon;
	struct netlink_kernel_cfg cfg;
	
#endif
	//add end by taogan 2017-11-30

	err = stp_proto_register(&br_stp_proto);
	if (err < 0) {
		pr_err("bridge: can't register sap for STP\n");
		return err;
	}

	err = br_fdb_init();
	if (err)
		goto err_out;

	err = register_pernet_subsys(&br_net_ops);
	if (err)
		goto err_out1;

	err = br_netfilter_init();
	if (err)
		goto err_out2;

	err = register_netdevice_notifier(&br_device_notifier);
	if (err)
		goto err_out3;

	err = br_netlink_init();
	if (err)
		goto err_out4;
	
	//add by taogan 2017-11-30	
#ifdef CONFIG_DHCP_MON
#ifdef CONFIG_PROC_FS
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
			proc_pid_dhcpmon = create_proc_entry( "dhcpmon_pid", 0644, NULL);
			if(NULL != proc_pid_dhcpmon)
			{
				proc_pid_dhcpmon->read_proc = dhcpmon_pid_read_proc;
				proc_pid_dhcpmon->write_proc = dhcpmon_pid_write_proc;
			}
#else
			if (!(proc_pid_dhcpmon = proc_create("dhcpmon_pid", 0644, NULL, &dhcpmon_pid_fops)))
			{
				printk("!! FAIL to create dhcpmon_pid PROC !!\n");
				goto err_out4;
			}
#endif
#endif
#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22))
		netlink_dhcpmon_sock = netlink_kernel_create(NETLINK_DHCP_TBS, 0, NULL, THIS_MODULE);
#elif(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24))
		netlink_dhcpmon_sock = netlink_kernel_create(NETLINK_DHCP_TBS, 0, NULL, NULL, THIS_MODULE);
#else
		netlink_dhcpmon_sock = netlink_kernel_create(&init_net, NETLINK_DHCP_TBS, &cfg);
#endif
#endif
	//add end by taogan 2017-11-30

	brioctl_set(br_ioctl_deviceless_stub);

#if IS_ENABLED(CONFIG_ATM_LANE)
	br_fdb_test_addr_hook = br_fdb_test_addr;
#endif

	return 0;
err_out4:
	unregister_netdevice_notifier(&br_device_notifier);
err_out3:
	br_netfilter_fini();
err_out2:
	unregister_pernet_subsys(&br_net_ops);
err_out1:
	br_fdb_fini();
err_out:
	stp_proto_unregister(&br_stp_proto);
	return err;
}

static void __exit br_deinit(void)
{
	stp_proto_unregister(&br_stp_proto);

	br_netlink_fini();
	unregister_netdevice_notifier(&br_device_notifier);
	brioctl_set(NULL);

	unregister_pernet_subsys(&br_net_ops);

	rcu_barrier(); /* Wait for completion of call_rcu()'s */

	br_netfilter_fini();
#if IS_ENABLED(CONFIG_ATM_LANE)
	br_fdb_test_addr_hook = NULL;
#endif

	br_fdb_fini();
}

module_init(br_init)
module_exit(br_deinit)
MODULE_LICENSE("GPL");
MODULE_VERSION(BR_VERSION);
MODULE_ALIAS_RTNL_LINK("bridge");
