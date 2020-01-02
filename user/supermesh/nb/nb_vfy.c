#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include <string.h>


#include "nbapi.h"

int acmd_get_topology(void)
{
    int i, mesh_role;
    TOPOLOGY_LIST_T tplg_info;
    nb_get_topology(&tplg_info);

    printf("=============================================\n");
    printf("Topology\n");
    printf("=============================================\n");

    printf("%-4s\t%-8s\t%-12s\t%-12s\t%-16s\t%-12s\t%-8s\t%-8s\t%-16s\t%-8s\t%-8s\n",
        "Hop", "Domain",
        "Name", "Location",
        "Mac",
        "Model Name", "IMG Ver",
        "Role", "Uplink Mac",
        "Signal", "UL Type");

    for(i = 0; i < tplg_info.ap_num; i++)
    {

        printf("%-4u\t", tplg_info.tplgy_info[i].hop);
        printf("%-8s\t", tplg_info.tplgy_info[i].domain);
        printf("%-12s\t", tplg_info.tplgy_info[i].name);
        printf("%-12s\t", tplg_info.tplgy_info[i].location);
        printf("" MACSTR "\t", MAC2STR(tplg_info.tplgy_info[i].mac));
        printf("%-12s\t", tplg_info.tplgy_info[i].model_name);
        printf("%-8s\t", tplg_info.tplgy_info[i].img_ver);
        
        mesh_role = tplg_info.tplgy_info[i].mesh_role;
        if(mesh_role == MESH_ROLE_RAP)
        {
            printf("%-8s\t", "RAP");
        }
        else if(mesh_role == MESH_ROLE_MAP)
        {
            printf("%-8s\t", "MAP");
        }
        else if(mesh_role == MESH_ROLE_WMAP)
        {
            printf("%-8s\t", "WMAP");
        }
        else if(mesh_role == MESH_ROLE_MASTERAP)
        {
            printf("%-8s\t", "MasterAP");
        }
        else
        {
            printf("%-8s\t", "UNKNOWN");
        }

        /* Remove RAP NONE status(GUI error handling), mesh_role == MESH_ROLE_RAP, assign master mac to uplink mac, 20171229 */
        printf("" MACSTR "\t", MAC2STR(tplg_info.tplgy_info[i].ul_mac));
        printf("%-8u\t", tplg_info.tplgy_info[i].sig_strength);
        if(tplg_info.tplgy_info[i].ul_type == UL_TYPE_WIRELESS)
        {
            printf("%-8s\n", "Wireless");
        }
        else if(tplg_info.tplgy_info[i].ul_type == UL_TYPE_WIRED)
        {
            printf("%-8s\n", "Wired");
        }
        else if(tplg_info.tplgy_info[i].ul_type == UL_TYPE_POWERLINE)
        {
            printf("%-8s\n", "Powerline");
        }
        else if(tplg_info.tplgy_info[i].ul_type == UL_TYPE_MOCA)
        {
            printf("%-8s\n", "MOCA");
        }
        else
        {
            printf("%-8s\n", "UNKNOWN");
        }
    }    
    return 0;
}


int acmd_get_net_review(void)
{
    NET_REVIEW net_rv;
    unsigned int i, ap_count;
    unsigned int reason_code = 0;
    int mesh_role;

    nb_get_net_review(&net_rv);


    printf("=============================================\n");
    printf("Network Review\n");
    printf("=============================================\n");

    printf("Review Criterion\n");
    printf("%-20s\t%-5u\n", "Max Hops", net_rv.rv_criterion.max_hops);
    printf("%-20s\t%-5u\n", "Max Downlink APs", net_rv.rv_criterion.max_children);
    printf("%-20s\t%-5u\n", "Min Local Speed", net_rv.rv_criterion.local_speed);
    printf("---------------------------------------------------------------------\n");

    printf("%-16s\t%-32s\n", "AP MAC", "Reason");
    ap_count = net_rv.ap_num;
    for(i = 0; i < ap_count; i++)
    {
        reason_code = net_rv.rv_rslt[i].errorcode;
        printf("" MACSTR "\t", MAC2STR(net_rv.rv_rslt[i].mac));

        if(reason_code == NET_RV_TOO_MANY_HOP)
        {
            printf("Too many hops.  ");
            printf("%s", net_rv.rv_rslt[i].varinfo);
        }
        if(reason_code == NET_RV_TOO_MANY_DL_AP)
        {
            printf("Too Many Downlink APs.  ");
            printf("%s", net_rv.rv_rslt[i].varinfo);
        }
        if(reason_code == NET_RV_UNQUALIFIED_UL)
        {
            printf("Unqualified uplink");
            printf("%s", net_rv.rv_rslt[i].varinfo);
        }
        if(reason_code == NET_RV_NORMAL)
        {
            printf("Normal  ");
            printf("%s", net_rv.rv_rslt[i].varinfo);
        }
        printf("\n");
 
    }

    printf("\n---------------------------------------------------------------------\n");
    
    printf("%-4s\t%-8s\t%-12s\t%-12s\t%-16s\t%-12s\t%-8s\t%-8s\t%-16s\t%-8s\n",
    "Hop", "Domain",
    "Name", "Location",
    "Mac",
    "Model Name", "IMG Ver",
    "Role", "Uplink Mac"
    "Signal", "UL Type");
    for(i = 0; i < net_rv.ap_num; i++)
    {

        printf("%-4u\t", net_rv.tplgy_info[i].hop);
        printf("%-8s\t", net_rv.tplgy_info[i].domain);
        printf("%-12s\t", net_rv.tplgy_info[i].name);
        printf("%-12s\t", net_rv.tplgy_info[i].location);
        printf("" MACSTR "\t", MAC2STR(net_rv.tplgy_info[i].mac));
        printf("%-12s\t", net_rv.tplgy_info[i].model_name);
        printf("%-8s\t", net_rv.tplgy_info[i].img_ver);
        
        mesh_role = net_rv.tplgy_info[i].mesh_role;
        if(mesh_role == MESH_ROLE_RAP)
        {
            printf("%-8s\t", "RAP");
        }
        else if(mesh_role == MESH_ROLE_MAP)
        {
            printf("%-8s\t", "MAP");
        }
        else if(mesh_role == MESH_ROLE_WMAP)
        {
            printf("%-8s\t", "WMAP");
        }
        else if(mesh_role == MESH_ROLE_MASTERAP)
        {
            printf("%-8s\t", "MasterAP");
        }
        else
        {
            printf("%-8s\t", "UNKNOWN");
        }

        /* Remove RAP NONE status(GUI error handling), mesh_role == MESH_ROLE_RAP, assign master mac to uplink mac, 20171229 */
        printf("" MACSTR "\t", MAC2STR(net_rv.tplgy_info[i].ul_mac));
        printf("%-8u\t", net_rv.tplgy_info[i].sig_strength);
        if(net_rv.tplgy_info[i].ul_type == UL_TYPE_WIRELESS)
        {
            printf("%-8s\n", "Wireless");
        }
        else if(net_rv.tplgy_info[i].ul_type == UL_TYPE_WIRED)
        {
            printf("%-8s\n", "Wired");
        }
        else if(net_rv.tplgy_info[i].ul_type == UL_TYPE_POWERLINE)
        {
            printf("%-8s\n", "Powerline");
        }
        else if(net_rv.tplgy_info[i].ul_type == UL_TYPE_MOCA)
        {
            printf("%-8s\n", "MOCA");
        }
        else
        {
            printf("%-8s\n", "UNKNOWN");
        }
    }    


    return 0;
}

int acmd_set_time_schedule(void)
{
    unsigned int i, j;
    SCHE_SET_T tm_sche;


    tm_sche.list_count = 2;
    for(i = 0; i < tm_sche.list_count; i++)
    {
        strncpy(tm_sche.sche_list[i].name, "default", sizeof(tm_sche.sche_list[i].name));
        tm_sche.sche_list[i].info_count = 3;
        for(j = 0; j < tm_sche.sche_list[i].info_count; j++)
        {
            tm_sche.sche_list[i].sche_info[j].allday = j;
            tm_sche.sche_list[i].sche_info[j].date = j;
            tm_sche.sche_list[i].sche_info[j].timeformat = j;

            tm_sche.sche_list[i].sche_info[j].end_tm.hour = j+8;
            tm_sche.sche_list[i].sche_info[j].end_tm.middate = j+8;
            tm_sche.sche_list[i].sche_info[j].end_tm.minute = j+8; 

            tm_sche.sche_list[i].sche_info[j].start_tm.hour = j;
            tm_sche.sche_list[i].sche_info[j].start_tm.middate = j;
            tm_sche.sche_list[i].sche_info[j].start_tm.minute= j;
        }

    }
    nb_set_time_schedule_rule(&tm_sche);
 
    return 0;
}

int acmd_set_dev_mgmt_list(void)
{
    unsigned int i;
    MAC_FILTER_T mac_flt;

    mac_flt.enable = 1;
    mac_flt.is_allow_list = 1;
    mac_flt.mac_info_count = 2;

    for(i = 0; i < mac_flt.mac_info_count; i++)
    {
        strncpy(mac_flt.mac_info[i].dev_name, "mac_name", sizeof(mac_flt.mac_info[i].dev_name));
        strncpy(mac_flt.mac_info[i].mac, "11:11:11:22:22:22", sizeof(mac_flt.mac_info[i].mac));
        strncpy(mac_flt.mac_info[i].sche_name, "sche_name", sizeof(mac_flt.mac_info[i].sche_name));
        mac_flt.mac_info[i].status = i;
    }            


    nb_set_dev_mgmt_list(&mac_flt);

    return 0;
}


int acmd_set_service_wlan_default(void)
{
    WLAN_RADIO_SETTING_T serv_info;
    WLAN_RADIO_SEC_T sec_info;


    strncpy(serv_info.radio_id, "dlink_24", sizeof(serv_info.radio_id));
    strncpy(serv_info.mode, "mode1", sizeof(serv_info.mode));
    strncpy(serv_info.ssid, "123456", sizeof(serv_info.ssid));
    strncpy(serv_info.schedule_name, "sche1",sizeof(serv_info.schedule_name));
    serv_info.enable = 1;
    serv_info.ssid_broadcast = 12;
    serv_info.channel_width = 13;
    serv_info.channel = 114;
    serv_info.secondary_channel = 10;
    serv_info.qos = 1;
    serv_info.tx_power = 442;
    serv_info.coexistence = 1;
    serv_info.mumimo_enable = 1;
    serv_info.band_steering_enable = 1;
    serv_info.air_time_fairness_enable = 0;
    serv_info.isolate = 0;

    strncpy(sec_info.radio_id, "dlink_24", sizeof(sec_info.radio_id));
    strncpy(sec_info.encryption, "encrypt",sizeof(sec_info.encryption));
    strncpy(sec_info.radius_ip1, "r_ip1", sizeof(sec_info.radius_ip1));
    strncpy(sec_info.radius_secret1, "r_sec1", sizeof(sec_info.radius_secret1));
    strncpy(sec_info.radius_ip2, "r_ip2", sizeof(sec_info.radius_ip2));
    strncpy(sec_info.radius_secret2, "r_sec2", sizeof(sec_info.radius_secret2));
    strncpy(sec_info.type, "r_type", sizeof(sec_info.type));
    strncpy(sec_info.key, "keyForAp", sizeof(sec_info.key));

    sec_info.radius_prot1 = 1;
    sec_info.radius_prot2 = 2;
    sec_info.renewal = 11;
    sec_info.enable = 2;

    nb_set_service_wlan(&serv_info, &sec_info);
    return 0;
}


int acmd_set_guest_wlan_default(void)
{
    WLAN_RADIO_SETTING_T g_wlan;
    WLAN_RADIO_SEC_T sec_info;


    strncpy(g_wlan.radio_id, "dlink_24", sizeof(g_wlan.radio_id));
    strncpy(g_wlan.mode, "mode1", sizeof(g_wlan.mode));
    strncpy(g_wlan.ssid, "123456", sizeof(g_wlan.ssid));
    strncpy(g_wlan.schedule_name, "sche1",sizeof(g_wlan.schedule_name));
    g_wlan.enable = 1;
    g_wlan.ssid_broadcast = 12;
    g_wlan.channel_width = 13;
    g_wlan.channel = 114;
    g_wlan.secondary_channel = 10;
    g_wlan.qos = 1;
    g_wlan.tx_power = 442;
    g_wlan.coexistence = 1;
    g_wlan.mumimo_enable = 1;
    g_wlan.band_steering_enable = 1;
    g_wlan.air_time_fairness_enable = 0;
    g_wlan.isolate = 0;
            
    strncpy(sec_info.radio_id, "dlink_24", sizeof(sec_info.radio_id));
    strncpy(sec_info.encryption, "encrypt",sizeof(sec_info.encryption));
    strncpy(sec_info.radius_ip1, "r_ip1", sizeof(sec_info.radius_ip1));
    strncpy(sec_info.radius_secret1, "r_sec1", sizeof(sec_info.radius_secret1));
    strncpy(sec_info.radius_ip2, "r_ip2", sizeof(sec_info.radius_ip2));
    strncpy(sec_info.radius_secret2, "r_sec2", sizeof(sec_info.radius_secret2));
    strncpy(sec_info.type, "r_type", sizeof(sec_info.type));
    strncpy(sec_info.key, "keyForAp", sizeof(sec_info.key));
            
    sec_info.radius_prot1 = 1;
    sec_info.radius_prot2 = 2;
    sec_info.renewal = 11;
    sec_info.enable = 2;


    nb_set_guest_wlan(&g_wlan, &sec_info);
    return 0;
}


int acmd_set_wps(void)
{
    WPS_T wps;

    wps.wpspbc = 6;
    wps.wpspin = 8;
    nb_set_wps(&wps);
    return 0;
}


int acmd_set_ntp(void)
{
    TIME_SETTING_T tm_set;

    strncpy(tm_set.cur_date, "20170921", sizeof(tm_set.cur_date));
    strncpy(tm_set.cur_time, "1820", sizeof(tm_set.cur_time));
    strncpy(tm_set.ntp_server, "dlink", sizeof(tm_set.ntp_server));
    strncpy(tm_set.tz, "tw", sizeof(tm_set.tz));
    strncpy(tm_set.dst_start_month, "11", sizeof(tm_set.dst_start_month));
    strncpy(tm_set.dst_start_week, "2", sizeof(tm_set.dst_start_week));
    strncpy(tm_set.dst_start_day_of_week, "5", sizeof(tm_set.dst_start_day_of_week));
    strncpy(tm_set.dst_start_time, "11", sizeof(tm_set.dst_start_time));
    strncpy(tm_set.dst_end_month, "15", sizeof(tm_set.dst_end_month));
    strncpy(tm_set.dst_end_week, "9", sizeof(tm_set.dst_end_week));
    strncpy(tm_set.dst_end_day_of_week, "3", sizeof(tm_set.dst_end_day_of_week));
    strncpy(tm_set.dst_end_time, "19:30", sizeof(tm_set.dst_end_time));

    tm_set.ntp = 2;
    tm_set.daylight_saving = 8;

    nb_set_ntp(&tm_set);
    return 0;
}


int acmd_set_dev_setting(void)
{
    DEV_SETTING_T dev_set;

    strncpy(dev_set.dev_name, "dlink_dev", sizeof(dev_set.dev_name));
    strncpy(dev_set.admin_pwd, "pwd", sizeof(dev_set.admin_pwd));
    strncpy(dev_set.presentation_url, "URL", sizeof(dev_set.presentation_url));
    strncpy(dev_set.tz_location, "Asia/Taipei", sizeof(dev_set.tz_location));

    dev_set.capthcha = 1;
    dev_set.change_pwd = 5050;

    nb_set_dev_setting(&dev_set);
    return 0;
}

int acmd_set_config(void)
{
    unsigned int i, j;
    CONFIG_T config;


    srand(time(NULL));
    /*Schedule Setting*/
    config.sche_set.list_count = 2;
    for(i = 0; i < config.sche_set.list_count; i++)
    {
        strncpy(config.sche_set.sche_list[i].name, "default", sizeof(config.sche_set.sche_list[i].name));
        config.sche_set.sche_list[i].info_count = 3;
        for(j = 0; j < config.sche_set.sche_list[i].info_count; j++)
        {
            config.sche_set.sche_list[i].sche_info[j].allday = (rand() % 10);
            config.sche_set.sche_list[i].sche_info[j].date = (rand() % 31);
            config.sche_set.sche_list[i].sche_info[j].timeformat = (rand() % 100);

            config.sche_set.sche_list[i].sche_info[j].end_tm.hour = (rand() % 24);
            config.sche_set.sche_list[i].sche_info[j].end_tm.middate = (rand() % 3);
            config.sche_set.sche_list[i].sche_info[j].end_tm.minute = (rand() % 60);

            config.sche_set.sche_list[i].sche_info[j].start_tm.hour = (rand() % 24);
            config.sche_set.sche_list[i].sche_info[j].start_tm.middate = (rand() % 3);
            config.sche_set.sche_list[i].sche_info[j].start_tm.minute= (rand() % 60);
        }
    }

    /*Device Management*/
    config.mac_filt.enable = 1;
    config.mac_filt.is_allow_list = 1;
    config.mac_filt.mac_info_count = 2;
    for(i = 0; i < config.mac_filt.mac_info_count; i++)
    {
        strncpy(config.mac_filt.mac_info[i].dev_name, "mac_name", sizeof(config.mac_filt.mac_info[i].dev_name));
        strncpy(config.mac_filt.mac_info[i].mac, "11:11:11:22:22:22", sizeof(config.mac_filt.mac_info[i].mac));
        strncpy(config.mac_filt.mac_info[i].sche_name, "sche_name", sizeof(config.mac_filt.mac_info[i].sche_name));
        config.mac_filt.mac_info[i].status = i;
    }

    /*Wlan*/
    config.wlan_lst.num = 2;
    config.wlan_sec_lst.num= 2;
    for(i = 0; i < 2; i++)
    {
        if(i == 0)
        {
            strncpy(config.wlan_lst.info[i].radio_id, "RADIO_2.4GHz", sizeof(config.wlan_lst.info[i].radio_id));
            strncpy(config.wlan_sec_lst.sec_info[i].radio_id, "RADIO_2.4GHz", sizeof(config.wlan_sec_lst.sec_info[i].radio_id));
            config.wlan_lst.info[i].isolate = 0;
        }
        else
        {
            strncpy(config.wlan_lst.info[i].radio_id, "RADIO_2.4GHz_Guest", sizeof(config.wlan_lst.info[i].radio_id));
            strncpy(config.wlan_sec_lst.sec_info[i].radio_id, "RADIO_2.4GHz_Guest", sizeof(config.wlan_sec_lst.sec_info[i].radio_id));
            config.wlan_lst.info[i].isolate = 1;
        }

        strncpy(config.wlan_lst.info[i].mode, "mode1", sizeof(config.wlan_lst.info[i].mode));
        strncpy(config.wlan_lst.info[i].ssid, "123456", sizeof(config.wlan_lst.info[i].ssid));
        strncpy(config.wlan_lst.info[i].schedule_name, "sche1",sizeof(config.wlan_lst.info[i].schedule_name));
        config.wlan_lst.info[i].enable = 1;
        config.wlan_lst.info[i].ssid_broadcast = 12;
        config.wlan_lst.info[i].channel_width = 13;
        config.wlan_lst.info[i].channel = (rand() % 120);
        config.wlan_lst.info[i].secondary_channel = 10;
        config.wlan_lst.info[i].qos = 1;
        config.wlan_lst.info[i].tx_power = 442;
        config.wlan_lst.info[i].coexistence = 1;
        config.wlan_lst.info[i].mumimo_enable = 1;
        config.wlan_lst.info[i].band_steering_enable = 1;
        config.wlan_lst.info[i].air_time_fairness_enable = 0;
        strncpy(config.wlan_sec_lst.sec_info[i].encryption, "encrypt",sizeof(config.wlan_sec_lst.sec_info[i].encryption));
        strncpy(config.wlan_sec_lst.sec_info[i].radius_ip1, "r_ip1", sizeof(config.wlan_sec_lst.sec_info[i].radius_ip1));
        strncpy(config.wlan_sec_lst.sec_info[i].radius_secret1, "r_sec1", sizeof(config.wlan_sec_lst.sec_info[i].radius_secret1));
        strncpy(config.wlan_sec_lst.sec_info[i].radius_ip2, "r_ip2", sizeof(config.wlan_sec_lst.sec_info[i].radius_ip2));
        strncpy(config.wlan_sec_lst.sec_info[i].radius_secret2, "r_sec2", sizeof(config.wlan_sec_lst.sec_info[i].radius_secret2));
        strncpy(config.wlan_sec_lst.sec_info[i].type, "r_type", sizeof(config.wlan_sec_lst.sec_info[i].type));
        strncpy(config.wlan_sec_lst.sec_info[i].key, "keyForAp", sizeof(config.wlan_sec_lst.sec_info[i].key));
        config.wlan_sec_lst.sec_info[i].radius_prot1 = 1;
        config.wlan_sec_lst.sec_info[i].radius_prot2 = 2;
        config.wlan_sec_lst.sec_info[i].renewal = (rand() % 10);
        config.wlan_sec_lst.sec_info[i].enable = 1;
    }

    /*WPS*/
    config.wps.wpspbc = (rand() % 10);
    config.wps.wpspin = (rand() % 10);

    /*NTP Setting*/
    strncpy(config.tm_set.cur_date, "20170921", sizeof(config.tm_set.cur_date));
    strncpy(config.tm_set.cur_time, "1820", sizeof(config.tm_set.cur_time));
    strncpy(config.tm_set.ntp_server, "dlink", sizeof(config.tm_set.ntp_server));
    strncpy(config.tm_set.tz, "tw", sizeof(config.tm_set.tz));
    strncpy(config.tm_set.dst_start_month, "11", sizeof(config.tm_set.dst_start_month));
    strncpy(config.tm_set.dst_start_week, "2", sizeof(config.tm_set.dst_start_week));
    strncpy(config.tm_set.dst_start_day_of_week, "5", sizeof(config.tm_set.dst_start_day_of_week));
    strncpy(config.tm_set.dst_start_time, "11", sizeof(config.tm_set.dst_start_time));
    strncpy(config.tm_set.dst_end_month, "15", sizeof(config.tm_set.dst_end_month));
    strncpy(config.tm_set.dst_end_week, "1", sizeof(config.tm_set.dst_end_week));
    strncpy(config.tm_set.dst_end_day_of_week, "3", sizeof(config.tm_set.dst_end_day_of_week));
    strncpy(config.tm_set.dst_end_time, "19:30", sizeof(config.tm_set.dst_end_time));
    config.tm_set.ntp = (rand() % 100);
    config.tm_set.daylight_saving = 8;

    /*device setting*/
    strncpy(config.dev_set.dev_name, "dlink_dev", sizeof(config.dev_set.dev_name));
    strncpy(config.dev_set.admin_pwd, "pwd", sizeof(config.dev_set.admin_pwd));
    strncpy(config.dev_set.presentation_url, "URL", sizeof(config.dev_set.presentation_url));
    config.dev_set.capthcha = 10;
    config.dev_set.change_pwd = (rand() % 1000);

    nb_set_config(&config);

    return 0;
}


int acmd_reboot_ap(void)
{
    unsigned char mac[6];

    /* Give the ap mac address which you want to reboot */
    mac[0] = 0xff;
    mac[1] = 0xff;
    mac[2] = 0xff;
    mac[3] = 0xff;
    mac[4] = 0xff;
    mac[5] = 0xff;

    /* Call NB API to do reboot ap*/
    nb_reboot_ap(mac);

    return 0;
}

int acmd_img_check(void)
{
    nb_check_img();
    return 0;
}


int acmd_img_upgrade(void)
{
    IMG_INFO_T      img_info;
    unsigned int    ap_num, i;

    printf("acmd_img_upgrade\n");

    /* Give total AP num which you plan to upgrade their image */
    ap_num = 1;

    /* Fill in ap information*/
    for(i = 0; i < ap_num; i++)
    {
        /* Fill in everyone ap mac whose you want to upgrade their image */
        img_info.ap_info[ap_num].mac[0] = 0xff;
        img_info.ap_info[ap_num].mac[1] = 0xff;
        img_info.ap_info[ap_num].mac[2] = 0xff;
        img_info.ap_info[ap_num].mac[3] = 0xff;
        img_info.ap_info[ap_num].mac[4] = 0xff;
        img_info.ap_info[ap_num].mac[5] = 0xff;
    }
    img_info.ap_num = ap_num;
    nb_upgrade_img(&img_info);
    return 0;
}

int acmd_get_interface_stats(void)
{
    unsigned int    i;
    IF_STATS_HIS_T  if_stats;

    /* Fill in the mac address you want to test */
    if_stats.mac_addr[0] = 0xff;
    if_stats.mac_addr[0] = 0xff;
    if_stats.mac_addr[0] = 0xff;
    if_stats.mac_addr[0] = 0xff;
    if_stats.mac_addr[0] = 0xff;
    if_stats.mac_addr[0] = 0xff;

    /* Fill in interface you want to get */
    strncpy(if_stats.interface, "WLAN2.4G", sizeof(if_stats.interface));

    /* Call NB api to get interface statistic*/
    nb_get_interface_stats(&if_stats);

    printf("=============================================\n");
    printf("Interface Statistics History\n");
    printf("=============================================\n");
    printf("MAC" MACSTR "\n", MAC2STR(if_stats.mac_addr));
    printf("Interface:%s\n", if_stats.interface);
    printf("Record:%u\n", if_stats.records);
    printf("%-5s\t%-10s\t%-10s\t%-10s\t%-10s\t%-10s\t%-10s\t%-10s\t%-10s\t%-12s\n", "Array", "Send", "Recv", "TX_PKTS", "RX_PKTS", "TX_DROP", "RX_DROP", "Session", "Error", "Time");

    for(i = 0; i < if_stats.records; i++)
    {
        printf("[%-u]\t", i);
        printf("%-10llu\t%-10llu\t%-10llu\t%-10llu\t%-10llu\t%-10llu\t%-10llu\t%-10llu\t", if_stats.stats_info[i].sent, if_stats.stats_info[i].received,
                                                                                                        if_stats.stats_info[i].tx_pkts, if_stats.stats_info[i].rx_pkts,
                                                                                                        if_stats.stats_info[i].tx_drop, if_stats.stats_info[i].rx_drop,
                                                                                                        if_stats.stats_info[i].session, if_stats.stats_info[i].errors);

        if(if_stats.stats_info[i].record_tm.middate == TRUE)
            printf("[PM]");
        else
            printf("[AM]");
        printf("%02d:%02d\n", if_stats.stats_info[i].record_tm.hour, if_stats.stats_info[i].record_tm.minute);
    }
    return 0;
}

int example_set_ap_info(void)
{
    SET_AP_INFO_T   ap_info;

    /* Give the ap mac address which you want to set ap's information */
    ap_info.ap_mac[0] = 0xff;
    ap_info.ap_mac[1] = 0xff;
    ap_info.ap_mac[2] = 0xff;
    ap_info.ap_mac[3] = 0xff;
    ap_info.ap_mac[4] = 0xff;
    ap_info.ap_mac[5] = 0xff;

    /* Give the ap's name which you want to change */
    strncpy(ap_info.ap_name, "default_name", sizeof(ap_info.ap_name));
    nb_set_ap_info(&ap_info);
    return 0;
}

int example_reset_to_default(void)
{
    unsigned char mac[6];

    /* Give the ap mac address which you want to reset */
    mac[0] = 0xff;
    mac[1] = 0xff;
    mac[2] = 0xff;
    mac[3] = 0xff;
    mac[4] = 0xff;
    mac[5] = 0xff;

    /* Call NB API to do reboot ap*/
    nb_reset_to_default(mac);

    return 0;
}

int main(int argc, char **argv)
{
    acmd_get_net_review();
    acmd_get_topology();
    return 0;
}

