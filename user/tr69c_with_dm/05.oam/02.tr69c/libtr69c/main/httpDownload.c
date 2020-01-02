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
 * File Name  : httpDownload.c
 *
 * Description: download functions 
 * $Revision: 1.16 $
 * $Id: httpDownload.c,v 1.16 2006/02/17 21:15:09 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>

#include "cms_log.h"
#include "cms_msg.h"
#include "cms_mem.h"

#include "../bcmLibIF/bcmWrapper.h"

#include "../inc/appdefs.h"
#include "../inc/utils.h"
#include "../SOAPParser/RPCState.h"
#include "event.h"

#include "../webproto/protocol.h"
#include "../webproto/www.h"
#include "../webproto/wget.h"
#include "../main/informer.h"
#include "../main/httpProto.h"
#include "../SOAPParser/xmlParserSM.h"
#include "../SOAPParser/xmlTables.h"
#include "../SOAPParser/RPCState.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif

#include "../inc/tr69cdefs.h" /* defines for ACS state */


extern ACSState    acsState;
extern void *msgHandle;

/* interface name from socket used for image uploading */
char connIfName[CMS_IFNAME_LENGTH]={0};
/*
  * display SOAP messages on serial console.
  * This flag is initialize, enabled or disabled in main.c,
  * and perform action in protocol.c
  */
extern UBOOL8   loggingSOAP; 




void downloadStop(char *msg, int status)
{
    setInformState(eACSDownloadReboot);
    addInformEventToList(INFORM_EVENT_DOWNLOAD_METHOD);
    addInformEventToList(INFORM_EVENT_TRANSER_COMPLETE);

    acsState.fault = 0;
    acsState.dlFaultStatus = status; 

    if ( msg == NULL )
    {
       CMSMEM_REPLACE_STRING(acsState.dlFaultMsg,"Download successful"); 
    }
    else
    {
       CMSMEM_REPLACE_STRING(acsState.dlFaultMsg,msg);
    }

    acsState.endDLTime = time(NULL);
    saveTR69StatusItems();
    sendDownloadFault();
}

void updateDownLoadKey(DownloadReq *dlr)
{
    if (acsState.downloadCommandKey)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.downloadCommandKey);
    }
    acsState.downloadCommandKey = dlr->commandKey;
    dlr->commandKey = NULL; 
}

/* this is called by the callback from the startTimer in doDownload. */
/* we have to use wget_Connect in case of authentication in which case */
/* we need to control the connection */
void downloadStart( void *handle) {
    RPCAction   *action = (RPCAction *)handle;

    DownloadReq *dlreq = &action->ud.downloadReq;
    
    // update download command key to fix CSP# 41725
    updateDownLoadKey(dlreq);

    if (dlreq->url == NULL)
    {
        downloadStop("Invalid arguments", 9003);
        return;
    }

    if (strstr(dlreq->url, "http:") == NULL &&
        strstr(dlreq->url, "https:") == NULL &&
        strstr(dlreq->url, "ftp:") == NULL &&
        strstr(dlreq->url, "ftps:") == NULL)
    {
        downloadStop("Protocol not supported", 9013);
        return;
    }

    if (strstr(dlreq->url, "@") != NULL)
    {
        downloadStop("Invalid arguments", 9003);
        return;
    }

    if (dlreq->efileType != eFirmwareUpgrade &&
        /*dlreq->efileType != eVendorConfig &&*/
        dlreq->efileType != eVendorDbDefaultFile &&
        dlreq->efileType != eVendorDbRawFiles &&
        dlreq->efileType != eVendorDbRawInner &&
        dlreq->efileType != eVendorDbDefaultInner &&
        dlreq->efileType != eVendorFirmware &&
        dlreq->efileType != eVendorClCert &&
        dlreq->efileType != eVendorCaCert &&
        dlreq->efileType != eVendorDbCnData &&
        dlreq->efileType != eVendorDbCnDataDefault)
    {
        downloadStop("Invalid download file type", 9010);       
        return;
    }

    acsState.startDLTime = time(NULL);
    CmsRet ret = SendDownloadReqToOam(dlreq);
    if(ret != CMSRET_SUCCESS)
    {
        downloadStop("Failed to send download request to OAM.", ret);
        return;
    }
}

