
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
    	bndstrg.c
*/
#include <stdlib.h>
#include <stdio.h>
#include "bndstrg.h"
#include <time.h>
#ifdef ROAMING_SUPPORT
#include <sys/socket.h>
#include <sys/ioctl.h>
#endif /*ROAMING_SUPPORT*/

#ifdef NVRAM_LIB_SUPPORT
#include "nvram.h"
#define NVRAM_INDEX RT2860_NVRAM
#endif
int DebugLevel = DEBUG_ERROR;
#define BNDSTRG_DAEMON_VER	"3.1.13"

#ifdef CONFIG_SUPPORT_OPENWRT
#define BNDSTRG_DAT_FILE_PATH	"/etc/bndstrg.conf"
#else
#define BNDSTRG_DAT_FILE_PATH	"/etc_ro/bndstrg.conf"
#endif

int	PriorityList[] = {	fBND_STRG_PRIORITY_RSSI_DIFF,
						fBND_STRG_PRIORITY_5G_RSSI,
					    fBND_STRG_PRIORITY_HT_SUPPORT,
						fBND_STRG_PRIORITY_DEFAULT_5G};
u8 	PriorityListSize = sizeof(PriorityList)/sizeof(PriorityList[0]);

/* make sure that it match the sequence in BND_STRG_PRIORITY_FLAGS enum */
char *entry_match_steering_str[]={
		"RSSI_DIFF",
		"BAND_PERSIST",
		"HT_SUPPORT",
		"5G_RSSI",
		"VHT_SUPPORT",
		"NSS_SUPPORT",
		"LB_CHANLOAD_STATIC",
		"LB_STA_COUNT",
		"LB_RSSI",
		"LB_MCS",
		"DEFAULT_2G",
		"DEFAULT_5G",
		"RSSI_DOWNSTEER",
		"LB_CHANLOAD_IDLE",
		"LB_CHANLOAD_ACTIVE",
		"RSSI_UPSTEER",
#ifdef VENDOR_IOS_SUPPORT
		"IOS_STEER",
#endif
#ifdef 	VENDOR_FEATURE7_SUPPORT
		"DOWNSTEER_CHLOAD_RSSI",
		"UPSTEER_CHLOAD_RSSI",
#endif
		"UNKNOWN"
};

/* make sure that it match the sequence in BND_STRG_CHANLOAD_MATCH_ID enum */
char *entry_match_chanload_active_str[] = {
		"UNKNOWN", "DOWN_STEER", "UP_STEER"
	};

char *bndstrg_stop_str [] = {
	"BNDSTRG_DAEMON_TBL_FULL",
	"BNDSTRG_DRIVER_TBL_FULL",
	"BNDSTRG_LIMIT"};

/* refer enum entry_state */
char *entry_state [] = {
	"INIT",
	"READY_TO_ASSOC",
	"ASSOC_RCD",
	"DELETE_RCD",
	"OPER_STEER_ACTIVE"
};

char *phy_mode [] = {
	"Legacy",
	"HT",
	"VHT"
};
struct bndstrg_command_id_name bndstrg_commands[] = {
	{BND_SET_CND_PRIORITY, "CndPriority"},
	{BND_SET_DWELL_TIME, "DwellTime"},
	{BND_SET_STEER_TIME_WINDOW, "SteerTimeWindow"},
	{BND_SET_MAX_STEER_COUNT, "MaxSteerCount"},
	{BND_SET_AGETIME, "AgeTime"},
	{BND_SET_CHECKTIME, "CheckTime"},
	{BND_SET_HOLDTIME, "HoldTime"},
	{BND_SET_RSSILOW, "RssiLow"},
	{BND_SET_RSSIDIFF, "RssiDiff"},
	{BND_SET_STEERING_NUM, "SteeringNum"},
	{BND_SET_ASSOC_TH, "AssocThres"},
	{BND_SET_LOAD_TH, "LoadThres"},
	{BND_SET_MIN_RSSI_TH, "MinRssiThres"},
	{BND_SET_NSS_TH, "NSSThres"},
	{BND_SET_STA_POLL_PRD, "StaPollTime"},
	{BND_SET_DAEMON_STATE, "DaemonState"},
	{BND_SET_ADD_WHITE_LIST, "AddWhiteList"},
	{BND_SET_REMOVE_WHITE_LIST, "RemoveWhiteList"},
#ifdef BTM_DEBUG_SUPPORT /* For BTM manual steering debug only */
	{BND_SEND_BTM, "SendBTM"},
#endif
	{BND_SET_RSSI_DOWNSTEER, "RSSILowDownSteer"},
	{BND_SET_RSSI_UPSTEER, "RSSIHighUpSteer"},
	{BND_SET_NVRAM, "NVRAM"},
#ifdef VENDOR_IOS_SUPPORT
	{BND_SET_IOS_STEER_NUMBER, "IOSSteerNum"},
#endif
#ifdef	VENDOR_FEATURE7_SUPPORT
	{BND_SET_RSSI_DISCONNECT, "RSSIDisconnect"},
#endif
	{BND_SET_RSSI_CHECK_COUNT, "RSSICheckCount"},
	{BND_SET_IDLE_RX_BYTE_COUNT, "IdleRxByteCount"},
	{BND_SET_IDLE_TX_BYTE_COUNT, "IdleTxByteCount"},

};

extern struct bndstrg_drv_ops bndstrg_drv_wext_ops;

/* static function declaration */
static int _bndstrg_event_on_off(struct bndstrg *bndstrg, u8 onoff, u8 band, u8 channel, char *ifname);
static int _bndstrg_event_show_entries(struct bndstrg *bndstrg,struct bndstrg_msg *msg);
static int _bndstrg_event_table_info(struct bndstrg *bndstrg);
static int _bndstrg_print_ctrlflags(u32 flags);
static int _bndstrg_print_entry_statistics(struct bndstrg_entry_stat *statistics, u8 dump_cli_status);
static void _bndstrg_terminate(int sig, void *signal_ctx);
static int _bndstrg_print_entry( struct bndstrg_cli_table *table, struct bndstrg_cli_entry *entry);
int bndstrg_backup_entry(struct bndstrg_cli_table *table, unsigned char *pAddr, u32 Index, u8 steered_ok);
u32 bndstrg_get_entry_elapsed_time(struct bndstrg_cli_entry *entry);
u32 bndstrg_get_elapsed_time(struct timespec ref_time); 
u8 bndstrg_check_entry_aged(struct bndstrg *bndstrg, struct bndstrg_cli_entry *entry);
u8 bndstrg_table_en_polling(struct bndstrg *bndstrg, struct bndstrg_cli_table *table, u8 en);
void get_current_system_tick(struct timespec *now);
int bndstrg_delete_entry(struct bndstrg_cli_table *table, unsigned char *pAddr, u32 Index);
u8 bndstrg_polling_connected_sta(struct bndstrg *bndstrg, u8 inf_num);
void bndstrg_inf_init(struct bndstrg_iface *inf);
void bndstrg_read_inf_param(struct bndstrg *bndstrg);
void bndstrg_check_steering_limit(struct bndstrg_cli_table *table,struct bndstrg_cli_entry *entry);
struct bndstrg_iface * bndstrg_get_interface(
    struct bndstrg_ctrl_iface *ctrl_iface, 
    char *IfName, 
    u8 Band,
    u8 byBand);
struct bndstrg_iface * bndstrg_get_interface_by_channel(
    struct bndstrg_ctrl_iface *ctrl_iface,  
    u8 channel);

char* getFormattedTime(void) {

    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    /*  Must be static, otherwise won't work  */
    static char _retval[24];
    strftime(_retval, sizeof(_retval), "%H:%M:%S", timeinfo);

    return _retval;
}

static char* bndstrg_get_entry_band(u8 band)
{
	if(IS_2G_BAND(band))
		return "2G" ;
	else if(IS_5G_L_BAND(band))
		return "5GL";
	else if(IS_5G_H_BAND(band))
		return "5GH";
	else if(IS_5G_BAND(band))
		return "5G";
	else
		return "NG";
}

inline int bndstrg_accessible_cli(
				struct bndstrg *bndstrg,
				struct bndstrg_iface *inf,
				struct bndstrg_cli_entry *entry,
				u8 action)
{
	int ret = 0;

	DBGPRINT(DEBUG_TRACE, "\n");
    
	ret = bndstrg->drv_ops->drv_accessible_cli(bndstrg->drv_data, inf, entry, action);

	return ret;
}

inline int bndstrg_inf_status_query(
				struct bndstrg *bndstrg,
				const char *iface, u8 action)
{
	int ret = 0;

	DBGPRINT(DEBUG_TRACE, "\n");
	ret = bndstrg->drv_ops->drv_inf_status_query(bndstrg->drv_data, iface, action);

	return ret;
}

inline int bndstrg_chanload_status_query(
	struct bndstrg *bndstrg,
	const char *iface)
{
	int ret = 0;

	DBGPRINT(DEBUG_TRACE, "\n");
	ret = bndstrg->drv_ops->drv_chanload_status_query(bndstrg->drv_data, iface);

	return ret;
}

inline int bndstrg_onoff(
				struct bndstrg *bndstrg,
				const char *iface,
				u8 onoff,
				u8 mode)
{
	int ret = 0;

	DBGPRINT(DEBUG_TRACE, "\n");
#if 0
	ret = bndstrg_drv_ops_pre_check(bndstrg, iface);

	if (ret) {
		DBGPRINT(DEBUG_ERROR, "%s: bndstrg drv ops pre check fail\n", __FUNCTION__);
		return -1;
	}
#endif
	ret = bndstrg->drv_ops->drv_bndstrg_onoff(bndstrg->drv_data, iface, onoff, mode);

	return ret;
}

inline int bndstrg_send_white_black_list(
				struct bndstrg *bndstrg,
				const char *iface, unsigned char *macaddr, u8 list_type, u8 deladd)
{
	int ret = 0;

	DBGPRINT(DEBUG_TRACE, "\n");
	ret = bndstrg->drv_ops->drv_update_white_black_list(bndstrg->drv_data, iface, macaddr, list_type, deladd);

	return ret;
}

inline int bndstrg_wnm_send_btm_req(struct bndstrg *bndstrg, 
	struct bndstrg_iface *inf, 
	struct bndstrg_iface *destination_inf, 
	struct bndstrg_cli_entry *entry)
{
	int ret = 0;

	DBGPRINT(DEBUG_TRACE, "\n");		
	ret = bndstrg->drv_ops->drv_send_btm_req(bndstrg, inf, destination_inf, entry);
	return ret;
}

int bndstrg_update_entry_statistics_control_flags(
		struct bndstrg *bndstrg,
		struct bndstrg_cli_entry *entry,
		struct bndstrg_iface *inf,
		u8	band,
		char Rssi,
		u8 FrameSubType)
{
	int ret_val = BND_STRG_SUCCESS;
#ifdef BND_STRG_DBG
	time_t current_time;
	struct tm * time_info;
	time(&current_time);
	time_info = localtime(&current_time);
#endif
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bndstrg_entry_stat *statistics = NULL;

	if (IS_2G_BAND(band))
	{
		entry->Control_Flags |= fBND_STRG_CLIENT_SUPPORT_2G;
		statistics = &entry->statistics[IDX_2G];
		statistics->Rssi = Rssi;
	}
	else if (IS_5G_BAND(band))
	{
		if (IS_5G_H_BAND(inf->Band)) {
			statistics = &entry->statistics[IDX_5GH];
			statistics->Rssi = Rssi;
			entry->Control_Flags |= fBND_STRG_CLIENT_SUPPORT_H5G;
		} else {
			statistics = &entry->statistics[IDX_5GL];
			statistics->Rssi = Rssi;
			entry->Control_Flags |= fBND_STRG_CLIENT_SUPPORT_L5G;
		}
	}
	else
	{
		ret_val = BND_STRG_UNEXP;
	}

	switch (FrameSubType)
	{
		case APMT2_PEER_PROBE_REQ:
#ifdef BND_STRG_DBG			
			if(statistics->ProbeReqCount == 0)
				sprintf(statistics->FirstProbeTime, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
			sprintf(statistics->LastProbeTime, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
#endif
			statistics->ProbeReqCount++;
			break;
			
		case APMT2_PEER_AUTH_REQ:
#ifdef BND_STRG_DBG
			if(statistics->AuthReqCount == 0)
				sprintf(statistics->FirstAuthTime, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
			sprintf(statistics->LastAuthTime, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
#endif			
			statistics->AuthReqCount++;
			break;
			
		default:
			BND_STRG_DBGPRINT(DEBUG_OFF, "%s(): Unexpect FrameSubType (%u)\n", __FUNCTION__, FrameSubType);
			ret_val = BND_STRG_UNEXP;
			break;
	}

	if (statistics->Rssi != 0)
	{
		/* update low RSSI information when received probe/auth*/
		if (statistics->Rssi < (table->RssiLow))
		{
			if (IS_2G_BAND(band))
				entry->Control_Flags |= fBND_STRG_CLIENT_LOW_RSSI_2G;
			else
				entry->Control_Flags |= IS_5G_H_BAND(inf->Band) ? \
				fBND_STRG_CLIENT_LOW_RSSI_H5G : fBND_STRG_CLIENT_LOW_RSSI_L5G;
		} else {
			if (IS_2G_BAND(band))
				entry->Control_Flags &= (~fBND_STRG_CLIENT_LOW_RSSI_2G);
			else
				entry->Control_Flags &= IS_5G_H_BAND(inf->Band) ? \
				(~fBND_STRG_CLIENT_LOW_RSSI_H5G) : (~fBND_STRG_CLIENT_LOW_RSSI_L5G);
		}
	}

	/* For connection time some STA only scan single band although they are dual band client
	 * Need to change flag once STA is confirmed to be single band 
	 */
	if ((IS_BND_STRG_DUAL_BAND_CLIENT(entry->Control_Flags)) &&
		(entry->Control_Flags & (fBND_STRG_CLIENT_IS_2G_ONLY | fBND_STRG_CLIENT_IS_5G_ONLY)))
	{
		entry->Control_Flags &= ~ (fBND_STRG_CLIENT_IS_2G_ONLY | fBND_STRG_CLIENT_IS_5G_ONLY);
	}
	return ret_val;
}

struct bndstrg_cli_entry* bndstrg_table_lookup(struct bndstrg_cli_table *table, unsigned char *pAddr)
{
	unsigned long HashIdx;
	struct bndstrg_cli_entry *entry = NULL;
	
	HashIdx = MAC_ADDR_HASH_INDEX(pAddr);
	entry = table->Hash[HashIdx];

	while (entry && entry->bValid)
	{
		if (MAC_ADDR_EQUAL(entry->Addr, pAddr))
			break;
		else
			entry = entry->pNext;
	}

	return entry;
}

void bndstrg_ctrl_interface_update(
    struct bndstrg_ctrl_iface *ctrl_iface,
    struct bndstrg_cli_table *table,
    struct bnd_msg_inf_status_rsp *inf_status_rsp)
{
    u8 i,cnt;
    struct bndstrg_iface *inf = NULL, *inf_temp = NULL, *inf_same_src = NULL;
    struct bndstrg_ref_iface *ref_inf = NULL;
    
    for(i=0,cnt=0; i<MAX_INF_NUM; i++)
    {
        inf = &ctrl_iface->inf[i];
        if(inf->bValid)
        {
            cnt++;
            if(!strcmp(inf->ucIfName, inf_status_rsp->ucIfName))
            {
                if (!(inf->bInfReady^inf_status_rsp->bInfReady))
		            return;
                inf->bInfReady = inf_status_rsp->bInfReady;
                inf->Band = inf_status_rsp->band;
                inf->Channel = inf_status_rsp->Channel;
                inf->bVHTCapable = inf_status_rsp->bVHTCapable;
				inf->nss = inf_status_rsp->nss;
				inf->max_driver_table_size = inf_status_rsp->table_size;
                inf->table_src_addr = inf_status_rsp->table_src_addr;
                if(IS_5G_BAND(inf_status_rsp->band))
                {
                	inf_temp = bndstrg_get_interface(ctrl_iface,NULL,BAND_5G,TRUE);
					if((ctrl_iface->Active5GCount > 0) && inf_temp){
						if(inf->nss <= inf_temp->nss){
							if(inf->Channel > inf_temp->Channel){
								inf->Band |= BAND_5G_H;
								inf_temp->Band &= ~BAND_5G_H;
								inf_temp->Band |= BAND_5G_L;
							}else{
								inf->Band |= BAND_5G_L;
								inf_temp->Band &= ~BAND_5G_L;
								inf_temp->Band |= BAND_5G_H;
							}
				
						}else{
							inf->Band |= BAND_5G_H;
							inf_temp->Band &= ~BAND_5G_H;
							inf_temp->Band |= BAND_5G_L;
						}

					}
					else
					{
                        inf->Band |= BAND_5G_L;
					}

                    if(inf_status_rsp->bInfReady)
                    {
                        ctrl_iface->Active5GCount++;
                        ctrl_iface->ActiveCount++;
						bndstrg_inf_init(inf);
						if(inf_temp)
							bndstrg_inf_init(inf_temp);
                    }
                    else
                    {
                        ctrl_iface->Active5GCount--;
                        if(ctrl_iface->Active5GCount < 0)
                           ctrl_iface->Active5GCount = 0;
                        ctrl_iface->ActiveCount--;
                        if(ctrl_iface->ActiveCount < 0)
                            ctrl_iface->ActiveCount = 0;
                    }
                }
                else
                {
                    if(inf->bInfReady)
                    {
                        ctrl_iface->Active2GCount++;
                        ctrl_iface->ActiveCount++;
						bndstrg_inf_init(inf);
                    }
                    else
                    {
                        ctrl_iface->Active2GCount--;
                        if(ctrl_iface->Active2GCount < 0)
                           ctrl_iface->Active2GCount = 0;
                        ctrl_iface->ActiveCount--;
                        if(ctrl_iface->ActiveCount < 0)
                            ctrl_iface->ActiveCount = 0;
                    }
                }
                                
                if(ctrl_iface->Active2GCount > 0)
                    table->b2GInfReady = TRUE;
                else
                    table->b2GInfReady = FALSE;

                if(ctrl_iface->Active5GCount > 0)
                    table->b5GInfReady = TRUE;
                else
                    table->b5GInfReady = FALSE;
                
				DBGPRINT(DEBUG_OFF,"Rx INF STATUS RSP for inf(%s): %d\n",
                        (char*)inf_status_rsp->ucIfName, inf_status_rsp->bInfReady);
				DBGPRINT(DEBUG_TRACE,"Band:%s Channel:%d VHTCap:%s Nss:%d TableSize:%d",
						bndstrg_get_entry_band(inf->Band), inf->Channel, 
						inf->bVHTCapable ? "Yes":"No", inf->nss, inf->driver_table_size);
                return;
            }

            if(!inf_same_src && inf->bInfReady)
                inf_same_src = (inf->table_src_addr == inf_status_rsp->table_src_addr)?inf:NULL;
        }
        if(cnt >= ctrl_iface->Size)
            break;
    }

    /* For Debug Demand */
    if(inf_same_src && (inf_same_src->Size < MAX_REF_INF_NUM))
    {
        for(i=0,cnt=0; i<MAX_REF_INF_NUM; i++)
        {
            ref_inf = &inf_same_src->ref_iface[i];
            if(ref_inf->bValid)
            {
                cnt++;
                if(!strcmp(ref_inf->ucIfName, inf_status_rsp->ucIfName))
                {
                    ref_inf->bInfReady = inf_status_rsp->bInfReady;
                    break;
                }

            }
            else
            {
                ref_inf->bValid = TRUE;
                ref_inf->bInfReady = inf_status_rsp->bInfReady;
                strcpy(ref_inf->ucIfName, inf_status_rsp->ucIfName);
                inf_same_src->Size++;
                break;
            }
            if(cnt >= inf_same_src->Size)
                break;
        }
    }
}

struct bndstrg_iface * bndstrg_get_interface(
    struct bndstrg_ctrl_iface *ctrl_iface, 
    char *IfName, 
    u8 Band,
    u8 byBand)
{
    u8 i,cnt;
    struct bndstrg_iface *inf = NULL;

	if(byBand && (Band == BAND_INVALID))
		return NULL;
	
    for(i=0,cnt=0; i < ctrl_iface->Size; i++)
    {
        inf = &ctrl_iface->inf[i];
        if(inf->bValid)
        {
            cnt++;
            if(byBand)
            {
                if(inf->bInfReady && ((inf->Band & Band) == Band))
                    return inf;
            }
            else
            {
                if(!strcmp((char*)inf->ucIfName, IfName))
                    return inf;
            }
        }
    }

    return NULL;
}

int bndstrg_op_mode_update(struct bndstrg *bndstrg)
{
    struct bndstrg_cli_table *table = &bndstrg->table;

	if(table->Band == (BAND_2G | BAND_5G | BAND_5G_L | BAND_5G_H)) {
		table->op_mode = OP_TRIBAND;
		table->bEnabled = TRUE;		
	}
	else if ((table->Band == (BAND_2G | BAND_5G | BAND_5G_L))
		|| (table->Band == (BAND_5G | BAND_5G_L | BAND_5G_H))){
			table->op_mode = OP_DUALBAND;
			table->bEnabled = TRUE; 	
		}
	else{ 
		table->op_mode = OP_NO_BNDSTR;
		table->bEnabled = FALSE;
	}
    return 0;
}

inline int bndstrg_nvram_update_to_driver(
		struct bndstrg *bndstrg,
		const char *iface,
		struct bndstrg_nvram_client *entry,
		u8 action)
{
	int ret = 0;

	DBGPRINT(DEBUG_TRACE, "%s\n", __FUNCTION__);
	ret = bndstrg->drv_ops->drv_nvram_update(bndstrg->drv_data, iface, entry, action);
	return ret;
}

void bndstrg_nvram_write(struct bndstrg_cli_table *table, struct bndstrg_nvram_client *nvram_entry, int index)
{
#ifdef NVRAM_LIB_SUPPORT
	int nvram_index = NVRAM_INDEX;
	char *name = NULL;
	char *value = NULL;
	char indexname[20] = { 0 };
	char indexvalue[50] = { 0 };
#ifdef	VENDOR_FEATURE7_SUPPORT
	static char _timeinfo[24];
#endif

	if (!table->bndstrg->nvram_support)
		return;
	DBGPRINT(DEBUG_OFF, "%s\n", __FUNCTION__);
	nvram_init(nvram_index);
	name = indexname;
	value = indexvalue;
	sprintf(name, "BNDSTRTBL%d", index);
	sprintf(value,"%02x:%02x:%02x:%02x:%02x:%02x",PRINT_MAC(nvram_entry->Addr));
	sprintf(value+strlen(value), ";%u;%u;%u;%u", nvram_entry->Manipulable,nvram_entry->PhyMode,
		nvram_entry->Band,nvram_entry->Nss);
#ifdef	VENDOR_FEATURE7_SUPPORT
	sprintf(value+strlen(value),";%x",nvram_entry->ControlFlags);
	strftime(_timeinfo, sizeof(_timeinfo), TIMEFORMAT, &nvram_entry->timeinfo);
	sprintf(value+strlen(value),";%s",_timeinfo);
#endif
	nvram_bufset(nvram_index, name, value);
	nvram_close(nvram_index);
#endif
}

void bndstrg_nvram_update(struct bndstrg_cli_table *table, struct bndstrg_nvram_client *nvram_entry)
{
#ifdef NVRAM_LIB_SUPPORT
	int nvram_index = NVRAM_INDEX;
	char *name = NULL;
	char *value = NULL;
	char indexname[20] = { 0 };
	char indexvalue[50] = { 0 };
#ifdef	VENDOR_FEATURE7_SUPPORT
	static char _timeinfo[24];
#endif
	int i;
	for(i = 0; i < table->bndstrg_nvram_client_count ; i++)
	{
		if(MAC_ADDR_EQUAL(table->nvram_entry[i].Addr,nvram_entry->Addr))
			break;
	}
	nvram_init(nvram_index);
	name = indexname;
	value = indexvalue;
	sprintf(name, "BNDSTRTBL%d", (i+1));
	sprintf(value,"%02x:%02x:%02x:%02x:%02x:%02x",PRINT_MAC(nvram_entry->Addr));
	sprintf(value+strlen(value), ";%u;%u;%u;%u", nvram_entry->Manipulable,nvram_entry->PhyMode,
		nvram_entry->Band,nvram_entry->Nss);
#ifdef	VENDOR_FEATURE7_SUPPORT
	sprintf(value+strlen(value),";%x",nvram_entry->ControlFlags);
	strftime(_timeinfo, sizeof(_timeinfo), TIMEFORMAT, &nvram_entry->timeinfo);
	sprintf(value+strlen(value),";%s",_timeinfo);
#endif
	nvram_bufset(nvram_index, name, value);
	nvram_close(nvram_index);
#endif
}

void bndstrg_nvram_read_all(struct bndstrg *bndstrg)
{
#ifdef NVRAM_LIB_SUPPORT
	int i = 0, j = 0;
	int nvram_index = NVRAM_INDEX;
	char str[25] = { 0 };
	char bnd_str_entry[50] = { 0 };
	char *bndstrgentry = NULL;
	char *temp = NULL;
	struct bndstrg_nvram_client *nvram_entry = NULL;
#ifdef	VENDOR_FEATURE7_SUPPORT
	char *temp01=NULL;
#else
	struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
#endif
	nvram_init(nvram_index);
	DBGPRINT(DEBUG_OFF,"\n");

	for (i= 1 ; i <= bndstrg->table.nvram_table_size; i++) {
		sprintf(str, "BNDSTRTBL%d", i);
		bndstrgentry = (char *)nvram_bufget(nvram_index, str);
		DBGPRINT(DEBUG_TRACE,"%s:%s\n", str, bndstrgentry);
		if (strcmp(bndstrgentry,"") == 0)
			break;
		strcpy(bnd_str_entry , bndstrgentry);
		nvram_entry = &bndstrg->table.nvram_entry[i-1];
		memset(nvram_entry, 0, sizeof(struct bndstrg_nvram_client));
		for (j = 0, temp = strtok(bnd_str_entry,";"); temp; temp = strtok(NULL,";"), j++)
	    {
	    	switch ( j ) {
				case 0 :
				{
					sscanf(temp,"%02x:%02x:%02x:%02x:%02x:%02x",(unsigned int*)&nvram_entry->Addr[0],(unsigned int*)&nvram_entry->Addr[1],(unsigned int*)&nvram_entry->Addr[2], 
									(unsigned int*)&nvram_entry->Addr[3],(unsigned int*)&nvram_entry->Addr[4],(unsigned int*)&nvram_entry->Addr[5]);
					break;
				}
				case 1 :
				{
					nvram_entry->Manipulable = strtol(temp, 0, 10);
					break;
				}
				case 2 :
				{
					nvram_entry->PhyMode = strtol(temp, 0, 10);
					break;
				}
				case 3 :
				{
					nvram_entry->Band = strtol(temp, 0, 10);
					break;
				}
				case 4 :
				{
					nvram_entry->Nss = strtol(temp, 0, 10);
					break;
				}
#ifdef	VENDOR_FEATURE7_SUPPORT
				case 5 :
				{
					nvram_entry->ControlFlags = strtol(temp, 0, 16);
					break;
				}
				case 6 :
				{
					/* time format string "2017-10-10:12:30:12" lenght should be 19 */
					if(strlen(temp) == 19 ){
						nvram_entry->timeinfo.tm_year = strtol(temp, &temp01, 10);
						nvram_entry->timeinfo.tm_year -= 1900; /*tm_year is the number of years since 1900.*/ 
						temp01++;
						nvram_entry->timeinfo.tm_mon = strtol(temp01, &temp01, 10);
						nvram_entry->timeinfo.tm_mon -= 1;/*tm_mon The number of months since January, in the range 0 to 11 */
						temp01++;
						nvram_entry->timeinfo.tm_mday = strtol(temp01, &temp01, 10);
						temp01++;
						nvram_entry->timeinfo.tm_hour = strtol(temp01, &temp01, 10);
						temp01++;
						nvram_entry->timeinfo.tm_min = strtol(temp01, &temp01, 10);
						temp01++;
						nvram_entry->timeinfo.tm_sec = strtol(temp01, 0, 10);
					}
					break;	
				}
#endif
				default :
				{
					printf("Error\n");
				}					
	    	}
	    }
#ifndef VENDOR_FEATURE7_SUPPORT
		bndstrg_nvram_update_to_driver(bndstrg, (char*)ctrl_iface->inf[0].ucIfName, nvram_entry, NVRAM_UPDATE);
#endif
	}
	bndstrg->table.bndstrg_nvram_client_count = i-1;
	nvram_close(nvram_index);
#endif	
	return;
}

#ifdef BNDSTRG_NVRAM_SUPPORT
void bndstrg_nvram_print_all_entry(struct bndstrg_cli_table *table)
{
	struct bndstrg_nvram_client *nvram_entry = NULL;
	u8 i;
#ifdef VENDOR_FEATURE7_SUPPORT
	static char _timeinfo[24];
#endif
	DBGPRINT(DEBUG_OFF,"\tIdx  Addr \t\t Band Nss PhyMode Manuplable"
#ifdef VENDOR_FEATURE7_SUPPORT
		" ControlFlags Time"
#endif
		"\n");
	for(i=0; i< table->bndstrg_nvram_client_count; i++){
		nvram_entry = &table->nvram_entry[i];
#ifdef VENDOR_FEATURE7_SUPPORT
		strftime(_timeinfo, sizeof(_timeinfo), TIMEFORMAT, &nvram_entry->timeinfo);
#endif
		DBGPRINT(DEBUG_OFF,"\t%-2d   %02x:%02x:%02x:%02x:%02x:%02x   %-4s %-3d %-7s %-10s"
#ifdef VENDOR_FEATURE7_SUPPORT
			" %-12x %-4s"
#endif
			"\n", i, PRINT_MAC(nvram_entry->Addr), bndstrg_get_entry_band(nvram_entry->Band), 
			nvram_entry->Nss, (nvram_entry->PhyMode < fPhyMode_Max)? phy_mode[nvram_entry->PhyMode]:"Unknown", (nvram_entry->Manipulable == TRUE ? "TRUE":"FALSE")
#ifdef	VENDOR_FEATURE7_SUPPORT
			,nvram_entry->ControlFlags, _timeinfo
#endif
			);
	}
}

void bndstrg_nvram_clear_all_entry(struct bndstrg_cli_table *table)
{
#ifdef NVRAM_LIB_SUPPORT
		int nvram_index = NVRAM_INDEX;
		char *name = NULL, *bndstrgentry=NULL;
		char value[1] = "\0";
		char indexname[20] = { 0 };
		u8 index;
		
		if (!table->bndstrg->nvram_support)
			return;

		nvram_init(nvram_index);
		name = indexname;
		for(index = 1; index <= table->nvram_table_size; index++){
			sprintf(name, "BNDSTRTBL%d", index);
			bndstrgentry = (char *)nvram_bufget(nvram_index, name);
			if (strcmp(bndstrgentry,"") == 0)
				continue;
			else{
				DBGPRINT(DEBUG_OFF,"%s:%s\n", name, bndstrgentry);
				nvram_bufset(nvram_index, name, value);
			}
		}
		nvram_close(nvram_index);
#endif
	table->bndstrg_nvram_client_count = 0;
}

#ifdef VENDOR_FEATURE7_SUPPORT
u8 nvram_get_least_used_entry_idx(struct bndstrg *bndstrg){

	struct bndstrg_cli_table *table = &bndstrg->table;
	time_t Old_time, New_time;
	u8 i=0, idx=0;
	
	DBGPRINT(DEBUG_TRACE, "%d\n",__LINE__);
	Old_time = mktime(&table->nvram_entry[0].timeinfo);
	if(Old_time == -1)
		return 1;
	for(i=1; i<table->nvram_table_size; i++){ 
		New_time = mktime(&table->nvram_entry[i].timeinfo);
		if(New_time == -1)
			return i;
		if(Old_time > New_time){
			Old_time = New_time;
			idx = i;
		}
	}
	return (idx + 1);
}
#endif

u8 bndstrg_nvram_table_lookup(struct bndstrg_cli_table *table, unsigned char *pAddr)
{
	u8 i;
	if (!table->bndstrg->nvram_support)
		return TRUE;
	for(i = 0; i < table->bndstrg_nvram_client_count ; i++)
	{
		if(MAC_ADDR_EQUAL(table->nvram_entry[i].Addr,pAddr))
			return TRUE;
	}
	return FALSE;		
}

static u8 _bndstrg_nvram_insert_entry(struct bndstrg *bndstrg,  struct bndstrg_msg *msg, u8 Manipulable)
{
	struct bndstrg_nvram_client *nvram_entry = NULL;
	struct bndstrg_cli_entry *entry = NULL;
#ifndef VENDOR_FEATURE7_SUPPORT
    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
#endif
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bnd_msg_cli_event *cli_event = &msg->data.cli_event;
	struct bnd_msg_cli_assoc *cli_assoc = &cli_event->data.cli_assoc;
	u8 index;
#ifdef	VENDOR_FEATURE7_SUPPORT
	struct tm* timeinfo;
	time_t rawtime;
	time(&rawtime);
    timeinfo = localtime(&rawtime);
#endif
	if (!bndstrg->nvram_support)
		return TRUE;
	if (table->bndstrg_nvram_client_count < table->nvram_table_size){
		table->bndstrg_nvram_client_count++;
		index = table->bndstrg_nvram_client_count;
	}
	else{
#ifdef	VENDOR_FEATURE7_SUPPORT
		index = nvram_get_least_used_entry_idx(bndstrg);
		if(index == 0){
			DBGPRINT(DEBUG_OFF,"Unable to add entry to NVRAM table\n");
			return FALSE;
		}
#else
		return FALSE;
#endif
	}

	entry = bndstrg_table_lookup(table, cli_event->Addr);
	if(!entry)
		return FALSE;	
	nvram_entry = &table->nvram_entry[index-1];
	memset(nvram_entry, 0, sizeof(struct bndstrg_nvram_client));
	memcpy(nvram_entry->Addr, cli_event->Addr, MAC_ADDR_LEN);
	nvram_entry->Band = cli_event->Band;
	nvram_entry->Nss = cli_assoc->Nss;
	nvram_entry->Manipulable = Manipulable;
	if(cli_assoc->bVHTCapable == TRUE)
		nvram_entry->PhyMode = fPhyMode_VHT;
	else if(cli_assoc->bAllowStaConnectInHt == TRUE)
		nvram_entry->PhyMode = fPhyMode_HT;
	else
		nvram_entry->PhyMode = fPhyMode_Legacy;
#ifdef	VENDOR_FEATURE7_SUPPORT
	nvram_entry->ControlFlags = entry->Control_Flags;
	memcpy(&nvram_entry->timeinfo, timeinfo, sizeof(struct tm));
#endif
	bndstrg_nvram_write(table, nvram_entry, index);
#ifndef	VENDOR_FEATURE7_SUPPORT
	bndstrg_nvram_update_to_driver(bndstrg, (char*)ctrl_iface->inf[0].ucIfName, nvram_entry, NVRAM_UPDATE);
#endif
	return TRUE;
}

u8 bndstrg_nvram_update_entry(struct bndstrg *bndstrg,  struct bndstrg_cli_entry *entry)
{
	struct bndstrg_nvram_client *nvram_entry = NULL;
	struct bndstrg_cli_table *table = &bndstrg->table;
	int i=0 ;

#ifdef	VENDOR_FEATURE7_SUPPORT
	struct tm* timeinfo;
	time_t rawtime;
	time(&rawtime);
    timeinfo = localtime(&rawtime);
#endif
	
	if (!bndstrg->nvram_support)
		return TRUE;
	for(i = 0; i < table->bndstrg_nvram_client_count ; i++)
	{
		if(MAC_ADDR_EQUAL(table->nvram_entry[i].Addr, entry->Addr))
			break;
	}
	
	if(i >= table->bndstrg_nvram_client_count)
	{
		printf("\n entry not found for ::%02x:%02x:%02x:%02x:%02x:%02x",PRINT_MAC(entry->Addr));
		return FALSE;
	}
	nvram_entry = &table->nvram_entry[i];
	nvram_entry->Band = entry->connected_band;
	nvram_entry->Nss = entry->Nss;
	nvram_entry->Manipulable = entry->Manipulable;
	DBGPRINT(DEBUG_TRACE,BLUE("\n[%s]-[Band=%s]::%02x:%02x:%02x:%02x:%02x:%02x [Nss=%u] [Phymode=%u]"),
		__FUNCTION__, (IS_2G_BAND(nvram_entry->Band)? "2G":"5G"), PRINT_MAC(nvram_entry->Addr),nvram_entry->Nss, nvram_entry->PhyMode);

#ifdef	VENDOR_FEATURE7_SUPPORT
	nvram_entry->ControlFlags = entry->Control_Flags;
	memcpy(&nvram_entry->timeinfo, timeinfo, sizeof(struct tm));
#endif
	bndstrg_nvram_update(table, nvram_entry);
#ifndef	VENDOR_FEATURE7_SUPPORT
	{
		struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
		bndstrg_nvram_update_to_driver(bndstrg, (char*)ctrl_iface->inf[0].ucIfName, nvram_entry, NVRAM_UPDATE);
	}
#endif
	return TRUE;
}
#endif

void bndstrg_update_assoc_info(struct bndstrg *bndstrg, struct bndstrg_iface *inf, 
	struct bndstrg_cli_entry *entry, struct bndstrg_msg *msg)
{
	struct bndstrg_iface *prev_inf = NULL;
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bnd_msg_cli_event *cli_event = &msg->data.cli_event;
	struct bnd_msg_cli_assoc *cli_assoc = &cli_event->data.cli_assoc;
	u8 band_idx = 0;
#ifdef BND_STRG_DBG
	time_t current_time = 0;
	struct tm * time_info = NULL;
#endif	
#ifdef BND_STRG_DBG
	if(entry->total_steered_cnt == 0){
		time(&current_time);
		time_info = localtime(&current_time);				
	}
#endif
	entry->Recv_AuthTime=0; //add_shuai
	entry->Recv_AuthTime_1=0;//add_shuai

if(table->BndStrgMode == POST_CONNECTION_STEERING){
	/* In Operational steering only condition control flags need to set according to assoc req*/
	if( entry->connected_band != BAND_INVALID){
		if (IS_2G_BAND(entry->connected_band)){
			entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G);
			table->active_client_2G -- ;
		}
		else if(IS_5G_L_BAND(entry->connected_band)){
			entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G);
			table->active_client_5G --;
			table->active_client_L5G --;
		}
		else if(IS_5G_H_BAND(entry->connected_band)){
			entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G);
			table->active_client_5G --;
			table->active_client_H5G --;
		}
	}

	if((entry->Channel != 0) && (entry->Channel != cli_event->Channel)){
		/*
		* In POST_STEERING_ONLY STA may disconnect without sending DeAuth and reconnect back to previous band.
		* This will cause driver entry will be maintained for 480 sec, result into two CLI_STATUS_RSP for one STA.
		* For RSSI based steering it will cause problem as it will update RSSI based on past data.
		*/
		prev_inf = bndstrg_get_interface_by_channel(&bndstrg->ctrl_iface , entry->Channel);
		if(prev_inf) bndstrg_accessible_cli(bndstrg, prev_inf, entry, CLI_DEL);
	}
}

#ifdef BNDSTRG_NVRAM_SUPPORT
	if (bndstrg_nvram_table_lookup(table, cli_event->Addr))
	{	
		//if (entry->bActiveStatus == CLI_NOTACTIVE)
		{
			if(IS_2G_BAND(cli_event->Band))
			{
				entry->connected_band = BAND_2G;
				table->active_client_2G ++;
				band_idx = IDX_2G;
			}
			else if(IS_5G_BAND(cli_event->Band))
			{
				table->active_client_5G ++;
				if (IS_5G_H_BAND(inf->Band))
				{
					entry->connected_band = BAND_5G_H;
					table->active_client_H5G ++;
					band_idx = IDX_5GH;
				}
				else 
				{
					entry->connected_band = BAND_5G_L;
					table->active_client_L5G ++;
					band_idx = IDX_5GL;
				}
			}
			DBGPRINT(DEBUG_TRACE,("%s()[%d]:Entry Add for::%02x:%02x:%02x:%02x:%02x:%02x,channel=%d \n"),
				__FUNCTION__, __LINE__,PRINT_MAC(cli_event->Addr),entry->Channel);
			entry->bActiveStatus = CLI_ACTIVE;
		}
		if(bndstrg_nvram_update_entry(bndstrg, entry))
		{
			DBGPRINT(DEBUG_TRACE,("Client Updated To NVRAM Table,channel=%d\n"),entry->Channel);
		}
	}else
#endif    		
	{
		if(IS_2G_BAND(cli_event->Band))
		{
			entry->connected_band = BAND_2G;
			table->active_client_2G ++;
			if(table->active_client_2G > inf->max_driver_table_size)
			{
				DBGPRINT(DEBUG_OFF, "2G driver table full\n");
				bndstrg_stop(bndstrg,BNDSTRG_DRIVER_TBL_FULL);
			}
			band_idx = IDX_2G;
		}
		else if(IS_5G_BAND(cli_event->Band))
		{
			table->active_client_5G ++;
			if(IS_5G_H_BAND(inf->Band))
            {
				entry->connected_band = BAND_5G_H;
				table->active_client_H5G ++;
				if(table->active_client_H5G > inf->max_driver_table_size)
				{
					DBGPRINT(DEBUG_OFF, "5GH driver table full\n");
					bndstrg_stop(bndstrg,BNDSTRG_DRIVER_TBL_FULL);
				}
				band_idx = IDX_5GH;
            }
			else 
            {
				entry->connected_band = BAND_5G_L;
				table->active_client_L5G ++;
				if(table->active_client_L5G > inf->max_driver_table_size)
				{
					DBGPRINT(DEBUG_OFF, "5GL driver table full\n");
					bndstrg_stop(bndstrg,BNDSTRG_DRIVER_TBL_FULL);	
				}
				band_idx = IDX_5GL;
            }
		}

#ifdef BNDSTRG_NVRAM_SUPPORT				
		if(_bndstrg_nvram_insert_entry(bndstrg, msg, entry->Manipulable))
		{
			DBGPRINT(DEBUG_TRACE,("Client Added To NVRAM Table\n"));
		}
		else
		{
			DBGPRINT(DEBUG_ERROR,("NVRAM Table full\n"));
		}			
#endif				
	}

	entry->bConnStatus = TRUE;
#ifdef WPS_SUPPORT
	//WPS: update band and then update driver entry
	if((entry->Channel != cli_event->Channel) && (cli_assoc->bWpsAssoc))
	{
		if (IS_2G_BAND(inf->Band))
			entry->Control_Flags |= (fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G);
		else if(IS_5G_L_BAND(inf->Band))
			entry->Control_Flags |= (fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G);
		else if(IS_5G_H_BAND(inf->Band))
			entry->Control_Flags |= (fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G);
		entry->match_steered_rule_id[0] = fBND_STRG_PRIORITY_MAX;
		entry->match_steered_rule_id[1] = fBND_STRG_PRIORITY_MAX;
		entry->match_steered_rule_id[2] = fBND_STRG_PRIORITY_MAX;
		if(entry->band != entry->connected_band)
			bndstrg_client_band_update(bndstrg, entry, entry->connected_band);
	}
	entry->WpsAssoc = cli_assoc->bWpsAssoc;

	//WPS: trigger wps first connection timer counter
	if(entry->WpsAssoc == 1)
		entry->WpsFirstAssocCounter = 1;
	else
		entry->WpsFirstAssocCounter = 0;
#endif /*WPS_SUPPORT*/
	
	if(cli_assoc->BTMSupport == TRUE)
		entry->btm_info.BTMSupport = cli_assoc->BTMSupport;
	entry->bActiveStatus = CLI_ACTIVE;

	if(entry->state != ENTRY_OPER_STEER_ACTIVE)
		get_current_system_tick(&entry->tp);
	
	if(table->BndStrgMode == POST_CONNECTION_STEERING){
		/* In Operational steering only condition control flags need to set according to assoc req*/
		if (IS_2G_BAND(inf->Band))
			entry->Control_Flags |= (fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G);
		else if(IS_5G_L_BAND(inf->Band))
			entry->Control_Flags |= (fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G);
		else if(IS_5G_H_BAND(inf->Band))
			entry->Control_Flags |= (fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G);
		entry->band = entry->connected_band;
		/* add entry to driver tabel to get CLI status rsp */
		if(entry->Channel != cli_event->Channel)
			bndstrg_accessible_cli(bndstrg, inf, entry, CLI_ADD);
		/* Remove client from backlist*/
		bndstrg_update_white_black_list(bndstrg, NULL, entry->Addr, bndstrg_blacklist, 0);
	}

	entry->Channel = cli_event->Channel;

	DBGPRINT(DEBUG_OFF,GRN("%s()[%d]:Entry Add for::%02x:%02x:%02x:%02x:%02x:%02x,channel=%d \n"),
			__FUNCTION__, __LINE__,PRINT_MAC(cli_event->Addr),entry->Channel);

#ifdef BND_STRG_QA
#ifdef WPS_SUPPORT
	BND_STRG_PRINTQAMSG(table, entry,("[%s] state:%s band:%s connected band:%s Channel:%d bConnStatus:%d bActiveStatus:%d WpsAssocState:%d\n"),
		inf->ucIfName, entry_state[entry->state], bndstrg_get_entry_band(entry->band), bndstrg_get_entry_band(entry->connected_band), entry->Channel, 
		entry->bConnStatus, entry->bActiveStatus, entry->WpsAssoc);
#else
	BND_STRG_PRINTQAMSG(table, entry,("[%s] state:%s band:%s connected band:%s Channel:%d bConnStatus:%d bActiveStatus:%d \n"),
		inf->ucIfName, entry_state[entry->state], bndstrg_get_entry_band(entry->band), bndstrg_get_entry_band(entry->connected_band), entry->Channel, entry->bConnStatus, entry->bActiveStatus);
#endif /*WPS_SUPPORT*/
#endif /* BND_STRG_QA */

#ifdef BND_STRG_DBG
		if(entry->total_steered_cnt == 0)
			sprintf(entry->statistics[band_idx].FirstAssocTime, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
		entry->statistics[band_idx].AssocCount++;
#endif

}

void bndstrg_update_disassoc_info(struct bndstrg *bndstrg, struct bndstrg_iface *inf, 
	struct bndstrg_cli_entry *entry, struct bndstrg_msg *msg)
{	
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bnd_msg_cli_event *cli_event = &msg->data.cli_event;
	u8 band_idx = 0;
#ifdef BND_STRG_DBG
	time_t current_time = 0;
	struct tm * time_info = NULL;
#endif
#ifdef BND_STRG_DBG
	if(entry->total_steered_cnt == 0){
		time(&current_time);
		time_info = localtime(&current_time);				
	}
#endif	

	if(entry->bActiveStatus == CLI_ACTIVE)
	{
		if(IS_2G_BAND(cli_event->Band))
		{
			if(table->active_client_2G)
				table->active_client_2G -- ;
			band_idx = IDX_2G;
			entry->Control_Flags &= (~ fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G);
		}
		else if(IS_5G_BAND(cli_event->Band))
		{
			if(table->active_client_5G)
				table->active_client_5G -- ;
			if (IS_5G_H_BAND(inf->Band)){
				table->active_client_H5G --;
			band_idx = IDX_5GH;
				entry->Control_Flags &= (~ fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G);
			}else{
				table->active_client_L5G --;
				band_idx = IDX_5GL;
				entry->Control_Flags &= (~ fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G);
			}
		}

#ifdef BNDSTRG_NVRAM_SUPPORT
		bndstrg_nvram_update_entry(bndstrg, entry);
#endif
		entry->bActiveStatus = CLI_NOTACTIVE;
		entry->connected_band = BAND_INVALID;
#ifdef BND_STRG_DBG			
		if(entry->total_steered_cnt == 0)
			sprintf(entry->statistics[band_idx].FirstClientDelTime, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
		entry->statistics[band_idx].MacDelCount++;
#endif						
		DBGPRINT(DEBUG_OFF,RED("%s()::Entry DEL for::%02x:%02x:%02x:%02x:%02x:%02x,channel=%d\n"), 
			__FUNCTION__, PRINT_MAC(cli_event->Addr),cli_event->Channel);
	}

#ifdef BND_STRG_QA
		BND_STRG_PRINTQAMSG(table, entry,("[%s] state:%s band:%s connected band:%s Channel:%d bConnStatus:%d bActiveStatus:%d \n"),
			inf->ucIfName, entry_state[entry->state], bndstrg_get_entry_band(entry->band), bndstrg_get_entry_band(entry->connected_band), entry->Channel, entry->bConnStatus, entry->bActiveStatus);
#endif /* BND_STRG_QA */

}

u8 bndstrg_assoc_sanity_check(struct bndstrg *bndstrg, struct bndstrg_iface *inf, 
	struct bndstrg_cli_entry *entry, struct bndstrg_msg *msg)
{
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bnd_msg_cli_event *cli_event = &msg->data.cli_event;

#ifdef BND_STRG_QA
	BND_STRG_PRINTQAMSG(table, entry,("[%s] state:%s band:%s connected band:%s Channel:%d bConnStatus:%d bActiveStatus:%d \n"),
		inf->ucIfName, entry_state[entry->state], bndstrg_get_entry_band(entry->band), bndstrg_get_entry_band(entry->connected_band), entry->Channel, entry->bConnStatus, entry->bActiveStatus);
#endif /* BND_STRG_QA */

	if(table->BndStrgMode == POST_CONNECTION_STEERING){
		/* In Operational steering only condition entry-> band is not decided as assoc steering is not executing,
		 * so there is no need to check association sanity, as client can connect any band.
		*/
		return TRUE;
	}
	
#ifdef WPS_SUPPORT	
	//WPS
	if(cli_event->data.cli_assoc.bWpsAssoc){
		/* In WPS, its Station who decides band not deamon, so allow assoc on different band than entry->band
		*/
		DBGPRINT(DEBUG_TRACE,BLUE("\n[%s][%d]Addr::%02x:%02x:%02x:%02x:%02x:%02x, channel=%d(%d,%d,%d) (%d,%d,%d,%d)\n\r"),
		__FUNCTION__,__LINE__,PRINT_MAC(cli_event->Addr),cli_event->Channel,entry->bConnStatus,cli_event->FrameType,entry->bActiveStatus,
	    table->active_client_5G,table->active_client_L5G,table->active_client_H5G,cli_event->Band);
		return TRUE;
	}
#endif /*WPS_SUPPORT*/

	if(IS_2G_BAND(cli_event->Band))
	{
		if (entry->band != cli_event->Band) {
			DBGPRINT(DEBUG_ERROR,("\n[%s][%d]Error Addr::%02x:%02x:%02x:%02x:%02x:%02x, channel=%d\n\r"),
				__FUNCTION__,__LINE__,PRINT_MAC(cli_event->Addr),cli_event->Channel);
			return FALSE;
		}
	}
	else if(IS_5G_BAND(cli_event->Band))
	{
		if (IS_5G_H_BAND(inf->Band))
		{
			if (entry->band != BAND_5G_H) {
				DBGPRINT(DEBUG_ERROR,("\n[%s][%d]Error Addr::%02x:%02x:%02x:%02x:%02x:%02x, channel=%d\n\r"),
					__FUNCTION__,__LINE__,PRINT_MAC(cli_event->Addr),cli_event->Channel);
				return FALSE;
			}
		} else {
			if (entry->band != BAND_5G_L) {
				DBGPRINT(DEBUG_ERROR,("\n[%s][%d]Error Addr::%02x:%02x:%02x:%02x:%02x:%02x, channel=%d\n\r"),
					__FUNCTION__,__LINE__,PRINT_MAC(cli_event->Addr),cli_event->Channel);
				return FALSE;
			}
		}
	}
	
    DBGPRINT(DEBUG_TRACE,BLUE("\n[%s][%d]Addr::%02x:%02x:%02x:%02x:%02x:%02x, channel=%d(%d,%d,%d) (%d,%d,%d,%d)\n\r"),
		__FUNCTION__,__LINE__,PRINT_MAC(cli_event->Addr),cli_event->Channel,entry->bConnStatus,cli_event->FrameType,entry->bActiveStatus,
	    table->active_client_5G,table->active_client_L5G,table->active_client_H5G,cli_event->Band);
	return TRUE;
}

u8 bndstrg_disassoc_sanity_check(struct bndstrg *bndstrg, struct bndstrg_iface *inf, 
	struct bndstrg_cli_entry *entry, struct bndstrg_msg *msg)
{
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bnd_msg_cli_event *cli_event = &msg->data.cli_event;

#ifdef BND_STRG_QA
		BND_STRG_PRINTQAMSG(table, entry,("[%s] state:%s band:%s connected band:%s Channel:%d bConnStatus:%d bActiveStatus:%d \n"),
			inf->ucIfName, entry_state[entry->state], bndstrg_get_entry_band(entry->band), bndstrg_get_entry_band(entry->connected_band), entry->Channel, entry->bConnStatus, entry->bActiveStatus);
#endif /* BND_STRG_QA */

	/* check CLI_UPDATE is from correct band */
	if(IS_2G_BAND(cli_event->Band))
	{		
		if (entry->connected_band != cli_event->Band) {
			DBGPRINT(DEBUG_ERROR,("\n[%s][%d]Error Addr::%02x:%02x:%02x:%02x:%02x:%02x, channel=%d\n\r"),
				__FUNCTION__,__LINE__,PRINT_MAC(cli_event->Addr),cli_event->Channel);
			return FALSE;
		}
	}
	else if(IS_5G_BAND(cli_event->Band))
	{
		if (IS_5G_H_BAND(inf->Band))
		{
			if (entry->connected_band != BAND_5G_H) {
				DBGPRINT(DEBUG_ERROR,("\n[%s][%d]Error Addr::%02x:%02x:%02x:%02x:%02x:%02x, channel=%d\n\r"),
					__FUNCTION__,__LINE__,PRINT_MAC(cli_event->Addr),cli_event->Channel);
				return FALSE;
			}
		} else {
			if (entry->connected_band != BAND_5G_L) {
				DBGPRINT(DEBUG_ERROR,("\n[%s][%d]Error Addr::%02x:%02x:%02x:%02x:%02x:%02x, channel=%d\n\r"),
					__FUNCTION__,__LINE__,PRINT_MAC(cli_event->Addr),cli_event->Channel);
				return FALSE;
			}
		}
	}
    DBGPRINT(DEBUG_TRACE,BLUE("\n[%s][%d]Addr::%02x:%02x:%02x:%02x:%02x:%02x, channel=%d(%d,%d,%d) (%d,%d,%d,%d)\n\r"),
		__FUNCTION__,__LINE__,PRINT_MAC(cli_event->Addr),cli_event->Channel,entry->bConnStatus,cli_event->FrameType,entry->bActiveStatus,
	    table->active_client_5G,table->active_client_L5G,table->active_client_H5G,cli_event->Band);
	return TRUE;
}

void get_current_system_tick(
	struct timespec *now)
{
	clock_gettime(CLOCK_REALTIME, now);
}


int bndstrg_insert_entry(
	struct bndstrg_cli_table *table,
	unsigned char *pAddr,
	struct bndstrg_cli_entry **entry_out)
{
	int i;
	unsigned char HashIdx;
	struct bndstrg_cli_entry *entry = NULL, *this_entry = NULL;
	struct bndstrg_cli_entry *entry_del = NULL;
	struct bndstrg_iface *inf_target = NULL;
	struct bndstrg *bndstrg = table->bndstrg;
	struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;

	if (table->Size >= table->max_steering_size) {
		entry_del = bndstrg_get_old_entry(bndstrg, NULL);
		if(entry_del){			
			inf_target = bndstrg_get_interface(ctrl_iface, NULL, entry_del->band, TRUE);
			if(inf_target && entry_del->band!=BAND_INVALID) bndstrg_accessible_cli(bndstrg, inf_target, entry_del, CLI_DEL);
			bndstrg_delete_entry(table,entry_del->Addr,entry_del->TableIndex);
		}
		else {
		DBGPRINT(DEBUG_WARN, "%s(): Table is full!\n", __FUNCTION__);
		return BND_STRG_TABLE_FULL;
	}
	}

	for (i = 0; i< table->max_steering_size; i++)
	{
		entry = &table->Entry[i];

		/* pick up the first available vacancy*/
		if (entry->bValid == FALSE)	{
			memset(entry, 0, sizeof(struct bndstrg_cli_entry));
			/* Fill Entry */
			get_current_system_tick(&entry->tp);
			get_current_system_tick(&entry->activity_tp);
			memcpy(entry->Addr, pAddr, MAC_ADDR_LEN);
			entry->TableIndex = i;
			entry->bValid = TRUE;
			entry->match_steered_rule_id[0] = fBND_STRG_PRIORITY_MAX;
			entry->match_steered_rule_id[1] = fBND_STRG_PRIORITY_MAX;
			entry->match_steered_rule_id[2] = fBND_STRG_PRIORITY_MAX;
#ifdef BND_STRG_QA			
			BND_STRG_PRINTQAMSG(table, entry, YLW("%s[%d]Addr=%02x:%02x:%02x:%02x:%02x:%02x\n"),
								__func__,__LINE__,PRINT_MAC(entry->Addr));
#endif
			break;
		}
		entry = NULL;
	}

	if (entry) {
		/* add this MAC entry into HASH table */
		HashIdx = MAC_ADDR_HASH_INDEX(pAddr);
		if (table->Hash[HashIdx] == NULL) {
			table->Hash[HashIdx] = entry;
		} else {
			this_entry = table->Hash[HashIdx];
			while (this_entry->pNext != NULL) {
				this_entry = this_entry->pNext;
			}
			this_entry->pNext = entry;
		}
		entry->CliSteerInfo.end_idx = -1;
		entry->band = BAND_INVALID;
		if(table->BndStrgMode == POST_CONNECTION_STEERING)
			entry->state = ENTRY_READY_TO_ASSOC;
		else
			entry->state = ENTRY_INIT;
		*entry_out = entry;
		table->Size++;
	}

	BND_STRG_DBGPRINT(DEBUG_INFO,"%s(): Index=%u, %02x:%02x:%02x:%02x:%02x:%02x, Table Size = %u\n",
		__FUNCTION__, i, PRINT_MAC(pAddr), table->Size);

	return BND_STRG_SUCCESS;
}

int bndstrg_delete_entry(struct bndstrg_cli_table *table, unsigned char *pAddr, u32 Index)
{
	u16 HashIdx = 0;
	struct bndstrg_cli_entry *entry = NULL, *pre_entry, *this_entry;
	int ret_val = BND_STRG_SUCCESS;

	if (Index >= table->max_steering_size)
	{
		if (pAddr == NULL) {
			DBGPRINT(DEBUG_ERROR, RED("%s()::debug here\n"), __FUNCTION__);
			return BND_STRG_INVALID_ARG;
		}
		BND_STRG_DBGPRINT(DEBUG_TRACE, "%s(): Index=%u, %02x:%02x:%02x:%02x:%02x:%02x, Table Size = %u\n",
			__FUNCTION__, Index, PRINT_MAC(pAddr), table->Size);
		HashIdx = MAC_ADDR_HASH_INDEX(pAddr);
#ifdef BND_STRG_QA		
		BND_STRG_PRINTQAMSG(table, entry, YLW("%s[%d]Addr=%02x:%02x:%02x:%02x:%02x:%02x\n"),
			__func__,__LINE__,PRINT_MAC(entry->Addr));
#endif
		entry = table->Hash[HashIdx];
		while (entry) {
			if (MAC_ADDR_EQUAL(pAddr, entry->Addr)) {
				/* this is the entry we're looking for */
				break;
			} else {
				entry = entry->pNext;
			}
		}

		if (entry == NULL)
		{
			BND_STRG_DBGPRINT(DEBUG_WARN, "%s(): Index=%u, %02x:%02x:%02x:%02x:%02x:%02x, Entry not found.\n",
				__FUNCTION__, Index, PRINT_MAC(pAddr));
			return BND_STRG_INVALID_ARG;
		}
	}
	else {
		entry = &table->Entry[Index];
		BND_STRG_DBGPRINT(DEBUG_TRACE, "%s(): Index=%u, %02x:%02x:%02x:%02x:%02x:%02x, Table Size = %u\n",
			__FUNCTION__, Index, PRINT_MAC(entry->Addr), table->Size);
		if (entry && entry->bValid) {
			HashIdx = MAC_ADDR_HASH_INDEX(entry->Addr);
		}
	}

	if (entry && entry->bValid) 
	{
		pre_entry = NULL;
		this_entry = table->Hash[HashIdx];
		if (this_entry != NULL)
		{
			/* update Hash list*/
			do
			{
				if (this_entry == entry)
				{
					if (pre_entry == NULL)
						table->Hash[HashIdx] = entry->pNext;
					else
						pre_entry->pNext = entry->pNext;
					break;
				}

				pre_entry = this_entry;
				this_entry = this_entry->pNext;
			} while (this_entry);
		}

		/* not found !!!*/
		memset(entry->Addr, 0, MAC_ADDR_LEN);
		entry->tp.tv_sec = 0;
		entry->elapsed_time = 0;
		entry->bValid = FALSE;
		entry->band = BAND_INVALID;
		entry->Control_Flags = 0;
		memset(entry,0x00,sizeof(struct bndstrg_cli_entry));
		table->Size--;
	}

	return ret_val;
}

int bndstrg_cli_event_req(
		struct bndstrg *bndstrg,
		struct bndstrg_msg *msg)
{
	int 	ret_val = BND_STRG_SUCCESS;
	struct bnd_msg_cli_event *cli_event = &msg->data.cli_event;
	struct bndstrg_cli_entry *entry = NULL;
	struct bndstrg_iface *inf = NULL;
	struct bndstrg_cli_table *table = &bndstrg->table;
	u8 			channel = cli_event->Channel;
	unsigned char *pSrcAddr = cli_event->Addr;
	u8			FrameType = cli_event->FrameType;
	
	DBGPRINT(DEBUG_TRACE, "%02x:%02x:%02x:%02x:%02x:%02x, Band = %u, Channel = %u frame_type = %u\n",
		PRINT_MAC(cli_event->Addr), cli_event->Band, cli_event->Channel, cli_event->FrameType);

	if (table->bInitialized == FALSE || table->bEnabled == FALSE){
		DBGPRINT(DEBUG_OFF, "%s BND_STRG_NOT_INITIALIZED\n", __FUNCTION__);
		return BND_STRG_NOT_INITIALIZED;
	}

	inf = bndstrg_get_interface_by_channel(&bndstrg->ctrl_iface,channel);
	if(!inf){
		DBGPRINT(DEBUG_OFF,"Invalid Channel info for [%02x:%02x:%02x:%02x:%02x:%02x] Channel:%d \n",
			PRINT_MAC(pSrcAddr),channel);
		return BND_STRG_UNEXP;
	}
	
#if 1/* debug purpose */
	if (table->debug_only_allow_monitor_addr == 1)
	{
		unsigned char ZeroAddr[MAC_ADDR_LEN] = {0x00,0x00,0x00,0x00,0x00,0x00};
		if (!MAC_ADDR_EQUAL(table->MonitorAddr, ZeroAddr)) 
		{/* debug mode */
			if (!MAC_ADDR_EQUAL(table->MonitorAddr, pSrcAddr)) 
				return BND_STRG_UNEXP;
		}
	}
#endif
	
	entry = bndstrg_table_lookup(table, pSrcAddr);
	
	if (entry == NULL) {
		/* Discard Whitelisted STA's Assoc and Disassoc event*/
		if((cli_event->FrameType == APMT2_PEER_ASSOC_REQ) || (cli_event->FrameType == APMT2_PEER_DISASSOC_REQ)){
			int i = 0;
			for (i = 0; i < BND_STRG_MAX_DISABLE_LIST; i++){
				if (memcmp(table->WhiteEntryList[i], cli_event->Addr, MAC_ADDR_LEN) == 0){
					DBGPRINT(DEBUG_TRACE,("Discard whitelisted STA event %02x:%02x:%02x:%02x:%02x:%02x, Band = %u, Channel = %u frame_type = %u \n"),
							PRINT_MAC(cli_event->Addr), cli_event->Band, cli_event->Channel, cli_event->FrameType);
					return BND_STRG_SUCCESS;
				}
			}
		}

		ret_val = bndstrg_insert_entry(table, pSrcAddr, &entry);
		if (ret_val == BND_STRG_TABLE_FULL) {
			//bndstrg_stop(bndstrg,BNDSTRG_DAEMON_TBL_FULL);
			ret_val = BND_STRG_UNEXP;
			return ret_val;
		}
#ifdef BNDSTRG_NVRAM_SUPPORT
		if (table->bndstrg->nvram_support && entry){
			struct bndstrg_nvram_client *nvram_entry = NULL;
			int i;

			for(i = 0; i < table->bndstrg_nvram_client_count ; i++){
				if(MAC_ADDR_EQUAL(table->nvram_entry[i].Addr,entry->Addr))
					break;
			}

			if(i < table->bndstrg_nvram_client_count)
			{
				nvram_entry = &table->nvram_entry[i];
				/* 
				 * for single band STA, it will connect to AP after 2GCheckTime/5GCheckTime
				 * in first time and store its' Manipulable as 0 in nvram.
				 * In next time, this STA can connect to the AP immediately since
				 * daemon will consider it belong to specific band from manipulable setting
				 */
#ifdef	VENDOR_FEATURE7_SUPPORT
				entry->Control_Flags |= (nvram_entry->ControlFlags & ControlFlag_ReadMask);
#else		
			if((IS_2G_BAND(cli_event->Band)) && (!nvram_entry->Manipulable)){
				entry->Control_Flags |= fBND_STRG_CLIENT_IS_2G_ONLY ;	
			} else if((IS_5G_BAND(cli_event->Band)) && (!nvram_entry->Manipulable)){
				entry->Control_Flags |= fBND_STRG_CLIENT_IS_5G_ONLY ;	
			}
				
#endif
			}
		}
#endif	
	}

	if(FrameType == APMT2_PEER_PROBE_REQ)
		bndstrg_update_probe_info(bndstrg, inf, entry, cli_event);
		
	if(FrameType == APMT2_PEER_AUTH_REQ)
		bndstrg_update_auth_info(bndstrg, inf, entry, cli_event);

	switch (entry->state) {
	case ENTRY_INIT:
		if(FrameType == APMT2_PEER_AUTH_REQ) {
		}

		else if(FrameType == APMT2_PEER_ASSOC_REQ) {
#ifdef WPS_SUPPORT		
			//WPS: handle in case probe only on single band
			struct bnd_msg_cli_assoc *cli_assoc = &cli_event->data.cli_assoc;
			if(cli_assoc->bWpsAssoc)
			{
				if (bndstrg_assoc_sanity_check(bndstrg, inf, entry, msg) == FALSE)
					return BND_STRG_INVALID_ARG;
				
				bndstrg_update_assoc_info(bndstrg, inf, entry, msg);
				entry->state = ENTRY_ASSOC_RCD;	

#ifdef BND_STRG_QA
				BND_STRG_PRINTQAMSG(table, entry, (" %02x:%02x:%02x:%02x:%02x:%02x [band:%s][Channel:%d] direct WPS ASSOC in ENTRY_INIT\n"),
					PRINT_MAC(entry->Addr),bndstrg_get_entry_band(entry->band), entry->Channel);
#endif /* BND_STRG_QA */
			}
#endif /*WPS_SUPPORT*/	
		}

		else if(FrameType == APMT2_PEER_DISASSOC_REQ) {
		}

		/* For IOS immediately connect */
		if (entry->bAllowStaConnectImmediately == TRUE && entry->state == ENTRY_INIT &&
			(entry->statistics[IDX_5GL].ProbeReqCount> 0 || entry->statistics[IDX_5GH].ProbeReqCount > 0))
		{
			entry->Control_Flags |= fBND_STRG_CLIENT_IS_5G_ONLY;
			BND_STRG_PRINTQAMSG(table, entry,GRN("%s[%d]:Detect IOT issue, treat Ios STA %02x:%02x:%02x:%02x:%02x:%02x band:%d, as 5G_ONLY STA.\n\n"),
				__func__,__LINE__, PRINT_MAC(entry->Addr), entry->band);
		}
		
		if (entry->bAllowStaConnectImmediately == TRUE && entry->state == ENTRY_INIT &&
			entry->statistics[IDX_2G].ProbeReqCount> 0)
		{
			entry->Control_Flags |= fBND_STRG_CLIENT_IS_2G_ONLY;
			BND_STRG_PRINTQAMSG(table, entry,GRN("%s[%d]:Detect IOT issue, treat Ios STA %02x:%02x:%02x:%02x:%02x:%02x band:%d, as 2G_ONLY STA.\n\n"),
				__func__,__LINE__, PRINT_MAC(entry->Addr), entry->band);
		}

	break;

	case ENTRY_READY_TO_ASSOC:
		if(FrameType == APMT2_PEER_ASSOC_REQ) {
			struct bnd_msg_cli_assoc *cli_assoc = &cli_event->data.cli_assoc;
			u32 cmp_flag=0;
			if (bndstrg_assoc_sanity_check(bndstrg, inf, entry, msg) == FALSE)
				return BND_STRG_INVALID_ARG;
			
			bndstrg_update_assoc_info(bndstrg, inf, entry, msg);
			entry->state = ENTRY_ASSOC_RCD;

			if(table->BndStrgMode & PRE_CONNECTION_STEERING){
				/* 
				 *  Handle scenario in which STA's Probe and Assoc info missmatch
				 */
				if (IS_2G_BAND(inf->Band))
					cmp_flag = fBND_STRG_CLIENT_NOT_SUPPORT_HT_2G;
				else if (IS_5G_L_BAND(inf->Band))
					cmp_flag = fBND_STRG_CLIENT_NOT_SUPPORT_HT_L5G;
				else if (IS_5G_H_BAND(inf->Band))
					cmp_flag = fBND_STRG_CLIENT_NOT_SUPPORT_HT_H5G;
			
				if((cli_assoc->Nss != entry->Nss) ||
					((cli_assoc->bVHTCapable == TRUE) && !(entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_VHT)) ||
					((cli_assoc->bAllowStaConnectInHt == TRUE) && (entry->Control_Flags & cmp_flag))){
					DBGPRINT(DEBUG_OFF,"Assoc and Probe info missmatch for [%02x:%02x:%02x:%02x:%02x:%02x] "
						"ProbeInfo-> nss:%d HTCap:%s VHTCap:%s \t AssocInfo-> nss:%d HTCap:%s VHTCap:%s\n",
						PRINT_MAC(pSrcAddr), entry->Nss, (entry->Control_Flags & cmp_flag)? "FALSE":"TRUE", (entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_VHT)? "TRUE":"FALSE",
						cli_assoc->Nss, (cli_assoc->bAllowStaConnectInHt == TRUE)? "TRUE":"FALSE", (cli_assoc->bVHTCapable == TRUE)? "TRUE":"FALSE");
					entry->Nss = cli_assoc->Nss;
					if(cli_assoc->bVHTCapable == TRUE)
						entry->Control_Flags |= fBND_STRG_CLIENT_SUPPORT_VHT;
					if(cli_assoc->bAllowStaConnectInHt == TRUE)
						entry->Control_Flags &= ~(fBND_STRG_CLIENT_NOT_SUPPORT_HT_2G | fBND_STRG_CLIENT_NOT_SUPPORT_HT_L5G | fBND_STRG_CLIENT_NOT_SUPPORT_HT_H5G);
					entry->AssocProbeInfoMissMatch = TRUE;
					if(!((entry->match_steered_rule_id[CMP_5GL_2G] == fBND_STRG_PRIORITY_BAND_PERSIST ) ||
						 (entry->match_steered_rule_id[CMP_5GH_2G] == fBND_STRG_PRIORITY_BAND_PERSIST) || 
						 (entry->match_steered_rule_id[CMP_5GH_5GL] == fBND_STRG_PRIORITY_BAND_PERSIST))){
						entry->bConnStatus = FALSE;
						entry->Channel = 0;
					}
				}
			}
		}

		if(FrameType == APMT2_PEER_DISASSOC_REQ) 
		{
			/* 
			 *  Handle scenario in which STA's Probe and Assoc info missmatch
			 */
			if (entry->AssocProbeInfoMissMatch){
				if (bndstrg_disassoc_sanity_check(bndstrg, inf, entry, msg) == FALSE)
					return BND_STRG_INVALID_ARG;
	    		bndstrg_update_disassoc_info(bndstrg, inf, entry, msg);
				get_current_system_tick(&entry->tp);
			}
		}

		break;

	case ENTRY_ASSOC_RCD:
		if(FrameType == APMT2_PEER_AUTH_REQ) 
		{	
			if((table->BndStrgMode != POST_CONNECTION_STEERING) && (IS_BND_STRG_DUAL_BAND_CLIENT(entry->Control_Flags) | IS_BND_STRG_H5G_L5G_BAND_CLIENT(entry->Control_Flags)))
			{
				/* client disconnected without sending deauth*/
#ifdef BND_STRG_QA
				BND_STRG_PRINTQAMSG(table, entry, (" %02x:%02x:%02x:%02x:%02x:%02x [band:%s][Channel:%d] re-steering needed! \n"),
					PRINT_MAC(entry->Addr),bndstrg_get_entry_band(entry->band), entry->Channel);
#endif /* BND_STRG_QA */
				get_current_system_tick(&entry->tp);
				if(entry->connected_band == BAND_2G)
					table->active_client_2G--;
				else if(entry->connected_band == BAND_5G_H){
					table->active_client_5G--;
					table->active_client_H5G--;
				}
				else if(entry->connected_band == BAND_5G_L){
					table->active_client_5G--;
					table->active_client_L5G--;
				}
			/* Remove STA deauth causing IOT issue*/	
			/*
				inf = bndstrg_get_interface_by_channel(&bndstrg->ctrl_iface,entry->Channel);
				if(inf){
					if(IS_2G_BAND(entry->band))
						entry->Control_Flags &= (~ fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G);
					else if(IS_5G_L_BAND(entry->band))
						entry->Control_Flags &= (~ fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G);
					else if(IS_5G_H_BAND(entry->band))
						entry->Control_Flags &= (~ fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G);
					bndstrg_accessible_cli(bndstrg,inf,entry,CLI_DEL);
				}
			*/
				/* clear entry*/
				entry->Channel = 0;
				entry->band = BAND_INVALID;
				entry->Control_Flags &= (fBND_STRG_CLIENT_NOT_SUPPORT_HT_2G | fBND_STRG_CLIENT_NOT_SUPPORT_HT_L5G | fBND_STRG_CLIENT_NOT_SUPPORT_HT_H5G);
				entry->Control_Flags |= fBND_STRG_CLIENT_RE_STEERING;
				/* connected_band info needed to adjust stat if algo switches band*/
				/*entry->connected_band = BAND_INVALID; */
				entry->bActiveStatus = CLI_INIT;
				entry->bConnStatus = FALSE;
				entry->state = ENTRY_INIT;
				/* Clear stat as old data may be used for */
				//memset(&entry->statistics, 0, sizeof(struct bndstrg_entry_stat) * MAX_INF_NUM );
				bndstrg_update_auth_info(bndstrg, inf, entry, cli_event); /* update Rx Auth data*/
			}
		}

		else if(FrameType == APMT2_PEER_ASSOC_REQ) {

			if (bndstrg_assoc_sanity_check(bndstrg, inf, entry, msg) == FALSE)
				return BND_STRG_INVALID_ARG;
			
			if(table->BndStrgMode == POST_CONNECTION_STEERING){
				bndstrg_update_assoc_info(bndstrg, inf, entry, msg);
			}
		}

		else if(FrameType == APMT2_PEER_DISASSOC_REQ) {

			if (bndstrg_disassoc_sanity_check(bndstrg, inf, entry, msg) == FALSE)
				return BND_STRG_INVALID_ARG;
#ifdef WPS_SUPPORT				
			//WPS: handle first wps disassoc, to allow second connection
			if(entry->WpsAssoc)
			{
				entry->WpsAssoc = WPS_FIRST_DISCONNECTION;
				entry->state = ENTRY_DELETE_RCD;
			}
			else
#endif /*WPS_SUPPORT*/			
			{
	        	entry->bConnStatus = FALSE;
	    		bndstrg_update_disassoc_info(bndstrg, inf, entry, msg);
				get_current_system_tick(&entry->tp);
				entry->state = ENTRY_DELETE_RCD;
			}
		}

	break;

	case ENTRY_DELETE_RCD:
		if(FrameType == APMT2_PEER_AUTH_REQ) {
		}
		else if(FrameType == APMT2_PEER_ASSOC_REQ) {
	
			if (bndstrg_assoc_sanity_check(bndstrg, inf, entry, msg) == FALSE)
				return BND_STRG_INVALID_ARG;

    		bndstrg_update_assoc_info(bndstrg, inf, entry, msg);
			entry->state = ENTRY_ASSOC_RCD;
		}

		else if(FrameType == APMT2_PEER_DISASSOC_REQ) {
			if (bndstrg_disassoc_sanity_check(bndstrg, inf, entry, msg) == FALSE)
				return BND_STRG_INVALID_ARG;
		}

	break;

	case ENTRY_OPER_STEER_ACTIVE:
		if(FrameType == APMT2_PEER_ASSOC_REQ) {

			if (bndstrg_assoc_sanity_check(bndstrg, inf, entry, msg) == FALSE)
				return BND_STRG_INVALID_ARG;

    		if (entry->steered)
            {
        		entry->steered = 0;
        		entry->tx_mcs_bad_cnt = 0;
        		entry->rx_mcs_bad_cnt = 0;
				entry->low_rssi_bad_cnt = 0;
				entry->good_rssi_cnt = 0;
				if(entry->Operation_steered == TRUE){
					entry->Operation_steered = FALSE;
					entry->Operation_steered_tick = 0;
					INC(entry->CliSteerInfo.end_idx,table->max_steer_count);
					get_current_system_tick(&entry->CliSteerInfo.steer_tp[entry->CliSteerInfo.end_idx]);
					entry->CliSteerInfo.band[entry->CliSteerInfo.end_idx] = inf->Band;
					entry->total_steered_cnt++ ;
				}
    		}
			bndstrg_update_assoc_info(bndstrg, inf, entry, msg);
			entry->state = ENTRY_ASSOC_RCD;
		}
		else if(FrameType == APMT2_PEER_DISASSOC_REQ) {
			if (bndstrg_disassoc_sanity_check(bndstrg, inf, entry, msg) == FALSE)
				return BND_STRG_INVALID_ARG;
		    entry->bConnStatus = FALSE;
		    bndstrg_update_disassoc_info(bndstrg, inf, entry, msg);
			get_current_system_tick(&entry->tp);
		}

	break;

	}		
	
	if (ret_val != BND_STRG_SUCCESS)
	{
		DBGPRINT(DEBUG_TRACE, "Error in %s(), error code = %d!\n", __FUNCTION__, ret_val);
	}

	return ret_val;
}

static int _bndstrg_print_entry_statistics(struct bndstrg_entry_stat *statistics,
										 u8 dump_cli_status)
{
#ifdef BND_STRG_DBG
	static char *phy_str[]={"CCK","OFDM","HT-M","HT-G","VHT"};
	static char *bw_str[]={"20","40","80","160"};
	if (dump_cli_status) {
		BND_STRG_DBGPRINT(DEBUG_OFF,
						  	"Rssi = %4d, FirstAuthTime %s LastAuthTime %s AuthReqCount = %u FirstProbeTime %s LastProbeTime %s\n"
						  	"\t\t\t ProbeReqCount = %u FirstAssocTime %s AssocCount = %u FirstClientDelTime %s MacDelCount = %u\n",
							statistics->Rssi, statistics->FirstAuthTime, statistics->LastAuthTime, statistics->AuthReqCount, statistics->FirstProbeTime,
							statistics->LastProbeTime, statistics->ProbeReqCount, statistics->FirstAssocTime, statistics->AssocCount,
							statistics->FirstClientDelTime, statistics->MacDelCount);
		if (statistics->got_cli_status) {
			BND_STRG_DBGPRINT(DEBUG_OFF,
							  "\t\tData Rssi = %4d, tx/rx Data Rate = %4u/%4u,tx/rx TP = %4u/%4u\n",
							  statistics->data_Rssi,
							  statistics->data_tx_Rate,statistics->data_rx_Rate,
							  statistics->data_tx_TP,statistics->data_rx_TP);
			BND_STRG_DBGPRINT(DEBUG_OFF,
							  "\t\ttx/rx Phy = %4s/%4s, MCS = %u/%u, bw = %4s/%4s, sgi = %u/%u, stbc = %u/%u, ant = %u/%u\n"
							  "\t\t\ttx/rx avg_util = %3u%%/%3u%%  tx/rx packets = %6lu/%6lu tx/rx byte = %u/%u\n",
							  phy_str[statistics->data_tx_Phymode], phy_str[statistics->data_tx_Phymode],
							  statistics->data_tx_mcs, statistics->data_rx_mcs,
							  bw_str[statistics->data_tx_bw], bw_str[statistics->data_rx_bw],
							  statistics->data_tx_sgi, statistics->data_rx_sgi,
							  statistics->data_tx_stbc, statistics->data_rx_stbc,
							  statistics->data_tx_ant, statistics->data_rx_ant,
							  statistics->data_tx_util.avg/(BND_STRG_DATA_SAMPLE),
							  statistics->data_rx_util.avg/(BND_STRG_DATA_SAMPLE),
							  (long unsigned int)statistics->data_tx_packets, 
							  (long unsigned int)statistics->data_rx_packets,
							  statistics->data_tx_byte.avg,
							  statistics->data_rx_byte.avg);
		}
	} else {
	    BND_STRG_DBGPRINT(DEBUG_OFF,
						  	"Rssi = %4d, FirstAuthTime %s LastAuthTime %s AuthReqCount = %3u FirstProbeTime %s LastProbeTime %s\n" 
						  	"\t\t\t ProbeReqCount = %u FirstAssocTime %s AssocCount = %u FirstClientDelTime %s MacDelCount = %u\n",
							statistics->Rssi, statistics->FirstAuthTime, statistics->LastAuthTime, statistics->AuthReqCount, statistics->FirstProbeTime,
							statistics->LastProbeTime, statistics->ProbeReqCount, statistics->FirstAssocTime, statistics->AssocCount,
							statistics->FirstClientDelTime, statistics->MacDelCount);
	}
#endif	
	return BND_STRG_SUCCESS;
}

static int _bndstrg_print_ctrlflags(u32 flags)
{
#ifdef BND_STRG_DBG
	BND_STRG_DBGPRINT(DEBUG_OFF,
						"\t\tSupport_2G = %s\n"
						"\t\tSupport_L5G = %s\n"
						"\t\tSupport_H5G = %s\n"
						"\t\tAllow to connect 2G = %s\n"
						"\t\tAllow to connect L5G = %s\n"
						"\t\tAllow to connect H5G = %s\n"
#if 0
						"\t\tHT Support 2G = %s\n"
						"\t\tHT Support L5G = %s\n"
						"\t\tHT Support H5G = %s\n"
#endif
						"\t\tLow Rssi 2G = %s\n"
						"\t\tLow Rssi L5G = %s\n"
						"\t\tLow Rssi H5G = %s\n"
						"\t\t2G Only = %s\n"
						"\t\t5G Only = %s\n",
						(flags & fBND_STRG_CLIENT_SUPPORT_2G) ? "yes" : "no",
						(flags & fBND_STRG_CLIENT_SUPPORT_L5G) ? "yes" : "no",
						(flags & fBND_STRG_CLIENT_SUPPORT_H5G) ? "yes" : "no",
						(flags & fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G) ? "yes" : "no",
						(flags & fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G) ? "yes" : "no",
						(flags & fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G) ? "yes" : "no",
#if 0
						(flags & fBND_STRG_CLIENT_NOT_SUPPORT_HT_2G) ? "no" : "yes",
						(flags & fBND_STRG_CLIENT_NOT_SUPPORT_HT_L5G) ? "no" : "yes",
						(flags & fBND_STRG_CLIENT_NOT_SUPPORT_HT_H5G) ? "no" : "yes",
#endif
						(flags & fBND_STRG_CLIENT_LOW_RSSI_2G) ? "yes" : "no",
						(flags & fBND_STRG_CLIENT_LOW_RSSI_L5G) ? "yes" : "no",
						(flags & fBND_STRG_CLIENT_LOW_RSSI_H5G) ? "yes" : "no",
						(flags & fBND_STRG_CLIENT_IS_2G_ONLY) ? "yes" : "no",
						(flags & fBND_STRG_CLIENT_IS_5G_ONLY) ? "yes" : "no");
#endif /* BND_STRG_DBG */
	return BND_STRG_SUCCESS;
}

u32 bndstrg_get_entry_elapsed_time(struct bndstrg_cli_entry *entry)
{
	struct timespec now;
	u32 elapsed_time = 0;

	if (entry->tp.tv_sec) {
		get_current_system_tick(&now);
		elapsed_time = (now.tv_sec - entry->tp.tv_sec);
						/* ((now.tv_nsec - entry->tp.tv_nsec)/1000000); */
	}

	return elapsed_time;
}

u32 bndstrg_get_elapsed_time(struct timespec ref_time)
{
	struct timespec now;
	u32 elapsed_time = 0;

	if (ref_time.tv_sec) {
		get_current_system_tick(&now);
		elapsed_time = (now.tv_sec - ref_time.tv_sec);
						/* ((now.tv_nsec - ref_time.tv_nsec)/1000000); */
	}

	return elapsed_time;
}

u32 bndstrg_get_entry_idle_time(struct bndstrg_cli_entry *entry)
{
	struct timespec now;
	u32 idle_time = 0;

	if (entry->tp.tv_sec) {
		get_current_system_tick(&now);
		idle_time = (now.tv_sec - entry->activity_tp.tv_sec);
					 /* ((now.tv_nsec - entry->activity_tp.tv_nsec)/1000000); */
	}

	return idle_time;
}

static int _bndstrg_print_entry(
			struct bndstrg_cli_table *table,
			struct bndstrg_cli_entry *entry)
{
#ifdef BND_STRG_DBG
	char *entry_prefer_bnd[]={"?","2G","L-5G","H-5G"};
	int entry_prefer_bnd_id=0;
	u8 dump_cli_status[3];
	char temp_buf[30];
	u8	single_band = 0, i=0;
	
	if (IS_BND_STRG_H5G_ONLY_BAND_CLIENT(entry->Control_Flags))
		single_band = 1;
	if (IS_BND_STRG_L5G_ONLY_BAND_CLIENT(entry->Control_Flags))
		single_band = 1;
	if (IS_BND_STRG_2G_ONLY_BAND_CLIENT(entry->Control_Flags))
		single_band = 1;
	/* Update idle elapsed time */
	entry->idle_elapsed_time = bndstrg_get_entry_idle_time(entry);
	entry->elapsed_time = bndstrg_get_entry_elapsed_time(entry);
	memset(dump_cli_status,0x00,sizeof(dump_cli_status));
	if (entry->bValid)
	{
		if (MAC_ADDR_EQUAL(table->MonitorAddr, entry->Addr))
			BND_STRG_DBGPRINT(DEBUG_OFF, YLW("*"));
		entry_prefer_bnd_id = 0;
		if (entry->Control_Flags & fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G)
			entry_prefer_bnd_id = 3;
		else if (entry->Control_Flags & fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G)
			entry_prefer_bnd_id = 2;
		else if (entry->Control_Flags & fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G)
			entry_prefer_bnd_id = 1;
		memset(temp_buf,0x00,sizeof(temp_buf));
		if (table->op_mode == OP_TRIBAND)  {
			snprintf(temp_buf,sizeof(temp_buf),"[%d|%d|%d|%d (%d)][%d/%d]",
					 entry->steered,
					 IS_2G_BAND(entry->band)? 1:0,
					 IS_5G_L_BAND(entry->band)? 1:0,
					 IS_5G_H_BAND(entry->band)? 1:0,
					 entry->total_steered_cnt,
					 entry->Isbad,
					 entry->IsIdle);
		} 
		else 
		{
			if (IS_2G_BAND(table->Band))
			{
				snprintf(temp_buf,sizeof(temp_buf),"[%d|%d|%d (%d)][%d/%d]",
						 entry->steered,
						 IS_2G_BAND(entry->band)? 1:0,
						 IS_5G_L_BAND(entry->band)? 1:0,
						 entry->total_steered_cnt,
						 entry->Isbad,
						 entry->IsIdle);
			}
			else 
			{
				snprintf(temp_buf,sizeof(temp_buf),"[%d|%d|%d (%d)][%d/%d]",
						 entry->steered,
						 IS_5G_L_BAND(entry->band)? 1:0,
						 IS_5G_H_BAND(entry->band)? 1:0,
						 entry->total_steered_cnt,
						 entry->Isbad,
						 entry->IsIdle);
			}
		}
		if (entry->bConnStatus) {
			dump_cli_status[(entry_prefer_bnd_id >0)?(entry_prefer_bnd_id-1):0] = 1;
			BND_STRG_DBGPRINT(DEBUG_OFF,
							  YLW("\tbValid = %u, Index = %02u, Control_Flags = 0x%08x(%4s)(%d/%d/%d) %s,"
							  " Addr=%02x:%02x:%02x:%02x:%02x:%02x, idle time = %06u (s), elapsed time = %06u (s), nss=%2u, channel= %3u"
							  " bad cnt rssi/tx_mcs/rx_mcs %u/%u/%u GoodRSSICnt %u BTMSupport=%d state:%s AssoProbeInfoMissMatch:%s\n"),
							  entry->bValid, entry->TableIndex, entry->Control_Flags,
							  entry_prefer_bnd[entry_prefer_bnd_id],entry->bConnStatus,entry->bActiveStatus,entry->Manipulable,
							  temp_buf,
							  PRINT_MAC(entry->Addr), entry->idle_elapsed_time, entry->elapsed_time, entry->Nss, entry->Channel,
							  entry->low_rssi_bad_cnt, entry->tx_mcs_bad_cnt, entry->rx_mcs_bad_cnt, entry->good_rssi_cnt, 
							  entry->btm_info.BTMSupport, entry_state[entry->state], (entry->AssocProbeInfoMissMatch == TRUE ? "TRUE":"FALSE"));
		} else {
			BND_STRG_DBGPRINT(DEBUG_OFF,
				"\tbValid = %u, Index = %02u, Control_Flags = 0x%08x(%4s)(%d/%d/%d) %s,"
				  " Addr=%02x:%02x:%02x:%02x:%02x:%02x, idle time = %06u (s), elapsed time = %06u (s), nss=%2u, channel=%3u, BTMSupport=%d state:%s AssoProbeInfoMissMatch:%s\n",
					entry->bValid, entry->TableIndex, entry->Control_Flags,
					entry_prefer_bnd[entry_prefer_bnd_id],entry->bConnStatus,entry->bActiveStatus,entry->Manipulable,
					temp_buf,
					PRINT_MAC(entry->Addr), entry->idle_elapsed_time, entry->elapsed_time, entry->Nss, entry->Channel, 
					entry->btm_info.BTMSupport, entry_state[entry->state], (entry->AssocProbeInfoMissMatch == TRUE ? "TRUE":"FALSE"));
		}

		if(IS_2G_BAND(table->Band)){
			BND_STRG_DBGPRINT(DEBUG_OFF,
				"\t\t2G  :");
			_bndstrg_print_entry_statistics(&entry->statistics[IDX_2G],dump_cli_status[0]);
		}
		if (IS_5G_L_BAND(table->Band))
		{
			BND_STRG_DBGPRINT(DEBUG_OFF,
				"\t\tL-5G:");
			_bndstrg_print_entry_statistics(&entry->statistics[IDX_5GL],dump_cli_status[1]);
		}
		if (IS_5G_H_BAND(table->Band))
		{
			BND_STRG_DBGPRINT(DEBUG_OFF,
				"\t\tH-5G:");
			_bndstrg_print_entry_statistics(&entry->statistics[IDX_5GH],dump_cli_status[2]);
		}
		if (MAC_ADDR_EQUAL(table->MonitorAddr, entry->Addr)) {
			_bndstrg_print_ctrlflags(entry->Control_Flags);
			BND_STRG_DBGPRINT(DEBUG_OFF,
							  "\t\tPrefer    Band = %d\n"
							  "\t\t\tConnected Band = %d\n",
							  entry->band,
							  entry->connected_band);
		}
		if(entry->total_steered_cnt > 0){
			BND_STRG_DBGPRINT(DEBUG_OFF,
				  "\t\tClient steered info[Elapsed time for steered band in sec]\n");
			printf("\t\t\t");
			i = (entry->CliSteerInfo.end_idx + (table->max_steer_count - entry->total_steered_cnt + 1))%table->max_steer_count;
			for(;i<entry->total_steered_cnt;i++)
			{
				printf("Time: %u Band: %u  ",
					bndstrg_get_elapsed_time(entry->CliSteerInfo.steer_tp[i]),
					entry->CliSteerInfo.band[i]);
			}
			printf("\n");
		}
		
		if (single_band == 1)
		{
			BND_STRG_DBGPRINT(DEBUG_OFF, "\t\tSINGLE BAND\n");
			return BND_STRG_SUCCESS;
		}
		
		if (IS_BND_STRG_H5G_L5G_BAND_CLIENT(entry->Control_Flags))
		{
			BND_STRG_DBGPRINT(DEBUG_OFF, "\t\tH-5G vs L-5G = %s ",
				entry_match_steering_str[entry->match_steered_rule_id[CMP_5GH_5GL]]);
			if(entry->match_steered_rule_id[CMP_5GH_5GL] == fBND_STRG_PRIORITY_LB_CND_CHANLOAD_ACTIVE)
			{
				printf("[MachedChanloadActiveCnd:%s]\n",entry_match_chanload_active_str[entry->match_lb_chanload_active_cond_id[CMP_5GH_5GL]]);
			}else
				printf("\n");
		}
		
		if (IS_5G_H_BAND(table->Band))
		{
			if (IS_BND_STRG_DUAL_BAND_CLIENT(entry->Control_Flags))
			{
				BND_STRG_DBGPRINT(DEBUG_OFF, "\t\tH-5G vs 2G = %s",
					entry_match_steering_str[entry->match_steered_rule_id[CMP_5GH_2G]]);
			}
			if(entry->match_steered_rule_id[CMP_5GH_2G] == fBND_STRG_PRIORITY_LB_CND_CHANLOAD_ACTIVE)
			{
				printf("[MachedChanloadCnd:%s]\n",entry_match_chanload_active_str[entry->match_lb_chanload_active_cond_id[CMP_5GH_2G]]);
			}else
				printf("\n");
		}
		
		if (IS_5G_L_BAND(table->Band))
		{
			if (IS_BND_STRG_DUAL_BAND_CLIENT(entry->Control_Flags))
			{
				BND_STRG_DBGPRINT(DEBUG_OFF, "\t\tL-5G vs 2G = %s ",
					entry_match_steering_str[entry->match_steered_rule_id[CMP_5GL_2G]]);
			}
			if(entry->match_steered_rule_id[CMP_5GL_2G] == fBND_STRG_PRIORITY_LB_CND_CHANLOAD_ACTIVE)
			{
				printf("[MachedChanloadCnd:%s]\n",entry_match_chanload_active_str[entry->match_lb_chanload_active_cond_id[CMP_5GL_2G]]);
			}else
				printf("\n");
		}
		if (entry->Operation_steered)
		{
			BND_STRG_DBGPRINT(DEBUG_OFF, "\t\tOperation Steering tick=%d!\n",entry->Operation_steered_tick );
		}
	}
#endif /* BND_STRG_DBG */
	return BND_STRG_SUCCESS;
}

static int _bndstrg_event_show_entries(struct bndstrg *bndstrg,struct bndstrg_msg *msg)
{
	int i;
	int entry_count_2G = 0;
	int entry_count_L5G = 0;
	int entry_count_H5G = 0;
	int entry_count_unkown = 0;
	int hash_cnt_total = 0;
	struct bndstrg_iface *inf = NULL;
	entry_count_2G = entry_count_L5G  =entry_count_H5G = entry_count_unkown = hash_cnt_total = 0;
	for (i = 0; i < bndstrg->table.max_steering_size; i++)
	{
		struct bndstrg_cli_entry *entry;
		entry = &bndstrg->table.Entry[i];
		if (entry->bValid) {
			if (entry->band == BAND_2G) {
				entry_count_2G++;
			} else if (entry->band == BAND_5G_L) {
				entry_count_L5G++;
			} else if (entry->band == BAND_5G_H) {
				entry_count_H5G++;
			} else {
				entry_count_unkown++;
			}
		}
	}
	if (msg->data.display_type.display_type != 3)
	{
		BND_STRG_DBGPRINT(DEBUG_OFF,
						  "2.4G   entry count=%d\n\r"
						  "L5G    entry count=%d\n\r"
						  "H5G    entry count=%d\n\r"
						  "unknow entry count=%d\n\r",
							entry_count_2G,
							entry_count_L5G,
							entry_count_H5G,
							entry_count_unkown);
		for (i = 0; i < bndstrg->table.max_steering_size; i++)
		{
			if (msg->data.display_type.display_type == 0) {
				/* dump all valid entry */
				_bndstrg_print_entry(&bndstrg->table, &bndstrg->table.Entry[i]);
			} else if (msg->data.display_type.display_type == 1) {
				/* dump connected entry */
				if (bndstrg->table.Entry[i].bConnStatus) {
					_bndstrg_print_entry(&bndstrg->table, &bndstrg->table.Entry[i]);
				}
			} else if (msg->data.display_type.display_type == 2) {
				/* dump entry with the same band */
				u8 band;
				if (msg->data.display_type.filer_band == BAND_2G)
					band = BAND_2G;
				else {
					inf = bndstrg_get_interface_by_channel(&bndstrg->ctrl_iface,msg->data.display_type.channel);
					if (IS_5G_H_BAND(inf->Band)) {
						band = BAND_5G_H;
					} else {
						band = BAND_5G_L;
					}
				}
				if (band == bndstrg->table.Entry[i].band){
					_bndstrg_print_entry(&bndstrg->table, &bndstrg->table.Entry[i]);
				}
			}
		}
		for (i = 0; i < HASH_TABLE_SIZE; i++)
		{
			struct bndstrg_cli_entry *this_entry;
			int hash_cnt = 0;

			this_entry = bndstrg->table.Hash[i];
			if (this_entry != NULL)
			{
				/* update Hash list*/
				do
				{
					hash_cnt++;
					this_entry = this_entry->pNext;
				} while (this_entry);
				hash_cnt_total = hash_cnt_total+hash_cnt;
			} else {
			}
		}
		/* sanity check for hash table entry */
		if (hash_cnt_total != (entry_count_2G +
								entry_count_L5G +
								entry_count_H5G +
								entry_count_unkown)) {
			BND_STRG_DBGPRINT(DEBUG_OFF,
							  "Inconsistent Hash cnt(%d) and entry count\n\r",hash_cnt_total);
		}
	}
	if (msg->data.display_type.display_type == 3)
	{
		BND_STRG_DBGPRINT(DEBUG_OFF,
						  "Non-Active entry\n\r");
		for (i = 0; i < bndstrg->table.max_steering_size; i++)
		{
			if (msg->data.display_type.display_type == 3)
			{
				/* dump all valid entry */
				if(bndstrg->table.Entry[i].bActiveStatus == CLI_NOTACTIVE)
					_bndstrg_print_entry(&bndstrg->table, &bndstrg->table.Entry[i]);
			}
		}
	}
#ifdef ROAMING_SUPPORT
	{
		PBS_LIST_ENTRY 	pBsListEntry = NULL;
		RT_LIST_ENTRY	*pListEntry = NULL;
		int index = 1;

		BND_STRG_DBGPRINT(DEBUG_OFF,
						  "ROAMING STATION ENTRIES(Count:%d):::::\n\r", bndstrg->table.roaming_list.size);
		BND_STRG_DBGPRINT(DEBUG_OFF,
						  "Index\tADDRESS\t\t\tASSOCIATED_AP\t\tLAST_UPDATE(sec)\n\r");
		pListEntry = 	bndstrg->table.roaming_list.pHead;
		pBsListEntry = (PBS_LIST_ENTRY)pListEntry;
		while (pBsListEntry != NULL)
		{
			BND_STRG_DBGPRINT(DEBUG_OFF, "%d\t%02x:%02x:%02x:%02x:%02x:%02x\t%d.%d.%d.%d\t\t%ld.%ld\n\r",
								index,
								PRINT_MAC(pBsListEntry->iapp_sta_entry.addr),
								SHOW_IP_HTONL(pBsListEntry->iapp_sta_entry.ap_ip_addr),
								pBsListEntry->last_update.sec, pBsListEntry->last_update.usec);
			index++;
			pListEntry = pListEntry->pNext;
			pBsListEntry = (PBS_LIST_ENTRY)pListEntry;
		}
	}
#endif /*ROAMING_SUPPORT*/
	return 0;
}

static int _bndstrg_event_table_info(struct bndstrg *bndstrg)
{
	struct bndstrg_cli_table *table = &bndstrg->table;
    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
    struct bndstrg_iface *inf = NULL;
    struct bndstrg_ref_iface *ref_inf = NULL;
    int i, j;
	int total_assoc_cnt=0, percent_2g_assoc=0, percent_l5g_assoc=0, percent_h5g_assoc=0;
	char *bndstrg_op_mode[] = {
		"No BandStrg Support","DualBand BandStrg Support","TriBand BandStrg Support"
	};
	char	*bndstrg_fsm_mode[] = {
		"BNDSTRG_INIT","BNDSTRG_INF_POLL","BNDSTRG_TBL_EN","BNDSTRG_TBL_READY","BNDSTRG_LEAVE"
	};

	if (!table)
	{
		DBGPRINT(DEBUG_ERROR,
					"%s(): Error! table is NULL!\n", __FUNCTION__);
		return 0;
	}
	BND_STRG_DBGPRINT(DEBUG_OFF,"Band Steering Daemon Ver - %s\n",BNDSTRG_DAEMON_VER);
	total_assoc_cnt = (table->active_client_2G+table->active_client_5G);
	if(total_assoc_cnt != 0) {
		percent_2g_assoc = table->active_client_2G *100/total_assoc_cnt;
		percent_l5g_assoc = table->active_client_L5G *100/total_assoc_cnt;
		percent_h5g_assoc = table->active_client_H5G *100/total_assoc_cnt;
	}
	BND_STRG_DBGPRINT(DEBUG_OFF,
						"%s():\n"
							"\tbInitialized = %u\n"
							"\tbEnabled = %u\n"
							"\tbActiveTotalInf = %d\n"
							"\tbActive2GInf = %d\n"
							"\tbActive5GInf = %d\n"
							"\tb2GInfReady = %u\n"
							"\tb5GInfReady = %u\n"
							"\tRssiDiff = %d (dB)\n"
							"\tRssiLow = %d (dB)\n"
							"\tRSSILowDownSteer = %d (dB)\n"
							"\tRSSIHighUpSteer = %d (dB)\n"
#ifdef VENDOR_FEATURE7_SUPPORT
							"\tRSSIDisconnect = %d (dB)\n"
							"\tBlackListTime = %d (sec)\n"
#endif
#ifdef VENDOR_IOS_SUPPORT
							"\tIOSSteerNum = %d\n"
#endif
							"\tAgeTime = %d (s)\n"
							"\tDormantTime = %d (s)\n"
							"\tHoldTime = %d (s)\n"
							"\tCheckTime = %d (s)\n"
 							"\tRSSICheckCount = %d \n"
 							"\tIdle_Tx_Byte_Count = %d \n"
 							"\tIdle_Rx_Byte_Count = %d \n"
#ifdef BND_STRG_DBG
							"\tMntAddr = %02x:%02x:%02x:%02x:%02x:%02x\n"
#endif /* BND_STRG_DBG */
							"\tSize = %u\n"
							"\tActiveClient2G = %03u\n"
							"\tActiveClient5G = %03u = (L:%03u,H:%03u)\n"
							"\tMax Steering Size=%u\n"
							"\tAssoc balance threshold=%d%%  current=(2G:%d%%,L5G:%d%%,H5G:%d%%)\n"
							"\tTable Band=0x%x\n"
							"\tOp_mode=%s\n"
							"\tBndStrg FSM=%s\n"
							"\tNvram_support=%d"
							"\tNvram_Table_Size=%d"
							"\tNvram_Entry_Count=%d\n"
							"\tNVRAMReset = %d \n"							
							"\tBtmMode=%d\n"
							"\tBndStrgMode=%d\n",
							__FUNCTION__,
							table->bInitialized,
							table->bEnabled,
							ctrl_iface->ActiveCount,
							ctrl_iface->Active2GCount,
							ctrl_iface->Active5GCount,
							table->b2GInfReady,
							table->b5GInfReady,
							table->RssiDiff,
							table->RssiLow,
							table->RSSILowDownSteer,
							table->RSSIHighUpSteer,
#ifdef VENDOR_FEATURE7_SUPPORT
							table->RSSIDisconnect,
							table->BlackListTime,
#endif
#ifdef VENDOR_IOS_SUPPORT
							table->IOSNumLimit,
#endif
							table->AgeTime,
							table->DormantTime,
							table->HoldTime,
							table->CheckTime,
							table->RssiCheckCount,
							table->IDLE_TX_Byte_Count,
							table->IDLE_RX_Byte_Count,
#ifdef BND_STRG_DBG
							PRINT_MAC(table->MonitorAddr),
#endif /* BND_STRG_DBG */
							table->Size,
							table->active_client_2G,
							table->active_client_5G,
							table->active_client_L5G,
							table->active_client_H5G,
							table->max_steering_size,
							table->assoc_bl_th,percent_2g_assoc,percent_l5g_assoc,percent_h5g_assoc,
								table->Band,
							bndstrg_op_mode[(table->op_mode > 2)?0:table->op_mode],
							bndstrg_fsm_mode[bndstrg->state],
							bndstrg->nvram_support,
							table->nvram_table_size,
							table->bndstrg_nvram_client_count,
							table->nvram_reset,							
							table->BtmMode,
							table->BndStrgMode);
		BND_STRG_DBGPRINT(DEBUG_OFF," PriorityListSize=%u\n",table->PriorityListSize);
		BND_STRG_DBGPRINT(DEBUG_OFF," PriorityList:\n");
		for (i = 0; i < table->PriorityListSize; i++){
			if(table->PriorityList[i] < fBND_STRG_PRIORITY_MAX)
				printf("\t\t %d: %s\n",table->PriorityList[i], entry_match_steering_str[table->PriorityList[i]]);
			else
				printf("\t\t\t Invalid PriorityID:%d",table->PriorityList[i]);
		}
		printf("\n");

		if(table->WhiteEntryListSize){
			u8 i=0, count=0;
			BND_STRG_DBGPRINT(DEBUG_OFF," WhiteList:\n");
			for(i=0; i < BND_STRG_MAX_DISABLE_LIST; i++){
				if(IS_VALID_MAC(table->WhiteEntryList[i])){
					printf("\t\t[%d]:%02x:%02x:%02x:%02x:%02x:%02x \n", i, PRINT_MAC(table->WhiteEntryList[i]));
					count++;
				}
				if(count >= table->WhiteEntryListSize)
					break;
			}				
		}
		
	BND_STRG_DBGPRINT(DEBUG_OFF,
						" Assoc_thr = %d\n"
						"\tdwell_time = %d(sec)\n"
						"\tmax_steer_time_window = %d(sec)\n"
						"\tmax_steer_count = %d\n"
						"\tdynamic_bndstrg_period = %d\n",
						table->assoc_bl_th,
						table->dwell_time,
						table->max_steer_time_window,
						table->max_steer_count,
						table->dynamic_bndstrg_period);

    /* For Debug Demand */
    if(ctrl_iface->Size > 0)
    {
        for (i = 0; i < ctrl_iface->Size; i++)
        {
            inf = &ctrl_iface->inf[i];
            if(inf->bValid)
            {
                BND_STRG_DBGPRINT(DEBUG_OFF,
    						"Main inf = %s\n"
    						"\t\tbInfReady = %u\n"
    						"\t\tbinfEnable = %u\n"
    						"\t\tBand = %u\n"
    						"\t\tChannel = %u\n"
    						"\t\tVHTCapable = %u\n"
    						"\t\tnss = %u\n"    						    						
    						"\t\tnss_thr = %u\n"
    						"\t\trssi_thr = %d\n"
    						"\t\tchanload_thr = %u\n"
    						"\t\tchanload = %u\n"
    						"\t\tavg_chanload = %u\n"
    						"\t\tchanload_heavy_cnt = %u\n"
    						"\t\tchanload_heavy_max_cnt = %u\n"
    						"\t\tchan_busy_load = %u\n"
    						"\t\tDriver_Max_table_Size = %u\n"
    						"\t\tDriver_Table_Size = %u\n"
							"\t\tinf = %p\n",
    						(char*)inf->ucIfName,
    						inf->bInfReady,
    						inf->binfEnable,
    						inf->Band,
    						inf->Channel,
    						inf->bVHTCapable,
    						inf->nss,
    						inf->min_nss_thr,
    						inf->min_rssi_thr,
    						inf->chanload_thr,
    						inf->chanload,
    						inf->avg_chanload.avg/BND_STRG_DATA_SAMPLE,
    						inf->chanload_heavy_cnt,
    						inf->chanload_heavy_max_cnt,
    						inf->chan_busy_load,
    						inf->max_driver_table_size,
    						inf->driver_table_size,
						   	inf);
				if(inf->worst_entry){
					 BND_STRG_DBGPRINT(DEBUG_OFF,
					 			"Worst Client :: %02x:%02x:%02x:%02x:%02x:%02x\n",
					 			PRINT_MAC(inf->worst_entry->Addr));
				}	 
                for (j = 0; j < MAX_REF_INF_NUM; j++)
                {
                    ref_inf = &inf->ref_iface[j];
                    if(ref_inf->bValid)
                    {
                        BND_STRG_DBGPRINT(DEBUG_OFF,
    						"\t\tReference inf = %s\n"
    						"\t\t\tbInfReady = %u\n",
    						(char*)ref_inf->ucIfName,
    						ref_inf->bInfReady);
                    }
                }
            }
        }
    }
	return 0;
}

static int _bndstrg_event_on_off(struct bndstrg *bndstrg, u8 onoff, u8 band, u8 channel, char *ifname)
{
    struct bndstrg_cli_table *table = &bndstrg->table;
    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
    struct bndstrg_iface *inf = NULL;
    
    inf = bndstrg_get_interface(ctrl_iface, ifname, band, FALSE);
    if(!inf)
    {
        DBGPRINT(DEBUG_WARN, "invalid interface : %s\n", ifname);
        return BND_STRG_UNEXP;
    }
	DBGPRINT(DEBUG_TRACE,
			 "onoff = %u,band = %u,channel = %u,ifname=%s,inf = %p\n",
			 onoff, band, channel, ifname, inf);
    if(inf->binfEnable^onoff)
    {
        inf->binfEnable = onoff;
        if (!onoff) 
        {
            if(IS_5G_BAND(band))
            {
                if(IS_5G_H_BAND(inf->Band)) {
                    table->Band &= ~BAND_5G_H;
				} else {
					table->Band &= ~BAND_5G_L;
				}
				if((!IS_5G_L_BAND(table->Band)) &&
				   (!IS_5G_H_BAND(table->Band)))
					table->Band &= ~band;
				
            }
            else
            {
				table->Band &= ~band;
            }
    	} 
        else 
    	{
    		table->Band |= band;
            if(IS_5G_H_BAND(inf->Band))
                table->Band |= BAND_5G_H;
			else if(IS_5G_L_BAND(inf->Band))
				table->Band |= BAND_5G_L;
    	}

    	bndstrg_op_mode_update(bndstrg);
		
		/* aged entry when table on or off */
		{
			struct bndstrg_cli_entry *entry = NULL;
			int i;
			for (i = 0; i < table->max_steering_size; i++)
			{
				entry = &table->Entry[i];
				if (entry->bValid == TRUE) {
					bndstrg_check_entry_aged(bndstrg,
											  entry);
				}
			}
		}
    }
	return 0;
}

void bndstrg_update_white_black_list(struct bndstrg *bndstrg,struct bndstrg_iface *inf, unsigned char *macAddr, u8 list_type, int deladd)
{
	u8 i;
	if(inf)
		bndstrg_send_white_black_list(bndstrg, (char*)inf->ucIfName, macAddr, list_type, deladd);
	else{
		for(i = 0; i < bndstrg->ctrl_iface.Size; i++)
		{
	    	inf = &bndstrg->ctrl_iface.inf[i];
	    	if(inf->bValid && inf->bInfReady) {
	       		bndstrg_send_white_black_list(bndstrg, (char*)inf->ucIfName, macAddr, list_type, deladd);
	    	}
		}
	}
}

static void bndstrg_param (struct bndstrg *bndstrg, struct bndstrg_msg *msg)
{
	struct bndstrg_cli_table *table = &bndstrg->table;
    struct bnd_msg_param temp_bndstrg_param, *bndstrg_param = &msg->data.bndstrg_param;
	struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
	struct bndstrg_iface *inf = NULL;
	char *token = NULL;
	int i = 0, j = 0;
	int token_length = 0;
	int token_value = 0;
	
	inf = bndstrg_get_interface_by_channel(ctrl_iface,bndstrg_param->Channel);

	/* sanity check for command*/
	memcpy(&temp_bndstrg_param, &msg->data.bndstrg_param, sizeof(struct bnd_msg_param));
	if((strtok(temp_bndstrg_param.arg , ";") == NULL) || (strtok(NULL , ";") == NULL)){
		DBGPRINT(DEBUG_OFF,"Wrong command format \n");
		return;
	}

	token = strtok(bndstrg_param->arg , ";");
	token_length = strlen(token);
	for(i = 0; i < BND_SET_MAX_COMMAND ; i++) {
		if(strncmp(token , bndstrg_commands[i].name , token_length) == 0) {
			break;
		}
	}
	if(i == BND_SET_MAX_COMMAND)
		return;

	switch (bndstrg_commands[i].id) {
	case BND_SET_CND_PRIORITY:
	{
		u8 bDefault_set = FALSE;
		memset(table->PriorityList, 0, sizeof(table->PriorityList));
		j = 0;
		while((token = strtok(NULL, ";")) != NULL) {
			token_value = strtol(token, NULL, 10);
			if(token_value < fBND_STRG_PRIORITY_MAX){
				table->PriorityList[j] = token_value ;
				j++;
		    }
		}
		table->PriorityListSize = j;
		table->chanload_priority_enabled = FALSE;
		for(i=0; i < table->PriorityListSize; i++)
		{
			if(table->PriorityList[i] < fBND_STRG_PRIORITY_LB_CND_MCS){
				table->BndStrgMode |= PRE_CONNECTION_STEERING;
			} else if((table->PriorityList[i] == fBND_STRG_PRIORITY_DEFAULT_5G) ||
			(table->PriorityList[i] == fBND_STRG_PRIORITY_DEFAULT_2G)){
				bDefault_set = TRUE;
			} else
				table->BndStrgMode |= POST_CONNECTION_STEERING;

			if ((table->PriorityList[i] == fBND_STRG_PRIORITY_LB_CND_CHANLOAD_STATIC) ||
				 (table->PriorityList[i] == fBND_STRG_PRIORITY_LB_CND_CHANLOAD_IDLE) ||
				 (table->PriorityList[i] == fBND_STRG_PRIORITY_LB_CND_CHANLOAD_ACTIVE)
#ifdef VENDOR_FEATURE7_SUPPORT
				||(table->PriorityList[i] == fBND_STRG_PRIORITY_DOWNSTEER_CHLOAD_RSSI)
				||(table->PriorityList[i] == fBND_STRG_PRIORITY_UPSTEER_CHLOAD_RSSI)
#endif
			){
				table->chanload_priority_enabled = TRUE;
			}
		}
		if((table->BndStrgMode & PRE_CONNECTION_STEERING) && (bDefault_set == FALSE)){
			table->PriorityList[table->PriorityListSize] = fBND_STRG_PRIORITY_DEFAULT_5G;
			table->PriorityListSize ++;
		}
	}
	break;

	case BND_SET_DWELL_TIME:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		if (token_value > 0)
			table->dwell_time = token_value;
	break;
	case BND_SET_STEER_TIME_WINDOW:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		if (token_value > 0)
			table->max_steer_time_window = token_value;
	break;
	case BND_SET_MAX_STEER_COUNT:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		if((token_value >= 1) && (token_value < MAX_STEERING_COUNT))
			table->max_steer_count = token_value;
		else
			table->max_steer_count = MAX_STEERING_COUNT;
	break;
	case BND_SET_AGETIME:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		table->AgeTime = token_value;
	break;
	case BND_SET_CHECKTIME:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		table->CheckTime= token_value;
	break;
	case BND_SET_HOLDTIME:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		table->HoldTime= token_value;
	break;
	case BND_SET_RSSI_CHECK_COUNT:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		table->RssiCheckCount = token_value;
	break;
	case BND_SET_IDLE_RX_BYTE_COUNT:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		table->IDLE_RX_Byte_Count= token_value;
	break;
	case BND_SET_IDLE_TX_BYTE_COUNT:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		table->IDLE_TX_Byte_Count= token_value;
	break;
	case BND_SET_RSSILOW:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		table->RssiLow = token_value;
	break;
	case BND_SET_RSSI_DOWNSTEER:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		table->RSSILowDownSteer = token_value;
	break;
	case BND_SET_RSSI_UPSTEER:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		table->RSSIHighUpSteer = token_value;
	break;
	case BND_SET_RSSIDIFF:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		table->RssiDiff= token_value;
	break;
	case BND_SET_STEERING_NUM:
	{
		int i;
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		if (token_value > BND_STRG_MAX_TABLE_SIZE)
			return;
		DBGPRINT(DEBUG_ERROR,GRN("%s()::steering num=%d\n"), 
				 __FUNCTION__, token_value);
		/*remove all daemon entry and disable dirver band steering*/
		for (i=0;i<table->max_steering_size;i++) {
			bndstrg_delete_entry(table, NULL, i);
		}
		table->active_client_2G=0;
		table->active_client_5G=0;
		table->active_client_L5G=0;
		table->active_client_H5G=0;
		bndstrg_table_en_polling(table->bndstrg,table,0);
		table->max_steering_size = token_value;
		break;
	}

	case BND_SET_ASSOC_TH:
	{
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		DBGPRINT(DEBUG_ERROR,GRN("%s()::assoc balance threshold=%d\n"), 
				 __FUNCTION__, token_value);
		if(token_value <= 100)
			table->assoc_bl_th = token_value;
		break;
	}

	case BND_SET_LOAD_TH:
	{
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		DBGPRINT(DEBUG_ERROR,GRN("%s()::CHANLOAD threshold=%d\n"), 
				 __FUNCTION__, token_value);
		if(inf)
			if(token_value > 0 && token_value < 100)
				inf->chanload_thr = token_value;
		break;
	}

	case BND_SET_MIN_RSSI_TH:
	{
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		DBGPRINT(DEBUG_ERROR,GRN("%s()::MIN RSSI threshold=%d\n"), 
				 __FUNCTION__, token_value);
		if(inf)
			if(token_value < 0 && token_value > -100)
				inf->min_rssi_thr = token_value;
		break;
	}

	case BND_SET_NSS_TH:
	{
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		DBGPRINT(DEBUG_ERROR,GRN("%s()::NSS threshold = %d\n"), 
				 __FUNCTION__, token_value);
		if(inf)
			if(token_value >= 1 && token_value <=4)
				inf->min_nss_thr = token_value;
		break;
	}

	case BND_SET_STA_POLL_PRD:
	{
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		DBGPRINT(DEBUG_ERROR,GRN("%s()::STA polling period=%d\n"), 
				 __FUNCTION__, token_value);
		table->polling_sta_period = token_value;
		if (token_value < 3) 
		{
			table->polling_sta_period = 3;
			DBGPRINT(DEBUG_OFF, RED("Debug:Polling Period = %d, min is 3\n"),token_value);
		}
		break;
	}

	case BND_SET_DAEMON_STATE:
	{
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		DBGPRINT(DEBUG_ERROR,GRN("%s()::Deamon state=%d\n"), 
		__FUNCTION__, token_value);
		if(token_value <= 5)
			table->bndstrg->state = token_value;
		break;
	}

	case BND_SET_ADD_WHITE_LIST:
	{
		int i = 0, j = 0;
		char	*value;
		u8		macAddr[MAC_ADDR_LEN];
		s8 		emptyindex = -1;
		u8		macAddrzero[MAC_ADDR_LEN] = { 0 };
		struct bndstrg_cli_entry *entry = NULL;

		if(table->WhiteEntryListSize >= BND_STRG_MAX_DISABLE_LIST)
			break;
		token = strtok(NULL, ";");
		if (strlen(token) != 17) /*Mac address acceptable format 01:02:03:04:05:06 length 17 */
			break;
		for (i = 0, value = strtok(token, ":"); value; value = strtok(NULL, ":")) {
			if ((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))))
				break;  /*Invalid */
			AtoH(value, (char *)&macAddr[i++], 1);
		}
		if (i != MAC_ADDR_LEN)
		{
			break;
		}
		for (j = 0; j < BND_STRG_MAX_DISABLE_LIST; j++)
		{
			if (memcmp(table->WhiteEntryList[j], macAddr, MAC_ADDR_LEN) == 0)
				return;

			if ((emptyindex == -1) && (memcmp(table->WhiteEntryList[j], macAddrzero, MAC_ADDR_LEN) == 0))
				emptyindex = j;
		}

		if(emptyindex != -1){
			memcpy(table->WhiteEntryList[emptyindex], macAddr, MAC_ADDR_LEN);
			table->WhiteEntryListSize++;
			DBGPRINT(DEBUG_TRACE, "Add [%02x:%02x:%02x:%02x:%02x:%02x]to WhiteList \n",PRINT_MAC(macAddr));
			bndstrg_update_white_black_list(bndstrg, NULL, macAddr, bndstrg_whitelist, 1);
			entry = bndstrg_table_lookup(table, table->WhiteEntryList[emptyindex]);
			if(entry) {
				inf = bndstrg_get_interface(ctrl_iface, NULL, entry->band, TRUE);
				if(inf){
#ifndef ROAMING_SUPPORT
					bndstrg_accessible_cli(bndstrg, inf, entry, CLI_DEL);
					bndstrg_delete_entry(table, entry->Addr, entry->TableIndex);
#endif /*ROAMING_SUPPORT*/
				}
			}else{
				DBGPRINT(DEBUG_WARN, "Entry [%02x:%02x:%02x:%02x:%02x:%02x] not found \n",PRINT_MAC(macAddr));
			}
		}else{
			DBGPRINT(DEBUG_WARN, "White list table full \n");
		}
	}
	break;
	case BND_SET_REMOVE_WHITE_LIST:
	{
		int i = 0, j = 0;
		char	*value;
		u8		macAddr[MAC_ADDR_LEN];
		u8		macAddrzero[MAC_ADDR_LEN] = { 0 };

		token = strtok(NULL, ";");
		if (strlen(token) != 17) /*Mac address acceptable format 01:02:03:04:05:06 length 17 */
			break;
		for (i = 0, value = strtok(token, ":"); value; value = strtok(NULL, ":")) {
			if ((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))))
				break;  /*Invalid */
			AtoH(value, (char *)&macAddr[i++], 1);
		}
		if (i != MAC_ADDR_LEN)
		{
			break;
		}
		for (j = 0; j < BND_STRG_MAX_DISABLE_LIST; j++)
		{
			if (memcmp(table->WhiteEntryList[j], macAddr, MAC_ADDR_LEN) == 0)
			{
				memcpy(table->WhiteEntryList[j], macAddrzero, MAC_ADDR_LEN);
				table->WhiteEntryListSize--;
				DBGPRINT(DEBUG_TRACE, "Remove [%02x:%02x:%02x:%02x:%02x:%02x] from WhiteList \n",PRINT_MAC(macAddr));
				bndstrg_update_white_black_list(bndstrg, NULL, macAddr, bndstrg_whitelist, 0);
			}
		}
	}
	break;

#ifdef BTM_DEBUG_SUPPORT /* For BTM manual steering debug only */
	case BND_SEND_BTM:
		{
			int i = 0;
			u16 HashIdx=0;
			char	*value;
			u8		macAddr[MAC_ADDR_LEN];
			//u8		macAddrzero[MAC_ADDR_LEN] = { 0 };
			struct bndstrg_cli_entry *entry =NULL;
			
			token = strtok(NULL, ";");
			if (strlen(token) != 17) /*Mac address acceptable format 01:02:03:04:05:06 length 17 */
				break;
			for (i = 0, value = strtok(token, ":"); value; value = strtok(NULL, ":")) {
				if ((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))))
					break;  /*Invalid */
				AtoH(value, (char *)&macAddr[i++], 1);
			}
			if (i != MAC_ADDR_LEN)
			{
				break;
			}
			HashIdx = MAC_ADDR_HASH_INDEX(macAddr);
			entry = table->Hash[HashIdx];
			while (entry) {
				if (MAC_ADDR_EQUAL(macAddr, entry->Addr)) {
					/* this is the entry we're looking for */
					break;
				} else {
					entry = entry->pNext;
				}
			}
			if(entry){
				entry->Operation_steered = 1;
				if(IS_2G_BAND(entry->connected_band))
					bndstrg_client_band_update(bndstrg,entry,BAND_5G_L);
				else if(IS_5G_H_BAND(entry->connected_band))
					bndstrg_client_band_update(bndstrg,entry,BAND_5G_L);
				else if(IS_5G_L_BAND(entry->connected_band)){
					if(IS_2G_BAND(table->Band))
						bndstrg_client_band_update(bndstrg,entry,BAND_2G);
					else
						bndstrg_client_band_update(bndstrg,entry,BAND_5G_H);
				}	
			}
		}
			
	break;
#endif

#ifdef BNDSTRG_NVRAM_SUPPORT
case BND_SET_NVRAM:
	token = strtok(NULL, ";");
	token_value = strtol(token, NULL, 10);
	if(token_value == 1)
		bndstrg_nvram_print_all_entry(table);
	else if(token_value == 0)
		bndstrg_nvram_clear_all_entry(table);
break;
#endif

#ifdef	VENDOR_FEATURE7_SUPPORT
	case BND_SET_RSSI_DISCONNECT:
		token = strtok(NULL, ";");
		token_value = strtol(token, NULL, 10);
		if(token_value < 0)
			table->RSSIDisconnect = token_value;
	break;
#endif

	default:
		DBGPRINT(DEBUG_ERROR,RED("%s()::invalid command=%s\n"), 
				 __FUNCTION__, token);
		break;
	}
	return;
}

int bndstrg_wnm_event_btm_query (struct bndstrg *bndstrg, struct wnm_event *wnm_event_data)
{
#if 0
	struct bndstrg_iface *inf;
	struct bndstrg_cli_entry *entry;
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct btm_query_data *query_data = (struct btm_query_data *)wnm_event_data->event_body;

	
	entry = bndstrg_table_lookup(table, query_data->peer_mac_addr);
	if(!entry)
		return 0;
	inf = bndstrg_get_interface_by_channel(&bndstrg->ctrl_iface,entry->Channel);
	bndstrg_wnm_send_btm_req(bndstrg, inf, entry);
#endif
	hex_dump("btm_query", wnm_event_data->event_body, wnm_event_data->event_len);
	return 0;
}

int bndstrg_wnm_event_btm_rsp (struct bndstrg *bndstrg, struct wnm_event *wnm_event_data)
{
	struct bndstrg_cli_entry *entry;
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct btm_rsp_data *rsp_data = (struct btm_rsp_data *)wnm_event_data->event_body;
	struct btm_rsp_frame *rsp_frame = NULL;
	struct bndstrg_iface *inf = NULL;
	entry = bndstrg_table_lookup(table, (unsigned char*)rsp_data->peer_mac_addr);
	if(!entry)
		return 0;

	rsp_frame = (struct btm_rsp_frame *)rsp_data->btm_rsp;
	entry->btm_info.BTMState = btm_rsp_rx;
	entry->btm_info.BTMRsp = rsp_frame->status_code;
	
#ifdef BND_STRG_QA
		BND_STRG_PRINTQAMSG(table, entry,
			("BTM RSP: Req is %s \n"),(rsp_frame->status_code == 0 ? "Accepted":"Rejected"));
#endif /* BND_STRG_QA */

	if(entry->btm_info.BTMRsp == 0)
	{
		/*BTM accepted clear source band info from entry*/
		if(entry->connected_band != BAND_INVALID){
		switch (entry->btm_info.BTMSourceBand){
			case BAND_2G:
				entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G);
				table->active_client_2G --;
			break;
			case BAND_5G_L:
				entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G);
				table->active_client_L5G --;
				table->active_client_5G --;
			break;
			case BAND_5G_H:
				entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G);
				table->active_client_H5G --;
				table->active_client_5G --;
			break;
		}
		}
		entry->btm_info.BTMState = btm_init;
		inf = bndstrg_get_interface(&bndstrg->ctrl_iface, NULL, entry->btm_info.BTMSourceBand, TRUE);
		if(inf) bndstrg_accessible_cli(bndstrg, inf, entry, CLI_DEL);
		entry->btm_info.BTMSourceBand = BAND_INVALID;
		entry->connected_band = BAND_INVALID;
	} 
#ifdef VENDOR_FEATURE7_SUPPORT
	else {
		bndstrg_update_white_black_list(bndstrg, NULL, entry->Addr, bndstrg_blacklist, 1);
		entry->BlackListTimer = table->BlackListTime;
		inf = bndstrg_get_interface(&bndstrg->ctrl_iface, NULL, entry->connected_band, TRUE);
		if(inf) bndstrg_accessible_cli(bndstrg, inf, entry, CLI_DEL);
	}
#endif
	hex_dump("BTMRsp", wnm_event_data->event_body, wnm_event_data->event_len);
	return 0;

}

int bndstrg_wnm_event_handle(struct bndstrg *bndstrg, char *data)
{
	struct wnm_event *wnm_event_data = (struct wnm_event *)data;

	switch (wnm_event_data->event_id)
	{
		case OID_802_11_WNM_EVT_BTM_QUERY:
			DBGPRINT(DEBUG_TRACE,"EVT_BTM_QUERY\n");
			bndstrg_wnm_event_btm_query(bndstrg, wnm_event_data);			
			break;
			
		case OID_802_11_WNM_EVT_BTM_RSP:
			
			DBGPRINT(DEBUG_TRACE,"EVT_BTM_RSP\n");
			bndstrg_wnm_event_btm_rsp(bndstrg, wnm_event_data); 		
			break;
			
		default:
			BND_STRG_DBGPRINT(DEBUG_WARN,
				"Unkown event. (%u)\n",
				wnm_event_data->event_id);
			break;
	}
	
	return 0;
}


int bndstrg_event_handle(struct bndstrg *bndstrg, char *data)
{
	struct bndstrg_msg msg;
	struct bndstrg_cli_table *table = &bndstrg->table;
    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
	struct bndstrg_iface *inf = NULL;

	memcpy(&msg, data, sizeof(struct bndstrg_msg));

	switch (msg.Action)
	{
		case CLI_EVENT:
			if (bndstrg->state == BNDSTRG_TBL_READY)
			bndstrg_cli_event_req(bndstrg, &msg);
			break;
			
		case INF_STATUS_RSP:
			if (bndstrg->state != BNDSTRG_FROZEN)
            {
				struct bnd_msg_inf_status_rsp *inf_status_rsp = &msg.data.inf_status_rsp;;
				bndstrg_ctrl_interface_update(ctrl_iface,table,inf_status_rsp);
            }
			break;

		case TABLE_INFO:
			_bndstrg_event_table_info(bndstrg);
			break;
	
		case ENTRY_LIST:
			_bndstrg_event_show_entries(bndstrg,&msg);
			break;

		case BNDSTRG_ONOFF:
			if (bndstrg->state != BNDSTRG_FROZEN)
            {
                struct bnd_msg_onoff *onoff = &msg.data.onoff;
			    _bndstrg_event_on_off(bndstrg, onoff->OnOff, onoff->Band, onoff->Channel, (char*)onoff->ucIfName);
				if (!onoff->OnOff){
					if(table->fStopBndstrg)
						bndstrg->state = BNDSTRG_LEAVE;
					else
						bndstrg->state = BNDSTRG_INF_POLL;
					}
            }
			break;

		case CLI_STATUS_RSP:
			if (bndstrg->state != BNDSTRG_FROZEN)
			{
				struct bnd_msg_cli_status_rsp *cli_status_rsp = &msg.data.cli_status_rsp;
				if (cli_status_rsp->ReturnCode == BND_STRG_SUCCESS)
				{
					bndstrg_cli_status_rsp(bndstrg, cli_status_rsp);
				} else{
					BND_STRG_DBGPRINT(DEBUG_OFF,"Got cli status rsp. return code = %u\n\r", cli_status_rsp->ReturnCode);
				}		
			}
			break;

		case CHANLOAD_STATUS_RSP:
			if (bndstrg->state != BNDSTRG_FROZEN)
            {
                struct bnd_msg_chanload_status_rsp *chanload_status_rsp = &msg.data.chanload_status_rsp;
				/* update chanload information */
				if (chanload_status_rsp->ReturnCode == BND_STRG_SUCCESS) 
				{
					inf = bndstrg_get_interface_by_channel(ctrl_iface,chanload_status_rsp->Channel);
					if(inf){
						inf->chanload = chanload_status_rsp->chanload;
						inf->avg_chanload.avg -= inf->avg_chanload.data[inf->avg_chanload.idx];
						inf->avg_chanload.avg += inf->chanload;
						inf->avg_chanload.data[inf->avg_chanload.idx] = inf->chanload;
						INC(inf->avg_chanload.idx,BND_STRG_DATA_SAMPLE);
					}
					}
					BND_STRG_DBGPRINT(DEBUG_TRACE,
									  "band=%d,channel=%d,(%d,%d,%d)\n",
										chanload_status_rsp->band,
										chanload_status_rsp->Channel,
										inf->obss_load,
										inf->edcca_load,
										inf->myair_load);
            }
			break;

#ifdef BND_STRG_DBG
		case SET_MNT_ADDR:
            {
                struct bnd_msg_mnt_addr *mnt_addr = &msg.data.mnt_addr;
			    memcpy(table->MonitorAddr, mnt_addr->Addr, MAC_ADDR_LEN);
            }
			break;
#endif

#ifdef BNDSTRG_NVRAM_SUPPORT
		case NVRAM_UPDATE:
			if (bndstrg->state != BNDSTRG_FROZEN)
            {
                struct bnd_msg_nvram_entry_update *entry_update = &msg.data.entry_update;
    			if(!(bndstrg_nvram_table_lookup(table, entry_update->Addr))) {
    				if(_bndstrg_nvram_insert_entry(bndstrg, &msg, entry_update->nvram_entry.Manipulable))
    				{
    					DBGPRINT(DEBUG_TRACE,("\n%s[%d]Client Added To NVRAM Table\n"),__func__,__LINE__);
    				}
    				else
    				{
    					DBGPRINT(DEBUG_TRACE,("\n%s[%d] NVRAM Table full\n"),__func__,__LINE__);
    				}
    			}	
            }
			break;
#endif

		case REJECT_EVENT:
			if (bndstrg->state != BNDSTRG_FROZEN) {
				/* driver was occupied by other daemon*/
				if (msg.data.reject_body.DaemonPid != getpid()) {
					BND_STRG_DBGPRINT(DEBUG_OFF,
									  "driver was occpuy by other daemon,pid=%d,%d\n\r",getpid(),msg.data.reject_body.DaemonPid);
					bndstrg->state = BNDSTRG_LEAVE;
				}
			}
			break;

		case BNDSTRG_PARAM:
			bndstrg_param(bndstrg, &msg);
			break;

		case BNDSTRG_NEIGHBOR_REPORT:
			{
				struct bnd_msg_neighbor_report *NeighborReport = &msg.data.NeighborReport;
				inf = bndstrg_get_interface_by_channel(ctrl_iface,NeighborReport->Channel);
				if(inf) 
					memcpy(&inf->NeighborRepInfo, &NeighborReport->NeighborRepInfo, sizeof(RRM_NEIGHBOR_REP_INFO));
				else
					BND_STRG_DBGPRINT(DEBUG_ERROR, "BNDSTRG_NEIGHBOR_REPORT: No inf with Channel:%d\n", NeighborReport->Channel);
			}
			break;

		default:
			BND_STRG_DBGPRINT(DEBUG_WARN,
						"Unkown event. (%u)\n",
						msg.Action);
			break;
	}

	return 0;
}


struct bndstrg_event_ops bndstrg_event_ops = {
	.event_handle = bndstrg_event_handle,
	.wnm_event_handle = bndstrg_wnm_event_handle,		
};

u8 bndstrg_chanload_heavy_update(struct bndstrg *bndstrg)
{
	u8 i;
	struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
	struct bndstrg_iface *inf = NULL;
	for(i=0;i<ctrl_iface->Size;i++){
		inf=&ctrl_iface->inf[i];
		if(inf->bValid){
			if (inf->chanload > inf->chanload_thr)
				(inf->chanload_heavy_cnt >= inf->chanload_heavy_max_cnt)?
									inf->chanload_heavy_cnt = inf->chanload_heavy_max_cnt: inf->chanload_heavy_cnt++;
	else
				(inf->chanload_heavy_cnt>0)?inf->chanload_heavy_cnt--:0; 
		}
	}	
	return TRUE;
}

void bndstrg_worst_sta_update(struct bndstrg *bndstrg, 
					struct bndstrg_cli_entry *entry, 
					u8 band,
					u8 stat_idx)
{
	struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
	struct bndstrg_iface *inf = NULL;
	struct bndstrg_cli_entry **worst_entry = NULL;
	u32	tx_bad=0;
	u32	rx_bad=0;
	inf= bndstrg_get_interface(ctrl_iface, NULL, band, TRUE);
	worst_entry = &inf->worst_entry;
	if(inf->chanload_heavy_cnt >= inf->chanload_heavy_max_cnt){	
		if (entry->statistics[stat_idx].data_tx_util.avg/(BND_STRG_DATA_SAMPLE) >= LOW_CHANNEL_UTIL) {
			if (entry->statistics[stat_idx].data_tx_mcs <= LOW_MCS_IDX) {
				tx_bad = 1;
			} else {
				tx_bad = 0;
			}
		} else {
			tx_bad = 0;
		}
		/* decide bad rx sta */
		if (entry->statistics[stat_idx].data_rx_util.avg/(BND_STRG_DATA_SAMPLE) >= LOW_CHANNEL_UTIL) {
			if (entry->statistics[stat_idx].data_rx_mcs <= LOW_MCS_IDX) {
				rx_bad = 1;
			} else {
				rx_bad = 0;
			} 
		} else {
			rx_bad = 0;
		}
	} else {
		tx_bad = 0;
		rx_bad = 0;
		*worst_entry = NULL;
	}
	if(tx_bad) {
		if (*worst_entry == NULL)
			*worst_entry = entry;
		else {
			if ((*worst_entry)->statistics[stat_idx].data_tx_util.avg < 
				entry->statistics[stat_idx].data_tx_util.avg)
				*worst_entry = entry;
		}
		entry->Isbad = TRUE;
	}
	else if(rx_bad){
		if (*worst_entry == NULL)
			*worst_entry = entry;
		else {
			if ((*worst_entry)->statistics[stat_idx].data_rx_util.avg < 
				entry->statistics[stat_idx].data_rx_util.avg)
				*worst_entry = entry;
		}
		entry->Isbad = TRUE;
	}else
		entry->Isbad = FALSE;
	return;
}

u8 bndstrg_sta_update(struct bndstrg *bndstrg)
{
	u32	i;
	u32 count=0;
	struct bndstrg_cli_entry *entry = NULL;
	struct bndstrg_cli_table *table = &bndstrg->table;
	
	for (i = 0; i < table->max_steering_size; i++)
	{
		entry = &table->Entry[i];
		if ((entry->bValid == TRUE) && (entry->bConnStatus))
		{
		    count++;
			if (IS_BND_STRG_DUAL_BAND_CLIENT(entry->Control_Flags) ||
				IS_BND_STRG_H5G_L5G_BAND_CLIENT(entry->Control_Flags))
			{
				switch(entry->band) 
				{
					case BAND_2G:
						if (entry->statistics[IDX_2G].data_tx_byte.avg  <= table->IDLE_TX_Byte_Count &&
							entry->statistics[IDX_2G].data_rx_byte.avg  <= table->IDLE_RX_Byte_Count ) 
						{
							entry->IsIdle = TRUE;
						}else
							entry->IsIdle = FALSE;
						break;
					case BAND_5G_L:
						bndstrg_worst_sta_update(bndstrg, entry, BAND_5G_L, IDX_5GL);
						if ( entry->statistics[IDX_5GL].data_tx_byte.avg  <= table->IDLE_TX_Byte_Count &&
							 entry->statistics[IDX_5GL].data_rx_byte.avg  <= table->IDLE_RX_Byte_Count ) 
						{
							entry->IsIdle = TRUE;
						}else
							entry->IsIdle = FALSE;
						break;
					case BAND_5G_H:
						bndstrg_worst_sta_update(bndstrg, entry, BAND_5G_H, IDX_5GH);
						entry->IsIdle = FALSE;
						break;
				}
			}
		}
		if(count >= table->Size)
		{
			break;
		}
	}
	return TRUE;
}

u8 bndstrg_check_entry_aged(struct bndstrg *bndstrg,
									struct bndstrg_cli_entry *entry)
{
	u32 elapsed_time;
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
	struct bndstrg_iface *inf = NULL;

	elapsed_time = bndstrg_get_entry_elapsed_time(entry);

	if (table->op_mode == OP_TRIBAND) {
		/* for triband, it should wait L5G/H5G probe information ready */
		if (elapsed_time > 5) {
			entry->enable_compare_flag = TRUE;
        }
		else {
            if (IS_BND_STRG_DUAL_BAND_CLIENT(entry->Control_Flags) &&
				(entry->AssocProbeInfoMissMatch != TRUE)) {
    			if ((entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_L5G) &&
                    (entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G))
    				entry->enable_compare_flag = TRUE;
                else 
    				entry->enable_compare_flag = FALSE;
		    } 
        }
	} 
    else 
		entry->enable_compare_flag = TRUE;
	
	if(entry->Channel == 0 && elapsed_time >= table->DormantTime)
	{
		inf = bndstrg_get_interface(ctrl_iface, NULL, entry->band, TRUE);
		if(inf)
			bndstrg_accessible_cli(bndstrg, inf, entry, CLI_DEL);
#ifdef BND_STRG_QA
		BND_STRG_PRINTQAMSG(table, entry, RED("Delete entry (%02x:%02x:%02x:%02x:%02x:%02x) as elapsed time %u sec >= DormantTime:%d \n"),
			PRINT_MAC(entry->Addr), elapsed_time, table->DormantTime);
#endif
		bndstrg_delete_entry(table, entry->Addr, entry->TableIndex);
		return TRUE;
	}else if ((elapsed_time >= table->AgeTime) && (entry->Channel != 0) && (!entry->bConnStatus))
	{

		if((IS_BND_STRG_2G_ONLY_BAND_CLIENT(entry->Control_Flags) ||
			 IS_BND_STRG_L5G_ONLY_BAND_CLIENT(entry->Control_Flags) ||
			 IS_BND_STRG_H5G_ONLY_BAND_CLIENT(entry->Control_Flags)
#if 0		
			|| (entry->match_steered_rule_id[CMP_5GH_5GL] = fBND_STRG_PRIORITY_BAND_PERSIST)
			|| (entry->match_steered_rule_id[CMP_5GH_2G] = fBND_STRG_PRIORITY_BAND_PERSIST)
			|| (entry->match_steered_rule_id[CMP_5GL_2G] = fBND_STRG_PRIORITY_BAND_PERSIST)
#endif
			)&& (elapsed_time <= table->single_band_timeout)) {
				return TRUE;
			}

		inf = bndstrg_get_interface(ctrl_iface, NULL, entry->band, TRUE);
		if(inf)
			bndstrg_accessible_cli(bndstrg, inf, entry, CLI_DEL);
#ifdef BND_STRG_QA
		BND_STRG_PRINTQAMSG(table, entry, RED("Delete entry (%02x:%02x:%02x:%02x:%02x:%02x) as elapsed time %u sec >= AgeTime:%d \n"),
			PRINT_MAC(entry->Addr),elapsed_time, table->AgeTime);
#endif		
		if(table->BndStrgMode == POST_CONNECTION_STEERING)
			bndstrg_update_white_black_list(bndstrg, NULL, entry->Addr, bndstrg_blacklist, 0);
		bndstrg_delete_entry(table, entry->Addr, entry->TableIndex);
		return TRUE;
	}
	else
	{

		if(!entry->bConnStatus){
		/* Update elapsed time */
		entry->elapsed_time = elapsed_time;
		
			if (elapsed_time >= table->CheckTime &&
				!(entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_L5G) &&
				!(entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G) &&
				!(entry->Control_Flags & fBND_STRG_CLIENT_IS_2G_ONLY))
		{
			/* If we don't get any connection req from 5G for a long time,
			  * we condider this client is 2.4G only 
			  */
#ifdef BND_STRG_QA
			BND_STRG_PRINTQAMSG(table, entry, YLW("Receive no frame by 5G interface within %u seconds,"
				" set client (%02x:%02x:%02x:%02x:%02x:%02x) to 2.4G only.\n"),
				table->CheckTime, PRINT_MAC(entry->Addr));
#endif /* BND_STRG_QA */
			entry->Control_Flags |= \
									fBND_STRG_CLIENT_IS_2G_ONLY;

		}else if (elapsed_time >= table->CheckTime &&
				!(entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_2G) &&
				!(entry->Control_Flags & fBND_STRG_CLIENT_IS_5G_ONLY))
		{
			/*	If we don't get any connection req from 2G for a long time,
			  *   we condider this client is 5G only 
			  */
#ifdef BND_STRG_QA
			BND_STRG_PRINTQAMSG(table, entry,
								YLW("Receive no frame by 2G interface within %u seconds,"
									" set client (%02x:%02x:%02x:%02x:%02x:%02x) to 5G only.\n"),
									table->CheckTime, PRINT_MAC(entry->Addr));
#endif /* BND_STRG_QA */
			entry->Control_Flags |= \
									fBND_STRG_CLIENT_IS_5G_ONLY;
		}
		}
		return FALSE;
	}
}

u8 bndstrg_polling_connected_sta(struct bndstrg *bndstrg, u8 inf_num)
{
	if(inf_num < MAX_INF_NUM)
	{
		struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
		struct bndstrg_iface *inf = &ctrl_iface->inf[inf_num];
		if(inf->bValid && inf->bInfReady)
			bndstrg_accessible_cli(bndstrg, inf, NULL, CLI_STATUS_REQ);
	}
	return TRUE;
}

/*
 * compare_mode: 0=H-5G vs L-5G    prio,H > L
 * 				 1=H-5G vs 2.4G		prio,H > 2.4G
 * 				 2=L-5G vs 2.4G		prio,L > 2.4G
 * return: band: BAND_5G_L/BAND_5G_H/BAND_2G/0xff means fail
 */
u8 bndstrg_mtk_rule_iterate(
	struct bndstrg_cli_table *table,
	struct bndstrg_cli_entry *entry,
	struct bndstrg *bndstrg,
	int	compare_mode)
{
	struct bndstrg_entry_stat *statistics_1st_band = NULL, *statistics_2nd_band = NULL;
    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
    struct bndstrg_iface *inf_1st_band = NULL, *inf_2nd_band = NULL;
	u8 i, band = BAND_INVALID, done = FALSE; 
	u8 pre_band[2];
	u32 allow_to_connect_band_flag[2]={0,0};
	u32 low_rssi_flag[2];
	u32 not_support_ht_flag[2]={0,0};
	u32 band_assoc_percent[2]={0,0};
	u32 chanload_heavy_cnt[2]={0,0};
	s32 min_rssi[2]={0,0};
	u32 nss_thres[2]={0,0};

	u32 total_assoc_cnt = (table->active_client_2G+table->active_client_5G);
	if (!table)
	{
		DBGPRINT(DEBUG_ERROR,"%s(): Error! table is NULL!\n", __FUNCTION__);
		return BAND_INVALID;
	}
	if (!entry)
	{
		DBGPRINT(DEBUG_ERROR,RED("%s(): Error! entry is NULL!\n"), __FUNCTION__);
		return BAND_INVALID;
	}

	if (!IS_BND_STRG_DUAL_BAND_CLIENT(entry->Control_Flags))
	{
		if (IS_BND_STRG_H5G_L5G_BAND_CLIENT(entry->Control_Flags))
		{//H5G vs L5G can run compare rule
		//	DBGPRINT(DEBUG_ERROR,RED("%s(): Error! this sta doesn't support dual band!\n"), __FUNCTION__);
		} else {
		    DBGPRINT(DEBUG_ERROR,RED("%s(): Error! this sta doesn't support dual band!\n"), __FUNCTION__);
		    return BAND_INVALID;
		}
	}

	switch (compare_mode) {
		case CMP_5GH_5GL:
            inf_1st_band = bndstrg_get_interface(ctrl_iface, NULL, BAND_5G_H, TRUE);
            inf_2nd_band = bndstrg_get_interface(ctrl_iface, NULL, BAND_5G_L, TRUE);
			statistics_1st_band = &entry->statistics[IDX_5GH];
			statistics_2nd_band = &entry->statistics[IDX_5GL];
			pre_band[0]=BAND_5G_H;
			pre_band[1]=BAND_5G_L;
			allow_to_connect_band_flag[0]=fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G;
			allow_to_connect_band_flag[1]=fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G;
			low_rssi_flag[0]=fBND_STRG_CLIENT_LOW_RSSI_H5G;
			low_rssi_flag[1]=fBND_STRG_CLIENT_LOW_RSSI_L5G;
			not_support_ht_flag[0]=fBND_STRG_CLIENT_NOT_SUPPORT_HT_H5G;
			not_support_ht_flag[1]=fBND_STRG_CLIENT_NOT_SUPPORT_HT_L5G;
			if (total_assoc_cnt != 0) {
				band_assoc_percent[0]=table->active_client_H5G*100/total_assoc_cnt;
				band_assoc_percent[1]=table->active_client_L5G*100/total_assoc_cnt;
			}
			chanload_heavy_cnt[0] = inf_1st_band->chanload_heavy_cnt;
			chanload_heavy_cnt[1] = inf_2nd_band->chanload_heavy_cnt;
			min_rssi[0] = inf_1st_band->min_rssi_thr;
			min_rssi[1] = inf_2nd_band->min_rssi_thr;
			nss_thres[0] = inf_1st_band->min_nss_thr;
			nss_thres[1] = inf_2nd_band->min_nss_thr;
			
			break;
		case CMP_5GH_2G:
			inf_1st_band = bndstrg_get_interface(ctrl_iface, NULL, BAND_5G_H, TRUE);
            inf_2nd_band = bndstrg_get_interface(ctrl_iface, NULL, BAND_2G, TRUE);
			statistics_1st_band = &entry->statistics[IDX_5GH];
			statistics_2nd_band = &entry->statistics[IDX_2G];
			pre_band[0]=BAND_5G_H;
			pre_band[1]=BAND_2G;
			allow_to_connect_band_flag[0]=fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G;
			allow_to_connect_band_flag[1]=fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G;
			low_rssi_flag[0]=fBND_STRG_CLIENT_LOW_RSSI_H5G;
			low_rssi_flag[1]=fBND_STRG_CLIENT_LOW_RSSI_2G;
			not_support_ht_flag[0]=fBND_STRG_CLIENT_NOT_SUPPORT_HT_H5G;
			not_support_ht_flag[1]=fBND_STRG_CLIENT_NOT_SUPPORT_HT_2G;
			if (total_assoc_cnt != 0) {
				band_assoc_percent[0]=table->active_client_H5G*100/total_assoc_cnt;
				band_assoc_percent[1]=table->active_client_2G*100/total_assoc_cnt;
			}
			chanload_heavy_cnt[0] = inf_1st_band->chanload_heavy_cnt;
			chanload_heavy_cnt[1] = inf_2nd_band->chanload_heavy_cnt;
			min_rssi[0] = inf_1st_band->min_rssi_thr;
			min_rssi[1] = inf_2nd_band->min_rssi_thr;
			nss_thres[0] = inf_1st_band->min_nss_thr;
			nss_thres[1] = inf_2nd_band->min_nss_thr;
			break;
		case CMP_5GL_2G:
			inf_1st_band = bndstrg_get_interface(ctrl_iface, NULL, BAND_5G_L, TRUE);
            inf_2nd_band = bndstrg_get_interface(ctrl_iface, NULL, BAND_2G, TRUE);
			statistics_1st_band = &entry->statistics[IDX_5GL];
			statistics_2nd_band = &entry->statistics[IDX_2G];
			pre_band[0]=BAND_5G_L;
			pre_band[1]=BAND_2G;
			allow_to_connect_band_flag[0]=fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G;
			allow_to_connect_band_flag[1]=fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G;
			low_rssi_flag[0]=fBND_STRG_CLIENT_LOW_RSSI_L5G;
			low_rssi_flag[1]=fBND_STRG_CLIENT_LOW_RSSI_2G;
			not_support_ht_flag[0]=fBND_STRG_CLIENT_NOT_SUPPORT_HT_L5G;
			not_support_ht_flag[1]=fBND_STRG_CLIENT_NOT_SUPPORT_HT_2G;
			if (total_assoc_cnt != 0) {
				band_assoc_percent[0]=table->active_client_L5G*100/total_assoc_cnt;
				band_assoc_percent[1]=table->active_client_2G*100/total_assoc_cnt;
			}
			chanload_heavy_cnt[0] = inf_1st_band->chanload_heavy_cnt;
			chanload_heavy_cnt[1] = inf_2nd_band->chanload_heavy_cnt;
			min_rssi[0] = inf_1st_band->min_rssi_thr;
			min_rssi[1] = inf_2nd_band->min_rssi_thr;
			nss_thres[0] = inf_1st_band->min_nss_thr;
			nss_thres[1] = inf_2nd_band->min_nss_thr;
			break;
		default:
			return BAND_INVALID;
	}

	for(i=0; i < table->PriorityListSize; i++)
	{
		if (entry->bConnStatus) {
			if ((table->PriorityList[i] != fBND_STRG_PRIORITY_RSSI_DOWNSTEER) &&
			    (table->PriorityList[i] != fBND_STRG_PRIORITY_LB_CND_CHANLOAD_IDLE)	&&
			    (table->PriorityList[i] != fBND_STRG_PRIORITY_LB_CND_CHANLOAD_ACTIVE) &&
			    (table->PriorityList[i] != fBND_STRG_PRIORITY_LB_CND_MCS) &&
			    (table->PriorityList[i] != fBND_STRG_PRIORITY_RSSI_UPSTEER)
#ifdef VENDOR_IOS_SUPPORT
			    &&(table->PriorityList[i] != fBND_STRG_PRIORITY_IOS_STEER)
#endif 
#ifdef VENDOR_FEATURE7_SUPPORT
			    &&(table->PriorityList[i] != fBND_STRG_PRIORITY_DOWNSTEER_CHLOAD_RSSI)
			    &&(table->PriorityList[i] != fBND_STRG_PRIORITY_UPSTEER_CHLOAD_RSSI)
#endif
				)
				continue;
		}
		if ((entry->elapsed_time >= table->HoldTime) && (!entry->bConnStatus))
		{
			if(statistics_1st_band->AuthReqCount < statistics_2nd_band->AuthReqCount)
			{
				entry->Control_Flags |= allow_to_connect_band_flag[1];
				done = TRUE;
				band = pre_band[1];
			}
			else if(statistics_1st_band->AuthReqCount != 0)
			{
				entry->Control_Flags |= allow_to_connect_band_flag[0];
				done = TRUE;
				band = pre_band[0];
			}
			else
			{
				if((entry->match_steered_rule_id[compare_mode] != fBND_STRG_PRIORITY_BAND_PERSIST)){
					if(entry->band == pre_band[0])
					{
						entry->Control_Flags |= allow_to_connect_band_flag[1];
						done = TRUE;
						band = pre_band[1];
					}else if(entry->band == pre_band[1])
					{
						entry->Control_Flags |= allow_to_connect_band_flag[0];
						done = TRUE;
						band = pre_band[0];
					}
				}
				else
				{
					done = TRUE;
					band = entry->band;
				}
			}
			entry->match_steered_rule_id[compare_mode] = fBND_STRG_PRIORITY_BAND_PERSIST;
		}
		else {
		switch(table->PriorityList[i]){
				
			case fBND_STRG_PRIORITY_RSSI_DIFF:
				if (statistics_1st_band->Rssi & statistics_2nd_band->Rssi)
				{
					s8 RssiDiff = statistics_2nd_band->Rssi - statistics_1st_band->Rssi;
					if (RssiDiff >= table->RssiDiff)
					{//allow coonnect to 2nd band
						entry->Control_Flags |= allow_to_connect_band_flag[1];
						done = TRUE;
						band = pre_band[1];
						entry->Manipulable = TRUE;
					}
				}
				break;
#ifdef VENDOR_IOS_SUPPORT
				case fBND_STRG_PRIORITY_IOS_STEER:

				// For IOS entry, only steer one time.
				if (entry->bAllowStaConnectImmediately == FALSE)
					break;

				// IOS down steer,  when 5g band client  > IOSNumLimit || rssi is bad,  steer to 2.4g
				if ((entry->bConnStatus) && 
					((entry->band & pre_band[0]) == pre_band[0]) &&
					(entry->bAllowStaConnectImmediately) &&
					(statistics_1st_band->got_cli_status))
				{
					if (table->active_client_5G > table->IOSNumLimit || 
						(entry->low_rssi_bad_cnt >= RSSI_CHECK_COUNT))
					{
					 	DBGPRINT(DEBUG_OFF,RED("===hit iOS down steer [%02x:%02x:%02x:%02x:%02x:%02x] ===\n\n"), PRINT_MAC(entry->Addr));
						entry->Operation_steered = TRUE;
						entry->Control_Flags |= allow_to_connect_band_flag[1];
						done = TRUE;
						band = pre_band[1];
						entry->Manipulable = TRUE;
						entry->bAllowStaConnectImmediately = FALSE;
						//decision made
						break; 
					}else
						band = pre_band[0];
				}

				// IOS up steer, 
				/* 1. 5g band client < 5
				     2. RSSI is better than RSSIHighUpSteer.
				*/
				if ((entry->bConnStatus) && 
					((entry->band & pre_band[1]) == pre_band[1]) &&
					(entry->bAllowStaConnectImmediately) &&
					(statistics_2nd_band->got_cli_status))
				{
					if ((table->active_client_5G < table->IOSNumLimit) && 
						(entry->good_rssi_cnt >= RSSI_CHECK_COUNT))
					{
						DBGPRINT(DEBUG_OFF,RED("===hit iOS up steer [%02x:%02x:%02x:%02x:%02x:%02x] ===\n\n"), PRINT_MAC(entry->Addr));
						entry->Operation_steered = TRUE;
						entry->Control_Flags |= allow_to_connect_band_flag[0];
						done = TRUE;
						band = pre_band[0];
						entry->Manipulable = TRUE;
						entry->bAllowStaConnectImmediately = FALSE;
						//decision made
						break; 
					}else
						band = pre_band[1];
				}
				break;
#endif								
			case fBND_STRG_PRIORITY_RSSI_DOWNSTEER:
				if((entry->bConnStatus) &&
					((entry->band & pre_band[0]) == pre_band[0]) &&
					(statistics_1st_band->got_cli_status)) 
				{
					if (entry->low_rssi_bad_cnt >= table->RssiCheckCount)
					{
						entry->Operation_steered = TRUE;
						entry->Control_Flags |= allow_to_connect_band_flag[1];
						done = TRUE;
						band = pre_band[1];
						entry->Manipulable = TRUE;
					}else
						band = pre_band[0];
				}
				break;

			case fBND_STRG_PRIORITY_RSSI_UPSTEER:
				if((entry->bConnStatus) &&
					((entry->band & pre_band[1]) == pre_band[1]) &&
					(statistics_2nd_band->got_cli_status)) 
				{
					if (entry->good_rssi_cnt >= table->RssiCheckCount)
					{
						entry->Operation_steered = TRUE;
						entry->Control_Flags |= allow_to_connect_band_flag[0];
						done = TRUE;
						band = pre_band[0];
						entry->Manipulable = TRUE;
					}else
						band = pre_band[1];
				}
				break;

			case fBND_STRG_PRIORITY_HT_SUPPORT:
				if (entry->Control_Flags & not_support_ht_flag[0])
				{
					done = TRUE;
					band = pre_band[1];
					entry->Control_Flags |= allow_to_connect_band_flag[1];
					entry->Manipulable = TRUE;
				}
				break;
				
			case fBND_STRG_PRIORITY_5G_RSSI:
				if(entry->Control_Flags & low_rssi_flag[0])
				{
					entry->Control_Flags |= allow_to_connect_band_flag[1];
					done = TRUE;
					band = pre_band[1];
					entry->Manipulable = TRUE;
				}
				break;

            case fBND_STRG_PRIORITY_VHT_SUPPORT:
				{
                    if(entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_VHT)
    				{
    				    if(inf_1st_band && inf_1st_band->bVHTCapable) 
                        {
                            entry->Control_Flags |= allow_to_connect_band_flag[0];
        					done = TRUE;
        					band = pre_band[0];
        					entry->Manipulable = TRUE;
                        }
                        else if(inf_2nd_band && inf_2nd_band->bVHTCapable) 
                        {
                            entry->Control_Flags |= allow_to_connect_band_flag[1];
        					done = TRUE;
        					band = pre_band[1];
        					entry->Manipulable = TRUE;
                        }
    				}
                }
                break;
                
			case fBND_STRG_PRIORITY_NSS_SUPPORT:				
				/* high band prefer if STA's NSS >= nss_thres */
				{
					if ((nss_thres[0]!= 0) && (entry->Nss >= nss_thres[0]))
					{
						if (!(entry->Control_Flags & not_support_ht_flag[0]))
						{
							/* HT support*/
							entry->Control_Flags |= allow_to_connect_band_flag[0];
							done = TRUE;
							band = pre_band[0];
							entry->Manipulable = TRUE;
						}
					}
				}
				break;
			case fBND_STRG_PRIORITY_LB_CND_STA_CONNECTED_COUNT:
				/*sta connected count, it will balance the connected sta for each band*/
				{
					u32		assoc_bl_delta=0;
					if (band_assoc_percent[0] > band_assoc_percent[1]) 
					{
						assoc_bl_delta = band_assoc_percent[0] - band_assoc_percent[1];
						if (assoc_bl_delta >= table->assoc_bl_th)
						{
							entry->Control_Flags |= allow_to_connect_band_flag[1];
							done = TRUE;
							band = pre_band[1];
							entry->Manipulable = TRUE;
						}
					} 
					else 
					{
						assoc_bl_delta = band_assoc_percent[1] - band_assoc_percent[0];
						if (assoc_bl_delta >= table->assoc_bl_th)
						{
							entry->Control_Flags |= allow_to_connect_band_flag[0];
							done = TRUE;
							band = pre_band[0];
							entry->Manipulable = TRUE;
						}
					}
					
				}
				break;

			case fBND_STRG_PRIORITY_LB_CND_CHANLOAD_STATIC:
				if((chanload_heavy_cnt[0]  >= inf_1st_band->chanload_heavy_max_cnt) && 
					(chanload_heavy_cnt[1]  < inf_2nd_band->chanload_heavy_max_cnt))
				{
					entry->Control_Flags |= allow_to_connect_band_flag[1];
					done = TRUE;
					band = pre_band[1];
					entry->Manipulable = TRUE;
					//entry->match_lb_chanload_cond_id[compare_mode] = fBND_STRG_CHANLOAD_STATIC; 
				}
				break;
				
			case fBND_STRG_PRIORITY_LB_CND_CHANLOAD_IDLE:
					if((entry->bConnStatus) &&
						(chanload_heavy_cnt[0] <= inf_1st_band->chanload_heavy_max_cnt) &&
						(entry->band & pre_band[1]) &&
						(entry->IsIdle) && 
						(!(entry->Control_Flags & (fBND_STRG_CLIENT_NOT_SUPPORT_HT_H5G | fBND_STRG_CLIENT_NOT_SUPPORT_HT_L5G))) 
						&& (statistics_2nd_band->data_Rssi >= inf_1st_band->min_rssi_thr)) 
					{
						entry->Operation_steered = TRUE;
						entry->Control_Flags |= allow_to_connect_band_flag[0];
						done = TRUE;
						band = pre_band[0];
						entry->Manipulable = TRUE;
						//entry->match_lb_chanload_cond_id[compare_mode] = fBND_STRG_CHANLOAD_IDLE_CLI;
					}

				break;

			case fBND_STRG_PRIORITY_LB_CND_CHANLOAD_ACTIVE:
					/* if sta is in heavy band, steer it to low priority band */
					if((entry->bConnStatus) &&
						(chanload_heavy_cnt[0] >= inf_1st_band->chanload_heavy_max_cnt) &&
						(chanload_heavy_cnt[1] < inf_1st_band->chanload_heavy_max_cnt) &&
						(entry->band & pre_band[0]) &&
						(entry->Isbad)) 
					{
						entry->Operation_steered = TRUE;
						entry->Control_Flags |= allow_to_connect_band_flag[1];
						done = TRUE;
						band = pre_band[1];
						entry->Manipulable = TRUE;
						entry->match_lb_chanload_active_cond_id[compare_mode] = fBND_STRG_CHANLOAD_ACTIVE_DOWN_STEER;
					}

					if((entry->bConnStatus) &&
						(chanload_heavy_cnt[1] >= inf_2nd_band->chanload_heavy_max_cnt) &&
						((entry->band & pre_band[1]) == pre_band[1])) 
					{
						if(statistics_2nd_band->data_tx_util.avg/BND_STRG_DATA_SAMPLE  >= LOW_CHANNEL_UTIL || 
						   statistics_2nd_band->data_rx_util.avg/BND_STRG_DATA_SAMPLE  >= LOW_CHANNEL_UTIL)
						{
							if((inf_1st_band->avg_chanload.avg/BND_STRG_DATA_SAMPLE) <= inf_1st_band->chanload_thr/2 &&
							   statistics_2nd_band->data_Rssi >= inf_1st_band->min_rssi_thr &&
							   !(entry->Control_Flags & (fBND_STRG_CLIENT_NOT_SUPPORT_HT_H5G | fBND_STRG_CLIENT_NOT_SUPPORT_HT_L5G)))
							{
								entry->Operation_steered = TRUE;
								entry->Control_Flags |= allow_to_connect_band_flag[0];
								done = TRUE;
								band = pre_band[0];
								entry->Manipulable = TRUE;
								entry->match_lb_chanload_active_cond_id[compare_mode] = fBND_STRG_CHANLOAD_ACTIVE_UP_STEER;
							}
						}
					}
				break;

			case fBND_STRG_PRIORITY_LB_CND_RSSI:
				{
					/*
					 * use default rssi to classify the sta to prefer band
					 * RSSI >= -45 : H5G
					 * RSSI >= -55 : L5G
					 * RSSI >= -65 : 2G
					 */
					if (statistics_1st_band->Rssi >= min_rssi[0]) 
					{
						entry->Control_Flags |= allow_to_connect_band_flag[0];
						done = TRUE;
						band = pre_band[0];
						entry->Manipulable = TRUE;
					} 
					else if (statistics_2nd_band->Rssi >= min_rssi[1]) 
					{
						entry->Control_Flags |= allow_to_connect_band_flag[1];
						done = TRUE;
						band = pre_band[1];
						entry->Manipulable = TRUE;
					}
				}
				break;
			case fBND_STRG_PRIORITY_LB_CND_MCS: 
				if ((entry->bConnStatus) && (statistics_1st_band->got_cli_status)) 
				{
					switch(entry->band) {
					case BAND_5G_L:
					case BAND_5G_H:
						if (entry->tx_mcs_bad_cnt >= MCS_BAD_COUNT) 
						{
							/* Tx MCS based*/
							entry->Operation_steered = TRUE;
							entry->Control_Flags |= allow_to_connect_band_flag[1];
							done = TRUE;
							band = pre_band[1];
							entry->Manipulable = TRUE;
						}else if ((entry->rx_mcs_bad_cnt >= MCS_BAD_COUNT) &&
							(statistics_1st_band->data_Rssi < (table->RSSILowDownSteer + 5)))
						{
							/*
							 * for rx MCS, steer it if rx MCS is too low 
							 * and RSSI is below RssiLow+5
							 */
							entry->Operation_steered = TRUE;
							entry->Control_Flags |= allow_to_connect_band_flag[1];
							done = TRUE;
							band = pre_band[1];
							entry->Manipulable = TRUE;
						}else
							band = pre_band[0];
							
						break;
					default:
						DBGPRINT(DEBUG_ERROR,"%s(%d): debug!\n", __FUNCTION__,__LINE__);
						break;
					}
				} 
				break;

#ifdef VENDOR_FEATURE7_SUPPORT
			case fBND_STRG_PRIORITY_DOWNSTEER_CHLOAD_RSSI:
				if((entry->bConnStatus) &&
					((entry->band & pre_band[0]) == pre_band[0]) &&
					(statistics_1st_band->got_cli_status)){

					if ((entry->low_rssi_bad_cnt >= table->RssiCheckCount) && 
						(chanload_heavy_cnt[1] == 0))
					{
						entry->Operation_steered = TRUE;
						entry->Control_Flags |= allow_to_connect_band_flag[1];
						done = TRUE;
						band = pre_band[1];
						entry->Manipulable = TRUE;
					}else
						band = pre_band[0];
				}
				break;

			case fBND_STRG_PRIORITY_UPSTEER_CHLOAD_RSSI:
				if((entry->bConnStatus) &&
					((entry->band & pre_band[1]) == pre_band[1]) &&
					(statistics_2nd_band->got_cli_status)){

					if ((entry->good_rssi_cnt >= table->RssiCheckCount) && 
						(chanload_heavy_cnt[0] == 0)
#ifdef VENDOR_FEATURE8_SUPPORT
						&& (entry->IsIdle)
#endif
						)
					{
						entry->Operation_steered = TRUE;
						entry->Control_Flags |= allow_to_connect_band_flag[0];
						done = TRUE;
						band = pre_band[0];
						entry->Manipulable = TRUE;
					}else
						band = pre_band[1];
				}
				
				break;
#endif

			case fBND_STRG_PRIORITY_DEFAULT_2G:
					entry->Control_Flags |= allow_to_connect_band_flag[1];
					done = TRUE;
					band = pre_band[1];
					entry->Manipulable = TRUE;
			break;
				
			case fBND_STRG_PRIORITY_DEFAULT_5G:
					entry->Control_Flags |= allow_to_connect_band_flag[0];
					done = TRUE;
					band = pre_band[0];
					entry->Manipulable = TRUE;
			break;

		}		
		}

		if(done)  // break for loop if decision has made.
		{
			if(entry->match_steered_rule_id[compare_mode] != fBND_STRG_PRIORITY_BAND_PERSIST)
				entry->match_steered_rule_id[compare_mode] = table->PriorityList[i];
			if ((entry->match_steered_rule_id[compare_mode] != fBND_STRG_PRIORITY_DEFAULT_2G) &&
					(entry->match_steered_rule_id[compare_mode] != fBND_STRG_PRIORITY_DEFAULT_5G)) 
			{
					entry->steered ++;
			}
			break;
		}
	}
	return band;
}

u8 bndstrg_client_band_update(
	struct bndstrg *bndstrg,
	struct bndstrg_cli_entry *entry,
	u8 band)

{
    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
    struct bndstrg_iface *inf_source = NULL, *inf_target = NULL;
	struct bndstrg_cli_table *table = &bndstrg->table;

	/* Check for is re-steering triggered for entry */
	if(entry->Control_Flags & fBND_STRG_CLIENT_RE_STEERING){
		inf_source = bndstrg_get_interface(ctrl_iface, NULL, entry->connected_band, TRUE);
		if(band == entry->connected_band){
			/* CLI entry already present on driver table, adjust stat */
			inf_source->driver_table_size--;
			inf_source = NULL;
		}
		entry->Control_Flags &= ~fBND_STRG_CLIENT_RE_STEERING;
		entry->connected_band = BAND_INVALID;
	} else{
		inf_source = bndstrg_get_interface(ctrl_iface, NULL, entry->band, TRUE);
	}

	inf_target = bndstrg_get_interface(ctrl_iface, NULL, band, TRUE);
	
	if(!inf_target)
	{
		DBGPRINT(DEBUG_TRACE,("Invalid target band [%d] for Addr::%02x:%02x:%02x:%0x:%02x:%02x\n\r"),
		band,PRINT_MAC(entry->Addr));
		return FALSE;
	}
	
#ifdef BND_STRG_QA	

	BND_STRG_PRINTQAMSG(table,entry,"[%d]Addr=%02x:%02x:%02x:%02x:%02x:%02x,entry->band=%s,prefer_band=%s elpased_time=%u sec\n\r",
						__LINE__,PRINT_MAC(entry->Addr),bndstrg_get_entry_band(entry->band),bndstrg_get_entry_band(band), bndstrg_get_elapsed_time(entry->tp));
	BND_STRG_PRINTQAMSG(table,entry,"5GH_5GL:%s \t5GH_2G:%s \t5GL_2G:%s \n",
					entry_match_steering_str[entry->match_steered_rule_id[CMP_5GH_5GL]],
					entry_match_steering_str[entry->match_steered_rule_id[CMP_5GH_2G]],
					entry_match_steering_str[entry->match_steered_rule_id[CMP_5GL_2G]]);
#endif /* BND_STRG_QA */

	if(table->BtmMode != BTM_Only || 
		entry->Operation_steered != TRUE)
	{
		switch (entry->band) {
		case BAND_2G :
			entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G);
		break;

		case BAND_5G_L :
			entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G);
		break;

		case BAND_5G_H :
			entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G);
		break;
		
		}
	}

	if(entry->Operation_steered && entry->btm_info.BTMSupport && (table->BtmMode != BTM_Off)){
		/* Steer 11v client using BTM*/
		entry->btm_info.BTMSourceBand = entry->connected_band;
		bndstrg_wnm_send_btm_req(bndstrg, inf_source, inf_target, entry);
	} else {
		if(inf_source) bndstrg_accessible_cli(bndstrg, inf_source, entry, CLI_DEL);
	}
	
	if(inf_target->driver_table_size >=  inf_target->max_driver_table_size){
		struct bndstrg_cli_entry *entry_del = NULL;
		entry_del = bndstrg_get_old_driver_entry(bndstrg, inf_target, entry->Addr);
		if(entry_del){
			bndstrg_accessible_cli(bndstrg, inf_target, entry_del, CLI_DEL);
			bndstrg_delete_entry(table,entry_del->Addr,entry_del->TableIndex);
			bndstrg_accessible_cli(bndstrg, inf_target, entry, CLI_ADD);
			entry->band = band;
		}else{
			DBGPRINT(DEBUG_ERROR,("%s() interface(%s) driver table full [max size:%d], drop this entry steering!\n\r"),
			__func__, (IS_2G_BAND(band)? "2G":"5G"), inf_target->driver_table_size);
			return FALSE;
		}
	}else{
		bndstrg_accessible_cli(bndstrg, inf_target, entry, CLI_ADD);
		entry->band = band;
	}

	if(table->BndStrgMode == POST_CONNECTION_STEERING && table->BtmMode != BTM_Only)
	{
		bndstrg_update_white_black_list(bndstrg, inf_source, entry->Addr, bndstrg_blacklist, 1);
	}
	return TRUE;
}

u8 bndstrg_dump_steered_STA(
	struct bndstrg *bndstrg,
	struct bndstrg_cli_entry *entry)
{
	if (entry->steered)
	{
		if (entry->bConnStatus) 
		{
			DBGPRINT(DEBUG_ERROR,YLW("[Operation Steering]steering![%d|%d|%d|%d]\n"),
				entry->steered,IS_2G_BAND(entry->band)?1:0,IS_5G_L_BAND(entry->band)?1:0,IS_5G_H_BAND(entry->band)?1:0);
		}
		else
		{
			DBGPRINT(DEBUG_TRACE,RED("[Association Steering]steering![%d|%d|%d|%d]\n"),
				entry->steered,IS_2G_BAND(entry->band)?1:0,IS_5G_L_BAND(entry->band)?1:0,IS_5G_H_BAND(entry->band)?1:0);
		}
		return TRUE;
	}
	DBGPRINT(DEBUG_TRACE,RED("[Association Steering]Default steering!\n"));
	return FALSE;
}

/* this function only run by not connected yet STA*/
u8 bndstrg_association_steering(
        struct bndstrg_cli_table *table,
		struct bndstrg_cli_entry *entry,
		struct bndstrg *bndstrg)
{
	u8 band = BAND_INVALID; 
	
	if (!table)
	{
		DBGPRINT(DEBUG_ERROR,"%s(): Error! table is NULL!\n", __FUNCTION__);
		return FALSE;
	}
	if (!entry)
	{
		DBGPRINT(DEBUG_ERROR,RED("%s(): Error! entry is NULL!\n"), __FUNCTION__);
		return FALSE;
	}
				
    if (!IS_BND_STRG_DUAL_BAND_CLIENT(entry->Control_Flags)) 
	{
        if (entry->Control_Flags & fBND_STRG_CLIENT_IS_2G_ONLY)
		{
            entry->Manipulable = FALSE;
			entry->Control_Flags |= fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G;
			band = BAND_2G;
		}
		else if(entry->Control_Flags & fBND_STRG_CLIENT_IS_5G_ONLY)
		{
            entry->Manipulable = FALSE;
			if (IS_BND_STRG_H5G_L5G_BAND_CLIENT(entry->Control_Flags))
			{
				 band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_5GL);
			}
            else if(entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G) 
			{
                entry->Control_Flags |= fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G;
                band = BAND_5G_H;
            }
            else 
			{
                entry->Control_Flags |= fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G;
				band = BAND_5G_L;
            }
		}else if(!IS_2G_BAND(table->Band)){
			if (IS_BND_STRG_H5G_L5G_BAND_CLIENT(entry->Control_Flags))
			{
				 band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_5GL);
			}
		}
    }
    else 
	{
        /*
              * compare_mode:  0=H-5G vs L-5G
              *                         1=H-5G vs 2.4G
              *                         2=L-5G vs 2.4G
             */
        if(table->op_mode == OP_TRIBAND) 
		{
			u8 temp_band = BAND_INVALID; 
            /* For 5G L/H band all support case*/
			if ((entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G)&&
				(entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_L5G))
			{
				band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_5GL);
                switch(band)
				{
                    case BAND_5G_L:
					case BAND_5G_H:
						temp_band = band;
						if (band == BAND_5G_L)
							band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GL_2G);
						else if (band == BAND_5G_H)
							band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_2G);
						if (band == BAND_INVALID)
						{
							//restore 1st round prefer band
							band = temp_band;
						}
                        break;
                    
                    case BAND_2G:
                    default:
					    //DBGPRINT(DEBUG_ERROR,RED("%s():[%d] debug here!\n"), __FUNCTION__,__LINE__);
					    break;
                }
            }
            /* For 5G L band only support case*/
            else if (entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_L5G) 
			{
                band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GL_2G);
            }
            /* For 5G H band only support case*/
            else if (entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G)
			{
                band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_2G);       
            }
            else 
			{
                DBGPRINT(DEBUG_ERROR,RED("%s(): Unexpected Error!\n"), __FUNCTION__);
                return FALSE;
            }
        }
        else if(table->op_mode == OP_DUALBAND) 
		{
			if(IS_2G_BAND(table->Band)){
	            if(entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_L5G)
	                band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GL_2G); 
	            else 
			    {
	                DBGPRINT(DEBUG_ERROR,RED("%s(): Unexpected Error!\n"), __FUNCTION__);
	                return FALSE;
	            }
			} 
        }
	}

	if ((band != BAND_INVALID) && (band != entry->band)) 
	{
		if (bndstrg_client_band_update(bndstrg,entry,band)==TRUE) {
			entry->state = ENTRY_READY_TO_ASSOC;
			bndstrg_dump_steered_STA(bndstrg,entry);
		}
		else
		{
			DBGPRINT(DEBUG_ERROR,("%s() steering fail\n\r"), __func__);
			bndstrg_dump_steered_STA(bndstrg,entry);
			return FALSE;
		}
	}

	return TRUE;
}

u8 bndstrg_steer_sta(struct bndstrg *bndstrg, struct bndstrg_cli_entry *entry)
{
	u8 band = BAND_INVALID;
	u32 elapsed_time;
	struct bndstrg_cli_table *table = &bndstrg->table;
	if (!entry || (!entry->bValid))
		return FALSE;

	if((table->BtmMode & BTM_Only) && (!entry->btm_info.BTMSupport))
		return FALSE;
	
	if (entry->Operation_steered == TRUE)
	{
		entry->Operation_steered_tick++;
		/* give up operation steering after 30 sec*/
		if (entry->Operation_steered_tick >= ENTRY_BACKUP_TIME)
		{
			entry->Operation_steered_tick = 0;
			entry->Operation_steered = FALSE;
		}

		if ((table->BtmMode == BTM_Only) && 
			(entry->btm_info.BTMSupport) &&
			(entry->bConnStatus == TRUE) &&
			(entry->Operation_steered_tick >= 3))
		{
			bndstrg_update_btm_status(bndstrg, entry);
		}

		band = entry->band;
		goto find_out;
	}

	if(!entry->bConnStatus)
		return FALSE;	
	
	if(entry->total_steered_cnt)
		elapsed_time = bndstrg_get_elapsed_time(entry->CliSteerInfo.steer_tp[entry->CliSteerInfo.end_idx]);
	else
		elapsed_time = bndstrg_get_elapsed_time(entry->tp);
	
	if((elapsed_time) < table->dwell_time){
#ifdef BND_STRG_QA		
		BND_STRG_PRINTQAMSG(table, entry,("%s:%d Addr=%02x:%02x:%02x:%02x:%02x:%02x skip operational steering as elapsed time is %u sec less than dwell time %u sec\n"),
						__func__,__LINE__,PRINT_MAC(entry->Addr), elapsed_time, table->dwell_time);
#endif
		return FALSE;
	}

	if(entry->total_steered_cnt >= table->max_steer_count){
#ifdef BND_STRG_QA		
		BND_STRG_PRINTQAMSG(table, entry,("%s:%d Addr=%02x:%02x:%02x:%02x:%02x:%02x Max Steer count reached to %d, can't do operational steering\n"),
						__func__,__LINE__,PRINT_MAC(entry->Addr), entry->total_steered_cnt);
#endif
		bndstrg_check_steering_limit(table,entry);
		return FALSE;
	}
	
	if (!IS_BND_STRG_DUAL_BAND_CLIENT(entry->Control_Flags))
	{
		if (IS_BND_STRG_H5G_L5G_BAND_CLIENT(entry->Control_Flags))
		{
			/* 5GH vs 5GL up/down steer call*/
			band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_5GL);
		}
	}
	else
	{
		if (IS_5G_H_BAND(entry->band))
		{
			if (IS_BND_STRG_H5G_L5G_BAND_CLIENT(entry->Control_Flags))
			{
				/* 5GH vs 5GL down steer call */
				band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_5GL);
				if (band == BAND_INVALID)
				{
					/* 5GH vs 2.4G down steer call*/
					band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_2G);
				}
			}
			else
			{
				/*5GH vs 2.4G down steer call*/
				band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_2G);
			}
		}
		else if (IS_5G_L_BAND(entry->band))
		{
			if (IS_BND_STRG_H5G_L5G_BAND_CLIENT(entry->Control_Flags))
			{
				/* 5GL vs 2.4G down steer call*/
				band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GL_2G);
				if (band == BAND_INVALID)
				{
					/* 5GL vs 5GH  up steer call*/
					band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_5GL);
				}
			}
			else
			{
				/*5GL vs 2.4G down steer call */
				band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GL_2G);
			}
		}
		else if(IS_2G_BAND(entry->band)) {
			if ((entry->Control_Flags & fBND_STRG_CLIENT_SUPPORT_H5G))
			{
				/* H-5G vs 2.4G */
				band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GH_2G);
				if (band == BAND_INVALID)
				{
					/* L-5G vs 2.4G */
					band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GL_2G);
				}
			}
			else
			{
				/* L-5G vs 2.4G */
				band = bndstrg_mtk_rule_iterate(table,entry,bndstrg,CMP_5GL_2G);
			}
		}
	}

find_out:
	if ((band != BAND_INVALID) && (band != entry->band)) 
	{
		if (bndstrg_client_band_update(bndstrg,entry,band)==TRUE) {
			entry->state = ENTRY_OPER_STEER_ACTIVE;
			bndstrg_dump_steered_STA(bndstrg,entry);
			return TRUE;
		}
		else
		{
			DBGPRINT(DEBUG_ERROR,("%s() steering fail\n\r"), __func__);
			bndstrg_dump_steered_STA(bndstrg,entry);
			return FALSE;
		}
	}
	return FALSE;
}

/* this function only run by connected STA*/
u8 bndstrg_operation_steering(
	struct bndstrg_cli_table *table,
	struct bndstrg *bndstrg)
{
	u8 i;
	struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
	struct bndstrg_cli_entry *entry=NULL;
#ifdef VENDOR_FEATURE7_SUPPORT
	struct bndstrg_iface *inf=NULL;
#else
	struct bndstrg_iface *inf_5GL=NULL, *inf_5GH=NULL;
#endif
	u32 count=0;
	u8 steered;
	if (!table)
	{
		DBGPRINT(DEBUG_ERROR,"%s(): Error! table is NULL!\n", __FUNCTION__);
		return FALSE;
	}

#ifndef VENDOR_FEATURE7_SUPPORT
	if(bndstrg->table.op_mode == OP_TRIBAND){
		inf_5GL = bndstrg_get_interface(ctrl_iface, NULL, BAND_5G_L, TRUE);
		inf_5GH = bndstrg_get_interface(ctrl_iface, NULL, BAND_5G_H, TRUE);
	}else{
		if(IS_2G_BAND(bndstrg->table.Band))
			inf_5GL = bndstrg_get_interface(ctrl_iface, NULL, BAND_5G_L, TRUE);
		else
			inf_5GH = bndstrg_get_interface(ctrl_iface, NULL, BAND_5G_H, TRUE);
	}
	
	if(inf_5GH && inf_5GH->worst_entry) {
		steered = bndstrg_steer_sta(bndstrg, inf_5GH->worst_entry);
		if(steered == TRUE){
			DBGPRINT(DEBUG_OFF, "%s::5G_H worst client %02x:%02x:%02x:%02x:%02x:%02x steer to lower band",
				__FUNCTION__, PRINT_MAC(inf_5GH->worst_entry->Addr));
			inf_5GH->worst_entry = NULL;
			return TRUE;
		}
	}
	else if(inf_5GL && inf_5GL->worst_entry) {
		steered = bndstrg_steer_sta(bndstrg, inf_5GL->worst_entry);
		if(steered == TRUE){
			DBGPRINT(DEBUG_OFF, "%s::5G_L worst client %02x:%02x:%02x:%02x:%02x:%02x steer to lower band",
				__FUNCTION__, PRINT_MAC(inf_5GL->worst_entry->Addr));
			inf_5GL->worst_entry = NULL;
			return TRUE;
		}
	}
#endif
	
	for(i=0;i<table->max_steering_size;i++){

		entry = &table->Entry[i];
		if ((!entry) || (!entry->bValid))
			continue;
		else
		    count++;
		
		if ((entry->bConnStatus == FALSE) && (entry->Operation_steered == FALSE))
			continue;
		
#ifdef VENDOR_FEATURE7_SUPPORT
		if(entry->low_rssi_disconnect_cnt >= table->RssiCheckCount){
#ifdef BND_STRG_QA		
		BND_STRG_PRINTQAMSG(table, entry,("Addr=%02x:%02x:%02x:%02x:%02x:%02x Disconnect STA as rssi less than %d rssi threshold\n"),
			PRINT_MAC(entry->Addr), table->RSSIDisconnect);
#endif
			bndstrg_update_white_black_list(bndstrg,NULL,entry->Addr,bndstrg_blacklist,1);
			entry->BlackListTimer = table->BlackListTime;
			inf = bndstrg_get_interface(ctrl_iface, NULL, entry->connected_band, TRUE);
			if(inf) bndstrg_accessible_cli(bndstrg, inf, entry, CLI_DEL);
			break;
		}
#endif
		steered = bndstrg_steer_sta(bndstrg, entry);

		if (steered == TRUE)
			break;
		
		if(count >= table->Size)
		    break;
	}

	return TRUE;
}

u8 bndstrg_inf_status_polling(struct bndstrg *bndstrg,struct bndstrg_cli_table *table)
{
    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
    struct bndstrg_iface *inf;
    u8 i,inf_ready_count = 0;
	u8 band_cnt[MAX_INF_NUM] = {0};
	static	int polling_cnt = 0;
	
	polling_cnt++;
	memset(band_cnt,0x00,sizeof(band_cnt));
    /* polling all interface until bndstrg is ready */
    for(i=0; i<ctrl_iface->Size; i++)
    {
        inf = &ctrl_iface->inf[i];
        if(inf->bValid && !inf->bInfReady){
            bndstrg_inf_status_query(bndstrg, (char*)ctrl_iface->inf[i].ucIfName, INF_STATUS_QUERY);
			DBGPRINT(DEBUG_OFF, "\n[%s]inf_name:[%s]\n",__FUNCTION__,ctrl_iface->inf[i].ucIfName);
        }
		
		if (inf->bInfReady) {
			if ((inf->Band&BAND_2G) == BAND_2G)
				band_cnt[0]++;
			if ((inf->Band&BAND_5G_L) == BAND_5G_L)
				band_cnt[1]++;
			if ((inf->Band&BAND_5G_H) == BAND_5G_H)
				band_cnt[2]++;
		}

		if(inf->bValid && inf->bInfReady)
			inf_ready_count++;	
    }

	if(ctrl_iface->Size < 2){
		DBGPRINT(DEBUG_OFF, RED("[%d]:Invalid Band topology. 2.4G=%d,L-5G=%d,H-5G=%d\n"),__LINE__,band_cnt[0],band_cnt[1],band_cnt[2]);
		return FALSE;
	}	
		
    if(inf_ready_count != ctrl_iface->Size)
        return FALSE;

	if ((band_cnt[0]>=2) ||(band_cnt[1]>=2)||(band_cnt[2]>=2)){
		if (polling_cnt > 3) {
			DBGPRINT(DEBUG_OFF, RED("[%d]:Invalid Band topology. 2.4G=%d,L-5G=%d,H-5G=%d\n"),__LINE__,band_cnt[0],band_cnt[1],band_cnt[2]);
		}
        return FALSE;
	}
	polling_cnt = 0;
    return TRUE;
}

u8 bndstrg_chanload_status_polling(struct bndstrg *bndstrg,struct bndstrg_cli_table *table)
{
    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
    struct bndstrg_iface *inf;
    u8 i;

    /* polling all interface until bndstrg is ready */
    for(i=0; i<ctrl_iface->Size; i++)
    {
        inf = &ctrl_iface->inf[i];
        if(inf->bValid && inf->bInfReady)
            bndstrg_chanload_status_query(bndstrg, (char*)ctrl_iface->inf[i].ucIfName);
    }
    return TRUE;
}

u8 bndstrg_table_en_polling(struct bndstrg *bndstrg,struct bndstrg_cli_table *table,u8 en)
{
//    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
    struct bndstrg_iface *inf;
	u8 i;
	if (bndstrg == NULL) {
		DBGPRINT(DEBUG_ERROR,RED("bndstrg is NULL\n"));
		return FALSE;
	}
	if (table == NULL) {
		DBGPRINT(DEBUG_ERROR,RED("table is NULL\n"));
		return FALSE;
	}
	DBGPRINT(DEBUG_OFF,("bndstrg=%p,table=%p,en=%d\n"),bndstrg,table,en);
	if (en) {
		for(i = 0; i < bndstrg->ctrl_iface.Size; i++)
		{
	    	inf = &bndstrg->ctrl_iface.inf[i];
	    	if(inf && !inf->binfEnable)
	       		bndstrg_onoff(bndstrg, (char*)inf->ucIfName, en, table->BndStrgMode);
		}
	}
	else {
		for(i = 0; i < bndstrg->ctrl_iface.Size; i++)
		{
	    	inf = &bndstrg->ctrl_iface.inf[i];
	    	if(inf && inf->binfEnable)
	       		bndstrg_onoff(bndstrg, (char*)inf->ucIfName, en, table->BndStrgMode);
		}
	}

	return TRUE;
}

void bndstrg_send_heartbeat(struct bndstrg *bndstrg)
{
    struct bndstrg_iface *inf = NULL;
	u8 i;
	
	for(i = 0; i < bndstrg->ctrl_iface.Size; i++)
	{
    	inf = &bndstrg->ctrl_iface.inf[i];
    	if(inf->bValid && inf->bInfReady)
       		bndstrg_inf_status_query(bndstrg, (char*)inf->ucIfName, HEARTBEAT_MONITOR);
	}
}

void bndstrg_periodic_exec(void *eloop_data, void *user_ctx)
{
	int i, count=0;
	struct bndstrg_cli_entry *entry = NULL;
	struct bndstrg *bndstrg = (struct bndstrg*) user_ctx;
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bndstrg_iface *inf = NULL;
	static enum bndstrg_state old_state;
	static char *bndstrg_state[]={"BNDSTRG_INIT","BNDSTRG_INF_POLL","BNDSTRG_TBL_EN","BNDSTRG_TBL_READY","BNDSTRG_LEAVE","BNDSTRG_FROZEN"};
	static char *table_opmode[]={"NO_BNDSTR","DUALBAND","TRIBAND"};
#ifdef ROAMING_SUPPORT
	u8 *buf = NULL, iapp_msg_size = 0, connected_sta_count = 0, sta_counter = 0;
	struct bndstrg_iapp_frame iapp_frame ;
	u16 length = 0;
#endif /*ROAMING_SUPPORT*/

	if (bndstrg->state != old_state)
		DBGPRINT(DEBUG_OFF, GRN("bndstrg->state=%s %s\n"),bndstrg_state[bndstrg->state],
				(bndstrg->state==BNDSTRG_TBL_READY)?(table_opmode[table->op_mode]):"");
	switch (bndstrg->state) {
		case BNDSTRG_INIT:
			old_state = bndstrg->state;
			bndstrg->state = BNDSTRG_INF_POLL;
			goto end_of_periodic_exec;
		case BNDSTRG_INF_POLL:
			if (bndstrg->state != old_state) {
				/*
				 * state change, disable bandstrg
				 * remove all daemon entry and disable dirver band steering
				 */
				for (i=0;i<table->max_steering_size;i++) {
					bndstrg_delete_entry(table, NULL, i);
				}
				table->active_client_2G=0;
				table->active_client_5G=0;
				table->active_client_L5G=0;
				table->active_client_H5G=0;
				bndstrg_table_en_polling(bndstrg,table,0);
			}
			old_state = bndstrg->state;
			/* polling band inf status */
			if (bndstrg_inf_status_polling(bndstrg,table) == FALSE) {
				goto end_of_periodic_exec;
			}
			bndstrg->inf_stable_cnt++;
			if (bndstrg->inf_stable_cnt >= 1) {
				bndstrg->inf_stable_cnt = 0;
				bndstrg->state = BNDSTRG_TBL_EN;
			}
			goto end_of_periodic_exec;
		case BNDSTRG_TBL_EN:
			old_state = bndstrg->state;
			/* enale all active inf bandsteering */
			bndstrg_table_en_polling(bndstrg,table,1);
			bndstrg->inf_stable_cnt++;
			if (bndstrg->inf_stable_cnt >= 1) {
				bndstrg->inf_stable_cnt = 0;
				if (table->bEnabled) {
					bndstrg->state = BNDSTRG_TBL_READY;
				}
			}
			goto end_of_periodic_exec;
		case BNDSTRG_TBL_READY:
			old_state = bndstrg->state;/* Ready to run bndstrg algorithm */
			bndstrg_read_inf_param(bndstrg); /*Read interface related parameters*/
			break;
		case BNDSTRG_LEAVE:
			old_state = bndstrg->state;
			eloop_terminate();
			return;
		case BNDSTRG_FROZEN:
			old_state = bndstrg->state;
			goto end_of_periodic_exec;
		default:
			goto end_of_periodic_exec;
	}

#ifdef ROAMING_SUPPORT
	//Delete obsolete station entries
	{
		//PBS_LIST_ENTRY 	pBsListEntry = NULL;
		RT_LIST_ENTRY *pListEntry = NULL, *pDelEntry = NULL;
		struct os_time now = {0};

		os_get_time(&now);

		pListEntry = table->roaming_list.pHead;
		//pBsListEntry = (PBS_LIST_ENTRY)pListEntry;
		while (pListEntry != NULL)
		{
			if ((now.sec - ((PBS_LIST_ENTRY)pListEntry)->last_update.sec) > table->roaming_entry_timeout)
			{
				DBGPRINT(DEBUG_INFO, "pListEntry before del:%p, pListEntry->next:%p listsize:%d\n",
									pListEntry, pListEntry->pNext, table->roaming_list.size);

				pDelEntry = delEntryList(&table->roaming_list,pListEntry);
				DBGPRINT(DEBUG_INFO, "pDelEntry :%p\n", pDelEntry);

				if(pDelEntry)
				{
					pListEntry = pDelEntry->pNext;
					DBGPRINT(DEBUG_INFO, "pListEntry after del:%p\n", pListEntry);
					//call driver entry delete
					DBGPRINT(DEBUG_TRACE, "call driver whitelist delete\n");
					bndstrg_update_white_black_list(bndstrg, NULL, ((PBS_LIST_ENTRY)pDelEntry)->iapp_sta_entry.addr, bndstrg_whitelist, 0);
					os_free(pDelEntry);
					pDelEntry = NULL;
				}
				else
				{
					pListEntry = NULL;
				}
			}
			else
				pListEntry = pListEntry->pNext;
		}
	}

	table->iapp_counter++;
	if(table->iapp_counter >= table->iapp_msg_send_timer)
	{
		connected_sta_count = table->active_client_2G + table->active_client_5G;
		iapp_msg_size = sizeof(struct bndstrg_iapp_hdr) +
						((connected_sta_count + 2) * sizeof(struct bndstrg_iapp_sta_entry));	//extra 2 station entry buffer to avoid any realloc
		buf = os_malloc(iapp_msg_size);
		if(buf)
		{
			memset(buf, 0, iapp_msg_size);
			iapp_frame.iapp_hdr.command = 1;	// 1 = broadcast sta list
			iapp_frame.iapp_hdr.sender_ip = htonl(bndstrg->AddrOwn.s_addr);
			length += sizeof(struct bndstrg_iapp_hdr);		//leave space for iapp_header
			DBGPRINT(DEBUG_INFO, BLUE("bndstrg_iapp_frame_size:%d bndstrg_iapp_hdr_size:%d bndstrg_iapp_sta_entry:%d iapp_msg_size:%d length:%d\n"),
				sizeof(struct bndstrg_iapp_frame),sizeof(struct bndstrg_iapp_hdr),sizeof(struct bndstrg_iapp_sta_entry), iapp_msg_size, length);
		}
		else
			DBGPRINT(DEBUG_ERROR, "Buffer alloc failed\n");
	}
#endif /*ROAMING_SUPPORT*/

	for (i = 0; i < table->max_steering_size; i++)
	{
		entry = &table->Entry[i];
		if (entry->bValid == TRUE)
		{
		    count++;

#ifdef ROAMING_SUPPORT
			if(entry->bConnStatus == TRUE)
			{
				struct bndstrg_iapp_sta_entry sta_entry = {0};
				sta_counter++;
				if(buf)
				{
					if((connected_sta_count + 2) < sta_counter)
					{
						os_free(buf);	//exceeded the allocated buffer
						buf = NULL;
					}
					else
					{
						sta_entry.band = entry->band;
						sta_entry.control_flags = htonl(entry->Control_Flags);
						sta_entry.elapsed_connection_time = htonl(entry->elapsed_time);
						sta_entry.ap_ip_addr = htonl(bndstrg->AddrOwn.s_addr);
						memcpy(sta_entry.addr, entry->Addr, MAC_ADDR_LEN);

						//copy data to buf
						memcpy(&buf[length], (u8 *)&sta_entry, sizeof(struct bndstrg_iapp_sta_entry));
						length += sizeof(struct bndstrg_iapp_sta_entry);
						DBGPRINT(DEBUG_INFO, BLUE("length:%d, sta_counter:%d"),length, sta_counter);
					}
				}
			}
#endif /*ROAMING_SUPPORT*/
			/* process non-connected STA entry only */
			if((entry->Operation_steered == TRUE) || (entry->bConnStatus == TRUE))
				continue;

#ifdef VENDOR_FEATURE7_SUPPORT
			if(entry->BlackListTimer > 0){
				entry->BlackListTimer --;
				if(entry->BlackListTimer == 0)
					bndstrg_update_white_black_list(bndstrg, NULL, entry->Addr, bndstrg_blacklist, 0);
				continue;
			}
#endif

#ifdef WPS_SUPPORT
			//WPS: update wps first connection timer
			if(entry->WpsFirstAssocCounter > 0)
				entry->WpsFirstAssocCounter++;

			if(entry->WpsFirstAssocCounter > 20)
			{
				entry->bConnStatus = FALSE;
#ifdef BND_STRG_QA
				BND_STRG_PRINTQAMSG(table, entry, RED("entry (%02x:%02x:%02x:%02x:%02x:%02x) WPS second connection timed out, change ConnStatus to FALSE\n"),
					PRINT_MAC(entry->Addr));
#endif
			}
#endif /*WPS_SUPPORT*/

			/* check aged entry */
			if (bndstrg_check_entry_aged(bndstrg,entry)) 
			{
				/* if entry is connected, it should run _bndstrg_allow_connection
				 * to do 5G_DYNAMIC_RSSI and LOAD_BALANCE
				 */
				if (!entry->bConnStatus)
					continue;
			}

			/* For disconnected STA, process association steering */
			if((table->BndStrgMode & PRE_CONNECTION_STEERING) &&
			    (entry->enable_compare_flag) && (entry->Channel == 0))
				bndstrg_association_steering(table, entry, bndstrg);

			/* Handle Assoc Probe info missmatch case*/
			if((entry->AssocProbeInfoMissMatch == TRUE) && (entry->state == ENTRY_ASSOC_RCD)){
				entry->bConnStatus = TRUE;
				inf = bndstrg_get_interface(&bndstrg->ctrl_iface,NULL,entry->band,TRUE);
				entry->Channel = inf->Channel;
			}
		}
		if(count >= table->Size)
		    break;
	}

#ifdef ROAMING_SUPPORT
	if(buf)				//means it got buffer, ie its 10th second, so need to broadcast
	{
		if(((connected_sta_count + 2) < sta_counter)/*|| (sta_counter == 0)*/)
		{
			free(buf);	//exceeded the allocated buffer or no connected stations
			buf = NULL;
		}
		else
		{
			iapp_frame.iapp_hdr.sta_count = sta_counter;
			iapp_frame.iapp_hdr.length = htons(length - sizeof(struct bndstrg_iapp_hdr));
			memcpy(buf, (u8 *)&iapp_frame, sizeof(struct bndstrg_iapp_frame));
			udp_send(bndstrg, buf, length);
			table->iapp_counter = 0;
			free(buf);
			buf = NULL;
		}
	}
#endif /*ROAMING_SUPPORT*/

	if(table->BndStrgMode & POST_CONNECTION_STEERING){
		if(table->chanload_priority_enabled == TRUE) {
			bndstrg_chanload_status_polling(bndstrg,table);
		}
		bndstrg_polling_connected_sta(bndstrg, (table->bndstrg_run_time % table->polling_sta_period));
		if(table->chanload_priority_enabled == TRUE){ 
			/*heavy chanload update*/
			bndstrg_chanload_heavy_update(bndstrg);
			/*update connect STA */
			bndstrg_sta_update(bndstrg);
		}
		
		table->bndstrg_run_time++;
		if((table->bndstrg_run_time % table->dynamic_bndstrg_period) == 0)
			bndstrg_operation_steering(table, bndstrg);
	}else if(table->chanload_priority_enabled){
		bndstrg_chanload_status_polling(bndstrg,table);
		bndstrg_chanload_heavy_update(bndstrg);
	}
	bndstrg_send_heartbeat(bndstrg);

end_of_periodic_exec:
	eloop_register_timeout(1, 0, bndstrg_periodic_exec, NULL, bndstrg);

}

int BndStrgGetKeyParameter(char *key, char *keyvalue, char *buf)
{
	char pMemBuf[128] = { 0 }, *temp_buf1 = NULL, *temp_buf2 = NULL;
	char *start_ptr, *end_ptr;
	char *ptr;
	int  len;

	temp_buf1 = pMemBuf;
	temp_buf2 = (char *)(pMemBuf + 64);

	strcpy(temp_buf1, "\n");
	strcat(temp_buf1, key);
	strcat(temp_buf1, "=");

	/*search key*/
	if((start_ptr = strstr(buf, temp_buf1)) == NULL)
	{
		return (FALSE);
	}

	start_ptr += strlen("\n");
	if((end_ptr = strstr(start_ptr, "\n")) == NULL)
		end_ptr = start_ptr+strlen(start_ptr);

	if (end_ptr < start_ptr)
	{
		return (FALSE);
	}

	strncpy(temp_buf2, start_ptr, end_ptr-start_ptr);
	temp_buf2[end_ptr-start_ptr]='\0';

	if((start_ptr = strstr(temp_buf2, "=")) == NULL)
	{
		return (FALSE);
	}
	ptr = (start_ptr + 1);
	len = strlen(start_ptr);

	memset(keyvalue, 0x00, 64);
	strncpy(keyvalue, ptr, len);


	return TRUE;
}

void BndStrg_Update_Default_Param(struct bndstrg_cli_table *table)
{
	FILE *fp;
	char buf[FILE_BUFFER_SIZE] = {0};
	char keyvalue[64] = { 0 };
	int i = 0, value = 0;
	int PriorityList[fBND_STRG_PRIORITY_MAX] = { 0 }, PriorityListSize = 0;
	char *macptr = NULL;
	DBGPRINT(DEBUG_TRACE,"\n");
	
	fp = fopen(BNDSTRG_DAT_FILE_PATH, "r");
	
	if (fp) {
		fread(buf, sizeof(char), FILE_BUFFER_SIZE, fp);
		if (BndStrgGetKeyParameter("DwellTime", keyvalue, buf)) {
			table->dwell_time = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("RSSICheckCount", keyvalue, buf)) {
			table->RssiCheckCount= strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("RssiDiff", keyvalue, buf)) {
			table->RssiDiff= strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("IdleTxByteCount", keyvalue, buf)) {
			table->IDLE_TX_Byte_Count= strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("IdleRxByteCount", keyvalue, buf)) {
			table->IDLE_RX_Byte_Count= strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("SteerTimeWindow", keyvalue, buf)) {
			table->max_steer_time_window = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("MaxSteerCount", keyvalue, buf)) {
			table->max_steer_count = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("AgeTime", keyvalue, buf)) {
			table->AgeTime = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("CheckTime", keyvalue, buf)) {
			table->CheckTime = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("HoldTime", keyvalue, buf)) {
			table->HoldTime = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("RssiLow", keyvalue, buf)) {
			table->RssiLow = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("RSSILowDownSteer", keyvalue, buf)) {
			table->RSSILowDownSteer = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("RSSIHighUpSteer", keyvalue, buf)) {
			table->RSSIHighUpSteer = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("BtmMode", keyvalue, buf)) {
			table->BtmMode = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("CndPriority", keyvalue, buf)) {
			//memset(table->PriorityList, 0, sizeof(table->PriorityList));
			//table->PriorityListSize = 0;
			for (i = 0, macptr = strtok(keyvalue, ";"); macptr; macptr = strtok(NULL, ";"), i++) {
				value = strtol(macptr, NULL, 10);
				if(value < fBND_STRG_PRIORITY_MAX){
					PriorityList[i] = value;
					PriorityListSize ++;
				}
			}
			if(PriorityListSize > 0){
				os_memcpy(table->PriorityList, PriorityList, sizeof(PriorityList));
				table->PriorityListSize = PriorityListSize;
			}
		}
		if (BndStrgGetKeyParameter("NVRAMReset", keyvalue, buf)) {
			value = strtol(keyvalue, NULL, 10);
			if(value == 0 || value == 1)
				table->nvram_reset = value;
			else
				table->nvram_reset = FALSE;
		}
#ifdef VENDOR_IOS_SUPPORT
		if (BndStrgGetKeyParameter("IOSSteerNum", keyvalue, buf)) {
			table->IOSNumLimit = strtol(keyvalue, NULL, 10);
			if (table->IOSNumLimit > MAX_IOS_NUM)
				table->IOSNumLimit = MAX_IOS_NUM;
		}
#endif		
		
#ifdef VENDOR_FEATURE7_SUPPORT
		if (BndStrgGetKeyParameter("RSSIDisconnect", keyvalue, buf)) {
			value = strtol(keyvalue, NULL, 10);
			if(value < 0)
				table->RSSIDisconnect = value;
		}
		if (BndStrgGetKeyParameter("BlackListTime", keyvalue, buf)) {
			value = strtol(keyvalue, NULL, 10);
			if(value > 0)
				table->BlackListTime = value;
		}
		if (BndStrgGetKeyParameter("NVRAMTableSize", keyvalue, buf)) {
			value = strtol(keyvalue, NULL, 10);
			if(value <= MAX_NVRAM_TABLE_SIZE)
				table->nvram_table_size = value;
		}
#endif

#ifdef ROAMING_SUPPORT
		if (BndStrgGetKeyParameter("EthIface", keyvalue, buf)) {
			strcpy(table->eth_iface, keyvalue);
		}
		if (BndStrgGetKeyParameter("EthPort", keyvalue, buf)) {
			table->eth_port = (unsigned short)strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("IappSendTimer", keyvalue, buf)) {
			table->iapp_msg_send_timer = strtol(keyvalue, NULL, 10);
		}
		if (BndStrgGetKeyParameter("RoamingEntryTimeout", keyvalue, buf)) {
			table->roaming_entry_timeout = strtol(keyvalue, NULL, 10);
		}
#endif
		fclose(fp);
	}
	return;
}

void bndstrg_read_inf_param(struct bndstrg *bndstrg)
{
	FILE *fp;
	char buf[FILE_BUFFER_SIZE] = {0};
	char keyvalue[64] = { 0 }, *temp;
	int i = 0;// value = 0;
	//char *macptr = NULL;
	DBGPRINT(DEBUG_TRACE,"\n");
	struct bndstrg_iface *inf_2G=NULL, *inf_5GL=NULL, *inf_5GH=NULL;

	fp = fopen(BNDSTRG_DAT_FILE_PATH, "r");

	inf_2G = bndstrg_get_interface(&bndstrg->ctrl_iface, NULL, BAND_2G, TRUE);
	inf_5GL = bndstrg_get_interface(&bndstrg->ctrl_iface, NULL, BAND_5G_L, TRUE);
	inf_5GH = bndstrg_get_interface(&bndstrg->ctrl_iface, NULL, BAND_5G_H, TRUE);

	if (fp) {
		fread(buf, sizeof(char), FILE_BUFFER_SIZE, fp);
		if (BndStrgGetKeyParameter("ChanLoadThr", keyvalue, buf)) {
				for (i = 0, temp = strtok(keyvalue, ";"); temp; temp = strtok(NULL, ";"), i++){
					switch (i) {
						case 0:
							if(inf_2G) inf_2G->chanload_thr = strtol(temp, 0, 10);
							break;
							
						case 1:
							if(inf_5GL) inf_5GL->chanload_thr = strtol(temp, 0, 10);
							break;

						case 2:
							if(inf_5GH) inf_5GH->chanload_thr = strtol(temp, 0, 10);
							break;

						default:
							DBGPRINT(DEBUG_ERROR, "Error \n");
					}
				}
				memset(keyvalue, 0, sizeof(keyvalue)/sizeof(keyvalue[0]));
			}
			fclose(fp);
		}
	return;
}

int bndstrg_table_init(struct bndstrg_cli_table *table)
{
	int i;
	if (table->bInitialized == TRUE)
		return BND_STRG_SUCCESS;

	memset(table, 0, sizeof(struct bndstrg_cli_table));

#ifdef ROAMING_SUPPORT
	//init roaming list
	initList(&table->roaming_list);
	strcpy(table->eth_iface, IAPP_ETH_IFACE);
	table->eth_port = IAPP_ETH_PORT;
	table->iapp_msg_send_timer = IAPP_MSG_SEND_TIMER;
	table->roaming_entry_timeout = ROAMING_ENTRY_TIMEOUT;
#endif /*ROAMING_SUPPORT*/

	table->Band = 0;
	table->bEnabled = 0;
	table->RssiDiff= BND_STRG_RSSI_DIFF;
	table->RssiLow = BND_STRG_RSSI_LOW;
	table->RSSILowDownSteer = BND_STRG_DOWNSTEER_RSSI;
	table->RSSIHighUpSteer = BND_STRG_UPSTEER_RSSI;
#ifdef VENDOR_FEATURE7_SUPPORT
	table->RSSIDisconnect = BND_STRG_DISCONNECT_RSSI;
	table->BlackListTime = BLACK_LIST_TIME;
#endif
	table->AgeTime = BND_STRG_AGE_TIME;
	table->DormantTime = BND_STRG_DORMANT_TIME;
	table->HoldTime = BND_STRG_HOLD_TIME;
	table->CheckTime = BND_STRG_CHECK_TIME;
	table->bInitialized = TRUE;
	table->RssiCheckCount = RSSI_CHECK_COUNT;
	table->IDLE_RX_Byte_Count = IDLE_RXTX_BYTE_COUNT;
	table->IDLE_TX_Byte_Count = IDLE_RXTX_BYTE_COUNT;
	os_memcpy(table->PriorityList, PriorityList, sizeof(PriorityList));
	table->PriorityListSize = PriorityListSize;
	table->n_diff= BND_STRG_N_DIFF;
	table->op_mode = OP_NO_BNDSTR;
	table->max_steering_size = BND_STRG_MAX_TABLE_SIZE;
	table->assoc_bl_th = BND_STRG_ASSOC_THR;
	table->dynamic_bndstrg_period = BND_STRG_DYNAMIC_STEERING_PERIOD; // sec, min is 3sec
	table->polling_sta_period = 3; // sec, min is 3sec	
	table->debug_only_allow_monitor_addr = fg_only_allow_monitor_addr;
	table->max_steer_count = MAX_STEERING_COUNT;
	table->dwell_time = DWELL_TIME;
	table->max_steer_time_window = MAX_STEER_TIME_WINDOW;
	table->fDynamicSteering = FALSE;
	table->single_band_timeout = BND_STRG_SINGLE_BAND_TIMEOUT;
	table->nvram_table_size = NVRAM_TABLE_SIZE;
	table->nvram_reset = FALSE;
#ifdef VENDOR_IOS_SUPPORT
	table->IOSNumLimit = MAX_IOS_NUM;
#endif
	BndStrg_Update_Default_Param(table);
	{
		u8 bDefault_set = FALSE;
		table->chanload_priority_enabled = FALSE;
		for(i=0; i < table->PriorityListSize; i++)
		{
			if(table->PriorityList[i] < fBND_STRG_PRIORITY_LB_CND_MCS){
				table->BndStrgMode |= PRE_CONNECTION_STEERING;
			} else if((table->PriorityList[i] == fBND_STRG_PRIORITY_DEFAULT_5G) ||
			(table->PriorityList[i] == fBND_STRG_PRIORITY_DEFAULT_2G)){
				bDefault_set = TRUE;
			} else
				table->BndStrgMode |= POST_CONNECTION_STEERING;

			if ((table->PriorityList[i] == fBND_STRG_PRIORITY_LB_CND_CHANLOAD_STATIC) ||
				 (table->PriorityList[i] == fBND_STRG_PRIORITY_LB_CND_CHANLOAD_IDLE) ||
				 (table->PriorityList[i] == fBND_STRG_PRIORITY_LB_CND_CHANLOAD_ACTIVE)
#ifdef VENDOR_FEATURE7_SUPPORT
				 ||(table->PriorityList[i] == fBND_STRG_PRIORITY_DOWNSTEER_CHLOAD_RSSI)
				 ||(table->PriorityList[i] == fBND_STRG_PRIORITY_UPSTEER_CHLOAD_RSSI)
#endif
				){
					table->chanload_priority_enabled = TRUE;
			}
		}
		if((table->BndStrgMode & PRE_CONNECTION_STEERING) && (bDefault_set == FALSE)){
			table->PriorityList[table->PriorityListSize] = fBND_STRG_PRIORITY_DEFAULT_5G;
			table->PriorityListSize ++;
		}
	}
	return BND_STRG_SUCCESS;
}

#ifdef ROAMING_SUPPORT
void handle_iapp_receive_msg(struct bndstrg *bndstrg, struct bndstrg_iapp_frame *iapp_frame, int length)
{
	int i=0, add_status = 0;
	struct bndstrg_iapp_hdr iapp_hdr = {0};
	iapp_hdr.command = iapp_frame->iapp_hdr.command;
	iapp_hdr.length = ntohs(iapp_frame->iapp_hdr.length);
	iapp_hdr.sta_count = iapp_frame->iapp_hdr.sta_count;
	iapp_hdr.sender_ip = ntohl(iapp_frame->iapp_hdr.sender_ip);

	hex_dump("handle_iapp_receive_msg",(unsigned char *)&iapp_hdr,length);

	if(iapp_hdr.command != 1)
	{
		DBGPRINT(DEBUG_ERROR, RED("Receive msg command mismatch\n"));
		return;
	}

	DBGPRINT(DEBUG_TRACE, GRN("Receive msg command:%d, sta_count:%d, hdr_length:%d\n"),
		iapp_hdr.command, iapp_hdr.sta_count, iapp_hdr.length);
	DBGPRINT(DEBUG_TRACE, "handle_iapp_receive_msg> sender IP address (%d.%d.%d.%d)\n",
					SHOW_IP_HTONL(iapp_hdr.sender_ip));

	if(iapp_hdr.length != (length - sizeof(struct bndstrg_iapp_hdr)))
	{
		DBGPRINT(DEBUG_ERROR, RED("Receive msg length mismatch iapp_hdr.length:%d msg_length:%d hdr_size:%d\n"),
										iapp_hdr.length, length, sizeof(struct bndstrg_iapp_hdr));
		return;
	}

	if(!iapp_hdr.sta_count)
	{
		DBGPRINT(DEBUG_TRACE, RED("Receive msg without station list\n"));
		return;
	}

	for(i = 0; i < iapp_hdr.sta_count; i++)
	{
		struct bndstrg_iapp_sta_entry *piapp_sta_entry = NULL, iapp_sta_entry = {};
		piapp_sta_entry = (struct bndstrg_iapp_sta_entry *)&iapp_frame->buf[i * sizeof(struct bndstrg_iapp_sta_entry)];

		//convert network to host and fill sta entry
		memcpy(iapp_sta_entry.addr, piapp_sta_entry->addr, MAC_ADDR_LEN);
		iapp_sta_entry.band = piapp_sta_entry->band;
		iapp_sta_entry.ap_ip_addr = ntohl(piapp_sta_entry->ap_ip_addr);
		iapp_sta_entry.elapsed_connection_time = ntohl(piapp_sta_entry->elapsed_connection_time);
		iapp_sta_entry.control_flags = ntohl(piapp_sta_entry->control_flags);

		//add entry to roaming list
		add_status = AddBsListEntry(&bndstrg->table.roaming_list, &iapp_sta_entry);
		if(add_status > 0)
			bndstrg_update_white_black_list(bndstrg, NULL, iapp_sta_entry.addr, bndstrg_whitelist, 1);
	}
}

static void udp_receive(int sock, void *eloop_ctx, void *sock_ctx)
{
	//struct bndstrg *bndstrg = eloop_ctx;
	unsigned char udpbuf[8192] = {0};
	int left;
	struct sockaddr_in from;
	socklen_t fromlen;
	int max_events = 10;
	struct bndstrg *bndstrg = (struct bndstrg *)sock_ctx;
	struct bndstrg_iapp_frame *iapp_frame;

try_again:
	fromlen = sizeof(from);
	left = recvfrom(sock, udpbuf, sizeof(udpbuf), MSG_DONTWAIT,
			(struct sockaddr *) &from, &fromlen);
	if (left < 0) {
		if (errno != EINTR && errno != EAGAIN)
			DBGPRINT(DEBUG_ERROR, "UDP: recvfrom failed: %s",
				   strerror(errno));
		return;
	}

	if (left > 0)
	{
		DBGPRINT(DEBUG_TRACE, " UDP: %d bytes in udp broadcast message from:%lu, own address:%lu",
						left, (unsigned long)from.sin_addr.s_addr, (unsigned long)bndstrg->AddrOwn.s_addr);

		//discard broadcast from self
		if((unsigned long)from.sin_addr.s_addr != (unsigned long)bndstrg->AddrOwn.s_addr)
		{
			hex_dump("udp_receive::", udpbuf, left);

			//typecast buf to iapp_frame
			iapp_frame = (struct bndstrg_iapp_frame *)udpbuf;
			handle_iapp_receive_msg(bndstrg, iapp_frame, left);
		}
		else
			DBGPRINT(DEBUG_TRACE, " UDP: discard broadcast from self:::from:%lu, own address:%lu \n",
						(unsigned long)from.sin_addr.s_addr, (unsigned long)bndstrg->AddrOwn.s_addr);
	}

	if (--max_events > 0) {
		/*
		 * Try to receive all events in one eloop call in order to
		 * limit race condition on cases where AssocInfo event, Assoc
		 * event, and EAPOL frames are received more or less at the
		 * same time. We want to process the event messages first
		 * before starting EAPOL processing.
		 */
		goto try_again;
	}
}

unsigned int udp_send(
	struct bndstrg *bndstrg,
	unsigned char 	*pPkt,
	unsigned int		PktLen)
{
	struct sockaddr_in AddrMulticast;
	unsigned int Status;

	memset(&AddrMulticast, 0, sizeof(AddrMulticast));
	//IAPP_MEM_ZERO(&AddrMulticast, sizeof(AddrMulticast));
	AddrMulticast.sin_family      = AF_INET;
//	AddrMulticast.sin_addr.s_addr = inet_addr(IAPP_MULTICAST_ADDR);
	AddrMulticast.sin_addr.s_addr = bndstrg->AddrBroadcast.s_addr;
	AddrMulticast.sin_port        = htons(2107);

	hex_dump("udp_buffer:::",pPkt,PktLen);

	if (sendto(bndstrg->SocketUdpSend,
				pPkt,
				PktLen,
				0,
				(struct sockaddr *)&AddrMulticast,
				sizeof(AddrMulticast)) != (PktLen))
	{
		DBGPRINT(DEBUG_TRACE,RED("iapp> Send UDP packet failed!\n"));
		Status = FALSE;
	}
	else
		DBGPRINT(DEBUG_TRACE,GRN("UDP pkt sent\n"));
	/* End of if */
	return Status;
}

//inter-AP udp communication socket init/open

int udp_init(struct bndstrg *bndstrg)
{

	int SockIf, FlgIsLoop, FlgIsReUsed, FlgIsBroadcast, ret = 0;
	struct ifreq ReqIf;
	struct sockaddr_in	AddrUdp;

	strcpy(bndstrg->IfNameEth, "br0");

	memcpy(ReqIf.ifr_name, bndstrg->IfNameEth, IFNAMSIZ);

	SockIf = socket(AF_INET, SOCK_DGRAM, 0);

	if(SockIf < 0)
	{
		DBGPRINT(DEBUG_ERROR, "UDP socket open failed!\n");
		ret = BND_STRG_UDP_INIT_FAILED;
	}

	DBGPRINT(DEBUG_TRACE, "UDP socket open %d!\n", SockIf);

	ioctl(SockIf, SIOCGIFADDR, (int) &ReqIf);

	memcpy(&bndstrg->AddrOwn,
			&((struct sockaddr_in *) &ReqIf.ifr_addr)->sin_addr,
			sizeof(bndstrg->AddrOwn));
	DBGPRINT(DEBUG_OFF, GRN("Inter-AP udp_init> own address (%d.%d.%d.%d)\n"),
				SHOW_IP_HTONL(bndstrg->AddrOwn.s_addr));

	ioctl(SockIf, SIOCGIFBRDADDR, (int) &ReqIf);

	memcpy(&bndstrg->AddrBroadcast,
			&((struct sockaddr_in *) &ReqIf.ifr_addr)->sin_addr,
			sizeof(bndstrg->AddrBroadcast));
	DBGPRINT(DEBUG_OFF, GRN("Inter-AP udp_init> broadcast address (%d.%d.%d.%d)\n"),
				SHOW_IP_HTONL(bndstrg->AddrBroadcast.s_addr));

	ioctl(SockIf, SIOCGIFNETMASK, (int) &ReqIf);

	memcpy(&bndstrg->AddrNetmask,
			&((struct sockaddr_in *) &ReqIf.ifr_addr)->sin_addr,
			sizeof(bndstrg->AddrNetmask));
	DBGPRINT(DEBUG_OFF, GRN("Inter-AP udp_init> network Mask address (%d.%d.%d.%d)\n"),
		SHOW_IP_HTONL(bndstrg->AddrNetmask.s_addr));

	close(SockIf);

	//inter-AP udp communication socket open and registeration
	bndstrg->SocketUdpSend = -1;

	bndstrg->SocketUdpSend = socket(PF_INET, SOCK_DGRAM, 0);

	if(bndstrg->SocketUdpSend < 0)
	{
		DBGPRINT(DEBUG_ERROR, "UDP socket open failed!\n");
		ret = BND_STRG_UDP_INIT_FAILED;
	}

	FlgIsReUsed = 1;
	setsockopt(bndstrg->SocketUdpSend, SOL_SOCKET, SO_REUSEADDR,
				&FlgIsReUsed, sizeof(FlgIsReUsed));

	AddrUdp.sin_family = AF_INET;
	AddrUdp.sin_addr.s_addr = INADDR_ANY;
	AddrUdp.sin_port = ntohs(2107);

	if(bind(bndstrg->SocketUdpSend, (struct sockaddr *)&AddrUdp, sizeof(AddrUdp)) < 0)
	{
		DBGPRINT(DEBUG_ERROR, "UDP socket Bind failed!\n");
		ret = BND_STRG_UDP_INIT_FAILED;
	}

	FlgIsLoop = 0;
	setsockopt(bndstrg->SocketUdpSend, IPPROTO_IP, IP_MULTICAST_LOOP, &FlgIsLoop, sizeof(FlgIsLoop));

	FlgIsBroadcast = 1;
	setsockopt(bndstrg->SocketUdpSend, SOL_SOCKET, SO_BROADCAST, &FlgIsBroadcast, sizeof(FlgIsBroadcast));

	ret = eloop_register_read_sock(bndstrg->SocketUdpSend, udp_receive, NULL, bndstrg);

	return ret;
}

int udp_deinit(struct bndstrg *bndstrg)
{
	if (bndstrg->SocketUdpSend >= 0)
	{
		eloop_unregister_read_sock(bndstrg->SocketUdpSend);
		close(bndstrg->SocketUdpSend);
	}
	return 0;
}
#endif /*ROAMING_SUPPORT*/

int bndstrg_init(struct bndstrg *bndstrg, 
				 struct bndstrg_event_ops *event_ops,
				 int drv_mode,
				 int opmode,
				 int version)
{
	int ret = 0;

	DBGPRINT(DEBUG_TRACE, "%s\n", __FUNCTION__);

	/* Initialze event loop */
	ret = eloop_init();
	
	if (ret)
	{	
		DBGPRINT(DEBUG_OFF, "eloop_register_timeout failed.\n");
		return -1;
	}

	/* use wireless extension */
	bndstrg->drv_ops = &bndstrg_drv_wext_ops;

	bndstrg->event_ops = event_ops;

	bndstrg->version = version;

	bndstrg->drv_data = bndstrg->drv_ops->drv_inf_init(bndstrg, opmode, drv_mode);

	ret = bndstrg_table_init(&bndstrg->table);
	bndstrg->table.bndstrg = bndstrg;//quick path to access bndstrg
	DBGPRINT(DEBUG_TRACE,GRN("::bndstrg=%p,table=%p\n\r"), bndstrg,&bndstrg->table);

#ifdef ROAMING_SUPPORT
	ret = udp_init(bndstrg);
	if (ret)
	{
		DBGPRINT(DEBUG_OFF, "udp init failed.\n");
		return -1;
	}
#endif /*ROAMING_SUPPORT*/

#ifdef BNDSTRG_NVRAM_SUPPORT
	bndstrg->nvram_support = TRUE;
	if(bndstrg->table.nvram_reset == TRUE){
		DBGPRINT(DEBUG_OFF,"Clear NVRAM data\n");
		bndstrg_nvram_clear_all_entry(&bndstrg->table);
	}
#endif
	bndstrg_nvram_read_all(bndstrg);

	if (ret == BND_STRG_SUCCESS)
		ret = eloop_register_timeout(1, 0, bndstrg_periodic_exec, NULL, bndstrg);
	return 0;
}

int bndstrg_deinit(struct bndstrg *bndstrg)
{
    int ret = 0;

    DBGPRINT(DEBUG_TRACE, "\n");

    ret = bndstrg->drv_ops->drv_inf_exit(bndstrg);

    if (ret)
        return -1;

    return 0;
}

static void _bndstrg_terminate(int sig, void *signal_ctx)
{
#ifdef ROAMING_SUPPORT
	struct bndstrg *pbndstrg = (struct bndstrg *)signal_ctx;
#endif /*ROAMING_SUPPORT*/

	DBGPRINT(DEBUG_OFF, "sig[%d]\n",sig);
	if (sig == SIGINT) {
		DBGPRINT(DEBUG_ERROR, "%s[%d]:receive SIGINT\n",__func__,__LINE__);
	}
	if (sig == SIGTERM) {
		DBGPRINT(DEBUG_ERROR, RED("%s[%d]:receive SIGTERM, kill daemon\n"),__func__,__LINE__);
#ifdef ROAMING_SUPPORT
		ClearBsList(&pbndstrg->table.roaming_list);
#endif /*ROAMING_SUPPORT*/

		eloop_terminate();
	}
}

void bndstrg_run(struct bndstrg *bndstrg)
{
    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg->ctrl_iface;
    struct bndstrg_iface *inf;
    u8 i;

	DBGPRINT(DEBUG_OFF, GRN("%s[%d]:start\n"),__func__,__LINE__);
	eloop_register_signal_terminate(_bndstrg_terminate, bndstrg);

	eloop_run();

	/* before daemon leaved, make sure all bandstrg disable */
    for(i=0; i < ctrl_iface->Size; i++)
    {
        inf = &ctrl_iface->inf[i];
        if(inf->bValid && inf->bInfReady)
           bndstrg_onoff(bndstrg, (char*)inf->ucIfName, 0, 0);
    }
	DBGPRINT(DEBUG_OFF, GRN("%s[%d]:exit\n"),__func__,__LINE__);
}

void bndstrg_check_steering_limit(struct bndstrg_cli_table *table,struct bndstrg_cli_entry *entry)
{
	u8 start_idx,end_idx,i;
	end_idx = entry->CliSteerInfo.end_idx;
	start_idx = end_idx; /* end index will be max_steer_count -1*/
	INC(start_idx,table->max_steer_count);
	for(i=0;i<table->max_steer_count;i++)
	{
		if(bndstrg_get_elapsed_time(entry->CliSteerInfo.steer_tp[start_idx]) < (table->max_steer_time_window))
			break;
		else
		{
			INC(start_idx,table->max_steer_count);
		}
	}
	if(i == 0)
		return;
	else
	{
		if(end_idx > start_idx)
			entry->total_steered_cnt = end_idx - start_idx + 1;
		else if (end_idx < start_idx)
			entry->total_steered_cnt -= (start_idx - end_idx - 1);
		else
			entry->total_steered_cnt = 0;
	}
	return;
}

void bndstrg_inf_init(struct bndstrg_iface *inf)
{
	if(IS_5G_H_BAND(inf->Band)){
		inf->min_nss_thr = BND_STRG_NSS_THR_5GH;
		inf->min_rssi_thr = BND_STRG_MIN_RSSI_5GH;
		inf->chanload_thr = BND_STRG_CHANLOAD_THR_5GH;
		inf->chanload_heavy_max_cnt = BND_STRG_CHANLOAD_MAX_CNT_5G_H;
	}
	
	if(IS_5G_L_BAND(inf->Band)){	
		inf->min_nss_thr = BND_STRG_NSS_THR_5GL;
		inf->min_rssi_thr = BND_STRG_MIN_RSSI_5GL;
		inf->chanload_thr = BND_STRG_CHANLOAD_THR_5GL;
		inf->chanload_heavy_max_cnt = BND_STRG_CHANLOAD_MAX_CNT_5G_L;
	}

	if(IS_2G_BAND(inf->Band)){
		inf->min_nss_thr = BND_STRG_NSS_THR_2G;
		inf->min_rssi_thr = BND_STRG_MIN_RSSI_2G;
		inf->chanload_thr = BND_STRG_CHANLOAD_THR_2G;
		inf->chanload_heavy_max_cnt = BND_STRG_CHANLOAD_MAX_CNT_2G;
	}
	return;
}

struct bndstrg_iface * bndstrg_get_interface_by_channel(
    struct bndstrg_ctrl_iface *ctrl_iface,  
    u8 channel)
{
    u8 i,cnt;
    struct bndstrg_iface *inf = NULL;
    for(i=0,cnt=0; i<ctrl_iface->Size; i++)
    {
        inf = &ctrl_iface->inf[i];
        if(inf->bValid)
        {
        	cnt++;
        	if(inf->bInfReady && (inf->Channel == channel))
            	return inf;
        }
    }
    return NULL;
}

struct bndstrg_cli_entry * bndstrg_get_old_entry(
	struct bndstrg *bndstrg,
	struct bndstrg_iface *inf)
{
	u8 i, elapsed_time = 0, max_elapsed_time = 0, count=0;
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bndstrg_cli_entry * entry = NULL, *temp_entry = NULL;
	u32 AuthReqCount = 0;

	for(i=0; i<table->max_steering_size; i++){
		temp_entry = & table->Entry[i];
		if (temp_entry->bValid == TRUE)
		{
			count ++;
			elapsed_time = bndstrg_get_elapsed_time(temp_entry->tp);
			if(temp_entry->bConnStatus ||
				temp_entry->Operation_steered ||
				(inf && inf->Band != temp_entry->band) ||
				(!IS_BND_STRG_DUAL_BAND_CLIENT(temp_entry->Control_Flags) &&
				elapsed_time < (table->CheckTime + 20)))
				continue;
			if(elapsed_time > max_elapsed_time){
				max_elapsed_time = elapsed_time;
				entry = temp_entry;
			}
		}
		if(count >= table->Size)
			break;
	}
	if(!entry){
		max_elapsed_time = 0;
		count = 0;
		for(i=0; i<table->max_steering_size; i++){
			temp_entry = & table->Entry[i];
			if (temp_entry->bValid == TRUE)
			{
				count ++;
				if(temp_entry->bConnStatus || temp_entry->Operation_steered || (inf && !(inf->Band & temp_entry->band)))
					continue;
				AuthReqCount = temp_entry->statistics[0].AuthReqCount + temp_entry->statistics[1].AuthReqCount + temp_entry->statistics[2].AuthReqCount;
				if (AuthReqCount == 0) {
					elapsed_time = bndstrg_get_elapsed_time(temp_entry->tp);
					if(elapsed_time > max_elapsed_time){
						max_elapsed_time = elapsed_time;
						entry = temp_entry;
					}
				}
			}
			if(count >= table->Size)
				break;				
		}
	}
	if(!entry){
		if(inf)
		bndstrg_stop(bndstrg, BNDSTRG_LIMIT);
		else
			bndstrg_stop(bndstrg, BNDSTRG_DAEMON_TBL_FULL);
	}
	return entry;
};

void bndstrg_stop(struct bndstrg *bndstrg, u8 reason_code)
{
	u8 i;
	struct bndstrg_iface *inf;
	DBGPRINT(DEBUG_OFF,RED("<%s>\n"),bndstrg_stop_str[reason_code]);
	for(i = 0; i < bndstrg->ctrl_iface.Size; i++)
	{
    	inf = &bndstrg->ctrl_iface.inf[i];
    	if(inf->bValid && inf->bInfReady)
       	bndstrg_onoff(bndstrg, (char*)inf->ucIfName, 0, 0);
	}
	_bndstrg_event_table_info(bndstrg);
	for (i = 0; i < bndstrg->table.max_steering_size; i++)
	{
		if(bndstrg->table.Entry[i].bValid)
			_bndstrg_print_entry(&bndstrg->table, &bndstrg->table.Entry[i]);
	}
	bndstrg->table.fStopBndstrg = TRUE;
	return;
}

void bndstrg_cli_status_rsp (struct bndstrg *bndstrg, struct bnd_msg_cli_status_rsp *cli_status_rsp)
{
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct data_avg *data_tx_util=NULL,*data_rx_util=NULL,*data_tx_byte=NULL,*data_rx_byte=NULL;
	u32 tx_util=0,rx_util=0;
	int update_stat_idx = 0;
	struct bndstrg_cli_entry *entry = &table->Entry[cli_status_rsp->TableIndex];
	if(!entry || !entry->bValid) {
		DBGPRINT(DEBUG_ERROR,"[%d]: entry not found = %02x:%02x:%02x:%02x:%02x:%02x TableIdx:%d\n",
			__LINE__, PRINT_MAC(cli_status_rsp->Addr), cli_status_rsp->TableIndex);
		return ;
	}
	if(entry->state != ENTRY_ASSOC_RCD)
	{
		DBGPRINT(DEBUG_ERROR,"[%d]: entry state is not assoc [state:%s] = %02x:%02x:%02x:%02x:%02x:%02x TableIdx:%d\n",
			__LINE__, entry_state[entry->state], PRINT_MAC(cli_status_rsp->Addr), cli_status_rsp->TableIndex);
		return ;
	}
#ifdef BND_STRG_QA					
	BND_STRG_PRINTQAMSG(table, entry, 
						YLW("Got cli status rsp. return code = %u,"
						"Addr = %02x:%02x:%02x:%02x:%02x:%02x, RSSI=%d,tx/rx PHY RATE=%d/%d, tx/rx TP=%u/%u Mbps"
						"  tx/rx packets = %lu/%lu tx/rx byte = %u/%u\n"),
						cli_status_rsp->ReturnCode,
						PRINT_MAC(cli_status_rsp->Addr),
						cli_status_rsp->data_Rssi,
						cli_status_rsp->data_tx_Rate,
						cli_status_rsp->data_rx_Rate,
						(unsigned int)(cli_status_rsp->data_tx_Byte >> 17),
						(unsigned int)(cli_status_rsp->data_rx_Byte >> 17),
						(long unsigned int)cli_status_rsp->data_tx_packets,
						(long unsigned int)cli_status_rsp->data_rx_packets,
						(unsigned int)cli_status_rsp->data_tx_Byte,
						(unsigned int)cli_status_rsp->data_rx_Byte); 
#endif		
	if(IS_2G_BAND(entry->band))
		update_stat_idx = 0;
	else if(IS_5G_L_BAND(entry->band))
		update_stat_idx = 1;
	else if(IS_5G_H_BAND(entry->band))
		update_stat_idx = 2;
	else
	{
		DBGPRINT(DEBUG_OFF,"entry band[%d] not correct %02x:%02x:%02x:%02x:%02x:%02x TableIdx:%d\n",
			entry->band,PRINT_MAC(entry->Addr), cli_status_rsp->TableIndex);
	}
	data_tx_util = &entry->statistics[update_stat_idx].data_tx_util;
	data_rx_util = &entry->statistics[update_stat_idx].data_rx_util;
	data_tx_byte = &entry->statistics[update_stat_idx].data_tx_byte;
	data_rx_byte = &entry->statistics[update_stat_idx].data_rx_byte;
	
	entry->statistics[update_stat_idx].got_cli_status = 1;
	/*update Data rssi related info*/
	entry->statistics[update_stat_idx].data_Rssi = cli_status_rsp->data_Rssi;

#ifdef VENDOR_FEATURE7_SUPPORT
	if (cli_status_rsp->data_Rssi < table->RSSIDisconnect){
		if(entry->low_rssi_disconnect_cnt < table->RssiCheckCount)
			entry->low_rssi_disconnect_cnt ++;
	} else if (cli_status_rsp->data_Rssi < table->RSSILowDownSteer){
		if(entry->low_rssi_bad_cnt < table->RssiCheckCount)
			entry->low_rssi_bad_cnt ++;
		entry->low_rssi_disconnect_cnt = 0;
	}	else{
		entry->low_rssi_bad_cnt = 0;
		entry->low_rssi_disconnect_cnt = 0;
	}
#else
	if(cli_status_rsp->data_Rssi < table->RSSILowDownSteer){
		if(entry->low_rssi_bad_cnt < table->RssiCheckCount)
			entry->low_rssi_bad_cnt ++;
	}else {
		entry->low_rssi_bad_cnt = 0;
	}
#endif

	if(cli_status_rsp->data_Rssi > table->RSSIHighUpSteer){
		if(entry->good_rssi_cnt < table->RssiCheckCount)
			entry->good_rssi_cnt ++;
	} else{ 
		entry->good_rssi_cnt = 0;
	}

	/*update  MCS info*/
	entry->statistics[update_stat_idx].data_tx_mcs = cli_status_rsp->data_tx_mcs;
	entry->statistics[update_stat_idx].data_rx_mcs = cli_status_rsp->data_rx_mcs;

	if(cli_status_rsp->data_tx_mcs < LOW_MCS_IDX)
		entry->tx_mcs_bad_cnt ++ ;
	else
		entry->tx_mcs_bad_cnt = 0;
	
	if(cli_status_rsp->data_rx_mcs < LOW_MCS_IDX)
		entry->rx_mcs_bad_cnt ++ ;
	else
		entry->rx_mcs_bad_cnt = 0;
	
	entry->statistics[update_stat_idx].data_tx_Rate = cli_status_rsp->data_tx_Rate;
	entry->statistics[update_stat_idx].data_rx_Rate = cli_status_rsp->data_rx_Rate;
	entry->statistics[update_stat_idx].data_tx_TP = cli_status_rsp->data_tx_Byte >> 17; // Mbps
	entry->statistics[update_stat_idx].data_rx_TP = cli_status_rsp->data_rx_Byte >> 17; // Mbps
	entry->statistics[update_stat_idx].data_tx_Phymode = cli_status_rsp->data_tx_Phymode;
	entry->statistics[update_stat_idx].data_rx_Phymode = cli_status_rsp->data_rx_Phymode;
	entry->statistics[update_stat_idx].data_tx_bw = cli_status_rsp->data_tx_bw;
	entry->statistics[update_stat_idx].data_rx_bw = cli_status_rsp->data_rx_bw;
	entry->statistics[update_stat_idx].data_tx_sgi = cli_status_rsp->data_tx_sgi;
	entry->statistics[update_stat_idx].data_rx_sgi = cli_status_rsp->data_rx_sgi;
	entry->statistics[update_stat_idx].data_tx_stbc = cli_status_rsp->data_tx_stbc;
	entry->statistics[update_stat_idx].data_rx_stbc = cli_status_rsp->data_rx_stbc;
	entry->statistics[update_stat_idx].data_tx_ant = cli_status_rsp->data_tx_ant;
	entry->statistics[update_stat_idx].data_rx_ant = cli_status_rsp->data_rx_ant;
	if ((entry->statistics[update_stat_idx].data_tx_packets != cli_status_rsp->data_tx_packets) ||
		(entry->statistics[update_stat_idx].data_rx_packets != cli_status_rsp->data_rx_packets))
	{
		entry->statistics[update_stat_idx].data_tx_packets = cli_status_rsp->data_tx_packets;
		get_current_system_tick(&entry->activity_tp);
	}
	entry->statistics[update_stat_idx].data_tx_packets = cli_status_rsp->data_tx_packets;
	entry->statistics[update_stat_idx].data_rx_packets = cli_status_rsp->data_rx_packets;
	
	if(entry->statistics[update_stat_idx].data_tx_Rate !=0)
		tx_util = entry->statistics[update_stat_idx].data_tx_TP*100/entry->statistics[update_stat_idx].data_tx_Rate;

	if(entry->statistics[update_stat_idx].data_rx_Rate !=0)
		rx_util = entry->statistics[update_stat_idx].data_rx_TP*100/entry->statistics[update_stat_idx].data_rx_Rate;

	data_tx_util->avg += tx_util;
	data_tx_util->avg -= (data_tx_util->data[data_tx_util->idx]);
	data_tx_util->data[data_tx_util->idx] = tx_util;
	INC(data_tx_util->idx,BND_STRG_DATA_SAMPLE);
	data_rx_util->avg += rx_util;
	data_rx_util->avg -= (data_rx_util->data[data_rx_util->idx]);
	data_rx_util->data[data_rx_util->idx] = rx_util;
	INC(data_rx_util->idx,BND_STRG_DATA_SAMPLE);

	data_tx_byte->avg += cli_status_rsp->data_tx_Byte;
	data_tx_byte->avg -= (data_tx_byte->data[data_tx_byte->idx]);
	data_tx_byte->data[data_tx_byte->idx] = cli_status_rsp->data_tx_Byte;
	INC(data_tx_byte->idx,BND_STRG_DATA_SAMPLE);
	data_rx_byte->avg += cli_status_rsp->data_rx_Byte;
	data_rx_byte->avg -= (data_rx_byte->data[data_rx_byte->idx]);
	data_rx_byte->data[data_rx_byte->idx] = cli_status_rsp->data_rx_Byte;
	INC(data_rx_byte->idx,BND_STRG_DATA_SAMPLE);
	return;
}

void bndstrg_update_probe_info(	struct bndstrg *bndstrg,
										struct bndstrg_iface *inf,
										struct bndstrg_cli_entry *entry,
										struct bnd_msg_cli_event *cli_event)
{
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bnd_msg_cli_probe *cli_probe = &cli_event->data.cli_probe;
	signed char	*rssi = cli_probe->Rssi;
	u8	Nss = cli_probe->Nss;			
	s8 	MaxRssi = rssi[0], i;

#ifdef BND_STRG_QA
	BND_STRG_PRINTQAMSG(table, entry," [%s] %02x:%02x:%02x:%02x:%02x:%02x, Band:%s, Channel:%d Probe, rssi = %hhd/%hhd/%hhd/%hhd HTCap %s, VHTCap %s, Nss %d\n",
			inf->ucIfName,PRINT_MAC(cli_event->Addr),bndstrg_get_entry_band(cli_event->Band),cli_event->Channel, rssi[0], rssi[1], rssi[2], rssi[3], 
			(cli_probe->bAllowStaConnectInHt == 1 ? "TRUE":"FALSE"), (cli_probe->bVHTCapable == 1 ? "TRUE":"FALSE"), Nss);
#endif /* BND_STRG_QA */

	for ( i = 1; i < 4; i++)
	{
		if (rssi[i] && (rssi[i] < 0))
			MaxRssi = max(MaxRssi, rssi[i]);
	}

	bndstrg_update_entry_statistics_control_flags(bndstrg, entry, inf, inf->Band, MaxRssi, APMT2_PEER_PROBE_REQ);

	if (entry->AssocProbeInfoMissMatch == FALSE){
		if (!(cli_probe->bAllowStaConnectInHt == TRUE)){
			if(IS_2G_BAND(inf->Band))
				entry->Control_Flags |= fBND_STRG_CLIENT_NOT_SUPPORT_HT_2G;
			else if(IS_5G_L_BAND(inf->Band))
				entry->Control_Flags |= fBND_STRG_CLIENT_NOT_SUPPORT_HT_L5G;
			else if(IS_5G_H_BAND(inf->Band))
				entry->Control_Flags |= fBND_STRG_CLIENT_NOT_SUPPORT_HT_H5G;
		}
		if (cli_probe->bVHTCapable == TRUE){
			entry->Control_Flags |= fBND_STRG_CLIENT_SUPPORT_VHT;
		}
		entry->Nss = Nss;
	}

	/* For IOS immediately connect */
	if (cli_probe->bIosCapable == TRUE && 
		entry->Operation_steered == FALSE && entry->bConnStatus == FALSE)
	{
		entry->bAllowStaConnectImmediately = TRUE;
	}

	return;
}
#if 0
void bndstrg_update_auth_info(	struct bndstrg *bndstrg,
										struct bndstrg_iface *inf,
										struct bndstrg_cli_entry *entry,
										struct bnd_msg_cli_event *cli_event)
{
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bnd_msg_cli_auth *cli_auth = &cli_event->data.cli_auth;
	signed char	*rssi = cli_auth->Rssi;
	s8	MaxRssi = rssi[0], i;

#ifdef BND_STRG_QA
	BND_STRG_PRINTQAMSG(table, entry," [%s] %02x:%02x:%02x:%02x:%02x:%02x, Band:%s, Channel:%d Auth, rssi = %hhd/%hhd/%hhd/%hhd\n",
			inf->ucIfName,PRINT_MAC(cli_event->Addr), bndstrg_get_entry_band(cli_event->Band), cli_event->Channel, rssi[0], rssi[1], rssi[2], rssi[3]);
#endif /* BND_STRG_QA */

	for ( i = 1; i < 4; i++)
	{
		if (rssi[i] && (rssi[i] < 0))
			MaxRssi = max(MaxRssi, rssi[i]);
	}			
	bndstrg_update_entry_statistics_control_flags(bndstrg, entry, inf, inf->Band, MaxRssi, APMT2_PEER_AUTH_REQ);
	return;
}
#endif

void bndstrg_update_auth_info(	struct bndstrg *bndstrg,
										struct bndstrg_iface *inf,
										struct bndstrg_cli_entry *entry,
										struct bnd_msg_cli_event *cli_event)
{
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bnd_msg_cli_auth *cli_auth = &cli_event->data.cli_auth;
	signed char *rssi = cli_auth->Rssi;
	s8	MaxRssi = rssi[0], i;
	struct bndstrg_iface *prev_inf = NULL;
	u8 STA_Connect_Flag=0;
#ifdef BND_STRG_QA
	BND_STRG_PRINTQAMSG(table, entry," inf->ucIfName=[%s] %02x:%02x:%02x:%02x:%02x:%02x, Band:%s, Channel:%d Auth, rssi = %hhd/%hhd/%hhd/%hhd\n",
			inf->ucIfName,PRINT_MAC(cli_event->Addr), bndstrg_get_entry_band(cli_event->Band), cli_event->Channel, rssi[0], rssi[1], rssi[2], rssi[3]);
#endif /* BND_STRG_QA */

	for ( i = 1; i < 4; i++)
	{
		if (rssi[i] && (rssi[i] < 0))
			MaxRssi = max(MaxRssi, rssi[i]);
	}	

#ifdef BND_STRG_QA
		BND_STRG_PRINTQAMSG(table, entry,"entry->Addr=%02x:%02x:%02x:%02x:%02x:%02x entry->bConnStatus=%d\n",
				PRINT_MAC(entry->Addr),entry->bConnStatus);
#endif /* BND_STRG_QA */	

#ifdef BND_STRG_QA
		BND_STRG_PRINTQAMSG(table, entry,"%02x:%02x:%02x:%02x:%02x:%02x, table->Band:%s,entry->band:%d entry->band:%s entry->Channel:%d table->BndStrgMode=%d\n",
				PRINT_MAC(cli_event->Addr), bndstrg_get_entry_band(table->Band), entry->band,bndstrg_get_entry_band(entry->band), entry->Channel, 
				table->BndStrgMode);
#endif /* BND_STRG_QA */		

	prev_inf = bndstrg_get_interface(&bndstrg->ctrl_iface, NULL, entry->band, TRUE);
	if(prev_inf) 
	{
#ifdef BND_STRG_QA
		BND_STRG_PRINTQAMSG(table, entry," prev_inf->ucIfName=[%s] %02x:%02x:%02x:%02x:%02x:%02x\n",
				prev_inf->ucIfName,PRINT_MAC(cli_event->Addr));
#endif /* BND_STRG_QA */
	}

	
	if((table->BndStrgMode & PRE_CONNECTION_STEERING) && (!entry->bConnStatus)) //处于关联阶段
	{
		if(entry->band == BAND_INVALID)
		{
			entry->Recv_AuthTime_1++;
#ifdef BND_STRG_QA
			BND_STRG_PRINTQAMSG(table, entry," %02x:%02x:%02x:%02x:%02x:%02x Recv_AuthTime_1=%d\n",
					PRINT_MAC(cli_event->Addr),entry->Recv_AuthTime_1);
#endif /* BND_STRG_QA */	
				
		}
		if(entry->Recv_AuthTime_1 >= 1)//坚持连接当前band
		{
			entry->Recv_AuthTime_1 =0;
			STA_Connect_Flag=1;
			if(table->op_mode == OP_DUALBAND)//dualband，5G=5GL
			{
				if(cli_event->Band == BAND_5G)
					cli_event->Band =BAND_5G_L;
			}
			entry->band = cli_event->Band;
			entry->state = ENTRY_READY_TO_ASSOC;
#ifdef BND_STRG_QA
				BND_STRG_PRINTQAMSG(table, entry," STA connect	entry->band:%s %02x:%02x:%02x:%02x:%02x:%02x\n",
					bndstrg_get_entry_band(entry->band),PRINT_MAC(cli_event->Addr));
#endif /* BND_STRG_QA */
			bndstrg_accessible_cli(bndstrg, inf, entry, CLI_ADD);//当前接口添加table
		}
		
		{
			if(table->op_mode == OP_DUALBAND)//dualband，5G=5GL
			{
				if(cli_event->Band == BAND_5G)
					cli_event->Band =BAND_5G_L;
			}
			if((cli_event->Band != entry->band) && (entry->band != BAND_INVALID) && (STA_Connect_Flag == 0))
			{
				entry->Recv_AuthTime++;
			}
			else
				entry->Recv_AuthTime=0;
#ifdef BND_STRG_QA
			BND_STRG_PRINTQAMSG(table, entry," %02x:%02x:%02x:%02x:%02x:%02x Recv_AuthTime=%d\n",
					PRINT_MAC(cli_event->Addr),entry->Recv_AuthTime);
#endif /* BND_STRG_QA */		
			//printf("--------> Recv_AuthTime=%d\n",Recv_AuthTime);
			if(entry->Recv_AuthTime >= 1)
			{
				entry->Recv_AuthTime=0;
				if(prev_inf) 
				{
	
					if(prev_inf->ucIfName != inf->ucIfName)//接口不一致
					{
#ifdef BND_STRG_QA
					BND_STRG_PRINTQAMSG(table, entry," delete[%s] add[%s] %02x:%02x:%02x:%02x:%02x:%02x\n",
						prev_inf->ucIfName,inf->ucIfName,PRINT_MAC(cli_event->Addr));
#endif /* BND_STRG_QA */
						//if(cli_event->Band == BAND_5G)
						//	cli_event->Band =BAND_5G_L;
						
						entry->band = cli_event->Band;
#ifdef BND_STRG_QA
						BND_STRG_PRINTQAMSG(table, entry," entry->band:%s %02x:%02x:%02x:%02x:%02x:%02x\n",
							bndstrg_get_entry_band(entry->band),PRINT_MAC(cli_event->Addr));
#endif /* BND_STRG_QA */					
						bndstrg_accessible_cli(bndstrg, prev_inf, entry, CLI_DEL);//删除旧band的table
						bndstrg_accessible_cli(bndstrg, inf, entry, CLI_ADD);//当前接口添加table
					}
				}
				
			}
		}
	}
		
	bndstrg_update_entry_statistics_control_flags(bndstrg, entry, inf, inf->Band, MaxRssi, APMT2_PEER_AUTH_REQ);
	return;
}

void bndstrg_update_btm_status(struct bndstrg *bndstrg, struct bndstrg_cli_entry *entry)
{
	struct bndstrg_iface *inf = NULL;
	if(entry->btm_info.BTMState != btm_rsp_rx || entry->btm_info.BTMRsp != 0)
	{
#ifdef VENDOR_FEATURE7_SUPPORT
		bndstrg_update_white_black_list(bndstrg, NULL, entry->Addr, bndstrg_blacklist, 1);
		entry->BlackListTimer = bndstrg->table.BlackListTime;
		inf = bndstrg_get_interface(&bndstrg->ctrl_iface, NULL, entry->connected_band, TRUE);
		if(inf) bndstrg_accessible_cli(bndstrg, inf, entry, CLI_DEL);
#else
		/*BTM rejected clear target band info from entry*/
		switch (entry->band){
			case BAND_2G:
				entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_2G);
			break;
			case BAND_5G_L:
				entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_L5G);
			break;
			case BAND_5G_H:
				entry->Control_Flags &= (~fBND_STRG_CLIENT_ALLOW_TO_CONNET_H5G);
			break;
		}
		if(entry->steered){
			entry->steered = 0;
			entry->tx_mcs_bad_cnt = 0;
			entry->rx_mcs_bad_cnt = 0;
			entry->low_rssi_bad_cnt = 0;
			entry->good_rssi_cnt = 0;
			if(entry->Operation_steered == TRUE){
				entry->Operation_steered = FALSE;
				entry->Operation_steered_tick = 0;
			}
		}
		inf = bndstrg_get_interface(&bndstrg->ctrl_iface, NULL, entry->band, TRUE);
		if(inf) bndstrg_accessible_cli(bndstrg, inf, entry, CLI_DEL);
		entry->band = entry->connected_band;
		entry->state = ENTRY_ASSOC_RCD;
#endif
	} 
	entry->btm_info.BTMState = btm_init;
	return;
}

#ifdef ROAMING_SUPPORT
void initList(LIST_HEADER *pList)
{
	pList->pHead = pList->pTail = NULL;
	pList->size = 0;
	return;
}

void insertTailList(LIST_HEADER *pList, RT_LIST_ENTRY *pEntry)
{
	pEntry->pNext = NULL;
	if (pList->pTail)
		pList->pTail->pNext = pEntry;
	else
		pList->pHead = pEntry;
	pList->pTail = pEntry;
	pList->size++;

	return;
}

RT_LIST_ENTRY *delEntryList(LIST_HEADER *pList, RT_LIST_ENTRY *pEntry)
{
	RT_LIST_ENTRY *pCurEntry;
	RT_LIST_ENTRY *pPrvEntry;

	if(pList->pHead == NULL)
		return NULL;

	if(pEntry == pList->pHead)
	{
		pCurEntry = pList->pHead;
		pList->pHead = pCurEntry->pNext;

		if(pList->pHead == NULL)
			pList->pTail = NULL;

		pList->size--;
		return pCurEntry;
	}

	pPrvEntry = pList->pHead;
	pCurEntry = pPrvEntry->pNext;
	while(pCurEntry != NULL)
	{
		if (pEntry == pCurEntry)
		{
			pPrvEntry->pNext = pCurEntry->pNext;

			if(pEntry == pList->pTail)
				pList->pTail = pPrvEntry;

			pList->size--;
			break;
		}
		pPrvEntry = pCurEntry;
		pCurEntry = pPrvEntry->pNext;
	}

	return pCurEntry;
}

RT_LIST_ENTRY *removeHeadList(LIST_HEADER *pList)
{
	RT_LIST_ENTRY *pNext;
	RT_LIST_ENTRY *pEntry;

	pEntry = pList->pHead;
	if (pList->pHead != NULL)
	{
		pNext = pList->pHead->pNext;
		pList->pHead = pNext;
		if (pNext == NULL)
			pList->pTail = NULL;
		pList->size--;
	}
	return pEntry;
}


PBS_LIST_ENTRY FindBsListEntry(
	PLIST_HEADER pBsList,
	struct bndstrg_iapp_sta_entry *sta_entry)
{
	PBS_LIST_ENTRY 	pBsListEntry = NULL;
	RT_LIST_ENTRY		        *pListEntry = NULL;

	pListEntry = pBsList->pHead;
	pBsListEntry = (PBS_LIST_ENTRY)pListEntry;
	while (pBsListEntry != NULL)
	{
		if (!os_memcmp(pBsListEntry->iapp_sta_entry.addr, sta_entry->addr, MAC_ADDR_LEN))
			return pBsListEntry;
		pListEntry = pListEntry->pNext;
		pBsListEntry = (PBS_LIST_ENTRY)pListEntry;
	}

	return NULL;
}

int AddBsListEntry(
	PLIST_HEADER pBsList,
	struct bndstrg_iapp_sta_entry *sta_entry)
{
	PBS_LIST_ENTRY pBsListEntry = NULL;

	pBsListEntry = FindBsListEntry(pBsList, sta_entry);

	/* the Entry already exist */
	if (pBsListEntry)
	{
		if(pBsListEntry->iapp_sta_entry.ap_ip_addr == sta_entry->ap_ip_addr)
		{
			memcpy(&pBsListEntry->iapp_sta_entry, sta_entry, sizeof(struct bndstrg_iapp_sta_entry));
			os_get_time(&pBsListEntry->last_update);
		}
		else		//
		{
			if(pBsListEntry->iapp_sta_entry.elapsed_connection_time > sta_entry->elapsed_connection_time)
			{
				memcpy(&pBsListEntry->iapp_sta_entry, sta_entry, sizeof(struct bndstrg_iapp_sta_entry));
			}
			os_get_time(&pBsListEntry->last_update);
		}
		return 0;
	}
	else
	{
		/* Add new Entry */
		pBsListEntry = os_malloc(sizeof(BS_LIST_ENTRY));
		if (pBsListEntry)
		{
			os_memset(pBsListEntry, 0, sizeof(BS_LIST_ENTRY));
			pBsListEntry->pNext = NULL;
			memcpy(&pBsListEntry->iapp_sta_entry, sta_entry, sizeof(struct bndstrg_iapp_sta_entry));
			os_get_time(&pBsListEntry->last_update);
			insertTailList(pBsList, (RT_LIST_ENTRY *)pBsListEntry);
			DBGPRINT(DEBUG_TRACE, "AddBsListEntry: New %02x:%02x:%02x:%02x:%02x:%02x added in BsList:size::%d. \n",
                                                        PRINT_MAC(pBsListEntry->iapp_sta_entry.addr), pBsList->size);
			return 1;
		}
		else
		{
			DBGPRINT(DEBUG_ERROR, "Error in alloc mem in AddBsListEntry Function.\n");
			return -1;
		}
	}
	//should not reach here
	return -1;

}


void DelBsListEntry(
	PLIST_HEADER pBsList,
	struct bndstrg_iapp_sta_entry *sta_entry)
{
	PBS_LIST_ENTRY pBsListEntry = NULL;
	RT_LIST_ENTRY	*pListEntry = NULL, *pDelEntry = NULL;

	pBsListEntry = FindBsListEntry(pBsList, sta_entry);

	if (!pBsListEntry)
	{
		/* the Entry already exist */
		DBGPRINT(DEBUG_TRACE, "DelBsListEntry: Not Found %02x:%02x:%02x:%02x:%02x:%02x in BsList.\n",
			PRINT_MAC(sta_entry->addr));
		return;
	}
	else
	{
		pListEntry = (RT_LIST_ENTRY *)pBsListEntry;
		/* Delete the Entry */
		pDelEntry = delEntryList(pBsList, pListEntry);
		DBGPRINT(DEBUG_TRACE,"DelBsListEntry: Sta %02x:%02x:%02x:%02x:%02x:%02x deleted from BsList:size::%d. \n",
                                                        PRINT_MAC(pBsListEntry->iapp_sta_entry.addr), pBsList->size);
		os_free(pDelEntry);
	}
	return;
}

void ClearBsList(
	PLIST_HEADER pBsList)
{
	RT_LIST_ENTRY     *pListEntry = NULL;
	PLIST_HEADER    pListHeader = pBsList;

	if (pListHeader->size == 0)
	{
		DBGPRINT(DEBUG_TRACE, ("ClearBsList: BsList already Empty.\n"));
		return;
	}

	pListEntry = pListHeader->pHead;
	while (pListEntry != NULL)
	{
		/*Remove ListEntry from Header*/
		removeHeadList(pListHeader);
		os_free(pListEntry);
		pListEntry = pListHeader->pHead;
	}

	DBGPRINT(DEBUG_TRACE, "ClearBsList: Clean BsList.\n");
}
#endif /*ROAMING_SUPPORT*/


struct bndstrg_cli_entry * bndstrg_get_old_driver_entry(
	struct bndstrg *bndstrg,
	struct bndstrg_iface *inf, 
	unsigned char *pAddr)
{
	u8 i, elapsed_time = 0, max_elapsed_time = 0, count=0;
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bndstrg_cli_entry * entry = NULL, *temp_entry = NULL;
	u32 AuthReqCount = 0;

	for(i=0; i<table->max_steering_size; i++){
		temp_entry = & table->Entry[i];
		if (temp_entry->bValid == TRUE)
		{
			count ++;
			elapsed_time = bndstrg_get_elapsed_time(temp_entry->tp);
			if(temp_entry->bConnStatus ||
				temp_entry->Operation_steered ||
				(inf && inf->Band != temp_entry->band) ||
				(!IS_BND_STRG_DUAL_BAND_CLIENT(temp_entry->Control_Flags) &&
				elapsed_time < (table->CheckTime + 20)) ||
				MAC_ADDR_EQUAL(temp_entry->Addr, pAddr) || temp_entry->band == BAND_INVALID)
				continue;
			if(elapsed_time > max_elapsed_time){
				max_elapsed_time = elapsed_time;
				entry = temp_entry;
			}
		}
		if(count >= table->Size)
			break;
	}
	if(!entry){
		max_elapsed_time = 0;
		count = 0;
		for(i=0; i<table->max_steering_size; i++){
			temp_entry = & table->Entry[i];
			if (temp_entry->bValid == TRUE)
			{
				count ++;
				if(temp_entry->bConnStatus || temp_entry->Operation_steered ||
					(inf && !(inf->Band & temp_entry->band)) ||
					temp_entry->band == BAND_INVALID ||MAC_ADDR_EQUAL(temp_entry->Addr, pAddr))
					continue;
				AuthReqCount = temp_entry->statistics[0].AuthReqCount + temp_entry->statistics[1].AuthReqCount + temp_entry->statistics[2].AuthReqCount;
				if (AuthReqCount == 0) {
					elapsed_time = bndstrg_get_elapsed_time(temp_entry->tp);
					if(elapsed_time > max_elapsed_time){
						max_elapsed_time = elapsed_time;
						entry = temp_entry;
					}
				}
			}
			if(count >= table->Size)
				break;				
		}
	}
	if(!entry){
		if(inf)
		bndstrg_stop(bndstrg, BNDSTRG_LIMIT);
		else
			bndstrg_stop(bndstrg, BNDSTRG_DAEMON_TBL_FULL);
	}
	return entry;
};

