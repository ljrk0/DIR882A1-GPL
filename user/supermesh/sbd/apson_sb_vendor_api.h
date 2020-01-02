#ifndef _ACMD_SBAPI_
#define _ACMD_SBAPI_


typedef struct t_time_info {
    int32_t         checksum;
    int32_t         hour;                   // Value of hour
    int32_t         minute;                 // Value of minute
    int32_t         middate;                // false:AM, true:PM
} time_info_s;

typedef struct t_schedule_info {
    int32_t         checksum;
    int32_t         date;                   // which day of the week of schedule. 0:all week, 1:Monday, 2:Tuesday, 3:Wednesday, 4:Thursday, 5:Friday, 6:Saturday, 7:Sunday
    int32_t         allday;                 // true: schedule tales all day long, false: Not a all day schedule
    int32_t         time_format;            // true: 24-hr format, false 12-hr format
    time_info_s     start_time;             // specific start time of schedule
    time_info_s     end_time;               // specific end time of schedule
} schedule_info_s;

typedef struct t_schedule_list {
    int32_t         checksum;
    char            name[16];               // the schedule name for scheduling
    int             sche_info_count;        // num of sche_info
    schedule_info_s sche_info[84];
} schedule_list_s;

typedef struct t_schedule_setting {
    int32_t         checksum;
    int             sche_list_count;        // num of sche_list
    schedule_list_s sche_list[10];
} schedule_setting_s;

typedef struct t_mac_info {
    int32_t         checksum;
    int32_t         status;                 // specifies whether access rule of this device enabled(true or false)
    char            mac[18];                // The MAC Address of the host, In xx:xx:xx:xx:xx:xx format
    char            dev_name[21];           // an optional human-readable string describing the device.
    char            sche_name[16];          // specify a schedule rule and the client will be blocked as this time range
} mac_info_s;

typedef struct t_mac_filter {
    int32_t         checksum;
    int32_t         enable;                 // specifies whether filters is enables(true or false)
    int32_t         is_allow_list;          // true: only the list of MAC address can connect to the network
                                            // false: any MAC address in the list will be blocked from network
    int             mac_info_count;         // num of mac_info
    mac_info_s      mac_info[24];
} mac_filter_s;

typedef struct t_mac_del_filter {
    int32_t         checksum;
    char            mac[18];                // delete this mac in mac filter
} mac_del_filter_s;

typedef struct t_wps {
    int32_t         checksum;
    int32_t         wpspbc;                 // true: WPS-PIN enabled, false: WPS-PIN disabled
    int32_t         wpspin;                 // the WPS personal identification number of this device
} wps_s;

typedef struct t_time_setting {
    int32_t         checksum;
    char            cur_date[11];               // specify current date of the device
    char            cur_time[9];                // specify current date of the time
    int32_t         ntp;                        // true: NTP enable, flase: NTP disable
    char            ntp_server[257];            // specify NTP server name or location
    char            tz[3];                      // specify time zone index of the device.
    int32_t         daylight_saving;            // true: daylight saving enabled, false: daylight saving disabled
    char            dst_start_month[3];         // specify start month for daylight saving
    char            dst_start_week[2];          // specify start week of a month for daylight saving
    char            dst_start_day_of_week[2];   // specify start day of a week for daylight saving
    char            dst_start_time[9];          // specify start time of a day for daylight saving
    char            dst_end_month[3];           // specify end month for daylight saving
    char            dst_end_week[2];            // specify end week of a month for daylight saving
    char            dst_end_day_of_week[2];     // specify end day of a week for daylight saving
    char            dst_end_time[9];            // specify end time of a day for daylight saving
} time_setting_s;

typedef struct t_dev_setting {
    int32_t         checksum;
    char            dev_name[21];           // the name to use for this device
    char            admin_pwd[16];          // the administrator password for this device
    char            presentation_url[257];  // A URL to the Web-based user interface for administering device
    int32_t         captcha;                // specifies whether CAPTCHA is enabled(true or false)
    int32_t         change_pwd;             // true: Password will be changed, false: Follow the old
    char            tz_location[32];        // specify time zone index of the device by specifying the location in represented of the timezone.
                                            // Follow the stander IANA format of Area/City, such as "Asia/Taipei" format.
} dev_setting_s;

typedef struct t_wlan_info {
    int32_t         checksum;
    char            device[9];
    char            ifname[9];              // the wireless network service on which interface
    char            ssid[33];               // the wireless network name for this wireless band
    char            encryption[10];         // encryption method to be used
    char            key[64];                // this value depends on the settings of the type parameter
    char            sche_name[16];          // specificies associated schedule name
    int32_t         secu_enable;            // specificies whether security is enabled(true or false)
    int32_t         enable;                 // Configures the wireless function is enalbed(true or false), now not used
    int32_t         isolate;                // if this wlan need blocked client from access any other device, only can connect to the Internet
                                            // true: guest wlan, false: service wlan
    char            radioid[20];            // pass the radioid of the device whose settings are to be obtained.
    int32_t         ssid_bcast;             // specifies whether the device is broadcasting the wirelwss network name for network detection(true or false)
    int32_t         qos;                    // returns the current configuration of wireless quality of service setting as defined by 802.1e(true or false)
    char            type[15];               // the type of wireless security to be used
    int32_t         key_renewal;            // time in seconds of the WPA key renewal period.
                                            // Ignored if security type is not WPA variant
    char            radius_ip1[16];         // if RADIUS is used, type the primary/preferred RADIUS sever's IP address in x.x.x.x format
    int32_t         radius_port1;           // the RADIUS sever port number.
    char            radius_secret1[150];    // shared secret to use with first set of RADIUS severs.
    char            radius_ip2[16];         // if RADIUS is used, type the secondary/fallback RADIUS sever's IP address in x.x.x.x format
    int32_t         radius_port2;           // the secondary/fallback RADIUS sever port number.
    char            radius_secret2[150];    // shared secret to use with second set of RADIUS severs.
} wlan_info_s;

typedef struct t_wlan {
    int32_t         checksum;
    int32_t         wlan_sum;           // num of wlan
    int32_t         wlan_sub_count;     // num of wlan
    wlan_info_s     wlan_sub[4];
} wlan_s;

typedef struct t_radio_info {
    int32_t         checksum;
    char            radioid[20];        // pass the radioid of the device whose settings are to be obtained.
    int32_t         enable;             // Configures the wireless function is enalbed(true or false)
    char            mode[15];           // specifies the 802.11 mode that the router is currently configured for.
    int32_t         ch_width;           // Width of each channel. this corresponds to the 802.11ac channel width standard.
    int32_t         ch;                 // Channel number the radio is currently set to.
    int32_t         second_ch;          // returns the secondary channel being used by the device
    int32_t         tx_power;           // the parameters of the uplink transmit power
    int32_t         coexistence;        // specifies HT20/40 coexistence in or off
    int32_t         mumimo_enable;      // specifies MU-MIMO on or off
    int32_t         band_steer_enable;  // specifies Band Streering on or off
    int32_t         air_tm_fairness_en; // specifies Air Time Fairness on or off
} radio_info_s;

typedef struct t_radio {
    int32_t         checksum;
    int32_t         radio_sum;          // num of radio
    int32_t         radio_sub_count;    // num of radio
    radio_info_s    radio_sub[16];
} radio_s;

typedef struct t_config {
    int32_t             checksum;
    wlan_s              wlan;
    radio_s             radio;
    schedule_setting_s  sche_set;
    mac_filter_s        mac_filt;
    wps_s               wps;
    mac_del_filter_s    mac_del_flt;
    time_setting_s      tm_set;
    dev_setting_s       dev_set;
} config_s;
enum timezong_index_e{
    tz_International_Date_Line_West=1,
    tz_Midway,
    tz_Hawaii,
    tz_Alaska,
    tz_Pacific,     //5
    tz_Arizona,
    tz_Chihuahua,
    tz_Mountain_Canada,
    tz_Central_America,
    tz_Central,     //10
    tz_Guadalajara,
    tz_Saskatchewan,
    tz_Bogota,
    tz_Eastern,
    tz_Caracas,     //15
    tz_Georgetown,
    tz_Atlantic,
    tz_Santiago,
    tz_Newfoundland,
    tz_Brasilia,    //20
    tz_Buenos,
    tz_Greenland,
    tz_Mid_Atlantic,
    tz_Azores,
    tz_Cape,        //25
    tz_Casablanca,
    tz_Greenwich,
    tz_Amsterdam,
    tz_Belgrade,
    tz_Brussels,    //30
    tz_Sarajevo,
    tz_West,
    tz_Athens,
    tz_Bucharest,
    tz_Cairo,       //35
    tz_Harare,
    tz_Helsinki,
    tz_Jerusalem,
    tz_Baghdad,
    tz_Kuwait,      //40
    tz_Nairobi,
    tz_Tehran,
    tz_Abu,
    tz_Baku,
    tz_Moscow,      //45
    tz_Kabul,
    tz_Islamabad,
    tz_Chennai,
    tz_Sri,
    tz_Kathmandu,   //50
    tz_Astana,
    tz_Ekaterinburg,
    tz_Rangoon,
    tz_Almaty,
    tz_Bangkok,     //55
    tz_Krasnoyarsk,
    tz_Beijing,
    tz_Kuala,
    tz_Perth,
    tz_Taipei,      //60
    tz_Ulaan,
    tz_Irkutsk,
    tz_Osaka,
    tz_Seoul,
    tz_Adelaide,    //65
    tz_Darwin,
    tz_Brisbane,
    tz_Canberra,
    tz_Guam,
    tz_Hobart,      //70
    tz_Yakutsk,
    tz_Solomon,
    tz_Vladivostok,
    tz_Magadan,
    tz_Auckland,    //75
    tz_Fiji,
    tz_Nuku,
    tz_Samoa
};

typedef struct _daylight_saving_t{ 
    int  start_month;
    int  start_week;
    int  start_day;
    char start_time[10];
    int  end_month;
    int  end_week;
    int  end_day;
    char end_time[10];
    //
}daylight_saving_t;



#define   RADIO_ID_24G         "RADIO_2.4GHz"
#define   RADIO_ID_24G_GUEST   "RADIO_2.4GHz_Guest"
#define   RADIO_ID_5G          "RADIO_5GHz"
#define   RADIO_ID_5G_GUEST    "RADIO_5GHz_Guest"
#define   RADIO_ID_5G_2        "RADIO_5GHz_2"
#define   RADIO_ID_5G_2_GUEST  "RADIO_5GHz_2_Guest"

#if defined WIFI_DBDC_SUPPORT
#define INTERFACE_2G			"rax0"
#define INTERFACE_5G			"ra0"
#define APCLI_IF_2G 			"apclix0"
#define APCLI_IF_5G 			"apcli0"
#define INTERFACE_2G_GUEST1		"rax1"
#define	INTERFACE_5G_GUEST1		"ra1"
#define INTERFACE_5G_MESH_DOWNLINK 			"ra3"
#define	INTERFACE_5G_MESH_UPLINK			"apcli0"
#else
#define INTERFACE_2G			"ra0"
#define INTERFACE_5G			"rai0"
#define APCLI_IF_2G 			"apcli0"
#define APCLI_IF_5G 			"apclii0"
#define INTERFACE_2G_GUEST1		"ra1"
#define	INTERFACE_5G_GUEST1		"rai1"
#define INTERFACE_5G_MESH_DOWNLINK 			"rai3"
#define	INTERFACE_5G_MESH_UPLINK			"apclii0"
#endif

typedef enum
{
	MODULE_LAN,
	MODULE_DNSMASQ,
	MODULE_WLAN,
	MODULE_WLAN_GUEST,
	MODULE_WAN,
	MODULE_WAN6,
	MODULE_DHCPS,
	MODULE_PLC,
	MODULE_MLD,
	MODULE_IGMP,
	MODULE_ALG,
	MODULE_UPNP,
	MODULE_DOS,
	MODULE_MAC,
	MODULE_AUTH,
	MODULE_DNS,
	MODULE_QOS,
	MODULE_IPT,
	MODULE_URLFILTER,
	MODULE_FIREWALL,
	MODULE_SNTP,
	MODULE_UPG,
	MODULE_AUTOUPG,
	MODULE_REBOOT,
	MODULE_ACL,
	MODULE_LOG,
	MODULE_DSL_ATM,
	MODULE_DSL_PTM,
	MODULE_DSL_MODULATION,
	MODULE_QUICKVPN,
	MODULE_STATICROUTE_IPV4,
	MODULE_STATICROUTE_IPV6,	
	MODULE_SYSTEMLOG,		
	MODULE_SCHEDULE,
	MODULE_FTP,
	MODULE_SAMBA,
	MODULE_DDNS_IPV4,
	MODULE_DDNS_IPV6,
	MODULE_ADMIN,
	MODULE_UPNP_AND_IGMP,
	MODULE_EMAIL,
	MODULE_MDNS,
	MODULE_DLNA,
#ifdef CONFIG_AP_REPEATER_SUPPORT
	MODULE_MODE,
#endif
	MODULE_TR69C
};


typedef enum{
	RESULT_ERROR,
	RESULT_OK,
	RESULT_REBOOT,
	SET_JSON_FORMAT_ERROR,
	SET_JSON_TYPE_ERROR,
	SET_CONFIG_ERROR,
	SET_DATA_TYPE_ERROR,
	SET_DATA_VALUE_ERROR,
	SET_CHECK_CONFLICT_ERROR,
	GET_JSON_FORMAT_ERROR,
	GET_JSON_TYPE_ERROR,
	GET_JSON_NULL_ERROR,
	GET_JSON_VALUE_ERROR,
	GET_LIB_CONFIG_ERROR,
	GET_DATA_TYPE_ERROR,
	GET_DATA_VALUE_ERROR,
	GET_CONFIG_ERROR,
	GET_CONFIG_RECUR_DEEP_ERROR,
	SET_CONFIG_RECUR_DEEP_ERROR,
	SAVE_CONFIG_ERROR,
	GET_NEW_JSON_OBJECT_ERROR,
	RESULT_WIZARD_FINISH,
	XMLDB_FORMAT_ERROR,
	RESULT_HNAP_ERROR,
	MALLOC_FAIL_ERROR,
};

/* ÊµÀýÀàÐÍ */
typedef enum _RT_802_11_PHY_MODE {
	PHY_11BG_MIXED = 0,
	PHY_11B = 1,
	PHY_11A = 2,
	PHY_11ABG_MIXED = 3,
	PHY_11G = 4,
	PHY_11ABGN_MIXED = 5,	/* both band   5 */
	PHY_11N_2_4G = 6,		/* 11n-only with 2.4G band      6 */
	PHY_11GN_MIXED = 7,		/* 2.4G band      7 */
	PHY_11AN_MIXED = 8,		/* 5G  band       8 */
	PHY_11BGN_MIXED = 9,	/* if check 802.11b.      9 */
	PHY_11AGN_MIXED = 10,	/* if check 802.11b.      10 */
	PHY_11N_5G = 11,		/* 11n-only with 5G band                11 */
	PHY_11VHT_N_ABG_MIXED = 12, /* 12 -> AC/A/AN/B/G/GN mixed */
	PHY_11VHT_N_AG_MIXED = 13, /* 13 -> AC/A/AN/G/GN mixed  */
	PHY_11VHT_N_A_MIXED = 14, /* 14 -> AC/AN/A mixed in 5G band */
	PHY_11VHT_N_MIXED = 15, /* 15 -> AC/AN mixed in 5G band */
	PHY_11VHT_AC = 16, /* 16 -> AC only */
	PHY_MODE_MAX,
} RT_802_11_PHY_MODE;

static char G_wlMode[][32]=
{
	{"802.11bg"},   /*PHY_11BG_MIXED 0 */
	{"802.11b"},    /*PHY_11B 1 */
	{"802.11a"},    /*PHY_11A 2 */
	{"802.11abg"},  /*PHY_11ABG_MIXED 3 */
	{"802.11g"},    /*PHY_11G 4 */
	{"802.11abgn"}, /*PHY_11ABGN_MIXED 5 */
	{"802.11n"},    /*PHY_11N_2_4G 6 ->11n-only with 2.4G band*/
	{"802.11gn"},   /*PHY_11GN_MIXED 7 */
	{"802.11an"},   /*PHY_11AN_MIXED 8 */
	{"802.11bgn"},  /*PHY_11BGN_MIXED 9 */
	{"802.11agn"},  /*PHY_11AGN_MIXED 10 */
	{"802.11n"},    /*PHY_11N_5G 11 ->11n-only with 5G band*/
	{"802.11abgnac"},/*PHY_11VHT_N_ABG_MIXED 12 -> AC/A/AN/B/G/GN mixed*/
	{"802.11ag"},    /*PHY_11VHT_N_AG_MIXED 13 -> AC/A/AN/G/GN mixed*/
	{"802.11anac"},     /*PHY_11VHT_N_A_MIXED 14 -> AC/AN/A mixed in 5G band*/
	{"802.11nac"},     /*PHY_11VHT_N_MIXED 15 -> AC/AN mixed in 5G band*/
	{"802.11ac"},     /*PHY_11VHT_AC  16 -> AC only */
	{"802.11"},      /*PHY_11BG_MIXED PHY_MODE_MAX */
};

#define   CHANNEL_WIDTH_AUTO2040M         0
#define   CHANNEL_WIDTH_AUTO204080M       1
#define   CHANNEL_WIDTH_AUTO204080160M    2
#define   CHANNEL_WIDTH_AUTO20408080M     3
#define   CHANNEL_WIDTH_20M               20
#define   CHANNEL_WIDTH_40M               40
#define   CHANNEL_WIDTH_80M               80
#define   CHANNEL_WIDTH_160M              160

#define   TX_POWER_HIGH             "100"
#define   TX_POWER_MID              "50"
#define   TX_POWER_LOW              "25"

#define MAX_SCHEDULE_RULE_NAME_LEN  15
#define MAX_SCHEDULE_RULE_BUF_LEN   2048
#define NVRAM_SCHEDULE_ITEM_PREFIX  "schedule"

enum sch_rule_type_e {
    SCH_TIME_ALL_WEEK_NULL = 1,
    SCH_TIME_ALL_WEEK_ALL_DAY,
    //SCH_TIME_ALL_WEEK_NOT_ALL_DAY,
    SCH_TIME_OTHER
};

enum sch_day_index_e {
    SCH_ALL_WEEK = 0,
    SCH_MONDAY,
    SCH_TUESDAY,
    SCH_WEDNESDAY,
    SCH_THURSDAY,
    SCH_FRIDAY,
    SCH_SATURDAY,
    SCH_SUNDAY
};

struct sch_rule_s {
    int action;
    char name[MAX_SCHEDULE_RULE_NAME_LEN + 1];
    char rule[MAX_SCHEDULE_RULE_BUF_LEN];
};

#define APSON_CONF_FILE_PATH "/etc/config/apson"

typedef struct t_apson {
    char                domain[32];
    char                ssid[33];
    uint32_t            security_mode; /* 0 = OPEN, 1 = SHARED, 2 = WPAPSK, 3 = WPA2PSK, 4 = WPAPSKWPA2PSK, 5 = else */
    uint32_t            encryption_mode; /* 0 = NONE, 1 = WEP, 2 = TKIP, 3 = TKIPAES, 4 = AES, 5 = else */
    char                psk[65];
} vendor_apson_s;

/* init config */
int apson_sb_init_apson(void);

/* remote ap command */
int apson_sb_reboot();
int apson_sb_reset_to_default();
int apson_sb_image_check(char *pMsg);
int apson_sb_image_upgrade();

/* LED control */
int apson_sb_set_led(int status);
int apson_sb_set_led_provision(int status); // 0 = unprovision, 1 = provision.

/* set */
int apson_sb_set_schedule_rule(schedule_setting_s *pSetting_data);
int apson_sb_set_managed_device(mac_filter_s *pSetting_filter, mac_del_filter_s *pSetting_del);
int apson_sb_set_wps(wps_s *pSetting_data);
int apson_sb_set_ntp(time_setting_s *pSetting_data);
int apson_sb_set_administration(dev_setting_s *pSetting_data);
int apson_sb_set_service_wlan(wlan_info_s *pSetting_wlan, radio_info_s *pSetting_radio);
int apson_sb_set_guest_wlan(wlan_info_s *pSetting_wlan, radio_info_s *pSetting_radio);
int apson_sb_set_dlinkmaster(char *domain, char *ip);
int apson_sb_set_apson(vendor_apson_s *pSetting_apson);
int apson_sb_set_ap_name(char* pMsg);
int apson_sb_set_channel(char *radioid, int ch);

/* get */
int apson_sb_get_schedule_rule(schedule_setting_s *pDevice_data);
int apson_sb_get_managed_device(mac_filter_s *pDevice_data);
int apson_sb_get_wps(wps_s *pDevice_data);
int apson_sb_get_ntp(time_setting_s *pDevice_data);
int apson_sb_get_administration(dev_setting_s *pDevice_data);
int apson_sb_get_wlan_all(wlan_s *pDevice_wlan, radio_s *pDevice_radio);
int apson_sb_get_firmware_version(char *pDevice_data);
#endif
