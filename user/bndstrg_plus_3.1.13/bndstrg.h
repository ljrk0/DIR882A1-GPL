/*
 ***************************************************************************
 * MediaTek Inc. 
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************

    Module Name:
    	bndstrg.h
*/

#ifndef __BNDSTRG_H__
#define __BNDSTRG_H__

#include <net/if.h>
#include <stdint.h>
#include "types.h"
#include "os.h"
#include "util.h"
#include "driver.h"
#include "event.h"
#include "debug.h"
#include "eloop.h"
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "driver_wext.h"
#include <ctype.h>

//#define BNDSTRG_BACKUP 1
#define IFNAME_2G "ra0"
#define IFNAME_5G "rai0"

#define HASH_TABLE_SIZE                 256
//#define MAC_ADDR_LEN				6
#define BAND_5G		                1
#define BAND_2G	                    2
#define BAND_2G5G                   (BAND_2G | BAND_5G)
#define BAND_5G_L	                4
#define BAND_5G_H	                8
#define BAND_INVALID                0x00

#define APMT2_PEER_PROBE_REQ		0
#define APMT2_PEER_DISASSOC_REQ     1
#define APMT2_PEER_ASSOC_REQ        2
#define APMT2_PEER_AUTH_REQ			3

#define BND_STRG_MAX_TABLE_SIZE	(64*3)
#define BND_STRG_TIMER_PERIOD	1000
#define BND_STRG_AGE_TIME		0			// in sec
#define BND_STRG_DORMANT_TIME	80			// in sec
#define BND_STRG_HOLD_TIME		50     		// in sec
#define BND_STRG_CHECK_TIME     30			// in sec
#define BND_STRG_RSSI_DIFF		30
#define BND_STRG_RSSI_LOW		-70
#define BND_STRG_DOWNSTEER_RSSI	-80
#define BND_STRG_UPSTEER_RSSI	-50
#define BND_STRG_N_DIFF			3
#define BND_STRG_NSS_THR_5GH	3
#define BND_STRG_NSS_THR_5GL	2
#define BND_STRG_NSS_THR_2G		1
#define BND_STRG_MIN_RSSI_2G	-65
#define BND_STRG_MIN_RSSI_5GH	-45
#define BND_STRG_MIN_RSSI_5GL	-55
#define BND_STRG_CHANLOAD_THR_2G	90
#define BND_STRG_CHANLOAD_THR_5GH	90
#define BND_STRG_CHANLOAD_THR_5GL	90
#define BND_STRG_ASSOC_THR		0
#define MAX_NVRAM_TABLE_SIZE		128
#ifdef VENDOR_FEATURE7_SUPPORT
#define NVRAM_TABLE_SIZE 		32
#else
#define NVRAM_TABLE_SIZE 		128
#endif
#define MAX_INF_NUM             3
#define MAX_REF_INF_NUM         3
#define CHANLOAD_HEAVY_CNT         10
#define MAX_STEERING_COUNT		10
#ifdef VENDOR_IOS_SUPPORT
#define MAX_IOS_NUM             5
#endif
#define DWELL_TIME				300			// time in sec 
#define MAX_STEER_TIME_WINDOW	2*60*60 	// time in sec  
#define BND_STRG_DATA_SAMPLE	10 			// no of sample used to calculate avarage utilization 
#define BND_STRG_DYNAMIC_STEERING_PERIOD 3 	//period of dynamic steering
#define BND_STRG_CHANLOAD_MAX_CNT_2G	10
#define BND_STRG_CHANLOAD_MAX_CNT_5G_L	10
#define BND_STRG_CHANLOAD_MAX_CNT_5G_H	10
#define TIME_TO_IDLE 			30
#define LOW_MCS_IDX 			3
#define LOW_CHANNEL_UTIL 		30
#define MCS_BAD_COUNT 			10
#define RSSI_CHECK_COUNT 		10
#define IDLE_RXTX_BYTE_COUNT	30
#define ENTRY_BACKUP_TIME 		30	
#define BND_STRG_PRIORITY_MAX	32
#define FILE_BUFFER_SIZE		1024
#ifndef H_CHANNEL_BIGGER_THAN
#define H_CHANNEL_BIGGER_THAN   			100 //144
#endif /* H_CHANNEL_BIGGER_THAN */
#define BND_STRG_SINGLE_BAND_TIMEOUT	86400 //24 hrs 60*60*24
#define BND_STRG_MAX_DISABLE_LIST 16
extern u8 H5G_channel_thres;
extern u8 fg_only_allow_monitor_addr;
#define IS_2G_BAND(_p)			(((_p)&BAND_2G)==BAND_2G)
#define IS_5G_BAND(_p)			(((_p)&BAND_5G)==BAND_5G)
#define IS_5G_L_BAND(_p)		(((_p)&BAND_5G_L)==BAND_5G_L)
#define IS_5G_H_BAND(_p)		(((_p)&BAND_5G_H)==BAND_5G_H)

#ifdef VENDOR_FEATURE7_SUPPORT
#define TIMEFORMAT "%F:%H:%M:%S"
#define BLACK_LIST_TIME			10
#define ControlFlag_ReadMask	(fBND_STRG_CLIENT_SUPPORT_2G | fBND_STRG_CLIENT_SUPPORT_H5G | fBND_STRG_CLIENT_SUPPORT_L5G)
#define BND_STRG_DISCONNECT_RSSI -75
#endif

#ifndef TRUE
#define TRUE					1
#endif
#ifndef FALSE
#define FALSE					0
#endif

#ifdef ROAMING_SUPPORT
#define IAPP_ETH_IFACE	"br0"
#define IAPP_ETH_PORT	2107
#define IAPP_MSG_SEND_TIMER		10
#define	ROAMING_ENTRY_TIMEOUT	120
#endif

//make sure that string rule id matches to entry_match_steering_str[] string array in bndstrg.c file
enum BND_STRG_PRIORITY_FLAGS {
	fBND_STRG_PRIORITY_RSSI_DIFF = 0	,
	fBND_STRG_PRIORITY_BAND_PERSIST		,
	fBND_STRG_PRIORITY_HT_SUPPORT		,
	fBND_STRG_PRIORITY_5G_RSSI			,
	fBND_STRG_PRIORITY_VHT_SUPPORT		,
	fBND_STRG_PRIORITY_NSS_SUPPORT		,
	fBND_STRG_PRIORITY_LB_CND_CHANLOAD_STATIC,
	fBND_STRG_PRIORITY_LB_CND_STA_CONNECTED_COUNT,
	fBND_STRG_PRIORITY_LB_CND_RSSI		,
	fBND_STRG_PRIORITY_LB_CND_MCS		,
	fBND_STRG_PRIORITY_DEFAULT_2G		,
	fBND_STRG_PRIORITY_DEFAULT_5G		,
	fBND_STRG_PRIORITY_RSSI_DOWNSTEER	,
	fBND_STRG_PRIORITY_LB_CND_CHANLOAD_IDLE,
	fBND_STRG_PRIORITY_LB_CND_CHANLOAD_ACTIVE,
	fBND_STRG_PRIORITY_RSSI_UPSTEER,
#ifdef VENDOR_IOS_SUPPORT
	fBND_STRG_PRIORITY_IOS_STEER,
#endif	
#ifdef VENDOR_FEATURE7_SUPPORT
	fBND_STRG_PRIORITY_DOWNSTEER_CHLOAD_RSSI,
	fBND_STRG_PRIORITY_UPSTEER_CHLOAD_RSSI,
#endif
	fBND_STRG_PRIORITY_MAX 				,//always to be the last
};

//make sure that string rule id matches to entry_match_chanload_str[] string array in bndstrg.c file
enum BND_STRG_CHANLOAD_MATCH_ID{
	fBND_STRG_CHANLOAD_ACTIVE_DOWN_STEER,
	fBND_STRG_CHANLOAD_ACTIVE_UP_STEER,
	fBND_STRG_CHANLOAD_MAX,        //always to be last
};

enum bndstrg_return_code {
	BND_STRG_SUCCESS = 0,
	BND_STRG_INVALID_ARG,
	BND_STRG_RESOURCE_ALLOC_FAIL,
	BND_STRG_TABLE_FULL,
	BND_STRG_TABLE_IS_NULL,
	BND_STRG_NOT_INITIALIZED,
	BND_STRG_2G_INF_NOT_READY,
	BND_STRG_5G_INF_NOT_READY,
	BND_STRG_STA_IS_CONNECTED,
	BND_STRG_UNEXP,
#ifdef ROAMING_SUPPORT
	BND_STRG_UDP_INIT_FAILED,
#endif /*ROAMING_SUPPORT*/
};

enum bndstrg_command {
	BND_SET_CND_PRIORITY = 0,
	BND_SET_DWELL_TIME,
	BND_SET_STEER_TIME_WINDOW,
	BND_SET_MAX_STEER_COUNT,
	BND_SET_AGETIME,
	BND_SET_CHECKTIME,
	BND_SET_HOLDTIME,
	BND_SET_RSSILOW,
	BND_SET_RSSIDIFF,
	BND_SET_STEERING_NUM,
	BND_SET_ASSOC_TH,
	BND_SET_LOAD_TH,
	BND_SET_MIN_RSSI_TH,
	BND_SET_NSS_TH,
	BND_SET_STA_POLL_PRD,
	BND_SET_DAEMON_STATE,
	BND_SET_ADD_WHITE_LIST,
	BND_SET_REMOVE_WHITE_LIST,
#ifdef BTM_DEBUG_SUPPORT
	BND_SEND_BTM, /* For BTM manual steering debug only */
#endif
	BND_SET_RSSI_DOWNSTEER,
	BND_SET_RSSI_UPSTEER,
	BND_SET_NVRAM,
#ifdef VENDOR_IOS_SUPPORT
	BND_SET_IOS_STEER_NUMBER,
#endif
#ifdef	VENDOR_FEATURE7_SUPPORT
	BND_SET_RSSI_DISCONNECT,
#endif
    BND_SET_RSSI_CHECK_COUNT,
    BND_SET_IDLE_RX_BYTE_COUNT,
    BND_SET_IDLE_TX_BYTE_COUNT,
	BND_SET_MAX_COMMAND, //always to be the last
};

enum bndstrg_control_flags {
	fBND_STRG_CLIENT_SUPPORT_2G			    = (1 << 0),
	fBND_STRG_CLIENT_SUPPORT_L5G			= (1 << 1),
	fBND_STRG_CLIENT_SUPPORT_H5G			= (1 << 2),
	fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G	    = (1 << 3),
	fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G	= (1 << 4),
	fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G	= (1 << 5),
	fBND_STRG_CLIENT_NOT_SUPPORT_HT_2G	    = (1 << 6),	
	fBND_STRG_CLIENT_NOT_SUPPORT_HT_L5G	    = (1 << 7),
	fBND_STRG_CLIENT_NOT_SUPPORT_HT_H5G		= (1 << 8),
	fBND_STRG_CLIENT_LOW_RSSI_2G		    = (1 << 9),
	fBND_STRG_CLIENT_LOW_RSSI_L5G		    = (1 << 10),
	fBND_STRG_CLIENT_LOW_RSSI_H5G		    = (1 << 11),
	fBND_STRG_CLIENT_IS_2G_ONLY			    = (1 << 12),
	fBND_STRG_CLIENT_IS_5G_ONLY			    = (1 << 13),
	fBND_STRG_CLIENT_SUPPORT_VHT		    = (1 << 14),
	fBND_STRG_CLIENT_RE_STEERING		    = (1 << 15),
};

enum bndstrg_frame_check_flags {
	fBND_STRG_FRM_CHK_PRB_REQ			= (1 << 0),
	fBND_STRG_FRM_CHK_ATH_REQ			= (1 << 1),
	fBND_STRG_FRM_CHK_ASS_REQ			= (1 << 2),
};

enum bndstrg_compare_mode{
	CMP_5GH_5GL = 0,
	CMP_5GH_2G,
	CMP_5GL_2G,
};

enum bndstrg_state {
	BNDSTRG_INIT,
	BNDSTRG_INF_POLL,
	BNDSTRG_TBL_EN,
	BNDSTRG_TBL_READY,
	BNDSTRG_LEAVE,
	BNDSTRG_FROZEN,//debug mode
};

enum bndstrg_band_index {
	IDX_2G = 0,
	IDX_5GL = 1,
	IDX_5GH = 2,
};

enum bndstrg_stop_reason_code{
	BNDSTRG_DAEMON_TBL_FULL = 0,
	BNDSTRG_DRIVER_TBL_FULL,
	BNDSTRG_LIMIT,
	MAX_REASON_CODE,
};

enum bndstrg_btm_state{
	btm_init = 0,
	btm_req_tx = 1,	
	btm_rsp_rx = 2,
};

enum bndstrg_mode{
	PRE_CONNECTION_STEERING	= 0x01,
	POST_CONNECTION_STEERING = 0x02,
};

enum bndstrg_btm_mode{
	BTM_Off				=	0x00,
	BTM_Only			=	0x01,
	BTM_Force_Steer		=	0x02,
};

enum bndstrg_list_type {
	bndstrg_whitelist	=	1,
	bndstrg_blacklist	=	2,
};

#ifdef ROAMING_SUPPORT
struct bndstrg_iapp_sta_entry {
	u32 ap_ip_addr;
	u32 elapsed_connection_time;
	u32 control_flags;
	u8 addr[6];
	u8 band;
	u8 reserved;
};

struct bndstrg_iapp_hdr {
	u32 sender_ip;
	u16 length;	//length of remaining buffer
	u8 command;
	u8 sta_count;
};

struct bndstrg_iapp_frame {
	struct bndstrg_iapp_hdr iapp_hdr;
	u8	buf[0];
};
#endif /*ROAMING_SUPPORT*/

struct data_avg{
	u32 data[BND_STRG_DATA_SAMPLE];
	u32 avg;
	u8	idx;	
};

struct bndstrg_command_id_name{
	u32 id;
	char *name;
};

struct bndstrg_entry_stat {
	s8	Rssi;
	u8	AuthReqCount;
	u8	ProbeReqCount;
#ifdef BND_STRG_DBG
	u8 	AssocCount;
	u8 	MacDelCount;
	char	FirstAuthTime[16];
	char	FirstProbeTime[16];
	char	FirstAssocTime[16];
	char	FirstClientDelTime[16];
	char	LastAuthTime[16];
	char	LastProbeTime[16];
#endif
	u8	got_cli_status;
	s8	data_Rssi;
	u32	data_tx_Rate;
	u32	data_rx_Rate;
	u32	data_tx_TP;
	u32	data_rx_TP;
	u8 	data_tx_Phymode;
	u8 	data_rx_Phymode;
	u8 	data_tx_mcs;
	u8 	data_rx_mcs;
	u8 	data_tx_bw;
	u8 	data_rx_bw;
	u8 	data_tx_sgi;
	u8 	data_rx_sgi;
	u8 	data_tx_stbc;
	u8 	data_rx_stbc;
	u8 	data_tx_ant;
	u8 	data_rx_ant;
	struct data_avg data_tx_util;
	struct data_avg data_rx_util;
	struct data_avg data_tx_byte;
	struct data_avg data_rx_byte;
	u64	data_tx_packets;
	u64	data_rx_packets;
};

enum OPMODE {
	OP_NO_BNDSTR,
	OP_DUALBAND,
	OP_TRIBAND,
};

enum PhyMode {
	fPhyMode_Legacy,
	fPhyMode_HT,
	fPhyMode_VHT,
	fPhyMode_Max,
};

enum STATUS {
	CLI_INIT = 0,
	CLI_ACTIVE = 1,
	CLI_NOTACTIVE = 2,
};

struct bndstrg_nvram_client{
	u8 Addr[MAC_ADDR_LEN];
	u8 Manipulable;
	u8 PhyMode;
	u8 Band;
	u8 Nss;
#ifdef	VENDOR_FEATURE7_SUPPORT
	u32 ControlFlags;
	struct tm timeinfo;
#endif
};

struct client_Steer_info{
	struct timespec steer_tp[MAX_STEERING_COUNT];	
	u8	band[MAX_STEERING_COUNT];
	//u8 	start_idx;
	u8 	end_idx;
};

struct bndstrg_ref_iface {
    u8  bValid;
    u8	bInfReady;
    char ucIfName[32];
};

struct bndstrg_btm_info{	
	u8 BTMSupport;
	u8 BTMSourceBand;
	u8 BTMState;
	u8 BTMRsp;
};

struct bndstrg_iface {
    u8      bValid;
    u8		bInfReady;
    u8		binfEnable;
    u8		Band;
    u8		Channel;
    u8 	    bVHTCapable;
    unsigned long table_src_addr;
    u8      Size;
    char	ucIfName[32];
	u8		nss;
	u8		min_nss_thr; //client should have this min nss to connect this interface
	s8		min_rssi_thr; //client should have min rssi to connect this interface
	u8		chanload_thr; //min chanload limit for which interface considered as busy
	u8		chanload;
	u8		chanload_heavy_max_cnt;
	u8		chan_busy_load;
	u8		obss_load;
	u8		edcca_load;
	u8		myair_load;
	u8		mytxair_load;
	u8		myrxair_load;
	u32 	chanload_heavy_cnt;
	struct data_avg avg_chanload;
	struct bndstrg_cli_entry *worst_entry;
    struct  bndstrg_ref_iface ref_iface[MAX_REF_INF_NUM];
	u32 	max_driver_table_size;
	u32		driver_table_size;
	RRM_NEIGHBOR_REP_INFO NeighborRepInfo;
};

struct bndstrg_ctrl_iface {
    u32 	Size;
    s8     ActiveCount;
    s8     Active2GCount;
    s8     Active5GCount;
    struct bndstrg_iface inf[MAX_INF_NUM];
};

/* make sure to add enum entry to entry_state string pointer in bndstrg.c file*/
enum bndstrg_entry_state {
	ENTRY_INIT,
	ENTRY_READY_TO_ASSOC,
	ENTRY_ASSOC_RCD,
	ENTRY_DELETE_RCD,
	ENTRY_OPER_STEER_ACTIVE,
};

#ifdef WPS_SUPPORT
enum wps_state {
	NON_WPS_CONNECTION,
	WPS_CONNECTION,
	WPS_FIRST_DISCONNECTION,
};
#endif /*WPS_SUPPORT*/

struct bndstrg_cli_entry {
	u8 				bValid;
	u8				TableIndex;
	u8				bActiveStatus;
	u8				bConnStatus;
	u8				Channel;
	u8				insert_from_which_channel;
	u32 			Control_Flags;
	u8 				Addr[MAC_ADDR_LEN];
	struct bndstrg_entry_stat statistics[MAX_INF_NUM];
	struct bndstrg_cli_entry *pNext;
	u8				Nss;
	u8 				Manipulable;
	u8 				band;/* prefer band for steering client */
	u8 				connected_band;
	u32				match_steered_rule_id[MAX_INF_NUM];
	u8				match_lb_chanload_active_cond_id[MAX_INF_NUM];
	struct timespec	tp;					/* timestamp when insert-entry */
	u32  			elapsed_time; 		/* sec */
	struct timespec activity_tp;		/* timestamp when last tx/rx or backup entry update*/
	u32  			idle_elapsed_time;	/* sec */
	u8 				steered;
	u8 				total_steered_cnt;
	u8				enable_compare_flag;
	u8				tx_mcs_bad_cnt;
	u8				rx_mcs_bad_cnt;
	u8				low_rssi_bad_cnt;
	u8				good_rssi_cnt;
	u8				Isbad;
	u8				Operation_steered;
	u8				Operation_steered_tick;
	u8				IsIdle;
	u8				state;
	u8				AssocProbeInfoMissMatch;
	struct bndstrg_btm_info btm_info;
	struct client_Steer_info CliSteerInfo;
#ifdef VENDOR_FEATURE7_SUPPORT
	u8				low_rssi_disconnect_cnt;
	u8				BlackListTimer;
#endif
#ifdef WPS_SUPPORT
	u8				WpsAssoc;		//WpsAssoc State
	u8				WpsFirstAssocCounter;		//timer for wps second connection
#endif /*WPS_SUPPORT*/
	u8 				bAllowStaConnectImmediately;   /* For IOS immediately connect */
	u8           	Recv_AuthTime_1;
	u8           	Recv_AuthTime;
};

#ifdef ROAMING_SUPPORT
typedef struct _ROAMING_LIST_ENTRY
{
	struct _ROAMING_LIST_ENTRY *pNext;
	struct bndstrg_iapp_sta_entry iapp_sta_entry;
	struct os_time last_update;
} BS_LIST_ENTRY, *PBS_LIST_ENTRY;

typedef struct _RT_LIST_ENTRY
{
	struct _RT_LIST_ENTRY *pNext;
} RT_LIST_ENTRY;

typedef struct _LIST_HEADER
{
	RT_LIST_ENTRY *pHead;
	RT_LIST_ENTRY *pTail;
	u8 size;
} LIST_HEADER, *PLIST_HEADER;
#endif /*ROAMING_SUPPORT*/

struct bndstrg_cli_table {
	u8		 bInitialized;
	u8		 bEnabled;
	u8		 Band;
    u8       op_mode;
	u8		 b2GInfReady;
	u8		 b5GInfReady;
	s8		RssiDiff;			/* if Rssi2.4G > Rssi5G by RssiCheck, then allow client to connect 2.4G */
	s8	 	RssiLow;			/* if Rssi5G < RssiLow, then this client cannot connect to 5G */
	s8		RSSILowDownSteer;
	s8		RSSIHighUpSteer;
#ifdef	VENDOR_IOS_SUPPORT
	u8		IOSNumLimit;
#endif
	u32       RssiCheckCount;
	u32       IDLE_RX_Byte_Count;
	u32       IDLE_TX_Byte_Count;
#ifdef	VENDOR_FEATURE7_SUPPORT
	s8		RSSIDisconnect;
	u8		BlackListTime;
#endif
	u32		AgeTime;			/* Entry Age Time (ms) */
	u32		DormantTime;
	u32		HoldTime;		/* Time for holding 2.4G connection rsp (ms) */
	u32		CheckTime;	/* Time for deciding if a client is 2.4G only (ms) */
	u32 	Size;
	u32 	max_steering_size;
	u32		assoc_bl_th;
	u32		dynamic_bndstrg_period;
	u32		polling_sta_period;
	struct bndstrg_cli_entry 	Entry[BND_STRG_MAX_TABLE_SIZE];
	struct bndstrg_cli_entry* 	Hash[HASH_TABLE_SIZE];
	int PriorityList[fBND_STRG_PRIORITY_MAX];
	u8  PriorityListSize;
	u8 	active_client_2G;	
	u8 	active_client_5G;
	u8 	active_client_L5G;
	u8 	active_client_H5G;
	u8	n_diff;
	struct bndstrg_nvram_client nvram_entry[MAX_NVRAM_TABLE_SIZE];
	u8 nvram_table_size;
	u8 nvram_reset;
	u8 bndstrg_nvram_client_count;
#ifdef BND_STRG_DBG
	unsigned char MonitorAddr[MAC_ADDR_LEN];
#endif /* BND_STRG_DBG */
	u32 debug_only_allow_monitor_addr;
	u32	dwell_time;		/* in sec */	/* Min time client need to stay in steered band before next steering */
	u8 	max_steer_count;
	u32 max_steer_time_window; /* in sec*/ /* window time for which max steering allowed*/
	u64 bndstrg_run_time;
	u8	chanload_priority_enabled;
	u8	fDynamicSteering;
	u8	fStopBndstrg;
	u32		single_band_timeout;
	u8		WhiteEntryList[BND_STRG_MAX_DISABLE_LIST][MAC_ADDR_LEN];
	u8		WhiteEntryListSize;
	u8		BtmMode;
	u8		BndStrgMode;
	struct bndstrg *bndstrg;
#ifdef ROAMING_SUPPORT
	u8	iapp_counter;	//counter for broadcasting iapp msg with connected stations info
	LIST_HEADER roaming_list;
	char eth_iface[16];	//ethernet interface name used for inter ap access
	u16	eth_port;		//ethernet port
	u32	iapp_msg_send_timer;	//time in sec after which iapp msg be sent for connected stations
	u32	roaming_entry_timeout;	//if no entry update, time in seconds after which a roaming station entry be deleted 
#endif /*ROAMING_SUPPORT*/
};

enum ACTION_CODE{
	CLI_EVENT = 1,
	CLI_ADD,
	CLI_DEL,
	CLI_STATUS_REQ,
	CLI_STATUS_RSP,
	CHANLOAD_STATUS_REQ,
	CHANLOAD_STATUS_RSP,
	INF_STATUS_QUERY,
	INF_STATUS_RSP,
	TABLE_INFO,
	ENTRY_LIST,
	BNDSTRG_ONOFF,
	SET_MNT_ADDR,
	NVRAM_UPDATE,
	REJECT_EVENT,
	HEARTBEAT_MONITOR,
	BNDSTRG_WNM_BTM,
	BNDSTRG_PARAM,
	BNDSTRG_NEIGHBOR_REPORT,
	UPDATE_WHITE_BLACK_LIST,
};

/* HEARTBEAT_MONITOR */
struct bnd_msg_heartbeat {
    s8 ucIfName[32];
};

struct bnd_msg_cli_probe {
	u8 bAllowStaConnectInHt;
	u8 bIosCapable;          /* For IOS immediately connect */
	u8	bVHTCapable;
	u8	Nss;
	signed char	Rssi[4];
};

struct bnd_msg_cli_auth {
	signed char	Rssi[4];
};

struct bnd_msg_cli_assoc {
	u8 	bAllowStaConnectInHt;
	u8	bVHTCapable;
	u8	Nss;
	u8	BTMSupport;
#ifdef WPS_SUPPORT	
	u8	bWpsAssoc;			//WpsAssoc flag
#endif /*WPS_SUPPORT*/
};

struct bnd_msg_cli_delete {
};

/* CLI_EVENT */
struct bnd_msg_cli_event {
	u8	FrameType;
    u8	Band;
	u8   Channel;
	unsigned char	Addr[MAC_ADDR_LEN];
	union {
		struct bnd_msg_cli_probe cli_probe;
		struct bnd_msg_cli_auth cli_auth;
		struct bnd_msg_cli_assoc cli_assoc;
		struct bnd_msg_cli_delete cli_delete;
	} data;
};

/* CLI_ADD */
struct bnd_msg_cli_add {
    u8	TableIndex;
    unsigned char Addr[MAC_ADDR_LEN];
};

/* CLI_DEL */
struct bnd_msg_cli_del {
    u8	TableIndex;
    unsigned char Addr[MAC_ADDR_LEN];
};
/* CLI_STATUS_REQ */
struct bnd_msg_cli_status_req {
	//u8 TableIndex;
	//unsigned char Addr[MAC_ADDR_LEN];
};

/* CLI_STATUS_RSP */
struct bnd_msg_cli_status_rsp {
    u8 	TableIndex;
	u8	ReturnCode;
	unsigned char Addr[MAC_ADDR_LEN];
	s8 	data_Rssi;
	u32 	data_tx_Rate;
	u32 	data_rx_Rate;
	u64	data_tx_Byte;
	u64	data_rx_Byte;
	u8 	data_tx_Phymode;
	u8 	data_rx_Phymode;
	u8 	data_tx_mcs;
	u8 	data_rx_mcs;
	u8 	data_tx_bw;
	u8 	data_rx_bw;
	u8 	data_tx_sgi;
	u8 	data_rx_sgi;
	u8 	data_tx_stbc;
	u8 	data_rx_stbc;
	u8 	data_tx_ant;
	u8 	data_rx_ant;
	u64	data_tx_packets;
	u64	data_rx_packets;
};

struct bnd_msg_chanload_status_req {
    s8 ucIfName[32];
//	u8 band;
//	u8 Channel;
};

struct bnd_msg_chanload_status_rsp {
    u8 ReturnCode;
	u8 band;
	u8 Channel;
	//TBD
	u8 	chanload;
	u8	chan_busy_load;
	u8	obss_load;
	u8	edcca_load;
	u8	myair_load;
	u8	mytxair_load;
	u8	myrxair_load;
};

/* INF_STATUS_QUERY */
struct bnd_msg_inf_status_req {
    s8 ucIfName[32];
};

/* set band steering paramater */
struct bnd_msg_param {
	u8 Band;
	u8 Channel;
	u8 len;
	char arg[64];
};

struct bnd_msg_neighbor_report
{
	unsigned char Addr[MAC_ADDR_LEN];
	u8 Band;
	u8 Channel;
	char  NeighborRepInfo[64];
};

/* INF_STATUS_RSP_5G / INF_STATUS_RSP_2G */
struct bnd_msg_inf_status_rsp {
    u8 bInfReady;
    u8 Channel;
    u8 bVHTCapable;
    unsigned long table_src_addr;
    char ucIfName[32];
	u8 nvram_support;
	u8 nss;
	u8 band;
	u32 table_size;	
};

/* BNDSTRG_ONOFF */
struct bnd_msg_onoff {
    u8  Band;
    u8  Channel;
    u8 	OnOff;
	u8	BndStrgMode;
    s8  ucIfName[32];
};

/* SET_MNT_ADDR */
struct bnd_msg_mnt_addr {
    unsigned char Addr[MAC_ADDR_LEN];
};

/* NVRAM_UPDATE */
struct bnd_msg_nvram_entry_update {
    unsigned char Addr[MAC_ADDR_LEN];
	struct bndstrg_nvram_client nvram_entry;
};

struct bnd_msg_reject_body {
    unsigned int DaemonPid;
};

/*display type for list*/
struct bnd_msg_display_entry_list {
    u32 display_type;
	u8 filer_band;
	u8 channel;
};

struct bnd_msg_update_white_black_list {
    s8 ucIfName[32];
	u8 list_type;
	unsigned char Addr[MAC_ADDR_LEN];
	u8 	deladd;
};

struct bnd_msg_wnm_command {
	u8 wnm_data[64];
};

struct bndstrg_msg{
   u8  Action;
   union {
        struct bnd_msg_cli_event cli_event;
        struct bnd_msg_cli_add cli_add;
        struct bnd_msg_cli_del cli_del;
        struct bnd_msg_cli_status_req cli_status_req;
        struct bnd_msg_cli_status_rsp cli_status_rsp;
		struct bnd_msg_chanload_status_req chanload_status_req;
        struct bnd_msg_chanload_status_rsp chanload_status_rsp;
        struct bnd_msg_inf_status_req inf_status_req;
        struct bnd_msg_inf_status_rsp inf_status_rsp;
        struct bnd_msg_onoff onoff;
        struct bnd_msg_mnt_addr mnt_addr;
        struct bnd_msg_nvram_entry_update entry_update;
        struct bnd_msg_reject_body reject_body;
        struct bnd_msg_display_entry_list display_type;
		struct bnd_msg_heartbeat heartbeat;
		struct bnd_msg_wnm_command wnm_cmd_data;
		struct bnd_msg_param bndstrg_param;
		struct bnd_msg_neighbor_report NeighborReport;
        struct bnd_msg_update_white_black_list	update_white_black_list;
   } data;
};


/* Use for I/O between driver and daemon */
/* Must make sure the structure is the same as the one in driver */
struct bndstrg {
	/* driver interface operation */
	const struct bndstrg_drv_ops *drv_ops;

	/* event operation */
	const struct bndstrg_event_ops *event_ops;

	/* driver interface data */
	void *drv_data;

	/* control interface */
	struct bndstrg_ctrl_iface ctrl_iface;
	
	u8 version;

	struct bndstrg_cli_table table;
	
	enum bndstrg_state state;
	
	u8	inf_stable_cnt;
	
	u8 nvram_support;

#ifdef ROAMING_SUPPORT
	//udp: iapp
	char			IfNameEth[IFNAMSIZ]; /* ethernet interface name */
	int			SocketUdpSend; /* used to send/rcv IAPP broadcast frame */
	struct in_addr	AddrOwn; /* IP address of ethernet interface */
	struct in_addr	AddrNetmask; /* netmask of ethernet interface */
	struct in_addr	AddrBroadcast; /* broadcast address of ethernet interface */
#endif /*ROAMING_SUPPORT*/
};

int bndstrg_init(struct bndstrg *bndstrg, struct bndstrg_event_ops *event_ops, int drv_mode, int opmode, int version);
int bndstrg_deinit(struct bndstrg *bndstrg);
void bndstrg_run(struct bndstrg *bndstrg);
void bndstrg_stop(struct bndstrg *bndstrg, u8 reason_code);
struct bndstrg_cli_entry * bndstrg_get_old_entry(struct bndstrg *bndstrg, struct bndstrg_iface *inf);
struct bndstrg_cli_entry * bndstrg_get_old_driver_entry(struct bndstrg *bndstrg, struct bndstrg_iface *inf, unsigned char *pAddr);
u8 bndstrg_client_band_update(struct bndstrg *bndstrg,struct bndstrg_cli_entry *entry,u8 band);
void bndstrg_cli_status_rsp (struct bndstrg *bndstrg, struct bnd_msg_cli_status_rsp *cli_status_rsp);
void bndstrg_update_btm_status(struct bndstrg *bndstrg, struct bndstrg_cli_entry *entry);
void bndstrg_update_probe_info(struct bndstrg *bndstrg, struct bndstrg_iface *inf,struct bndstrg_cli_entry *entry, struct bnd_msg_cli_event *cli_event);
void bndstrg_update_auth_info(struct bndstrg *bndstrg, struct bndstrg_iface *inf,struct bndstrg_cli_entry *entry, struct bnd_msg_cli_event *cli_event);
void bndstrg_update_white_black_list(struct bndstrg *bndstrg,struct bndstrg_iface *inf, unsigned char *macAddr, u8 list_type, int deladd);

#ifdef ROAMING_SUPPORT
unsigned int udp_send(struct bndstrg *bndstrg, unsigned char *pPkt,	unsigned int PktLen);

void initList(LIST_HEADER *pList);
void insertTailList(LIST_HEADER *pList, RT_LIST_ENTRY *pEntry);
RT_LIST_ENTRY *delEntryList(LIST_HEADER *pList, RT_LIST_ENTRY *pEntry);
RT_LIST_ENTRY *removeHeadList(LIST_HEADER *pList);
PBS_LIST_ENTRY FindBsListEntry(PLIST_HEADER pBsList, struct bndstrg_iapp_sta_entry *sta_entry);
int AddBsListEntry(PLIST_HEADER pBsList, struct bndstrg_iapp_sta_entry *sta_entry);
void DelBsListEntry(PLIST_HEADER pBsList, struct bndstrg_iapp_sta_entry *sta_entry);
void ClearBsList(PLIST_HEADER pBsList);

#define SHOW_IP_HTONL(__IP)	\
	(htonl(__IP) >> 24) & 0xFF,		\
	(htonl(__IP) >> 16) & 0xFF,		\
	(htonl(__IP) >> 8) & 0xFF,		\
	(htonl(__IP) & 0xFF)

#define SHOW_IP(__IP)			\
	(__IP) & 0xFF,					\
	((__IP) >> 8) & 0xFF,			\
	((__IP) >> 16) & 0xFF,			\
	((__IP) >> 24) & 0xFF
#endif /*ROAMING_SUPPORT*/

#define IS_VALID_MAC(addr) \
	((addr[0])|(addr[1])|(addr[2])|(addr[3])|(addr[4])|(addr[5]))

#define IS_BND_STRG_DUAL_BAND_CLIENT(_Control_Flags) \
	((_Control_Flags & fBND_STRG_CLIENT_SUPPORT_2G) && \
	 ((_Control_Flags & fBND_STRG_CLIENT_SUPPORT_L5G) || (_Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G)))

#define IS_BND_STRG_H5G_L5G_BAND_CLIENT(_Control_Flags) \
   ((_Control_Flags & fBND_STRG_CLIENT_SUPPORT_L5G) && (_Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G))

#define IS_BND_STRG_H5G_ONLY_BAND_CLIENT(_Control_Flags) \
((_Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G) && !(_Control_Flags & fBND_STRG_CLIENT_SUPPORT_L5G) && !(_Control_Flags & fBND_STRG_CLIENT_SUPPORT_2G))

#define IS_BND_STRG_L5G_ONLY_BAND_CLIENT(_Control_Flags) \
((_Control_Flags & fBND_STRG_CLIENT_SUPPORT_L5G) && !(_Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G) && !(_Control_Flags & fBND_STRG_CLIENT_SUPPORT_2G))

#define IS_BND_STRG_2G_ONLY_BAND_CLIENT(_Control_Flags) \
((_Control_Flags & fBND_STRG_CLIENT_SUPPORT_2G) && !(_Control_Flags & fBND_STRG_CLIENT_SUPPORT_L5G) && !(_Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G))
#endif /* __BNDSTRG_H__ */

