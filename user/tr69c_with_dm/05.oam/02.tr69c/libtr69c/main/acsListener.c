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
 * File Name  : acsListener.c
 *
 * Description: ACS listener: Listens for connections from ACS
 * $Revision: 1.11 $
 * $Id: acsListener.c,v 1.11 2006/01/31 23:11:06 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>

#include "cms_log.h"
#include "cms_tmr.h"
#include "../inc/tr69cdefs.h" /* defines for ACS state */
#include "../inc/appdefs.h"
#include "../inc/utils.h"
#include "../SOAPParser/RPCState.h"
#include "../SOAPParser/xmlParserSM.h"
#include "../SOAPParser/xmlTables.h"
#include "../webproto/protocol.h"
#include "../webproto/www.h"
#include "../webproto/wget.h"
#include "../bcmLibIF/bcmWrapper.h"
#include "event.h"
#include "informer.h"




/** external data **/
extern ACSState acsState;
extern UBOOL8 openConnReqServerSocket;
extern eSessionState sessionState;
extern void *tmrHandle;

/** external functions **/
extern CmsRet rut_isGponIpHostInterface(char *ifname, UINT32 *meId);

/** public data **/
UBOOL8 connReqInSession = FALSE;

typedef enum
{
   sIdle,
   sAuthenticating,
   sAuthenticated,
   sAuthFailed,
   sShutdown
} eACSConnState;

typedef struct ACSConnection
{
   eACSConnState  cState; /* authentication state */
   tProtoCtx      *cpc;   /* so we can use wget proto functions */
   int            lfd;    /* listener socket */
   tHttpHdrs      hdrs;
} ACSConnection;

/** private data **/
static ACSConnection connection;
static SessionAuth   acsConnSess;

static void startACScallback(void *handle);
void handleConnectionRequest(void);

static void free_http_headers(tHttpHdrs *hdrs)
{
   CookieHdr   *cp, *last;

   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->content_type);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->protocol);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->wwwAuthenticate);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->Authorization);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->TransferEncoding);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->Connection);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->method);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->path);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->host);
   cp = hdrs->setCookies;
   while (cp)
   {
      last = cp->next;
      CMSMEM_FREE_BUF_AND_NULL_PTR(cp->name);
      CMSMEM_FREE_BUF_AND_NULL_PTR(cp->value);
      CMSMEM_FREE_BUF_AND_NULL_PTR(cp);
      cp = last;
   }
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->message);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->locationHdr);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->filename);
   CMSMEM_FREE_BUF_AND_NULL_PTR(hdrs->arg);
   // do not free(hdrs) since it's needed for Connection Request
   memset(hdrs, 0, sizeof(tHttpHdrs));
}


static int testChallenge( ACSConnection *cd)
{
   return (parseAuthorizationHdr(cd->hdrs.Authorization, &acsConnSess,
                                    acsState.connReqUser,acsState.connReqPwd));
}

void statusWriteComplete(void *handle)
{
   ACSConnection *cd = (ACSConnection*)handle;
   UINT32 delayMs=2000;  /* may want smarter time value here ????*/
   CmsRet ret;

   free_http_headers(&cd->hdrs);
   proto_FreeCtx(cd->cpc);

   /*
    * The orig code was:
    * memset(cd, 0, sizeof(struct ACSConnection));
    * but that wipes out cd->lfd, which we still want to keep open.
    * When we call startACScallback from the 2 sec. timer, we will
    * start listening to that fd again.
    */
   cd->cState = 0;
   cd->cpc = NULL;
   memset(&(cd->hdrs), 0, sizeof(tHttpHdrs));


   /*
    * mwang: Why do we wait a whole 2 seconds before we accept another connection?
    */
   ret = cmsTmr_set(tmrHandle, startACScallback, NULL, delayMs, "wrt_comp_startACScallback");
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_TIMER_ERROR,"could not set startACSCallback timer, ret=%d", ret);
   }

   return;
}

static int sendOK( ACSConnection *cd)
{
   char    response[300];
   int     i;

   i = snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n");
   i+= snprintf(response+i, sizeof(response)-i, "Content-Length: 0\r\n\r\n");
   if (proto_Writen(cd->cpc, response, i) < i)
   {
      return 0;
   }
   return 1;
}

static int sendAuthFailed( ACSConnection *cd )
{
   char    response[300];
   int     i;

   i = snprintf(response, sizeof(response), "HTTP/1.1 401 Unauthorized\r\n");
   i+= snprintf(response+i, sizeof(response)-i, "Content-Length: 0\r\n\r\n");
   if (proto_Writen(cd->cpc, response, i) < i)
   {
      return 0;
   }
   return 1;
}

static int sendChallenge( ACSConnection *cd )
{
   char    response[300];
   char    *h;
   int     i;

   i = snprintf(response, sizeof(response), "HTTP/1.1 401 Unauthorized\r\n");
   i+= snprintf(response+i, sizeof(response)-i, "Content-Length: 0\r\n");
   h = generateWWWAuthenticateHdr( &acsConnSess, ACSREALM, ACSDOMAIN, cd->hdrs.method);
   i+= snprintf(response+i, sizeof(response)-i,"%s\r\n\r\n", h);
   CMSMEM_FREE_BUF_AND_NULL_PTR(h);
   if (proto_Writen(cd->cpc, response, i) < i)
   {
      return 0;
   }
   return 1;
}
/**
 * A connected ACS is sending us data,
 * Our action is to generate a digest authentication challange
 * with a 401 Unauthorized status code and
 * wait for the response to the challange. Then  send a 
 * 200 OK or a 401 Unauthorized. */
static void acsReadData(void *handle) 
{
   ACSConnection *cd = (ACSConnection *)handle;

   cmsLog_debug("acsReadData");
   /* Free resources allocated earlier */
   free_http_headers(&cd->hdrs);
   // Do not need to call updateConnectionRequestInfo() anymore since this information
   // is updated automatically by using event mechanism so that
   // when Management Sever or Tr69cConfig object is changed, event is sent to tr69c.
   //updateConnectionRequestInfo();

   if (proto_ParseRequest(cd->cpc, &cd->hdrs) == 0)
   {
      proto_ParseHdrs(cd->cpc, &cd->hdrs);
      if (!strcasecmp("http/1.1", cd->hdrs.protocol))
      {
         /* protocol is correct */
         if (!strcmp(acsState.connReqPath,cd->hdrs.path))
         {
            /* path is correct proceed with authorization */
            if (acsState.connReqUser==NULL || acsState.connReqUser[0]=='\0')
            {
               cd->cState = sAuthenticated;
            }
            if (cd->cState == sIdle)
            {
               if(acsState.noneConnReqAuth) 
               {
                  cd->cState = sAuthenticated;
                  sendOK(cd);
                  handleConnectionRequest();
                  cd->cState = sShutdown;
                  setListenerType(cd->cpc->fd, statusWriteComplete,cd, iListener_Write);
                  return;
               }
               else 
               {
                  /* send 401 with digest challange */
                  sendChallenge( cd );
                  cd->cState = sAuthenticating;
                  setListener(cd->cpc->fd, acsReadData, (void *)cd);
                  return;
               }           
            }
            else if (cd->cState == sAuthenticating)
            {
               if (testChallenge(cd))
               {
                  sendOK(cd);
                  /* avoid race condition between periodic inform and connection request inform */
                  resetPeriodicInform(acsState.informInterval);
                  handleConnectionRequest();
                  cd->cState = sShutdown;
               }
               else
               {
                  cmsLog_debug("ConnectRequest authentication error");
                  sendAuthFailed(cd);
                  cd->cState = sShutdown;
               }
               setListenerType(cd->cpc->fd, statusWriteComplete,cd, iListener_Write);
               return;
            }
            else if (cd->cState == sAuthenticated)
            {
               sendOK(cd);
               /* avoid race condition between periodic inform and connection request inform */
               resetPeriodicInform(acsState.informInterval);
               handleConnectionRequest();
               cd->cState = sShutdown;
               setListenerType(cd->cpc->fd, statusWriteComplete,cd, iListener_Write);
               return;
            }
         }
      }
   }
   else
   {
      cmsLog_debug("acsListener Error reading response");
   }
   cd->cState = sShutdown;
   setListenerType(cd->cpc->fd, statusWriteComplete, cd, iListener_Write);
}


/** ACS is trying to connect to us.
 *
 * Accept the connection, add the new fd to the listeners list.
 */
static void acsConnect(void *handle) 
{
   struct sockaddr_storage addr;
   ACSConnection      *cd = (ACSConnection *) handle;
   socklen_t          sz  = sizeof(addr);
   int flags=1;
   int fd;
   int res;
   const UINT32 delayMs=5000;  /* reenable listen in 5 sec */
   CmsRet ret;

   cmsLog_debug("cd->lfd=%d", cd->lfd);

   if (sessionState != eSessionEnd)
   {
      cmsLog_debug("Connection Request is received while session exists, set connReqInSession = TRUE");
      connReqInSession = TRUE;
   }
   
   stopListener(cd->lfd);
   memset(&addr, 0, sz);
   if (( fd = accept(cd->lfd, (struct sockaddr *)&addr, &sz)) < 0)
   {
      cmsLog_error(TR069_ALARM_SOCKET_ERROR,"acsListen accept failed errno=%d (%s) fd=%d",errno, strerror(errno), cd->lfd);
      ret = cmsTmr_set(tmrHandle, startACScallback, NULL, delayMs, "conn_err_startACScallback"); 
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error(TR069_ALARM_TIMER_ERROR,"could not set startACSCallback timer, ret=%d", ret);
      }

      return;
   }
   else
   {
      char addrBuf[CMS_IPADDR_LENGTH]={0};

      if (addr.ss_family == AF_INET6)
      {
         inet_ntop(addr.ss_family, &(((struct sockaddr_in6 *)&addr)->sin6_addr), addrBuf, CMS_IPADDR_LENGTH);
      }
      else
      {
         inet_ntop(addr.ss_family, &(((struct sockaddr_in *)&addr)->sin_addr), addrBuf, CMS_IPADDR_LENGTH);
      }

      cmsLog_debug("accepted conn from %s on fd=%d", addrBuf, fd);
      cmsLog_log("connection request accepted from %s on fd=%d", addrBuf, fd);
   }

   /*
    * Orig code:
    * close(cd->lfd);
    * cd->lfd = 0;
    *
    * In CMS, tr69c will not close the listener socket fd after it accepts a
    * connection.  Instead, stopListener call above will effectively cause tr69c
    * to stop servicing any new connection requests.
    * After this transaction is done, statusWriteComplete() will schedule a
    * delayed timer call, which will call startACSCallback, which will call
    * setListener() on the listener socket fd again.  We don't close the
    * listener socket in tr69c because smd also has a reference to the socket.
    */

   cd->cpc = proto_NewCtx(fd);

   if (cd->cpc==NULL)
   {
      cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"malloc() failed for cd->cpc");
	  return;
   }

   if ( (res = setsockopt(cd->cpc->fd, SOL_SOCKET, SO_REUSEADDR, &flags,sizeof(flags)))<0)
      cmsLog_error(TR069_ALARM_SOCKET_ERROR,"proxy_connection() setsockopt error %d %d %s", cd->cpc->fd, errno, strerror(errno));

   setListener(cd->cpc->fd, acsReadData, cd);
}


/* 
* return -1: for error
*       != -1 is socket
*/                              
static int initSocket(int port,int af) 
{
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    int port_sock = 0;
    int res, i = 1;
    port_sock = socket(af, SOCK_STREAM, IPPROTO_TCP);
    if (port_sock < 0)
    {
        cmsLog_notice("web: init_listen_socket(af=%d, port=%d), socket failed", af, port);
        return -1;
    }

    res = setsockopt(port_sock, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i));
    if (res < 0)
    {
        cmsLog_notice("web: %s, af=%d, port=%d", "Socket error listening to ACS", af, port);
        close(port_sock);
        return -1;
    }
#if 0
    if (rut_isGponIpHostInterface(acsState.boundIfName, NULL) == CMSRET_SUCCESS)
    {
        res = setsockopt(port_sock, SOL_SOCKET, SO_BINDTODEVICE, acsState.boundIfName, strlen(acsState.boundIfName)+1);
        if (res < 0)
        {
            cmsLog_error("web: Socket error bind to device %s", acsState.boundIfName);
            close(port_sock);
            return -1;
        }
    }
#endif

    if (af == AF_INET)
    {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family       = af;
        addr.sin_addr.s_addr  = htonl(INADDR_ANY);
        addr.sin_port         = htons(port);
        
        res = bind(port_sock, (struct sockaddr *)&addr, sizeof(addr));
    }
    else
    {
        memset(&addr6, 0, sizeof(addr6));
        addr6.sin6_family       = af;
        addr6.sin6_addr  = in6addr_any;
        addr6.sin6_port         = htons(port);

        res = bind(port_sock, (struct sockaddr *)&addr6, sizeof(addr6));
    }

    if (res < 0)
    {
        cmsLog_notice("acsListener bind failed, af=%d, port=%d, errno=%d.%s", af, port, errno, strerror(errno));
        close(port_sock);
        return -1;
    }

    res = listen(port_sock,1);
    if (res < 0)
    {
        cmsLog_error(TR069_ALARM_SOCKET_ERROR,"acsListener listent failed, af=%d, port=%d, errno=%d.%s", af, port, errno, strerror(errno));
        close(port_sock);
        return -1;
    }
    return port_sock;
}  

static void startACScallback(void *handle __attribute__((unused)))
{
   ACSConnection *cd = &connection;

   cmsLog_debug("openConnReqServerSocket=%d cd->lfd=%d",
                openConnReqServerSocket, cd->lfd);

   if (openConnReqServerSocket)
   {
      /* used during unittests, tr69c started without smd, needs to open its
       * own connection request server socket. */
      if (cd->lfd == CMS_INVALID_FD)
      {
      	 //wwzh modify try ipv4 first, but tr69c can't support ipv6, 
         if ((cd->lfd = initSocket(TR69C_CONN_REQ_PORT,AF_INET)) == CMS_INVALID_FD/* try ipv6 first */
            && (cd->lfd = initSocket(TR69C_CONN_REQ_PORT,AF_INET6)) == CMS_INVALID_FD)/* if ipv6 failed, try ipv4 */
         {
            /* the original code had this:
             * cmsTmr_set(&timers, startACScallback, NULL, 5000 );  retry init_socket in 5 sec
             * But why should initSocket fail anyways?
             * Take it out of this code path because this is for unittests only.
             * If there really is a problem, we should root cause it (or at least
             * apply this hack to smd, which is responsible for opening the
             * server socket.)
             */
            cmsLog_error(TR069_ALARM_SOCKET_ERROR,"could not open server socket");
         }
         else
         {
            cmsLog_debug("ACS connection req server socket opened at %d", cd->lfd);
         }
      }
   }
   else
   {
      /* typically, I will inherit my server socket from smd */
      cd->lfd = CMS_DYNAMIC_LAUNCH_SERVER_FD;
      cmsLog_debug("ACS connection req server socket inherited at %d", cd->lfd);
   }

   if (cd->lfd != CMS_INVALID_FD)
   {
      setListener(cd->lfd, acsConnect, cd);
   }

}  /* End of startACScallback() */


/* 
* Listen for connections from the ACS
*/
void startACSListener(void)
{
   cmsLog_debug("=====>ENTER");

   memset(&connection,0, sizeof(struct ACSConnection));
   connection.lfd = CMS_INVALID_FD;

   startACScallback(NULL);
}

void handleConnectionRequest(void)
{
   /* avoid race condition between periodic inform and connection request inform */
   resetPeriodicInform(acsState.informInterval);                  

   addInformEventToList(INFORM_EVENT_CONNECTION_REQUEST);

   /* connection request should be handled right away.
    * When test with Cisco ACS simulator which is not always in the mode
    * to accept inform from CPE.  CPE is sitting in session retry algorithm
    * when the retry interval is getting bigger and bigger.
    * cmsTmr_set doesn't allow same routine being added.
    * So, we should cancel the current retry and execute this one right away.
    */
   cmsTmr_cancel(tmrHandle, sendInform, NULL);

   cmsTmr_set(tmrHandle, sendInform, NULL, 0, "acs_conn_sendInform");
}
