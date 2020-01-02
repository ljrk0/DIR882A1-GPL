/*******************************************************************************
 Copyright (c), 2012-2013, T&W ELECTRONICS(SHENTHEN) Co., Ltd.

 file name: common.h

*******************************************************************************/

#ifndef _APPS_COMMON_H_
#define _APPS_COMMON_H_

extern char *trim_r(char *str);
extern void killall_tk(const char *name);

extern int twsystem(char *command, int printFlag);
extern int twsystem_nowait(char *command, int printFlag);
extern int COMM_SystemEx(const char *pszCmd,char * pszResult,int iCount);
extern int tw_enable_coredump(int iflag);
extern int file_lock(char *tag);
extern void file_unlock(int lockfd);
/*****************************************************************************
enum

******************************************************************************/
enum {
    DEFAULT_TYPE = 0,
    ETH_WAN_TYPE,
    USB_WAN_TYPE,
    ADSL_WAN_TYPE,
    LAN_TYPE,
    WLAN_TYPE,
    ALL_TYPE = 65535 //liuchao. for restart all.
};
#endif /* _APPS_COMMON_H_ */

