/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : cli_strres.h
 文件描述 : 用来定义字符串资源的文件


 函数列表 :


 修订记录 :
          1 创建 : liuzequn
            日期 : 2007-10-29
            描述 :

**********************************************************************/


#ifndef __CLI_STRRES_H__
#define __CLI_STRRES_H__


/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/



#define CLI_LOGED_INFO           "\r\n\r\n  Welcome to command line system! " \
                                     "\r\n  Soft Version :1.0 \r\n"
#define CLI_HOSTNAME_NULL        "\r\n  Hostname get failed!"
#define CLI_SCROLL_ENABLE        "\r\n\r\n  Set screen output scroll manual"
#define CLI_SCROLL_DISABLE       "\r\n\r\n  Set screen output scroll automatic"
#define CLI_LOGOUT_CONFIRM       "\r\n\r\n  Are you sure to logout?"
#define CLI_INPUT_CONFIRM_ERROR   "\r\n  Error input!"
#define CLI_INPUT_CONFIRM_TIMEOUT "\r\n  Input timeout!"


#define CLI_LOG_HEAD_DISP        "\r\n Operation                 Username  Log Mode  Client IP     Date      Time"
#define CLI_LOG_TABLE_ITEM       "\r\n %-25s %-9s %-7s %-15s %-18s"

/*----------------------------------*/
/*    命令管理部分语种资源定义      */
/*----------------------------------*/
#define CLI_CMD_NO_HELPINFO      "No Help Information !!!!!!!"
#define CLI_HELPINFO_HEAD        "\r\n\r\n  Command Word"
#define CLI_HELPINFO_NEXT        "\r\n\r\n  Next Command Word"
#define CLI_INTP_CMD_INCOMPLETE  "\r\n  Incompleted command: "
#define CLI_INTP_NEED_COMWORD    "\r\n  need command key: "
#define CLI_INTP_NEED_PARAM      "\r\n  need parameter: "
#define CLI_INTP_NEED_VALUE      "\r\n  need value: "
#define CLI_INTP_CMD_ERR         "\r\n  Incorrect command: "
#define CLI_INTP_CMD_FAILED      "\r\n  Execute command failed."
#define CLI_INTP_CMD_NOEXEC      "\r\n  Action is not available."
#define CLI_INTP_INVALID_PARAM   "\r\n  Invalid parameter:"
#define CLI_INTP_INVALID_VALUE   "\r\n  Invalid parameter value:"
#define CLI_INTP_INNEED_PARAM    "\r\n  Too many parameters:superfluous-parameter-"
#define CLI_PARAM_CANNOT_SAVE    "\r\n  Saving parameter failed. [Value = %s] [Index = %d]"
#define CLI_VALUE_HAS_BLANK      "\r\n  Parameter or value exclude blank!"
#define CLI_INTEG_OVERFLOW       "\r\n  Integer value out of range."
#define CLI_HEX_OVERFLOW         "\r\n  Hex value out of range."
#define CLI_STRING_TOO_LONG      "\r\n  String too long!"
#define CLI_INVALID_KEY          "\r\n  Invalid keyword value."
#define CLI_INVALID_IP           "\r\n  Invalid IP address."
#define CLI_INVALID_MAC          "\r\n  Invalid MAC address."
#define CLI_INVALID_MASK         "\r\n  Invalid MASK address."
#define CLI_INVALID_DATE         "\r\n  Invalid Date."
#define CLI_INVALID_TIME         "\r\n  Invalid Time."
#define CLI_NO_SUCH_MODE         "\r\n  Wanted a non-existed mode."
#define CLI_PUSH_CMD_FAILED      "\r\n  Push command to stack failed."
#define CLI_MODE_DEL_FAILED      "\r\n  Connect count is 0, mode delete fail."
#define CLI_POP_CMD_FAILED       "\r\n  Pop command to stack failed."
#define CLI_MODE_CONV_FAILED     "\r\n  Mode conversion failed."
#define CLI_MODE_DEL_BANNED      "\r\n Some other users are configuring in the mode, so you don't delete it!"
#define CLI_MODE_INIT_ERR        "\r\n  Initializing Command Mode Structure Failed!"
#define CLI_ROOTOBJ_INIT_ERR     "\r\n  Initializing Root Object Failed!"
#define CLI_MODECMD_REG_ERR      "\r\n  Register Mode Command Failed!"
#define CLI_CMD_BAD_STATUS       "\r\n  Bad status Pointer! (Program Error)"
#define CLI_HELP_INFO            "\r\n  Input \"?\" to get the help information,"\
                                    "\r\n  Input \"cm xxx\" to change you command mode to xxx,"\
                                    "\r\n  Input CTRL+C to interrupt the command input"\
                                    "\r\n  Now list the command word of all mode:"
#define CLI_CMD_TOO_BIG          "\r\n  Too big command "
/*----------------------------------*/
/*    命令行其它部分语种资源定义    */
/*----------------------------------*/
#define PUB_TABLE_EMBODY  "\r\n ----------------------------------------------------------------------------"
#define PUB_STRING_OUT    "%s"

/* 厂商，型号，固件版本节点路径 */
#define NODE_MODEM_BRAND "InternetGatewayDevice.DeviceInfo.Manufacturer"
#define NODE_MODEM_MODEL "InternetGatewayDevice.DeviceInfo.ModelName"
#define NODE_FIRMWARE_VERSION "InternetGatewayDevice.DeviceInfo.SoftwareVersion"

/* SOC，adsl，flash及memory的品牌及型号 */
#define DEVICE_SOC_BRAND           "MediaTeK"
#define DEVICE_SOC_MODEL          "MT7513"
#define DEVICE_ADSL_BRAND         "MediaTeK"
#define DEVICE_ADSL_MODEL         "MT7513"
#define DEVICE_RAM_BRAND          "Winbond"
#define DEVICE_RAM_MODEL          "W9751G6KB-Z5"
#define DEVICE_FLASH_BRAND      "MXIC"
#define DEVICE_FLASH_MODEL      "MX25L12845"

#define CMDHELP_GLB_SHOW  "Commands of display system information"
#define CMDHELP_GLB_SHOW_LOG            HELP_FUN "Display log information of user "  \
                                        HELP_FMT "show log "
/* help 命令的帮助信息 */
#define CMDHELP_GLB_HELP                HELP_FUN "Help information of command line operate"

/* hostname 的帮助信息 */
#define CMDHELP_GLB_SET        "Operation of system attribute set"

/*adsl 的帮助信息*/
//#define CMDHELP_GLB_ADSL       "Display the table SNR or Bits per tones(carriers) of the interface ADSL2+ "
#define CMDHELP_GLB_ADSL           	HELP_FUN "Display the table SNR or Bits per tones(carriers) of the interface ADSL2+"  \
                                        HELP_FMT "adsl: {bits}|{snr}|{status}"\
                                        HELP_EXP "adsl bits: display the table Bits per tones(carriers)"\
                                        HELP_EXP "adsl snr: display the table SNR per tones(carriers)"\
                                        HELP_EXP "adsl status: display the synchronize information of adsl "

/* cls 的帮助信息 */
#define CMDHELP_GLB_CLS                 HELP_FUN "Clear screen "
/* exit 的帮助信息 */
#define CMDHELP_GLB_EXIT                HELP_FUN "Logout from the system "
/* set timeout 的帮助信息 */
#define CMDHELP_GLB_SET_TIMEOUT         HELP_FUN "Set the timeout value: long or short"  \
                                        HELP_FMT "set timeout {long|short}"  \
                                        HELP_OPT "long  : long time (120minutes)"\
                                        HELP_SPC "short : short time (5 minutes)"
/* set scroll 的帮助信息 */
#define CMDHELP_GLB_SET_SCROLL          HELP_FUN "Set screen scroll mode: manual or automatic " \
                                        HELP_FMT "set scroll {automatic|manual}"

#define CMDHELP_GLB_ADSL_CMD            HELP_FUN "display the table: snr or bits " \
                                        HELP_FMT "adsl {bits|snr}"

/* cm 的帮助信息 */
#define CMDHELP_GLB_CM        "Command of mode convert"
/* 命令 cm ..的帮助信息*/
#define CMDHELP_GLB_CM_PARENT           HELP_FUN "Exit to parent mode "
/* 命令 cm root的帮助信息*/
#define CMDHELP_GLB_CM_ROOT             HELP_FUN "Exit to root mode "


#define CMDHELP_GENL_CM_CONFIG          HELP_FUN "Enter configure mode" \
                                        HELP_FMT "cm configure"

#define CMDHELP_GLB_IF_CONFIG           HELP_FUN "Show or configure the WAN interface"  \
                                        HELP_SPC "Show all interface when the word 'set' was ignored."\
                                        HELP_FMT "ifconfig [set <interface> <ip> <mask> <gateway>]"  \
                                        HELP_OPT "interface : Number of WAN interface. range of 0~1"\
                                        HELP_SPC "ip        : IP addreass"\
                                        HELP_SPC "mask      : Subnet mask"\
                                        HELP_SPC "gateway   : Gateway if interface"\
                                        HELP_EXP "ifconfig"\
                                        HELP_EXP "ifconfig set 1 10.0.0.9 255.0.0.0 10.0.0.1"


#define CMDHELP_GLB_DEBUG           	HELP_FUN "Get or Set a Node,only for debug"  \
                                        HELP_FMT "debug {get <path>}|{set <path> <value>}"  \
                                        HELP_OPT "path	    : the node path"\
                                        HELP_SPC "value     : the node value"\
                                        HELP_EXP "debug get InternetGatewayDevice.DeviceInfo.Manufacturer"\
                                        HELP_EXP "debug set InternetGatewayDevice.DeviceInfo.ProvisioningCode 1234"

#define CMDHELP_GLB_SAVE_CFG           	HELP_FUN "save config"  \
                                        HELP_FMT "save-cfg"


#define CMDHELP_GLB_IPT                 HELP_FUN "Show or configure the IPT module"  \
                                        HELP_SPC "Show all entry indexes recorded when the specific command was ignored."\
                                        HELP_FMT "ipt"  \
                                        HELP_FMT "ipt [add <path>]"  \
                                        HELP_FMT "ipt [del <path>]"  \
                                        HELP_FMT "ipt [get <path>]"  \
                                        HELP_FMT "ipt [set <path> <value>]"  \
                                        HELP_FMT "ipt [commit]"  \
                                        HELP_FMT "ipt [cancel]"  \
                                        HELP_FMT "ipt [wanconnect]"  \
                                        HELP_FMT "ipt [wandisconnect]"  \
                                        HELP_OPT "path      : the node path"\
                                        HELP_SPC "value     : the node value"\
                                        HELP_EXP "ipt /* Not available */"\
                                        HELP_EXP "ipt add InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping."\
                                        HELP_EXP "ipt del InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.1."\
                                        HELP_EXP "ipt get InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.1.PortMappingEnabled"\
                                        HELP_EXP "ipt set InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.1.PortMappingEnabled 1"\
                                        HELP_EXP "ipt commit"\
                                        HELP_EXP "ipt cancel"\
                                        HELP_EXP "ipt wanconnect"\
                                        HELP_EXP "ipt wandisconnect"

#define CMDHELP_GLB_DMZ                 HELP_FUN "Show or configure the DMZ"  \
                                        HELP_FMT "dmz [add <name> <ip>]"  \
                                        HELP_FMT "dmz [del <name>]"  \
                                        HELP_FMT "dmz [show]"  \
                                        HELP_EXP "dmz show"\
                                        HELP_EXP "dmz add PVC:8/32 192.168.1.2 "\
                                        HELP_EXP "dmz del PVC:8/32"


#define CMDHELP_GLB_DNS                 HELP_FUN "Show or configure the WAN's DNS"  \
                                        HELP_FMT "dns [set <wanname> <dnsserver>]"  \
                                        HELP_FMT "dns [show]"  \
                                        HELP_EXP "dns show"\
                                        HELP_EXP "dns set ETH_STATIC_0_1 172.24.11.10,172.24.10.10 "


#define CMDHELP_GLB_DDNS           	HELP_FUN "Show or config the ddns module"  \
                                        HELP_FMT "ddns add tzo <connection-name> <hostname> <e-mail> <key>"  \
                                        HELP_FMT "ddns add dyndns <connection-name> <hostname> <username> <password>"  \
                                        HELP_FMT "ddns add gnudip <connection-name> <hostname> <ddns-server> <username> <password>"  \
			        	     HELP_FMT "ddns del <hostname>" \
					     HELP_FMT "ddns show" \
                                        HELP_OPT "connection-name	    : wan connection name"\
                                        HELP_SPC "hostname                  : URL of your domain, but "\
                                        HELP_SPC "                            in gnudip type,this is ongly"\
                                        HELP_SPC "                            your username "\
                                        HELP_OPT "e-mail         	    : your e-mail for tzo authericate"\
                                        HELP_OPT "Key            	    : key get from tzo ddns provider"\
                                        HELP_EXP "ddns add tzo dhcp1234 test.tzo.com ojh@163.com abcdefgh"\
                                        HELP_EXP "ddns add gnudip dhcp1234 gnudip gnudip.com my_username my_password"

#define CMDHELP_GLB_NATP                HELP_FUN "Show or config the natp(Port Forwarding) module" \
                                        HELP_FMT "natp add <server> <wan> <protocol> <ex-start> <ex-range> <internalport> <local> [schedule <name(Always|Never)>] [remote <IP>]" \
                                        HELP_FMT "natp del <server> <wan>" \
                                        HELP_FMT "natp show" \
                                        HELP_OPT "server    : The name of the NAT rule" \
                                        HELP_SPC "wan       : Set the Wan Interface on which the NAT rule takes effect" \
                                        HELP_SPC "protocol  : Set between tcp and udp" \
                                        HELP_SPC "ex-start  : The start number of the port on the wan interface" \
                                        HELP_SPC "ex-range    : Indicates the last port of the external port range that starts with ExternalPort" \
                                        HELP_SPC "InternalPort  : The start number of the port on the lan side" \
                                        HELP_SPC "local     : The IP of the LAN side PC" \
                                        HELP_OPT "schedule  : attach a schedule to the NAT rule to take effect" \
                                        HELP_OPT "name      : A string or can be Always or Never" \
                                        HELP_OPT "remote    : The IP on the Wan side" \
                                        HELP_EXP "natp add http-server ppp0 tcp 80 80 8080 192.168.1.5 schedule Always" 

#define CMDHELP_GLB_ROUTE               HELP_FUN "Show or config the route module including dynamic route and static route" \
                                        HELP_FMT "route dynamic set <wan> <rip-version>" \
                                        HELP_FMT "route static add <dest-network> <subnet> <gateway-ip> <wan>" \
                                        HELP_FMT "route static del <dest-network> <subnet>" \
                                        HELP_FMT "route show" \
                                        HELP_OPT "wan          : The name of the wan interface" \
                                        HELP_SPC "rip-version  : The value can be Off, RIPv1, RIPv2" \
                                        HELP_SPC "dest-network : If dest address of packet inside this network, it will use this rule" \
                                        HELP_SPC "subnet       : The subnet can specify the exact network" \
                                        HELP_SPC "gateway-ip   : Specify an IP which the packets will be routed to" \
                                        HELP_EXP "route dynamic set ppp0 RIPv2" \
                                        HELP_EXP "route static add 10.10.0.0 255.255.0.0 10.10.10.1 ppp0" \
                                        HELP_EXP "route static del 10.10.0.0 255.255.0.0"

#define CMDHELP_GLB_WAN                 HELP_FUN "Show or config the wan connection" \
                                        HELP_FMT "wan add <protocol> <mode-v(ATM|PTM|Ethernet)> [vpi <vpi-v(u0~255)>] [vci <vci-v(u32~65535)> ] [vlan <vlan-v(u0~65535)>] [vlanpri <vlanpri-v(u0~7)>] [nat (enable|disable)] [encap (LLC|VCMUX)][serv <serv-v(UBRPlus|UBR|CBR|VBR-nrt|VBR-rt)>][username <string>] [password <string>] [dialup <AlwaysOn|OnDemand|Manual>] [pppidletimeout <seconds>] [pppmru <num>] [service <string>] [ipaddr <ip>] [netmask <mask>] [remoteip <ip>][dnsip <ip>]" \
                                        HELP_FMT "wan modify <pvc> <protocol> <mode-v(ATM|PTM|Ethernet)> [vpi <vpi-v(u0~255)>] [vci <vci-v(u32~65535)> ] [vlan <vlan-v(u0~65535)>] [vlanpri <vlanpri-v(u0~7)>] [nat (enable|disable)] [encap (LLC|VCMUX)] [username <string>] [password <string>] [dialup <AlwaysOn|OnDemand|Manual>] [pppidletimeout <seconds>] [pppmru <num>] [service <string>] [ipaddr <ip>] [netmask <mask>] [remoteip <ip>]" \
										HELP_FMT "wan del <pvc name>" \
                                        HELP_FMT "wan show [conn <pvc name>]" \
                                        HELP_OPT "protocol : The connection type. It can be one of bridge, pppoe, pppoa, mer, ipoa" \
                                        HELP_SPC "vpi      : The range is 0~255" \
                                        HELP_SPC "vci      : The range is 32~65535" \
                                        HELP_SPC "encap    : The encapsulate mode. It can be one of LLC and VCMUX" \
                                        HELP_OPT "nat      : Choose enable or disable nat" \
                                        HELP_OPT "username : The user name of ppp account" \
                                        HELP_OPT "password : The password of ppp account" \
                                        HELP_OPT "dialup   : Choose the dial up mode for ppp session" \
                                        HELP_OPT "pppidleitmeout : Input the time that ppp will disconnect after no Internet access. Dialup should be set to OnDemand" \
                                        HELP_OPT "pppmru   : Set the mru for ppp. The range is 128~1540" \
                                        HELP_OPT "service  : Input the service name of PPPoE if necessary" \
                                        HELP_OPT "Status   : The connection status" \
                                        HELP_OPT "Authentication  : The Authentication Method" \
                                        HELP_OPT "ipaddr   : Tnput the ip address of connection if it needs to be specified" \
                                        HELP_OPT "netmask  : Input the netmask of ipaddr" \
                                        HELP_OPT "remoteip : Input the gateway IP" \
                                        HELP_OPT "pvc      : The name of this connection" \
                                        HELP_EXP "wan add bridge ATM vpi 0 vci 44 encap LLC" \
                                        HELP_EXP "wan add pppoa ATM vpi 0 vci 311 encap LLC nat enable username test password test serv UBRPlus" \
                                        HELP_EXP "wan add mer ATM  vpi 0 vci 32 encap LLC nat enable  ipaddr 10.10.10.10 netmask 255.255.255.0 remoteip 10.10.10.1 dns 10.10.10.1" \
                                        HELP_EXP "wan show              //show the Pvc list and Ethernet list" \
										HELP_EXP "wan modify D_PPPoE_0_1 mer ATM  vpi 0 vci 32  encap LLC nat enable ipaddr 10.10.10.10 netmask 255.255.255.0 remoteip 10.10.10.1" \
                                        HELP_EXP "wan del D_PPPoE_0_1               //according to 'wan show'"  \
                                        HELP_EXP "wan show conn D_PPPoE_0_1                //according to 'wan show'"  \
                                        HELP_FMT "\n wan set  [ppp <pvc name>] [vpi <vpi-v(u0~255)>] [vci  <vci-v(u32~65535)>] [username <username-value>] [password <password-value>]" \
                                        HELP_FMT "wan get [ppp <pvc name>] " \
                                        HELP_EXP "wan set ppp D_PPPoE_0_2 username abc password def" \
                                        HELP_EXP "wan get ppp D_PPPoE_0_2 "
                                        
                                        

#define CMDHELP_GLB_DHCPSERVER	HELP_FUN "Show or config or enable or disable the DHCPServer" \
                			    HELP_FMT "dhcpserver port lan|wlan <interface(u1~4)><enable-disable(enable|disable)>" \
                			    HELP_FMT "dhcpserver config <start-IP-address(p)> <end-IP-address(p)> <lease-time(u0~4294967295)> <dnsserver1(p)>" \
                			    HELP_FMT "dhcpserver enable" \
                			    HELP_FMT "dhcpserver disable" \
                			    HELP_FMT "dhcpserver show" \
	                                HELP_OPT "start-IP-address : The DHCPServer start IP" \
	                                HELP_SPC "end-IP-address : The DHCPServer end IP" \
	                                HELP_SPC "lease-time : The DHCPServer lease time" \
	                                HELP_SPC "dnsserver : dns server IP" \
	                                HELP_EXP "dhcpserver config 192.168.0.2 192.168.0.100 86400 192.168.0.1" \
	                                HELP_EXP "dhcpserver port lan 1 enable" \
	                                HELP_EXP "dhcpserver show"


#define CMDHELP_GLB_LAN_IPINTERFACE	HELP_FUN "Show or config or enabl or|disable the lan IPInterface" \
	                               HELP_FMT "lan config <interface(u1~2)> <ip(p)> <mask(p)>" \
	                               HELP_FMT "lan enable <interface(u1~2)/all>" \
					   HELP_FMT "lan disable <interface(u1~2)/all>" \
					   HELP_FMT "lan show <interface(u1~2)/all>" \
					   HELP_OPT "interface : which interface to be configed" \
	                               HELP_SPC "IP : The IP of the interface" \
	                               HELP_SPC "mask : The mask of the interface" \
	                               HELP_EXP "lan config 1 192.168.0.2 255.255.255.0" \
	                               HELP_EXP "lan enable all" \
            	                        HELP_EXP "lan show 1"

#define CMDHELP_GLB_WLAN	HELP_FUN "Show or config or enable or disable the wlan and the wlan encrpytion" \
	                              HELP_FMT "wlan show <interface(u1~4)> ssid" \
	                              HELP_FMT "wlan show <interface(u1~4)> channel" \
	                              HELP_FMT "wlan show <interface(u1~4)> 8211mode" \
	                              HELP_FMT "wlan show <interface(u1~4)> encrypt" \
	                              HELP_FMT "wlan show <interface(u1~4)> status" \
	                              HELP_FUN "Show key in wep mode "\
	                              HELP_FMT "wlan show <interface(u1~4)> wep" \
	                              HELP_FUN "Show key in wpa mode "\
	                              HELP_FMT "wlan show <interface(u1~4)> wpa" \
	                              HELP_FMT "wlan show <interface(u1~4)> status" \
	                              HELP_FMT "wlan enable [interface<value(u1~4)>]" \
	                              HELP_FMT "wlan disable [interface<value(u1~4)>]" \
	                              HELP_FMT "wlan config <interface(u1~4)> basic [ssid <ssid-v(s256)>] [channel <channel-v(u0~13)>] [8211mode <8211mode-v(b|g|n|bg|ng|bgn)>] " \
	                              HELP_FMT "wlan config <interface(u1~4)> encryption wep <wepkeylength(64bits|128bits)> <authentication(open|sharekey)> <wepkeyindex(u1~4)> <wepkeyvalue(s26)>" \
	                              HELP_FMT "wlan config <interface(u1~4)> encryption wpa <wpa-version(u1~2)> <wpa-encryption(TKIP|AES|TKIPAES)> psk <interval(u60~1000)> <pre-shared-key(s63)>" \
	                              HELP_FMT "wlan config <interface(u1~4)> encryption wpa <wpa-version(u1~2)> <wpa-encryption(TKIP|AES|TKIPAES)> ent <interval(u60~1000)> <server-ip(p)> <port(u1025~65535)> <secret(s64)>" \
	                              HELP_FMT "wlan noencryption <interface(u1~4)>" \
	                              HELP_OPT "ssid : the id of the device" \
	                              HELP_SPC "channel : The IP of the interface, 0 is the Auto Scan" \
	                              HELP_EXP "wlan config 1 basic ssid myssid" \
	                              HELP_EXP "wlan config 1 basic channel 11" \
	                              HELP_EXP "wlan config 1 basic 8211mode bgn" 

	                              
#define CMDHELP_GLB_MAC     HELP_FUN "Show or config or enable or filter mac black list" \
                                     HELP_FMT "mac show <mac-v(c)>" \
                                     HELP_FMT "mac del <mac-v(c)>" \
                                     HELP_FMT "mac add <mac-v(c)> <description(s256)> <starttime(t)> <endtime(t)> {{alldays} |{days <days-v(s27)>}" \
                                     HELP_OPT "mac-v: The mac address(00-00-00-00-00-00)" \
                                     HELP_SPC "description: The name of the mac" \
                                     HELP_SPC "starttime: The start time of a day" \
                                     HELP_SPC "endtime: The end time of a day" \
                                     HELP_SPC "days: Specify the days of a week (sun-mon-tue),select first three letters for a day and bind with '-'. " \
                                     HELP_SPC "alldays: Specify shoose all the days of a week" \
                                     HELP_EXP "mac add 00-00-00-11-11-11 macname 00:00:00 23:00:00 days sun-mon-tue" \
                                     HELP_EXP "mac del 00-00-00-11-11-11"
#define CMDHELP_GLB_WIRELESS_MAC HELP_FUN "Show or config or enable or filter wireless mac list" \
                                     HELP_FMT "wmac enable" \
                                     HELP_FMT "wmac disable" \
                                     HELP_FMT "wmac show" \
                                     HELP_FMT "wmac del <mac-v(c)>" \
                                     HELP_FMT "wmac add <mac-v(c)>" \
                                     HELP_OPT "mac-v: The mac address(00-00-00-00-00-00)" \
                                     HELP_EXP "mac add 00-00-00-11-11-11" \
                                     HELP_EXP "mac del 00-00-00-11-11-11"
    

#define CMDHELP_GLB_ADMIN            HELP_FUN "config the username and the password" \
                                     HELP_FMT "password change <userlevel(admin|user|support)> <oldpassword> <newusername> <newpassword>" \

#define CMDHELP_GLB_MODEM         HELP_FUN "display the brand,model,firmware version,firmware date" \
                                    HELP_OPT "showall:  display all of modem information"\
                                    HELP_OPT "brand:    display the brand of modem"\
                                    HELP_OPT "model:    display the model of modem"\
                                    HELP_OPT "version:  display the firmware version of modem"\
                                    HELP_OPT "date:     display the firmware date of modem" 

#define CMDHELP_GLB_DEVICE         HELP_FUN "display the brand and model of processor, ADSL, RAM memory, Flash memory" \
                                    HELP_OPT "showall: display all of device information"\
                                    HELP_OPT "soc:     display brand and model of the processor"\
                                    HELP_OPT "adsl:    display brand and model of the ADSL"\
                                    HELP_OPT "ram:     display brand and model of the RAM memory"\
                                    HELP_OPT "flash:   display brand and model of the Flash memory" 

#define CMDHELP_GLB_PMIRROR			HELP_FUN"pmirror vpi [vpival] vci[vcival] flags<pmirror_flags>"\
									HELP_OPT"vpival/vcival is optional"\
									HELP_EXP"pmirror_flags = 0: not mirror, 1: mirror sar input, 2: mirror sar output, 3: mirror both"

#define CMDHELP_GLB_ACL				HELP_FUN"acl: <conname(s)> <contype(s)> <port(s)> <optname(s)>"\
									HELP_OPT"conname:wan connection name, please use 'wan show' watch"\
									HELP_OPT"contype:wan|lan"\
									HELP_OPT"port:ftp|http|icmp|ssh|telnet|tftp"\
									HELP_OPT"optname:enable|disable|show"\
									HELP_OPT"eg1:acl pppoe_3_5 wan ftp disable"\
									HELP_OPT"eg2:acl pppoe_3_5 wan http enable"
									
#define CMDHELP_GLB_TR069           HELP_FUN "config or show or set connection request the Tr069 list" \
                                    HELP_FMT "tr069 config [status <status-value>][InfEnable <InfEnable-value>] [Inform <Inform-value>] [URL <url-value>] [username <username-value>] [password <password-value>]"\
                                    HELP_FMT "tr069 show"\
                                    HELP_FMT "tr069 request [RequestStatus<Enable-value(enable|disable)>] [username <RequestUsername>] [password <RequestPassword> [path <RequestPath> [port <RequestPort>]"\
                                    HELP_EXP "tr069 config 360 http://asc.o2online.de admin 123"\
                                    HELP_EXP "tr069 request enable username admin password 1234 path /tr069 port 7547"\
                                    HELP_EXP "tr069 request disable"

/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/


#endif /* __CLI_STRRES_H__ */



