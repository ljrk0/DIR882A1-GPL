/**********************************************************************
 Copyright (c), 1991-2008, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 �ļ�����: ipt_common.h
 �ļ�����: ����iptables�ӿ�ͷ�ļ�
 �޶���¼:
           1 ���� : ����
             ���� : 2009-03-11
             ���� : ����

           2 ���� : ������
             ���� : 2009-07-01
             ���� : ����ע��
**********************************************************************/

#ifndef __IPT_COMM__
#define __IPT_COMM__


/******************************************************************************
*                                 MACRO
******************************************************************************/

/* ������󳤶� */
#define MAX_NAME_LEN        56

/*parent Chain ��Ŀ*/
#define MAX_PARENT_CHAIN_NUM 6

/* ������Ӧ��ID */
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
    CN_DROP_SPECIAL_PRE,//�����������ĩβ��һЩ֪���˿ڣ���ACL��DMZ��PTMP�ȶ�û��ƥ���ϵ�����£��ڴ�DROP��

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
    CN_RIP_INPUT, // ����SPI״̬����ǽʱ����Ҫ����rip����
    CN_SPI_INPUT, // add by linwenquan 2012-8-8

    /* Chain OUTPUT */
	CN_OUTPUT = (((unsigned int)(0x06))<<16),
	QOS_OUTPUT_MATCH,
	QOS_IP6T_OUT_MATCH,
    WAN_TR069_OUTPUT,
    TF_FW_OUTPUT,
} EN_IPT_ChainId;

/* iptables���ñ��������� */
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
/* tbsOperateIPTables֧�ֵĶ��� */
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
/* �Զ������ڵ� */
typedef struct tag_ST_IPT_NODE
{
    int  iChainId;
    char *pcChainName;
    int  iAttachCount;
} ST_IPT_NODE;


/******************************************************************************
*                                FUNCTION
******************************************************************************/
/* iptables���������ӿ� */
int  tbsOperateIPTables(int iChain,enum eAction bAction,char *szRule);

/* ���iptables�������� */
void tbsDeleteAllChains(void);
/* ip6tables���������ӿ� */
int  tbsOperateIP6Tables(int iChain,enum eAction bAction,char *szRule);

/* ��ʼ��ָ������������ */
int TF_FW_InitTableChains(ST_IPT_NODE *pstChainList, int iCount);
/* ��ʼ��ָ��iptables/ip6tables�� */
int TF_FW_InitAllChains();

#endif /*__IPT_COMM__*/

