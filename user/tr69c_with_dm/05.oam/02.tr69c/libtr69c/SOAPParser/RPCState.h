#ifndef RPCSTATE_H
#define RPCSTATE_H
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
 * File Name  : RPCState.h
 *
 * Description: RPC states and data structures 
 * $Revision: 1.23 $
 * $Id: RPCState.h,v 1.23 2006/01/30 23:01:25 dmounday Exp $
 *----------------------------------------------------------------------*/
 
#include "../inc/tr69cdefs.h"
 
/* fault codes */
#define NO_FAULT    0
#define RPC_PENDING 1
#define RPC_FAULTS  9000

/* rpcRun return status */
typedef enum {
   eRPCRunOK,      /* sent RPC response to ACS */
   eRPCRunEnd,      /* sent NULL http msg to ACS */
   eRPCRunFail      /* RPC run failed no reponse to send */
               /*  this should probably send a fault */
}RunRPCStatus;


/* move informEvList to tr69cdefs.h */

typedef struct ParamItem {
   struct ParamItem   *next;
   char   *pname;
   char   *ptype; /* used during setParamValue to check the type given by ACS */
   char   *pvalue;
   char   *pOrigValue;
   int      fault;      /* 0 or set fault code */
}ParamItem;

typedef struct AttributeItem {
   struct AttributeItem *next;
   char   *pname;
   int    notification;
   int    chgNotify;
   int    chgAccess;
   int    subAccess;   /* need to add list here if spec changes or vendor reqmts */
} AttributeItem;

typedef struct ParamNamesReq {
   char    *parameterPath;
   int     nextLevel;
} ParamNamesReq;

typedef struct AddDelObject {
   char    *objectName;    /* For Add the object name is xxx.yyy. */
                            /* for Delete the object anme is xxx.yyy.i. */
                            /* where i is the instance number */
} AddDelObject;

typedef struct FaultDetailMsg {
    int code;
    char *string;
}FaultDetailMsg;

typedef struct FaultMsg {
    char    *code;
    char    *string;
    FaultDetailMsg detail;
} FaultMsg;

typedef struct RPCAction {
   char   *ID;    /* pointer to ID string */
   char    *informID;  /* ID string sent with last inform */
   eRPCMethods rpcMethod;
   int     arrayItemCnt;   /* cnt of items in parameter list-not used */
   char    *commandKey;    /* */
   char    *parameterKey;  /* for setParameterValue key */
   char *vendorRpcName;    /* if rpcMethod is rpcVendorSpecific, the name of the RPC is recorded */
   union {
        ParamItem       *pItem;
        AttributeItem   *aItem;
        ParamNamesReq   paramNamesReq;
        AddDelObject    addDelObjectReq;
        DownloadReq     downloadReq;
        ChangeDuStateOp *opItem;
        FaultMsg        fault;
        /* more items here later for each rpc method*/
   } ud;
} RPCAction;

/* structures to save notification entries */
typedef struct AttEntry {
   short int   nodeIndex;         /* index in CPE param table */
   short int   attrValue;         /* attribute value (1..2)  0 is not saved*/
   int         instanceId;         /* Id of instance or zero */
} AttEntry;

typedef struct AttSaveBuf {
   short int   sigValue;
   short int   numAttSaved;      /* number of notification attributes saved*/
   AttEntry    attEntry[];
} AttSaveBuf;


RPCAction* newRPCAction(void);
void freeRPCAction(RPCAction *item);

void dumpAcsState(void);
void dumpRpcAction(RPCAction *);
void buildInform(RPCAction *a, InformEvList *);
void updateKeys( RPCAction *a);
RunRPCStatus runRPC(void);
int  checkActiveNotifications(void);
void resetNotification(void);
void saveConfigurations(void);
UBOOL8 rebootCompletion(void);
UBOOL8 factoryResetCompletion(void);
void sendTransferComplete(void);
void sendGetRPCMethods(void);
void initTransferList(void);
int isTransferInProgress(void);
int requestQueued(DownloadReq *r, eRPCMethods method);
void transferListDequeueFree(DownloadReq *q);
void transferListEnqueueCopy(DownloadReq *q, DownloadReq *r);
void updateTransferState(char *commandKey, eTransferState state);
void setsaveConfigFlag(UBOOL8 flag);
void goSendAutonomousDuStateComplete(void);
void cleanupAcsDuReqList(void);
void sendDuStateComplete(void);
void sendAutonDuStateComplete(void);
void allResponseReceived(void);

#endif /*  RPCSTATE_H */
