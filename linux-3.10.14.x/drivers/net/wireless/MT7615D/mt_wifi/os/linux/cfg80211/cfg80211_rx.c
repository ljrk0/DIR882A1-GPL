#ifdef MTK_LICENSE
/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2013, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:

	Abstract:

	Revision History:
	Who 		When			What
	--------	----------		----------------------------------------------
*/
#endif /* MTK_LICENSE */
#define RTMP_MODULE_OS

#ifdef RT_CFG80211_SUPPORT

#include "rt_config.h"

#ifdef RT_CFG80211_P2P_SUPPORT
extern UCHAR CFG_P2POUIBYTE[];
#endif /* RT_CFG80211_P2P_SUPPORT */

VOID CFG80211_Announce802_3Packet(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk, UCHAR FromWhichBSSID)
{
#ifdef CONFIG_AP_SUPPORT
	if (RX_BLK_TEST_FLAG(pRxBlk, fRX_STA))
	{
		Announce_or_Forward_802_3_Packet(pAd, pRxBlk->pRxPacket, FromWhichBSSID, OPMODE_AP);
	}
	else
#endif /* CONFIG_AP_SUPPORT */
	{
		Announce_or_Forward_802_3_Packet(pAd, pRxBlk->pRxPacket, FromWhichBSSID, OPMODE_STA);
	}

}

BOOLEAN CFG80211_CheckActionFrameType(
        IN  RTMP_ADAPTER 								 *pAd,
		IN	PUCHAR										 preStr,
		IN	PUCHAR										 pData,
		IN	UINT32                              		 length)
{
	BOOLEAN isP2pFrame = FALSE;
	struct ieee80211_mgmt *mgmt;
	mgmt = (struct ieee80211_mgmt *)pData;
	if (ieee80211_is_mgmt(mgmt->frame_control))
	{
		if (ieee80211_is_probe_resp(mgmt->frame_control))
		{
			MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_INFO, ("CFG80211_PKT: %s ProbeRsp Frame %d\n", preStr, pAd->LatchRfRegs.Channel));
	        if (!mgmt->u.probe_resp.timestamp)
    		{
            		struct timeval tv;
            		do_gettimeofday(&tv);
            		mgmt->u.probe_resp.timestamp = ((UINT64) tv.tv_sec * 1000000) + tv.tv_usec;
    		}
		}
		else if (ieee80211_is_disassoc(mgmt->frame_control))
		{
			MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s DISASSOC Frame\n", preStr));
		}
		else if (ieee80211_is_deauth(mgmt->frame_control))
		{
			MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s Deauth Frame on %d\n", preStr, pAd->LatchRfRegs.Channel));
		}
		else if (ieee80211_is_action(mgmt->frame_control))
		{
			PP2P_PUBLIC_FRAME pFrame = (PP2P_PUBLIC_FRAME)pData;
			if ((pFrame->p80211Header.FC.SubType == SUBTYPE_ACTION) &&
			    (pFrame->Category == CATEGORY_PUBLIC) &&
			    (pFrame->Action == ACTION_WIFI_DIRECT))
			{
				isP2pFrame = TRUE;
				switch (pFrame->Subtype)
				{
					case GO_NEGOCIATION_REQ:
						MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s GO_NEGOCIACTION_REQ %d\n",
									preStr, pAd->LatchRfRegs.Channel));
						break;

					case GO_NEGOCIATION_RSP:
						MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s GO_NEGOCIACTION_RSP %d\n",
									preStr, pAd->LatchRfRegs.Channel));
						break;

					case GO_NEGOCIATION_CONFIRM:
						MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s GO_NEGOCIACTION_CONFIRM %d\n",
									preStr,  pAd->LatchRfRegs.Channel));
						break;

					case P2P_PROVISION_REQ:
						MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s P2P_PROVISION_REQ %d\n",
									preStr, pAd->LatchRfRegs.Channel));
						break;

					case P2P_PROVISION_RSP:
						MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s P2P_PROVISION_RSP %d\n",
									preStr, pAd->LatchRfRegs.Channel));
						break;

					case P2P_INVITE_REQ:
						MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s P2P_INVITE_REQ %d\n",
									preStr, pAd->LatchRfRegs.Channel));
						break;

					case P2P_INVITE_RSP:
						MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s P2P_INVITE_RSP %d\n",
									preStr, pAd->LatchRfRegs.Channel));
						break;
					case P2P_DEV_DIS_REQ:
                        MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s P2P_DEV_DIS_REQ %d\n",
                                                preStr, pAd->LatchRfRegs.Channel));
						break;
					case P2P_DEV_DIS_RSP:
                        MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s P2P_DEV_DIS_RSP %d\n",
                                                preStr, pAd->LatchRfRegs.Channel));
                        break;
				}
			}
			 else if ((pFrame->p80211Header.FC.SubType == SUBTYPE_ACTION) &&
					  (pFrame->Category == CATEGORY_PUBLIC) &&
					   ((pFrame->Action == ACTION_GAS_INITIAL_REQ) 	 ||
						(pFrame->Action == ACTION_GAS_INITIAL_RSP)	 ||
						(pFrame->Action == ACTION_GAS_COMEBACK_REQ ) ||
						(pFrame->Action == ACTION_GAS_COMEBACK_RSP)))
			{
											isP2pFrame = TRUE;
			}
#ifdef RT_CFG80211_P2P_SUPPORT
			else if	(pFrame->Category == CATEGORY_VENDOR_SPECIFIC_WFD)
			{
			    PP2P_ACTION_FRAME pP2PActionFrame = (PP2P_ACTION_FRAME) pData;
                if (RTMPEqualMemory(&pP2PActionFrame->Octet[2], CFG_P2POUIBYTE, 4))
                {
    				isP2pFrame = TRUE;
    				switch (pP2PActionFrame->Subtype)
    				{
                        case P2PACT_NOA:
                            MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s P2PACT_NOA %d\n",
                                                      preStr, pAd->LatchRfRegs.Channel));
    						break;
    					case P2PACT_PERSENCE_REQ:
                            MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s P2PACT_PERSENCE_REQ %d\n",
                                                      preStr, pAd->LatchRfRegs.Channel));
    						break;
    					case P2PACT_PERSENCE_RSP:
                            MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s P2PACT_PERSENCE_RSP %d\n",
                                                      preStr, pAd->LatchRfRegs.Channel));
    						break;
    					case P2PACT_GO_DISCOVER_REQ:
                            MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s P2PACT_GO_DISCOVER_REQ %d\n",
                                                      preStr, pAd->LatchRfRegs.Channel));
    						break;
    				}
                }
			}
#endif /* RT_CFG80211_P2P_SUPPORT */
			else
			{
				MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_INFO, ("CFG80211_PKT: %s ACTION Frame with Channel%d\n", preStr, pAd->LatchRfRegs.Channel));
			}
		}
		else
		{
			MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s UNKOWN MGMT FRAME TYPE\n", preStr));
		}
	}
	else
	{
		MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("CFG80211_PKT: %s UNKOWN FRAME TYPE\n", preStr));
	}

	return isP2pFrame;
}


#ifdef CFG_TDLS_SUPPORT
BOOLEAN CFG80211_HandleTdlsDiscoverRespFrame(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk, UCHAR OpMode)
{
#ifdef MT_MAC
#else
	RXWI_STRUC *pRxWI = pRxBlk->pRxWI;
	PNET_DEV pNetDev = NULL;
#endif /* MT_MAC */
	PHEADER_802_11 pHeader = pRxBlk->pHeader;
	UINT32 freq;
	//PNDIS_PACKET pRxPacket = pRxBlk->pRxPacket;
	PP2P_PUBLIC_FRAME pFrame = (PP2P_PUBLIC_FRAME)pHeader;
	UINT32 MPDUtotalByteCnt = 0;

#ifdef MT_MAC
	MPDUtotalByteCnt = pRxBlk->MPDUtotalByteCnt;
#else
	MPDUtotalByteCnt = pRxWI->RXWI_N.MPDUtotalByteCnt;
#endif /* MT_MAC */


	if ((pFrame->p80211Header.FC.SubType == SUBTYPE_ACTION) &&
		(pFrame->Category == CATEGORY_PUBLIC) &&
		(pFrame->Action == ACTION_TDLS_DISCOVERY_RSP))
	{
		MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("%s : GOT ACTION_TDLS_DISCOVERY_RSP ACTION: 0x%x \n",__FUNCTION__,pFrame->Action));
		MAP_CHANNEL_ID_TO_KHZ(pAd->LatchRfRegs.Channel, freq);
		freq /= 1000;
		CFG80211OS_RxMgmt(pAd->net_dev, freq, (PUCHAR)pHeader, MPDUtotalByteCnt);
		return TRUE;
	}

	/*
	report TDLS DiscoverRespFrame to supplicant , but it does nothing ,
	events.c line 2432
	*/

	return FALSE;


}
#endif /* CFG_TDLS_SUPPORT */


BOOLEAN CFG80211_HandleP2pMgmtFrame(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk, UCHAR OpMode)
{
#ifndef MT_MAC
	RXWI_STRUC *pRxWI = pRxBlk->pRxWI;
#endif /* !MT_MAC */
	HEADER_802_11 Header;
	PHEADER_802_11 pHeader = &Header;
	
	//MAC_TABLE_ENTRY *pEntry = MacTableLookup(pAd,pRxBlk->Addr2);
	struct wifi_dev *pWdev = WdevSearchByBssid(pAd,pRxBlk->Addr1);
	PNET_DEV pNetDev;

	PCFG80211_CTRL pCfg80211_ctrl = &pAd->cfg80211_ctrl;
	UINT32 freq;
	UINT32 MPDUtotalByteCnt = 0;
	
	if(pWdev == NULL)
	{
		MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_LOUD, 
			("%s return , can't find wdev for %02x:%02x:%02x:%02x:%02x:%02x\n"
			, __FUNCTION__, PRINT_MAC(pHeader->Addr2)));
		pNetDev = CFG80211_GetEventDevice(pAd);
	}
	else
	{
		pNetDev = pWdev->if_dev;
		
	}
	NdisZeroMemory(&Header, sizeof(HEADER_802_11));
	NdisCopyMemory((UCHAR *)&(pHeader->FC), pRxBlk->FC, sizeof(FRAME_CONTROL));
	NdisCopyMemory(pHeader->Addr1, pRxBlk->Addr1, MAC_ADDR_LEN);
	NdisCopyMemory(pHeader->Addr2, pRxBlk->Addr2, MAC_ADDR_LEN);
	NdisCopyMemory(pHeader->Addr3, pRxBlk->Addr3, MAC_ADDR_LEN);
	pHeader->Duration = pRxBlk->Duration;

#ifdef MT_MAC
	MPDUtotalByteCnt = pRxBlk->MPDUtotalByteCnt;
#else
	MPDUtotalByteCnt = pRxWI->RXWI_N.MPDUtotalByteCnt;
#endif /* MT_MAC */

	if ((pHeader->FC.SubType == SUBTYPE_PROBE_REQ) ||
	 	 ((pHeader->FC.SubType == SUBTYPE_ACTION) &&
	 	   CFG80211_CheckActionFrameType(pAd, "RX", (PUCHAR)pHeader, MPDUtotalByteCnt)))
		{
			MAP_CHANNEL_ID_TO_KHZ(pAd->LatchRfRegs.Channel, freq);
			freq /= 1000;

#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
			/* Check the P2P_GO exist in the VIF List */
			if (pCfg80211_ctrl->Cfg80211VifDevSet.vifDevList.size > 0)
			{
				if ((pNetDev = RTMP_CFG80211_FindVifEntry_ByType(pAd, RT_CMD_80211_IFTYPE_P2P_GO)) != NULL)
				{
					MTWF_LOG(DBG_CAT_P2P, DBG_SUBCAT_ALL, DBG_LVL_INFO, ("VIF STA GO RtmpOsCFG80211RxMgmt OK!! TYPE = %d, freq = %d, %02x:%02x:%02x:%02x:%02x:%02x\n",
									  pHeader->FC.SubType, freq, PRINT_MAC(pHeader->Addr2)));
					CFG80211OS_RxMgmt(pNetDev, freq, (PUCHAR)pHeader, MPDUtotalByteCnt);

					if (OpMode == OPMODE_AP)
						return TRUE;
				}
			}
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */

			if ( ((pHeader->FC.SubType == SUBTYPE_PROBE_REQ) &&
                 (pCfg80211_ctrl->cfg80211MainDev.Cfg80211RegisterProbeReqFrame == TRUE) ) ||
			     ((pHeader->FC.SubType == SUBTYPE_ACTION)  /*&& ( pAd->Cfg80211RegisterActionFrame == TRUE)*/ ))
			{
				MTWF_LOG(DBG_CAT_P2P, DBG_SUBCAT_ALL, DBG_LVL_INFO,("MAIN STA RtmpOsCFG80211RxMgmt OK!! TYPE = %d, freq = %d, %02x:%02x:%02x:%02x:%02x:%02x\n",
										pHeader->FC.SubType, freq, PRINT_MAC(pHeader->Addr2)));
#ifdef DISABLE_HOSTAPD_PROBE_RESP
#if defined BAND_STEERING || defined BAND_STEERING_PLUS

				if((pHeader->FC.SubType == SUBTYPE_PROBE_REQ) && (pAd->ApCfg.BandSteering == TRUE))
				{
						/* retreive Ht Support, Vht Support and RxMCSBitmask from probe request */
						PFRAME_802_11 Fr = (PFRAME_802_11)pRxBlk->FC;
						UCHAR		*Ptr;
   						UCHAR		eid =0, eid_len = 0, *eid_data;
						UINT	total_ie_len = 0;
						BOOLEAN IsHtSupport;
						BOOLEAN IsVhtSupport;
						UINT32 RxMCSBitmask = 0;
						struct wifi_dev* wdev = pWdev;
						struct raw_rssi_info rssi_info;
						RXD_BASE_STRUCT *rxd_base = (RXD_BASE_STRUCT *)pRxBlk->rmac_info;

						BOOLEAN bBndStrgCheck = FALSE;
						rssi_info.raw_rssi[0] = pRxBlk->rx_signal.raw_rssi[0];
						rssi_info.raw_rssi[1] = pRxBlk->rx_signal.raw_rssi[1];
						rssi_info.raw_rssi[2] = pRxBlk->rx_signal.raw_rssi[2];
						rssi_info.raw_rssi[3] = pRxBlk->rx_signal.raw_rssi[3];
						rssi_info.raw_snr = min(pRxBlk->rx_signal.raw_snr[0], pRxBlk->rx_signal.raw_snr[1]);
    					rssi_info.Channel =	(rxd_base != NULL) ? rxd_base->RxD1.ChFreq : 0;
						 
						Ptr = Fr->Octet;
    					eid = Ptr[0];
    					eid_len = Ptr[1];
						total_ie_len = eid_len + 2;
						eid_data = Ptr+2;
    
   					    /* get variable fields from payload and advance the pointer*/
						while((eid_data + eid_len) <= ((UCHAR*)Fr + pRxBlk->DataSize))
    					{    
        					switch(eid)
        					{
								case IE_HT_CAP:
									if (pAd->ApCfg.BandSteering != TRUE)
										break;
									if (eid_len >= SIZE_HT_CAP_IE)
									{
										IsHtSupport = TRUE;
										RxMCSBitmask = *(UINT32 *)(eid_data + 3);
									}
									else
										MTWF_LOG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_WARN, ("%s() - wrong IE_HT_CAP. eid_len = %d\n", __FUNCTION__, eid_len));
									break;			
							
								case IE_VHT_CAP:
									if (pAd->ApCfg.BandSteering != TRUE)
										break;
									if (eid_len >= SIZE_OF_VHT_CAP_IE)
										IsVhtSupport = TRUE;
									else
										MTWF_LOG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_WARN, ("%s() - wrong IE_VHT_CAP. eid_len = %d\n", __FUNCTION__, eid_len));
									break;			
								
        					}
							eid = Ptr[total_ie_len];
    						eid_len = Ptr[total_ie_len + 1];
							eid_data = Ptr + total_ie_len + 2;
							total_ie_len += (eid_len + 2);
						}
						if(pWdev == NULL)
								wdev = pAd->wdev_list[0];
						if(IS_BROADCAST_MAC_ADDR(pRxBlk->Addr1))
						{
							int apidx;
							for(apidx=0; apidx<pAd->ApCfg.BssidNum; apidx++)
							{
								BSS_STRUCT *mbss = &pAd->ApCfg.MBSSID[apidx];
								wdev = &mbss->wdev;
								if( wdev->channel == rssi_info.Channel ){
#ifdef BAND_STEERING									
									bBndStrgCheck = CFG80211_HandleBndStrProbeReq(pAd, wdev, pRxBlk,IsHtSupport,IsVhtSupport,RxMCSBitmask,rssi_info);
#endif
#ifdef BAND_STEERING_PLUS
								{
									MLME_QUEUE_ELEM Elem = { {0} };
									PEER_PROBE_REQ_PARAM ProbeReqParam = { {0} };
									ProbeReqParam.IsHtSupport = IsHtSupport;
									ProbeReqParam.IsVhtSupport = IsVhtSupport;
									ProbeReqParam.RxMCSBitmask = RxMCSBitmask;
									Elem.rssi_info.raw_rssi[0] = rssi_info.raw_rssi[0];
									Elem.rssi_info.raw_rssi[1] = rssi_info.raw_rssi[1];
									Elem.rssi_info.raw_rssi[2] = rssi_info.raw_rssi[2];
									Elem.rssi_info.raw_rssi[3] = rssi_info.raw_rssi[3];
									Elem.MsgType = APMT2_PEER_PROBE_REQ;
									bBndStrgCheck = BndStrg_CheckConnectionReq(pAd, wdev, pRxBlk->Addr2, &Elem, &ProbeReqParam);
								}
#endif
									if (bBndStrgCheck == FALSE)
										return TRUE;
								}
							}
						}
						else{
#ifdef BAND_STEERING							
							bBndStrgCheck = CFG80211_HandleBndStrProbeReq(pAd, wdev, pRxBlk,IsHtSupport,IsVhtSupport,RxMCSBitmask,rssi_info);
#endif
#ifdef BAND_STEERING_PLUS
						{
							MLME_QUEUE_ELEM Elem = { {0} };
							PEER_PROBE_REQ_PARAM ProbeReqParam = { {0} };
							ProbeReqParam.IsHtSupport = IsHtSupport;
							ProbeReqParam.IsVhtSupport = IsVhtSupport;
							ProbeReqParam.RxMCSBitmask = RxMCSBitmask;
							Elem.rssi_info.raw_rssi[0] = rssi_info.raw_rssi[0];
							Elem.rssi_info.raw_rssi[1] = rssi_info.raw_rssi[1];
							Elem.rssi_info.raw_rssi[2] = rssi_info.raw_rssi[2];
							Elem.rssi_info.raw_rssi[3] = rssi_info.raw_rssi[3];
							Elem.MsgType = APMT2_PEER_PROBE_REQ;
							bBndStrgCheck = BndStrg_CheckConnectionReq(pAd, wdev, pRxBlk->Addr2, &Elem, &ProbeReqParam);
						}
#endif
						}
						if (bBndStrgCheck == FALSE)
							return TRUE;
				}
#endif /* BAND_STEERING */
				if((pHeader->FC.SubType == SUBTYPE_PROBE_REQ) && IS_BROADCAST_MAC_ADDR(pRxBlk->Addr1))
				{
					int apidx;
					/*deliver broadcast frame to all virtual interface */
					for(apidx=0; apidx<pAd->ApCfg.BssidNum; apidx++)
					{
						BSS_STRUCT *mbss = &pAd->ApCfg.MBSSID[apidx];
						struct wifi_dev *wdev = &mbss->wdev;
						if(wdev->if_dev != NULL)
						{
							CFG80211OS_RxMgmt(wdev->if_dev, freq, pRxBlk->pData, MPDUtotalByteCnt);
						}
					}
				}
				else
					CFG80211OS_RxMgmt(pNetDev, freq, pRxBlk->pData, MPDUtotalByteCnt);
#else
					CFG80211OS_RxMgmt(pNetDev, freq, pRxBlk->pData, MPDUtotalByteCnt);
#endif

				if (OpMode == OPMODE_AP)
						return TRUE;
			}
		}

	return FALSE;
}


#endif /* RT_CFG80211_SUPPORT */

