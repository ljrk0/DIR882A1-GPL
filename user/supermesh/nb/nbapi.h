#include <stdio.h>
#include <stdlib.h>


#define NB_STATUS       int
#define NB_SUCCESS      0
#define NB_FAIL         -1
#define TRUE            1
#define FALSE           0

#define MAX_SUM_OF_AP       16

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

typedef enum {
    MESH_ROLE_INIT, 
    MESH_ROLE_RAP,
    MESH_ROLE_MAP,
    MESH_ROLE_WMAP,
    MESH_ROLE_MASTERAP
} MESH_ROLE_E;

typedef enum {
    NET_RV_NORMAL = 0,
    NET_RV_UNQUALIFIED_UL,
    NET_RV_TOO_MANY_HOP,
    NET_RV_TOO_MANY_DL_AP
} NET_REVIEW_E;

typedef enum {
    UL_TYPE_WIRELESS = 0,
    UL_TYPE_WIRED,
    UL_TYPE_POWERLINE,
    UL_TYPE_MOCA
}UL_TYPE;



typedef struct topology_info_t{
    unsigned int hop;           //ap's hop number
    char domain[32];            //ap's domain name, string length 32
    char name[32];              //ap's name, string length 32
    char location[32];          //ap's location, string length 32
    char model_name[16];        //ap's model name, string length 16
    char img_ver[16];           //ap's image version, string length 16
    unsigned int mesh_role;     //mesh role: RAP(1), MAP(2), WMAP(3) or MasterAP(4)
    unsigned char mac[6];       //ap's mac address
    unsigned char ul_mac[6];    //ap's uplink mac address
    unsigned int sig_strength;  //ap's signal strength
    unsigned int ul_type;       //uplink type
}TOPOLOGY_INFO_T, *PTOPOLOGY_INFO_T;

typedef struct topology_list_t{
    unsigned int ap_num;                        //sum of ap in this topology
    TOPOLOGY_INFO_T tplgy_info[MAX_SUM_OF_AP];  //ap's topology information
}TOPOLOGY_LIST_T, *PTOPOLOGY_LIST_T;

typedef struct rv_criterion_t{
    unsigned int max_hops;      //criterion: max num of hop
    unsigned int max_children;  //criterion: max num of children
    unsigned int local_speed;   //criterion: accepted min local speed
    unsigned int mesh_min_snr;  //criterion: accepted min snr speed
}RV_CRITERION_T, *PRV_CRITERION_T;

typedef struct rv_result_t{
    unsigned int reason_code;   //reason code, represent by bits
    unsigned char mac[6];       //ap's mac address
    char varinfo[80];           //detail error information
    int errorcode;              //error code by integer number
}RV_RESULT_T, *PRV_RESULT_T;

typedef struct net_review{
    unsigned int ap_num;                        //ap number
    RV_CRITERION_T rv_criterion;                //review standard
    RV_RESULT_T rv_rslt[MAX_SUM_OF_AP];         //Max of AP: 16
    TOPOLOGY_INFO_T tplgy_info[MAX_SUM_OF_AP];  //topology information
}NET_REVIEW, *PNET_REVIEW;

typedef struct img_ap_info_t{
    unsigned char mac[6];
}IMG_AP_INFO_T, *PIMG_AP_INFO_T;

typedef struct img_info_t{
    unsigned int ap_num;
    IMG_AP_INFO_T ap_info[MAX_SUM_OF_AP];
}IMG_INFO_T, *PIMG_INFO_T;

typedef struct time_info_t{
    int     hour;
    int     minute;
    int     middate;
}TIME_INFOT, *PTIME_INFO_T;

typedef struct sche_info_t{
    int         date;       //input
    int         allday;     //input
    int         timeformat; //input
    TIME_INFOT  start_tm;   //input
    TIME_INFOT  end_tm;     //input
}SCHE_INFO_T, *PSCHE_INFO_T;

typedef struct sche_list_t{
    int     info_count;         //input: shcedule information count
    char    name[16];           //input: schedule rule name, string lenght is 16
    SCHE_INFO_T sche_info[84];  //input: schedule information, max of rule is 84
}SCHE_LIST_T, *PSCHE_LIST_T;

typedef struct sche_set_t{
    int     list_count;         //input: amount of setting time schedule
    SCHE_LIST_T sche_list[10];  //input: setting time schedule list
}SCHE_SET_T, *PSCHE_SET_T;

typedef struct mac_info_t{
    int         status;
    char        mac[18];
    char        dev_name[21];
    char        sche_name[16];
}MAC_INFO_T, *PMAC_INFO_T;

typedef struct mac_filter_t{
    int             enable;
    int             is_allow_list;
    unsigned int    mac_info_count;
    MAC_INFO_T      mac_info[24];
}MAC_FILTER_T, *PMAC_FILTER_T;

typedef struct wlan_radio_setting_t{
    char    radio_id[20];
    int     enable;
    char    mode[15];
    char    ssid[33];
    int     ssid_broadcast;
    int     channel_width;
    int     channel;
    int     secondary_channel;
    int     qos;
    char    schedule_name[16];
    int     tx_power;
    int     coexistence;
    int     mumimo_enable;
    int     band_steering_enable;
    int     air_time_fairness_enable;
    int     isolate;
}WLAN_RADIO_SETTING_T, *PWLAN_RADIO_SETTING_T;

typedef struct wlan_radio_list_t{
    unsigned int            num;
    WLAN_RADIO_SETTING_T    info[4];
}WLAN_RADIO_LIST_T, *PWLAN_RADIO_LIST_T;

typedef struct wlan_radio_security_t{
    char    radio_id[20];
    int     enable;
    char    type[15];
    char    encryption[10];
    char    key[64];
    int     renewal;
    char    radius_ip1[16];
    int     radius_prot1;
    char    radius_secret1[150];
    char    radius_ip2[16];
    int     radius_prot2;
    char    radius_secret2[150];
}WLAN_RADIO_SEC_T, *PWLAN_RADIO_SEC_T;

typedef struct wlan_radio_sec_list_t{
    unsigned int            num;
    WLAN_RADIO_SEC_T        sec_info[4];
}WLAN_RADIO_SEC_LIST_T, *PWLAN_RADIO_SEC_LIST_T;

typedef struct wps_t{
    int wpspbc;
    int wpspin;
}WPS_T, *PWPS_T;

typedef struct time_setting_t{
    char    cur_date[11];
    char    cur_time[9];
    int     ntp;
    char    ntp_server[257];
    char    tz[3];
    int     daylight_saving;
    char    dst_start_month[3];
    char    dst_start_week[2];
    char    dst_start_day_of_week[2];
    char    dst_start_time[9];
    char    dst_end_month[3];
    char    dst_end_week[2];
    char    dst_end_day_of_week[2];
    char    dst_end_time[9];
}TIME_SETTING_T, *PTIME_SETTING_T;

typedef struct dev_setting_t{
    char    dev_name[21];
    char    admin_pwd[16];
    char    presentation_url[257];
    int     capthcha;
    int     change_pwd;
    char    tz_location[32];
}DEV_SETTING_T, *PDEV_SETTING_T;

typedef struct config_t{
    SCHE_SET_T              sche_set;
    MAC_FILTER_T            mac_filt;
    WLAN_RADIO_LIST_T       wlan_lst;
    WLAN_RADIO_SEC_LIST_T   wlan_sec_lst;
    WPS_T                   wps;
    TIME_SETTING_T          tm_set;
    DEV_SETTING_T           dev_set;
}CONFIG_T, *PCONFIG_T;

typedef struct stats_info_t{
    long long unsigned int sent;        //Output:this ap's interface sent traffic amount
    long long unsigned int received;    //Output:this ap's interface receibed traffic amount
    long long unsigned int tx_pkts;     //Output:tx packet
    long long unsigned int rx_pkts;     //Output:rx packet
    long long unsigned int tx_drop;     //Output:
    long long unsigned int rx_drop;     //Output:
    long long unsigned int session;     //Output:
    long long unsigned int errors;      //Output:
    TIME_INFOT  record_tm;              //Output:time: hr:00, hr:15, hr:30, hr:45
}STATS_INFO_T, *PSTATS_INFO_T;

typedef struct if_stats_his_t{
    unsigned char   mac_addr[6];        //Input, Output: mac address
    char            interface[20];      //Input, Output: WLAN2.4G, WLAN5G, WLAN2.4G_Guest, WLAN5G_Guest, WLAN5G_UP, All_CLT_IF
    int             records;            //Output: Num of historical records
    STATS_INFO_T    stats_info[4];      //Output: Bin informaiton 0--->3 old--->new
}IF_STATS_HIS_T, *PIF_STATS_HIS_T;

typedef struct set_ap_info_t{
    char            ap_name[32];    //input: New name.
    unsigned char   ap_mac[6];      //input: Specify which AP set its info
} SET_AP_INFO_T, *PSET_AP_INFO_T;

NB_STATUS nb_get_net_review(PNET_REVIEW pNet_rv);
NB_STATUS nb_get_topology(PTOPOLOGY_LIST_T pTplgy_list);
NB_STATUS nb_reboot_ap(unsigned char mac[]);
NB_STATUS nb_check_img(void);
NB_STATUS nb_upgrade_img(PIMG_INFO_T pImg_info);
NB_STATUS nb_set_time_schedule_rule(PSCHE_SET_T pTm_sche);
NB_STATUS nb_set_dev_mgmt_list(PMAC_FILTER_T pMac_flt);
NB_STATUS nb_set_service_wlan(PWLAN_RADIO_SETTING_T serv_wlan, PWLAN_RADIO_SEC_T sec_info);
NB_STATUS nb_set_guest_wlan(PWLAN_RADIO_SETTING_T g_wlan, PWLAN_RADIO_SEC_T sec_info);
NB_STATUS nb_set_wps(PWPS_T wps);
NB_STATUS nb_set_ntp(PTIME_SETTING_T tm_set);
NB_STATUS nb_set_dev_setting(PDEV_SETTING_T dev_set);
NB_STATUS nb_set_config(PCONFIG_T pConfig);
NB_STATUS nb_get_interface_stats(PIF_STATS_HIS_T pIf_stats);
NB_STATUS nb_set_ap_info(PSET_AP_INFO_T pAp_info);
NB_STATUS nb_reset_to_default(unsigned char mac[]);



