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
 * File Name  : httpUpload.c
 *
 * Description: upload functions 
 * $Revision: 1.16 $
 * $Id: httpUpload.c,v 1.16 2006/11/1 21:15:09 Peter Tran Exp $
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

#ifdef TESTBOX
#include "../bcmLibIF/bcmTestWrapper.h"
#else
#include "cms.h"
#include "cms_log.h"
#include "../bcmLibIF/bcmWrapper.h"
#endif

#include "../inc/appdefs.h"
#include "../inc/utils.h"
#include "../SOAPParser/RPCState.h"
#include "event.h"
#include "cms_mem.h"

#include "../webproto/protocol.h"
#include "../webproto/www.h"
#include "../webproto/wget.h"
#include "../main/informer.h"
#include "../main/httpProto.h"
#include "../main/httpDownload.h"
#include "../SOAPParser/xmlParserSM.h"
#include "../SOAPParser/xmlTables.h"
#include "../SOAPParser/RPCState.h"
#include "../inc/tr69cdefs.h" /* defines for ACS state */

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#define XML_PSI_END_TAG       "</psitree>"
#define UPLOAD_BUFFER_SIZE    512

//#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

extern ACSState     acsState;




extern int transferCompletePending;

static void uploadStop(char *msg, int status)
{
    setInformState( eACSUpload );
    addInformEventToList(INFORM_EVENT_UPLOAD_METHOD);
    addInformEventToList(INFORM_EVENT_TRANSER_COMPLETE);

    acsState.fault = 0;
    acsState.dlFaultStatus = status; 

    if ( msg == NULL )
    {
       CMSMEM_REPLACE_STRING(acsState.dlFaultMsg,"Upload successful"); 
    }
    else
    {
       CMSMEM_REPLACE_STRING(acsState.dlFaultMsg,msg);
    }

    acsState.endDLTime = time(NULL);
    saveTR69StatusItems();
    sendDownloadFault();
}

/* this is called by the callback from the startTimer in doUpload. */
/* we have to use wget_Connect in case of authentication in which case */
/* we need to control the connection */
void uploadStart( void *handle) 
{
    RPCAction   *action = (RPCAction *)handle;

    DownloadReq *ulreq = &action->ud.downloadReq;

    updateDownLoadKey(ulreq);

    if (ulreq->url == NULL)
    {
        uploadStop("Invalid arguments", 9003);
        return;
    }

    if (strstr(ulreq->url, "http:") == NULL &&
        strstr(ulreq->url, "https:") == NULL &&
        strstr(ulreq->url, "ftp:") == NULL &&
        strstr(ulreq->url, "ftps:") == NULL)
    {
        uploadStop("Protocol not supported", 9013);
        return;
    }

    if (strstr(ulreq->url, "@") != NULL)
    {
        uploadStop("Invalid arguments", 9003);
        return;
    }

    if (/*ulreq->efileType != eVendorConfig &&*/
        ulreq->efileType != eVendorLog &&
        ulreq->efileType != eVendorTcpdumpFiles &&
        /*ulreq->efileType != eVendorCertReq &&*/
        ulreq->efileType != eVendorDbRawFiles &&
        ulreq->efileType != eVendorDbRawInner &&
        ulreq->efileType != eVendorDbCnData)
    {
        uploadStop("Invalid upload file type", 9011);       
        return;
    }

    acsState.startDLTime = time(NULL);
    SendUploadReqToOam(ulreq);
}

