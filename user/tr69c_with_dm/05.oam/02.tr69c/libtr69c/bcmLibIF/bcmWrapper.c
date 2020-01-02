/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom Corporation and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#define _XOPEN_SOURCE
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <syslog.h>

#include "../inc/tr69cdefs.h" /* defines for ACS state */
#include "../inc/appdefs.h"

#include "bcmWrapper.h"
#include "cms_image.h"
#include "cms_msg.h"
#include "adslctlapi.h"
#include "bcmConfig.h"

/*#define DEBUG*/

extern int sysGetSdramSize( void );   /* in cms_utils/xboard_api.c */
extern void downloadStop(char *msg, int status);  /* in httpDownload.c */
extern void clearInformEventList(void);
extern ACSState   acsState;
extern TransferInfo transferList;
extern void *msgHandle;
extern InformEvList informEvList;
extern UINT32 addInformEventToList(UINT8 event);
extern void setsaveConfigFlag(UBOOL8 flagh); /* in RPCState.h */
extern void saveConfigurations(void); /* in RPCState.h */
extern void changeNameSpaceCwmpVersionURL(int version);

#ifdef DMP_DSLDIAGNOSTICS_1
DslLoopDiagData *pDslLoopDiagResults=NULL;
extern CmsRet rutWan_getAdslLoopDiagResultAndLinkUp(DslLoopDiagData *pResult);
#endif


struct informEvtTableEntry informEvtStringTable[] = {
   {INFORM_EVENT_BOOTSTRAP,               "0 BOOTSTRAP"},
   {INFORM_EVENT_BOOT,                    "1 BOOT" },
   {INFORM_EVENT_PERIODIC,                "2 PERIODIC"},
   {INFORM_EVENT_SCHEDULED,               "3 SCHEDULED"},
   {INFORM_EVENT_VALUE_CHANGE,            "4 VALUE CHANGE"},
   {INFORM_EVENT_KICKED,                  "5 KICKED"},
   {INFORM_EVENT_CONNECTION_REQUEST,      "6 CONNECTION REQUEST"},
   {INFORM_EVENT_TRANSER_COMPLETE,        "7 TRANSFER COMPLETE"},
   {INFORM_EVENT_DIAGNOSTICS_COMPLETE,    "8 DIAGNOSTICS COMPLETE"},
   {INFORM_EVENT_REBOOT_METHOD,           "M Reboot"},
   {INFORM_EVENT_SCHEDULE_METHOD,         "M ScheduleInform"},
   {INFORM_EVENT_DOWNLOAD_METHOD,         "M Download"},
   {INFORM_EVENT_UPLOAD_METHOD,           "M Upload"},
   {INFORM_EVENT_REQUEST_DOWNLOAD,        "9 REQUEST DOWNLOAD"},
   {INFORM_EVENT_AUTON_TRANSFER_COMPLETE, "10 AUTONOMOUS TRANSFER COMPLETE"},
   {INFORM_EVENT_DU_CHANGE_COMPLETE, "11 DU STATE CHANGE COMPLETE"},
   {INFORM_EVENT_AUTON_DU_CHANGE_COMPLETE, "12 AUTONOMOUS DU STATE CHANGE COMPLETE"},
   {INFORM_EVENT_SCHEDULE_DOWNLOAD_METHOD, "M ScheduleDownload"},
   {INFORM_EVENT_CHANGE_DU_CHANGE_METHOD,  "M ChangeDUState"},
   {INFORM_EVENT_VENDOR_ALARM, NULL}
};

#define NUM_INFORM_EVT_STRING_TABLE_ENTRIES (sizeof(informEvtStringTable)/sizeof(struct informEvtTableEntry))

const char *getInformEvtString(UINT32 evt)
{
   UINT32 i;

   for (i=0; i < NUM_INFORM_EVT_STRING_TABLE_ENTRIES; i++)
   {
      if (informEvtStringTable[i].informEvt == evt)
      {
         if (!informEvtStringTable[i].str)
         {
            informEvtStringTable[i].str = getInformVendorEvtString(informEvtStringTable[i].informEvt);
         }
         return informEvtStringTable[i].str;
      }
   }

   cmsLog_error(TR069_ALARM_PROCESS_ERROR,"Unsupported Inform Event value %d", evt);
   return "Internal Error (getInformEvtString)";
}

void setInformState(eInformState state)
{
   cmsLog_debug("set informState=%d", state);

   if (informState != state)
   {
      informState = state;
      saveTR69StatusItems();
   }
}  /* End of setInformState() */

/*
* Save the TR69 state values across the reboot
*/
void saveTR69StatusItems(void)
{
   GWStateData gwState;
   ACSState    *a = &acsState;

   /* init strings */
   memset(&gwState, 0, sizeof(GWStateData));

   /* fill State Data structure from acsState data */
   if (a->downloadCommandKey)
   {
      strncpy(gwState.downloadCommandKey, a->downloadCommandKey, sizeof(gwState.downloadCommandKey));
   }
   if (a->rebootCommandKey)
   {
      strncpy(gwState.rebootCommandKey, a->rebootCommandKey, sizeof(gwState.rebootCommandKey));
   }
   if (a->dlFaultMsg)
   {
       strncpy(gwState.dlFaultMsg, a->dlFaultMsg, sizeof(gwState.dlFaultMsg));
   }
   gwState.contactedState = informState;
   gwState.dlFaultStatus  = a->dlFaultStatus;
   gwState.startDLTime    = a->startDLTime;
   gwState.endDLTime      = a->endDLTime;
   gwState.retryCount    = a->retryCount;
   
   /* save inform event list */
   /* Verify that our runtime inform event list does not overflow the list
    * on flash.
    */
   {
      int i;
      int flashEvCount = sizeof(gwState.informEvList)/sizeof(unsigned char);
	  int informEvMaxCount = sizeof(informEvList.informEvList)/sizeof(unsigned char);

      if (informEvList.informEvCnt > flashEvCount)
      {
         cmsLog_error(TR069_ALARM_PROCESS_ERROR,"Runtime Event list size is %d, flash list size is %d, truncating!!",
               informEvList.informEvCnt, flashEvCount);
         gwState.informEvCount = flashEvCount;
      }
      else
      {
		 if(informEvList.informEvCnt > informEvMaxCount )
		 {
			cmsLog_error(TR069_ALARM_PROCESS_ERROR,"Impossible process branch, Event list size is %d, buffer overflow %d, truncating!!",
                         informEvList.informEvCnt, informEvMaxCount);
			 gwState.informEvCount = informEvMaxCount;
		 }
		 else
		 {
			cmsLog_notice("saving %d events", informEvList.informEvCnt);
			gwState.informEvCount = informEvList.informEvCnt;
		 }
      }

      for (i=0; i<gwState.informEvCount; i++)
      {
         gwState.informEvList[i] = informEvList.informEvList[i];
         cmsLog_notice("%d: saving (code=%d) %s", i, gwState.informEvList[i],
                      getInformEvtString(gwState.informEvList[i]));
      }
   }

   gwState.cwmpVersion = acsState.cwmpVersion;
   gwState.dataModel = acsState.dataModel;

   if (cmsPhl_setGWStateData(&gwState) != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"Unable to save TR69 status in scratch PAD");
   }
}  /* End of saveTR69StatusItems() */


void retrieveTR69StatusItems(void)
{
   GWStateData *objValue;
   SINT32 count, i;
   int informEvCnt=0;

   acsState.cwmpVersion = CWMP_VERSION_1_2;

   count = cmsPhl_getGWStateData((void**)&objValue);

   if (count != 0)
   {
      cmsLog_notice("No GWStateData info found in DB.");
      CMSMEM_FREE_BUF_AND_NULL_PTR(objValue);
      return;
   }
   
   /* check to make sure the data model mode hasn't changed from the last time tr69c ran */
   if (acsState.dataModel != objValue->dataModel)
   {
      /* erase all previously stored acsState, and PSP data */
      /* the old state is no long relevant because data model has changed. */
      cmsPhl_setGWStateData(NULL);
      CMSMEM_FREE_BUF_AND_NULL_PTR(objValue);
      return;
   }

   if (acsState.downloadCommandKey)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.downloadCommandKey);
      acsState.downloadCommandKey = NULL;
   }
   if (strlen(objValue->downloadCommandKey) > 0)
   {
      acsState.downloadCommandKey = cmsMem_strdup(objValue->downloadCommandKey);
   }

   if (acsState.rebootCommandKey)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.rebootCommandKey);
      acsState.rebootCommandKey = NULL;
   }
   if (strlen(objValue->rebootCommandKey) > 0)
   {
      acsState.rebootCommandKey = cmsMem_strdup(objValue->rebootCommandKey);
   }

   if (acsState.dlFaultMsg)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.dlFaultMsg);
      acsState.dlFaultMsg = NULL;
   }
   if (strlen(objValue->dlFaultMsg) > 0)
   {
      acsState.dlFaultMsg = cmsMem_strdup(objValue->dlFaultMsg);
   }

   acsState.dlFaultStatus = objValue->dlFaultStatus;
   acsState.startDLTime   = objValue->startDLTime;
   acsState.endDLTime     = objValue->endDLTime;

   acsState.retryCount    = objValue->retryCount;
   cmsLog_notice("retryCount=%d", acsState.retryCount);

   if (objValue->contactedState != eACSUpload
    && objValue->contactedState != eACSDownloadReboot)
   {
      informState            = objValue->contactedState;
   }

   cmsLog_notice("informState=%d", informState);

   /* retrieve inform states and inform event list from scratch pad.
    * TR69c may exit due to time out before an inform message could be sent out.
    * The number of events that can be saved in scratch pad is greater than
    * the runtime value.  Verify that number saved in scratch pad does not
    * overrun the runtime value (although it should not be possible to
    * write that many into the scratch pad in the first place.)
    */
   if (objValue->informEvCount > MAXINFORMEVENTS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"%d events stored in flash, can only restore %d, truncating!",
                   objValue->informEvCount, MAXINFORMEVENTS);
      informEvList.informEvCnt = MAXINFORMEVENTS;
   }
   else
   {
      cmsLog_notice("restoring %d events from DB", objValue->informEvCount);
      informEvList.informEvCnt = objValue->informEvCount;
   }

   for (i = 0; i < informEvList.informEvCnt; i++)
   {
      if (objValue->informEvList[i] == INFORM_EVENT_BOOT ||
        objValue->informEvList[i] == INFORM_EVENT_REBOOT_METHOD)
      {
         informEvList.informEvList[i] = objValue->informEvList[i];
         informEvCnt++;
         cmsLog_notice("%d: restoring (code=%d) %s", i,
                       informEvList.informEvList[i],
                       getInformEvtString(informEvList.informEvList[i]));
      }
   }

   informEvList.informEvCnt = informEvCnt;

   acsState.cwmpVersion = objValue->cwmpVersion;

   /* the default namespace ID is cwmpv1.2.  If it was cwmp1.0 communicating with ACS, change it to cwmp1.0 */
   if ((acsState.cwmpVersion != CWMP_VERSION_1_0) && (acsState.cwmpVersion != CWMP_VERSION_1_2))
   {
      acsState.cwmpVersion = CWMP_VERSION_1_2;
   }
   changeNameSpaceCwmpVersionURL(acsState.cwmpVersion);

   CMSMEM_FREE_BUF_AND_NULL_PTR(objValue);

   /* reset GWStateData in db after data retrieved */
   cmsPhl_setGWStateData(NULL);

   return;
}

void wrapperReboot(eInformState rebootContactValue)
{
   addInformEventToList(INFORM_EVENT_REBOOT_METHOD);

   setInformState(rebootContactValue);    /* reset on BcmCfm_download error */
   saveTR69StatusItems();

   cmsLog_notice("CPE is REBOOTING with rebootContactValue =%d", rebootContactValue);

   tr69SaveTransferList();
   wrapperReset(FALSE);
}

/*
* Call library factory reset 
*/
UBOOL8 wrapperFactoryReset(void)
{
   //CmsRet ret = CMSRET_SUCCESS;

   cmsLog_notice("wrapperFactoryReset)");

   {
      saveTR69StatusItems();
      
      tr69SaveTransferList();

      wrapperReset(TRUE);
      return TRUE;
   }
}


/** Request modem reset.
 *  This was called sysMipsSoftReset in the old cfm code.
 *  In CMS, all management apps send a reboot message to smd and smd will
 *  do a proper shutdown of the system.
 */
void wrapperReset(UBOOL8 bReset)
{
   
   cmsLog_notice("Sending msg to smd requesting reboot/reset bReset=%d",bReset);

   cmsUtil_sendRequestRebootMsg(msgHandle,bReset);

   return;         
}


/** Ask MDM to flush config to flash.
*/
void wrapperSaveConfigurations(void)
{
   //CmsRet ret;

   /*
    * This function really should be called inside the lock that
    * was acquired by runRPC.  But I don't know the workings of the
    * tr69c state machine (yet) to make that happen.  For now, acquire
    * the lock again and write the config out.  The downside of this
    * approach is that some other management app may have done another
    * write after our write (highly unlikely).
    */
#if 0
   if ((ret = cmsLck_acquireLockWithTimeout(TR69C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not acquire lock (ret=%d), abort config save", ret);
      return;
   }


   if ((ret = cmsMgm_saveConfigToFlash()) != CMSRET_SUCCESS)
   {
      cmsLog_error("saveConfigToFlash failed, ret=%d", ret);
   }
   else
   {
      cmsLog_debug("config saved to flash");
   }

   cmsLck_releaseLock();
#endif
   return;
}

/* downloadComplete()
*  Called when image has been downloaded. If successful the *buf will point to the
*  image buffer. If *buf is NULL the download failed.
*/
CmsRet downloadComplete(DownloadReq *r, char *buf)
{
//   CmsImageFormat format;
   CmsRet ret=CMSRET_SUCCESS;
#if 0
   if ( buf ) {
      format = cmsImg_validateImage(buf, r->fileSize, msgHandle);
      if (format == CMS_IMAGE_FORMAT_INVALID)
      {
         cmsLog_error("downloadComplete -- invalid image format");
         downloadStop("Invalid image format", 9010);
         ret = CMSRET_INVALID_IMAGE;
      }
      else if (format == CMS_IMAGE_FORMAT_XML_CFG && r->efileType != eVendorConfig)
      {
         cmsLog_error("parse says XML CFG, but stated type is %d", r->efileType);
         downloadStop("Invalid image format", 9010);
         ret = CMSRET_INVALID_IMAGE;
      }
      else
      {
         downloadStop("Download successful", 0);
         tr69SaveTransferList();

         if (r->efileType == eVendorConfig)
         {
            tr69SaveConfigFileInfo(r);
         }

         cmsLog_notice("downloadComplete -- save flash image");
         ret = cmsImg_writeValidatedImage(buf, r->fileSize, format, msgHandle);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("write of validated image failed, ret=%d", ret);
         }
         else
         {
            /*
             * In the modem, cmsImg_writeValidatedImage() will trigger a 
             * reboot.  On the desktop, we will still be here.
             */
            cmsLog_debug("image written");
         }
      }
   }
   else
   {
      cmsLog_debug("no buf to check or flash");
      ret = CMSRET_INTERNAL_ERROR;
   }
#endif
   return ret;
}


/* Returns state of WAN interface to be used by tr69 client */
eWanState getWanState(void)
{
   /* add BCM shared library call here to determine status of outbound I/F*/
   /* mwang: even in the released 3.0.8 code, this function just returns
    * eWAN_ACTIVE. So in most places in the code, we will continue to use this
    * until we understand the implication of fixing it or removing it.
    * In the mean time, there is also a getRealWanState function below. */
   return eWAN_ACTIVE;
}

eWanState getRealWanState(const char *ifName)
{
   CmsMsgHeader *msg;
   char *data;
   void *msgBuf;
   UINT32 msgDataLen = 0;
   eWanState realWanState;
   CmsRet ret;


   if (ifName != NULL)
   {
      msgDataLen = strlen(ifName) + 1;
   }

   msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
   if (NULL == msgBuf)
   {
      cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"Failed to allocate %d bytes for GET_WAN_CONN_STATUS msg, "
                   "just return eWAN_INACTIVE",
                   sizeof(CmsMsgHeader) + msgDataLen);
      realWanState = eWAN_INACTIVE;
      return realWanState;
   }

   msg = (CmsMsgHeader *)msgBuf;

   msg->type = CMS_MSG_GET_WAN_CONN_STATUS;
   msg->src = EID_TR69C;
   msg->dst = EID_SSK;
   msg->flags_request = 1;

   if (ifName != NULL)
   {
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      strcpy(data, ifName);
   }

   /*
    * ssk will reply with TRUE or FALSE for wan connection up.
    * Need to check for that instead of the usual CMSRET enum.
    */
   ret = cmsMsg_sendAndGetReply(msgHandle, msg);
   if (ret == TRUE)
   {
      cmsLog_debug("ssk says boundIfName=%s is up", acsState.boundIfName);
      realWanState = eWAN_ACTIVE;
   }
   else
   {
      cmsLog_debug("ssk says boundIfName=%s is not up", acsState.boundIfName);
      realWanState = eWAN_INACTIVE;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   return realWanState;
}

UBOOL8 matchAddrOnLanSide(const char *urlAddrString)
{
   UBOOL8 match=FALSE;
   char *ipAddrStr=NULL, *ipMaskStr=NULL;
   CmsRet ret;

   ret = getLanIPAddressInfo(&ipAddrStr,&ipMaskStr);
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("one or more LAN_IP_CFG parameters are null");
   }
   else
   {
      struct in_addr urlAddr, ipAddr, mask;

      inet_aton(urlAddrString, &urlAddr);
      inet_aton(ipAddrStr, &ipAddr);
      inet_aton(ipMaskStr, &mask);

      if ((urlAddr.s_addr & mask.s_addr) == (ipAddr.s_addr & mask.s_addr))
      {
         cmsLog_debug("urlAddr %s is on LAN side", urlAddrString);
         match = TRUE;
      }
      else
      {
         cmsLog_debug("urlAddr %s is not on LAN side (%s/%s)",
                      urlAddrString,
                      ipAddrStr,
                      ipMaskStr);
      }
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipAddrStr);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipMaskStr);
   }
   return match;
}




/*************** BEGIN FUNCTIONS MUST ALREADY HAVE MDM LOCK ********************/

void updateCredentialsInfo(void)
{
   CmsRet ret = CMSRET_SUCCESS;
   Dev2ManagementServerObject *acsCfg = NULL;

   ret = cmsPhl_getAcsCfg(&acsCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"cmsPhl_getAcsCfg error: %d", ret);
   }
   else
   {
      if (cmsUtl_strcmp(acsState.acsUser, acsCfg->username) != 0)
      {
          CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.acsUser);
          acsState.acsUser = cmsMem_strdup(acsCfg->username);
      }

      if (cmsUtl_strcmp(acsState.acsPwd, acsCfg->password) != 0)
      {
          CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.acsPwd);
          acsState.acsPwd = cmsMem_strdup(acsCfg->password);
      }

      cmsPhl_free((void*)&acsCfg, 1);
   }

}
#if 0
CmsRet setMSrvrURL(const char *value)
{
   PhlSetParamValue_t   paramValue;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER, url=%s", value);

   memset(&paramValue, 0, sizeof(paramValue));

   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      paramValue.pathDesc.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      paramValue.pathDesc.oid = MDMOID_MANAGEMENT_SERVER;
   }
   sprintf(paramValue.pathDesc.paramName, "%s", URL);
   paramValue.pValue = (char *)value;
   paramValue.status = CMSRET_SUCCESS;
      
   ret = cmsPhl_setParameterValues(&paramValue, 1);
   if (ret != CMSRET_SUCCESS || paramValue.status != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_setParamValues error: %d, %d", ret, paramValue.status);
   }
   else
   {
      if (acsState.acsURL == NULL || strcmp(acsState.acsURL, value))
      {
         /* This is a new ACS URL, mark this one as "never contacted". */
         setInformState(eACSNeverContacted);
         cmsMem_free(acsState.acsURL);
         acsState.acsURL = cmsMem_strdup(value);
      }
   }
   return ret;

}  /* end of setMSrvrURL() */

CmsRet getMSrvrURL(char **value)
{
   MdmPathDescriptor    path;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER");
   memset(&path, 0, sizeof(path));

   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      path.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      path.oid = MDMOID_MANAGEMENT_SERVER;
   }
   sprintf(path.paramName, "%s", URL);
   ret = cmsPhl_getParamValue(&path, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      if (cmsPhl_isParamValueChanged(&path) == TRUE)
      {
         /* This is a new ACS URL, mark this one as "never contacted". */
         setInformState(eACSNeverContacted);
      }
      cmsMem_free(acsState.acsURL);
      acsState.acsURL = cmsMem_strdup(pParamValue->pValue);
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }
   *value = acsState.acsURL?cmsMem_strdup(acsState.acsURL):cmsMem_strdup("");
   cmsLog_debug("acsURL=%s", *value);
   return ret;

}  /* End of getMSrvrURL() */


CmsRet getMSrvrConnReqURL(char **value)
{
   MdmPathDescriptor    path;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER");
   memset(&path, 0, sizeof(path));
   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      path.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      path.oid = MDMOID_MANAGEMENT_SERVER;
   }
   sprintf(path.paramName, "%s", ConnectionRequestURL);
   ret = cmsPhl_getParamValue(&path, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      cmsMem_free(acsState.connReqURL);
      acsState.connReqURL = cmsMem_strdup(pParamValue->pValue);
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }
   *value = acsState.connReqURL?cmsMem_strdup(acsState.connReqURL):cmsMem_strdup("");
   cmsLog_debug("connReqURL=%s", *value);

   return ret;
}  /* End of getMSrvrURL() */


CmsRet getMSrvrUsername(char **value)
{
   MdmPathDescriptor    path;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER");
   memset(&path, 0, sizeof(path));
   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      path.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      path.oid = MDMOID_MANAGEMENT_SERVER;
   }
   sprintf(path.paramName, "%s", Username);
   ret = cmsPhl_getParamValue(&path, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      cmsMem_free(acsState.acsUser);
      acsState.acsUser = cmsMem_strdup(pParamValue->pValue);
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }
   *value = acsState.acsUser?cmsMem_strdup(acsState.acsUser):cmsMem_strdup("");
   return ret;

}  /* End of getMSrvrUsername() */

CmsRet getMSrvrPassword(char **value)
{
   MdmPathDescriptor    path;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER");
   memset(&path, 0, sizeof(path));
   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      path.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      path.oid = MDMOID_MANAGEMENT_SERVER;
   }
   sprintf(path.paramName, "%s", Password);
   ret = cmsPhl_getParamValue(&path, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      cmsMem_free(acsState.acsPwd);
      acsState.acsPwd = cmsMem_strdup(pParamValue->pValue);
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }
   *value = acsState.acsPwd?cmsMem_strdup(acsState.acsPwd):cmsMem_strdup("");
   return ret;

}  /* End of getMSrvrPassword() */

CmsRet setMSrvrInformEnable(UBOOL8 enable)
{
   char                 valueBuf[BUFLEN_32];
   PhlSetParamValue_t   paramValue;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER, enable=%d", enable);

   memset(&paramValue, 0, sizeof(paramValue));
   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      paramValue.pathDesc.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      paramValue.pathDesc.oid = MDMOID_MANAGEMENT_SERVER;
   }
   sprintf(paramValue.pathDesc.paramName, "%s", PeriodicInformEnable);
   snprintf(valueBuf, sizeof(valueBuf), "%d", enable);
   paramValue.pValue = (char *) valueBuf;
   paramValue.status = CMSRET_SUCCESS;
      
   ret = cmsPhl_setParameterValues(&paramValue, 1);
   if (ret != CMSRET_SUCCESS || paramValue.status != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_setParamValue error: %d, %d",
                   ret, paramValue.status);
   }
   else
   {
      acsState.informEnable = enable;
   }
   return ret;

}  /* End of setMSrvrInformEnable() */

CmsRet getMSrvrInformEnable(char **value)
{
   MdmPathDescriptor    path;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER");
   memset(&path, 0, sizeof(path));
   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      path.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      path.oid = MDMOID_MANAGEMENT_SERVER;
   }
   sprintf(path.paramName, "%s", PeriodicInformEnable);
   ret = cmsPhl_getParamValue(&path, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      if (pParamValue->pValue[0] != 0)
      {
         acsState.informEnable = (!strcmp(pParamValue->pValue, "0"))? 0 : 1;
      }
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }
   *value = cmsMem_strdup(acsState.informEnable? "1": "0");
   return ret;

}  /* End of getMSrvrInformEnable() */

CmsRet setMSrvrInformInterval(UINT32 interval)
{
   char                 valueBuf[BUFLEN_32];
   PhlSetParamValue_t   paramValue;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER, interval=%u", interval);

   memset(&paramValue, 0, sizeof(paramValue));

   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      paramValue.pathDesc.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      paramValue.pathDesc.oid = MDMOID_MANAGEMENT_SERVER;
   }

   sprintf(paramValue.pathDesc.paramName, "%s", PeriodicInformInterval);
   snprintf(valueBuf, sizeof(valueBuf), "%u", interval);
   paramValue.pValue = (char *) valueBuf;
   paramValue.status = CMSRET_SUCCESS;
      
   ret = cmsPhl_setParameterValues(&paramValue, 1);
   if (ret != CMSRET_SUCCESS || paramValue.status != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_setParamValue error: %d, %d",
                   ret, paramValue.status);
   }
   else
   {
      acsState.informInterval = interval;
   }
   return ret;

}  /* End of setMSrvrInformInterval() */

CmsRet getMSrvrInformInterval(char **value)
{   
   char                 buf[10];
   MdmPathDescriptor    path;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER");
   memset(&path, 0, sizeof(path));
   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      path.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      path.oid = MDMOID_MANAGEMENT_SERVER;
   }
   sprintf(path.paramName, "%s", PeriodicInformInterval);
   ret = cmsPhl_getParamValue(&path, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      if (pParamValue->pValue[0] != 0)
      {
         acsState.informInterval = atoi(pParamValue->pValue);
      }
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }
   snprintf(buf, sizeof(buf), "%d", (int)acsState.informInterval);
   *value = cmsMem_strdup(buf);
   return ret;

}  /* End of getMSrvrInformInterval() */


CmsRet setMSrvrBoundIfName(const char *boundIfName)
{
   PhlSetParamValue_t   paramValue;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER, boundIfName=%s", boundIfName);

   memset(&paramValue, 0, sizeof(paramValue));

   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      paramValue.pathDesc.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      paramValue.pathDesc.oid = MDMOID_MANAGEMENT_SERVER;
   }

   sprintf(paramValue.pathDesc.paramName, "X_BROADCOM_COM_BoundIfName");
   paramValue.pValue = (char *) boundIfName;
   paramValue.status = CMSRET_SUCCESS;
      
   ret = cmsPhl_setParameterValues(&paramValue, 1);
   if (ret != CMSRET_SUCCESS || paramValue.status != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_setParamValue error: %d, %d",
                   ret, paramValue.status);
   }
   else
   {
      CMSMEM_REPLACE_STRING(acsState.boundIfName, boundIfName);
   }
   return ret;

}  /* End of setMSrvrInformInterval() */


CmsRet getConnectionUsername(char **value)
{
   MdmPathDescriptor    path;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER");
   memset(&path, 0, sizeof(path));
   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      path.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      path.oid = MDMOID_MANAGEMENT_SERVER;
   }
   sprintf(path.paramName, "%s", ConnectionRequestUsername);
   ret = cmsPhl_getParamValue(&path, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      cmsMem_free(acsState.connReqUser);
      acsState.connReqUser = cmsMem_strdup(pParamValue->pValue);
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }
   *value = acsState.connReqUser?cmsMem_strdup(acsState.connReqUser):cmsMem_strdup("");
   return ret;

}  /* End of getConnectionUsername() */

CmsRet getConnectionPassword(char **value)
{
   MdmPathDescriptor    path;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet               ret;

   cmsLog_debug("=====>ENTER");
   memset(&path, 0, sizeof(path));
   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      path.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   }
   else
   {
      path.oid = MDMOID_MANAGEMENT_SERVER;
   }
   sprintf(path.paramName, "%s", ConnectionRequestPassword);
   ret = cmsPhl_getParamValue(&path, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      cmsMem_free(acsState.connReqPwd);
      acsState.connReqPwd = cmsMem_strdup(pParamValue->pValue);
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }
   *value = acsState.connReqPwd?cmsMem_strdup(acsState.connReqPwd):cmsMem_strdup("");
   return ret;

}  /* End of getConnectionPassword() */
#endif

/*qingpu : save URL to LastConnectedURL if necessary---move from rpcState.c*/
void saveLastConnectedURL_igd(void)
{
#if 0
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   ManagementServerObject *acsCfg = NULL;
   UBOOL8 lastconnectedURLchanged = FALSE;
   CmsRet ret;

   /*
    * Get managment server object.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of MANAGEMENT_SERVER failed, ret=%d", ret);
      cmsLck_releaseLock();
      return;
   }

   if(acsCfg->URL != NULL)
   {
      if (acsCfg->lastConnectedURL == NULL){
         acsCfg->lastConnectedURL = cmsMem_strdup(acsCfg->URL);
         lastconnectedURLchanged = TRUE;
      }
      else if(strcmp(acsCfg->lastConnectedURL, acsCfg->URL) != 0)
      {
         cmsMem_free(acsCfg->lastConnectedURL); 
         acsCfg->lastConnectedURL = cmsMem_strdup(acsCfg->URL);
         lastconnectedURLchanged = TRUE;
      }

      cmsLog_debug("saving URL to lastConnectedURL. lastConnectedURL=%s, acsCfg->URL=%s", 
	  	acsCfg->lastConnectedURL, acsCfg->URL);

      if(lastconnectedURLchanged)
      {
         ret=cmsObj_set(acsCfg, &iidStack);
         if(ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set MDMOID_MANAGEMENT_SERVER failed ret=%d", ret);
         }
         else
            setsaveConfigFlag(TRUE);
      }
   }

   cmsObj_free((void **) &acsCfg);
#endif
}


/*************** END FUNCTIONS MUST ALREADY HAVE MDM LOCK ********************/
#if 0
void updateConnectionRequestInfo(void)
{
   char   *value=NULL;
   CmsRet ret;

   cmsLog_debug("=====>ENTER");

   if ((ret = cmsLck_acquireLockWithTimeout(TR69C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      /* yikes, what to do now?  Some other program is definately mis-behaving.
       * I can't read MDM because I don't have the lock, so just leave those
       * variables unitialized.
       */
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   getMSrvrConnReqURL(&value);
   CMSMEM_FREE_BUF_AND_NULL_PTR(value);
   
   getConnectionUsername(&value);
   CMSMEM_FREE_BUF_AND_NULL_PTR(value);
   
   getConnectionPassword(&value);
   CMSMEM_FREE_BUF_AND_NULL_PTR(value);

   cmsLck_releaseLock();
}

void updateConnectionAcsInfo(void)
{
   char   *value=NULL;
   CmsRet ret;

   cmsLog_debug("=====>ENTER");

   if ((ret = cmsLck_acquireLockWithTimeout(TR69C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      /* yikes, what to do now?  Some other program is definately mis-behaving.
       * I can't read MDM because I don't have the lock, so just leave those
       * variables unitialized.
       */
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   /* get items that may have been change by WEB-UI */
   getMSrvrInformEnable(&value);
   CMSMEM_FREE_BUF_AND_NULL_PTR(value);  /* need to free value since it's allocated memory by strdup */
   
   getMSrvrInformInterval(&value);
   CMSMEM_FREE_BUF_AND_NULL_PTR(value);

   getMSrvrURL(&value);
   CMSMEM_FREE_BUF_AND_NULL_PTR(value);

   getMSrvrUsername(&value);
   CMSMEM_FREE_BUF_AND_NULL_PTR(value);
   
   getMSrvrPassword(&value);
   CMSMEM_FREE_BUF_AND_NULL_PTR(value);

   cmsLck_releaseLock();
}

void updateConnectionInfo(void)
{
   updateConnectionAcsInfo();
   updateConnectionRequestInfo();
}  /* End of updateConnectionInfo() */
#endif
#if 0
UINT32 getMdmParamValueChanges(void)
{
   SINT32 count;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(TR69C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      /* yikes, what to do now?  Some other program is definately mis-behaving.
       * I can't read MDM because I don't have the lock, so just pretend no
       * notifications are pending.
       */
      cmsLog_error("could not get lock, ret=%d", ret);
      count = 0;
   }
   else
   {
      count = cmsPhl_getNumberOfParamValueChanges();
      cmsLck_releaseLock();
   }

   return count;
}
#endif
CmsRet tr69SaveTransferList(void)
{
   UINT16 saveSz = 0, saveEntryCount = 0, i = 0, j = 0;
   DownloadReqInfo *pSaveList, *pSaved;
   DownloadReq *q;
   CmsRet ret = CMSRET_SUCCESS;

   for (i = 0; i < TRANSFER_QUEUE_SIZE; i++)
   {
      q = &transferList.queue[i].request;
      if (q->state == eTransferNotYetStarted)
      {
         saveEntryCount++;
      }
   } /* for */ 

   cmsLog_debug("saveEntryCount=%d", saveEntryCount);
   if (saveEntryCount > 0)
   {
      saveSz = sizeof(DownloadReqInfo) * saveEntryCount;

      pSaveList = (DownloadReqInfo *)cmsMem_alloc(saveSz, ALLOC_ZEROIZE);
      if (pSaveList == NULL)
      {
         return CMSRET_RESOURCE_EXCEEDED;
      }

      for (i = 0, j = 0; i < TRANSFER_QUEUE_SIZE && j < saveEntryCount; i++)
      {
         q = &transferList.queue[i].request;
         pSaved = &pSaveList[j];
         if (q->state == eTransferNotYetStarted) 
         {
            pSaved->efileType = q->efileType;
            if (q->commandKey)
               strcpy(pSaved->commandKey,(q->commandKey));
            if (q->url)
               strcpy(pSaved->url,(q->url));
            if (q->user)
               strcpy(pSaved->user,q->user);
            if (q->pwd)
               strcpy(pSaved->pwd,q->pwd);
            pSaved->fileSize=q->fileSize;
            if (q->fileName)
               strcpy(pSaved->fileName,q->fileName);
            pSaved->delaySec = q->delaySec;
            pSaved->state = q->state;
            pSaved->rpcMethod = transferList.queue[i].rpcMethod;

            cmsLog_debug("tr69SaveTransferList(j %d): saving efileType %d,commandKey %s, url %s, user %s, pwd %s, filesize %d, filename %s, delaySec %d, state %d, rpcMethod %d\n",
                      j,pSaved->efileType,pSaved->commandKey,pSaved->url,pSaved->user,
                      pSaved->pwd,pSaved->fileSize,pSaved->fileName,pSaved->delaySec,pSaved->state,pSaved->rpcMethod);
            j++;
         } /* notyetStarted */        
      } /* for */
      if ((ret = cmsPhl_setTransferList(pSaveList, saveEntryCount)) != CMSRET_SUCCESS)
      {
         cmsLog_error(TR069_ALARM_PROCESS_ERROR,"Unable to save transferList in scratch PAD");
      }
      CMSMEM_FREE_BUF_AND_NULL_PTR(pSaveList);
   } /* saveEntryCount */

   return ret;
}

CmsRet tr69RetrieveTransferListFromStore(DownloadReqInfo *list, UINT16 *size)
{
   DownloadReqInfo *buf = NULL;

   CmsRet ret=CMSRET_INVALID_ARGUMENTS;
   UINT16 count = 0;

   ret = cmsPhl_getTransferList((void**)&buf, &count);
   if (count == 0)
   {
      /* could not find tr69c_transfer in scratch pad or other error */
      cmsLog_debug("could not find tr69c_transfer in scratch pad");
      *size = 0;
   }
   else
   {
      cmsLog_debug("read %d bytes from scratch pad for tr69c_transfer", count);
      *size = (UINT16) count;
      memcpy((void*)list, buf, (*size)*sizeof(DownloadReqInfo));
      ret = CMSRET_SUCCESS;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(buf);

   return ret;
}

CmsRet tr69SaveConfigFileInfo(DownloadReq *r)
{
//   UINT16 saveSz = 0;
//   DownloadVendorConfigInfo vendorConfig;
   CmsRet ret = CMSRET_SUCCESS;
//   char *namePtr;
#if 0
   saveSz = sizeof(DownloadVendorConfigInfo);
   memset(&vendorConfig,0,saveSz);

   cmsTms_getXSIDateTime(0,vendorConfig.date,sizeof(vendorConfig.date));
   if ((r->fileName) && (r->fileName[0] != '\0'))
   {
      strncpy(vendorConfig.name,r->fileName,BUFLEN_64);
   }
   else
   {
      /* retrieve file name from URL such as: http://220.128.128.236/ACS/Save/pd128_fw/dev4.0sbcConf.xml */
      namePtr = strrchr(r->url,'/');
      strncpy(vendorConfig.name,(namePtr+1),BUFLEN_64);
   }
   if ((ret = cmsPsp_set(VENDOR_CFG_INFO_TOKEN, &vendorConfig, saveSz)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Unable to save vendorConfig info in scratch PAD");
   }
#endif
   return ret;
} /* tr69SaveConfigFileInfo */

void setVendorConfigObj(DownloadVendorConfigInfo *vendorConfig)
{
#if 0
   vendorConfigUpdateMsgBody *pData;
   int msgDataLen = sizeof(vendorConfigUpdateMsgBody);
   CmsMsgHeader *msg=NULL;
   char *msgBuf;

   /* send a message to SSK and have it edit the vendorConfigFile
    * table.   This is a dynamic instance.  TR69 and other applications
    * are not allowed to update the object. 
    */
   msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
   if (NULL == msgBuf)
   {
      cmsLog_error("Failed to allocate %d bytes for VENDOR_CONFIG_UPDATE msg",
                   sizeof(CmsMsgHeader) + msgDataLen);
      return;
   }

   msg = (CmsMsgHeader *)msgBuf;
   pData = (vendorConfigUpdateMsgBody *) (msg + 1);

   msg->type = CMS_MSG_VENDOR_CONFIG_UPDATE;
   msg->src = EID_TR69C;
   msg->dst = EID_SSK;
   msg->flags_request = 1;
   msg->dataLength = msgDataLen;
   strcpy(pData->date, vendorConfig->date);
   if (vendorConfig->name != NULL)
   {
      strcpy(pData->name, vendorConfig->name);
   }
   if (vendorConfig->version != NULL)
   {
      strcpy(pData->version, vendorConfig->version);
   }
   if (vendorConfig->version != NULL)
   {
      strcpy(pData->name, vendorConfig->name);
   }
   if (vendorConfig->description != NULL)
   {
      strcpy(pData->description, vendorConfig->description);
   }

   if ((cmsMsg_sendAndGetReply(msgHandle, msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not send CMS_MSG_VENDOR_CONFIG_UPDATE message to SSK");
   }

   cmsMem_free(msgBuf);
#endif
} /* setVendorConfigObj */

void retrieveClearTR69VendorConfigInfo(void)
{
#if 0
   DownloadVendorConfigInfo *pVendorConfig;
   SINT32 count;
   CmsRet ret;
   cmsLog_debug("entered");

   if ((pVendorConfig = (DownloadVendorConfigInfo*)
        cmsMem_alloc(sizeof(DownloadVendorConfigInfo), ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("malloc failed");
      return;
   }

   count = cmsPsp_get(VENDOR_CFG_INFO_TOKEN, pVendorConfig, sizeof(DownloadVendorConfigInfo));
   if (count == 0)
   {
      cmsLog_debug("No existing VENDOR_CFG_INFO_TOKEN info found in scratch pad.");
      cmsMem_free(pVendorConfig);
      return;
   }
   else if (count != sizeof(DownloadVendorConfigInfo))
   {
      cmsLog_error("error while reading vendor config data from scratch pad, count=%d", count);
      cmsMem_free(pVendorConfig);
      return;
   }
   cmsLog_debug("pVendorConfig->name %s, date %s",pVendorConfig->name,pVendorConfig->date);
   
   setVendorConfigObj(pVendorConfig);
   cmsMem_free(pVendorConfig);

   /* we only record the config file once */
   ret = cmsPsp_set(VENDOR_CFG_INFO_TOKEN,NULL,0);
   if (CMSRET_SUCCESS != ret)
   {
      cmsLog_error("cmsPsp_set of VENDOR_CFG_INFO failed, ret=%d", ret);
   }
#endif
} /* retrieveTR69VendorConfigInfo */

/* this is a helper function for binary only objects */
int get_max_image_buf_size()
{
   //return ((int)cmsImg_getImageFlashSize());
   return 0;
}


/* this routine just set some parameters to have
 * default active notification as defined in section 2.4
 * of TR98 specfication.
 */
void setDefaultActiveNotification(void)
{
#if 0
   PhlSetParamAttr_t *pSetParamAttrList=NULL;
   CmsRet ret;
   int numEntries = 1;   /* ConnectionRequestURL counted */


   if ((cmsLck_acquireLockWithTimeout(TR69C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock");
      return;
   }

   setDefaultActiveNotificationLocked(&pSetParamAttrList,&numEntries);

   if (pSetParamAttrList == NULL)
   {
      cmsLck_releaseLock();
      return;
   }
   ret = cmsPhl_setParameterAttributes(pSetParamAttrList,numEntries);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Cannot set parameters to default active notifications: ret %d\n",ret);  
   }
   cmsLck_releaseLock();

   setsaveConfigFlag(TRUE);
   saveConfigurations();
   /* free pSetParamAttrList buffer */
   CMSMEM_FREE_BUF_AND_NULL_PTR(pSetParamAttrList);
#endif
}

#ifdef DMP_DSLDIAGNOSTICS_1
/* Calls xdsl driver directly to get DSL loop diag's
 * results.  Results are stored in tr69c; so as soon as
 * this process exits, the results are all gone.
 * ACS is expected to initiate DSL loop diag and retrieve
 * the results shortly after that.
 * We cannot afford to store Loop Diagnostics results 
 * in MDM because they are too large.
 */
void getDslLoopDiagResultsAndLinkUp(void)
{
   if (pDslLoopDiagResults == NULL)
   {
      pDslLoopDiagResults = (DslLoopDiagData*)cmsMem_alloc(sizeof(DslLoopDiagData),ALLOC_ZEROIZE);

      if (pDslLoopDiagResults == NULL)
      {
         cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"unable to allocate memory to store DSL Loop results");
         return;
      }
      else
      {
         cmsLog_debug("allocated memory for loop result %d",sizeof(DslLoopDiagData));
      }
   }
   else
   {
      memset(pDslLoopDiagResults,0,sizeof(DslLoopDiagData));
   }
   rutWan_getAdslLoopDiagResultAndLinkUp(pDslLoopDiagResults);
}

CmsRet dslLoopDiag_getParamValue(char *paramName, char **paramValue)
{
   CmsRet ret = CMSRET_SUCCESS;
   char dataStr[32];
   UINT16 *ptr;
   cmsLog_debug("Enter: paramName %s",paramName);

   dataStr[0]='\0';
   if ((pDslLoopDiagResults == NULL) ||
       ((pDslLoopDiagResults != NULL)&& (pDslLoopDiagResults->testCompleted == FALSE)))
   {
      /* there is no change, don't do anything */
      return (ret);
   }
       
   if (strcmp(paramName,"LoopDiagnosticsState") == 0)
   {
      /* unchange, don't need to do anything. */
   }
   else if (strcmp(paramName,"ACTATPds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->ACTATPds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"ACTATPus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->ACTATPus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLINSCds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLINSCds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRMTds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->SNRMTds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRMTus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->SNRMTus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"QLNMTds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->QLNMTds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"QLNMTus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->QLNMTus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLOGMTds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLOGMTds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLOGMTus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLOGMTus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }

   else if (strcmp(paramName,"HLINGds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLINGds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLINGus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLINGus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLOGGds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLOGGds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"HLOGGus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->HLOGGus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"QLNGds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->QLNGds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"QLNGus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->QLNGus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRGds") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->SNRGds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRGus") == 0)
   {
      sprintf(dataStr,"%d",pDslLoopDiagResults->SNRGus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"LATNpbds") == 0)
   {
      /* LATNpbds and LATNds are the same */
      ptr = &pDslLoopDiagResults->LATNds[0];
      sprintf(dataStr,"%d,%d,%d,%d,%d",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"LATNpbus") == 0)
   {
      /* LATNpbus and LATNus are the same */
      ptr = &pDslLoopDiagResults->LATNus[0];
      sprintf(dataStr,"%d,%d,%d,%d,%d",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SATNds") == 0)
   {
      ptr = &pDslLoopDiagResults->SATNds[0];
      sprintf(dataStr,"%d,%d,%d,%d,%d",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }   
   else if (strcmp(paramName,"SATNus") == 0)
   {
      ptr = &pDslLoopDiagResults->SATNus[0];
      sprintf(dataStr,"%d,%d,%d,%d,%d",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRMpbds") == 0)
   {
      sprintf(dataStr,"%.5d",(int)pDslLoopDiagResults->SNRMpbds);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }
   else if (strcmp(paramName,"SNRMpbus") == 0)
   {
      sprintf(dataStr,"%.5d",(int)pDslLoopDiagResults->SNRMpbus);
      CMSMEM_REPLACE_STRING(*paramValue,dataStr);
   }

   else if (strcmp(paramName,"HLINpsds") == 0)
   {
      ret = (cmsAdsl_formatHLINString(paramValue, pDslLoopDiagResults->HLINpsds,MAX_PS_STRING));
   }
   else if (strcmp(paramName,"HLINpsus") == 0)
   {
      ret = (cmsAdsl_formatHLINString(paramValue, pDslLoopDiagResults->HLINpsus,MAX_PS_STRING));
   }
   else if (strcmp(paramName,"QLNpsds") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->QLNpsds,(char*)"QLNpsds",MAX_QLN_STRING));
   }
   else if (strcmp(paramName,"QLNpsus") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->QLNpsus,(char*)"QLNpsus",MAX_QLN_STRING));
   }
   else if (strcmp(paramName,"SNRpsds") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->SNRpsds,(char*)"SNRpsds",MAX_PS_STRING));
   }
   else if (strcmp(paramName,"SNRpsus") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->SNRpsus,(char*)"SNRpsus",MAX_PS_STRING));
   }
   else if (strcmp(paramName,"HLOGpsds") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->HLOGpsds,(char*)"HLOGpsds",MAX_LOGARITHMIC_STRING));
   }
   else if (strcmp(paramName,"HLOGpsus") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->HLOGpsus,(char*)"HLOGpsus",MAX_LOGARITHMIC_STRING));
   }
   else if (strcmp(paramName,"BITSpsds") == 0)
   {
      ret = (cmsAdsl_formatSubCarrierDataString(paramValue,(void*)pDslLoopDiagResults->BITSpsds,(char*)"BITSpsds",MAX_PS_STRING));
   }
   else if (strcmp(paramName,"GAINSpsds") == 0)
   {
      ret = (cmsAdsl_formatPertoneGroupQ4String(paramValue,(void**)pDslLoopDiagResults->GAINSpsds,MAX_PS_STRING));
   }

   cmsLog_debug("End: paramValue %s",paramValue);
   return ret;
}

void freeDslLoopDiagResults(void)
{
   if (pDslLoopDiagResults != NULL)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pDslLoopDiagResults);
   }
}

#endif /* DMP_DSLDIAGNOSTICS_1 */
