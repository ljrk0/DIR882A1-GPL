/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 �ļ����� : tbstype.h
 �ļ����� : TBS �������Ͷ���


 �����б� :


 �޶���¼ :
          1 ���� : TBS
            ���� : 2007-8-23
            ���� :
            20080811,heyiming, ������WPS����PID_WPS��ģ��ID,MID_WPS
**********************************************************************/
#ifndef __TBS_TYPE_H__
#define __TBS_TYPE_H__

#ifdef _cplusplus
    #if _cplusplus
        extern "C" {
    #endif
#endif

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/

/*--------------------���ó�������----------------------------*/
#ifndef FALSE
    #define FALSE       0
#endif
#ifndef TRUE
    #define TRUE        (!FALSE)
#endif

#ifndef NULL
    #define NULL    ((void *)0)
#endif

/* �����������͵ĳ��ȶ��� */
#define MAX_BOOL_LEN 12
#define MAX_INT_LEN    12
#define MAX_UINT_LEN 12
#define MAX_ULONG_LEN    12

#define BOOL    unsigned int

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/

/*����PID----ͬ��s_astPID2PathTbl*/
#define PID_PC      0x01
#define PID_SNMPA   0x02
//#define PID_LBT     0x03
#define PID_CCP     0x04
#define PID_WEB     0x05
#define PID_LNB     0x06
#define PID_CLI     0x07
#define PID_TR069FE   0x09
#define PID_TR069_HTTPD   0x0a
#define PID_ELM     0x0b
#define PID_UPG     0x0c
#define PID_VDSLD   0x0d
#define PID_TM      0x0e
#define PID_UPGCGI  0x10
#define PID_FTPUPG  0x11
#define PID_PTI     0x12
#define PID_MSG4UDP 0x13            /* MSG4UDPͨ�Ž���ID, ��wsccmd����ͨ�� */
#define PID_TFTPUPG 0x14
#define PID_HOTPLUG 0x15
#define PID_TR064FE 0x16

//ipv6
#define PID_RAMON   0x17
#define PID_IP6MON  0x18
#define PID_IP6AAC  0x19

#define PID_ONETOUCH_DAEMON  0x1a
#define PID_GOAHEAD_DAEMON   0x1b
#define PID_DROPBEAR 0x20           /* Dropbear���� */
#define PID_TELNET   0x21           /* Telnet���� */
/*stunc process id*/
#define PID_STUNC  0x22
#define PID_LBT     PID_PC
#define PID_MON     PID_PC

#define PID_CTMDW   0x1c
#define PID_MSGSENDER   0x1d

#define PID_CMDDAEMON   0x1e

#define PID_NULL    0xFF

/*ģ��MID----ͬ��s_astMID2NameTbl*/
#define FLG_MID_GRP 0xFF

#define MID_CCP         ((PID_CCP << 8) | 0x01)
#define MID_AUTH        ((PID_CCP << 8) | 0x02)     /* ��ȫ��֤ģ�� */
#define MID_IGMP        ((PID_CCP << 8) | 0x03)     /* IGMP */
#define MID_CMM         ((PID_CCP << 8) | 0x04)     /* CMM */
#define MID_LAN         ((PID_CCP << 8) | 0x05)     /* LAN */
#define MID_IPT         ((PID_CCP << 8) | 0x06)     /* IPT */
#define MID_ETHLAN      ((PID_CCP << 8) | 0x07)     /* LAN����̫�� */
#define MID_ETHWAN      ((PID_CCP << 8) | 0x08)     /* WAN����̫�� */
#define MID_PPPOE       ((PID_CCP << 8) | 0x09)     /* PPPOE */
#define MID_WLAN        ((PID_CCP << 8) | 0x0a)     /* WLANģ�� */
#define MID_TR069BE     ((PID_CCP << 8) | 0x0b)     /* TR069BE */
#define MID_DGN         ((PID_CCP << 8) | 0x0c)     /* Diagnostics */
#define MID_DHCPR       ((PID_CCP << 8) | 0x0d)     /* dhcp relay */
#define MID_DHCPS       ((PID_CCP << 8) | 0x0e)     /* dhcp server */
#define MID_TIMER       ((PID_CCP << 8) | 0x0f)     /* Timer */
#define MID_IPCONN      ((PID_CCP << 8) | 0x10)     /* wan ip connection module */
#define MID_FIREWALL    ((PID_CCP << 8) | 0x11)     /* Fire wall */
#define MID_SNMPC       ((PID_CCP << 8) | 0x12)     /* SNMP���ù���ģ�� */
#define MID_QOS         ((PID_CCP << 8) | 0x13)     /* QOSģ�� */
#define MID_SROUTE      ((PID_CCP << 8) | 0x14)     /* static routing */
#define MID_STATIC_ROUTING  ((PID_CCP << 8) | 0x14) /* static routing */
#define MID_VDSL        ((PID_CCP << 8) | 0x15)     /* VDSLģ�� */
#define MID_DNS         ((PID_CCP << 8) | 0x16)     /* DNSģ�� */
#define MID_ALG         ((PID_CCP << 8) | 0x17)     /* ALGģ�� */
#define MID_WAN         ((PID_CCP << 8) | 0x18)     /* WANģ�� */
//#define MID_DYNAMIC_ROUTING  ((PID_CCP << 8) | 0x19)/* dynamic routing */
#define MID_DROUTE  ((PID_CCP << 8) | 0x19)/* dynamic routing */
#define MID_SNTP        ((PID_CCP << 8) | 0x1a)     /* sntp */
#define MID_VLAN        ((PID_CCP << 8) | 0x1b)     /* vlan */
#define MID_USB_MASS    ((PID_CCP << 8) | 0x1c)     /* USB mass storage module */
#define MID_LOG         ((PID_CCP << 8) | 0x1d)     /* LOGGER ģ�� */
#define MID_FTPD        ((PID_CCP << 8) | 0x1e)     /* FTPD module */
#define MID_NATPRIO     ((PID_CCP << 8) | 0x1f)     /* NAT ���ȼ� */
#define MID_WPS         ((PID_CCP << 8) | 0x20)     /* WPSģ�� */
#define MID_ACL         ((PID_CCP << 8) | 0x21)     /* ACL */
#define MID_UPNP        ((PID_CCP << 8) | 0x22)     /* UPNP */
#define MID_LSVLAN      ((PID_CCP << 8) | 0x23)     /* LSVLAN */
#define MID_PORTOFF     ((PID_CCP << 8) | 0x24)     /* PORTOFF:�˿ڸ��� */
#define MID_ANTIATTACK  ((PID_CCP << 8) | 0x25)     /* ANTIATTACK:������ */
#define MID_DEVINFO     ((PID_CCP << 8) | 0x26)     /* DEVINFOģ�� */
#define MID_PORTMAPPING ((PID_CCP << 8) | 0x27)     /* PortMappingģ�� */
#define MID_URLFILTER   ((PID_CCP << 8) | 0x28)     /* URL FILTER */
#define MID_ATM         ((PID_CCP << 8) | 0x29)     /* ATMģ�� */
#define MID_DSL         ((PID_CCP << 8) | 0x2a)     /* DSLģ�� */
#define MID_DDNS        ((PID_CCP << 8) | 0x2b)     /* DDNSģ�� */
#define MID_PROUTE      ((PID_CCP << 8) | 0x2e)     /* ����·�� */
#define MID_CFG         ((PID_CCP << 8) | 0x2f)     /* cfg�ָ�����Ĭ�ϱ����ؼ����� */
#define MID_SUPP        ((PID_CCP << 8) | 0x30)     /* SUPPģ�� */
#define MID_MACFILTER   ((PID_CCP << 8) | 0x31)     /* MACFILTERģ�� */
#define MID_TRACERT     ((PID_CCP << 8) | 0x32)     /* TRACERTģ��*/
#define MID_IPPD        ((PID_CCP << 8) | 0x33)     /* IPPDģ��*/
#define MID_WEBP        ((PID_CCP << 8) | 0x34)     /* web����ģ�� */
#define MID_BRIDGE_FILTER        ((PID_CCP << 8) | 0x35)     /* bridgefilterģ�� */
#define MID_SCHED       ((PID_CCP << 8) | 0x36)     /* schedules*/
#define	MID_PORTTRIGGER ((PID_CCP << 8) | 0x37)	    /*�˿ڴ���*/
#define MID_IP_ACL      ((PID_CCP << 8) | 0x38)     /* IP_ACL */
#define MID_DEFAULTGW   ((PID_CCP << 8) | 0x39)     /*Ĭ������*/
#define MID_UTMPROXY    ((PID_CCP << 8) | 0x3a)     /*utmproxy*/
#define MID_AUTOUPGRADE ((PID_CCP << 8) | 0x3b)     /*sitcom autoupgrade*/
#define MID_DIAG        ((PID_CCP << 8) | 0x40)     /* Diagnostics */
#define MID_WANSELECT   ((PID_CCP << 8) | 0x41)     /* WAN Select */
#define MID_DEVCONFIG   ((PID_CCP << 8) | 0x43)     /* DeviceConf */
#define MID_TRADITIONALNAT   ((PID_CCP << 8) | 0x44)     /* TraditionalNAT*/
#define MID_FIREWALLLOG ((PID_CCP << 8) | 0x45)     /*firewall log */
#define MID_IPMACFILTER ((PID_CCP << 8) | 0x46)        /* IPMACFilter */
#define MID_UDPECHO   ((PID_CCP << 8) | 0x47)               /*UDPEcho  TR143*/
#define MID_DOWNLOADDIAG   ((PID_CCP << 8) | 0x48)    /*DownloadDiagnostics  TR143*/
#define MID_UPLOADDIAG   ((PID_CCP << 8) | 0x49)    /*UpDiagnostics  TR143*/
#define MID_SAMBA       ((PID_CCP << 8) | 0x50)     /* SAMBA���ù���ģ�� */
#define MID_USB3G	((PID_CCP << 8) | 0x55)		/*PIN�����*/
#define MID_TF_PORTMAPPING	((PID_CCP << 8) | 0x59)		/*module id for web-wizard, response for lan eth-port bind*/

#define MID_SPT			((PID_CCP << 8) | 0x42)     /* Share Protection*/
#define MID_TR064C     ((PID_CCP << 8) | 0x51)     /* TR064BE */

//ipv6
#define MID_DHCP6S	    ((PID_CCP << 8) | 0x52)		/*dhcp6sģ��*/
#define MID_V6CONN      ((PID_CCP << 8) | 0x53)		/*ipv6 connectionģ��*/
#define MID_RAD         ((PID_CCP << 8) | 0x54)     /*router advertisement daemonģ��*/
//firewall for telefonica
#define MID_TF_FIREWALL       ((PID_CCP << 8) | 0x56)     /* firewall for telefonica */

//yuanchaoqun
#define MID_OS_INFO     ((PID_CCP << 8) | 0x57)    /* cpu �ڴ�nat ��Ϣ*/

#define MID_V6SROUTE      ((PID_CCP << 8) | 0x58)     /* ipv6 static routing*/


#define MID_LPVLAN      ((PID_CCP << 8) | 0x5a)  /*lan port vlan*/

//DLNA by chenkai
#define MID_DLNA	    ((PID_CCP << 8) | 0x60)

#define MID_IPTUNNEL      ((PID_CCP << 8) | 0x61)    /*IPTunnelģ�� */

#define MID_MLD         ((PID_CCP << 8) | 0x63)    /* MLDģ�� */
#define MID_TF_GUI      ((PID_CCP << 8) | 0x64)	   /* GUIģ��*/
#define MID_IPV6ENABLE  ((PID_CCP << 8) | 0x65)     /* ipv6 enable*/

#define MID_VOIPBE      ((PID_CCP << 8) | 0x67)     /* voip����ģ�� */
#define MID_SHAREPORT   ((PID_CCP << 8) | 0x68)    /* Shareportģ�� */
#define MID_PORTALMANAGE ((PID_CCP << 8) | 0x69)     /* ǿ���Ż�ģ�� */
#define MID_ONETOUCH     ((PID_CCP << 8) | 0x6a)     /* D-Link OneTouchģ��*/
#define MID_F5_DGN ((PID_CCP << 8) | 0x70)     /*F5���ģ�� */


//DLNA by chenkai
#define  MID_SELF_HELP  ((PID_CCP << 8) | 0x71) 

#define MID_NEW3G       ((PID_CCP << 8) | 0x99)   /*scb+ 2011-6-24 for new 3g*/
#define MID_NEW3GPIN  ((PID_CCP << 8) | 0x9a)   /*scb+ 2011-8-5 for new 3g pin*/
#define MID_NEW3GPINRESULT  ((PID_CCP << 8) | 0x9b)   /*scb+ 2011-8-6 for new 3g pin result*/
#define MID_NEW3GINFO  ((PID_CCP << 8) | 0x9c)   /*lbw+ 2011-9-21 for new 3g info*/
#define MID_CTMWBE      ((PID_CCP << 8) | 0x9d)     /* �м����� */
#define MID_WANMIRROR      ((PID_CCP << 8) | 0x9d)     /* �м����� */
#define MID_TR069DIAG			((PID_CCP << 8) | 0x9e)
#define MID_IPFILTER   ((PID_CCP << 8) | 0x9f)     /* IPFILTERģ�� */
#define MID_REDIRECT   ((PID_CCP << 8) | 0xA0)     /* REDIRECTģ�� */

#define MID_WEB         ((PID_WEB << 8) | 0x01)     /* WEBģ�� */
#define MID_LNB         ((PID_LNB << 8) | 0x01)     /* Load & Backup */
#define MID_CLI         ((PID_CLI << 8) | 0x01)     /* CLIģ�� */
#define MID_TR069FE ((PID_TR069FE << 8) | 0x01)     /* TR069FE ģ�� */


#define MID_TR069_HTTPD ((PID_TR069_HTTPD << 8) | 0x01)     /*TR069 httpd ģ��*/
#define MID_SNMPA    ((PID_SNMPA << 8)  | 0x01)     /* SNMP������� */

#define MID_PC      ((PID_PC << 8)    | 0x01)
#define MID_MON         ((PID_MON << 8)   | 0x02)       /* MON ģ�� */
#define MID_APPMON      ((PID_MON << 8)   | 0x03)       /* app MON ģ�� */
#define MID_LBT         ((PID_LBT << 8)   | 0x04)       /* LBT ģ�� */
#define MID_ELM     ((PID_ELM << 8)   | 0x01)       /* ELM ģ�� */
#define MID_UPG     ((PID_UPG << 8)   | 0x01)
#define MID_VDSLD   ((PID_VDSLD << 8) | 0x01)       /* VDSLD ģ�� */
#define MID_TM      ((PID_TM << 8)    | 0x01)     /*TMģ��*/
#define MID_TR064FE      ((PID_TR064FE << 8)    | 0x01)     /*TMģ��*/
#define MID_ONETOUCH_DAEMON  ((PID_ONETOUCH_DAEMON << 8) | 0x01)  /*D-Link OneTouch�ػ�����*/
#define MID_GOAHEAD_DAEMON   ((PID_GOAHEAD_DAEMON << 8) | 0x01)   /*D-Link GoAhead�ػ�����*/

#define MID_DROPBEAR  ((PID_DROPBEAR << 8) | 0x01)
#define MID_TELNET  ((PID_TELNET << 8) | 0x01)
#define MID_AUP		((PID_UPG << 8) | 0x02)        /* �Զ�����ģ�� */
#define MID_UPGCGI  ((PID_UPGCGI << 8)| 0x01)
#define MID_FTPUPG  ((PID_FTPUPG << 8)| 0x01)       /* FTP ����ģ�� */
#define MID_PTI     ((PID_PTI << 8)   | 0X01)
#define MID_MSG4UDP     ((PID_MSG4UDP << 8)   | 0X01)        /* MSG4UDPͨ��ģ�� */
#define MID_TFTPUPG ((PID_TFTPUPG << 8) | 0x01)
#define MID_HOTPLUG ((PID_HOTPLUG << 8) | 0x01)     /* HotPlug */
#define MID_RAMON       ((PID_RAMON << 8) | 0x01)
#define MID_IP6MON      ((PID_IP6MON << 8) | 0x01)
#define MID_IP6AAC      ((PID_IP6AAC << 8) | 0x01)
/* add by liuyousheng 1013.01.17 ���������󣬴� D-Link �к��� */
#define MID_VOIP        ((PID_VOIP << 8) | 0x01) /* voipģ�� */
#define MID_CTMDW       ((PID_CTMDW << 8) | 0x01)
#define MID_MSGSENDER   ((PID_MSGSENDER <<8)|0x01)
#define MID_CMDDAEMON   ((PID_CMDDAEMON <<8)|0x01)
#define MID_STUNC     ((PID_STUNC << 8) | 0x01)


#define MID_GRP_CCP ((PID_CCP << 8)   | FLG_MID_GRP)

#define MID_GRP_BASE   0xD0                         /* �ಥ���ַ */

#define MID_GRP_MACLIST_UPDATE   (((PID_CCP << 8) | MID_GRP_BASE) + 1)
#define MID_GRP_MON_LINK_UPDATE  (((PID_CCP << 8) | MID_GRP_BASE) + 2)
#define MID_GRP_WAN_LINK_UPDATE  (((PID_CCP << 8) | MID_GRP_BASE) + 3)
#define MID_GRP_WAN_CONN_UPDATE  (((PID_CCP << 8) | MID_GRP_BASE) + 4)
#define MID_GRP_DNS_UPDATE       (((PID_CCP << 8) | MID_GRP_BASE) + 5)
#define MID_GRP_WAN_DEL_INST     (((PID_CCP << 8) | MID_GRP_BASE) + 6)
#define MID_GRP_WAN_ADD_INST     (((PID_CCP << 8) | MID_GRP_BASE) + 7)
#define MID_GRP_TIME_CHANGED     (((PID_CCP << 8) | MID_GRP_BASE) + 9)
#define MID_GRP_WAN_CONN_ENABLE_UPDATE   (((PID_CCP << 8) | MID_GRP_BASE) + 10)
#define MID_GRP_LAN_DEL_INST     (((PID_CCP << 8) | MID_GRP_BASE) + 11)
#define MID_GRP_LAN_ADD_INST     (((PID_CCP << 8) | MID_GRP_BASE) + 12)

#define MID_GRP_WLAN_DEL_INST    (((PID_CCP << 8) | MID_GRP_BASE) + 13)
#define MID_GRP_WLAN_ADD_INST    (((PID_CCP << 8) | MID_GRP_BASE) + 14)
#define MID_GRP_LAN_ETH_INST_CHG (((PID_CCP << 8) | MID_GRP_BASE) + 15)

#define MID_GRP_LAN_IP_UPDATE (((PID_CCP << 8) | MID_GRP_BASE) + 16)
#define MID_GRP_DHCPS_DEVICE_MSG  (((PID_CCP << 8) | MID_GRP_BASE) + 17)
#define MID_GRP_LAN_NETADDR_CHG   (((PID_CCP << 8) | MID_GRP_BASE) + 18)
#define MID_GRP_WAN_CONN_SET_CHNG (((PID_CCP << 8) | MID_GRP_BASE) + 19)
#define MID_GRP_DEFAULT_CONN_CHNG (((PID_CCP << 8) | MID_GRP_BASE) + 20)

#define MID_GRP_WAN_BR_STATUS_CHNG (((PID_CCP << 8) | MID_GRP_BASE) + 21)
#define MID_GRP_WLAN_ENABLE_UPDATE (((PID_CCP << 8)| MID_GRP_BASE) + 22)
#define MID_GRP_LAN_GROUP_UPDATE  (((PID_CCP << 8) | MID_GRP_BASE) + 23)
#define MID_GRP_DEFAULT_ROUTE_CHANGE (((PID_CCP << 8) | MID_GRP_BASE) + 24)

//ipv6

#define MID_GRP_IP6CONN_ADDR_CHANGED  (((PID_CCP << 8) | MID_GRP_BASE) + 25)
#define MID_GRP_IP6LAN_ADDR_CHANGED   (((PID_CCP << 8) | MID_GRP_BASE) + 26)
#define MID_GRP_DFLT_IP6CONN_CHANGED  (((PID_CCP << 8) | MID_GRP_BASE) + 27)
#define MID_GRP_IP6LAN_PREFIX_CHANGED (((PID_CCP << 8) | MID_GRP_BASE) + 28)
#define MID_GRP_IP6CONN_DNS_CHANGED   (((PID_CCP << 8) | MID_GRP_BASE) + 29)
#define MID_GRP_IP6CONN_ROUTER_CHANGED  (((PID_CCP << 8) | MID_GRP_BASE) + 30)
//#define MID_GRP_WAN_DEL_CONN			(((PID_CCP << 8) | MID_GRP_BASE) + 31)
//#define MID_GRP_WAN_ADD_CONN			(((PID_CCP << 8) | MID_GRP_BASE) + 32)
#define MID_GRP_BR_BIND_PORT			(((PID_CCP << 8) | MID_GRP_BASE) + 33)
#define MID_GRP_BR_UNBIND_PORT			(((PID_CCP << 8) | MID_GRP_BASE) + 34)
#define MID_GRP_LAN_DEVICE_ADD_INST		(((PID_CCP << 8) | MID_GRP_BASE) + 35)
#define MID_GRP_LAN_DEVICE_DEL_INST		(((PID_CCP << 8) | MID_GRP_BASE) + 36)
#define MID_GRP_LAN_DEVICE_DOWN		(((PID_CCP << 8) | MID_GRP_BASE) + 37)
#define MID_GRP_LAN_DEVICE_UP		(((PID_CCP << 8) | MID_GRP_BASE) + 38)
#define MID_GRP_IP6LAN_IFCON_CHANGED (((PID_CCP << 8) | MID_GRP_BASE) + 39)

#define MID_GRP_PPPS_CLIENT_CHNG        (((PID_CCP << 8) | MID_GRP_BASE) + 40)
#define MID_GRP_MVLAN_CHANGED           (((PID_CCP << 8) | MID_GRP_BASE) + 41)
#define MID_GRP_IP6CONN_STAT_CHANGED  (((PID_CCP << 8) | MID_GRP_BASE) + 42)

#define MID_GRP_IP6LAN_DNS_CHANGED  (((PID_CCP << 8) | MID_GRP_BASE) + 43)

#define MID_NULL    ((PID_NULL << 8)    | FLG_MID_GRP)

/******************************************************************************
 *                                 ENUM                                       *
 ******************************************************************************/

/*������Ϣ����*/
enum
{
    MSG_POLL_EXIT       = 0x0001,

    MSG_PC_START_PROC,
    MSG_PC_SEND_SIGNAL,
    MSG_PC_PROC_STATE,
    MSG_PC_PROC_OUTPUT,
    MSG_PC_EXIT,
    MSG_PC_BTN_BAN,
    MSG_PC_BTN_RESUME,
    MSG_PC_BTN_TEST_END,              /*����ģʽ��BTN�������*/

    MSG_CMM_GET_VAL,             /* ��ȡ��Ϣ */
    MSG_CMM_SET_VAL,             /* ������Ϣ */
    MSG_CMM_ADD_NODE, /* 0009 */ /* �����Ϣ */
    MSG_CMM_DEL_NODE,            /* ɾ����Ϣ */
    MSG_CMM_GET_NAME,            /* ��ȡ��������, ���ر��ڵ���������� */
    MSG_CMM_GET_ATTR,
    MSG_CMM_SET_ATTR,

    MSG_CMM_INFORM_NOTI,        /* ������֪ͨ���Ի�ȡ */
    MSG_CMM_GET_NOTI,           /* ��ȡ֪ͨ */
    MSG_CMM_CLEAR_NOTI,         /* ���֪ͨ */

    MSG_CMM_COMMIT,
    MSG_CMM_CANCEL,
    MSG_CMM_UPDATE,  /* 0019 */

    MSG_CMM_SAVE_CFG,
    MSG_CMM_RECOVER_CFG,
    MSG_CMM_UPGRADE,
    MSG_CMM_BACKUP,
    MSG_CMM_REBOOT,


    MSG_TIMER_REGSTER,
    MSG_TIMER_UNREGSTER,
    MSG_TIMER_TIMEOUT,

    MSG_WAN_INTERFACE_STATE,
//    MSG_WAN_ADD_CONN,                    /*WAN����ʵ��������Ϣ*/
//    MSG_WAN_DEL_CONN,       /* 0029 */   /*WAN����ʵ��ɾ����Ϣ*/
    MSG_WAN_CONN_EST,  /* 0029 */
    MSG_WAN_CONN_FIN,
    MSG_WAN_LINK_CHG,


    MSG_LAN_IP_UPDATE,
    MSG_LAN_MACLIST_UPDATE,
    MSG_LAN_DOMAIN_NAME_UPDATE,

    MSG_GET_DHOST_LIST,
    MSG_DNS_CHANGE,

    MSG_WLAN_ADD_DEVICE,
    MSG_WLAN_DEL_DEVICE,


    MSG_TR069_SET_PARKEY,    /* 0039 */ /* ����ParameterKey ��Ϣ */
    MSG_TR069_GET_EVENTS,               /* ��ȡEvents ��Ϣ */
    MSG_TR069_CLEAR_EVENTS,             /* ���Events ��Ϣ */
    MSG_TR069_DOWNLOAD,                 /* ������Ϣ */
    MSG_TR069_UPLOADLOAD,               /* �ϴ���Ϣ */
    MSG_TR069_REBOOT,                   /* ������Ϣ */
    MSG_TR069_FACTORYRESET,             /* �ָ�������Ϣ */
    MSG_TR069_SCHEDULEINFORM,           /* ��ʱ֪ͨ��Ϣ */
    MSG_TR069_GET_TRANSFERCOMPLETEINFO, /* ��ȡ���������Ϣ */

    /* LBTģ����Ϣ���� */
    MSG_LBT_SET_STATE,   /* WLAN ģ����Ƶ���� */
    MSG_LBT_GET,     /* 0049 */   //wlanģ���wps��ť

    MSG_TR069_HTTPD_CONNECT,
    MSG_TR069_HTTPD_CREATE_UPFILE,

    MSG_ETHNET_LINK_STATE,
    MSG_ETHWAN_INTF_NAME,

    MSG_MON_INTF_REGISTER,
    MSG_MON_INTF_UNREGISTER,
    MSG_MON_INTF_STATUS_QUERY,
    MSG_MON_INTF_STATUS_INFORM,

    MSG_AUTH,                           /*  ��Ȩ��Ϣ */
    MSG_WAN_ADD_INST,                     /*WANʵ��������Ϣ*/
    MSG_WAN_DEL_INST,                    /*WANʵ��ɾ����Ϣ*/
    MSG_LAN_DEVICE_DEL_INST,              /*LANDeviceʵ��ɾ����Ϣ*/
    MSG_LAN_DEVICE_ADD_INST,              /*LANDeviceʵ��������Ϣ*/
    MSG_LAN_DEL_INST,
    MSG_LAN_ADD_INST,                    /*LANʵ��ɾ����Ϣ*/
    MSG_LAN_DEVICE_DOWN,                  /*LANDevice Down��Ϣ*/
    MSG_LAN_DEVICE_UP,                    /*LANDevice Up��Ϣ*/
    MSG_WLAN_DEL_INST,                    /*WLANʵ��ɾ����Ϣ*/
    MSG_WLAN_ADD_INST,                    /*WLANʵ��������Ϣ*/

    MSG_CMD,             /* ��չ����, ��ʾ����Ϣ���ڻ�������� */

    MSG_RESP,            /* ��ͨ��Ӧ��Ϣ */
    MSG_RESP_FRAGMENT,   /* ��ӦƬ��(���ܻ�Ӧ��Ϣ�ϴ�, �����Ƭ��Ӧ) */
    MSG_RESP_ERR,        /* ��Ӧ��Ϣ--���ش��� */

    MSG_CMM_CLEAR_CFG,   /* 0069 */

    MSG_BUTTON,          /* ��ť��Ϣ */

    MSG_SNTP_UPDATE_TIME,
    MSG_SNTP_TIME_CHANGED,
    MSG_NTPS_CHANGED,

    MSG_TR111_DEVICE_ADD,
    MSG_TR111_DEVICE_DEL,

    MSG_DIAGNOSTIC_COMPLETE,
    MSG_PING_COMPLETE,


    MSG_MSG4UDP_REGISTER,             /* ��MSG4UDPע�� */
    MSG_MSG4UDP_UNREGISTER,             /* ��MSG4UDPȡ��ע�� */
    MSG_MSG4UDP_MESSAGE,   /* 0079 */            /* MSG4UDP������Ϣ */

    MSG_WAN_CONN_ENABLE_UPDATE,

    MSG_VLAN_ADD_BRIDGE,
    MSG_VLAN_DEL_BRIDGE,
    MSG_VLAN_BIND_PORT,
    MSG_VLAN_UNBIND_PORT,

    MSG_VDSL_BRIEF_STATUS_OF_PORT,
    MSG_VDSL_EXTENDED_STATUS_OF_PORT,
    MSG_VDSL_STATUS,
    MSG_VDSL_SNR_STATUS,
    MSG_VDSL_BME_FIRMWARE_VERSION,   /* 0089 */

    MSG_LAN_ETH_DEL_INST,              /* LAN_ETHʵ��ɾ����Ϣ */
    MSG_LAN_ETH_ADD_INST,              /* LAN_ETHʵ��������Ϣ */

    MSG_PORTOFF_TRIGGER,

    MSG_CMM_INST_ADDED,   /* 0093 */
    MSG_CMM_INST_DELED,

    MSG_CMM_GET_VAL_IGNORE_ERR,             /* ��ȡ��Ϣ(���Բ����ڵĽڵ�) */
    MSG_TR069_REPORT_MON_EVENT,
    MSG_TR069_GET_MON_EVENT,
    MSG_DHCPS_DEVICE_CONNECTED,   /* 99 */
    MSG_DHCPS_DEVICE_DISCONNECTED,
    MSG_LAN_NETADDR_CHG,
    MSG_WAN_CONN_SET_CHNG, /* 0096 */
    MSG_UPDATE_MANAGE_ROUTE,
    MSG_DEFAULT_ROUTE_CHANGE,

    MSG_DEFAULT_CONN_CHNG,

    MSG_TR069_CONNECTSUCCESS,               /*TR069���ӳɹ���Ϣ*/
    MSG_TR069_GETINTERNETACCOUNT, /* 106 */ /*��ȡ���������ʺ���Ϣ*/

	MSG_DNS_CHANGE_BY_BACKUP,				/* ����������Ĭ�������л�->�л�DNS*/

    MSG_WAN_BR_STATUS_CHNG,                 /* �����͵�wan����״̬�ı� */

    MSG_INFORM_BIND_INFO,                   /*�󶨶������:�û���Ϣ���豸��Ϣ��ҵ����Ϣ*/

    MSG_WLAN_ENABLE_UPDATE,
    MSG_LAN_GROUP_UPDATE,

    MSG_VOIP_LED_SET,   /* VOIP����LED��Ϣ */
    MSG_VOIP_IPT_SET,  /* VOIP���÷���ǽ��Ϣ */
    MSG_VOIP_WAN_PARA_SET, /*VOIP WAN���Ӳ���������Ϣ*/
    MSG_VOIP_QOS_SET,   /*VOIPBE��QOS���õ���Ϣ*/
    MSG_VOIP_START_VTP, /*����VTP����Ϣ*/
    MSG_VOIP_VOIP_STATUS, /*VOIP״̬������Ϣ*/
    MSG_VOIP_PHONE_STATUS, /*PHONE״̬������Ϣ*/
    MSG_VOIP_WAN_EST,    /*����WAN������Ч*/
    MSG_VOIP_TR069_STATUS, /*����IAD�����Ϣ*/
    /* ���ں˽���ʹ�õ���Ϣ���� */
    MSG_BLP_BASE        = 0x0500,
    MSG_BLP_DEVCTL_ADD,
    MSG_BLP_DEVCTL_DEL,
    MSG_BLP_DEVCTL_QUERY,
    MSG_BLP_DEVCTL_CONFIG,

    MSG_WAN_ADD_LINK_DEV,
    MSG_WAN_DEL_LINK_DEV,
    MSG_WAN_LINK_DEV_STAT_INFORM,

    MSG_SNMPA_WLAN_DISABLED,
    MSG_SNMPA_WLAN_ENABLE,
    MSG_SNMPA_CFG_SAVECFG,
    MSG_SNMPA_AUTH_FAILED,

    MSG_USB_MODEM_LINK_CHG,
    MSG_USB_STORAGE_LINK_CHG,

	MSG_USB_PRINTER_LINK_CHG,
	MSG_UPG_UPGRADE_FAILED,
    /* Added by wangtao for dhcp option 121 */
    MSG_IPCONN_ADD_ROUTE,
    MSG_PPPV6_CONN_EST,
    MSG_PPPV6_CONN_FIN,

    MSG_IP6AAC_NOTIFY,
    MSG_IP6AAC_REQUEST,
    MSG_IP6AAC_RELEASE,
    MSG_IP6CONN_STAT_CHANGED, // ipv6 ����״̬�ı�󷢵���Ϣ���൱�ڽ������ӣ��Ͽ�����
    MSG_IP6CONN_ADDR_CHANGED,
    MSG_IP6CONN_DNS_CHANGED,
    MSG_IP6CONN_NTP_CHANGED,
    MSG_IP6CONN_DOMAIN_CHANGED,
    MSG_IP6CONN_PREFIX_CHANGED,
    MSG_IP6CONN_ROUTER_CHANGED,

    MSG_IPV6_ADDR_NOTIFY,
    MSG_IPV6_DFLTGW_NOTIFY,
    MSG_RS_FAILED,
    MSG_RA_UPDATE,

    MSG_DFLT_IP6CONN_CHANGED,
    MSG_IP6LAN_DOMAIN_NAME_UPDATE,
    MSG_IP6LAN_PREFIX_CHANGED,
    MSG_IP6LAN_DNS_CHANGED,
    MSG_IP6LAN_ADDR_CHANGED,
	MSG_IP6LAN_IFCON_CHANGED,
    MSG_CTMW_REGOK,
    MSG_TR069_UPGRADE_ERR,
    MSG_CTMW_UPLOAD,
    MSG_CTMW_DOWNLOAD,
    MSG_CTMW_SET_DEFAULT,
    MSG_CTMW_REBOOT,
    MSG_CTMW_FE_TRIGGER,

    MSG_CTMW_NOTI,
    MSG_MGNT_CONN_CHNG,
    MSG_CTMAINTN_OVER,
    MSG_AUTORUN_BIND,
    MSG_IPV6RD_PREFIX_SET,
    MSG_IPV6RD_PREFIX_UNSET,
    MSG_BRIDGE_BIND_PORT,
    MSG_BRIDGE_UNBIND_PORT,
    MSG_TRANSFER_COMPLETE,
    MSG_SETDEFAULT_KEY,
    MSG_DEVREG_NEEDFORCE,

    MSG_PPPS_CLIENT_CHNG,
	MSG_UPG_UPGRADER,

	MSG_BATCH_MESSAGE,
    MSG_TR069_ACS_CHANGED,                  /* ACS �����ı�֪ͨ��ͨ��option43 */
    MSG_CPU_RM_EVENT_CONNECTION,
    MSG_CPU_RM_EVENT_LOS,

    MSG_IPV6_SERVERTYPE_CHANGED,
    MSG_NO_DEFAULT_ROUTE,

    MSG_LOG_SET_LEVEL,

    MSG_CMDDAEMON_APPLYCMD_REQUEST,

    MSG_WAN_SERVERTYPE_CHANGE,

#ifdef CONFIG_LAST_SAVE_CFG
    MSG_CMM_SAVE_LAST_CFG,
    MSG_CMM_RECOVER_LAST_CFG,
#endif
    MSG_WLAN_WLANPASSWD,
	/*onetouch daemon��ʹ�õ�libstarter.so����logic�����ڵ�onetouchģ��ͨ��ʹ�õ���Ϣ���� */
    MSG_ONETOUCH_WAN_RESYNC,                /* ͬ��WAN���Ӳ��� */
    MSG_ONETOUCH_EASY_ROAMING_RESTART,      /* easyroaming daemon������ */
    MSG_ONETOUCH_WLAN_RESYNC,               /* ͬ��2.4G���߲��� */
    MSG_ONETOUCH_WLAN5_RESYNC,               /* ͬ��5.8G���߲��� */
    MSG_ONETOUCH_WLANALL_RESYNC,			/* ͬ���������߲��� */
    MSG_ONETOUCH_DEVICE_RESYNC,             /* ͬ���豸��Ϣ */
    MSG_ONETOUCH_CFG_SAVE_TO_FLASH,         /* �������õ�Flash */    
    MSG_ONETOUCH_WLAN_SITE_SURVEY,          /* ����վ��ɨ�� */
    MSG_ONETOUCH_WLAN_SITE_SURVEY_RESULT,   /* ����վ��ɨ���� */
    MSG_ONETOUCH_FWUPGRADE,                 /* ������� */    
    MSG_ONETOUCH_UPDATE_STA_RSSI,           /* ����STA RSSI��Ϣ */
    MSG_ONETOUCH_DEAUTH_STA,                /* STA���� */
    MSG_ONETOUCH_INIT_DXML_LAN,             /* ��ʼ��DXml���ݿ�lan�ڵ� */
    MSG_ONETOUCH_INIT_DXML_ONETOUCH,        /* ��ʼ��DXml���ݿ�onetouch�ڵ� */
    MSG_ONETOUCH_INIT_DXML_SYS,             /* ��ʼ��DXml���ݿ�sys�ڵ� */
    MSG_ONETOUCH_INIT_DXML_LAN_ACK,         /* ��ʼ��DXml���ݿ�lan�ڵ� */
    MSG_ONETOUCH_INIT_DXML_CONNECTTYPE,     /* ����hardwaretype�ڵ�ֵ */
    MSG_PUB_END      = 0x7FFF

} ;




#define TBS_MSG_ACK_TYPE(type)  (type | 0x8000)



/*������Ϣ��Ӧ��Ӧ������*/
#define MSG_POLL_EXIT_ACK                         TBS_MSG_ACK_TYPE(MSG_POLL_EXIT)

#define MSG_PC_START_PROC_ACK                     TBS_MSG_ACK_TYPE(MSG_PC_START_PROC)
#define MSG_PC_SEND_SIGNAL_ACK                    TBS_MSG_ACK_TYPE(MSG_PC_SEND_SIGNAL)
#define MSG_PC_PROC_STATE_ACK                     TBS_MSG_ACK_TYPE(MSG_PC_PROC_STATE)
#define MSG_PC_PROC_OUTPUT_ACK                    TBS_MSG_ACK_TYPE(MSG_PC_PROC_OUTPUT)
#define MSG_PC_EXIT_ACK                           TBS_MSG_ACK_TYPE(MSG_PC_EXIT)

#define MSG_CMM_GET_VAL_ACK                       TBS_MSG_ACK_TYPE(MSG_CMM_GET_VAL)
#define MSG_CMM_SET_VAL_ACK                       TBS_MSG_ACK_TYPE(MSG_CMM_SET_VAL)
#define MSG_CMM_ADD_NODE_ACK                      TBS_MSG_ACK_TYPE(MSG_CMM_ADD_NODE)
#define MSG_CMM_DEL_NODE_ACK                      TBS_MSG_ACK_TYPE(MSG_CMM_DEL_NODE)
#define MSG_CMM_GET_NAME_ACK                      TBS_MSG_ACK_TYPE(MSG_CMM_GET_NAME)
#define MSG_CMM_GET_ATTR_ACK                      TBS_MSG_ACK_TYPE(MSG_CMM_GET_ATTR)
#define MSG_CMM_SET_ATTR_ACK                      TBS_MSG_ACK_TYPE(MSG_CMM_SET_ATTR)
#define MSG_CMM_INFORM_NOTI_ACK                   TBS_MSG_ACK_TYPE(MSG_CMM_INFORM_NOTI)
#define MSG_CMM_GET_NOTI_ACK                      TBS_MSG_ACK_TYPE(MSG_CMM_GET_NOTI)
#define MSG_CMM_CLEAR_NOTI_ACK                    TBS_MSG_ACK_TYPE(MSG_CMM_CLEAR_NOTI)
#define MSG_CMM_COMMIT_ACK                        TBS_MSG_ACK_TYPE(MSG_CMM_COMMIT)
#define MSG_CMM_CANCEL_ACK                        TBS_MSG_ACK_TYPE(MSG_CMM_CANCEL)
#define MSG_CMM_UPDATE_ACK                        TBS_MSG_ACK_TYPE(MSG_CMM_UPDATE)
#define MSG_CMM_SAVE_CFG_ACK                      TBS_MSG_ACK_TYPE(MSG_CMM_SAVE_CFG)
#define MSG_CMM_RECOVER_CFG_ACK                   TBS_MSG_ACK_TYPE(MSG_CMM_RECOVER_CFG)
#ifdef CONFIG_LAST_SAVE_CFG
#define MSG_CMM_SAVE_LAST_CFG_ACK                      TBS_MSG_ACK_TYPE(MSG_CMM_SAVE_LAST_CFG)
#define MSG_CMM_RECOVER_LAST_CFG_ACK                   TBS_MSG_ACK_TYPE(MSG_CMM_RECOVER_LAST_CFG)
#endif
#define MSG_CMM_UPGRADE_ACK                       TBS_MSG_ACK_TYPE(MSG_CMM_UPGRADE)
#define MSG_CMM_BACKUP_ACK                        TBS_MSG_ACK_TYPE(MSG_CMM_BACKUP)
#define MSG_CMM_REBOOT_ACK                        TBS_MSG_ACK_TYPE(MSG_CMM_REBOOT)

#define MSG_TIMER_REGSTER_ACK                     TBS_MSG_ACK_TYPE(MSG_TIMER_REGSTER)
#define MSG_TIMER_UNREGSTER_ACK                   TBS_MSG_ACK_TYPE(MSG_TIMER_UNREGSTER)
#define MSG_TIMER_TIMEOUT_ACK                     TBS_MSG_ACK_TYPE(MSG_TIMER_TIMEOUT)

#define MSG_WAN_INTERFACE_STATE_ACK               TBS_MSG_ACK_TYPE(MSG_WAN_INTERFACE_STATE)
#define MSG_WAN_CONN_EST_ACK                      TBS_MSG_ACK_TYPE(MSG_WAN_CONN_EST)
#define MSG_WAN_CONN_FIN_ACK                      TBS_MSG_ACK_TYPE(MSG_WAN_CONN_FIN)
#define MSG_WAN_LINK_CHG_ACK                      TBS_MSG_ACK_TYPE(MSG_WAN_LINK_CHG)

#define MSG_LAN_IP_UPDATE_ACK                     TBS_MSG_ACK_TYPE(MSG_LAN_IP_UPDATE)
#define MSG_LAN_MACLIST_UPDATE_ACK                TBS_MSG_ACK_TYPE(MSG_LAN_MACLIST_UPDATE)
#define MSG_LAN_DOMAIN_NAME_UPDATE_ACK            TBS_MSG_ACK_TYPE(MSG_LAN_DOMAIN_NAME_UPDATE)

#define MSG_GET_DHOST_LIST_ACK                    TBS_MSG_ACK_TYPE(MSG_GET_DHOST_LIST)
#define MSG_DNS_CHANGE_ACK                        TBS_MSG_ACK_TYPE(MSG_DNS_CHANGE)
#define MSG_WLAN_ADD_DEVICE_ACK                   TBS_MSG_ACK_TYPE(MSG_WLAN_ADD_DEVICE)
#define MSG_WLAN_DEL_DEVICE_ACK                   TBS_MSG_ACK_TYPE(MSG_WLAN_DEL_DEVICE)

#define MSG_TR069_SET_PARKEY_ACK                  TBS_MSG_ACK_TYPE(MSG_TR069_SET_PARKEY)
#define MSG_TR069_GET_EVENTS_ACK                  TBS_MSG_ACK_TYPE(MSG_TR069_GET_EVENTS)
#define MSG_TR069_CLEAR_EVENTS_ACK                TBS_MSG_ACK_TYPE(MSG_TR069_CLEAR_EVENTS)
#define MSG_TR069_DOWNLOAD_ACK                    TBS_MSG_ACK_TYPE(MSG_TR069_DOWNLOAD)
#define MSG_TR069_UPLOADLOAD_ACK                  TBS_MSG_ACK_TYPE(MSG_TR069_UPLOADLOAD)
#define MSG_TR069_REBOOT_ACK                      TBS_MSG_ACK_TYPE(MSG_TR069_REBOOT)
#define MSG_TR069_FACTORYRESET_ACK                TBS_MSG_ACK_TYPE(MSG_TR069_FACTORYRESET)
#define MSG_TR069_SCHEDULEINFORM_ACK              TBS_MSG_ACK_TYPE(MSG_TR069_SCHEDULEINFORM)
#define MSG_TR069_GET_TRANSFERCOMPLETEINFO_ACK    TBS_MSG_ACK_TYPE(MSG_TR069_GET_TRANSFERCOMPLETEINFO)

#define MSG_LBT_SET_STATE_ACK                     TBS_MSG_ACK_TYPE(MSG_LBT_SET_STATE)
#define MSG_LBT_GET_ACK                           TBS_MSG_ACK_TYPE(MSG_LBT_GET)

#define MSG_TR069_HTTPD_CONNECT_ACK               TBS_MSG_ACK_TYPE(MSG_TR069_HTTPD_CONNECT)
#define MSG_TR069_HTTPD_CREATE_UPFILE_ACK         TBS_MSG_ACK_TYPE(MSG_TR069_HTTPD_CREATE_UPFILE)

#define MSG_ETHNET_LINK_STATE_ACK                 TBS_MSG_ACK_TYPE(MSG_ETHNET_LINK_STATE)
#define MSG_ETHWAN_INTF_NAME_ACK                  TBS_MSG_ACK_TYPE(MSG_ETHWAN_INTF_NAME)

#define MSG_MON_INTF_REGISTER_ACK                 TBS_MSG_ACK_TYPE(MSG_MON_INTF_REGISTER)
#define MSG_MON_INTF_UNREGISTER_ACK               TBS_MSG_ACK_TYPE(MSG_MON_INTF_UNREGISTER)
#define MSG_MON_INTF_STATUS_QUERY_ACK             TBS_MSG_ACK_TYPE(MSG_MON_INTF_STATUS_QUERY)
#define MSG_MON_INTF_STATUS_INFORM_ACK            TBS_MSG_ACK_TYPE(MSG_MON_INTF_STATUS_INFORM)

#define MSG_AUTH_ACK                              TBS_MSG_ACK_TYPE(MSG_AUTH)

#define MSG_WAN_DEL_INST_ACK                      TBS_MSG_ACK_TYPE(MSG_WAN_DEL_INST)
#define MSG_WAN_ADD_INST_ACK                      TBS_MSG_ACK_TYPE(MSG_WAN_ADD_INST)
#define MSG_LAN_DEL_INST_ACK                      TBS_MSG_ACK_TYPE(MSG_LAN_DEL_INST)
#define MSG_LAN_ADD_INST_ACK                      TBS_MSG_ACK_TYPE(MSG_LAN_ADD_INST)
#define MSG_WLAN_DEL_INST_ACK                     TBS_MSG_ACK_TYPE(MSG_WLAN_DEL_INST)
#define MSG_WLAN_ADD_INST_ACK                     TBS_MSG_ACK_TYPE(MSG_WLAN_ADD_INST)

#define MSG_CMD_ACK                               TBS_MSG_ACK_TYPE(MSG_CMD)
#define MSG_RESP_ACK                              TBS_MSG_ACK_TYPE(MSG_RESP)
#define MSG_RESP_FRAGMENT_ACK                     TBS_MSG_ACK_TYPE(MSG_RESP_FRAGMENT)
#define MSG_RESP_ERR_ACK                          TBS_MSG_ACK_TYPE(MSG_RESP_ERR)

#define MSG_CMM_CLEAR_CFG_ACK                     TBS_MSG_ACK_TYPE(MSG_CMM_CLEAR_CFG)

#define MSG_BUTTON_ACK                            TBS_MSG_ACK_TYPE(MSG_BUTTON)

#define MSG_SNTP_UPDATE_TIME_ACK                  TBS_MSG_ACK_TYPE(MSG_SNTP_UPDATE_TIME)
#define MSG_SNTP_TIME_CHANGED_ACK                 TBS_MSG_ACK_TYPE(MSG_SNTP_TIME_CHANGED)
#define MSG_NTPS_CHANGED_ACK                      TBS_MSG_ACK_TYPE(MSG_NTPS_CHANGED)

#define MSG_TR111_DEVICE_ADD_ACK                  TBS_MSG_ACK_TYPE(MSG_TR111_DEVICE_ADD)
#define MSG_TR111_DEVICE_DEL_ACK                  TBS_MSG_ACK_TYPE(MSG_TR111_DEVICE_DEL)

#define MSG_DIAGNOSTIC_COMPLETE_ACK                     TBS_MSG_ACK_TYPE(MSG_DIAGNOSTIC_COMPLETE)
#define MSG_PING_COMPLETE_ACK                     TBS_MSG_ACK_TYPE(MSG_PING_COMPLETE)

#define MSG_MSG4UDP_REGISTER_ACK                  TBS_MSG_ACK_TYPE(MSG_MSG4UDP_REGISTER)
#define MSG_MSG4UDP_UNREGISTER_ACK                TBS_MSG_ACK_TYPE(MSG_MSG4UDP_UNREGISTER)
#define MSG_MSG4UDP_MESSAGE_ACK                   TBS_MSG_ACK_TYPE(MSG_MSG4UDP_MESSAGE)

#define MSG_WAN_CONN_ENABLE_UPDATE_ACK            TBS_MSG_ACK_TYPE(MSG_WAN_CONN_ENABLE_UPDATE)

#define MSG_VLAN_ADD_BRIDGE_ACK                   TBS_MSG_ACK_TYPE(MSG_VLAN_ADD_BRIDGE)
#define MSG_VLAN_DEL_BRIDGE_ACK                   TBS_MSG_ACK_TYPE(MSG_VLAN_DEL_BRIDGE)
#define MSG_VLAN_BIND_PORT_ACK                    TBS_MSG_ACK_TYPE(MSG_VLAN_BIND_PORT)
#define MSG_VLAN_UNBIND_PORT_ACK                  TBS_MSG_ACK_TYPE(MSG_VLAN_UNBIND_PORT)

//ipv6
#define MSG_IP6AAC_REQUEST_ACK                  TBS_MSG_ACK_TYPE(MSG_IP6AAC_REQUEST)
#define MSG_IP6AAC_RELEASE_ACK                  TBS_MSG_ACK_TYPE(MSG_IP6AAC_RELEASE)

#define MSG_VDSL_BRIEF_STATUS_OF_PORT_ACK          TBS_MSG_ACK_TYPE(MSG_VDSL_BRIEF_STATUS_OF_PORT)
#define MSG_VDSL_EXTENDED_STATUS_OF_PORT_ACK      TBS_MSG_ACK_TYPE(MSG_VDSL_EXTENDED_STATUS_OF_PORT)
#define MSG_VDSL_STATUS_ACK                          TBS_MSG_ACK_TYPE(MSG_VDSL_STATUS)
#define MSG_VDSL_SNR_STATUS_ACK                      TBS_MSG_ACK_TYPE(MSG_VDSL_SNR_STATUS)
#define MSG_VDSL_BME_FIRMWARE_VERSION_ACK          TBS_MSG_ACK_TYPE(MSG_VDSL_BME_FIRMWARE_VERSION)

#define MSG_CMM_INST_ADDED_ACK                   TBS_MSG_ACK_TYPE(MSG_CMM_INST_ADDED)
#define MSG_CMM_INST_DELED_ACK                   TBS_MSG_ACK_TYPE(MSG_CMM_INST_DELED)

#define MSG_CMM_GET_VAL_IGNORE_ERR_ACK            TBS_MSG_ACK_TYPE(MSG_CMM_GET_VAL_IGNORE_ERR)
#define MSG_WLAN_WLANPASSWD_ACK                   TBS_MSG_ACK_TYPE(MSG_WLAN_WLANPASSWD)

#define PRIVATE_USB_STATUS "UsbStatus"
#define PRIVATE_USB_LINK_UP 1
#define PRIVATE_USB_LINK_DOWN 0
#define PRIVATE_USB_LINK_UP_STR "1"
#define PRIVATE_USB_LINK_DOWN_STR "0"

/******************************************************************************
 *                                 TYPEDEF                                    *
 ******************************************************************************/


/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/


/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/

/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/
//��Ʒģʽ����
#define FACTORY_MODE  "factory_settings"
#define DEFAULT_MODE  "default_settings"
#define USER_MODE     "user_settings"

#ifdef _cplusplus
    #if _cplusplus
        }
    #endif
#endif

#endif /*__TBS_TYPE_H__*/
