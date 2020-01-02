/**********************************************************************
 Copyright (c), 1991-2008, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称: ipt_common.h
 文件描述: 公用iptables接口头文件
 修订记录:
           1 作者 : 陈灼
             日期 : 2009-03-11
             描述 : 创建

           2 作者 : 匡素文
             日期 : 2009-07-01
             描述 : 完善注释
**********************************************************************/

#ifndef __IPT_COMM__
#define __IPT_COMM__


/******************************************************************************
*                                 MACRO
******************************************************************************/

/* 链名最大长度 */
#define MAX_NAME_LEN        56

/*parent Chain 数目*/
#define MAX_PARENT_CHAIN_NUM 6

/* 各链对应的ID */
typedef enum tag_EN_IPT_ChainId
{
    /* Chain PREROUTING */
    CN_PREROUTING  = (((unsigned int)(0x01))<<16),
    CN_ANTI_SYN_PRE,
    CN_ACL_ALLOW_PRE,
    CN_ACL_REMOTE_SUB,
    CN_ACL_REMOTE_PRE,
    PTMP_PREROUTING,
    DMZ_PREROUTING,
    ALG_PREROUTING,
    UPNP_PREROUTING,
    QOS_PRE_MATCH_UP,
    QOS_PRE_MATCH_DOWN,
    QOS_PREROUTING,

    CN_PT_PREROUTING,
    //CN_ANTI_STATE_INPUT,      //move to  Chain INPUT
	ALG_SIP_PREROUTING,
    QOS_IP6T_PRE_MATCH,	
    CN_DROP_SPECIAL_PRE,//这个子链放在末尾，一些知名端口，在ACL、DMZ、PTMP等都没有匹配上的情况下，在此DROP掉

    /* Chain POSTROUTING */
    CN_POSTROUTING  = (((unsigned int)(0x02))<<16),
    QOS_POSTROUTING_MARK,
    QOS_POSTROUTING_MATCH_DOWN,
    QOS_POSTROUTING,
    NAT_POSTROUTING,
    TDNAT_POSTROUTING,
    DMZ_POSTROUTING,
    PTMP_POSTROUTING,
    CN_WAN_VLAN_PRIOR,
    QOS_IP6T_POST_MARK,
	
    /* Chain FORWARD */  
    CN_FORWARD  = (((unsigned int)(0x03))<<16),
    CN_PT_FORWARD, 
    MACFILTER_WHITELIST,
    MACFILTER_BLACKLIST,
    IPFILTER_WHITELIST,
    IPFILTER_BLACKLIST,     
    URLFILTER_FORWARD,
    URLFILTER_WHITELIST,
    URLFILTER_BLACKLIST,
    
    CN_FW_FORWARD,
    CN_LanWhiteFilter_FORWARD,
    CN_LanBlackFilter_FORWARD,
    CN_LanPortBlackFilter_FORWARD,
    CN_WanWhiteFilter_FORWARD,
    CN_WanBlackFilter_FORWARD,  

    ALG_FORWARD,
    UPNP_FORWARD,
    CONTENTFILTER_BLACKLIST,
    QOS_FORWARD_MATCH,
    QOS_FORWARD_IMQ,
    WAN_TR069_FORWARD,
    CN_ANTI_STATE_FORWARD,
    QOS_IP6T_FORWARD_MATCH,
    QOS_IP6T_FORWARD_IMQ,

    /* Chain INPUT */
    CN_INPUT = (((unsigned int)(0x04))<<16),
    CN_MACFILTER_WHITELIST,
    CN_MACFILTER_BLACKLIST,    
    CN_FW_INPUT,
    CN_LanWhiteFilter_INPUT,
    CN_LanBlackFilter_INPUT,
    CN_LanPortBlackFilter_INPUT,
    CN_WanWhiteFilter_INPUT,
    CN_WanBlackFilter_INPUT, 
    CN_ANTI_SYN_INPUT,
    CN_VOIP_ALLOW_INPUT,
	CN_ALG_INPUT,
    CN_ACL_ALLOW_INPUT,
    CN_LOCAL_INPUT_TELNET,
    CN_ACL_LOCAL_INPUT,
    CN_ANTI_DOS_INPUT,
    CN_ANTI_SCAN_INPUT,
    CN_ANTI_PING_INPUT,
    UPNP_INPUT,
    CN_IP_ACL_INPUT,
    TF_FW_INPUT,
    CN_ANTI_STATE_INPUT,
    CN_ACL_REMOTE_INPUT, //????,????????ICMP,udp,tcp?????,??SPI?????
    CN_DHCP_RELAY_INPUT,
    CN_RIP_INPUT, // 开启SPI状态防火墙时，需要放行rip报文
    CN_SPI_INPUT, // add by linwenquan 2012-8-8

    /* Chain OUTPUT */
	CN_OUTPUT = (((unsigned int)(0x06))<<16),
	QOS_OUTPUT_MATCH,
	QOS_IP6T_OUT_MATCH,
    WAN_TR069_OUTPUT,
    TF_FW_OUTPUT,
} EN_IPT_ChainId;

/* iptables内置表名和链名 */
#define TAB_MANGLE          "mangle"
#define TAB_NAT             "nat"
#define TAB_FILTER          "filter"

#define CHAIN_PRE           "PREROUTING"
#define CHAIN_POST          "POSTROUING"
#define CHAIN_INPUT         "INPUT"
#define CHAIN_OUTPUT        "OUTPUT"
#define CHAIN_FORWARD       "FORWARD"


/******************************************************************************
*                                ENUM
******************************************************************************/
/* tbsOperateIPTables支持的动作 */
enum eAction
{
    CREATE_CHAIN = 0,
    DESTROY_CHAIN,
    ATTACH_CHAIN,
    DEATTACH_CHAIN,
    APPEND_CHAIN,
    ADD_RULE,
    INSERT_RULE,
    DELETE_RULE,
    FLUSH_CHAIN,
    RAW_COMMAND,
    INIT_CREATE_CHAIN,
    INIT_ATTACH_CHAIN
};

/******************************************************************************
*                                STRUCT
******************************************************************************/
/* 自定义链节点 */
typedef struct tag_ST_IPT_NODE
{
    int  iChainId;
    char *pcChainName;
    int  iAttachCount;
} ST_IPT_NODE;


/******************************************************************************
*                                FUNCTION
******************************************************************************/
/* iptables公共操作接口 */
int  tbsOperateIPTables(int iChain,enum eAction bAction,char *szRule);

/* 清空iptables中所有链 */
void tbsDeleteAllChains(void);
/* ip6tables公共操作接口 */
int  tbsOperateIP6Tables(int iChain,enum eAction bAction,char *szRule);

/* 初始化指定表中所有链 */
int TF_FW_InitTableChains(ST_IPT_NODE *pstChainList, int iCount);
/* 初始化指定iptables/ip6tables表 */
int TF_FW_InitAllChains();

#endif /*__IPT_COMM__*/

