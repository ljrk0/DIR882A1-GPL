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
 * File Name  : informer.c
 *
 * Description: Inform logic for tr69
 * $Revision: 1.51 $
 * $Id: informer.c,v 1.51 2006/02/17 21:22:41 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/if.h>
#include <syslog.h>
#include <fcntl.h>
#include <math.h>

#include "cms_util.h"
#include "cms_msg.h"
#include "../inc/tr69cdefs.h"
#include "../inc/appdefs.h"
#include "../inc/utils.h"
#include "../SOAPParser/RPCState.h"
#include "../SOAPParser/xmlParserSM.h"
#include "../SOAPParser/xmlTables.h"
#include "event.h"

#include "../webproto/protocol.h"
#include "../webproto/www.h"
#include "../webproto/wget.h"
#include "../main/informer.h"
#include "../main/httpProto.h"
#include "../main/httpDownload.h"
#include "acsListener.h"
#include "../bcmLibIF/bcmWrapper.h"
#include "../inc/tr69cdefs.h" /* defines for ACS state */
#include "../bcmLibIF/bcmConfig.h"

/** local definitions **/

/** external functions **/
extern void main_cleanup(SINT32 code);
extern void handleConnectionRequest(void);
extern void sendAutonTransferComplete(void);

/** external data **/
extern ACSState      acsState;
extern int transferCompletePending;
extern int saveNotificationsFlag;
extern int sendGETRPC;              /* send a GetRPCMethods */
extern int tr69cTerm;               /* TR69C termination flag */
extern void *tmrHandle;

extern void *msgHandle;
extern LimitNotificationQInfo limitNotificationList;
extern UBOOL8 connReqInSession;

extern UBOOL8 SmdStarted;

/** public data **/
eInformState   informState = eACSNeverContacted;
eSessionState  sessionState = eSessionEnd;
InformEvList   informEvList;

/** private data **/
static CookieHdr *glbCookieHdr = NULL;
static HttpTask      httpTask;       /* http io desc */
static SessionAuth   sessionAuth;
static int	sentACSNull;   /* set if last msg to ACS was NULL, cleared if non-null */
                           /* response received from ACS */
static UBOOL8 rebootingFlag = FALSE; /*set if we are doing system reboot or factoryreset*/
static char *prevSoapMsg = NULL;

/** private functions **/
static int  getInformEvCnt(void);
static void closeACSConnection(HttpTask *ht);
static void acsDisconnect(HttpTask *ht, AcsStatus acsStatus);
static void updateAuthorizationHdr(HttpTask *ht);
static void nullHttpTimeout(void *handle);
static void postComplete(void *handle);
static void Connected(void *handle);
static void sendInformData(void);
static void startACSComm(void *handle);
static int  getRandomNumber(int from, int to);
static int  getDelayTime(int acsConnectFails);

int getRandomNumber(int from, int to)
{
   int num = 0;
   
   srand((unsigned int)time((time_t *)NULL));
   num = (rand() % (to - from)) + from;
   
   return num;
}

int getDelayTime(int acsConnectFails)
{
   int delayTime = 0;
   int minWaitInterval = 5;
   int intervalMultiplier = 2000;
   int min=0, max=0;

   getSessionRetryParameters(&minWaitInterval,&intervalMultiplier);

   // the following implementation is based on section
   // 3.2.1.1 Session Retry Policy in "TR-069 Amendment 4"
   // Table 3 - Session Retry Wait Intervals
   if (acsConnectFails > 10)
   {
      acsConnectFails = 10;
   }

   min = minWaitInterval * (pow(((float)intervalMultiplier/1000.0),(acsConnectFails-1)));
   max = minWaitInterval * (pow(((float)intervalMultiplier/1000.0),acsConnectFails));
   delayTime = getRandomNumber(min, max);
   return delayTime;
}

void retrySessionConnection(void)
{
   HttpTask *ht = &httpTask;
   UINT32 retryCount = acsState.retryCount;

   if (ht->wio != NULL)
   {
      wget_Disconnect(ht->wio);
      ht->wio = NULL;
      ht->xfrStatus = ePostError;
      ht->eHttpState = eClosed;
      ht->eAuthState = sIdle;
   }

   if (acsState.retryCount > 0)
   {      
      // the following implementation is based on section
      // 3.2.1.1 Session Retry Policy in "TR-069 Amendment 1"
      // Table 3 - Session Retry Wait Intervals
      // delay time for acsConnectFails that is greater than 10
      // is the same with acsConnectFails that is equal to 10
      if (acsState.retryCount > 10)
         retryCount = 10;
      /* retry ACS connect errors with increasing retry time interval */
      UINT32 backOffTime = getDelayTime(retryCount) * 1000;

      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"ACS connect failed, retryCount = %d, backOffTime = %dms", retryCount, backOffTime);
      cmsTmr_replaceIfSooner(tmrHandle, sendInform, NULL, backOffTime, "conn_fail_backoff_inform");
   }
   else
   {
         cmsTmr_replaceIfSooner(tmrHandle, sendInform, NULL, 0, "conn_backoff_retryReset");
         cmsLog_debug("ACS connect retry, no more backoff, retryCount = %d", retryCount);
   }
}

UBOOL8 isAcsConnected(void)
{
	return ((httpTask.wio != NULL) && (httpTask.wio->pc != NULL));
}

tProtoCtx *getAcsConnDesc(void)
{
   if (!isAcsConnected())
   {
      return NULL;
   }
#ifdef USE_SSL
   if (httpTask.wio->pc->type == iSsl && httpTask.wio->pc->ssl != NULL)
   {
      if (httpTask.wio->pc->sslConn > 0)
      {
         return (httpTask.wio->pc);
      }
      else
      {
         return NULL;
      }
   }
   else
   {
      return (httpTask.wio->pc);
   }
#else
   return (httpTask.wio->pc);
#endif
}

void clearInformEventList(void)
{
   int count, i;

   cmsLog_debug("cleared informEvList");
   
   count = informEvList.informEvCnt;
   for (i=0; i< count; i++)
   {
      informEvList.informEvList[i] = 0;
   }
   informEvList.informEvCnt = 0;
}

int getInformEvCnt(void)
{
   return informEvList.informEvCnt;
}

static void closeACSConnection(HttpTask *ht)
{
   cmsLog_debug("=====>ENTER");
   if (ht->wio)
   {
      wget_Disconnect(ht->wio);
      ht->wio = NULL;
   }

#ifdef later
   /*
    * mwang: free these pointers to make resource checker happy.
    * I decided not to do this yet because freeing sessionAuth here may
    * have some unintended consequences.
    */
   resetSessionAuth(&sessionAuth);

   /*
    * Freeing the rpcAction should be OK, since we are done with this transaction,
    * but leave out for now also.
    */
   extern RPCAction *rpcAction;  // move this line outside of function

   freeRPCAction(rpcAction);
   rpcAction = NULL;
#endif

}

void stopAcsConnection(void)
{
    acsDisconnect(&httpTask, eAcsDone);
}

static void acsDisconnect(HttpTask *ht, AcsStatus acsStatus)
{
   static AcsStatus acsStatusPrev;

   cmsLog_debug("=====>ENTER");
   cmsTmr_cancel(tmrHandle, nullHttpTimeout, (void *)ht);
   CMSMEM_FREE_BUF_AND_NULL_PTR(ht->authHdr);
   
   closeACSConnection(ht);
   ht->xfrStatus = acsStatus;
   ht->eHttpState = eClosed;
   ht->eAuthState = sIdle;

   sessionState = eSessionEnd;
   releaseEmeSessionIfAccquired();

   switch (acsStatus)
   {
      case eAuthError:
         ++acsState.retryCount;
         if (acsState.informEnable)
         {
            cancelPeriodicInform();
         }
         cmsLog_notice("Failed authentication with ACS");
         cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"ACS Disconnect with error %d (0:eOK, 1:eConnectError, 2:eGetError, 3:ePostError, 4:eAuthError)", acsStatus);

         saveTR69StatusItems();   // save retryCount to scratchpad
         break;
      case eConnectError:
      case eGetError:
      case ePostError:
         ++acsState.retryCount;
         if (acsState.informEnable)
         {
            cancelPeriodicInform();
         }
         cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"ACS Disconnect with error %d (0:eOK, 1:eConnectError, 2:eGetError, 3:ePostError, 4:eAuthError)", acsStatus);

#ifdef ALLOW_DISCONNECT_ERROR
         cmsLog_notice("Continue processing even though ACS Disconnect with error");
         saveTR69StatusItems();
         saveConfigurations();
         rebootingFlag = factoryResetCompletion(); // this will cause a reboot if factoryResetFlag==1
         if(!rebootingFlag)
         {
            rebootingFlag = rebootCompletion();
         }
#endif

         /* TR69a4, section 3.2.1.1: cwmp fall back mechanism */
         /* if CPE fails to establish a session, it might be because CPE supports newer version which
          * ACS supports like v1.0.  CPE MUST revert to v1.0.  But it is hard to predict what the ACS
          * will do, to be absolutely sure that it is cwmp IOP issues, I will wait for at least 3 attempts
          * before falling back. 
          */
         if ((acsStatus == ePostError) && (acsState.cwmpVersion == CWMP_VERSION_1_2) && 
             (acsState.retryCount > 2))
         {
            changeNameSpaceCwmpVersionURL(CWMP_VERSION_1_0);
         }

         break;
      case eAcsDone:
      default:
         // should NOT clear acsState.retryCount here, only clear
         // it after receiving InformResponse to make this value 
         // can be shown correctly in Inform.
         if (acsStatusPrev != eAcsDone)
         {
            resetPeriodicInform(acsState.informInterval);
         }
         cmsLog_notice("ACS Disconnect: ok");
         /* if no error then run thru pending disconnect actions */
         saveTR69StatusItems();
         saveConfigurations();
         rebootingFlag = factoryResetCompletion(); // this will cause a reboot if factoryResetFlag==1
         if(!rebootingFlag)
         {
	        rebootingFlag = rebootCompletion();
         }
         break;
   }
   
   if (tr69cTerm)
   {
      cmsLog_notice("TR69C terminated due to tr69cTerm flag");
      tr69cTerm = 0;         /* Clear tr69c termination flag */

      /* this function calls all cleanup functions and exits with specified code */
      main_cleanup(0);
   }

   /*
    * instead of checking all these flags, just find out if there is event in
    * informEventList 
    */
   if ((getInformEvCnt() != 0)
       || handleTransferCompletePending(FALSE)
       || sendGETRPC
       || cmsPhl_CheckIfAnyParamValueChangePending()
       || (acsStatus==eAuthError)
       || (acsStatus==eConnectError)
       || (acsStatus==eGetError)
       || (acsStatus==ePostError))   // session retry policy
   {
      retrySessionConnection();
   }
   acsStatusPrev = acsStatus;
}  /* End of acsDisconnect() */

static void updateAuthorizationHdr(HttpTask *ht)
{
   cmsLog_debug("=====>ENTER");
   if (ht->authHdr)
   {
      if ( sessionAuth.qopType==eAuth )
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(ht->authHdr);
         ht->authHdr = generateNextAuthorizationHdrValue(&sessionAuth,
                                          acsState.acsUser, acsState.acsPwd);
         wget_AddPostHdr(ht->wio, "Authorization", ht->authHdr); /* replace header */
      }
      else
      {
         /* must be basic or not "Auth" */
         CMSMEM_FREE_BUF_AND_NULL_PTR(ht->authHdr);
         ht->authHdr = generateBasicAuthorizationHdrValue(&sessionAuth,
                                          acsState.acsUser, acsState.acsPwd);
         wget_AddPostHdr(ht->wio, "Authorization", ht->authHdr); /* replace header */
      }
   }
}  /* End of updateAuthorizationHdr() */

static void nullHttpTimeout(void *handle)
{
   HttpTask    *ht = (HttpTask*)handle;

   cmsLog_debug("=====>ENTER");
   acsDisconnect(ht, eAcsDone);
}

void sendNullHttp(UBOOL8 disconnect)
{
   HttpTask *ht = &httpTask;
   CmsRet ret;

   if (ht->wio)
   {
      cmsLog_notice("sendNullHttp(%s) to ACS",ht->eHttpState==eClose? "close": "keepOpen");
   }
   if (ht->postMsg)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
   }
   sendToAcs(0, NULL);

   // send empty POST only count when holdrequests is false
   if (acsState.holdRequests == FALSE)
   {
      sentACSNull = 1;
   }
   if (disconnect == TRUE)
   {
      ret = cmsTmr_set(tmrHandle, nullHttpTimeout, (void *)ht, ACSRESPONSETIME, "null_http"); /* 30s */
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error(TR069_ALARM_TIMER_ERROR,"could not set NULL httpd timer, ret=%d", ret);
      }
   }
}

#ifdef SUPPORT_ACS_CISCO
void informRspTimeout(void *handle)
{
   informState = eACSInformed;
   setInformState(eACSInformed);
   if (transferCompletePending)
   {
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
   }
   else
   {
      sendNullHttp(TRUE);
   }

   /* need this to get CISCO notifications to work.Since we never get a */
   /* InformResponse for the Cisco tool */
   resetNotification();
}
#endif

static void freeMsg(char *buf, int *bufSz) {
   if (buf != NULL)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
   }
   *bufSz = 0;
}

static char *readLengthMsg(tWget *wg, int readLth, int *mlth, int doFlushStream) {
   int bufCnt = 0, readCnt = 0;
   int bufLth = readLth;
   char *soapBuf = NULL;

   *mlth = 0;
   /*
    * This is the path taken when we do image download.  Don't zeroize
    * the buffer that is allocated here because that will force linux
    * to immediately assign physical pages to the buffer.  Intead, just
    * let the buffer fill in as the transfer progresses.  This will give
    * smd and the kernel more time to make physical pages available.
    */
   if ((soapBuf = (char *) cmsMem_alloc(readLth + 1, 0)) != NULL)
   {
      while (bufCnt < readLth)
      {
         // since proto_Readn cannot display SOAP message to console for debug,
         // proto_Readn is replaced with proto_ReadWait that has modifications to
         // call proto_Readn, and also display SOAP message to console.
         if ((readCnt = proto_ReadWait(wg->pc, soapBuf+bufCnt, bufLth)) > 0)
         //if ((readCnt = proto_Readn(wg->pc, soapBuf+bufCnt, bufLth)) > 0)
         {
            bufCnt += readCnt;
            bufLth -= readCnt;
         }
         else
         {
            if (readCnt == -99) {
            /* read error */
            *mlth = 0;
	    
            CMSMEM_FREE_BUF_AND_NULL_PTR(soapBuf);
	    soapBuf = NULL;
		      cmsLog_error(TR069_ALARM_SOCKET_ERROR,"download interrupted");

		break;
            }
         }
      }
      cmsLog_debug("soapBuf bufCnt=%d readLth=%d\n", bufCnt, readLth);
      if(readCnt != -99)
      {
         *mlth = bufCnt;
         soapBuf[bufCnt] = '\0';
      }
      if (doFlushStream)
      {
         // If we are not processing a chunked message, 
         // skip(flush) anything else
         proto_Skip(wg->pc);         
      }
   }
      
   return soapBuf;
}

static char *readChunkedImage(tWget *wg, int *mlth)
{
   int imageBufSize;
   int allocSize;
   char *imageBuf = NULL;
   char tmpBuf[128];
   int chunkSize;
   int total=0;
   int bytesRead;
   
   *mlth = 0;

   /* copying this logic from tftp.c */
   imageBufSize = get_max_image_buf_size();
   if ((imageBuf = (char *) cmsMem_alloc(imageBufSize, 0)) == NULL)
   {
      /* In some case the flash size is very big, imageBufSize could be a very big value.
       * We are making an assumption that the SW image is no bigger than CMS_IMAGE_MAX_ALLOC_LEN.
       * If a bigger image size is needed, then increase CMS_IMAGE_MAX_ALLOC_LEN.
       */
      if (imageBufSize > CMS_IMAGE_MAX_ALLOC_LEN)
      {
         allocSize = CMS_IMAGE_MAX_ALLOC_LEN;
      }
      else if (imageBufSize > CMS_IMAGE_REQUIRED_LEN)
      {
         allocSize = imageBufSize - (64*1024);
      }
      else
      {
         cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"Failed to allocate %d bytes for chunked image download", imageBufSize);
         /* not trying because the requested size is already minimum required length*/
         return NULL;
      }
      while ((imageBuf = (char *) cmsMem_alloc(allocSize, 0)) == NULL)
      {
         if (allocSize > CMS_IMAGE_REQUIRED_LEN)
         {
            allocSize -= 64 * 1024;
         }
         else
         {
            break;
         }
      }
      if (imageBuf == NULL)
      {
         cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"Failed to allocate %d bytes for chunked image download", \
                      allocSize);
         return NULL;
      }
      else
      {
         imageBufSize = allocSize;
      }
   } 
   // keep reading the chunk header for length info
   do {
      bytesRead = proto_Readline(wg->pc, tmpBuf, sizeof(tmpBuf));
      cmsLog_debug("chunk header bytes=%d", bytesRead);

      sscanf(tmpBuf, "%x", &chunkSize);
      cmsLog_debug("chunksize=0x%x (%d)", chunkSize, chunkSize);

      if (total+chunkSize > imageBufSize)
      {
         cmsLog_error(TR069_ALARM_SOCKET_ERROR,"next chunk would exceed imageBufSize, %d+%d>%d",
                      total, chunkSize, imageBufSize);
         total = 0;
         break;
      }

      if (0 == chunkSize)
      {
         cmsLog_debug("detected end chunk!");
      }
      else
      {
         // read the entire chunk of data
         bytesRead = proto_Readn(wg->pc, imageBuf+total, chunkSize);
         if (chunkSize != bytesRead)
         {
            cmsLog_error(TR069_ALARM_SOCKET_ERROR,"===>length mismatch, wanted chunkSize=%d, bytesRead=%d",
                         chunkSize, bytesRead);
            total = 0;
            break;
         }

         total += bytesRead;
      }

      // after reading a chunk, there will be a CRLF (0x0d 0x0a), swallow it
      bytesRead = proto_Readline(wg->pc, tmpBuf, sizeof(tmpBuf));
   } while (chunkSize > 0);

   if (total > 0)
   {
      // OK, properly formatted transfer, return the amount of data read.
      *mlth = total;
      cmsLog_debug("success! total data=%d", total);
   }
   else
   {
      // something wrong, suck in and discard all data
      proto_Skip(wg->pc);
      CMSMEM_FREE_BUF_AND_NULL_PTR(imageBuf);
      imageBuf = NULL;
   }

   return imageBuf;
}

static char *readChunkedMsg(tWget *wg, int *mlth, int maxSize) {
   char *soapBuf = NULL;
   char chunkedBuf[128];   
   
   *mlth = 0;
   // read chunked size of first chunk*/
   if (proto_Readline(wg->pc, chunkedBuf, sizeof(chunkedBuf)) > 0)
   {
      int  chunkedSz = 0, readSz = 0;
      char *newBuf = NULL, *readBuf = NULL;
      
      sscanf(chunkedBuf, "%x", &chunkedSz);
      while (chunkedSz > 0)
      {
         // read chunked data
         int doFlushStream=FALSE;
         readBuf = readLengthMsg(wg, chunkedSz, &readSz, doFlushStream);
         if (chunkedSz != readSz)
            cmsLog_error(TR069_ALARM_SOCKET_ERROR,"===> readChunkedMsg, chunked size = %d, read size = %d\n", chunkedSz, readSz);
         if (readBuf == NULL)
         {
            freeMsg(soapBuf, mlth);
            break;
         }
         if ((*mlth + readSz) > maxSize)
         {
            cmsLog_error(TR069_ALARM_SOCKET_ERROR,"reading more data than maxSize (%d)", maxSize);
            freeMsg(soapBuf, mlth);
            freeMsg(readBuf, &readSz);
            break;
         }
         if (soapBuf == NULL)
         {
            // allocate the first chunk since cmsMem_realloc
            // does not accept soapBuf as NULL pointer.
            newBuf = soapBuf = cmsMem_alloc(*mlth + readSz, ALLOC_ZEROIZE);
         }
         else 
         {
            // reallocate soap message size
            newBuf = cmsMem_realloc(soapBuf, *mlth + readSz);
         }
         
         if (newBuf == NULL)
         {
            freeMsg(soapBuf, mlth);
            freeMsg(readBuf, &readSz);
            break;
         }
         // point soap message to new allocated memory
         soapBuf = newBuf;
         // append chunked data to soap message 
         strncpy(soapBuf + *mlth, readBuf, readSz);
         // increase soap message size
         *mlth += readSz;
         // free chunked data
         freeMsg(readBuf, &readSz);
         chunkedSz = 0;
         // flush off trailing crlf
         do
         {
            chunkedBuf[0] = '\0';
            readSz = proto_Readline(wg->pc, chunkedBuf, sizeof(chunkedBuf));
         } while (readSz > 0 && isxdigit(chunkedBuf[0]) == 0);
         // read chunked size of next chunk*/
         if (isxdigit(chunkedBuf[0]) != 0)
         {
            sscanf(chunkedBuf, "%x", &chunkedSz);
         }
         else
         {
            freeMsg(soapBuf, mlth);
         }
      }      
      // skip(flush) anything else
      proto_Skip(wg->pc);         
   }

   return soapBuf;
}

/*
* tWget *wg is an connected web descriptor,
 *      *mlth is pointer to location of result read length,
 *      maxBufferSize is maximum size to read if non-zero. No limit if maxSize is 0.
 * Returns:
 *     pointer to response buffer or NULL.
 *      *mlth contain size of buffer. Undefined if return is NULL.
 */     
char *readResponse( tWget *wg, int *mlth, int maxBufferSize)
{
   char *soapBuf = NULL;

   if (wg->hdrs->content_length > 0)
   {
      int doFlushStream = TRUE;
      /* this is the path taken by traditional one-shot image downloads */
      cmsLog_debug("calling readLengthMsg for Download with content_length=%d", wg->hdrs->content_length);
      soapBuf = readLengthMsg(wg, wg->hdrs->content_length, mlth, doFlushStream);
   }
   else if (wg->hdrs->TransferEncoding && !strcasecmp(wg->hdrs->TransferEncoding,"chunked"))
   {
      /* content length is 0 */
      if (wg->hdrs->TransferEncoding && !strcasecmp(wg->hdrs->TransferEncoding,"chunked"))
      {
         if (wg->hdrs->content_type &&
             !strncasecmp(wg->hdrs->content_type, "application/octet-stream", sizeof("application/octet-stream") - 1))
         {
            /* this is the path taken by HTTP/1.1 chunked image downloads */
            cmsLog_debug("starting chunked image download!!");
            soapBuf = readChunkedImage(wg, mlth);
         }
         else
         {
            /* this is a standard soap message, content type prob text/xml */
            int maxSize = (maxBufferSize > 0) ? maxBufferSize : MAXWEBBUFSZ;
            cmsLog_debug("calling readChunkedMsg with maxSize=%d", maxSize);
            soapBuf = readChunkedMsg(wg, mlth, maxSize);
         }
      }
      else
      {
         cmsLog_error(TR069_ALARM_RPC_ERROR,"Don't know what kind of response this is!?!");
      }
   }

   return soapBuf;
}  /* End of readResponse() */

static void addCookie(CookieHdr *hdr, CookieHdr *cookie)
{
   CookieHdr *curr = hdr, *prev = hdr;
   // does cookie already exist in cookie list
   for (curr = hdr, prev = hdr;
        curr != NULL;
        prev = curr, curr = curr->next)
   {
      if (strcmp(curr->name, cookie->name) == 0 &&
          strcmp(curr->value, cookie->value) == 0)
      {
         break;
      }
   }
   // if cookie is not in cookie list then add it
   if (curr == NULL)
   {
      curr = (CookieHdr*) cmsMem_alloc(sizeof (CookieHdr), ALLOC_ZEROIZE);
      curr->name = cmsMem_strdup(cookie->name);
      curr->value = cmsMem_strdup(cookie->value);
      curr->next = NULL;
      prev->next = curr;
   }
}

static void copyCookies(CookieHdr **dst, CookieHdr *src)
{
   CookieHdr *cp = src;
   
   // create cookie head
   if (*dst == NULL && cp != NULL)
   {
      *dst = (CookieHdr*) cmsMem_alloc(sizeof (CookieHdr), ALLOC_ZEROIZE);
      (*dst)->name = cmsMem_strdup(cp->name);
      (*dst)->value = cmsMem_strdup(cp->value);
      (*dst)->next = NULL;
      cp = cp->next;
   }
   // add cookie to cookie head
   while (cp != NULL)
   {
      addCookie(*dst, cp);   
      cp = cp->next;
   }
}  /* End of copyCookies() */

static void freeCookies(CookieHdr **p)
{
   CookieHdr *next = *p;
   
   while (next)
   {
      CookieHdr *temp;
      temp = next->next;
      if (next->name) 
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(next->name);
      }
      if (next->value)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(next->value);
      }
      CMSMEM_FREE_BUF_AND_NULL_PTR(next);
      next = temp;
   }
}  /* End of freeCookies() */

static void handleSoapMessage(char *soapmsg, int len)
{
   if ( soapmsg )
   {
      eParseStatus   status;
      ParseHow       parseReq;
      #ifdef DUMPSOAPIN
      fprintf(stderr, "SoapInMsg=>>>>>\n%s\n<\n", soapmsg);
      #endif
      parseReq.topLevel = envelopeDesc;
      parseReq.nameSpace = nameSpaces;
      status = parseGeneric(NULL, soapmsg, len, &parseReq);
      if (status == NO_ERROR)
      {
         if (runRPC() == eRPCRunFail)
         {
            /* couldn't run the RPC: so just disconnect */
            acsDisconnect(&httpTask, eAcsDone);
         }
      }
      else
      {
         cmsLog_debug("ACS Msg. Parse Error %80s", soapmsg);
         acsDisconnect(&httpTask, eAcsDone);  /* force disconnect on parse error*/
      }
      // save the current soapmsg since it might be used later for
      // "Redirection at any Point in a Session" requirement
      // prevSoapMsg should be valid only in current session so
      // it should be free when session is started (by sendInformData)
      // and finished (by receiving 204)
      REPLACE_STRING_IF_NOT_EQUAL_FLAGS(prevSoapMsg, soapmsg, ALLOC_ZEROIZE);
      CMSMEM_FREE_BUF_AND_NULL_PTR(soapmsg);
   }
   else
   {                
      /* no response */
      cmsLog_debug("status = 200, no Soapmsg. sentACSNull=%d", sentACSNull);
      // free previous soapmsg that is used for
      // "Redirection at any Point in a Session" requirement
      CMSMEM_FREE_BUF_AND_NULL_PTR(prevSoapMsg);      
      if (!sentACSNull) 
      {
         sendNullHttp(FALSE);
      }
      acsDisconnect(&httpTask, eAcsDone);
   }
}

static void handleSoapMessageCallBack(void *handle)
{
   HttpTask *ht = &httpTask;
   tWget *wg = (tWget *)handle;
   char  *soapmsg = (char *)wg->handle;

   ht->eHttpState = eConnected;
   // copy cookies from previous connection to the new one
   if (glbCookieHdr != NULL)
   {
      copyCookies(&(ht->wio->cookieHdrs), glbCookieHdr);
   }

   handleSoapMessage(soapmsg, strlen(soapmsg));   
}

static void handleRedirectionCallBack(void *handle __attribute__((unused)))
{
   HttpTask *ht = &httpTask;

   ht->eHttpState = eConnected;
   // copy cookies from previous connection to the new one
   if (glbCookieHdr != NULL)
   {
      copyCookies(&(ht->wio->cookieHdrs), glbCookieHdr);
   }

   // do not redirect soap message if it is InformResponse
   if (prevSoapMsg != NULL && strstr(prevSoapMsg, "InformResponse") == NULL)
   {
      int len = strlen(prevSoapMsg);
      char *soapMsg = cmsMem_strndupFlags(prevSoapMsg, len, ALLOC_ZEROIZE);
      // soapMsg should be free in handleSoapMessage
      handleSoapMessage(soapMsg, len);
   }
   else
   {
      sendInformData();
   }      
}

/*
 * Data has been posted to the server or an
 * error has ocurred.
 */
static void postComplete(void *handle)
{
   tWget       *wg = (tWget *)handle;
   HttpTask    *ht = &httpTask;
   SessionAuth *sa = &sessionAuth;
   int         skipResult=1;

   cmsLog_debug("=====>ENTER");
   cmsTmr_cancel(tmrHandle, nullHttpTimeout, (void *)ht);

   if (wg->status==iWgetStatus_Ok)
   {
      if (ht->wio->hdrs->Connection &&
          !strcasecmp(ht->wio->hdrs->Connection,"close"))
      {
         ht->eHttpState = eClose;
      }
      cmsLog_debug("Connection = %s", ht->eHttpState==eClose?"close": "keep-alive");
      if (wg->hdrs->status_code== 401)
      {
         /* need to send authenticate */
         char *hdrvalue;
         if (wg->hdrs->content_length > 0 ||
             (wg->hdrs->TransferEncoding && streq(wg->hdrs->TransferEncoding,"chunked")))
         {
            int   mlth;
            char *tmpBuf;
            if ((tmpBuf=readResponse(wg,&mlth,0)))
            {
               CMSMEM_FREE_BUF_AND_NULL_PTR(tmpBuf);
            }
         }
         sentACSNull = 0;
         CMSMEM_FREE_BUF_AND_NULL_PTR(ht->authHdr); /* free in case of reauth requested during connection */
         if (ht->eAuthState == sAuthenticating)
         {
            ht->eAuthState = sAuthFailed;
            CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
            ht->postLth = 0;
            /* disconnect and delay */
            acsDisconnect(ht, eAuthError);
            return;
         }
         else
         {
            ht->eAuthState = sAuthenticating;
         }

         cmsLog_debug("WWW-Authenticate= %s", wg->hdrs->wwwAuthenticate);
         if (!(hdrvalue = generateAuthorizationHdrValue(sa, wg->hdrs->wwwAuthenticate,
                                                        "POST", ht->wio->uri,
                                                        acsState.acsUser,
                                                        acsState.acsPwd)))
         {
            cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"WWWAuthenticate header parsing error: %s",
                           wg->hdrs->wwwAuthenticate);
            CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
            acsDisconnect(ht, eAuthError);
            return;
         }
         ht->authHdr = hdrvalue;
         if (skipResult == 0 || ht->eHttpState == eClose)
         {  /* end of data on 401 skip_Proto() */
            // save cookies of the current connection
            if (wg->hdrs->setCookies != NULL)
            {
               copyCookies(&glbCookieHdr, wg->hdrs->setCookies);
            }
            /* close connection and reconnect with Authorization header */
            closeACSConnection(ht);
            ht->wio = wget_Connect(acsState.acsURL, Connected, NULL);

            if (ht->wio == NULL)
            {
               cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"ACS Connect failed: %s", wget_LastErrorMsg());
               CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
               CMSMEM_FREE_BUF_AND_NULL_PTR(ht->authHdr);
               return;
            }
            ht->eHttpState = eConnected;
            // copy cookies from previous connection to the new one
            if (glbCookieHdr != NULL) {
               copyCookies(&(ht->wio->cookieHdrs), glbCookieHdr);
            }
            // return here since sendInformData will be called in Connected
            return;
         }
         
         /* now just resend the last data with the Authorization header */
         sendInformData();
         /* now we just return to wait on the response from the server */
         #ifdef FORCE_NULL_AFTER_INFORM
         cmsLog_debug("set Timer to Force Null send to ACS (Cisco tool)");
         cmsTmr_set(tmrHandle, informRspTimeout, ht, 1*1000, "informRspTimeout"); /******** ?????????????CISCO TOOL ???????? send null if server doesn't respond */
         #endif
      }
      else if (wg->hdrs->status_code  >= 300 &&  wg->hdrs->status_code <= 307)
      {
         if (wg->hdrs->locationHdr != NULL)
         {
            /* Redirect status with new location */
            /* repost msg to new URL */
            closeACSConnection(ht);
            ht->eHttpState = eStart;
            cmsLog_debug("Redirect to %s", wg->hdrs->locationHdr);
            ht->wio = wget_Connect(wg->hdrs->locationHdr, handleRedirectionCallBack,  NULL);

            if (ht->wio == NULL)
            {
               cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"Redirect failed: %s", wget_LastErrorMsg());
               acsDisconnect(ht, eConnectError);
            }
         }
         else
         {
            cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"Redirect failed: location header is empty");
            acsDisconnect(ht, eConnectError);
         }
      }
      else
      {
         // If status != 401 after inform, and eAuthState == sIdle,
         // we're in the wrong authentication state.  The correct state should be from 
         // sAuthenticating to sAutenticated so disconnect unless
         // ACS does not have any authentications and returns status_code == 200
         // right after CPE sends the first Inform message 
         if (ht->eAuthState == sIdle && wg->hdrs->status_code != 200)
         {
            ht->eAuthState = sAuthFailed;
            /* disconnect and delay */
            acsDisconnect(ht, eAuthError);                
            cmsLog_debug("Error: From sIdle -> sAuthenticated w/t sAuthenticating");
            return;
         }

         ht->eAuthState = sAuthenticated;
         if (informState == eACSNeverContacted)
         {
            informState = eACSContacted;
         }
         
         if ((wg->hdrs->status_code == 200) &&
             ((wg->hdrs->content_length > 0) ||
              (wg->hdrs->TransferEncoding && streq(wg->hdrs->TransferEncoding,"chunked")))) 
         {
            /* allocate buffer and parse the response */
            int     mlth;
            char    *soapmsg;

            // save cookies of the current connection to be used later
            // for "eHttpSate = eClose" OR "307 Redirection"
            if (wg->hdrs->setCookies != NULL)
            {
               copyCookies(&glbCookieHdr, wg->hdrs->setCookies);
            }
            /* msg posted - free buffer */
            CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
            soapmsg = readResponse(wg, &mlth,0);

            // if TCP connection is closed by ACS
            if (ht->eHttpState == eClose) 
            {
               // close connection and reconnect since ACS asks for it
               closeACSConnection(ht);
               ht->wio = wget_Connect(acsState.acsURL, handleSoapMessageCallBack, soapmsg);
               if ( ht->wio == NULL )
               {
                  cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"ACS Connect failed: %s", wget_LastErrorMsg());
                  CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
                  CMSMEM_FREE_BUF_AND_NULL_PTR(ht->authHdr);
                  return;
               }
               // return here since handleSoapMessage will be called in handleSoapMessageCallBack
               return;
            }
            handleSoapMessage(soapmsg, mlth);                 
         }
         else if ((wg->hdrs->status_code == 204) ||
                    (wg->hdrs->status_code == 200 && !wg->hdrs->TransferEncoding))
         { 
            // only terminate session if no pending
            if (transferCompletePending == 0 && sendGETRPC == 0)
            {                     
               /* empty ACS message -- ACS is done */
               cmsLog_debug("empty ACS msg - sentACSNull=%d", sentACSNull);
               /* msg posted - free buffer */
               CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
               if (!sentACSNull)
               {
                  sendNullHttp(FALSE);
               }
               if (wg->hdrs->status_code == 200 && ht->eHttpState == eClose)
               {
                  closeACSConnection(ht);
               }
               else
               {
                  acsDisconnect(ht, eAcsDone);
                  // TR-069 session is finished
                  //sessionState = eSessionEnd;
                  // free cookies since tr-069 session is finished
                  if (glbCookieHdr != NULL) {
                     freeCookies(&glbCookieHdr);
                     glbCookieHdr = NULL;
                  }
                  // free soapmsg in current session that is used for
                  // "Redirection at any Point in a Session" requirement
                  // when session is closed by receiving 204
                  CMSMEM_FREE_BUF_AND_NULL_PTR(prevSoapMsg);      
                  if (connReqInSession == TRUE)
                  {
                     connReqInSession = FALSE;
                     // close connection
                     closeACSConnection(ht);
                     cmsLog_debug("Start TR-069 session immediately since Connection Request is received during previous session");
                     handleConnectionRequest();
                  }
               }
            }
            else
            {
               if (transferCompletePending == 1)
               {
                  // transfer complete is pending so send it when receive empty message
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
               else if (sendGETRPC == 1)
               {
                  sendGetRPCMethods();
                  sendGETRPC = 0;
               }
            }
         }
         else if (wg->hdrs->status_code == 100)
         {
            /* 100 Continue: Just ignore this status */
            /* msg posted - free buffer */
            CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
         }
         else if (wg->hdrs->status_code >= 300 &&
                  wg->hdrs->status_code <= 307 &&
                  wg->hdrs->locationHdr)
         {
            /* Redirect status with new location */
            /* repost msg to new URL */
            closeACSConnection(ht);
            ht->eHttpState = eStart;
            cmsLog_debug("Redirect to %s", wg->hdrs->locationHdr);
            ht->wio = wget_Connect(wg->hdrs->locationHdr, handleRedirectionCallBack,  NULL);

            if (ht->wio == NULL)
            {
               cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"Redirect failed: %s", wget_LastErrorMsg());
               acsDisconnect(ht, ePostError);
            }
         }
         else
         {
            /* msg posted - free buffer */
            CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
            cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"Unknown status_code=%d received from ACS or encoding",
                           wg->hdrs->status_code);
            acsDisconnect(ht, ePostError);
         }
      }
   }
   else
   {
      cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"Post to ACS failed, Status = %d %s\n", wg->status, wg->msg);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
      acsDisconnect(ht, ePostError);
   }
}  /* End of postComplete() */

/* 
 *  This routine store an events to the informEvList. 
 *  sendInformData will look at this informEvList and
 *  compose the inform message when it's ready to be sent 
 */
/*  Replace setupEventList, putInformEventMmethod and putInformEvent */

UINT32 addInformEventToList(UINT8 event)
{
   int   i;
   
   cmsLog_debug("entered with event=%d", event);

   /*
    * Check for duplicate event codes in the informEvList.
    * We only need each event to appear once in the List/array.
    * Single cumulative behavior cannot be repeated, but multiple can.
    * At this point, multiple ones are added according to ACSstate.-- May need to change this to rely less on global var.
    */
   for (i = 0; i < informEvList.informEvCnt; i++)
   {
      if (informEvList.informEvList[i] == event)
      {
         cmsLog_debug("event %d is already in informEvList, do nothing", event);
         return 0;
      }
   }


   if (informEvList.informEvCnt < MAXINFORMEVENTS)
   {
      informEvList.informEvList[informEvList.informEvCnt] = event;
      cmsLog_debug("adding event %d to informEvList, count is now %d", event, informEvList.informEvCnt);
      informEvList.informEvCnt++;
      return informEvList.informEvCnt;
   }
   else
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"too many events in informEvList, count=%d", informEvList.informEvCnt);
   }

   return 0;
}

/*
 * The connection to the ACS has been completed or
 * an error has ocurred.
 */
static void Connected(void *handle)
{
   HttpTask *ht = &httpTask;
   tWget    *wg = (tWget *)handle;

   cmsLog_debug("=====>ENTER");
   if (wg->status != 0)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
      acsDisconnect(ht, eConnectError);
      cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"ACS Connect Status = %d %s", wg->status, wg->msg);
      return;
   }
   if (ht->wio==NULL)
   {
      ht->eHttpState = eClosed;
      cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"Error -- pointer to IO desc is NULL");
      return;
   }

   ht->eHttpState = eConnected;
   sendInformData();
}  /* End of Connected() */

/*
 * Send the current buffer to the ACS. This is an async call. The 
 * return status only indicates that the connection has started.
 * The httpTask structure represents the only connection to an
 * ACS. If the connection is up then it is reused; otherwise,
 * a new connection is attempted.
 */
void sendToAcs(int contentLen, char *buf)
{
   HttpTask *ht = &httpTask;

   cmsLog_debug("=====>ENTER");
   if (ht->postMsg)
   {
      cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"postMsg buffer not null");
      CMSMEM_FREE_BUF_AND_NULL_PTR(ht->postMsg);
   }
   if (getAcsConnDesc() == NULL)
   {
      cmsLog_error(TR069_ALARM_ACS_CONNECT_FAILED,"Try to send message to ACS while connection is NULL!");
      return;
   }
   ht->content_len = contentLen;
   ht->postMsg     = buf;
   ht->postLth     = buf? strlen(buf): 0;
   wget_ClearPostHdrs(ht->wio);
   #ifdef GENERATE_SOAPACTION_HDR
   // only add "SOAPAction" header when it is NOT EMPTY POST
   if (contentLen != 0)
   {
      wget_AddPostHdr(ht->wio,"SOAPAction", "");
   }
   #endif
   updateAuthorizationHdr(ht);
   if (ht->eHttpState == eClose)
   {
      wget_PostDataClose(ht->wio, ht->postMsg, ht->postLth, "text/xml",
                         postComplete,( void*) ht);
   }
   else
   {
      wget_PostData(ht->wio, ht->postMsg, ht->postLth, "text/xml",
                    postComplete, (void *)ht);
   }
}  /* End of sendToAcs() */

/* 
 * Send an Inform message. The possible handle values are
 * eIEConnectionRequest
 * eIETransferComplete
 * eIEJustBooted.
 * The other possible events are set by xxxPending flags.
 * eIEPeriodix
 * eIEDiagnostics
 * 
 */
void sendInform(void *dummy __attribute__((unused)))
{
//   int numOfValueChanged = getMdmParamValueChanges();
//   CmsTimestamp now;
//   UINT32 deltaMs;
//   int timeToSend = 1;
//   LimitNotificationInfo *ptr;
   UINT32 eventCount = 0;
//   UINT32 notificationEvent = 0;

   if (!SmdStarted)
   {
      /* not registered to oam yet, we don't send inform */
      cmsLog_notice("not registered to oam yet, we don't send inform");
      return;
   }

   eventCount = informEvList.informEvCnt;
   if (eventCount == 0)
   {
      /* inform has been sent, there is nothing to send now */
      cmsLog_notice("no event found, we don't send inform");
      return;
   }

   if(rebootingFlag)
   {
      /* system is rebooting, we don't send inform */
      cmsLog_notice("system is rebooting, we don't send inform");
      return;
   }

   if (isAcsConnected())
   {
      cmsLog_notice("acs still in connection, we don't send inform");
      return;
   }

   sentACSNull = 0;

   if (acsState.acsURL != NULL && acsState.acsURL[0] != '\0')
   {
      HttpTask *ht = &httpTask;
      CmsRet ret;

      cmsLog_debug("Connect to ACS at %s", acsState.acsURL);
      ht->eHttpState = eStart;
      ht->wio = wget_Connect(acsState.acsURL, Connected, NULL);

      if (ht->wio == NULL)
      {
         cmsLog_debug("ACS Connect Failed: %s", wget_LastErrorMsg());
         cmsLog_debug("set delayed inform timer for %d ", CHECKWANINTERVAL);
         /* mwang: this looks a bit wrong.  The handle is used to 
          * look up this timer.  But handle is this function is actually
          * an eInformEvent enum.  So are we saying if we get here,
          * the eInformEvent enum will always be the same?
          */
         ret = cmsTmr_replaceIfSooner(tmrHandle, sendInform, NULL, CHECKWANINTERVAL, "WANCHECK_inform");

         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error(TR069_ALARM_TIMER_ERROR,"setting delayed wan inform timer failed, ret=%d", ret);
         }
      }
      else
      {
#if 0
         /* go through the whole notification list, cancel timers because they will all be sent now */
         /* in this case, everything will be sent out, so we cancel all timers and update lastSent field */
         ptr = limitNotificationList.limitEntry;
         while (ptr != NULL)
         {
            /* Everything will be sent, so we update our list.
             * To simplify the whole implementation, I just update the lastSent for everyone in the list.
             * LastSent is basically the last notification sent, not necessarily the notification of this
             * parameter.
             */
            if (ptr->notificationPending)
            {
               cmsLog_debug("timerCancel for ptr->parameterFullPathName %s",ptr->parameterFullPathName);
               cmsTmr_cancel(tmrHandle,ptr->func,NULL);
               ptr->notificationPending = 0;
            }
            cmsTms_get(&ptr->lastSent);
            ptr = ptr->next;
         } /* loop */
#endif
      } /* inform sent */  
   } /* acs */
   else
   {
      cmsLog_debug("acsURL is NULL!");
   }
}  /* End of sendInform() */

static void sendInformData(void)
{
   RPCAction *a = newRPCAction();
   HttpTask    *ht = &httpTask;

   cmsLog_debug("=====>ENTER");
   
   // free soapmsg in previous session that is used for
   // "Redirection at any Point in a Session" requirement
   // when session is started by sending Inform
   CMSMEM_FREE_BUF_AND_NULL_PTR(prevSoapMsg);      
   if (ht->eAuthState == sAuthenticating)
   {
      // second Inform is ready to send out for authentication
      sessionState = eSessionAuthenticating;
   }
   else
   {
      // first Inform is ready to send out to start TR-069 session
      sessionState = eSessionStart;
      cmsLog_log("CWMP session started. URL(%s)", acsState.acsURL);
   }

   buildInform(a, &informEvList);
   freeRPCAction(a);
}  /* End of sendInformData() */



/** Send msg to smd requesting a delayed msg.
 *
 * The delayed msg contains a special id (PERIODIC_INFORM_TIMEOUT_ID) in the
 * wordData field of the message header.  When I get a DELAYED_MSG with this
 * special id, I know it is time to do a periodic inform.
 * Requesting a delayed message will also cancel any previous requests with the
 * same id.
 */
void requestPeriodicInform(UINT32 interval)
{
   char buf[sizeof(CmsMsgHeader) + sizeof(RegisterDelayedMsgBody)] = {0};
   CmsMsgHeader *msg;
   RegisterDelayedMsgBody *body;
   CmsRet ret;

   msg = (CmsMsgHeader *) buf;
   body = (RegisterDelayedMsgBody *) (msg + 1);

   msg->type = CMS_MSG_REGISTER_DELAYED_MSG;
   msg->src = EID_TR69C;
   msg->dst = EID_SMD;
   msg->flags_request = 1;
   msg->wordData = PERIODIC_INFORM_TIMEOUT_ID;
   msg->dataLength = sizeof(RegisterDelayedMsgBody);

   body->delayMs = interval * MSECS_IN_SEC;  /* tr69c uses seconds, smd uses ms */

   ret = cmsMsg_sendAndGetReply(msgHandle, msg);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_SOCKET_ERROR,"request failed, ret=%d", ret);
   }
   else
   {
      cmsLog_debug("set next periodic inform for %u seconds in future", interval);
   }

   return;
}


void cancelPeriodicInform()
{
//   CmsMsgHeader msg;
//   CmsRet ret;

   // cancel local periodic inform timer in tr69c
   cmsTmr_cancel(tmrHandle, periodicInformTimeout,(void*)NULL);
#if 0
   memset(&msg, 0, sizeof(CmsMsgHeader));

   msg.type = CMS_MSG_UNREGISTER_DELAYED_MSG;
   msg.src = EID_TR69C;
   msg.dst = EID_SMD;
   msg.flags_request = 1;
   msg.wordData = PERIODIC_INFORM_TIMEOUT_ID;

   // send message from tr69c to smd so that smd will not wake up
   // tr69c when periodic inform is expired
   ret = cmsMsg_sendAndGetReply(msgHandle, &msg);
   if (ret != CMSRET_SUCCESS && ret != CMSRET_NO_MORE_INSTANCES)
   {
      cmsLog_error("request failed, ret=%d", ret);
   }
#endif
}

/** Periodic Inform Timer callback.
 *
 * If peridic inform is enabled and informInterval > 0 then always create another
 * timer event.
 */
void periodicInformTimeout(void *handle __attribute__((unused)))
{
   addInformEventToList(INFORM_EVENT_PERIODIC);
   sendInform(NULL);

   resetPeriodicInform(acsState.informInterval);
#ifdef USE_DMALLOC
   dmalloc_log_unfreed();
#endif // USE_DMALLOC
}  /* End of periodicInformTimeout() */

/*
 * Called from setter function to update next inform time
 */
void resetPeriodicInform(UINT32 interval)
{
   CmsRet ret;
   if (acsState.informEnable)
   {
      ret = cmsTmr_replaceIfSooner(tmrHandle, periodicInformTimeout, (void*)NULL, interval*MSECS_IN_SEC, "periodic_inform");
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error(TR069_ALARM_SOCKET_ERROR,"could not set timer, ret=%d", ret);
      }
   }
   else
   {
      cmsTmr_cancel(tmrHandle, periodicInformTimeout, (void*)NULL);
   }
}

void scheduleInformTimeout(void *handle __attribute__((unused)))
{
   addInformEventToList(INFORM_EVENT_SCHEDULED);
   addInformEventToList(INFORM_EVENT_SCHEDULE_METHOD);
   sendInform(NULL);
}  /* End of scheduleInformTimeout() */

void setScheduleInform(UINT32 interval)
{
   CmsRet ret;

   ret = cmsTmr_replaceIfSooner(tmrHandle, scheduleInformTimeout, (void*)NULL, interval*MSECS_IN_SEC, "schedule_inform");

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_SOCKET_ERROR,"could not set timer, ret=%d", ret);
   }
}

/*
* This can only be called after doDownload() has already sent the Download response.
* It is called by the httpDownload functions if an error occurs.
* Thus we need to send a TransferComplete message here.
* If the ACS is not connected then an Inform needs to be started.
*/
void sendDownloadFault()
{
   if (isAcsConnected())
   {
      transferCompletePending = 1;
      cmsLog_debug("acs is connected -- set transferCompletePending");
   }
   else
   {
      cmsLog_debug("acs is not connected -- sendInform(TransferComplete)");
      transferCompletePending = 1;
      addInformEventToList(INFORM_EVENT_TRANSER_COMPLETE);
      sendInform(NULL);
   }

   updateTransferState(acsState.downloadCommandKey,eTransferCompleted);
}  /* End of sendDownloadFault() */


/** This is only called at tr69c startup from initInformer.
 *
 */
void startACSComm(void *handle  __attribute__((unused)))
{
   /*
    * Initialize server socket for receiving connection requests from ACS.
    */
   startACSListener();
   sendInform(NULL);
   resetPeriodicInform(acsState.informInterval);
}  /* End of startACSComm() */

/* called once when tr69c starts - must be called AFTER all instances are initialized */
void initInformer(void)
{
   CmsRet ret;

   /* start trying ACSComm in a moment (ACSINFORMDELAY=500ms) */
   ret = cmsTmr_set(tmrHandle, startACSComm, NULL, ACSINFORMDELAY, "startACSComm");
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_TIMER_ERROR,"could not set timer, ret=d", ret);
   }

   initTransferList();
}  /* End of initInformer() */
