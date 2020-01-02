/* vi: set sw=4 ts=4 sts=4: */
/*
 * station.h -- Station Mode
 *
 * Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 * $Id: //WIFI_SOC/MP/SDK_5_0_0_0/RT288x_SDK/source/user/goahead/src/station.h#1 $
 */

/* It is AP profile
#if defined (RT2860_NEW_MBSS_SUPPORT)
#define	RT2860_MBSSID_NUM 16
#elif defined (RT2860_MBSS_SUPPORT)
#define	RT2860_MBSSID_NUM 8
#endif
#if defined (RTDEV_NEW_MBSS_SUPPORT)
#define RTDEV_MBSSID_NUM 16
#elif defined (RTDEV_MBSS_SUPPORT)
#define RTDEV_MBSSID_NUM 8
#endif
*/

void formDefineStation(void);
int initStaProfile(void);
void initStaConnection(void);
void freeHeaderProfileSettings(void);
