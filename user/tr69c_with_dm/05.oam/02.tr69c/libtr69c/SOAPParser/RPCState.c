/***********************************************************************
 *
 *  Copyright (c) 2005-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

/*----------------------------------------------------------------------*
 * File Name  : RPCState.c
 *
 * Description: RPC routines 
 * $Revision: 1.54 $
 * $Id: RPCState.c,v 1.54 2006/02/03 15:53:05 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <syslog.h>

#include "cms.h"
#include "cms_util.h"
#include "cms_mdm.h"
#include "cms_phl.h"
#include "cms_core.h"
#include "cms_qdm.h"
#include "cms_msg.h"
#include "cms_msg_modsw.h"
#include "cms_boardcmds.h"

#include "../inc/tr69cdefs.h" /* defines for ACS state */
#include "../inc/appdefs.h"
#include "../inc/utils.h"
#include "nanoxml.h"
#include "xmlParserSM.h"
#include "RPCState.h"
#include "xmlTables.h"
#include "../main/event.h"
#include "../main/informer.h"
#include "../main/httpDownload.h"
#include "../bcmLibIF/bcmWrapper.h"
#include "../bcmLibIF/bcmConfig.h"
#include "adslctlapi.h"
#include "osgid.h"
#include "RPCState_public.h"

//#define DEBUG

#define empty(x) ((x) == NULL || strcmp((x), "") == 0)
#define PNAME_LTH    257         /* parameter name length+1 */
#define MAX_PADDINGS 20

#ifdef OMIT_INDENT
#define xml_mIndent(A,B,C) ;
#else
#define xml_mIndent(A,B,C) XMLmIndent( A, B, (C * 2));
#endif

/** external functions **/

/** external data **/
extern ACSState acsState;
extern eSessionState sessionState;
extern void *msgHandle;
extern char *RootDevice;
extern void *tmrHandle;

/** public data **/
RPCAction   *rpcAction;
#ifdef SUPPORT_TR69C_VENDOR_RPC
/* list that keeps track of all vendor specific RPC that are resgistered*/
VendorSpecificRPCItem *vendorSpecificRPCList=NULL;
VendorRPCAction *pVendorRpcAction;
static void callDoRpcByName(char *rpcName);
#endif /* SUPPORT_TR69C_VENDOR_RPC */

int         transferCompletePending;
int         sendGETRPC;
unsigned int   SoapHeaderId = 0;

/** private data **/
static eInformState  rebootFlag;       /* a TRX_REBOOT has been returned from a setxxx */
static UBOOL8        saveConfigFlag=FALSE;   /* save config on disconnect */
static int           factoryResetFlag;
static UBOOL8        doSendDuStateComplete=FALSE;
static UBOOL8        doSendAutonDuStateComplete=FALSE;

static char **informDevIds;
static char *informDevIds_TR98[] = {
   "InternetGatewayDevice.DeviceInfo.Manufacturer",
   "InternetGatewayDevice.DeviceInfo.ManufacturerOUI",
   "InternetGatewayDevice.DeviceInfo.ProductClass",
   "InternetGatewayDevice.DeviceInfo.SerialNumber"
};
static char *informDevIds_TR181[] = 
{
   "Device.DeviceInfo.Manufacturer",
   "Device.DeviceInfo.ManufacturerOUI",
   "Device.DeviceInfo.ProductClass",
   "Device.DeviceInfo.SerialNumber"
};

static char **informParameters;
static char *informParameters_TR98[] = 
{
   "InternetGatewayDevice.DeviceSummary",
   "InternetGatewayDevice.DeviceInfo.SpecVersion",
   "InternetGatewayDevice.DeviceInfo.HardwareVersion",
   "InternetGatewayDevice.DeviceInfo.SoftwareVersion",
   "InternetGatewayDevice.DeviceInfo.ProvisioningCode",
   "InternetGatewayDevice.ManagementServer.ConnectionRequestURL",
   "InternetGatewayDevice.ManagementServer.ParameterKey",
   ""  /* this pathname can change, copied from acsState.connReqIfNameFullPath*/
};
static char *informParameters_TR181[] = {
   "Device.RootDataModelVersion",
   "Device.DeviceInfo.HardwareVersion",
   "Device.DeviceInfo.SoftwareVersion",
   "Device.DeviceInfo.ProvisioningCode",
   "Device.ManagementServer.ParameterKey",
   "Device.ManagementServer.ConnectionRequestURL",
   "Device.ManagementServer.AliasBasedAddressing"
//wwzh
#if 0
   "Device.Services.FAPService.1.FAPControl.UMTS.OpState",
   "Device.Services.FAPService.1.FAPControl.UMTS.RFTxStatus",
   "Device.Services.FAPService.1.FAPControl.LTE.OpState",
   "Device.Services.FAPService.1.FAPControl.LTE.RFTxStatus",
   "Device.Services.FAPService.1.FAPControl.CDMA2000.OneX.OpState",
   "Device.Services.FAPService.1.FAPControl.CDMA2000.OneX.RFTxStatus",
   "Device.Services.FAPService.1.FAPControl.CDMA2000.HRPD.OpState",
   "Device.Services.FAPService.1.FAPControl.CDMA2000.HRPD.RFTxStatus",
   "Device.FAP.GPS.ScanStatus",
   "Device.FAP.GPS.LastScanTime",
   "Device.FAP.GPS.ContinuousGPSStatus.CurrentFix",
   "Device.FAP.GPS.ContinuousGPSStatus.Latitude",
   "Device.FAP.GPS.ContinuousGPSStatus.Longitude"
#endif

};

TransferInfo transferList;
static void doGetQueuedTransfers(RPCAction *a);

/** public functions **/

/** private functions **/
static const char *getRPCMethodName(eRPCMethods);
static void freeParamItems(ParamItem *item);
static void freeAttributeItems(AttributeItem *item);
static void freeDownloadReq(DownloadReq *r);
static void mprintf(tProtoCtx *pc, int *len, const char *fmt, ...);
static void xml_mprintf(tProtoCtx *pc, int *len, char *s);
static void XMLmIndent(tProtoCtx *pc, int *len, int indent);
static void closeBodyEnvelope(tProtoCtx *pc, int *lth);
static void openBody(tProtoCtx *pc, int *lth);
static void openEnvWithHeader(char *idstr, tProtoCtx *pc, int *lth);
static const char *getFaultCode(int fault);
static int getParamCnt(ParamItem *pi);
static void writeSoapFault(RPCAction *a, int fault);
static void doGetRPCMethods(RPCAction *a);
static void writeGetAttribute(PhlGetParamAttr_t *pParamAttr, tProtoCtx *pc, int *bufsz);
static void writeGetPName(PhlGetParamInfo_t *pParamInfo, tProtoCtx *pc, int *bufsz);
static void doGetParameterNames(RPCAction *a);
static void doSetParameterAttributes(RPCAction *a);
static void doGetParameterAttributes(RPCAction *a);
static void doSetParameterValues(RPCAction *a);
static void writeGetPValue(PhlGetParamValue_t *pParamValue, tProtoCtx *pc, int *bufsz);
static void doGetParameterValues(RPCAction *a);
static void doAddObject(RPCAction *a);
static void doDeleteObject(RPCAction *a);
static void doRebootRPC(RPCAction *a);
static void doFactoryResetRPC(RPCAction *a);
static void doDownload(RPCAction *a);
static void goSendDuStateComplete(void *handle __attribute__((unused)));
static void freeChangeDuStateReq(ChangeDuStateOp *item);
static void doChangeDuState(RPCAction *a);
void handleOsgiAutonResponse(const CmsMsgHeader *msg);
void cleanupAcsDuReqList(void);
extern ChangeDuStateOpReqInfo *pAutonResult;


RPCAction* newRPCAction(void)
{
   return ((RPCAction *)cmsMem_alloc(sizeof(RPCAction), ALLOC_ZEROIZE));
}

#ifdef DEBUG
void dumpAcsState(void)
{
    fprintf(stderr, "ACS State dump\n");
    fprintf(stderr, "HoldRequests       %d\n", acsState.holdRequests);
    fprintf(stderr, "NoMoreRequest      %d\n", acsState.noMoreRequests);
    fprintf(stderr, "CommandKey(DL)     %s\n", acsState.downloadCommandKey);
    fprintf(stderr, "CommandKey(Reboot  %s\n", acsState.rebootCommandKey);
    fprintf(stderr, "MaxEnvelopes       %d\n", acsState.maxEnvelopes);
    fprintf(stderr, "RPC Methods supported by ACS:\n");
    fprintf(stderr, "   GetRpcMethods           %s\n", acsState.acsRpcMethods.rpcGetRPCMethods?
            "Yes": "No");
    fprintf(stderr, "   SetParameterValues      %s\n", acsState.acsRpcMethods.rpcSetParameterValues?
            "Yes": "No");
    fprintf(stderr, "   GetParameterValues      %s\n", acsState.acsRpcMethods.rpcGetParameterValues?
            "Yes": "No");
    fprintf(stderr, "   GetParameterNames       %s\n", acsState.acsRpcMethods.rpcGetParameterNames?
            "Yes": "No");
    fprintf(stderr, "   GetParameterAttributes  %s\n", acsState.acsRpcMethods.rpcGetParameterAttributes?
            "Yes": "No");
    fprintf(stderr, "   SetParameterAttributes  %s\n", acsState.acsRpcMethods.rpcSetParameterAttributes?
            "Yes": "No");
    fprintf(stderr, "   Reboot                  %s\n", acsState.acsRpcMethods.rpcReboot?
            "Yes": "No");
    fprintf(stderr, "   FactoryReset            %s\n", acsState.acsRpcMethods.rpcFactoryReset?
            "Yes": "No");
    fprintf(stderr, "   Download                %s\n", acsState.acsRpcMethods.rpcDownload?
            "Yes": "No");
    fprintf(stderr, "   ScheduleInform          %s\n", acsState.acsRpcMethods.rpcScheduleInform?
           "Yes": "No");
    fprintf(stderr, "   Upload                  %s\n", acsState.acsRpcMethods.rpcUpload?
            "Yes": "No");
    fprintf(stderr, "   GetQueuedTransfers      %s\n", acsState.acsRpcMethods.rpcGetQueuedTransfers?
           "Yes": "No");
    fprintf(stderr, "   ChangeDuState           %s\n", acsState.acsRpcMethods.rpcChangeDuState?
           "Yes": "No");
}

void dumpRpcAction(RPCAction *a)
{
    fprintf(stderr, "RPC description: RPC Method = %s ID=%s\n",
             getRPCMethodName(a->rpcMethod), a->ID);
}
#endif

/* rebootCompletion routine */
/* This is envoked following the ACS response to the rebootresponse msg */
UBOOL8 rebootCompletion(void)
{
   if (rebootFlag>=eACSDownloadReboot && rebootFlag<=eACSRPCReboot)
   {
      wrapperReboot(rebootFlag);
      return TRUE;
   }

   return FALSE;
}

/* factoryResetCompletion routine */
/* This is envoked following the ACS response to the FactoryResetResponse msg */
UBOOL8 factoryResetCompletion(void)
{
   if (factoryResetFlag)
   {
      factoryResetFlag = 0;
      //wrapperFactoryReset();
      if(wrapperFactoryReset())
	      return TRUE;
      else
	      return FALSE;
   }

   return FALSE;
}

/* save Configuration routine */
/* This is envoked following the ACS response to the a set/add/delete RPC msg */
void saveConfigurations(void)
{
   if (saveConfigFlag)
   {
      wrapperSaveConfigurations();
      saveConfigFlag = FALSE;
   }
}

/* Utility routines for data structures */
static const char *getRPCMethodName(eRPCMethods m)
{
   const char    *t;
   switch (m)
   {
      case rpcGetRPCMethods:
         t = "GetRPCMethods";
         break;
      case rpcScheduleInform:
         t="ScheduleInform";
         break;	  	
      case rpcSetParameterValues:
         t = "SetParameterValues";
         break;
      case rpcGetParameterValues:
         t = "GetParameterValues";
         break;
      case rpcGetParameterNames:
         t="GetParameterNames";
         break;
      case rpcGetParameterAttributes:
         t = "GetParameterAttributes";
         break;
      case rpcSetParameterAttributes:
         t = "SetParameterAttributes";
         break;
      case rpcAddObject:
         t = "AddObject";
         break;
      case rpcDeleteObject:
         t = "DeleteObject";
         break;
      case rpcReboot:
         t = "Reboot";
         break;
      case rpcDownload:
         t = "Download";
         break;
      case rpcUpload:
         t = "Upload";
         break;
      case rpcFactoryReset:
         t = "FactoryReset";
         break;
      case rpcInformResponse:
         t = "InformResponse";
         break;
      case rpcGetQueuedTransfers:
         t="GetQueuedTransfers";
         break;
      case rpcChangeDuState:
         t="ChangeDUState";
         break;
      default:
         t = "no RPC methods";
         break;
   }
   return t;
}

static void freeParamItems(ParamItem *item)
{
   ParamItem *next;
   while (item)
   {
      next = item->next;
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->pname);     /* free data */
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->ptype);
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->pvalue);
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->pOrigValue);
      CMSMEM_FREE_BUF_AND_NULL_PTR(item);           /* free ParamItem */
      item = next;
   }
}

static void freeAttributeItems(AttributeItem *item)
{
   AttributeItem *next;
   while (item)
   {
      next = item->next;
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->pname);
      CMSMEM_FREE_BUF_AND_NULL_PTR(item);
      item = next;
   }
}

static void freeDownloadReq(DownloadReq *r)
{
   CMSMEM_FREE_BUF_AND_NULL_PTR(r->url);
   CMSMEM_FREE_BUF_AND_NULL_PTR(r->user);
   CMSMEM_FREE_BUF_AND_NULL_PTR(r->pwd);
   CMSMEM_FREE_BUF_AND_NULL_PTR(r->fileName);
}

static void freeChangeDuStateReq(ChangeDuStateOp *item)
{
   if (item != NULL)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->url);
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->uuid);
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->user);
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->pwd);
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->exeEnvRef);
      CMSMEM_FREE_BUF_AND_NULL_PTR(item->version);
   }
}

/*
* item is undefined on return
*/
void freeRPCAction(RPCAction *item)
{
   CMSMEM_FREE_BUF_AND_NULL_PTR(item->ID);
   CMSMEM_FREE_BUF_AND_NULL_PTR(item->parameterKey);
   CMSMEM_FREE_BUF_AND_NULL_PTR(item->commandKey);
   CMSMEM_FREE_BUF_AND_NULL_PTR(item->informID);
#ifdef SUPPORT_TR69C_VENDOR_RPC
   CMSMEM_FREE_BUF_AND_NULL_PTR(item->vendorRpcName);
#endif /* SUPPORT_TR69C_VENDOR_RPC */
   switch (item->rpcMethod)
   {
      case rpcGetParameterNames:
         CMSMEM_FREE_BUF_AND_NULL_PTR(item->ud.paramNamesReq.parameterPath);
         break;
      case rpcSetParameterValues:
      case rpcGetParameterValues:
      case rpcScheduleInform:
         freeParamItems(item->ud.pItem);
         break;
      case rpcSetParameterAttributes:
      case rpcGetParameterAttributes:
         freeAttributeItems(item->ud.aItem);
         break;
      case rpcAddObject:
      case rpcDeleteObject:
         CMSMEM_FREE_BUF_AND_NULL_PTR(item->ud.addDelObjectReq.objectName);
         break;
      case rpcDownload:
         freeDownloadReq( &item->ud.downloadReq );
         break;
      case rpcUpload:
         freeDownloadReq( &item->ud.downloadReq );
         break;
      case rpcGetQueuedTransfers:
        /* do we need to free anything? */
        break;
      case rpcChangeDuState:
         freeChangeDuStateReq(item->ud.opItem);
         break;
      case rpcFault:
         CMSMEM_FREE_BUF_AND_NULL_PTR(item->ud.fault.code);
         CMSMEM_FREE_BUF_AND_NULL_PTR(item->ud.fault.string);
         CMSMEM_FREE_BUF_AND_NULL_PTR(item->ud.fault.detail.string);
         break;
      default:
         break;
   }
   CMSMEM_FREE_BUF_AND_NULL_PTR(item);
}  /* End of freeRPCAction() */

/*----------------------------------------------------------------------*
 * memory printf
 */
static void mprintf(tProtoCtx *pc, int *len, const char *fmt, ...)
{
   int      n;
   va_list  ap;
   char     buf[512];

   va_start(ap, fmt);
   n = vsnprintf(buf, 512, fmt, ap);
   va_end(ap);
   if (n < 0 || n >= 512)
   {
      /* out of memory */ 
      cmsLog_error(TR069_ALARM_SOCKET_ERROR,"xml: mprintf: out of memory");
      acsState.fault = CMSRET_RESOURCE_EXCEEDED;
   }
   else if (n > 0)
   {
      *len += n;
      if (pc != NULL)
      {
         if (pc->content != NULL)
         {
            snprintf(pc->content+pc->length,pc->maxlength-pc->length,buf);
            pc->length += n;
         }
         else
         {
            proto_SendRaw(pc, buf, n);
         }
      }
   }
}

/*----------------------------------------------------------------------*/
static void xml_mprintf(tProtoCtx *pc, int *len, char *s)
{
   if (s)
   {
      for (; *s; s++)
      {
         switch (*s)
         {
            case '&':
               mprintf(pc, len, "&amp;");
               break;
            case '<':
               mprintf(pc, len, "&lt;");
               break;
            case '>':
               mprintf(pc, len, "&gt;");
               break;
            case '"':
               mprintf(pc, len, "&quot;");
               break;
            case '\'':
               mprintf(pc, len, "&apos;");
               break;
            case 9:/* \t */
            case 10:/* \n */
            case 13:/* \r */
               mprintf(pc, len, "&#%d;", *s);
               break;
            default:
               if (isprint(*s))
               {
                  mprintf(pc, len, "%c", *s);
               }
               else
               {
                  mprintf(pc, len, " ");
               }
               break;
         }
      }
   }
}

/*----------------------------------------------------------------------*/
static void XMLmIndent(tProtoCtx *pc, int *len, int indent)
{
   int i;

   if (indent < 0)
   {
      return;
   }

   for (i = 0; i < indent; i++)
   {
      mprintf(pc, len, " ");
   }
}

static void closeBodyEnvelope(tProtoCtx *pc, int *lth)
{
   xml_mIndent(pc, lth, 2);
   mprintf(pc, lth, "</%sBody>\n", nsSOAP);
   mprintf(pc, lth, "</%sEnvelope>\n", nsSOAP);
}

/* Add <SOAP:Body>
*/
static void openBody(tProtoCtx *pc, int *lth)
{
   xml_mIndent(pc, lth, 2);
   mprintf(pc, lth, "<%sBody>\n", nsSOAP);
}

/*
* Add <SOAP:Envelope
 *      xmlns:....
 *      <..:Header>
 *      ....
 *      </..:Header>
 * to the buffer
 */
static void openEnvWithHeader(char *idstr, tProtoCtx *pc, int *lth)
{
   NameSpace   *ns;
   mprintf(pc, lth, "<%sEnvelope", nsSOAP);
   /* generate Namespace declarations */
   ns = nameSpaces;
   while (ns->sndPrefix)
   {
      char    pbuf[40];
      char    *e;
      mprintf(pc, lth, "\n");
      strncpy(pbuf, ns->sndPrefix, sizeof(pbuf));
      e=strchr(pbuf,':');
      if (e) *e='\0'; /* find : in prefix */
      xml_mIndent(pc, lth, 2);
      mprintf(pc, lth, "xmlns:%s=\"%s\"", pbuf, ns->nsURL);
      ++ns;
   }
   mprintf(pc, lth, ">\n");
   if (idstr)
   {
      xml_mIndent(pc, lth, 2);
      mprintf(pc, lth, "<%sHeader>\n", nsSOAP);
      xml_mIndent(pc, lth, 3);
      mprintf(pc, lth, "<%sID %smustUnderstand=\"1\">%s</%sID>\n",
              nsCWMP, nsSOAP,idstr , nsCWMP);
      xml_mIndent(pc, lth, 2);
      mprintf(pc, lth, "</%sHeader>\n", nsSOAP);
   }
   else
   {
      xml_mIndent(pc, lth, 2);
      mprintf(pc, lth, "<%sHeader>\n", nsSOAP);
      xml_mIndent(pc, lth, 3);
      mprintf(pc, lth, "<%sID %smustUnderstand=\"1\">%ld</%sID>\n",
              nsCWMP, nsSOAP,SoapHeaderId, nsCWMP);
      xml_mIndent(pc, lth, 2);
      mprintf(pc, lth, "</%sHeader>\n", nsSOAP);
      SoapHeaderId++;
   }
}

static const char *getFaultCode(int fault)
{
   const char *r;

   switch (fault)
   {
      case CMSRET_METHOD_NOT_SUPPORTED:
      case CMSRET_REQUEST_DENIED:
      case CMSRET_INTERNAL_ERROR:
      case CMSRET_RESOURCE_EXCEEDED:
      case CMSRET_NOTIFICATION_REQ_REJECTED:
      case CMSRET_DOWNLOAD_FAILURE:
      case CMSRET_UPLOAD_FAILURE:
      case CMSRET_FILE_TRANSFER_AUTH_FAILURE:
      case CMSRET_UNSUPPORTED_FILE_TRANSFER_PROTOCOL:
      case CMSRET_FILE_TRANSFER_UNABLE_JOIN_MULTICAST:
      case CMSRET_FILE_TRANSFER_UNABLE_CONTACT_FILE_SERVER:
      case CMSRET_FILE_TRANSFER_UNABLE_ACCESS_FILE:
      case CMSRET_FILE_TRANSFER_UNABLE_COMPLETE:
      case CMSRET_FILE_TRANSFER_FILE_CORRUPTED:
      case CMSRET_FILE_TRANSFER_FILE_AUTHENTICATION_ERROR:
      case CMSRET_INVALID_UUID_FORMAT:
      case CMSRET_UNKNOWN_EE:
      case CMSRET_EE_DISABLED:
      case CMSRET_DU_EE_MISMATCH:
      case CMSRET_DU_DUPLICATE:
      case CMSRET_SW_MODULE_SYSTEM_RESOURCE_EXCEEDED:
      case CMSRET_DU_UNKNOWN:
      case CMSRET_DU_STATE_INVALID:
      case CMSRET_DU_UPDATE_DOWNGRADE_NOT_ALLOWED:
      case CMSRET_DU_UPDATE_VERSION_NOT_SPECIFIED:
      case CMSRET_DU_UPDATE_VERSION_EXISTED:
         r = "Server";
         break;
      case CMSRET_INVALID_ARGUMENTS:
      case CMSRET_INVALID_PARAM_NAME:
      case CMSRET_INVALID_PARAM_TYPE:
      case CMSRET_INVALID_PARAM_VALUE:
      case CMSRET_SET_NON_WRITABLE_PARAM:
      case CMSRET_FILE_TRANSFER_FILE_TIMEOUT:
      case CMSRET_FILE_TRANSFER_FILE_CANCELLATION_NOT_ALLOW:
         r = "Client";
         break;
      default:
         r = "Vendor";
         break;
   }
   return r;
}


static int getParamCnt(ParamItem *pi)
{
   int   cnt = 0;

   while (pi != NULL)
   {
      cnt++;
      pi = pi->next;
   }

   return cnt;

}  /* End of getParamCnt() */

static void writeSoapFault(RPCAction *a, int fault)
{
   int         loop;
   tProtoCtx   *pc;
   int   bufsz = 0;

   cmsLog_debug("=====>ENTER");
   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sFault>\n", nsSOAP);
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<faultcode>%s</faultcode>\n", getFaultCode(fault));
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<faultstring>CWMP fault</faultstring>\n");
      xml_mIndent(pc, &bufsz, 5);
      mprintf(pc, &bufsz, "<detail>\n");
      xml_mIndent(pc, &bufsz, 6);
      mprintf(pc, &bufsz, "<%sFault>\n", nsCWMP);
      xml_mIndent(pc, &bufsz, 7);
      mprintf(pc, &bufsz, "<FaultCode>%d</FaultCode>\n", fault);
      xml_mIndent(pc, &bufsz, 7);
      mprintf(pc, &bufsz, "<FaultString>%s</FaultString>\n", cmsErr_getString(fault));
      if (a->rpcMethod == rpcSetParameterValues)
      {
         ParamItem   *pi = a->ud.pItem;
         /* walk thru parameters to generate errors */
         while (pi != NULL )
         {
            if (pi->fault)
            {
               xml_mIndent(pc, &bufsz, 7);
               mprintf(pc, &bufsz, "<SetParameterValuesFault>\n");
               xml_mIndent(pc, &bufsz, 8);
               mprintf(pc, &bufsz, "<ParameterName>%s</ParameterName>\n",
                           pi->pname);
               xml_mIndent(pc, &bufsz, 8);
               mprintf(pc, &bufsz, "<FaultCode>%d</FaultCode>\n", pi->fault);
               xml_mIndent(pc, &bufsz, 8);
               mprintf(pc, &bufsz, "<FaultString>%s</FaultString>\n",
                           cmsErr_getString(pi->fault));
               xml_mIndent(pc, &bufsz, 7);
               mprintf(pc, &bufsz, "</SetParameterValuesFault>\n");
            }
            pi = pi->next;
         }
      }
      xml_mIndent(pc, &bufsz, 6);
      mprintf(pc, &bufsz, "</%sFault>\n", nsCWMP);
      xml_mIndent(pc, &bufsz, 5);
      mprintf(pc, &bufsz, "</detail>\n");
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sFault>\n", nsSOAP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %ds", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %ds", time(NULL));
   
         cmsLog_log("RPC Fault sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

}  /* End of writeSoapFault() */

static void doScheduleInform(RPCAction *a)
{
   int interval = 0;
   unsigned long var = 0;
   ParamItem   *pi = a->ud.pItem;

   
   //printf("doScheduleInform name = %p;value = %s\n",a->ud.pItem->pname,a->ud.pItem->pvalue);
  
   acsState.fault = 0;  /* init to no fault */

   // if attribute name is empty then return fault message
   //invalid arguments
   if (pi == NULL) acsState.fault = CMSRET_REQUEST_DENIED;

   /* first set attributes */
   while (pi!=NULL && !acsState.fault )
   {
      const char  *pp = pi->pname;
      const char  *pv = pi->pvalue;

      if ((NULL == pp)||(NULL == pv))
      {
          acsState.fault = CMSRET_INVALID_ARGUMENTS;      
		  break;
      }

      if (0 == strcmp(pp,"DelaySeconds"))
      {
          interval = atoi(pv);
          if (interval > 0)
          {
              var = interval;
              setScheduleInform(var);              
          }
          else
          {
              acsState.fault = CMSRET_INVALID_ARGUMENTS;  
          }
      }
      pi = pi->next;
   }

   if (acsState.fault == 0) {

      int         loop;
      tProtoCtx   *pc;
      /* Loop through the code twice.  The first loop is to calculate
       * the size of the entire soap envelope so that the HTTP header
       * can be written to the TCP socket first.  The second loop is
       * to write the content of the envelope to the TCP socket as it
       * is generated.
       */
      int   bufsz = 0;
      for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
      {

         if (pc != NULL)
         {
            pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
            pc->length = 0;
            pc->maxlength = bufsz+1;
            cmsLog_debug("start to handle rpc body at %ds", time(NULL));
         }
         else
         {
            cmsLog_debug("start to handle rpc header at %ds", time(NULL));
         }
         
         bufsz = 0;

         /* build good response */
         openEnvWithHeader(a->ID, pc, &bufsz);
         openBody(pc, &bufsz);
         xml_mIndent(pc,&bufsz, 3);
         mprintf(pc,&bufsz,"<%sScheduleInformResponse/>\n", nsCWMP);
         closeBodyEnvelope(pc, &bufsz);

         if (loop == 0) {
            /* send the HTTP message header */
            sendToAcs(bufsz, NULL);
         }
         else
         {
            cmsLog_debug("start to send rpc body at %ds", time(NULL));
            proto_SendRaw(pc, pc->content, pc->length);
            cmsLog_debug("stop to send rpc body at %ds", time(NULL));

            cmsLog_log("RPC ScheduleInformResponse sent to ACS");
         }
      }  /* for (loop.... */

      if (pc && pc->content)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
         pc->length = 0;
         pc->maxlength = 0;
      }
   } else {
      writeSoapFault(a, acsState.fault);
      #ifdef DEBUG
      fprintf(stderr, "Fault in ScheduleInform %d\n", acsState.fault);
      #endif
   }
}  /* End of doScheduleInform() */

static void doGetRPCMethods(RPCAction *a)
{
   eRPCMethods m;
   int         loop;
   tProtoCtx   *pc;
   int   bufsz = 0;

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;

         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }

      bufsz = 0;

      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sGetRPCMethodsResponse>\n", nsCWMP);
      xml_mIndent(pc, &bufsz, 4);
      #ifdef SUPPRESS_SOAP_ARRAYTYPE
      mprintf(pc, &bufsz, "<MethodList>\n");
      #else
      mprintf(pc, &bufsz, "<MethodList %sarrayType=\"%sstring[%d]\">\n",
              nsSOAP_ENC, nsXSD, LAST_RPC_METHOD );
      #endif
   
      for (m = rpcGetRPCMethods; m <= LAST_RPC_METHOD; ++m)
      {
         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<string>%s</string>\n", getRPCMethodName(m));
      }
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "</MethodList>\n");
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sGetRPCMethodsResponse>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %ds", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %ds", time(NULL));

         cmsLog_log("RPC GetRPCMethodsResponse sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

}  /* End of doGetRPCMethods() */

static void writeGetAttribute(PhlGetParamAttr_t *pParamAttr, tProtoCtx *pc, int *bufsz)
{
   char           *pFullpath;
   unsigned int   accessList;

   /* convert MDM name path back to TR69 name string */
   cmsMdm_pathDescriptorToFullPath(&(pParamAttr->pathDesc), &pFullpath);

   /* now fill in ParameterAttributeStruct in response */
   xml_mIndent(pc, bufsz, 5);
   mprintf(pc, bufsz, "<ParameterAttributeStruct>\n");
   xml_mIndent(pc, bufsz, 6);
   mprintf(pc, bufsz, "<Name>%s</Name>\n", pFullpath);
   xml_mIndent(pc, bufsz, 6);
   mprintf(pc, bufsz, "<Notification>%d</Notification>\n",
            pParamAttr->attributes.notification);
   accessList = pParamAttr->attributes.accessBitMask;
   if (accessList == 0 || accessList == NDA_ACCESS_TR69C)
   {
      xml_mIndent(pc, bufsz, 6);
#ifdef SUPPRESS_SOAP_ARRAYTYPE
      mprintf(pc, bufsz, "<AccessList>\n");
#else
      mprintf(pc, bufsz, "<AccessList %sarrayType=\"%sstring[0]\">\n",
                 nsSOAP_ENC, nsXSD);
#endif
      xml_mIndent(pc, bufsz, 6);
      mprintf(pc, bufsz, "</AccessList>\n");
   }
   else
   {
      /*
       * If we get into this else block, then accessList definately
       * has a bit set for some entity that is not TR69C.  So just
       * report that as "Subscriber" has write access to the ACS.
       */
      xml_mIndent(pc, bufsz, 6);
#ifdef SUPPRESS_SOAP_ARRAYTYPE
      mprintf(pc, bufsz, "<AccessList>\n");
#else
      mprintf(pc, bufsz, "<AccessList %sarrayType=\"%sstring[1]\">\n",
              nsSOAP_ENC, nsXSD);
#endif
      xml_mIndent(pc, bufsz, 7);
      mprintf(pc, bufsz, "<string>Subscriber</string>\n");
      xml_mIndent(pc, bufsz, 6);
      mprintf(pc, bufsz, "</AccessList>\n");
   }
   xml_mIndent(pc, bufsz, 5);
   mprintf(pc, bufsz, "</ParameterAttributeStruct>\n");

   CMSMEM_FREE_BUF_AND_NULL_PTR(pFullpath);

}  /* End of writeGetAttribute() */

static void writeGetPName(PhlGetParamInfo_t *pParamInfo, tProtoCtx *pc, int *bufsz)
{
   char  *pFullpath;

   /* convert MDM name path back to TR69 name string */
   cmsMdm_pathDescriptorToFullPath(&(pParamInfo->pathDesc), &pFullpath);

   xml_mIndent(pc, bufsz, 5);
   mprintf(pc, bufsz, "<ParameterInfoStruct>\n");
   xml_mIndent(pc, bufsz, 6);
   mprintf(pc, bufsz, "<Name>%s</Name>\n", pFullpath);
   xml_mIndent(pc, bufsz, 6);
   mprintf(pc, bufsz, "<Writable>%s</Writable>\n", pParamInfo->writable?"1":"0");
   xml_mIndent(pc, bufsz, 5);
   mprintf(pc, bufsz, "</ParameterInfoStruct>\n");

   CMSMEM_FREE_BUF_AND_NULL_PTR(pFullpath);

}  /* End of writeGetPName() */

/* 
* GetParameterNames requests a single parameter path or single parameter path fragment 
*/
static void doGetParameterNames(RPCAction *a)
{
   int         loop;
   int         cnt           = 0;
   int         paramCnt      = 0;
   tProtoCtx   *pc           = NULL;
   UBOOL8      nextLevelOnly = a->ud.paramNamesReq.nextLevel;
   PhlGetParamInfo_t *pParamInfoList = NULL;
   int               bufsz       = 0;

   acsState.fault = CMSRET_SUCCESS; /* init to no fault */
          
   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      char              *pp         = a->ud.paramNamesReq.parameterPath;

      MdmPathDescriptor path;

      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;

         cmsLog_debug("start to handle rpc body at %d", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %d", time(NULL));
      }

      bufsz = 0;

      /* create response msg start */
      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sGetParameterNamesResponse>\n", nsCWMP);

      /* copy parameter list */
      xml_mIndent(pc, &bufsz, 4);
#ifdef SUPPRESS_SOAP_ARRAYTYPE
      mprintf(pc, &bufsz, "<ParameterList>\n");
#else
      /* In the first loop paramCnt=0.  In the second loop, paramCnt will have
       * the actual parameter count.
       */
      mprintf(pc, &bufsz,
              "<ParameterList %sarrayType=\"%sParameterInfoStruct[%06d]\">\n",
              nsSOAP_ENC, nsCWMP, paramCnt);
#endif

      if ((loop == 0) && (pp == NULL || strlen(pp) == 0) && nextLevelOnly == TRUE)
      {
         pParamInfoList = (PhlGetParamInfo_t *)cmsMem_alloc(sizeof(PhlGetParamInfo_t),ALLOC_ZEROIZE);

         if (pParamInfoList == NULL)
         {
             cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"memory allocation failure, in doGetParameterNames");
             acsState.fault = CMSRET_INTERNAL_ERROR;
             break; /* quit */
         }
         else
         {
            paramCnt = 1;
            acsState.fault = cmsMdm_fullPathToPathDescriptor(RootDevice, &(pParamInfoList[0].pathDesc));
         }
      }
      else
      {
         if (pp == NULL || strlen(pp) == 0)
         {
            acsState.fault = cmsMdm_fullPathToPathDescriptor(RootDevice,
                                                             &path);
         }
         else
         {
            acsState.fault = cmsMdm_fullPathToPathDescriptor(pp, &path);
         }
    
         if (acsState.fault != CMSRET_SUCCESS)
         {
            /* cannot find the given parameter name => fault should be invalid parameter name  */
            acsState.fault = CMSRET_INVALID_PARAM_NAME;
            break;   /* quit */
         }
    
         if (path.paramName[0] != 0 && nextLevelOnly == TRUE)
         {
            /* this is a parameter path */
            acsState.fault = CMSRET_INVALID_ARGUMENTS;
            break;
         }
    
         if (loop == 0)
         {
            paramCnt = 0;  /* reset paramCnt */
    
            acsState.fault = cmsPhl_getParameterNames(&path,nextLevelOnly,&pParamInfoList,&paramCnt);
    
            if (acsState.fault != CMSRET_SUCCESS)
            {
               break;
            }
         }
      }

      for (cnt=0; cnt<paramCnt; cnt++)
      {
         writeGetPName(&(pParamInfoList[cnt]), pc, &bufsz);
      }

      if (acsState.fault != CMSRET_SUCCESS)
      {
         break;   /* quit */
      }

      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "</ParameterList>\n");
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sGetParameterNamesResponse>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %ds", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %ds", time(NULL));

         cmsLog_log("RPC GetParameterNamesResponse sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   CMSMEM_FREE_BUF_AND_NULL_PTR(pParamInfoList);

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

   if (acsState.fault != CMSRET_SUCCESS)
   {
      /* build fault here */
      writeSoapFault(a, acsState.fault);
      cmsLog_debug("Fault %d", acsState.fault);
   }
   
}  /* doGetParameterNames() */

static void doSetParameterAttributes(RPCAction *a)
{
   AttributeItem  *pi = a->ud.aItem;

   acsState.fault = CMSRET_SUCCESS; /* init to no fault */
       
   if (pi != NULL)
   {
      int               numEntries = 0;
      PhlSetParamAttr_t *pSetParamAttrList;

      /* find out the total number of parameters requested */
      while (pi != NULL)
      {
         numEntries++;
         pi = pi->next;
      }
      pi = a->ud.aItem;

      /* allocate memory for the set parameter value list */
      pSetParamAttrList = cmsMem_alloc(numEntries * sizeof(PhlSetParamAttr_t),
                                        ALLOC_ZEROIZE);
      if (pSetParamAttrList == NULL)
      {
         cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"doSetParameterAttributes: malloc failed\n");
         acsState.fault = CMSRET_INTERNAL_ERROR;
      }
      else
      {
         PhlSetParamAttr_t   *pSetParamAttr = pSetParamAttrList;

         while (pi != NULL)
         {
            acsState.fault = cmsMdm_fullPathToPathDescriptor(pi->pname,
                                                 &(pSetParamAttr->pathDesc));
            if (acsState.fault != CMSRET_SUCCESS)
            {
               break;
            }
            pSetParamAttr->attributes.notificationChange = pi->chgNotify;
            pSetParamAttr->attributes.notification = pi->notification;
            pSetParamAttr->attributes.accessBitMaskChange = pi->chgAccess;
            pSetParamAttr->attributes.accessBitMask = NDA_ACCESS_TR69C;
            if (pi->subAccess != 0)
            {
               pSetParamAttr->attributes.accessBitMask |=  NDA_ACCESS_SUBSCRIBER;
            }

            pSetParamAttr++;
            pi = pi->next;
         }

         if (acsState.fault == CMSRET_SUCCESS)
         {
            acsState.fault = cmsPhl_setParameterAttributes(pSetParamAttrList,
                                                           numEntries);
         }
         /* free pSetParamAttrList buffer */
         CMSMEM_FREE_BUF_AND_NULL_PTR(pSetParamAttrList);
      }
   }
   else
   {
      /* no parameter specified - Invalid arguments */
      acsState.fault = CMSRET_INVALID_ARGUMENTS;
   }

   if (acsState.fault == CMSRET_SUCCESS)
   {
      int         loop;
      tProtoCtx   *pc;
      int   bufsz = 0;

      /* Loop through the code twice.  The first loop is to calculate
       * the size of the entire soap envelope so that the HTTP header
       * can be written to the TCP socket first.  The second loop is
       * to write the content of the envelope to the TCP socket as it
       * is generated.
       */
      for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
      {
         if (pc != NULL)
         {
            pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
            pc->length = 0;
            pc->maxlength = bufsz+1;
         
            cmsLog_debug("start to handle rpc body at %d", time(NULL));
         }
         else
         {
            cmsLog_debug("start to handle rpc header at %d", time(NULL));
         }
         
         bufsz = 0;

         openEnvWithHeader(a->ID, pc, &bufsz);
         openBody(pc, &bufsz);
         xml_mIndent(pc, &bufsz, 3);
         mprintf(pc, &bufsz, "<%sSetParameterAttributesResponse/>\n", nsCWMP);
         closeBodyEnvelope(pc, &bufsz);

         if (loop == 0)
         {
            /* send the HTTP message header */
            sendToAcs(bufsz, NULL);
         }
         else
         {
            cmsLog_debug("start to send rpc body at %ds", time(NULL));
            proto_SendRaw(pc, pc->content, pc->length);
            cmsLog_debug("stop to send rpc body at %ds", time(NULL));

            cmsLog_log("RPC SetParameterAttributesResponse sent to ACS");
         }
      }  /* for (loop = 0; loop < 2; loop++) */

      if (pc && pc->content)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
         pc->length = 0;
         pc->maxlength = 0;
      }

      saveConfigFlag = TRUE;
   }
   else
   {
      writeSoapFault(a, acsState.fault);
      cmsLog_debug("Fault %d", acsState.fault);
   }
}  /* End of doSetParameterAttributes() */

/* 
* GetParameterAttributes requests a single parameter path or single parameter path fragment
* This RPC uses the paramItem union member. 
*/
static void doGetParameterAttributes(RPCAction *a)
{
   int         loop;
   int         cnt           = 0;
   int         paramCnt      = 0;
   tProtoCtx   *pc           = NULL;

   MdmPathDescriptor *pNamePathList = NULL;

   PhlGetParamAttr_t *pParamAttrList = NULL;
   SINT32            NumParamAttrEntries = 0;
   int         bufsz = 0;

   acsState.fault = CMSRET_SUCCESS; /* init to no fault */

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      ParamItem   *pi   = a->ud.pItem;

      if (pi == NULL)
      {
         /* no parameter specified - Invalid arguments */
         acsState.fault = CMSRET_INVALID_ARGUMENTS;
         break;   /* quit */
      }

      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
      
         cmsLog_debug("start to handle rpc body at %d", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %d", time(NULL));
      }
      
      bufsz = 0;

      /* create response msg start */
      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sGetParameterAttributesResponse>\n", nsCWMP);

      /* copy parameter list */
      xml_mIndent(pc, &bufsz, 4);
#ifdef SUPPRESS_SOAP_ARRAYTYPE
      mprintf(pc, &bufsz, "<ParameterList>\n");
#else
      /* In the first loop paramCnt=0.  In the second loop, paramCnt will have
       * the actual parameter count.
       */
      mprintf(pc, &bufsz,
              "<ParameterList %sarrayType=\"%sParameterAttributeStruct[%06d]\">\n",
              nsSOAP_ENC, nsCWMP, paramCnt);
#endif

      if (loop == 0)
      {
         paramCnt = 0;  /* reset paramCnt */

         for ( ; pi != NULL && acsState.fault == CMSRET_SUCCESS; pi = pi->next)
         {
            paramCnt++;
         }

         pNamePathList = (MdmPathDescriptor*)cmsMem_alloc(sizeof(MdmPathDescriptor)*paramCnt,ALLOC_ZEROIZE);

         if (pNamePathList==NULL)
         {
            acsState.fault = CMSRET_INTERNAL_ERROR;
         }

         pi   = a->ud.pItem;
         for ( ; pi != NULL && acsState.fault == CMSRET_SUCCESS; pi = pi->next)
         {
             char              *pp = pi->pname;

             if (pp == NULL || strlen(pp) == 0)
             {
                acsState.fault = cmsMdm_fullPathToPathDescriptor(RootDevice,
                                                                 &(pNamePathList[cnt]));
             }
             else
             {
                acsState.fault = cmsMdm_fullPathToPathDescriptor(pp, &(pNamePathList[cnt]));
             }

             cnt++;

             if (acsState.fault != CMSRET_SUCCESS)
             {
                break;
             }
         }

         acsState.fault = cmsPhl_getParameterAttributes(pNamePathList,paramCnt,&pParamAttrList,&NumParamAttrEntries);

         if (acsState.fault != CMSRET_SUCCESS)
         {
            cmsLog_error(TR069_ALARM_PROCESS_ERROR,"cmsPhl_getParameterAttributes error(%d)", acsState.fault);
            break;   /* quit */
         }
      }

      for (cnt=0;cnt<NumParamAttrEntries;cnt++)
      {
         writeGetAttribute(&(pParamAttrList[cnt]), pc, &bufsz);
      }

      if (acsState.fault != CMSRET_SUCCESS)
      {
         break;   /* quit */
      }

      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "</ParameterList>\n");
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sGetParameterAttributesResponse>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %ds", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %ds", time(NULL));

         cmsLog_log("RPC GetParameterAttributesResponse sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

   cmsPhl_freeGetParamAttrBuf(pParamAttrList,NumParamAttrEntries);
   CMSMEM_FREE_BUF_AND_NULL_PTR(pNamePathList);

   if (acsState.fault != CMSRET_SUCCESS)
   {
      /* build fault here */
      writeSoapFault(a, acsState.fault);
      cmsLog_debug("Fault %d", acsState.fault);
   }
}  /* End of doGetParameterAttributes() */

static void doSetParameterValues(RPCAction *a)
{
   int         setParamReboot = rebootFlag;
   int notAllApplied=0;
   ParamItem   *pi            = a->ud.pItem;
#ifdef DMP_BASELINE_1               
   int configFileSavedToFlash = 0;
#endif
   int   bufsz = 0;

   acsState.fault = CMSRET_SUCCESS; /* init to no fault */

   if (pi != NULL)
   {
      int                  numEntries;
      PhlSetParamValue_t   *pSetParamValueList;

      /* find out the total number of parameters requested */
      numEntries = getParamCnt(pi);

      /* allocate memory for the set parameter value list */
      pSetParamValueList = cmsMem_alloc(numEntries * sizeof(PhlSetParamValue_t),
                                        ALLOC_ZEROIZE);
      if (pSetParamValueList == NULL)
      {
         cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"doSetParameterValues: malloc failed\n");
         acsState.fault = CMSRET_INTERNAL_ERROR;
      }
      else
      {
         PhlSetParamValue_t   *pSetParamValue = pSetParamValueList;

         /*
          * The tr69c xml parser (see fParameterNames) links the
          * parameter values in reverse order as they arrive over the
          * network.  So when we build the pSetParamValue struct,
          * build it in reverse order again so that the ODL sees the
          * same order as the ACS sent out.
          */
         pSetParamValue = pSetParamValueList + (numEntries - 1);

         while (pi != NULL)
         {
            /*
             * 1/21/08 plugfest: even when we detect an error early here,
             * keep going into cmsPhl_setParameterValues so we can detect
             * any other errors in the param list.
             */
            pi->fault = cmsMdm_fullPathToPathDescriptor(pi->pname,
                                                 &(pSetParamValue->pathDesc));

            if (pi->fault == CMSRET_SUCCESS)
            {
               if (pSetParamValue->pathDesc.paramName[0] == 0)
               {
                  /* set param value must have a param Name */
                  pSetParamValue->status = CMSRET_INVALID_PARAM_NAME;
               }

               if (pi->pvalue == NULL) /* only valid for tSting params */
               {
                  /* fake up a null string to avoid NULL ptr problem*/
                  pi->pvalue = cmsMem_strdup("");
               }

               pSetParamValue->pValue = pi->pvalue;
               pSetParamValue->pParamType = pi->ptype;

#ifdef DMP_BASELINE_1               
               if (acsState.dataModel == DATA_MODEL_TR98)
               {
                  /* this is only available in TR98, device:1.0 */
                  if (strcmp(pi->pname,"InternetGatewayDevice.DeviceConfig.ConfigFile") == 0)
                  {
                     /* This parameter takes ACS config file and saved to flash; 
                        do not overwrite the config file at the end of set RPC */
                     configFileSavedToFlash = 1;
                  }
               }
#endif
            }
            else
            {
               pSetParamValue->status = CMSRET_INVALID_PARAM_NAME;
            }

            pSetParamValue--;
            pi = pi->next;
         }

         acsState.fault = cmsPhl_setParameterValues(pSetParamValueList,
                                                       numEntries);
         if ((acsState.fault == CMSRET_SUCCESS_REBOOT_REQUIRED) ||
             (acsState.fault == CMSRET_SUCCESS_APPLY_NOT_COMPLETE))
         {
            notAllApplied = 1;
            if (acsState.fault == CMSRET_SUCCESS_REBOOT_REQUIRED)
            {
               setParamReboot = 1;
            }
            acsState.fault = CMSRET_SUCCESS;
         }
         else if (acsState.fault == CMSRET_INVALID_ARGUMENTS)
         {
            pi             = a->ud.pItem;
            pSetParamValue = pSetParamValueList + (numEntries - 1);

            while (pi != NULL)
            {
               pi->fault = pSetParamValue->status;
               pSetParamValue--;
               pi = pi->next;
            }
         }

         /*
          * Just free pSetParamValueList buffer itself.
          * pi->pvalue, pi->ptype, and the other buffers in paramItem will
          * be freed in freeParamItems at the end of the RPC.
          */
         CMSMEM_FREE_BUF_AND_NULL_PTR(pSetParamValueList);
      }
   }
   else
   {
      /* no parameter specified - Invalid arguments */
      acsState.fault = CMSRET_INVALID_ARGUMENTS;
   }

   if (acsState.fault == CMSRET_SUCCESS)
   {
      /* build good response */
      int         loop;
      tProtoCtx   *pc;

#ifdef DMP_BASELINE_1               
      if (!configFileSavedToFlash)
      {
         saveConfigFlag = TRUE;
      }
#endif

      if (setParamReboot)
      {
         rebootFlag = eACSSetValueReboot;
      }

      // update ACS username and password right away 
      // when credentials are changed BEFORE sending
      // SetParameterValuesResponse so that it can be sent
      // with NEW credentials and Status is 0. Since it might
      // be too late to wait for CMS_MSG_ACS_CONFIG_CHANGED 
      updateCredentialsInfo();

      /* Loop through the code twice.  The first loop is to calculate
       * the size of the entire soap envelope so that the HTTP header
       * can be written to the TCP socket first.  The second loop is
       * to write the content of the envelope to the TCP socket as it
       * is generated.
       */
      for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
      {

         if (pc != NULL)
         {
            pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
            pc->length = 0;
            pc->maxlength = bufsz+1;
         
            cmsLog_debug("start to handle rpc body at %ds", time(NULL));
         }
         else
         {
            cmsLog_debug("start to handle rpc header at %ds", time(NULL));
         }
         
         bufsz = 0;

         openEnvWithHeader(a->ID, pc, &bufsz);
         openBody(pc, &bufsz);
         xml_mIndent(pc, &bufsz, 3);
         mprintf(pc, &bufsz, "<%sSetParameterValuesResponse>\n", nsCWMP);
         xml_mIndent(pc, &bufsz, 4);
         mprintf(pc, &bufsz, "<Status>%s</Status>\n", (notAllApplied)?"1":"0");
         xml_mIndent(pc, &bufsz, 3);
         mprintf(pc, &bufsz,"</%sSetParameterValuesResponse>\n", nsCWMP);
         closeBodyEnvelope(pc, &bufsz);

         if (loop == 0)
         {
            /* send the HTTP message header */
            sendToAcs(bufsz, NULL);
         }
         else
         {
            cmsLog_debug("start to send rpc body at %ds", time(NULL));
            proto_SendRaw(pc, pc->content, pc->length);
            cmsLog_debug("stop to send rpc body at %ds", time(NULL));

            cmsLog_log("RPC SetParameterValuesResponse sent to ACS");
         }
      }  /* for (loop = 0; loop < 2; loop++) */

      if (pc && pc->content)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
         pc->length = 0;
         pc->maxlength = 0;
      }
   }
   else
   {
      /* build fault response */
      writeSoapFault(a, acsState.fault);
      cmsLog_debug("Fault %d", acsState.fault);
   }

}  /* End of doSetParameterValues() */

/*
* write SOAP value with type at buffer location **bp. Limit to size *bufsz.
 * increments paramCnt for each value written to buffer.
 *
*/
static void writeGetPValue(PhlGetParamValue_t *pParamValue, tProtoCtx *pc, int *bufsz)
{
   char  *pFullpath;

   /* convert MDM name path back to TR69 name string */
   cmsMdm_pathDescriptorToFullPath(&(pParamValue->pathDesc), &pFullpath);

   /* now fill in ParameterValueStruct in response */
   xml_mIndent(pc, bufsz, 6);
   mprintf(pc, bufsz, "<ParameterValueStruct>\n");
   xml_mIndent(pc, bufsz, 7);
   mprintf(pc, bufsz, "<Name>%s</Name>\n", pFullpath);
   xml_mIndent(pc, bufsz, 7);

   mprintf(pc, bufsz, "<Value %stype=\"%s%s\">",
               nsXSI, nsXSD, pParamValue->pParamType);

   /*
    * Certain parameters must always return empty string when read.
    * These parameters are marked with the isTr69Password="true" attribute
    * in cms-data-model.xml
    * If we detect that this is one of those password parameters, free the
    * actual value and replace it with an empty string.
    */
   if (pParamValue->isTr69Password)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pParamValue->pValue);
      pParamValue->pValue = cmsMem_strdup("");
   }

#ifdef SUPPRESS_EMPTY_PARAM
   if (empty(pParamValue->pValue))
   { 
      xml_mprintf(pc, bufsz, "empty");
   }
   else
   {
      xml_mprintf(pc, bufsz, pParamValue->pValue);
   }
#else
   xml_mprintf(pc, bufsz, pParamValue->pValue);
#endif
   mprintf(pc, bufsz, "</Value>\n");
   xml_mIndent(pc, bufsz, 6);
   mprintf(pc, bufsz, "</ParameterValueStruct>\n");

   CMSMEM_FREE_BUF_AND_NULL_PTR(pFullpath);

}  /* End of writeGetPValue() */

static void doGetParameterValues(RPCAction *a)
{
   int         loop;
   int         bufsz0        = 0;
   int         paramCnt      = 0;
   UINT32 cnt = 0;
   MdmPathDescriptor *pNamePathList = NULL;
   PhlGetParamValue_t   *pParamValueList = NULL;
   SINT32               numParamValueEntries = 0;
   tProtoCtx   *pc           = NULL;
   int         bufsz = 0;


   acsState.fault = CMSRET_SUCCESS; /* init to no fault */

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      ParamItem   *pi   = a->ud.pItem;

      if (pi == NULL)
      {
         /* no parameter specified - Invalid arguments */
         acsState.fault = CMSRET_INVALID_ARGUMENTS;
         break;   /* quit */
      }

      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
 
      bufsz = 0;


      /* create response msg start */
      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz,"<%sGetParameterValuesResponse>\n", nsCWMP);

      /* copy parameter list */
      xml_mIndent(pc, &bufsz, 4);
#ifdef SUPPRESS_SOAP_ARRAYTYPE
      mprintf(pc, &bufsz, "<ParameterList>\n");
#else
      /* In the first loop paramCnt=0.  In the second loop, paramCnt will have
       * the actual parameter count.
       */
      mprintf(pc, &bufsz,
              "<ParameterList %sarrayType=\"%sParameterValueStruct[%06d]\">\n",
              nsSOAP_ENC, nsCWMP, numParamValueEntries);
#endif

      if (loop == 0)
      {
          paramCnt = 0;  /* reset paramCnt */

          /* get all parameters now */
          for ( ; pi != NULL && acsState.fault == CMSRET_SUCCESS; pi = pi->next)
          {
             paramCnt++;
          }

          pNamePathList = (MdmPathDescriptor*)cmsMem_alloc(paramCnt * sizeof(MdmPathDescriptor),
                                        ALLOC_ZEROIZE);

          if (pNamePathList==NULL)
          {
             acsState.fault = CMSRET_INTERNAL_ERROR;
          }

          /* get all parameters now */
          pi   = a->ud.pItem;
          for ( ; pi != NULL && acsState.fault == CMSRET_SUCCESS; pi = pi->next)
          {
             char              *pp = pi->pname;
             if (pp == NULL || strlen(pp) == 0)
             {
                acsState.fault = cmsMdm_fullPathToPathDescriptor(RootDevice,
                                                                 &(pNamePathList[cnt]));
             }
             else
             {
                acsState.fault = cmsMdm_fullPathToPathDescriptor(pp, &(pNamePathList[cnt]));
             }

             cnt++;

             if (acsState.fault != CMSRET_SUCCESS)
             {
                break;
             }
          }

          if (acsState.fault != CMSRET_SUCCESS)
          {
             break;   /* quit */
          }

          acsState.fault = cmsPhl_getParameterValues(pNamePathList,paramCnt,&pParamValueList,&numParamValueEntries,TRUE);

          if (acsState.fault != CMSRET_SUCCESS)
          {
             cmsLog_error(TR069_ALARM_PROCESS_ERROR,"cmsPhl_getParameterValues error(%d)", acsState.fault);
             break;   /* quit */
          }
      }

      for (cnt=0;cnt<numParamValueEntries;cnt++)
      {
         writeGetPValue(&(pParamValueList[cnt]), pc, &bufsz);
      }

      if (acsState.fault != CMSRET_SUCCESS)
      {
         break;   /* quit */
      }

      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "</ParameterList>\n");
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sGetParameterValuesResponse>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         xml_mIndent(pc, &bufsz, MAX_PADDINGS);
         bufsz0 = bufsz;   /* save the envelope size */

         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         if (bufsz < bufsz0)
         {
            XMLmIndent(pc, &bufsz, bufsz0-bufsz);
         } 

         cmsPhl_freeGetParamValueBuf(pParamValueList,numParamValueEntries);

         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC GetParameterValuesResponse sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   CMSMEM_FREE_BUF_AND_NULL_PTR(pNamePathList);

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

   if (acsState.fault != CMSRET_SUCCESS)
   {
      writeSoapFault(a, acsState.fault);
      cmsLog_debug("Fault %d", acsState.fault);
   }

}  /* End of doGetParameterValues() */

/* AddObject
*
*/
static void doAddObject(RPCAction *a)
{
   char              *pp = a->ud.addDelObjectReq.objectName;
   int               len;
   MdmPathDescriptor path;

   acsState.fault = CMSRET_SUCCESS; /* init to no fault */
   
   /* The path name must end with a "." (dot) after the last node
    * in the hierarchical name of the object.
    */
   acsState.fault = CMSRET_INVALID_PARAM_NAME;
   if ((pp != NULL) &&
       ((len = strlen(pp)) > 0) && (len <= 256) &&
       (pp[len-1] == '.'))
   {
      char  *pLastToken;

      pp[len-1] = 0;
      if (((pLastToken = strrchr(pp, (int)'.')) != NULL) &&
          (isalpha(*(++pLastToken))))
      {
         acsState.fault = CMSRET_SUCCESS;
      }
      pp[len-1] = '.';
   }

   if (acsState.fault == CMSRET_SUCCESS)
   {
      acsState.fault = cmsMdm_fullPathToPathDescriptor(pp, &path);
      if (acsState.fault == CMSRET_SUCCESS && path.paramName[0] != 0)
      {
         acsState.fault = CMSRET_INVALID_PARAM_NAME;
      }

      if (acsState.fault == CMSRET_SUCCESS)
      {
         acsState.fault = cmsPhl_addObjInstance(&path);
         if (acsState.fault == CMSRET_SUCCESS_REBOOT_REQUIRED)
         {
            rebootFlag     = eACSAddObjectReboot;
            acsState.fault = CMSRET_SUCCESS;
         }
      }
   }

   if (acsState.fault == CMSRET_SUCCESS)
   {
      /* build AddObject response */
      int         loop;
      tProtoCtx   *pc;
      int   bufsz = 0;

      /* Loop through the code twice.  The first loop is to calculate
       * the size of the entire soap envelope so that the HTTP header
       * can be written to the TCP socket first.  The second loop is
       * to write the content of the envelope to the TCP socket as it
       * is generated.
       */
      for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
      {
         if (pc != NULL)
         {
            pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
            pc->length = 0;
            pc->maxlength = bufsz+1;
         
            cmsLog_debug("start to handle rpc body at %d", time(NULL));
         }
         else
         {
            cmsLog_debug("start to handle rpc header at %d", time(NULL));
         }
         
         bufsz = 0;

         openEnvWithHeader(a->ID, pc, &bufsz);
         openBody(pc, &bufsz);
         xml_mIndent(pc, &bufsz, 3);
         mprintf(pc, &bufsz, "<%sAddObjectResponse>\n", nsCWMP);
         xml_mIndent(pc, &bufsz, 4);
         mprintf(pc, &bufsz, "<InstanceNumber>%d</InstanceNumber>\n",
                     path.iidStack.instance[path.iidStack.currentDepth-1]);
         xml_mIndent(pc, &bufsz, 4);
         mprintf(pc, &bufsz, "<Status>%s</Status>\n", rebootFlag? "1":"0");
         xml_mIndent(pc, &bufsz, 3);
         mprintf(pc, &bufsz, "</%sAddObjectResponse>\n", nsCWMP);
         closeBodyEnvelope(pc, &bufsz);

         if (loop == 0)
         {
            /* send the HTTP message header */
            sendToAcs(bufsz, NULL);
         }
         else
         {
            cmsLog_debug("start to send rpc body at %d", time(NULL));
            proto_SendRaw(pc, pc->content, pc->length);
            cmsLog_debug("stop to send rpc body at %d", time(NULL));

            cmsLog_log("RPC AddObjectResponse sent to ACS");
         }
      }  /* for (loop = 0; loop < 2; loop++) */

      if (pc && pc->content)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
         pc->length = 0;
         pc->maxlength = 0;
      }

      saveConfigFlag = TRUE;
   }
   else
   {
      /* build fault here */
      writeSoapFault(a, acsState.fault);
      cmsLog_debug("Fault %d", acsState.fault);
   }
}  /* End of doAddObject() */

static void doDeleteObject(RPCAction *a)
{
   char              *pp = a->ud.addDelObjectReq.objectName;
   int               len;
   MdmPathDescriptor path;

   acsState.fault = CMSRET_SUCCESS; /* init to no fault */

   /* The path name must end with a "." (dot) after the instance
    * number of the object.
    */
   if ((pp != NULL) &&
       ((len = strlen(pp)) > 1) && (len <= 256) &&
       (pp[len-1] == '.') && (isdigit(pp[len-2])))
   {
      acsState.fault = CMSRET_SUCCESS;
   }
   else
   {
      acsState.fault = CMSRET_INVALID_PARAM_NAME;
      cmsLog_notice("not object path : %s", pp);
   }

   if (acsState.fault == CMSRET_SUCCESS)
   {
      acsState.fault = cmsMdm_fullPathToPathDescriptor(pp, &path);
      if (acsState.fault == CMSRET_SUCCESS && path.paramName[0] != 0)
      {
         acsState.fault = CMSRET_INVALID_PARAM_NAME;
         cmsLog_notice("convert to PathDescriptor failed : %s", pp);
      }

      if (acsState.fault == CMSRET_SUCCESS)
      {
         acsState.fault = cmsPhl_delObjInstance(&path);
         if (acsState.fault == CMSRET_SUCCESS_REBOOT_REQUIRED)
         {
            rebootFlag     = eACSDelObjectReboot;
            acsState.fault = CMSRET_SUCCESS;
         }
      }
   }

   if (acsState.fault == CMSRET_SUCCESS)
   {
      /* build DeleteObject response */
      int         loop;
      tProtoCtx   *pc;
      int   bufsz = 0;

      /* Loop through the code twice.  The first loop is to calculate
       * the size of the entire soap envelope so that the HTTP header
       * can be written to the TCP socket first.  The second loop is
       * to write the content of the envelope to the TCP socket as it
       * is generated.
       */
      for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
      {
         if (pc != NULL)
         {
            pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
            pc->length = 0;
            pc->maxlength = bufsz+1;
            cmsLog_debug("start to handle rpc body at %ds", time(NULL));
         }
         else
         {
            cmsLog_debug("start to handle rpc header at %ds", time(NULL));
         }
         
         bufsz = 0;

         openEnvWithHeader(a->ID, pc, &bufsz);
         openBody(pc, &bufsz);
         xml_mIndent(pc, &bufsz, 3);
         mprintf(pc, &bufsz, "<%sDeleteObjectResponse>\n", nsCWMP);
         xml_mIndent(pc, &bufsz, 4);
         mprintf(pc, &bufsz, "<Status>%s</Status>\n", rebootFlag? "1":"0");
         xml_mIndent(pc, &bufsz, 3);
         mprintf(pc, &bufsz, "</%sDeleteObjectResponse>\n", nsCWMP);
         closeBodyEnvelope(pc, &bufsz);

         if (loop == 0)
         {
            /* send the HTTP message header */
            sendToAcs(bufsz, NULL);
         }
         else
         {
            cmsLog_debug("start to send rpc body at %d", time(NULL));
            proto_SendRaw(pc, pc->content, pc->length);
            cmsLog_debug("stop to send rpc body at %d", time(NULL));

            cmsLog_log("RPC DeleteObjectResponse sent to ACS");
         }
      }  /* for (loop = 0; loop < 2; loop++) */

      if (pc && pc->content)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
         pc->length = 0;
         pc->maxlength = 0;
      }

      saveConfigFlag = TRUE;
   }
   else
   {
      /* build fault here */
      writeSoapFault(a, acsState.fault);
      cmsLog_debug("Fault %d", acsState.fault);
   }
}  /* End of doDeleteObject() */

static void doRebootRPC(RPCAction *a)
{
   int         loop;
   tProtoCtx   *pc;
   int   bufsz = 0;

   rebootFlag = eACSRPCReboot;

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sRebootResponse/>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC RebootResponse sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

}  /* End of doRebootRPC() */

static void doFactoryResetRPC(RPCAction *a)
{
   int         loop;
   tProtoCtx   *pc;
   int   bufsz = 0;

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sFactoryResetResponse/>\n", nsCWMP);
      closeBodyEnvelope(pc,&bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC FactoryResetResponse sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

   factoryResetFlag = 1;

}  /* End of doFactoryResetRPC() */


/** Build an Inform msg
*/
void buildInform(RPCAction *a, InformEvList *infEvent)
{
   int         cnt = 0;
   int         loop;
   int         bufsz0   = 0;
   int         paramCnt = 0;
   tProtoCtx   *pc      = NULL;
   char        dateTimeBuf[BUFLEN_64];
   CmsRet      ret;
   int numParamValueChanges = 0;
   PhlGetParamValue_t *pParamValueChanges = NULL;
   PhlGetParamValue_t *pParamChgValue = NULL;
   int bootstrap = 0, boot = 0, valuechange = 0;
   int numParms=0;
   int numInformDevIds=0;
   MdmPathDescriptor    *pNamePathList = NULL;

   PhlGetParamValue_t   *pParamValueList = NULL;
   SINT32               numParamValueEntries = 0;
   int                  bufsz        = 0;

   acsState.fault = CMSRET_SUCCESS; /* init to no fault */
   a->informID    = cmsMem_strdup(itoa(rand()));
   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      numInformDevIds = (sizeof(informDevIds_TR181)/sizeof(char*));
      informDevIds = informDevIds_TR181;
   }
   else
   {
      numInformDevIds = (sizeof(informDevIds_TR98)/sizeof(char *));
      informDevIds = informDevIds_TR98;
   }

   /* get informParameters */
   if (acsState.dataModel == DATA_MODEL_TR181)
   {
      informParameters = informParameters_TR181;
      numParms = (sizeof(informParameters_TR181)/sizeof(char*));
   }
   else
   {
      informParameters = informParameters_TR98;
      numParms = (sizeof(informParameters_TR98)/sizeof(char*));
   }

   pNamePathList = (MdmPathDescriptor*)cmsMem_alloc(sizeof(MdmPathDescriptor)*numParms,ALLOC_ZEROIZE);

   if (pNamePathList==NULL)
   {
      cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"malloc() failed for pNamePathList");
      return;
   }

   for (cnt = 0; cnt < (SINT32) numParms && acsState.fault == CMSRET_SUCCESS; cnt++)
   {
      acsState.fault = cmsMdm_fullPathToPathDescriptor(informParameters[cnt],
                                                       &pNamePathList[cnt]);
      if (acsState.fault != CMSRET_SUCCESS)
      {
         cmsLog_error(TR069_ALARM_PROCESS_ERROR,"fault on informParameters[%d]=%s", cnt, informParameters[cnt]);
      }
   
   }  /* for (i = 0; i < NUMREQINFORMPARMS; i++) */

   ret = cmsPhl_getParameterValues(pNamePathList, numParms, &pParamValueList, &numParamValueEntries,FALSE);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"fault on cmsPhl_getParameterValues ret = %d",ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(pNamePathList);

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      int                  i;

      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;


      openEnvWithHeader(a->informID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sInform>\n", nsCWMP);
      xml_mIndent(pc, &bufsz, 4);

      /* build DeviceId List */
      mprintf(pc, &bufsz, "<DeviceId>\n");

      for (i = 0; i < (SINT32) numInformDevIds && acsState.fault == CMSRET_SUCCESS; i++)
      {
         char  *pDevName;

         pDevName = strrchr(informDevIds[i], '.');
         pDevName++;
  
         xml_mIndent(pc, &bufsz, 5);
  
         if (strcmp(pDevName, "Manufacturer") == 0)
         {
            mprintf(pc, &bufsz, "<%s>%s</%s>\n",
                    pDevName, acsState.manufacturer, pDevName);
         }
         else if (strcmp(pDevName, "ManufacturerOUI") == 0)
         {
            mprintf(pc, &bufsz, "<OUI>%s</OUI>\n", acsState.manufacturerOUI);
         }
         else if (strcmp(pDevName, "ProductClass") == 0)
         {
            mprintf(pc, &bufsz, "<%s>%s</%s>\n",
                   pDevName, acsState.productClass, pDevName);
         }
         else if (strcmp(pDevName, "SerialNumber") == 0)
         {
            mprintf(pc, &bufsz, "<%s>%s</%s>\n",
                    pDevName, acsState.serialNumber, pDevName);
         }
      }  /* for (i = 0; i < NUMREQINFORMDEVIDS; i++) */

      if (acsState.fault != CMSRET_SUCCESS)
      {
         break;   /* quit */
      }

      /*If we get an BOOTSTRAP event, we need to discard all other event, except BOOT*/
      for (i = 0; loop == 0 && i < infEvent->informEvCnt; i++)
      {         
         if (infEvent->informEvList[i] == INFORM_EVENT_BOOTSTRAP)
            bootstrap = 1;
         else if (infEvent->informEvList[i] == INFORM_EVENT_BOOT)
            boot = 1;
         else if (infEvent->informEvList[i] == INFORM_EVENT_VALUE_CHANGE)
            valuechange = 1;
      }

      if( bootstrap == 1)
      {
         clearInformEventList(); //we got BOOTSTRAP event, so discard all event

         addInformEventToList(INFORM_EVENT_BOOTSTRAP);

         if(boot == 1)
            addInformEventToList(INFORM_EVENT_BOOT);
         
         //VALUE_CHANGE event is discarded, so we need to clear all parameters that value changed.
         if(valuechange == 1)
         {
            cmsPhl_clearAllParamValueChanges();
         }	 	
      }

      if (loop == 0)
      {
          /* get all value-change parameters */
          ret = cmsPhl_getAllParamValueChanges(&pParamValueChanges, &numParamValueChanges);

          if (ret != CMSRET_SUCCESS)
          {
             cmsLog_error(TR069_ALARM_PROCESS_ERROR,"fault on cmsPhl_getParameterValues ret = %d",ret);
          }
      }

      if (numParamValueChanges > 0)
      {
         /* This is for the case when passive notification needs to be sent.
          * If it's an active notination, the Value Change event is already in the 
          * list, and addInformEventToList() will do nothing but return
          */
         addInformEventToList(INFORM_EVENT_VALUE_CHANGE);
      }

      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "</DeviceId>\n");
      xml_mIndent(pc, &bufsz, 4);

      /* build Event List */
   #ifdef SUPPRESS_SOAP_ARRAYTYPE
      mprintf(pc, &bufsz, "<Event>\n");
   #else
      mprintf(pc, &bufsz, "<Event %sarrayType=\"%sEventStruct[%d]\">\n",
              nsSOAP_ENC, nsCWMP, infEvent->informEvCnt);
   #endif
      for (i = 0; i < infEvent->informEvCnt; i++)
      {
         char  *ck = NULL;

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<EventStruct>\n");
         xml_mIndent(pc, &bufsz, 6);
         mprintf(pc, &bufsz, "<EventCode>%s</EventCode>\n",
               getInformEvtString(infEvent->informEvList[i]));

         xml_mIndent(pc, &bufsz, 6);

         if (infEvent->informEvList[i]==INFORM_EVENT_REBOOT_METHOD)
         {
            ck = acsState.rebootCommandKey;
         }
         else if (infEvent->informEvList[i]==INFORM_EVENT_DOWNLOAD_METHOD)
         {
            ck = acsState.downloadCommandKey;
         }
         else if (infEvent->informEvList[i]==INFORM_EVENT_UPLOAD_METHOD)
         { 
            ck = acsState.downloadCommandKey;
         }
         else if (infEvent->informEvList[i]==INFORM_EVENT_SCHEDULE_METHOD)
         { 
            ck = acsState.scheduleInformCommandKey;
         }
         else if (infEvent->informEvList[i]==INFORM_EVENT_CHANGE_DU_CHANGE_METHOD)
         { 
            ck = acsState.duStateChangeCommandKey;
         }
         else if (infEvent->informEvList[i]==INFORM_EVENT_DU_CHANGE_COMPLETE)
         { 
            ck = acsState.duStateChangeCommandKey;
         }

   #ifdef SUPPRESS_EMPTY_PARAM
         if (empty(ck))
         {
            mprintf(pc, &bufsz, "<CommandKey>empty</CommandKey>\n");
         }
         else
         {
            mprintf(pc, &bufsz, "<CommandKey>%s</CommandKey>\n", ck);
         }
   #else
         mprintf(pc, &bufsz, "<CommandKey>%s</CommandKey>\n", ck? ck: "");
   #endif
         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz,"</EventStruct>\n");

      }  /* for (i = 0; i < infEvent->informEvCnt; i++) */

      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "</Event>\n");
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<MaxEnvelopes>1</MaxEnvelopes>\n");
      xml_mIndent(pc, &bufsz, 4);
      cmsTms_getXSIDateTime(0, dateTimeBuf, sizeof(dateTimeBuf));
      mprintf(pc, &bufsz, "<CurrentTime>%s</CurrentTime>\n", dateTimeBuf);
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<RetryCount>%d</RetryCount>\n", acsState.retryCount);
      xml_mIndent(pc, &bufsz, 4);

      /* build Parameter List */
   #ifdef SUPPRESS_SOAP_ARRAYTYPE
      mprintf(pc, &bufsz, "<ParameterList>\n");
   #else
      /* calculate the actual parameter count.
       */
      paramCnt = numParamValueEntries + numParamValueChanges;  /* reset paramCnt */

      mprintf(pc, &bufsz,
                  "<ParameterList %sarrayType=\"%sParameterValueStruct[%d]\">\n",
                  nsSOAP_ENC, nsCWMP, paramCnt);
   #endif

      for (i = 0; i < (SINT32) numParamValueEntries && acsState.fault == CMSRET_SUCCESS; i++)
      {
         writeGetPValue(&(pParamValueList[i]), pc, &bufsz);
      }  /* for (i = 0; i < NUMREQINFORMPARMS; i++) */

      if (acsState.fault != CMSRET_SUCCESS)
      {
         cmsLog_error(TR069_ALARM_PROCESS_ERROR,"failed to build inform, acsState.fault=%d", acsState.fault);
         break;   /* quit */
      }

      /* include all the value changed parameters */
      if (numParamValueChanges)
      {
          pParamChgValue = pParamValueChanges;
          for (i = 0; i < (SINT32) numParamValueChanges && pParamChgValue && acsState.fault == CMSRET_SUCCESS; i++)
          {
             writeGetPValue(pParamChgValue, pc, &bufsz);
             pParamChgValue = pParamChgValue->pNext;
          }  /* for (i = 0; i < numParamValueChanges; i++) */

      }
	  
      if (acsState.fault != CMSRET_SUCCESS)
      {
         break;   /* quit */
      }

      /* notify changes are released after ACS response */

      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "</ParameterList>\n");
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sInform>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         xml_mIndent(pc, &bufsz, MAX_PADDINGS);
         bufsz0 = bufsz;   /* save the envelope size */

         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         if (bufsz < bufsz0)
         {
            XMLmIndent(pc, &bufsz, bufsz0-bufsz);
         } 

         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         {
            char evtlist[128] = {0};
            for (i = 0; i < infEvent->informEvCnt; i++)
            {
               snprintf(evtlist+strlen(evtlist), sizeof(evtlist)-strlen(evtlist), "%s ", getInformEvtString(infEvent->informEvList[i]));
            }

            cmsLog_log("RPC Inform sent to ACS, event-list : %s",evtlist);
         }
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

   cmsPhl_freeGetParamValueBuf(pParamValueList, numParamValueEntries);

   if (acsState.fault == CMSRET_SUCCESS)
   {
      transferCompletePending = ((informState == eACSDownloadReboot) ||
                                 (informState == eACSUpload));
      /* increment retry count is moved to runRPC() function */
   }


}  /* End of buildInform() */

void sendGetRPCMethods(void)
{
   int         loop;
   tProtoCtx   *pc;
   int   bufsz = 0;

   cmsLog_debug("sendGetRPCMethods");

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      openEnvWithHeader(NULL, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sGetRPCMethods>\n", nsCWMP);
      xml_mIndent(pc, &bufsz,3);
      mprintf(pc, &bufsz, "</%sGetRPCMethods>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC GetRPCMethods sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

}  /* End of sendGetRPCMethods() */

static char* getFileName(char *pFileName)
{
    char*   pReturn = NULL;

    if ((pReturn = strrchr(pFileName,'/'))!=NULL)
    {
        pReturn++;
    }
    else
    {
        pReturn = pFileName;
    }

    return pReturn;
}

void sendAutonTransferComplete(void)
{
   int         loop;
   tProtoCtx   *pc;
   char        dateTimeBuf[BUFLEN_64];
   int   bufsz = 0;

   cmsLog_notice("sendAutonTransferComplete informState=%d",informState);

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      openEnvWithHeader(NULL, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sAutonomousTransferComplete>\n", nsCWMP);
  
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<AnnounceURL>%s</AnnounceURL>\n", acsState.announceURL);
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<TransferURL>%s</TransferURL>\n", acsState.transferURL);
      xml_mIndent(pc, &bufsz, 4);
      cmsLog_notice("sendAutonTransferComplete(%d) informState=%d",loop,informState);
      mprintf(pc, &bufsz, "<IsDownload>%d</IsDownload>\n", (!acsState.isDownload)?FALSE:TRUE);
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<FileType>%s</FileType>\n", cmsUtl_getFiletypeString(acsState.filetype));
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<FileSize>%ld</FileSize>\n", acsState.filesize);
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<TargetFileName>%s</TargetFileName>\n", getFileName(acsState.filename));
  
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<FaultStruct>\n");
      xml_mIndent(pc, &bufsz, 5);
      mprintf(pc, &bufsz, "<FaultCode>%d</FaultCode>\n",acsState.dlFaultStatus);
      xml_mIndent(pc, &bufsz, 5);
      mprintf(pc, &bufsz, "<FaultString>%s</FaultString>\n", acsState.dlFaultMsg);

      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "</FaultStruct>\n");
      xml_mIndent(pc, &bufsz, 4);
      cmsTms_getXSIDateTime(acsState.startDLTime, dateTimeBuf, sizeof(dateTimeBuf));

      cmsLog_notice("AutonomousTransferComplete StartTime is %s", dateTimeBuf);

      mprintf(pc, &bufsz, "<StartTime>%s</StartTime>\n", dateTimeBuf);
      xml_mIndent(pc, &bufsz, 4);
      cmsTms_getXSIDateTime(acsState.endDLTime, dateTimeBuf, sizeof(dateTimeBuf));

      cmsLog_notice("AutonomousTransferComplete CompleteTime is %s", dateTimeBuf);

      mprintf(pc, &bufsz, "<CompleteTime>%s</CompleteTime>\n", dateTimeBuf);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sAutonomousTransferComplete>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC AutonomousTransferComplete sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

}  /* End of sendTransferComplete() */

void sendTransferComplete(void)
{
   char        *ck;
   int         loop;
   tProtoCtx   *pc;
   char        dateTimeBuf[BUFLEN_64];
   int   bufsz = 0;

   cmsLog_notice("sendTransferComplete");

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      openEnvWithHeader("12345678", pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sTransferComplete>\n", nsCWMP);
      xml_mIndent(pc, &bufsz, 4);
      ck=acsState.downloadCommandKey;
      #ifdef SUPPRESS_EMPTY_PARAM
      if (ck && strlen(ck)>0)
      {
         mprintf(pc, &bufsz, "<CommandKey>%s</CommandKey>\n", ck);
      }
      else
      {
         mprintf(pc, &bufsz, "<CommandKey>empty</CommandKey>\n");
      }
      #else
      mprintf(pc, &bufsz, "<CommandKey>%s</CommandKey>\n", ck? ck: "");
      #endif
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<FaultStruct>\n");
      xml_mIndent(pc, &bufsz, 5);
      mprintf(pc, &bufsz, "<FaultCode>%d</FaultCode>\n",acsState.dlFaultStatus);
      xml_mIndent(pc, &bufsz, 5);
      mprintf(pc, &bufsz, "<FaultString>%s</FaultString>\n", acsState.dlFaultMsg?
      #ifdef SUPPRESS_EMPTY_PARAM
                  acsState.dlFaultMsg: "empty");
      #else
                  acsState.dlFaultMsg: "");
      #endif
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "</FaultStruct>\n");
      xml_mIndent(pc, &bufsz, 4);
      cmsTms_getXSIDateTime(acsState.startDLTime, dateTimeBuf, sizeof(dateTimeBuf));
      mprintf(pc, &bufsz, "<StartTime>%s</StartTime>\n", dateTimeBuf);
      xml_mIndent(pc, &bufsz, 4);
      cmsTms_getXSIDateTime(acsState.endDLTime, dateTimeBuf, sizeof(dateTimeBuf));
      mprintf(pc, &bufsz, "<CompleteTime>%s</CompleteTime>\n", dateTimeBuf);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sTransferComplete>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC TransferComplete sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

}  /* End of sendTransferComplete() */

static void doDownload(RPCAction *a)
{
   int         loop;
   tProtoCtx   *pc;
   DownloadReq *r = &a->ud.downloadReq;
   int   bufsz = 0;
   
   cmsLog_debug("doDownload");

   if (a == rpcAction)
   {
      rpcAction = NULL;  /* if *a is copy of rpcAction. set to NULL */
   }

   cmsLog_debug("preDownloadSetup: URL=%s", r->url);
   cmsLog_debug("User/pw: %s:%s", r->user, r->pwd);
   cmsLog_debug("Required memory buffer size will be %d", r->fileSize);

   /* build good response */
   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sDownloadResponse>\n", nsCWMP);
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<Status>1</Status>\n");
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<StartTime>0001-01-01T00:00:00Z</StartTime>\n");
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<CompleteTime>0001-01-01T00:00:00Z</CompleteTime>\n");
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sDownloadResponse>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC DownloadResponse sent to ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

   /* queue this event up */
   if (requestQueued(r,a->rpcMethod) == FALSE)
   {
      /* cannot queue this request, resource exceeded, mark it so when it's time to
         process it, an error is sent instead */
      r->state = eTransferRejected;
   }

   cmsTmr_set(tmrHandle, downloadStart, (void *)a, (1+r->delaySec)*1000, "download");
}  /* End of doDownload() */

static void doUpload(RPCAction *a)
{
   int         loop;
   tProtoCtx   *pc;
   DownloadReq *r = &a->ud.downloadReq;
   int   bufsz = 0;
   
   if (a == rpcAction)
      rpcAction = NULL;  /* if *a is copy of rpcAction. set to NULL */

   cmsLog_debug("UploadSetup: URL=%s", r->url);
   cmsLog_debug("User/pw: %s:%s", r->user, r->pwd);
   cmsLog_debug("Required memory buffer size will be %d", r->fileSize);
      
   /* build good response */
   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc()) 
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      /* build good response */
      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sUploadResponse>\n", nsCWMP);
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<Status>1</Status>\n");
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<StartTime>0001-01-01T00:00:00Z</StartTime>\n");
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "<CompleteTime>0001-01-01T00:00:00Z</CompleteTime>\n");
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sUploadResponse>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC UploadResponse sent to ACS");
      }
   }  /* for (loop.....) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

   /* queue this event up */
   if (requestQueued(r,a->rpcMethod) == FALSE)
   {
      /* cannot queue this request, resource exceeded, mark it so when it's time to
         process it, an error is sent instead */
      r->state = eTransferRejected;
   }
   cmsTmr_set(tmrHandle, uploadStart, (void *)a, (1+r->delaySec)*1000, "upload");
}  /* End of doUpload() */

/*
 * Update the ACSState commandKeys if they are in
 * the RPC.
 */
void updateKeys(RPCAction *a)
{
   if (a->parameterKey)
   {
      setMgmtServerParameterKeyLocked(a->parameterKey);
      a->parameterKey = NULL;
   }
   if (a->rpcMethod==rpcReboot && a->commandKey)
   {
      if (acsState.rebootCommandKey)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.rebootCommandKey);
      }
      acsState.rebootCommandKey = a->commandKey;
      a->commandKey = NULL;
   }
   if (a->rpcMethod==rpcScheduleInform && a->commandKey)
   {
      if (acsState.scheduleInformCommandKey)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.scheduleInformCommandKey);
      }
      acsState.scheduleInformCommandKey = a->commandKey;
      a->commandKey = NULL;
   }
   if (a->rpcMethod==rpcChangeDuState && a->commandKey)
   {
      if (acsState.duStateChangeCommandKey)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.duStateChangeCommandKey);
      }
      acsState.duStateChangeCommandKey = a->commandKey;
      a->commandKey = NULL;
   }
}  /* End of updateKeys() */

RunRPCStatus runRPC(void)
{
   RunRPCStatus   rpcStatus = eRPCRunOK;
   //CmsRet ret=CMSRET_SUCCESS;

   if (rpcAction != NULL)
   {
      cmsLog_debug("=====>ENTER: rcpMethod=%d", rpcAction->rpcMethod);
   }
   else
   {
      cmsLog_debug("=====>ENTER: NULL rpcAction pointer!");
   }

   acsState.fault = 0;

   if (!isAcsConnected())
   {
      cmsLog_debug("Not connected to ACS");
      rpcStatus = eRPCRunFail;
   }

   if ((rpcStatus == eRPCRunOK) && (rpcAction != NULL))
   {
      // after Inform is sent, no other RPCs should be sent before CPE receives InformResponse
      // if any RPC is sent before InformResponse (before sessionState is set to eSessionDeliveryConfirm)
      // then CPE should cancel current session and retry another session
      if (rpcAction->rpcMethod != rpcInformResponse && sessionState != eSessionDeliveryConfirm)
      {
         cmsLog_error(TR069_ALARM_RPC_ERROR,"state machine wrong state, fail conn");
         rpcStatus = eRPCRunFail;
         acsState.retryCount++;
         retrySessionConnection();
         saveTR69StatusItems();   // save retryCount to scratchpad
      }
      else
      {
        switch (rpcAction->rpcMethod)
        {
        case rpcGetRPCMethods:
           cmsLog_log("RPC GetRPCMethods received from ACS");
           doGetRPCMethods(rpcAction);
           break;
        case rpcSetParameterValues:
           cmsLog_log("RPC SetParameterValues received from ACS");
           doSetParameterValues(rpcAction);
           break;
        case rpcGetParameterValues:
           cmsLog_log("RPC GetParameterValues received from ACS");
           doGetParameterValues(rpcAction);
           break;
        case rpcGetParameterNames:
           cmsLog_log("RPC GetParameterNames received from ACS");
           doGetParameterNames(rpcAction);
           break;
        case rpcGetParameterAttributes:
           cmsLog_log("RPC GetParameterAttributes received from ACS");
           doGetParameterAttributes(rpcAction);
           break;
        case rpcSetParameterAttributes:
           cmsLog_log("RPC SetParameterAttributes received from ACS");
           doSetParameterAttributes(rpcAction);
           break;
        case rpcAddObject:
           cmsLog_log("RPC AddObject received from ACS");
           doAddObject(rpcAction);
           break;
        case rpcDeleteObject:
           cmsLog_log("RPC DeleteObject received from ACS");
           doDeleteObject(rpcAction);
           break;
        case rpcReboot:
           cmsLog_log("RPC Reboot received from ACS");
           doRebootRPC(rpcAction);
           break;
        case rpcFactoryReset:
           cmsLog_log("RPC FactoryReset received from ACS");
           doFactoryResetRPC(rpcAction);
           break;
        case rpcDownload:
           cmsLog_log("RPC Download received from ACS");
           doDownload(rpcAction);
           break;
        case rpcUpload:
           cmsLog_log("RPC Upload received from ACS");
           doUpload(rpcAction);
           break;
        case rpcScheduleInform:
           cmsLog_log("RPC ScheduleInform received from ACS");
           doScheduleInform(rpcAction);
           break;			
        case rpcGetQueuedTransfers:
           cmsLog_log("RPC GetQueuedTransfers received from ACS");
           doGetQueuedTransfers(rpcAction);
           break;
        case rpcInformResponse:
           cmsLog_log("RPC InformResponse received from ACS");
           // event delivery is confirmed by receiving InformResponse            
           sessionState = eSessionDeliveryConfirm;
           acsState.retryCount = 0;

           /* we have received an informResponse, clear inform event list */
           clearInformEventList();
	       saveLastConnectedURL();

           if (acsState.holdRequests == 0)
           {
              // only send pending request when holdrequests is false
              if (transferCompletePending == 1)
              {
                 /* make any callbacks that were setup when RPC started */
                 cmsLog_notice("acsState.autonomousDl=%d",acsState.autonomousDl);
                 if (acsState.autonomousDl)
                 {
                    sendAutonTransferComplete();
                 }
                 else
                 {
                    sendTransferComplete();
                 }

                 transferCompletePending = 0;
                 // setACSContactedState to eACSInformed for clearing 
                 // previous state which is eACSDownloadReboot or eACSUpload
                 setInformState(eACSInformed);
              }
              else if (sendGETRPC)
              {
                 sendGetRPCMethods();
                 sendGETRPC = 0;
              }
              else if (doSendDuStateComplete == TRUE)
              {
                 sendDuStateComplete();
                 doSendDuStateComplete = FALSE;
              }
              else if (doSendAutonDuStateComplete == TRUE)
              {
                 sendAutonDuStateComplete();
                 doSendAutonDuStateComplete = FALSE;
              }
              else
              {
                 /* send empty message to indcate no more requests */
                 sendNullHttp(TRUE);
                 rpcStatus=eRPCRunEnd;
              }
           }
           else
           { 
              // only send NULL msg when holdrequests is true
              sendNullHttp(TRUE);
              rpcStatus = eRPCRunEnd;
           }
           resetNotification();  /* update notifications following informResponse*/
           setInformState(eACSInformed);
           break;
        case rpcTransferCompleteResponse:
           cmsLog_log("RPC TransferCompleteResponse received from ACS");
           sendNullHttp(TRUE);
           rpcStatus = eRPCRunEnd;
           setInformState(eACSInformed);
           break;
        case rpcGetRPCMethodsResponse:
           cmsLog_log("RPC GetRPCMethodsResponse received from ACS");
           sendNullHttp(TRUE);
           rpcStatus = eRPCRunEnd;
           break;
        case rpcChangeDuState:
           cmsLog_log("RPC ChangeDuState received from ACS");
           doChangeDuState(rpcAction);
           break;
        case rpcAutonomousTransferCompleteResponse:
            cmsLog_log("RPC AutonomousTransferCompleteResponse received from ACS");
            sendNullHttp(TRUE);
            rpcStatus = eRPCRunEnd;
            break;
        case rpcAutonomousDUStateChangeCompleteResponse:
            cmsLog_log("RPC AutonomousDUStateChangeCompleteResponse received from ACS");
            sendNullHttp(TRUE);
            rpcStatus = eRPCRunEnd;
            break;
        case rpcUnknown:
           cmsLog_log("RPC Unknown received from ACS");
           rpcStatus = eRPCRunFail;
           acsState.fault = CMSRET_METHOD_NOT_SUPPORTED;
           writeSoapFault(rpcAction, acsState.fault);
           break;
#ifdef SUPPORT_TR69C_VENDOR_RPC
        case rpcVendorSpecific:
           cmsLog_log("RPC VendorSpecific received from ACS");
           callDoRpcByName(rpcAction->vendorRpcName);
           break;
#endif /* SUPPORT_TR69C_VENDOR_RPC */
        case rpcFault:
           cmsLog_log("RPC Fault received from ACS, code=%s string=%s, detail-fault(code=%d %s)",rpcAction->ud.fault.code,rpcAction->ud.fault.string,rpcAction->ud.fault.detail.code,rpcAction->ud.fault.detail.string);
           rpcStatus = eRPCRunFail;
           break;
        default:
           cmsLog_notice("default rpc case");
           rpcStatus = eRPCRunFail;
           break;
        }  /* end of switch(rpcAction->rpcMethod) */
      }

      /* if no faults then update ACS state with parameter key or command key. */
      if (rpcStatus != eRPCRunFail && acsState.fault == NO_FAULT && rpcAction != NULL)
      {
         /* in the case of download this must wait until the download completes*/
         updateKeys(rpcAction);
      }

   } /* end of if ((rpcStatus == eRPCRunOK) && (rpcAction != NULL)) */


   cmsLog_debug("=====>EXIT, rpcStatus=%d", rpcStatus);

#ifdef verbose_mem_stats
   {
      /*
       * This bit of code can be used to detect whether tr69c memory use
       * is increasing after each RPC.  Increasing memory usage most likely
       * means memory leak.
       */
      CmsMemStats stats;

      cmsMem_getStats(&stats);

      printf("\n\n=========== tr69c (pid=%d) mem stats after RPC =================\n", getpid());
      printf("shmBytesAllocd=%u (total=%u) shmNumAllocs=%u shmNumFrees=%u delta=%u\n",
             stats.shmBytesAllocd, stats.shmTotalBytes,
             stats.shmNumAllocs, stats.shmNumFrees, stats.shmNumAllocs - stats.shmNumFrees);
      printf("bytesAllocd=%u numAllocs=%u numFrees=%u delta=%u\n",
             stats.bytesAllocd, stats.numAllocs, stats.numFrees,
             stats.numAllocs-stats.numFrees);
      printf("============================================\n\n");
   }
#endif

   return rpcStatus;
}  /* End of runRPC() */

void resetNotification(void)
{
   cmsLog_debug("============>ENTER");
   /* we must be inside an RPC method function because we have the lock at this point. */
   cmsPhl_clearAllParamValueChanges();
}

int checkActiveNotifications(void)
{
   int active = 0;
   return active;
}

void initTransferList(void)
{
   UINT16 size, i, queueEntryCount;
   DownloadReq *q;
   RPCAction *a;
   DownloadReqInfo savedList[TRANSFER_QUEUE_SIZE], *saved;

   memset((void*)&transferList,0,sizeof(TransferInfo));
   size = i = queueEntryCount = 0;

   /* if retrieve fails because nothing was read or error occured, clear list */
   if (tr69RetrieveTransferListFromStore(savedList, &size) == CMSRET_SUCCESS && size > 0)
   {
      /* remove the tr69c_transfer list from scratch pad by setting it again with len=0 */
      cmsPhl_setTransferList(NULL, 0);

      queueEntryCount = size / sizeof(DownloadReqInfo);
      for (i = 0; i < queueEntryCount; i++)
      {
         saved = &savedList[i];
         q = &transferList.queue[i].request;
         
         q->efileType = saved->efileType;
         q->commandKey = (strlen(saved->commandKey) > 0) ? cmsMem_strdup(saved->commandKey) : NULL;
         q->url = (strlen(saved->url) > 0) ? cmsMem_strdup(saved->url) : cmsMem_strdup("");
         q->user = (strlen(saved->user) > 0) ? cmsMem_strdup(saved->user) : cmsMem_strdup("");
         q->pwd = (strlen(saved->pwd) > 0) ? cmsMem_strdup(saved->pwd) : cmsMem_strdup("");
         q->fileSize = saved->fileSize;
         q->fileName = (strlen(saved->fileName) > 0) ? cmsMem_strdup(saved->fileName) : cmsMem_strdup("");
         q->delaySec = saved->delaySec;
         q->state = saved->state;
         transferList.queue[i].rpcMethod = saved->rpcMethod;
         if (q->state == eTransferNotYetStarted)
         {
            a = newRPCAction(); /* need to be freed somewhere? */
            if(a==NULL)
            {
                CMSMEM_FREE_BUF_AND_NULL_PTR(q->commandKey);
                CMSMEM_FREE_BUF_AND_NULL_PTR(q->url);
                CMSMEM_FREE_BUF_AND_NULL_PTR(q->user);
                CMSMEM_FREE_BUF_AND_NULL_PTR(q->pwd);
                CMSMEM_FREE_BUF_AND_NULL_PTR(q->fileName);
                memset((void*)q,0,sizeof(DownloadReq));
                return;
            }
            memcpy(&a->ud.downloadReq,q,sizeof(DownloadReq));
            a->rpcMethod = transferList.queue[i].rpcMethod;
            if (transferList.queue[i].rpcMethod == rpcDownload)
            {
               cmsTmr_set(tmrHandle, downloadStart, (void *)a, (1+q->delaySec)*1000, "download");
            }
            else
            {
               cmsTmr_set(tmrHandle, uploadStart, (void *)a, (1+q->delaySec)*1000, "upload");
            }
         } /* eTransferNotYetStarted */         
      } /* for */
   } /* read from scratch pad */
   else
   {
#ifdef DEBUG
      printf("initTransferList(): no list read from persistent storage.\n");
#endif
   }
} /* initTransferList */

/* return true if a transfer transaction is in progess; 0 if not.*/
int isTransferInProgress(void)
{
   int i;
   DownloadReq *q;
   
   for (i = 0; i < TRANSFER_QUEUE_SIZE; i++)
   {
      q = &transferList.queue[i].request;
      if (q->state == eTransferInProgress)
      {
         return 1;
      }
   }
   return (0);
}

void transferListEnqueueCopy(DownloadReq *q, DownloadReq *r)
{
   if ((q != NULL) && (r != NULL))
   {
      q->efileType = r->efileType;
      q->commandKey = (r->commandKey != NULL) ? cmsMem_strdup(r->commandKey) : NULL;
      q->url = (r->url != NULL) ? cmsMem_strdup(r->url) : cmsMem_strdup("");
      q->user = (r->user != NULL) ? cmsMem_strdup(r->user) : cmsMem_strdup("");
      q->pwd = (r->pwd != NULL) ? cmsMem_strdup(r->pwd) : cmsMem_strdup("");
      q->fileSize = r->fileSize;
      q->fileName = (r->fileName != NULL) ? cmsMem_strdup(r->fileName) : cmsMem_strdup("");
      q->delaySec = r->delaySec;
      q->state = eTransferNotYetStarted;
   }
}

void transferListDequeueFree(DownloadReq *q)
{
   if (q->commandKey)
      CMSMEM_FREE_BUF_AND_NULL_PTR(q->commandKey);
   if (q->url)
      CMSMEM_FREE_BUF_AND_NULL_PTR(q->url);
   if (q->user)
      CMSMEM_FREE_BUF_AND_NULL_PTR(q->user);
   if (q->pwd)
      CMSMEM_FREE_BUF_AND_NULL_PTR(q->pwd);
   if (q->fileName)
      CMSMEM_FREE_BUF_AND_NULL_PTR(q->fileName);
   memset((void*)q,0,sizeof(DownloadReq));
}

/* This function queue the request if there is room and return 1, otherwise, return 0 */
int requestQueued(DownloadReq *r, eRPCMethods method)
{
   int i, j;
   DownloadReq *q;
   
   for (i = 0; i < TRANSFER_QUEUE_SIZE; i++)
   {
      q = &transferList.queue[i].request;
      if (q->state == eTransferNotInitialized)
      {
         transferListEnqueueCopy(q,r);
         transferList.queue[i].rpcMethod = method;
         /* request is queued */

         return (1); 
      }
   } /* for */
   /* queue is full; purge the completed one for this new request */
   /* the goal is to leave the recently completed ones for ACS to queried, overwrite the oldest
      completed entries when when purged */
   for (i = transferList.mostRecentCompleteIndex+1, j=0; j < TRANSFER_QUEUE_SIZE; i++, j++)
   {
      q = &transferList.queue[i%TRANSFER_QUEUE_SIZE].request;
      if (q->state == eTransferCompleted)
      {
         transferListDequeueFree(q);
         transferListEnqueueCopy(q,r);
         transferList.queue[i%TRANSFER_QUEUE_SIZE].rpcMethod = method;

         return (1); 
      }
   } /* for */
   /* queue is full; we have to reject this request; there is no room to queue it */
   /* return CMSRET_RESOURCE_EXCEEDED, resource exceeded: how?
      acsState.fault is a global flag and if a transfer is in progress, this doesn't work */
   
   return (0);
}

void updateTransferState(char *commandKey, eTransferState state)
{
   int i;
   DownloadReq *q;

   for (i = 0; i < TRANSFER_QUEUE_SIZE; i++)
   {
      q = &transferList.queue[i].request;
      if (q->state != eTransferNotInitialized)
      {
         if ((commandKey == NULL && q->commandKey == NULL) ||
             (commandKey != NULL && q->commandKey != NULL && 
              strcmp(commandKey,q->commandKey) == 0))
         {
            q->state = state;
            if (state == eTransferCompleted)
            {
               transferList.mostRecentCompleteIndex = i;
            }
            break;
         }
      }
   } /* for */
}

static void doGetQueuedTransfers(RPCAction *a)
{
   int i;
   int         loop;
   tProtoCtx   *pc;
   DownloadReq *q;
   TransferInfo list;
   int qEntryCount = 0;
   int   bufsz = 0;

   memcpy(&list,&transferList,sizeof(TransferInfo));

   for (i = 0; i < TRANSFER_QUEUE_SIZE; i++) {
      q = &list.queue[i].request;
      if (q->state != eTransferNotInitialized)
      {
         qEntryCount++;
      }
   } /* for */

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc()) {

      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sGetQueuedTransfersResponse>\n", nsCWMP);
      xml_mIndent(pc, &bufsz, 4);
      #ifdef SUPPRESS_SOAP_ARRAYTYPE
      mprintf(pc, &bufsz, "<TransferList>\n");
      #else
      mprintf(pc, &bufsz, "<TransferList %sarrayType=\"%sQueuedTransferStruct[%d]\">\n",
              nsSOAP_ENC, nsCWMP, qEntryCount );
      #endif

      
      for (i = 0; i < qEntryCount; i++) {
         q = &list.queue[i].request;
         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<QueuedTransferStruct>\n");
         if (q->state != eTransferNotInitialized)
         {
            xml_mIndent(pc, &bufsz, 6);
            if (q->commandKey != NULL)
               mprintf(pc, &bufsz, "<CommandKey>%s</CommandKey>\n", q->commandKey);
            else
               mprintf(pc, &bufsz, "<CommandKey></CommandKey>\n");
            xml_mIndent(pc, &bufsz, 6);
            mprintf(pc, &bufsz, "<State>%d</State>\n", q->state);
         }
         else
         {
            xml_mIndent(pc, &bufsz, 6);
            mprintf(pc, &bufsz, "<CommandKey></CommandKey>\n");
            xml_mIndent(pc, &bufsz, 6);
            mprintf(pc, &bufsz, "<State></State>\n");
         }
         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "</QueuedTransferStruct>\n");
      } /* for all queued entries */
      xml_mIndent(pc, &bufsz, 4);
      mprintf(pc, &bufsz, "</TransferList>\n");
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sGetQueuedTransfersResponse>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0) {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC GetQueuedTransfersResponse sent to ACS");
      }
   }  /* for (loop.....) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

}  /* End of doGetQueuedTransfers() */

void setsaveConfigFlag(UBOOL8 flag)
{
   saveConfigFlag = flag;
}


/* in main.c where it can be in source format */
extern CmsRet sendReqToExecEnvMngrLocked(ChangeDuStateOpReqInfo *op, SINT32 opNum);

void cleanupAcsDuReqList(void)
{
   ChangeDuStateOpReqInfo *pOpReq;
   ChangeDuStateOpReqInfo *p;

   /* XXX miwang: can we consolidate this with function below? */

   pOpReq = acsState.duStateOpList;
   while (pOpReq != NULL)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pOpReq->duRef);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pOpReq->version);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pOpReq->euRef);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pOpReq->fault.faultString);
      p = pOpReq;
      pOpReq = pOpReq->next;
      CMSMEM_FREE_BUF_AND_NULL_PTR(p);
   } /* while opReqList */
   acsState.duStateOpList = NULL;
   acsState.pendingDuReq = 0;
}

static void freeOpReqList(ChangeDuStateOpReqInfo *pOpReqList)
{
   ChangeDuStateOpReqInfo *p = pOpReqList;
   ChangeDuStateOpReqInfo *pFree;

   /* XXX miwang: is this the same as freeOsgiResultList ?
    * and can we consolidate with the function above?
    */
   while (p != NULL)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(p->fault.faultString);
      CMSMEM_FREE_BUF_AND_NULL_PTR(p->url);
      CMSMEM_FREE_BUF_AND_NULL_PTR(p->version);
      pFree = p;
      p = p->next;
      CMSMEM_FREE_BUF_AND_NULL_PTR(pFree);
   }
}

static void doChangeDuState(RPCAction *a)
{
   int         loop;
   tProtoCtx   *pc;
   ChangeDuStateOp  *pRequestOp = a->ud.opItem;
   ChangeDuStateOp  *pOp = pRequestOp;
   int opCount = 0;

   ChangeDuStateOpReqInfo *pOpReqList =NULL;
   ChangeDuStateOpReqInfo *pReq, *p;


   cmsLog_debug("doChangeDuState, pRequestOp %p",pRequestOp);

   if (pRequestOp != NULL)
   {
      /* first determine the number of operation requested in the method */
      while (pOp != NULL)
      {
         opCount++;
         pOp = pOp->next;
      }
      if (opCount > MAX_DU_OP_REQUEST)
      {
         acsState.fault = CMSRET_SW_MODULE_SYSTEM_RESOURCE_EXCEEDED;
         cmsLog_debug("opCount %d exceeds %d",opCount,MAX_DU_OP_REQUEST);
      }
      else
      {
         /*
          * so here, we maintain a seperate list to keep track of the status for
          * DUStateChangeComplete response.  This list is a superset of action
          * since it not only contains the action's parameter, but also other info
          * needed for the response.
          * The request in the action list is processed from last one to first since
          * XML parser store the last op request at the beginning of the list.
          * DUStateChangeComplete must be in the same order as they come in from the
          * ACS, the order of this list is significant.
          * *pRequestOp and *pOp traverse action's request list from XML parser.
          * *pOpReqList and *pReq is the list we process at this level (saved to ACSState).
          */
         cmsLog_debug("opCount %d in rpcaction",opCount);

         /* now, build up the pOpReqList from the rpc action, flipping the order around */
         while (pRequestOp != NULL)
         {
            pReq = cmsMem_alloc(sizeof(ChangeDuStateOpReqInfo),ALLOC_ZEROIZE);
            if (pReq == NULL)
            {
               cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"doSetDuState: malloc of ChangeDuStateOpReqInfo failed\n");
               acsState.fault = CMSRET_INTERNAL_ERROR;
               /* we do nothing else if it fails at this state, go free what we had allocated */
               freeOpReqList(pOpReqList);
            }
            else
            {
               /* copy the info over */
               /* save these later for changeDuStateComplete */
               pReq->operationType = pRequestOp->operationType;
               if (pReq->operationType == eDuInstall)
               {
                  /*
                   * URL: required.
                   * UUID: if not provided, CPE generates.
                   * username: optional
                   * password: optional
                   * ExecutionRef: If not provided, its OK if there is only
                   *               one Exec Env on the system, otherwise, error.
                   */
                  if (pRequestOp->url == NULL || pRequestOp->url[0] == '\0')
                  {
                     pReq->fault.faultCode = CMSRET_INVALID_ARGUMENTS;
                     pReq->fault.faultString = cmsMem_strdup(cmsErr_getString(pReq->fault.faultCode));
                     cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"install op: missing URL");
                  }
                  else
                  {
                     /* more checking on the URL */
                     if (strchr(pRequestOp->url, '@') != NULL)
                     {
                        /* it looks like name, password are included in URL */
                        pReq->fault.faultCode = CMSRET_INVALID_ARGUMENTS;
                        pReq->fault.faultString = cmsMem_strdup(cmsErr_getString(pReq->fault.faultCode));
                        cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"install op: url %s contains password/user info?",
                                     pRequestOp->url);
                     }
                     else
                     {
                        pReq->url = cmsMem_strdup(pRequestOp->url);
                     }
                  }

                  if (pReq->fault.faultCode == CMSRET_SUCCESS)
                  {
                     if (pRequestOp->uuid == NULL || pRequestOp->uuid[0] == '\0')
                     {
                        UINT8 systemMacAddr[MAC_ADDR_LEN]={0};
                        devCtl_getBaseMacAddress(systemMacAddr);
                        cmsUtl_generateUuidStr(pReq->uuid,sizeof(pReq->uuid),systemMacAddr);
                        printf("tr69c is generating local UUID=%s\n", pReq->uuid);
                     }
                     else
                     {
                        cmsUtl_strncpy(pReq->uuid, pRequestOp->uuid, sizeof(pReq->uuid));
                     }
                  }

                  if (pReq->fault.faultCode == CMSRET_SUCCESS)
                  {
                     if (pRequestOp->user != NULL)
                     {
                        pReq->user = cmsMem_strdup(pRequestOp->user);
                     }
                     if (pRequestOp->pwd != NULL)
                     {
                        pReq->pwd = cmsMem_strdup(pRequestOp->pwd);
                     }
                  }

                  if (pReq->fault.faultCode == CMSRET_SUCCESS)
                  {
                     if (pRequestOp->exeEnvRef == NULL || pRequestOp->exeEnvRef[0] == '\0')
                     {
                        pReq->fault.faultCode =
                               qdmModsw_getExecEnvFullPathByNameLocked(NULL,
                                              pReq->execEnvFullPath,
                                              sizeof(pReq->execEnvFullPath));
                        if (pReq->fault.faultCode != CMSRET_SUCCESS)
                        {
                           pReq->fault.faultString = cmsMem_strdup(cmsErr_getString(pReq->fault.faultCode));
                        }
                     }
                     else
                     {
                        cmsUtl_strncpy(pReq->execEnvFullPath, pRequestOp->exeEnvRef,
                                       sizeof(pReq->execEnvFullPath));
                     }
                  }
               }
               else if (pReq->operationType == eDuUpdate)
               {
                  /*
                   * URL: required
                   * UUID: required
                   * version: optional
                   * username: optional
                   * password: optional
                   */
                  if (pRequestOp->url == NULL || pRequestOp->url[0] == '\0')
                  {
                     pReq->fault.faultCode = CMSRET_INVALID_ARGUMENTS;
                     pReq->fault.faultString = cmsMem_strdup(cmsErr_getString(pReq->fault.faultCode));
                     cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"update op: missing URL");
                  }
                  else
                  {
                     /* more checking on the URL */
                     if (strchr(pRequestOp->url, '@') != NULL)
                     {
                        /* it looks like name, password are included in URL */
                        pReq->fault.faultCode = CMSRET_INVALID_ARGUMENTS;
                        pReq->fault.faultString = cmsMem_strdup(cmsErr_getString(pReq->fault.faultCode));
                        cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"update op: url %s contains password/user info?",
                                     pRequestOp->url);
                     }
                     else
                     {
                        pReq->url = cmsMem_strdup(pRequestOp->url);
                     }
                  }

                  if (pReq->fault.faultCode == CMSRET_SUCCESS)
                  {
                     if (pRequestOp->uuid == NULL || pRequestOp->uuid[0] == '\0')
                     {
                        pReq->fault.faultCode = CMSRET_INVALID_ARGUMENTS;
                        pReq->fault.faultString = cmsMem_strdup(cmsErr_getString(pReq->fault.faultCode));
                        cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"update op: missing UUID argument");
                     }
                     else
                     {
                        cmsUtl_strncpy(pReq->uuid, pRequestOp->uuid, sizeof(pReq->uuid));
                     }
                  }

                  if (pReq->fault.faultCode == CMSRET_SUCCESS)
                  {
                     if (pRequestOp->version != NULL)
                     {
                        pReq->version = cmsMem_strdup(pRequestOp->version);
                     }
                     if (pRequestOp->user != NULL)
                     {
                        pReq->user = cmsMem_strdup(pRequestOp->user);
                     }
                     if (pRequestOp->pwd != NULL)
                     {
                        pReq->pwd = cmsMem_strdup(pRequestOp->pwd);
                     }
                  }
               }
               else if (pReq->operationType == eDuUninstall)
               {
                  /*
                   * UUID: required
                   * version: optional
                   * ExecEnv: optional
                   */
                  if (pRequestOp->uuid == NULL || pRequestOp->uuid[0] == '\0')
                  {
                     pReq->fault.faultCode = CMSRET_INVALID_ARGUMENTS;
                     pReq->fault.faultString = cmsMem_strdup(cmsErr_getString(pReq->fault.faultCode));
                     cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"uninstall op: missing UUID argument");
                  }
                  else
                  {
                     cmsUtl_strncpy(pReq->uuid, pRequestOp->uuid, sizeof(pReq->uuid));
                  }

                  if (pReq->fault.faultCode == CMSRET_SUCCESS)
                  {
                     if (pRequestOp->version != NULL)
                     {
                        pReq->version = cmsMem_strdup(pRequestOp->version);
                     }
                     if (pRequestOp->exeEnvRef != NULL)
                     {
                        cmsUtl_strncpy(pReq->execEnvFullPath, pRequestOp->exeEnvRef,
                                       sizeof(pReq->execEnvFullPath));
                     }
                  }
               }
               else
               {
                  cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"Unrecognized op %d", pReq->operationType);
               }

               /* put this pReq on list */
               if (pOpReqList == NULL)
               {
                  /* first one */
                  pOpReqList = pReq;
               }
               else
               {
                  /* put it at the end of the list */
                  p = pOpReqList;
                  while (p->next != NULL)
                  {
                     p = p->next;
                  }
                  p->next = pReq;
               }
            } /* start building the list saved on AcsState */

            pRequestOp = pRequestOp->next;
         } /* process the whole request list of request from XML parser while (pRequestOp != NULL)*/

         acsState.duStateOpList = pOpReqList;
         /* now, process each request at a time */
         pReq = pOpReqList;
         while (pReq != NULL)
         {
            if (pReq->fault.faultString == NULL)
            {
               acsState.pendingDuReq++;
            }
            /* move to the next request if any */
            pReq = pReq->next;
         } /* while until done with sending request ExecEnv Mngr */

         if (acsState.pendingDuReq > 0)
         {
            cmsLog_debug("sendReqToExecEnvMngr called successfully, pendingDuReq %d",acsState.pendingDuReq);

            if (sendReqToExecEnvMngrLocked(pOpReqList,acsState.pendingDuReq) != CMSRET_SUCCESS)
            {
               pReq = pOpReqList;
               while (pReq != NULL)
               {
                  if (pReq->fault.faultString == NULL)
                  {
                     pReq->fault.faultCode = CMSRET_INTERNAL_ERROR;
                     pReq->fault.faultString = cmsMem_strdup(cmsErr_getString(pReq->fault.faultCode));
                  }
                  /* move to the next request if any */
                  pReq = pReq->next;
               }

               acsState.pendingDuReq = 0;
               acsState.fault = CMSRET_INTERNAL_ERROR;
            }
         }
         else
         {
            cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"all du-req info are not corret");
            acsState.fault = CMSRET_DOWNLOAD_FAILURE;
         }

         /* send changeDUStateResponse; just a no argument respond */
         /* Loop through the code twice.  The first loop is to calculate
          * the size of the entire soap envelope so that the HTTP header
          * can be written to the TCP socket first.  The second loop is
          * to write the content of the envelope to the TCP socket as it
          * is generated.
          */
         int   bufsz = 0;
         for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
         {
            if (pc != NULL)
            {
               pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
               pc->length = 0;
               pc->maxlength = bufsz+1;
               cmsLog_debug("start to handle rpc body at %ds", time(NULL));
            }
            else
            {
               cmsLog_debug("start to handle rpc header at %ds", time(NULL));
            }
            
            bufsz = 0;

            openEnvWithHeader(a->ID, pc, &bufsz);
            openBody(pc, &bufsz);
            xml_mIndent(pc, &bufsz, 3);
            mprintf(pc, &bufsz, "<%sChangeDUStateResponse/>\n", nsCWMP);
            closeBodyEnvelope(pc,&bufsz);

            if (loop == 0)
            {
               /* send the HTTP message header */
               sendToAcs(bufsz, NULL);
            }
            else
            {
               cmsLog_debug("start to send rpc body at %d", time(NULL));
               proto_SendRaw(pc, pc->content, pc->length);
               cmsLog_debug("stop to send rpc body at %d", time(NULL));

               cmsLog_log("RPC ChangeDUStateResponse sent to ACS");
            }
         }  /* for (loop = 0; loop < 2; loop++) */

         if (pc && pc->content)
         {
            CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
            pc->length = 0;
            pc->maxlength = 0;
         }

      } /* else malloc ok */
   } /* pRequestOp != NULL */

   cmsLog_debug("sent changeDUStateResponse.  Now wait for all requests to complete to send Complete Msg");

   /*
    * We only send sendDuStateComplete when all DU requests are completed by OSGI.
    * However, if there is a problem, timeout will takes place, and it's sent with an error.
    */
   if (acsState.fault == CMSRET_SUCCESS)
   {
      cmsTmr_set(tmrHandle, goSendDuStateComplete, (void*)NULL, CHANGEDUSTATE_TIMEOUT, "sendDuStateComplete");
   }
   else
   {
      cmsTmr_set(tmrHandle, goSendDuStateComplete, (void*)NULL, CHANGEDUSTATE_ERR_TIMEOUT, "sendDuStateComplete");
   }
}  /* doChangeDuState */

static void goSendDuStateComplete(void *handle __attribute__((unused)))
{
   cmsLog_debug("goSendDuStateComplete (entered)");

   cmsTmr_cancel(tmrHandle, goSendDuStateComplete, (void*)NULL);
   addInformEventToList(INFORM_EVENT_DU_CHANGE_COMPLETE);
   addInformEventToList(INFORM_EVENT_CHANGE_DU_CHANGE_METHOD);
   sendInform(NULL);
   doSendDuStateComplete = TRUE;
} /* goSendDuStateComplete */

void goSendAutonomousDuStateComplete(void)
{
   cmsLog_debug("goSendAutonomousDuStateComplete");
   addInformEventToList(INFORM_EVENT_AUTON_DU_CHANGE_COMPLETE);
   sendInform(NULL);
   doSendAutonDuStateComplete = TRUE;
} /* goSendAutonomousDuStateComplete */

void allResponseReceived(void)
{
   cmsLog_debug("All responses for pending request have been received, goSendDuStateComplete");
   goSendDuStateComplete((void*)NULL);
}


void sendDuStateComplete(void)
{
   char        *ck;
   int         loop;
   tProtoCtx   *pc;
//   DUObject *pDuObjs=NULL;
//   DUObject *pDuObj=NULL;
//   SINT32 cnt = 0;
//   SINT32 numEntries = 0;
   UBOOL8      timeout = FALSE;
   ChangeDuStateOpReqInfo *pOpReq = acsState.duStateOpList;
//   int opCount = 0;
//   int found=0;
//   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_notice("sendDuStateComplete");

   if (acsState.pendingDuReq != 0)
   {
      /* this is a timeout.  We need to return error to ACS */
      timeout = TRUE;
   }
   if (timeout)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"XXX todo: this is a timeout.  We need to return error to ACS ");
   }

#if 0 
        /* we should release lock when doRPC is processing this RPC because this could
         take too much time holding up the lock */
   if ((cmsLck_acquireLockWithTimeout(TR69C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock");
      return;
   }


   ret = cmsPhl_getDUObject(&pDuObjs, &numEntries);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getDUObject failed");
      return;
   }

   /* We need to follow the request list's and find the result of each DU request.
    * The order of the result needs to go out as the way the were sent from ACS
    */
   while (pOpReq != NULL)
   {


      opCount++;
      found = 0;

      cnt = 0;
      while (cnt<numEntries)
      {
         pDuObj = &(pDuObjs[cnt]);
         cnt++;

         if (pOpReq->operationType == eDuInstall)
         {
            if (cmsUtl_strcmp(pDuObj->URL,pOpReq->url) == 0)
            {
               found = 1;
               if (strcmp(pDuObj->status,MDMVS_INSTALLED) == 0)
               {
                  pOpReq->currentState = cmsMem_strdup(pDuObj->status);
               }

               break;
            }
         }
         else
         {
            if ((cmsUtl_strcmp(pDuObj->UUID,pOpReq->uuid) == 0) &&
                (cmsUtl_strcmp(pDuObj->version,pOpReq->version) == 0))
            {
               found = 1;

               if ((pOpReq->operationType == eDuUpdate) && (strcmp(pDuObj->status,MDMVS_INSTALLED) == 0))
               {
                  pOpReq->currentState = cmsMem_strdup(pDuObj->status);
               }
               break;
            }
         }
      } /* while all the DU object list */

      if (found)
      {
         cmsLog_debug("-- opCount %d for pOpReq found",opCount);

         if (cmsUtl_strcmp(pOpReq->uuid,pDuObj->UUID))
         {
            cmsLog_error("UUID mismatch! %s %s", pOpReq->uuid, pDuObj->UUID);
         }

         REPLACE_STRING_IF_NOT_EQUAL(pOpReq->version,pDuObj->version);
         pOpReq->resolved = pDuObj->resolved;
         pOpReq->startTime = cmsMem_strdup(pDuObj->X_BROADCOM_COM_startTime);
         pOpReq->completeTime = cmsMem_strdup(pDuObj->X_BROADCOM_COM_completeTime);
         pOpReq->euRef = cmsMem_strdup(pDuObj->executionUnitList);
      }
      else
      {
         if (pOpReq->operationType == eDuInstall)
         {
            /* this means this is a flesh install error */
            cmsLog_debug("--could not find MDM object for install with url %s: install error",pOpReq->url);
         }
         else
         {
            /* this means it's an uninstall */
            cmsLog_debug("--could not find MDM object for opType %d with uuid %s, version %s",
                         pOpReq->operationType, pOpReq->uuid, pOpReq->version);
            pOpReq->currentState = cmsMem_strdup(MDMVS_UNINSTALLED);
         }
      }
      pOpReq = pOpReq->next;
   } /* while opReqList */

   cmsLck_releaseLock();


   cmsPhl_free((void**)&pDuObjs,numEntries);
#endif

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   int   bufsz = 0;
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      pOpReq = acsState.duStateOpList;
      openEnvWithHeader("12345678", pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sDUStateChangeComplete>\n", nsCWMP);
      xml_mIndent(pc, &bufsz, 4);
      ck=acsState.duStateChangeCommandKey;
#ifdef SUPPRESS_EMPTY_PARAM
      if (ck && strlen(ck)>0)
      {
         mprintf(pc, &bufsz, "<CommandKey>%s</CommandKey>\n", ck);
      }
      else
      {
         mprintf(pc, &bufsz, "<CommandKey>empty</CommandKey>\n");
      }
#else
      mprintf(pc, &bufsz, "<CommandKey>%s</CommandKey>\n", ck? ck: "");
#endif

      /* array of opResultStruct */

      while (pOpReq != NULL)
      {
         xml_mIndent(pc, &bufsz, 4);

#if 0
         /* yen : need to define in xmlTables.c, not hardcode */
         /* iTatel ACS doesn't like this */
         mprintf(pc, &bufsz,
                 "<Results xsi:type=\"%sOpResultStruct\">\n",
                 nsCWMP_1_2);
#else
         mprintf(pc, &bufsz,"<Results>\n");

#endif

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<UUID>%s</UUID>\n", pOpReq->uuid);

         xml_mIndent(pc, &bufsz, 5);
         if (pOpReq->duRef == NULL)
         {
            mprintf(pc, &bufsz, "<DeploymentUnitRef>%s</DeploymentUnitRef>\n","");
         }
         else
         {
            mprintf(pc, &bufsz, "<DeploymentUnitRef>%s</DeploymentUnitRef>\n",pOpReq->duRef);
         }

         xml_mIndent(pc, &bufsz, 5);
         if (pOpReq->version == NULL)
         {
            mprintf(pc, &bufsz, "<Version>%s</Version>\n", "");
         }
         else
         {
            mprintf(pc, &bufsz, "<Version>%s</Version>\n", pOpReq->version);
         }

         xml_mIndent(pc, &bufsz, 5);
         if (pOpReq->currentState == NULL)
         {
            mprintf(pc, &bufsz, "<CurrentState>%s</CurrentState>\n","Failed" );
         }
         else
         {
            mprintf(pc, &bufsz, "<CurrentState>%s</CurrentState>\n",pOpReq->currentState );
         }

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<Resolved>%d</Resolved>\n", pOpReq->resolved);

         xml_mIndent(pc, &bufsz, 5);
         if (pOpReq->euRef == NULL)
         {
            mprintf(pc, &bufsz, "<ExecutionUnitRefList>%s</ExecutionUnitRefList>\n", "");
         }
         else
         {
            mprintf(pc, &bufsz, "<ExecutionUnitRefList>%s</ExecutionUnitRefList>\n", pOpReq->euRef);
         }
         xml_mIndent(pc, &bufsz, 5);
         if (pOpReq->startTime == NULL)
         {
            mprintf(pc, &bufsz, "<StartTime>%s</StartTime>\n", UNKNOWN_DATETIME_STRING);
         }
         else
         {
            mprintf(pc, &bufsz, "<StartTime>%s</StartTime>\n", pOpReq->startTime);
         }

         xml_mIndent(pc, &bufsz, 5);
         if (pOpReq->completeTime == NULL)
         {
            mprintf(pc, &bufsz, "<CompleteTime>%s</CompleteTime>\n", UNKNOWN_DATETIME_STRING);
         }
         else
         {
            mprintf(pc, &bufsz, "<CompleteTime>%s</CompleteTime>\n", pOpReq->completeTime);
         }

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<Fault>\n");
         xml_mIndent(pc, &bufsz, 6);
         mprintf(pc, &bufsz, "<FaultCode>%d</FaultCode>\n",pOpReq->fault.faultCode);
         xml_mIndent(pc, &bufsz, 6);

         if (pOpReq->fault.faultCode == CMSRET_SUCCESS)
         {
#ifdef SUPPRESS_EMPTY_PARAM
            mprintf(pc, &bufsz, "<FaultString>%s</FaultString>\n", "empty");
#else
            mprintf(pc, &bufsz, "<FaultString>%s</FaultString>\n", "");
#endif
         }
         else
         {
            mprintf(pc, &bufsz, "<FaultString>%s</FaultString>\n", cmsErr_getString(pOpReq->fault.faultCode));
         }
         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "</Fault>\n");

         xml_mIndent(pc, &bufsz, 4);
         mprintf(pc, &bufsz, "</Results>\n");

         pOpReq = pOpReq->next;
      } /* for each request */

      xml_mIndent(pc, &bufsz, 2);
      mprintf(pc, &bufsz, "</%sDUStateChangeComplete>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC DUStateChangeComplete received from ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

   cleanupAcsDuReqList();

}  /* End of sendDuStateComplete() */

/* this is send of Du State change that is not initiated by ACS */
void sendAutonDuStateComplete()
{
   DUObject *pDuObjs=NULL;
   DUObject *duObject=NULL;
   SINT32 cnt = 0;
   SINT32 numEntries = 0;
   int found = 0;
   int installError = 0;
   MdmPathDescriptor pathDesc;
   int         loop;
   tProtoCtx   *pc;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   ChangeDuStateOpReqInfo *pResult = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if (pAutonResult == NULL)
   {
      return;
   }

   cmsLog_notice("sendAutonDuStateComplete");

   ret = cmsPhl_getDUObject(&pDuObjs, &numEntries);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"cmsPhl_getDUObject failed");
      return;
   }

   pResult = pAutonResult;

   while (pResult != NULL)
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      found = 0;
      /* find the object and parameter of DU to send to ACS */
      cnt = 0;
      while( !found && cnt<numEntries)
      {
         duObject = &(pDuObjs[cnt]);
         cnt++;

         if (pResult->operationType == (eOperationType) OSGID_OPERATION_INSTALL)
         {
            if (pResult->fault.faultCode != 0)
            {
               installError = 1;
               break;
            }
            else if (cmsUtl_strcmp(duObject->URL, pResult->url) == 0)
            {
               found = 1;
               break;
            }
         }
         else
         {
            if (cmsUtl_strcmp(duObject->UUID, pResult->uuid) == 0)
            {
               found = 1;
               break;
            }
         }
      } /* while object */
      if ((found) && (!installError))
      {
         /* copy the details from pDuObject */
         if (cmsUtl_strcmp(pResult->uuid,duObject->UUID))
         {
            cmsLog_error(TR069_ALARM_PROCESS_ERROR,"UUID mismatch! %s %s", pResult->uuid, duObject->UUID);
         }

         memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
         pathDesc.oid = MDMOID_DU;
         pathDesc.iidStack = iidStack;
         cmsMdm_pathDescriptorToFullPath(&pathDesc, &(pResult->duRef));
         REPLACE_STRING_IF_NOT_EQUAL(pResult->version,duObject->version);
         if ((strcmp(duObject->status,MDMVS_UNINSTALLED) == 0) ||
             (strcmp(duObject->status,MDMVS_INSTALLED) == 0))
         {
            REPLACE_STRING_IF_NOT_EQUAL(pResult->currentState,duObject->status);
         }
         else
         {
            REPLACE_STRING_IF_NOT_EQUAL(pResult->currentState,"Failed");
         }
         pAutonResult->resolved = duObject->resolved;
         REPLACE_STRING_IF_NOT_EQUAL(pResult->euRef,duObject->executionUnitList);
         REPLACE_STRING_IF_NOT_EQUAL(pResult->startTime,duObject->X_BROADCOM_COM_startTime);
         REPLACE_STRING_IF_NOT_EQUAL(pResult->completeTime,duObject->X_BROADCOM_COM_completeTime);
      }
      else
      {
         if (pResult->fault.faultCode == CMSRET_SUCCESS)
         {
            REPLACE_STRING_IF_NOT_EQUAL(pResult->currentState,"Uninstalled");
         }
         else
         {
            REPLACE_STRING_IF_NOT_EQUAL(pResult->currentState,"Failed");
         }
         /* this is meaningless, but per specification, it should be set to true */
         pAutonResult->resolved = 1;
         REPLACE_STRING_IF_NOT_EQUAL(pResult->startTime,UNKNOWN_DATETIME_STRING);
         REPLACE_STRING_IF_NOT_EQUAL(pResult->completeTime,UNKNOWN_DATETIME_STRING);
         /* initialize details from received message */
      }
      pResult = pResult->next;
      installError = 0;
   } /* pResult loop */

   cmsPhl_free((void**)&pDuObjs,numEntries);

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   int   bufsz = 0;
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      if (pc != NULL)
      {
         pc->content = cmsMem_alloc(bufsz+1,ALLOC_ZEROIZE);
         pc->length = 0;
         pc->maxlength = bufsz+1;
         cmsLog_debug("start to handle rpc body at %ds", time(NULL));
      }
      else
      {
         cmsLog_debug("start to handle rpc header at %ds", time(NULL));
      }
      
      bufsz = 0;

      openEnvWithHeader("12345678", pc, &bufsz);
      openBody(pc, &bufsz);

      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "<%sAutonomousDUStateChangeComplete>\n", nsCWMP);

      pResult = pAutonResult;
      while (pResult != NULL)
      {
         xml_mIndent(pc, &bufsz, 4);
         mprintf(pc, &bufsz, "<Results>\n");

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<UUID>%s</UUID>\n", pResult->uuid);

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<DeploymentUnitRef>%s</DeploymentUnitRef>\n",pResult->duRef);

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<Version>%s</Version>\n", pResult->version);

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<CurrentState>%s</CurrentState>\n",pResult->currentState);

         xml_mIndent(pc, &bufsz, 5);
         if (pResult->resolved)
         {
            mprintf(pc, &bufsz, "<Resolved>%s</Resolved>\n", "true");
         }
         else
         {
            mprintf(pc, &bufsz, "<Resolved>%s</Resolved>\n", "false");
         }

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<ExecutionUnitRefList>%s</ExecutionUnitRefList>\n", pResult->euRef);

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<StartTime>%s</StartTime>\n", pResult->startTime);

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<CompleteTime>%s</CompleteTime>\n", pResult->completeTime);

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "<Fault>\n");

         xml_mIndent(pc, &bufsz, 6);
         mprintf(pc, &bufsz, "<FaultCode>%d</FaultCode>\n",pResult->fault.faultCode);
         xml_mIndent(pc, &bufsz, 6);
         if (pResult->fault.faultCode == CMSRET_SUCCESS)
         {
#ifdef SUPPRESS_EMPTY_PARAM
            mprintf(pc, &bufsz, "<FaultString>%s</FaultString>\n", "empty");
#else
            mprintf(pc, &bufsz, "<FaultString>%s</FaultString>\n", "");
#endif
         }
         else
         {
            mprintf(pc, &bufsz, "<FaultString>%s</FaultString>\n", cmsErr_getString(pResult->fault.faultCode));
         }

         xml_mIndent(pc, &bufsz, 5);
         mprintf(pc, &bufsz, "</Fault>\n");

         xml_mIndent(pc, &bufsz, 5);
         if (pResult->operationType == (eOperationType) OSGID_OPERATION_INSTALL)
         {
            mprintf(pc, &bufsz, "<OperationPerformed>%s</OperationPerformed>\n",SW_MODULES_OPERATION_INSTALL);
         }
         else if (pResult->operationType == (eOperationType)OSGID_OPERATION_UNINSTALL)
         {
            mprintf(pc, &bufsz, "<OperationPerformed>%s</OperationPerformed>\n",SW_MODULES_OPERATION_UNINSTALL);
         }
         else
         {
            mprintf(pc, &bufsz, "<OperationPerformed>%s</OperationPerformed>\n",SW_MODULES_OPERATION_UPDATE);
         }

         xml_mIndent(pc, &bufsz, 4);
         mprintf(pc, &bufsz, "</Results>\n");

         pResult = pResult->next;
      } /* for each pAutonResult */

      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, "</%sAutonomousDUStateChangeComplete>\n", nsCWMP);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
      else
      {
         cmsLog_debug("start to send rpc body at %d", time(NULL));
         proto_SendRaw(pc, pc->content, pc->length);
         cmsLog_debug("stop to send rpc body at %d", time(NULL));

         cmsLog_log("RPC AutonomousDUStateChangeComplete received from ACS");
      }
   }  /* for (loop = 0; loop < 2; loop++) */

   if (pc && pc->content)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(pc->content);
      pc->length = 0;
      pc->maxlength = 0;
   }

   /* XXX miwang: is this also the same as freeOpReqList */
   while (pAutonResult != NULL)
   {
      pResult = pAutonResult;
      pAutonResult = pAutonResult->next;
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResult->url);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResult->version);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResult->user);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResult->pwd);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResult->startTime);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResult->completeTime);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResult->duRef);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResult->euRef);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResult->currentState);
      CMSMEM_FREE_BUF_AND_NULL_PTR(pResult);
   }
}  /* sendAutonDuStateComplete() */


#ifdef SUPPORT_TR69C_VENDOR_RPC
void registerVendorRPC(char *rpcNameStr, XmlNodeDesc *rpcNodeDesc, 
                       DO_VENDOR_RPC_FUNC doRpcAction)
{
   VendorSpecificRPCItem *pRpcItem;

   cmsLog_debug("Enter");

   if ((rpcNameStr == NULL) || (rpcNodeDesc == NULL) || (doRpcAction == NULL))
   {
      cmsLog_debug("Invalid parameters-- RPC name string, XmlNodeDesc, VendorRPC handler and paramter must not be NULL");
      return;
   }
   if (cmsUtl_strncmp(rpcNameStr, VENDOR_RPC_PREFIX, strlen(VENDOR_RPC_PREFIX)) != 0)
   {
      cmsLog_debug("Invalid RPC name string %s",rpcNameStr);
   }
   pRpcItem = cmsMem_alloc(sizeof(VendorSpecificRPCItem),ALLOC_ZEROIZE);      
   if (pRpcItem == NULL)
   {
      cmsLog_debug("Error allocating vendor RPC item for registration");
      return;
   }
   pRpcItem->rpcName = cmsMem_strdup(rpcNameStr);
   pRpcItem->rpcNodeDesc = rpcNodeDesc;
   pRpcItem->doRpcFunc = doRpcAction;
   if (vendorSpecificRPCList == NULL)
   {
      vendorSpecificRPCList = pRpcItem;
   }
   else
   {
      pRpcItem->next = vendorSpecificRPCList;
      vendorSpecificRPCList = pRpcItem;
   }
} /* registerVendorRPC */

static void callDoRpcByName(char *rpcName)
{
   VendorSpecificRPCItem *pRpcItem = vendorSpecificRPCList;

   cmsLog_debug("Enter: rpcName %s",rpcName);

   while (pRpcItem)
   {
      if (cmsUtl_strcmp(pRpcItem->rpcName,rpcName) == 0)
      {
         (pRpcItem->doRpcFunc)(pVendorRpcAction);
         break;
      }
      pRpcItem=pRpcItem->next;
   }
   return;
}

void sendVendorResponseToAcs(char *response)
{
   int         loop;
   tProtoCtx   *pc;
   RPCAction *a = rpcAction;

   cmsLog_debug("Enter: response %s");

   /* Loop through the code twice.  The first loop is to calculate
    * the size of the entire soap envelope so that the HTTP header
    * can be written to the TCP socket first.  The second loop is
    * to write the content of the envelope to the TCP socket as it
    * is generated.
    */
   for (loop = 0, pc = NULL; loop < 2; loop++, pc = getAcsConnDesc())
   {
      int   bufsz = 0;
   
      openEnvWithHeader(a->ID, pc, &bufsz);
      openBody(pc, &bufsz);
      xml_mIndent(pc, &bufsz, 3);
      mprintf(pc, &bufsz, response);
      closeBodyEnvelope(pc, &bufsz);

      if (loop == 0)
      {
         /* send the HTTP message header */
         sendToAcs(bufsz, NULL);
      }
   }  /* for (loop = 0; loop < 2; loop++) */
}  /* End of sendVendorReponseToAcs() */

#endif /* SUPPORT_TR69C_VENDOR_RPC */
