#ifndef __WPS__H__
#define __WPS__H__



#if defined (CONFIG_RALINK_RT2880)
#define WPS_AP_PBC_LED_GPIO     13   // 0 ~ 24( or disable this feature by undefine it)

#elif defined (CONFIG_RALINK_RT3883)
#define WPS_AP_PBC_LED_GPIO     0   // 0 ~ 24( or disable this feature by undefine it)

#elif defined (CONFIG_RALINK_RT6855A)
#if defined (CONFIG_RT6855A_PCIE_PORT0_ENABLE)
#define WPS_AP_PBC_LED_GPIO     32	// rt6855 WPS LED
#else
#define WPS_AP_PBC_LED_GPIO     21	// rt6856 WPS LED
#endif

#elif defined (CONFIG_RALINK_MT7620)
#define WPS_AP_PBC_LED_GPIO     39	// MT7620 WPS LED

#elif defined (CONFIG_RALINK_MT7628)
#define WPS_AP_PBC_LED_GPIO     37	// MT7628 WPS LED

#else
#define WPS_AP_PBC_LED_GPIO     14   // 0 ~ 24( or disable this feature by undefine it)
#endif

#ifdef WPS_AP_PBC_LED_GPIO
#include "utils.h"
#define LedReset()                  {ledWps(WPS_AP_PBC_LED_GPIO, WPS_LED_RESET);}
#define LedInProgress()             {ledWps(WPS_AP_PBC_LED_GPIO, WPS_LED_PROGRESS);}
#define LedError()                  {ledWps(WPS_AP_PBC_LED_GPIO, WPS_LED_ERROR);}
#define LedSessionOverlapDetected() {ledWps(WPS_AP_PBC_LED_GPIO, WPS_LED_SESSION_OVERLAP);}
#define LedSuccess()                {ledWps(WPS_AP_PBC_LED_GPIO, WPS_LED_SUCCESS);}
#else
#define LedReset()
#define LedInProgress()
#define LedError()
#define LedSessionOverlapDetected()
#define LedSuccess()
#endif


void WPSRestart(void);
void formDefineWPS(void);

#define WSC_CONF_STATUS_STR "WscConfStatus"
#define WSC_CONF_STATUS_UNCONFIGURED    1   /* these value are taken from 2860 driver Release Note document. */
#define WSC_CONF_STATUS_CONFIGURED      2


/*
 * ripped from driver wsc.h,....ugly
  */
#define PACKED  __attribute__ ((packed))
#define USHORT  unsigned short
#define UCHAR   unsigned char


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif




#define WSC_ID_VERSION					0x104A
#define WSC_ID_VERSION_LEN				1
#define WSC_ID_VERSION_BEACON			0x00000001

#define WSC_ID_SC_STATE					0x1044
#define WSC_ID_SC_STATE_LEN				1
#define WSC_ID_SC_STATE_BEACON			0x00000002

#define WSC_ID_AP_SETUP_LOCKED			0x1057
#define WSC_ID_AP_SETUP_LOCKED_LEN		1
#define WSC_ID_AP_SETUP_LOCKED_BEACON	0x00000004

#define WSC_ID_SEL_REGISTRAR			0x1041
#define WSC_ID_SEL_REGISTRAR_LEN		1
#define WSC_ID_SEL_REGISTRAR_BEACON		0x00000008

#define WSC_ID_DEVICE_PWD_ID			0x1012
#define WSC_ID_DEVICE_PWD_ID_LEN		2
#define WSC_ID_DEVICE_PWD_ID_BEACON		0x00000010


#define WSC_ID_SEL_REG_CFG_METHODS		0x1053
#define WSC_ID_SEL_REG_CFG_METHODS_LEN	2
#define WSC_ID_SEL_REG_CFG_METHODS_BEACON	0x00000020

#define WSC_ID_UUID_E					0x1047
#define WSC_ID_UUID_E_LEN				16
#define WSC_ID_UUID_E_BEACON			0x00000040

#define WSC_ID_RF_BAND					0x103C
#define WSC_ID_RF_BAND_LEN				1
#define WSC_ID_RF_BAND_BEACON			0x00000080

#define WSC_ID_PRIMARY_DEVICE_TYPE		0x1054
#define WSC_ID_PRIMARY_DEVICE_TYPE_LEN	8
#define WSC_ID_PRIMARY_DEVICE_TYPE_BEACON	0x00000100

#if defined (RTDEV_SUPPORT)
#ifndef CONFIG_UNIQUE_WPS
void formDefineRaixWPS(void);
unsigned int getAPPIN(char *interface);
int isSafeForShell(char *str);
int getCurrentWscProfile(char *interface, WSC_CONFIGURED_VALUE *data, int len);
int getWscProfile(char *interface, WSC_PROFILE *wsc_profile);
void getWPSAuthMode(WSC_CONFIGURED_VALUE *result, char *ret_str);
void getWPSEncrypType(WSC_CONFIGURED_VALUE *result, char *ret_str);
int getWscStatus(char *interface);
char *getWscStatusStr(int status);
int getAPMac(char *ifname, char *if_hw);

void RaixWPSRestart();
#endif
#endif

#endif /* __WPS__H_ */

