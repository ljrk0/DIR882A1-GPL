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
    	driver_wext.c
*/

#include "bndstrg.h"
#include "driver_wext.h"
#include "priv_netlink.h"
#include "wireless_copy.h"
#include "netlink.h"
#include <sys/ioctl.h>


static void event_handle(struct driver_wext_data *drv_data, char *buf)
{
	struct bndstrg *bndstrg = (struct bndstrg *)drv_data->priv;

	bndstrg->event_ops->event_handle(bndstrg, buf);
}

static void wnm_event_handle(struct driver_wext_data *drv_data, char *buf)
{
	struct bndstrg *bndstrg = (struct bndstrg *)drv_data->priv;

	bndstrg->event_ops->wnm_event_handle(bndstrg, buf);
}

static void driver_wext_event_wireless(struct driver_wext_data *drv,
                 void *ctx, char *data, int len)
{               
    struct iw_event iwe_buf, *iwe = &iwe_buf;
    char *pos, *end, *custom, *buf /*,*assoc_info_buf, *info_pos */;

    /* info_pos = NULL; */
	/* assoc_info_buf = NULL; */
    pos = data;
    end = data + len;   
    
    while (pos + IW_EV_LCP_LEN <= end) {
        /* 
 		 * Event data may be unaligned, so make a local, aligned copy
         * before processing. 
         */
        os_memcpy(&iwe_buf, pos, IW_EV_LCP_LEN);
		
		DBGPRINT(DEBUG_INFO, "cmd = 0x%x len = %d\n", iwe->cmd, iwe->len);
        
		if (iwe->len <= IW_EV_LCP_LEN)
            return;

        custom = pos + IW_EV_POINT_LEN;

        //if (drv->we_version_compiled > 18 && iwe->cmd == IWEVCUSTOM) {
            /* WE-19 removed the pointer from struct iw_point */
            char *dpos = (char *) &iwe_buf.u.data.length;
            int dlen = dpos - (char *) &iwe_buf;
            os_memcpy(dpos, pos + IW_EV_LCP_LEN,
                  sizeof(struct iw_event) - dlen);
        //} else {
            //os_memcpy(&iwe_buf, pos, sizeof(struct iw_event));
            //custom += IW_EV_POINT_OFF;
		//}
		
		switch (iwe->cmd) {
        case IWEVCUSTOM:
			if (custom + iwe->u.data.length > end)
               	return;
           	buf = os_malloc(iwe->u.data.length + 1);
            if (buf == NULL)
                return;
            os_memcpy(buf, custom, iwe->u.data.length);
            buf[iwe->u.data.length] = '\0';

            switch (iwe->u.data.flags) {
			case OID_BNDSTRG_MSG:
				event_handle(drv, buf); 
				break;
			case OID_802_11_WNM_EVENT:
				DBGPRINT(DEBUG_TRACE, "WNM_EVENT\n");
				wnm_event_handle(drv,buf);
				break;
			default:
				//DBGPRINT(DEBUG_TRACE, "unkwnon event type(%d)\n", iwe->u.data.flags);
				break; 
			}

           	os_free(buf);
            break;
        }

        pos += iwe->len;
    }
}

static int driver_wext_set_oid(struct driver_wext_data *drv_data, const char *ifname,
              				   unsigned short oid, char *data, size_t len)    
{
    char *buf;                             
    struct iwreq iwr;
	
    buf = os_zalloc(len);

    os_memset(&iwr, 0, sizeof(iwr));       
    os_strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.data.flags = oid;
    iwr.u.data.flags |= OID_GET_SET_TOGGLE;

    if (data)
        os_memcpy(buf, data, len);

	if (buf) {
    	iwr.u.data.pointer = (caddr_t)buf;    
    	iwr.u.data.length = len;
	} else {
    	iwr.u.data.pointer = NULL;    
    	iwr.u.data.length = 0;
	}

    if (ioctl(drv_data->ioctl_sock, RT_PRIV_IOCTL, &iwr) < 0) {
        DBGPRINT(DEBUG_ERROR, " oid=0x%x len (%zd) failed\n", oid, len);
        os_free(buf);
        return -1;
    }

    os_free(buf);
    return 0;
}


/*
 * Test bndstrg cmd
 */
static int driver_wext_test(void *drv_data, const char *ifname)
{
	int ret;
	struct driver_wext_data *drv_wext_data = (struct driver_wext_data *)drv_data;	

	DBGPRINT(DEBUG_OFF, "\n");

	ret = driver_wext_set_oid(drv_wext_data, ifname, OID_BNDSTRG_TEST, "123", 4);

	return ret;
}

static int driver_wext_accessible_cli(
				void *drv_data,
				struct bndstrg_iface *inf,
				struct bndstrg_cli_entry *entry,
				u8 action)
{
	int ret;
	struct driver_wext_data *drv_wext_data = \
					(struct driver_wext_data *)drv_data;	
	struct bndstrg_msg msg = { 0 };

	DBGPRINT(DEBUG_TRACE, "\n");
    msg.Action = action;

    switch (action)
    {
    case CLI_ADD:
        {
            struct bnd_msg_cli_add *cli_add = &msg.data.cli_add;
            memcpy(cli_add->Addr, entry->Addr, MAC_ADDR_LEN);
    	    cli_add->TableIndex = entry->TableIndex;
			inf->driver_table_size++;
        }
        break;

     case CLI_DEL:
        {
            struct bnd_msg_cli_del *cli_del = &msg.data.cli_del;
            memcpy(cli_del->Addr, entry->Addr, MAC_ADDR_LEN);
    	    cli_del->TableIndex = entry->TableIndex;
			inf->driver_table_size--;
        }
        break;

    }

	ret = driver_wext_set_oid(
				drv_wext_data,
				(char *)inf->ucIfName,
				OID_BNDSTRG_MSG,
				(char *) &msg,
				sizeof(struct bndstrg_msg));

	return ret;
}

static int driver_wext_nvram_update(
				void *drv_data,
				const char *ifname,
				struct bndstrg_nvram_client *entry,
				u8 action)
{
	int ret;
	struct driver_wext_data *drv_wext_data = \
					(struct driver_wext_data *)drv_data;	
	struct bndstrg_msg msg = { 0 };
    struct bnd_msg_nvram_entry_update *entry_update = &msg.data.entry_update;

	memcpy(entry_update->nvram_entry.Addr, entry->Addr, MAC_ADDR_LEN);
	memcpy(entry_update->Addr, entry->Addr, MAC_ADDR_LEN);
	entry_update->nvram_entry.Band = entry->Band;
	entry_update->nvram_entry.Manipulable = entry->Manipulable ;
	entry_update->nvram_entry.Nss = entry->Nss ;
	entry_update->nvram_entry.PhyMode = entry->PhyMode;
	msg.Action = action;
	ret = driver_wext_set_oid(
				drv_wext_data,
				ifname,
				OID_BNDSTRG_MSG,
				(char *) &msg,
				sizeof(struct bndstrg_msg));
	return ret;
}

static int driver_wext_inf_status_query(
				void *drv_data,
				const char *ifname, u8 action)
{
	int ret;
	struct driver_wext_data *drv_wext_data = \
					(struct driver_wext_data *)drv_data;	
	struct bndstrg_msg msg = { 0 };

	DBGPRINT(DEBUG_TRACE, "\n");
    msg.Action = action;

    switch (action)
    {
	    case INF_STATUS_QUERY:
        {            
			struct bnd_msg_inf_status_req *inf_status_req = &msg.data.inf_status_req;
			strcpy((char*)inf_status_req->ucIfName, ifname);
            
        }
        break;

	    case HEARTBEAT_MONITOR:
        {
            struct bnd_msg_heartbeat *heartbeat = &msg.data.heartbeat;
			strcpy((char*)heartbeat->ucIfName, ifname);
        }
        break;
    }	

	ret = driver_wext_set_oid(
				drv_wext_data,
				ifname,
				OID_BNDSTRG_MSG,
				(char *) &msg,
				sizeof(struct bndstrg_msg));
	DBGPRINT(DEBUG_TRACE, "ret = %u\n", ret);
	return ret;
}

static int driver_update_white_black_list(
				void *drv_data,
				const char *ifname, unsigned char *addr, u8 list_type, u8 deladd)
{
	int ret;
	struct driver_wext_data *drv_wext_data = \
					(struct driver_wext_data *)drv_data;	
	struct bndstrg_msg msg = { 0 };
    struct bnd_msg_update_white_black_list *update_list = &msg.data.update_white_black_list;

	DBGPRINT(DEBUG_TRACE, "\n");
    msg.Action = UPDATE_WHITE_BLACK_LIST;
	strcpy((char*)update_list->ucIfName, ifname);
	update_list->list_type = list_type;
	update_list->deladd = deladd;
	memcpy(update_list->Addr, addr, MAC_ADDR_LEN);
	ret = driver_wext_set_oid(
				drv_wext_data,
				ifname,
				OID_BNDSTRG_MSG,
				(char *) &msg,
				sizeof(struct bndstrg_msg));
	DBGPRINT(DEBUG_TRACE, "ret = %u\n", ret);
	return ret;
}

static int driver_wext_chanload_status_query(
				void *drv_data,
				const char *ifname)
{
	int ret;
	struct driver_wext_data *drv_wext_data = \
			(struct driver_wext_data *)drv_data;	
	struct bndstrg_msg msg = { 0 };
    struct bnd_msg_chanload_status_req *chanload_status_req = &msg.data.chanload_status_req;

	DBGPRINT(DEBUG_TRACE, "\n");
	msg.Action = CHANLOAD_STATUS_REQ;
    strcpy((char*)chanload_status_req->ucIfName, ifname);

	ret = driver_wext_set_oid(
			  drv_wext_data,
			  ifname,
			  OID_BNDSTRG_MSG,
			  (char *) &msg,
			  sizeof(struct bndstrg_msg));
	DBGPRINT(DEBUG_TRACE, "ret = %u\n", ret);
	return ret;
}

static int driver_wext_bndstrg_onoff(
				void *drv_data,
				const char *ifname,
				u8 onoff,
				u8 mode)
{
	int ret;
	struct driver_wext_data *drv_wext_data = \
					(struct driver_wext_data *)drv_data;	
	struct bndstrg_msg msg = { 0 };
    struct bnd_msg_onoff *msg_onoff = &msg.data.onoff;

	DBGPRINT(DEBUG_TRACE, "\n");
	msg.Action = BNDSTRG_ONOFF;
	msg_onoff->OnOff = onoff;
	msg_onoff->BndStrgMode = mode;
    strcpy((char*)msg_onoff->ucIfName, ifname);

	ret = driver_wext_set_oid(
				drv_wext_data,
				ifname,
				OID_BNDSTRG_MSG,
				(char *) &msg,
				sizeof(struct bndstrg_msg));

	return ret;
}

static void driver_wext_event_rtm_newlink(void *ctx, struct ifinfomsg *ifi,
                    					  u8 *buf, size_t len)
{
    struct driver_wext_data *drv = ctx;
    int attrlen, rta_len;
    struct rtattr *attr;
    
    attrlen = len;

   	DBGPRINT(DEBUG_TRACE, "attrlen=%d", attrlen);

    attr = (struct rtattr *) buf;
    rta_len = RTA_ALIGN(sizeof(struct rtattr));
    while (RTA_OK(attr, attrlen)) {
        DBGPRINT(DEBUG_TRACE, "rta_type=%02x\n", attr->rta_type);
        if (attr->rta_type == IFLA_WIRELESS) {
            driver_wext_event_wireless(
                drv, ctx,
                ((char *) attr) + rta_len,
                attr->rta_len - rta_len);
        }
        attr = RTA_NEXT(attr, attrlen);
    }
}

int driver_wnm_send_btm_req(
		struct bndstrg *bndstrg,
		struct bndstrg_iface *inf,
		struct bndstrg_iface *destination_inf,
		struct bndstrg_cli_entry *entry)
{
	int len, bndstrg_msg_len=0;
	struct driver_wext_data *drv_wext_data = \
					(struct driver_wext_data *)bndstrg->drv_data;
	struct bndstrg_cli_table *table = &bndstrg->table;
	struct bndstrg_msg msg = { 0 };
    struct wnm_command *cmd_data = (struct wnm_command *)&msg.data.wnm_cmd_data;
	struct btm_req_data *btm_req_data;
	int btm_req_len = 4;
	u8 btm_req[255] = { 0 };
	struct btm_req_frame *frame = (struct btm_req_frame *)btm_req;
	int ret;

	bndstrg_msg_len = sizeof(struct bndstrg_msg);
	len = sizeof(struct wnm_command) + sizeof(struct btm_req_data) + btm_req_len + sizeof(RRM_NEIGHBOR_REP_INFO) ;
	if(len > bndstrg_msg_len)
	{
		DBGPRINT(DEBUG_OFF, "WNM Cmd larger than bndstrg msg, can't send it\n");
		return TRUE;
	}
	//msg = (struct bndstrg_msg *)os_zalloc(bndstrg_msg_len);
	//cmd_data = (struct wnm_command *)&msg->data.wnm_cmd_data;
	cmd_data->command_id = OID_802_11_WNM_CMD_SEND_BTM_REQ;
	cmd_data->command_len = len;

	btm_req_data = (struct btm_req_data *)cmd_data->command_body;
	//btm_req_data->ifindex = ifindex;
	os_memcpy(btm_req_data->peer_mac_addr, entry->Addr,MAC_ADDR_LEN);
	btm_req_data->btm_req_len = btm_req_len;
	
	frame->request_mode = (frame->request_mode & ~0x01) | (1 << 0); //Neighbor report added
	frame->request_mode = (frame->request_mode & ~0x02) | (1 << 1); //set abridge bit (exclude bss not present in neighbour list)
	if(table->BtmMode & BTM_Force_Steer){
		frame->request_mode = (frame->request_mode & ~0x04) | (1 << 2);	//imminent disassoc enable
	//frame->request_mode = (frame->request_mode & ~0x04);	//imminent disassoc disablse
	//frame->request_mode = (frame->request_mode & ~0x08) | (1 << 3);	//bss termnation value included
		frame->disassociation_timer = 20; /* 20 TBTT ~= 2 sec Timer */
	}
	frame->validity_interval = 200;

	os_memcpy(btm_req_data->btm_req,btm_req,btm_req_len );
	os_memcpy((btm_req_data->btm_req + btm_req_len),&destination_inf->NeighborRepInfo,sizeof(RRM_NEIGHBOR_REP_INFO));
	msg.Action = BNDSTRG_WNM_BTM;
	ret = driver_wext_set_oid(
				drv_wext_data,
				(char *)inf->ucIfName,
				OID_BNDSTRG_MSG,
				(char *) &msg,
				bndstrg_msg_len);

	//os_free(cmd_data);
	entry->btm_info.BTMState = btm_req_tx;
#ifdef BND_STRG_QA
	BND_STRG_PRINTQAMSG(table, entry, ("BTM Request Send {BtmMode:%d}\n"), table->BtmMode);
#endif

	return ret;

}

static void *driver_wext_init(struct bndstrg *bndstrg, 
						      const int opmode,
							  const int drv_mode)
{
	struct driver_wext_data *drv_wext_data;
#if 1
	struct netlink_config *cfg;
#endif
	DBGPRINT(DEBUG_TRACE, "\n");
	
	drv_wext_data = calloc(1, sizeof(*drv_wext_data));
	
	if (!drv_wext_data) {
		DBGPRINT(DEBUG_ERROR, "No avaliable memory for driver_wext_data\n");
		goto err1;

	}

	DBGPRINT(DEBUG_OFF, "Initialize ralink wext interface\n");

	drv_wext_data->ioctl_sock = socket(PF_INET, SOCK_DGRAM, 0);
	
	if (drv_wext_data->ioctl_sock < 0) {
		DBGPRINT(DEBUG_ERROR, "socket(PF_INET,SOCK_DGRAM)");
		goto err2;
	}
#if 1
	cfg = os_zalloc(sizeof(*cfg));

    if (!cfg) {
		DBGPRINT(DEBUG_ERROR, "No avaliable memory for netlink cfg\n");
        goto err3;
    }

	cfg->ctx = drv_wext_data;
	cfg->newlink_cb = driver_wext_event_rtm_newlink;

	drv_wext_data->netlink = netlink_init(cfg);

	if (!drv_wext_data->netlink) {
		DBGPRINT(DEBUG_ERROR, "wext netlink init fail\n");
		goto err3;
	}
#endif

	drv_wext_data->priv = (void *)bndstrg;

	return (void *)drv_wext_data;

err3:
	close(drv_wext_data->ioctl_sock);
err2:
	os_free(drv_wext_data);
err1:
	return NULL;
}

static int driver_wext_exit(struct bndstrg *bndstrg)
{
	struct driver_wext_data *drv_wext_data = bndstrg->drv_data;

	DBGPRINT(DEBUG_TRACE, "\n");

	netlink_deinit(drv_wext_data->netlink);

	close(drv_wext_data->ioctl_sock);
	
	os_free(drv_wext_data);

	return 0;
}

const struct bndstrg_drv_ops bndstrg_drv_wext_ops = {
	.drv_inf_init = driver_wext_init,
	.drv_inf_exit = driver_wext_exit,
	.drv_test = driver_wext_test,
	.drv_accessible_cli = driver_wext_accessible_cli,
	.drv_inf_status_query = driver_wext_inf_status_query,
	.drv_bndstrg_onoff = driver_wext_bndstrg_onoff,
	.drv_nvram_update = driver_wext_nvram_update,
	.drv_chanload_status_query = driver_wext_chanload_status_query,
	.drv_send_btm_req = driver_wnm_send_btm_req,
    .drv_update_white_black_list = driver_update_white_black_list,
};

