/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    rt_proc.c

    Abstract:
    Create and register proc file system for ralink device

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#include "rt_config.h"

int wl_proc_init(void);
int wl_proc_exit(void);

#ifdef CONFIG_RALINK_RT2880
#define PROCREG_DIR             "rt2880"
#endif /* CONFIG_RALINK_RT2880 */

#ifdef CONFIG_RALINK_RT3052
#define PROCREG_DIR             "rt3052"
#endif /* CONFIG_RALINK_RT3052 */

#ifdef CONFIG_RALINK_RT2883
#define PROCREG_DIR             "rt2883"
#endif /* CONFIG_RALINK_RT2883 */

#ifdef CONFIG_RALINK_RT3883
#define PROCREG_DIR             "rt3883"
#endif /* CONFIG_RALINK_RT3883 */

#ifdef CONFIG_RALINK_RT5350
#define PROCREG_DIR             "rt5350"
#endif /* CONFIG_RALINK_RT5350 */

#ifndef PROCREG_DIR
#define PROCREG_DIR             "rt2880"
#endif /* PROCREG_DIR */

/* Raider 20161103 */
int proc_wsc_status=0;
struct proc_dir_entry *proc_dir_wsc;
static struct proc_dir_entry *entry_wsc_status;
ssize_t proc_wsc_status_set(struct file *file, const char __user *buffer, 
size_t count, loff_t *ppos);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
ssize_t proc_wsc_status_get(struct file *file, char __user *buffer, size_t 
count, loff_t *ppos);
static const struct file_operations proc_wsc_status_fops = {
        .read = proc_wsc_status_get,
        .write = proc_wsc_status_set,
};
#else
ssize_t proc_wsc_status_get(char *page, char **start, off_t off, int count, 
int *eof, void *data_unused);
#endif
/* Raider 20161103 */
#if defined(CONFIG_PROC_FS)
extern struct proc_dir_entry *procRegDir;

/*T&W 2013-12-20 start  linsd copy code from chenqi for associated status*/
static struct proc_dir_entry *wlconfig; // 2013-12-20 for associated status
extern int wlconfig_assoc_flag_2g; // 20191014 for 2g associated status
extern int wlconfig_assoc_flag_5g; // 20191014 for 5g associated status

static wlconfig_assoc_read_2g(struct file *file, char __user *buffer,
						size_t count, loff_t *ppos)
{
    int iLen = 0;

    sprintf(buffer, "%d\n", wlconfig_assoc_flag_2g);
    iLen = strlen(buffer);

    if (iLen <= *ppos)
    {
        return 0;
    }
    iLen -= *ppos;

    *ppos += iLen;
    return iLen;
}


/*
*   funcname: wlconfig_assoc_write
*
*/
static wlconfig_assoc_write_2g( struct file *filp, const char __user *buff,unsigned long len, void *data )
{	
	unsigned int a[20];
	int i;
	unsigned long int value = 0;

	if( len > 6 )
	{
		printk("you input feed time is too long !\n");
		return -1;
	}
	
	for(i=0; i<len; i++) 
	{
		a[i] = *(buff+i);
	}

	for( i = 0 ; i < len-1 ; i++ )
	{
		if( a[i] >= 48 && a[i] <= 57 )      /*'0'<=argv[j]<='9'*/
		{
			a[i] -= 48;
		}
		else
		{  
			printk("you input data is invalid ! \n");
			return -1;
		}
	}
        printk("\n");
	for( i = 0 ; i < len-1; i++ )
	{
		value += a[i];
		if( i != ( len -2 ) )
			value *= 10;		
	}
        printk("%d \n",value);

	printk("---cut value is %d ----\n",value);
	
	wlconfig_assoc_flag_2g = value;	


	return len;

}

static wlconfig_assoc_read_5g(struct file *file, char __user *buffer,
						size_t count, loff_t *ppos)
{
    int iLen = 0;

    sprintf(buffer, "%d\n", wlconfig_assoc_flag_5g);
    iLen = strlen(buffer);

    if (iLen <= *ppos)
    {
        return 0;
    }
    iLen -= *ppos;

    *ppos += iLen;
    return iLen;
}


/*
*   funcname: wlconfig_assoc_write
*
*/
static wlconfig_assoc_write_5g( struct file *filp, const char __user *buff,unsigned long len, void *data )
{	
	unsigned int a[20];
	int i;
	unsigned long int value = 0;

	if( len > 6 )
	{
		printk("you input feed time is too long !\n");
		return -1;
	}
	
	for(i=0; i<len; i++) 
	{
		a[i] = *(buff+i);
	}

	for( i = 0 ; i < len-1 ; i++ )
	{
		if( a[i] >= 48 && a[i] <= 57 )      /*'0'<=argv[j]<='9'*/
		{
			a[i] -= 48;
		}
		else
		{  
			printk("you input data is invalid ! \n");
			return -1;
		}
	}
        printk("\n");
	for( i = 0 ; i < len-1; i++ )
	{
		value += a[i];
		if( i != ( len -2 ) )
			value *= 10;		
	}
        printk("%d \n",value);

	printk("---cut value is %d ----\n",value);
	
	wlconfig_assoc_flag_5g = value;	


	return len;

}

/*
*   funcname: wlconfig_module_init
*
*/

struct file_operations proc_fops_wlconfig_2g={
    .read=wlconfig_assoc_read_2g,
    .write=wlconfig_assoc_write_2g,
    .owner=THIS_MODULE,
};

struct file_operations proc_fops_wlconfig_5g={
    .read=wlconfig_assoc_read_5g,
    .write=wlconfig_assoc_write_5g,
    .owner=THIS_MODULE,
};

static int __init wlconfig_module_init( void )
{
    struct proc_dir_entry *iwconfig_assoc_2g, *iwconfig_assoc_5g;
	iwconfig_assoc_2g = proc_create("iwconfig_assoc_flag_2g", 0x0644, wlconfig, &proc_fops_wlconfig_2g);
	iwconfig_assoc_5g = proc_create("iwconfig_assoc_flag_5g", 0x0644, wlconfig, &proc_fops_wlconfig_5g);
}
static int __init wlconfig_module_exit( void )
{
	remove_proc_entry("iwconfig_assoc_flag_2g", wlconfig);
	remove_proc_entry("iwconfig_assoc_flag_5g", wlconfig);
}


/* 2013-12-20 end */


/*T&W 2018-08-11 for reconnect status*/
static struct proc_dir_entry *wlstatus; // 2018-08-11 for reconnect status
extern int wlstatus_disconnect_flag_2g; // 2018-08-11 for reconnect status
extern int wlstatus_disconnect_flag_5g; // 2018-08-11 for reconnect status

static wlstatus_disconnect_read_2g(struct file *file, char __user *buffer,
						size_t count, loff_t *ppos)
{
    int iLen = 0;

    sprintf(buffer, "%d\n", wlstatus_disconnect_flag_2g);
    iLen = strlen(buffer);

    if (iLen <= *ppos)
    {
        return 0;
    }
    iLen -= *ppos;

    *ppos += iLen;
    return iLen;
}


/*
*   funcname: wlstatus_disconnect_write
*
*/
static wlstatus_disconnect_write_2g( struct file *filp, const char __user *buff,unsigned long len, void *data )
{	
	unsigned int a[20];
	int i;
	unsigned long int value = 0;

	if( len > 6 )
	{
		printk("you input feed time is too long !\n");
		return -1;
	}
	
	for(i=0; i<len; i++) 
	{
		a[i] = *(buff+i);
	}

	for( i = 0 ; i < len-1 ; i++ )
	{
		if( a[i] >= 48 && a[i] <= 57 )      /*'0'<=argv[j]<='9'*/
		{
			a[i] -= 48;
		}
		else
		{  
			printk("you input data is invalid ! \n");
			return -1;
		}
	}
        printk("\n");
	for( i = 0 ; i < len-1; i++ )
	{
		value += a[i];
		if( i != ( len -2 ) )
			value *= 10;		
	}
        printk("%d \n",value);

	printk("---cut value is %d ----\n",value);
	
	wlstatus_disconnect_flag_2g = value;	


	return len;

}

static wlstatus_disconnect_read_5g(struct file *file, char __user *buffer,
						size_t count, loff_t *ppos)
{
    int iLen = 0;

    sprintf(buffer, "%d\n", wlstatus_disconnect_flag_5g);
    iLen = strlen(buffer);

    if (iLen <= *ppos)
    {
        return 0;
    }
    iLen -= *ppos;

    *ppos += iLen;
    return iLen;
}


/*
*   funcname: wlstatus_disconnect_write
*
*/
static wlstatus_disconnect_write_5g( struct file *filp, const char __user *buff,unsigned long len, void *data )
{	
	unsigned int a[20];
	int i;
	unsigned long int value = 0;

	if( len > 6 )
	{
		printk("you input feed time is too long !\n");
		return -1;
	}
	
	for(i=0; i<len; i++) 
	{
		a[i] = *(buff+i);
	}

	for( i = 0 ; i < len-1 ; i++ )
	{
		if( a[i] >= 48 && a[i] <= 57 )      /*'0'<=argv[j]<='9'*/
		{
			a[i] -= 48;
		}
		else
		{  
			printk("you input data is invalid ! \n");
			return -1;
		}
	}
        printk("\n");
	for( i = 0 ; i < len-1; i++ )
	{
		value += a[i];
		if( i != ( len -2 ) )
			value *= 10;		
	}
        printk("%d \n",value);

	printk("---cut value is %d ----\n",value);
	
	wlstatus_disconnect_flag_5g = value;	


	return len;

}
/*
*   funcname: wlstatus_module_init
*
*/
struct file_operations proc_fops_wlstatus_2g={
    .read=wlstatus_disconnect_read_2g,
    .write=wlstatus_disconnect_write_2g,
    .owner=THIS_MODULE,
};
struct file_operations proc_fops_wlstatus_5g={
    .read=wlstatus_disconnect_read_5g,
    .write=wlstatus_disconnect_write_5g,
    .owner=THIS_MODULE,
};

static int __init wlstatus_module_init( void )
{
    struct proc_dir_entry *wlstatus_disconnect_2g, *wlstatus_disconnect_5g;
	wlstatus_disconnect_2g = proc_create("wlstatus_disconnect_flag_2g", 0x0644, wlstatus, &proc_fops_wlstatus_2g);
	wlstatus_disconnect_5g = proc_create("wlstatus_disconnect_flag_5g", 0x0644, wlstatus, &proc_fops_wlstatus_5g);
}
static int __init wlstatus_module_exit( void )
{
	remove_proc_entry("wlstatus_disconnect_flag_2g", wlstatus);
	remove_proc_entry("wlstatus_disconnect_flag_5g", wlstatus);
}

/* 2018-08-11 end */
#if defined(VIDEO_TURBINE_SUPPORT)
extern BOOLEAN UpdateFromGlobal;
AP_VIDEO_STRUCT GLOBAL_AP_VIDEO_CONFIG;
/*struct proc_dir_entry *proc_ralink_platform, *proc_ralink_wl, *proc_ralink_wl_video; */
struct proc_dir_entry *proc_ralink_wl, *proc_ralink_wl_video;
static struct proc_dir_entry *entry_wl_video_Update, *entry_wl_video_Enable, *entry_wl_video_ClassifierEnable, *entry_wl_video_HighTxMode, *entry_wl_video_TxPwr, *entry_wl_video_VideoMCSEnable;
static struct proc_dir_entry *entry_wl_video_VideoMCS, *entry_wl_video_TxBASize, *entry_wl_video_TxLifeTimeMode, *entry_wl_video_TxLifeTime, *entry_wl_video_TxRetryLimit;


ssize_t video_Update_get(char *page, char **start, off_t off, int count,
						 int *eof, void *data_unused)
{
	sprintf(page, "%d\n", UpdateFromGlobal);
	*eof = 1;
	return strlen(page);
}

ssize_t video_Update_set(struct file *file, const char __user *buffer,
						 size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 10);

		UpdateFromGlobal = val;
	}

	return count;
}

ssize_t video_Enable_get(char *page, char **start, off_t off, int count,
						 int *eof, void *data_unused)
{
	sprintf(page, "%d\n", GLOBAL_AP_VIDEO_CONFIG.Enable);
	*eof = 1;
	return strlen(page);
}

ssize_t video_Enable_set(struct file *file, const char __user *buffer,
						 size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 10);

		GLOBAL_AP_VIDEO_CONFIG.Enable = val;
	}

	return count;
}

ssize_t video_ClassifierEnable_get(char *page, char **start, off_t off, int count,
								   int *eof, void *data_unused)
{
	sprintf(page, "%d\n", GLOBAL_AP_VIDEO_CONFIG.ClassifierEnable);
	*eof = 1;
	return strlen(page);
}

ssize_t video_ClassifierEnable_set(struct file *file, const char __user *buffer,
								   size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 10);

		GLOBAL_AP_VIDEO_CONFIG.ClassifierEnable = val;
	}

	return count;
}

ssize_t video_HighTxMode_get(char *page, char **start, off_t off, int count,
							 int *eof, void *data_unused)
{
	sprintf(page, "%d\n", GLOBAL_AP_VIDEO_CONFIG.HighTxMode);
	*eof = 1;
	return strlen(page);
}

ssize_t video_HighTxMode_set(struct file *file, const char __user *buffer,
							 size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 10);

		GLOBAL_AP_VIDEO_CONFIG.HighTxMode = val;
	}

	return count;
}

ssize_t video_TxPwr_get(char *page, char **start, off_t off, int count,
						int *eof, void *data_unused)
{
	sprintf(page, "%d\n", GLOBAL_AP_VIDEO_CONFIG.TxPwr);
	*eof = 1;
	return strlen(page);
}

ssize_t video_TxPwr_set(struct file *file, const char __user *buffer,
						size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 10);

		GLOBAL_AP_VIDEO_CONFIG.TxPwr = val;
	}

	return count;
}

ssize_t video_VideoMCSEnable_get(char *page, char **start, off_t off, int count,
								 int *eof, void *data_unused)
{
	sprintf(page, "%d\n", GLOBAL_AP_VIDEO_CONFIG.VideoMCSEnable);
	*eof = 1;
	return strlen(page);
}

ssize_t video_VideoMCSEnable_set(struct file *file, const char __user *buffer,
								 size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 10);

		GLOBAL_AP_VIDEO_CONFIG.VideoMCSEnable = val;
	}

	return count;
}

ssize_t video_VideoMCS_get(char *page, char **start, off_t off, int count,
						   int *eof, void *data_unused)
{
	sprintf(page, "%d\n", GLOBAL_AP_VIDEO_CONFIG.VideoMCS);
	*eof = 1;
	return strlen(page);
}

ssize_t video_VideoMCS_set(struct file *file, const char __user *buffer,
						   size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 10);

		GLOBAL_AP_VIDEO_CONFIG.VideoMCS = val;
	}

	return count;
}

ssize_t video_TxBASize_get(char *page, char **start, off_t off, int count,
						   int *eof, void *data_unused)
{
	sprintf(page, "%d\n", GLOBAL_AP_VIDEO_CONFIG.TxBASize);
	*eof = 1;
	return strlen(page);
}

ssize_t video_TxBASize_set(struct file *file, const char __user *buffer,
						   size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 10);

		GLOBAL_AP_VIDEO_CONFIG.TxBASize = val;
	}

	return count;
}

ssize_t video_TxLifeTimeMode_get(char *page, char **start, off_t off, int count,
								 int *eof, void *data_unused)
{
	sprintf(page, "%d\n", GLOBAL_AP_VIDEO_CONFIG.TxLifeTimeMode);
	*eof = 1;
	return strlen(page);
}

ssize_t video_TxLifeTimeMode_set(struct file *file, const char __user *buffer,
								 size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 10);

		GLOBAL_AP_VIDEO_CONFIG.TxLifeTimeMode = val;
	}

	return count;
}

ssize_t video_TxLifeTime_get(char *page, char **start, off_t off, int count,
							 int *eof, void *data_unused)
{
	sprintf(page, "%d\n", GLOBAL_AP_VIDEO_CONFIG.TxLifeTime);
	*eof = 1;
	return strlen(page);
}

ssize_t video_TxLifeTime_set(struct file *file, const char __user *buffer,
							 size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 10);

		GLOBAL_AP_VIDEO_CONFIG.TxLifeTime = val;
	}

	return count;
}

ssize_t video_TxRetryLimit_get(char *page, char **start, off_t off, int count,
							   int *eof, void *data_unused)
{
	sprintf(page, "0x%x\n", GLOBAL_AP_VIDEO_CONFIG.TxRetryLimit);
	*eof = 1;
	return strlen(page);
}

ssize_t video_TxRetryLimit_set(struct file *file, const char __user *buffer,
							   size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
			return -EFAULT;

		if (buf)
			val = os_str_toul(buf, NULL, 16);

		GLOBAL_AP_VIDEO_CONFIG.TxRetryLimit = val;
	}

	return count;
}

int wl_video_proc_init(void)
{
	GLOBAL_AP_VIDEO_CONFIG.Enable = FALSE;
	GLOBAL_AP_VIDEO_CONFIG.ClassifierEnable = FALSE;
	GLOBAL_AP_VIDEO_CONFIG.HighTxMode = FALSE;
	GLOBAL_AP_VIDEO_CONFIG.TxPwr = 0;
	GLOBAL_AP_VIDEO_CONFIG.VideoMCSEnable = FALSE;
	GLOBAL_AP_VIDEO_CONFIG.VideoMCS = 0;
	GLOBAL_AP_VIDEO_CONFIG.TxBASize = 0;
	GLOBAL_AP_VIDEO_CONFIG.TxLifeTimeMode = FALSE;
	GLOBAL_AP_VIDEO_CONFIG.TxLifeTime = 0;
	GLOBAL_AP_VIDEO_CONFIG.TxRetryLimit = 0;
	proc_ralink_wl = proc_mkdir("wl", procRegDir);

	if (proc_ralink_wl)
		proc_ralink_wl_video = proc_mkdir("VideoTurbine", proc_ralink_wl);

	if (proc_ralink_wl_video) {
		entry_wl_video_Update = create_proc_entry("UpdateFromGlobal", 0, proc_ralink_wl_video);

		if (entry_wl_video_Update) {
			entry_wl_video_Update->read_proc = (read_proc_t *)&video_Update_get;
			entry_wl_video_Update->write_proc = (write_proc_t *)&video_Update_set;
		}

		entry_wl_video_Enable = create_proc_entry("Enable", 0, proc_ralink_wl_video);

		if (entry_wl_video_Enable) {
			entry_wl_video_Enable->read_proc = (read_proc_t *)&video_Enable_get;
			entry_wl_video_Enable->write_proc = (write_proc_t *)&video_Enable_set;
		}

		entry_wl_video_ClassifierEnable = create_proc_entry("ClassifierEnable", 0, proc_ralink_wl_video);

		if (entry_wl_video_ClassifierEnable) {
			entry_wl_video_ClassifierEnable->read_proc = (read_proc_t *)&video_ClassifierEnable_get;
			entry_wl_video_ClassifierEnable->write_proc = (write_proc_t *)&video_ClassifierEnable_set;
		}

		entry_wl_video_HighTxMode = create_proc_entry("HighTxMode", 0, proc_ralink_wl_video);

		if (entry_wl_video_HighTxMode) {
			entry_wl_video_HighTxMode->read_proc = (read_proc_t *)&video_HighTxMode_get;
			entry_wl_video_HighTxMode->write_proc = (write_proc_t *)&video_HighTxMode_set;
		}

		entry_wl_video_TxPwr = create_proc_entry("TxPwr", 0, proc_ralink_wl_video);

		if (entry_wl_video_TxPwr) {
			entry_wl_video_TxPwr->read_proc = (read_proc_t *)&video_TxPwr_get;
			entry_wl_video_TxPwr->write_proc = (write_proc_t *)&video_TxPwr_set;
		}

		entry_wl_video_VideoMCSEnable = create_proc_entry("VideoMCSEnable", 0, proc_ralink_wl_video);

		if (entry_wl_video_VideoMCSEnable) {
			entry_wl_video_VideoMCSEnable->read_proc = (read_proc_t *)&video_VideoMCSEnable_get;
			entry_wl_video_VideoMCSEnable->write_proc = (write_proc_t *)&video_VideoMCSEnable_set;
		}

		entry_wl_video_VideoMCS = create_proc_entry("VideoMCS", 0, proc_ralink_wl_video);

		if (entry_wl_video_VideoMCS) {
			entry_wl_video_VideoMCS->read_proc = (read_proc_t *)&video_VideoMCS_get;
			entry_wl_video_VideoMCS->write_proc = (write_proc_t *)&video_VideoMCS_set;
		}

		entry_wl_video_TxBASize = create_proc_entry("TxBASize", 0, proc_ralink_wl_video);

		if (entry_wl_video_TxBASize) {
			entry_wl_video_TxBASize->read_proc = (read_proc_t *)&video_TxBASize_get;
			entry_wl_video_TxBASize->write_proc = (write_proc_t *)&video_TxBASize_set;
		}

		entry_wl_video_TxLifeTimeMode = create_proc_entry("TxLifeTimeMode", 0, proc_ralink_wl_video);

		if (entry_wl_video_TxLifeTimeMode) {
			entry_wl_video_TxLifeTimeMode->read_proc = (read_proc_t *)&video_TxLifeTimeMode_get;
			entry_wl_video_TxLifeTimeMode->write_proc = (write_proc_t *)&video_TxLifeTimeMode_set;
		}

		entry_wl_video_TxLifeTime = create_proc_entry("TxLifeTime", 0, proc_ralink_wl_video);

		if (entry_wl_video_TxLifeTime) {
			entry_wl_video_TxLifeTime->read_proc = (read_proc_t *)&video_TxLifeTime_get;
			entry_wl_video_TxLifeTime->write_proc = (write_proc_t *)&video_TxLifeTime_set;
		}

		entry_wl_video_TxRetryLimit = create_proc_entry("TxRetryLimit", 0, proc_ralink_wl_video);

		if (entry_wl_video_TxRetryLimit) {
			entry_wl_video_TxRetryLimit->read_proc = (read_proc_t *)&video_TxRetryLimit_get;
			entry_wl_video_TxRetryLimit->write_proc = (write_proc_t *)&video_TxRetryLimit_set;
		}
	}

	return 0;
}

int wl_video_proc_exit(void)
{
	if (entry_wl_video_Enable)
		remove_proc_entry("Enable", proc_ralink_wl_video);

	if (entry_wl_video_ClassifierEnable)
		remove_proc_entry("ClassifierEnabl", proc_ralink_wl_video);

	if (entry_wl_video_HighTxMode)
		remove_proc_entry("HighTxMode", proc_ralink_wl_video);

	if (entry_wl_video_TxPwr)
		remove_proc_entry("TxPwr", proc_ralink_wl_video);

	if (entry_wl_video_VideoMCSEnable)
		remove_proc_entry("VideoMCSEnable", proc_ralink_wl_video);

	if (entry_wl_video_VideoMCS)
		remove_proc_entry("VideoMCS", proc_ralink_wl_video);

	if (entry_wl_video_TxBASize)
		remove_proc_entry("TxBASize", proc_ralink_wl_video);

	if (entry_wl_video_TxLifeTimeMode)
		remove_proc_entry("TxLifeTimeMode", proc_ralink_wl_video);

	if (entry_wl_video_TxLifeTime)
		remove_proc_entry("TxLifeTime", proc_ralink_wl_video);

	if (entry_wl_video_TxRetryLimit)
		remove_proc_entry("TxRetryLimit", proc_ralink_wl_video);

	if (proc_ralink_wl_video)
		remove_proc_entry("Video", proc_ralink_wl);

	return 0;
}
#endif
int wl_apcli_status_proc_init(void)
{
	/*Add by T&W*/
	if (wlconfig == NULL)
		wlconfig = proc_mkdir("apcli_assoc", NULL);
	if(wlconfig)
	{
		wlconfig_module_init();
	}
	if (wlstatus == NULL)
		wlstatus = proc_mkdir("apcli_disconnect", NULL);
	if(wlstatus)
	{
		wlstatus_module_init();
	}
	/*End by T&W*/

	return 0;
}

int wl_apcli_status_proc_exit(void)
{
#ifdef VIDEO_TURBINE_SUPPORT
	if (proc_ralink_wl_video) {
		wl_video_proc_exit();
		remove_proc_entry("Video", proc_ralink_wl);
	}
	if (proc_ralink_wl)
		remove_proc_entry("wl", procRegDir);
#endif /* VIDEO_TURBINE_SUPPORT */

	if (wlconfig)
		wlconfig_module_exit();
	
	if (wlstatus)
		wlstatus_module_exit();
	remove_proc_entry("apcli_assoc", NULL);
	remove_proc_entry("apcli_disconnect", NULL);
	wlconfig = NULL;
	wlstatus = NULL;
	pr_info("[%s] - Remove proc entry\n", __FUNCTION__);
	
	return 0;
}

#else
int wl_proc_init(void)
{
	return 0;
}

int wl_proc_exit(void)
{
	return 0;
}
#endif /* VIDEO_TURBINE_SUPPORT  && CONFIG_PROC_FS */

/* Raider 20161103 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
ssize_t proc_wsc_status_get(struct file *file, char __user *buffer,
						size_t count, loff_t *ppos)
{
    int iLen = 0;

    sprintf(buffer, "%d\n", proc_wsc_status);
    iLen = strlen(buffer);

    if (iLen <= *ppos)
    {
        return 0;
    }
    iLen -= *ppos;

    *ppos += iLen;
    return iLen;
}
#else
ssize_t proc_wsc_status_get(char *page, char **start, off_t off, int count,
						int *eof, void *data_unused)
{
	sprintf(page, "%d\n", proc_wsc_status);
	*eof = 1;
        return strlen(page);
}
#endif

ssize_t proc_wsc_status_set(struct file *file, const char __user *buffer,
						size_t count, loff_t *ppos)
{
	char *buf = kmalloc(count, GFP_KERNEL);

	if (buf) {
		unsigned long val;

		if (copy_from_user(buf, buffer, count))
		{
			kfree(buf);
			return -EFAULT;
		}
		
		val = simple_strtoul(buf, NULL, 10);

		proc_wsc_status = val;
	}

	if(NULL != buf)
		kfree(buf);
	
	return count;
}

int wsc_status_proc_init(void)
{
	if (proc_dir_wsc == NULL)
		proc_dir_wsc = proc_mkdir("wsc", NULL);

	if(proc_dir_wsc && entry_wsc_status == NULL)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
		entry_wsc_status = proc_create("wsc_status", 0x0644, proc_dir_wsc, &
proc_wsc_status_fops);
#else
		entry_wsc_status = create_proc_entry("wsc_status", 0x0644, proc_dir_wsc);

	if(entry_wsc_status)
	{
		entry_wsc_status->read_proc=(read_proc_t*)&proc_wsc_status_get;
		entry_wsc_status->write_proc=(write_proc_t*)&proc_wsc_status_set;
	}
#endif

	return 0;
}
/* Raider 20161103 */
