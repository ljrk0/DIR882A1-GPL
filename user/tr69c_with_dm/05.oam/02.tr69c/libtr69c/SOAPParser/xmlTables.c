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
 * File Name  : xmlTables.c
 *
 * Description: SOAP xmlTables and data structures 
 * $Revision: 1.17 $
 * $Id: xmlTables.c,v 1.17 2006/01/25 17:02:00 dmounday Exp $
 *----------------------------------------------------------------------*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <syslog.h>

#include "cms_util.h"
#include "../inc/tr69cdefs.h"   /* defines for RPCAction and ACSState */
#include "../inc/utils.h"
#include "nanoxml.h"
#include "RPCState.h"
#include "xmlTables.h"
#include "../main/informer.h"

extern RPCAction   *rpcAction;
extern ACSState acsState;
#ifdef SUPPORT_TR69C_VENDOR_RPC
#include "RPCState_public.h"
extern void freeVendorRPCAction(VendorRPCAction* pRpcAction);
extern VendorRPCAction *pVendorRpcAction;
extern VendorSpecificRPCItem *vendorSpecificRPCList;
VendorRPCAction* newVendorRPCAction(void);
#endif

/* List of namespaces recognized by this parser */
/* Prefixes are set by each message */
/* !!!!! Notice change MACROS if this table is changed */
/* Namespace version NOTE: 
 * TR69a3, section 3.5 states that highest namespace version (dslforum-org:cwmp-1-2) 
 * should be used if we support cwmp-1-2.  However, for compatibility with some older ACS, we may have to use
 * cwmp-1-0.   The cwmp version will be modified with the following according to these rules: 
 * 1. default cwmp-1-2.  User can choose cwmp-1-0 via WEBUI, this will be the only version used by tr69c.
 * 2. If user does not specify cwmp-1-0, then tr69c uses cwmp-1-2 for 5 retries count.
 *    Tr69c will revert to cwmp-1-0 assuming that ACS only understands cwmp-1-0 (TR69 section 3.2.1.1).
 *    We cannot predict the ACS's behavior, so if fault is received, fall back.  If post error occurs after 3 retries, fall back
 */
static char cwmp_url_str_v1_0[] = "urn:dslforum-org:cwmp-1-0";
static char cwmp_url_str_v1_1[] = "urn:dslforum-org:cwmp-1-1";
static char cwmp_url_str_v1_2[] = "urn:dslforum-org:cwmp-1-2";
static char cwmp_url_str_v1_3[] = "urn:dslforum-org:cwmp-1-3";

NameSpace nameSpaces[] = {
    {NULL, "SOAP-ENV:", "http://schemas.xmlsoap.org/soap/envelope/"},
    {NULL, "SOAP-ENC:", "http://schemas.xmlsoap.org/soap/encoding/"},
    {NULL, "xsd:",      "http://www.w3.org/2001/XMLSchema"}, 
    {NULL, "xsi:",      "http://www.w3.org/2001/XMLSchema-instance"},
    {NULL, "cwmp:",     cwmp_url_str_v1_2},
    {NULL, NULL,NULL}
};

/* MACROS for referencing the above namespace */
/* strings from xml node description tables   */
/* must match initializations in xmlTables */
#define iSOAP       &nameSpaces[0]
#define iSOAP_ENC   &nameSpaces[1]
#define iXSD        &nameSpaces[2]
#define iXSI        &nameSpaces[3]
#define iCWMP       &nameSpaces[4]

/* */
/* Node descriptors for SOAP Header */
static XmlNodeDesc mustDesc[] = {
    {iSOAP, "mustUnderstand",NULL,NULL},
    {iSOAP, "actor", NULL,NULL}, /* not sure about this-- ignore attr value */
    {iXSI,	"type", NULL,NULL},
    {NULL, NULL, NULL, NULL}
};

XMLFUNC(fIDValue);
XMLFUNC(fHoldRequest);
XMLFUNC(fNoMoreRequests);
static XmlNodeDesc headerDesc[] = {
    {iCWMP, "ID", fIDValue, mustDesc},               /* data value */
    {iCWMP, "HoldRequests",fHoldRequest, mustDesc}, /* data value */
    {iCWMP, "NoMoreRequests", fNoMoreRequests,NULL}, /* data value flag */
    {NULL, NULL, NULL, NULL}
};

/* Node descriptors for body */

/* fault codes from ACS */
XMLFUNC(fFaultDetailFaultCode);
XMLFUNC(fFaultDetailFaultString);
static XmlNodeDesc faultDetailFaultDesc[] = {
    {NULL,  "FaultCode", fFaultDetailFaultCode, NULL},               /* data value */
    {NULL,  "FaultString",fFaultDetailFaultString, NULL}, /* data value */
    {NULL, NULL, NULL, NULL}
};

XMLFUNC(fFaultDetailFault);
static XmlNodeDesc faultDetailDesc[] = {
    {NULL,  "Fault", fFaultDetailFault, faultDetailFaultDesc},
    {NULL, NULL, NULL, NULL}
};

XMLFUNC(fFaultCode);
XMLFUNC(fFaultString);
XMLFUNC(fFaultDetail);
static XmlNodeDesc faultDesc[] = {
    {NULL,  "faultcode", fFaultCode, NULL},               /* data value */
    {NULL,  "faultstring",fFaultString, NULL}, /* data value */
    {NULL,  "detail",fFaultDetail, faultDetailDesc},
    {NULL, NULL, NULL, NULL}
};

/* <cmsp:Reboot>
*/
XMLFUNC(fMaxEnvelopes);
static XmlNodeDesc informResponseDesc[] = {
    {NULL,  "MaxEnvelopes", fMaxEnvelopes, NULL},               /*  */
    {NULL, NULL, NULL, NULL}
};
/* <cmsp:Reboot>
*/

XMLFUNC(fCommandKey);
static XmlNodeDesc rebootDesc[] = {
    {NULL,  "CommandKey", fCommandKey, NULL},               /*  */
    {NULL, NULL, NULL, NULL}
};
/* <cmwp:Download >
*/

XMLFUNC(fDownloadFileType);
XMLFUNC(fDLCommandKey);
XMLFUNC(fDownloadURL);
XMLFUNC(fDownloadUser);
XMLFUNC(fDownloadPwd);
XMLFUNC(fDownloadFileSize);
XMLFUNC(fTargetFileName);
XMLFUNC(fDownloadDelaySec);
static XmlNodeDesc downLoadDesc[] = {
    {NULL,  "CommandKey", fDLCommandKey,NULL},
    {NULL,  "FileType", fDownloadFileType,NULL},
    {NULL,  "URL", fDownloadURL,NULL},
    {NULL,  "Username", fDownloadUser,NULL},
    {NULL,  "Password", fDownloadPwd,NULL},
    {NULL,  "FileSize", fDownloadFileSize,NULL},
    {NULL,  "TargetFileName", fTargetFileName,NULL},
    {NULL,  "DelaySeconds", fDownloadDelaySec,NULL},
    {NULL,  "SuccessURL", NULL,NULL},
    {NULL,  "FailureURL", NULL,NULL},
    {NULL, NULL, NULL, NULL}
};
/* <cmwp:Upload >
*/

XMLFUNC(fUploadFileType);
static XmlNodeDesc upLoadDesc[] = {
    {NULL,  "CommandKey", fDLCommandKey,NULL},
    {NULL,  "FileType", fUploadFileType,NULL},
    {NULL,  "URL", fDownloadURL,NULL},
    {NULL,  "Username", fDownloadUser,NULL},
    {NULL,  "Password", fDownloadPwd,NULL},
    {NULL,  "FileSize", fDownloadFileSize,NULL},
    {NULL,  "TargetFileName", fTargetFileName,NULL},
    {NULL,  "DelaySeconds", fDownloadDelaySec,NULL},
    {NULL,  "SuccessURL", NULL,NULL},
    {NULL,  "FailureURL", NULL,NULL},
    {NULL, NULL, NULL, NULL}
};
/* <cwmp:DeleteObject> 
*  <cwmp:AddObject>
*/

XMLFUNC(fObjectName);
XMLFUNC(fSetParameterKey);
static XmlNodeDesc addDelObjectDesc[] = {
    {NULL,  "ObjectName", fObjectName,NULL},
    {NULL,  "ParameterKey", fSetParameterKey, NULL},
    {NULL, NULL, NULL, NULL}
};

/* <cwmp:GetParameterValues ..../> */
/*
 * NOTE: fParameterNames is shared by multiple nodes.
 *       arrayType is used generically for determining array size.
* <GetParameterValues>
 *  <ParameterNames arrayType="xsd:string[x]>
 *      <xsd:string>name1</xsd:string>
 *      .....
 *  </ParameterNames>
 * </GetParameterValues>
 */
XMLFUNC(fParameterNameArraySize);
XMLFUNC(fParameterNames);
XMLFUNC(fGetParameterValues);
static XmlNodeDesc pNamesDesc[] = {
    {iSOAP, "arrayType", fParameterNameArraySize,NULL},
    {iXSD,  "string", fParameterNames,NULL},    /* namevalue is TAGDATA */
    {iCWMP, "item", fParameterNames,NULL},    /* namevalue is TAGDATA -- added for dimark */
    {iCWMP, "ParameterNames", fParameterNames,NULL},    /* namevalue is TAGDATA -- added for dimark */
    {NULL, NULL, NULL, NULL}
};
static XmlNodeDesc getParameterValuesDesc[] = {
    {NULL,  "ParameterNames", NULL,pNamesDesc},
    {NULL, NULL, NULL, NULL}
};

/* <SetParameterValue>
*   <ParameterList <ParameterValueStruct>
*                       <Name>xxxx</Name> <Value>yyyy</value>
*   </ParamterList>
*   <ParameterKey>kkkkk</ParameterKey>
*/
XMLFUNC(fParameterValueType );
XMLFUNC(fParameterValue );
XMLFUNC(fSetParameterValues );

static XmlNodeDesc valueDesc[] = {
    {iXSD, "type", fParameterValueType, NULL},
    {NULL, NULL, NULL, NULL}
};
static XmlNodeDesc parameterValueStructDesc[] = {
    {NULL, "Name", fParameterNames, NULL},
    {NULL, "Value", fParameterValue, valueDesc},
    {NULL, NULL, NULL, NULL}
};


/* <SetParameterAttributes>
*  <ParameterList <SetParameterAttributesStruct>
*               <Name>xxx</Name>  ....
*/

XMLFUNC(fSetAttParameterName );
XMLFUNC(fNotification);
XMLFUNC(fNotificationChange );
XMLFUNC(fAcclistChange);
XMLFUNC(fAccessList);
static XmlNodeDesc accessListDesc[] = {
    {NULL, "string", fAccessList, NULL},	/* only subscriber defined */
    {NULL, "arrayType", NULL,NULL},
    {NULL, NULL, NULL, NULL}
};
static XmlNodeDesc setparameterAttriStructDesc[] = {
    {NULL, "Name", fSetAttParameterName, NULL},
    {NULL, "Notification", fNotification, NULL},
    {NULL, "NotificationChange", fNotificationChange, NULL},
    {NULL, "AccessListChange", fAcclistChange, NULL},
    {NULL, "AccessList", NULL, accessListDesc},
    {NULL, NULL, NULL, NULL}
};

XMLFUNC(fSetParameterAttributesList);
/* this is being used by multiple complex types -- will not detect mixed element errors*/
static XmlNodeDesc parameterListDesc[] = {
    {iSOAP, "arrayType", fParameterNameArraySize, NULL},
    {NULL, "ParameterValueStruct",NULL,parameterValueStructDesc},
    {NULL, "SetParameterAttributesStruct",fSetParameterAttributesList,setparameterAttriStructDesc},
    {NULL, NULL, NULL, NULL}
};
static XmlNodeDesc setParameterValuesDesc[] = {
    {NULL, "ParameterList", NULL, parameterListDesc},
    {NULL, "ParameterKey", fSetParameterKey, NULL},
    {NULL, NULL, NULL, NULL}
};
/*
*/
static XmlNodeDesc setParameterAttributesDesc[] = {
    {NULL, "ParameterList", NULL, parameterListDesc},
    {NULL, "ParameterKey", fSetParameterKey, NULL},
    {NULL, NULL, NULL, NULL}
};

XMLFUNC(fDelaySeconds);
static XmlNodeDesc scheduleInformDesc[] = {
    {NULL, "DelaySeconds", fDelaySeconds,NULL},
    {NULL, "CommandKey", fCommandKey, NULL},
    {NULL, NULL, NULL, NULL}
};
/*
*/
static XmlNodeDesc getParameterAttributesDesc[] = {
    {NULL, "ParameterNames", NULL, pNamesDesc},
    {NULL, NULL, NULL, NULL}
};

/*
*/
XMLFUNC(fParameterPath);
XMLFUNC( fNextLevel);
static XmlNodeDesc getParameterNamesDesc[] = {
    {NULL,  "ParameterPath", fParameterPath, NULL},               /* data value */
    {NULL,  "NextLevel",fNextLevel, NULL}, /* data value */
    {NULL, NULL, NULL, NULL}
};



/* <ChangeDuState>
*   <Operation <OperationStruct>
*                       <Name>xxxx</Name> <Value>yyyy</value>
*   </Operation>
*   <ParameterKey>kkkkk</ParameterKey>
*/
XMLFUNC(fSetDuOperation);
XMLFUNC(fDuUrl);
XMLFUNC(fDuUuid);
XMLFUNC(fDuUser);
XMLFUNC(fDuPwd);
XMLFUNC(fDuExecEnvRef);
XMLFUNC(fDuVersion);

static XmlNodeDesc duOperationStructDesc[] = {
    {iXSI, "type", fSetDuOperation, NULL},  /* attribute */
    {NULL, "URL", fDuUrl, NULL},
    {NULL, "UUID", fDuUuid, NULL},
    {NULL, "Username", fDuUser, NULL},
    {NULL, "Password", fDuPwd, NULL},
    {NULL, "ExecutionEnvRef", fDuExecEnvRef, NULL},
    {NULL, "Version", fDuVersion, NULL}, /* update */
    {NULL,NULL,NULL,NULL}
};

static XmlNodeDesc changeDuStateDesc[] = {
    {iCWMP, "Operations", NULL, duOperationStructDesc}, /* tag */
    {NULL, "CommandKey", fCommandKey, NULL},
    {NULL,NULL,NULL,NULL}
};

XMLFUNC(fFault);
XMLFUNC(fGetRPCMethods);
XMLFUNC(fScheduleInform);
XMLFUNC(fGetParameterNames);
XMLFUNC(fSetParameterAttributes);
XMLFUNC(fGetParameterAttributes);
XMLFUNC(fAddObject);
XMLFUNC(fDeleteObject);
XMLFUNC(fDownload);
XMLFUNC(fUpload);
XMLFUNC(fGetQueuedTransfers);
XMLFUNC(fReboot);
XMLFUNC(fInformResponse);
XMLFUNC(fFactoryReset);
XMLFUNC(fTransferCompleteResponse);
XMLFUNC(fGetRPCMethodsResponse);
XMLFUNC(fUnsupportedRPCMethod);
XMLFUNC(fDuStateChange);
XMLFUNC(fAutonomousTransferCompleteResponse);
XMLFUNC(fAutonomousDUStateChangeCompleteResponse);
#ifdef SUPPORT_TR69C_VENDOR_RPC 
XMLFUNC(fVendorSpecific);
#endif
static XmlNodeDesc bodyDesc[] = {
    {iSOAP, "Fault", fFault, faultDesc}, 
    {iCWMP, "GetRPCMethods", fGetRPCMethods, NULL},
    {iCWMP, "ScheduleInform", fScheduleInform, scheduleInformDesc},
    {iCWMP, "GetParameterNames", fGetParameterNames, getParameterNamesDesc},
    {iCWMP, "GetParameterValues", fGetParameterValues, getParameterValuesDesc},
    {iCWMP, "SetParameterValues", fSetParameterValues, setParameterValuesDesc},
    {iCWMP, "SetParameterAttributes", fSetParameterAttributes, setParameterAttributesDesc},
    {iCWMP, "GetParameterAttributes", fGetParameterAttributes, getParameterAttributesDesc},
    {iCWMP, "AddObject", fAddObject, addDelObjectDesc},
    {iCWMP, "DeleteObject", fDeleteObject, addDelObjectDesc},
    {iCWMP, "Download", fDownload, downLoadDesc},
    {iCWMP, "Upload", fUpload, upLoadDesc},
    {iCWMP, "GetQueuedTransfers", fGetQueuedTransfers, NULL},
    {iCWMP, "Reboot", fReboot, rebootDesc},
    {iCWMP, "InformResponse", fInformResponse, informResponseDesc},
    {iCWMP, "FactoryReset", fFactoryReset, NULL},
    {iCWMP, "TransferCompleteResponse", fTransferCompleteResponse, faultDesc},
    {iCWMP, "GetRPCMethodsResponse", fGetRPCMethodsResponse, NULL},	/* ignore content*/
    {iCWMP, "SetVouchers", fUnsupportedRPCMethod, NULL},
    {iCWMP, "GetOptions", fUnsupportedRPCMethod, NULL},
    {iCWMP, "GetAllQueuedTransfers", fUnsupportedRPCMethod, NULL},
    {iCWMP, "ScheduleDownload", fUnsupportedRPCMethod, NULL},
    {iCWMP, "CancelTransfer", fUnsupportedRPCMethod, NULL},
    {iCWMP, "ChangeDUState", fDuStateChange, changeDuStateDesc},
    {iCWMP, "AutonomousTransferCompleteResponse", fAutonomousTransferCompleteResponse, NULL},   /* ignore content*/
    {iCWMP, "AutonomousDUStateChangeCompleteResponse", fAutonomousDUStateChangeCompleteResponse, NULL},   /* ignore content*/
#ifdef SUPPORT_TR69C_VENDOR_RPC 
    {iCWMP, "X_", fVendorSpecific, NULL},
#endif
    {NULL, NULL, NULL, NULL}
};

//XMLFUNC(fHeader);
XMLFUNC(fBody);
static XmlNodeDesc envDesc[] = {
    {iSOAP, "Header", NULL, headerDesc},
    {iSOAP, "Body", fBody, bodyDesc},
    {NULL, NULL, NULL, NULL}
};
/* TopLevel node for a SOAP envelope */
XMLFUNC(fEnvelope);
XmlNodeDesc envelopeDesc[] = {
    {iNULL,"Envelope",fEnvelope, envDesc}, /* -1 is namespace exception flag*/
    {NULL, NULL, NULL, NULL}
};

static XML_STATUS fFaultDetailFaultString(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA) {
        rpcAction->ud.fault.detail.string = cmsMem_strdup(value);
    }
    else {
    }
    return XML_STS_OK;
}
static XML_STATUS fFaultDetailFaultCode(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA) {
        rpcAction->ud.fault.detail.code = atoi(value);
    }
    else {
    }
    return XML_STS_OK;
}
static XML_STATUS fFaultDetailFault(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN) {
    }
    else {
    }
    return XML_STS_OK;
}
static XML_STATUS fFaultDetail(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN) {
    }
    else {
    }
    return XML_STS_OK;
}
static XML_STATUS fFaultString(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA) {
        rpcAction->ud.fault.string = cmsMem_strdup(value);
    }
    else {
    }
    return XML_STS_OK;
}
static XML_STATUS fFaultCode(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA) {
        rpcAction->ud.fault.code = cmsMem_strdup(value);
    }
    else {
    }
    return XML_STS_OK;
}
static XML_STATUS fFault(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN) {
        rpcAction->rpcMethod = rpcFault;
    }
    else {
    }
    return XML_STS_OK;
}
/* <Download>
*/

static XML_STATUS fDownload(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN)
        rpcAction->rpcMethod = rpcDownload;
    return XML_STS_OK;
}
static XML_STATUS fDLCommandKey(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA){
        rpcAction->ud.downloadReq.commandKey = cmsMem_strdup(value);
    }
    return XML_STS_OK;
}
static XML_STATUS fDownloadFileType(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
    {
        if (value[0] == 'X')
        {
            rpcAction->ud.downloadReq.efileType = cmsUtl_getVendorFiletype(value);
        }
        else
        {
            rpcAction->ud.downloadReq.efileType = atoi(value);
        }
    }
    return XML_STS_OK;
}
static XML_STATUS fDownloadURL(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
    {
        char *escapedStr = NULL;

        if (cmsXml_isUnescapeNeeded(value))
        {
           cmsXml_unescapeString(value, &escapedStr);
        }
        else
        {
           escapedStr = cmsMem_strdup(value);
        }

        rpcAction->ud.downloadReq.url = escapedStr;
    }
    return XML_STS_OK;
}
static XML_STATUS fDownloadUser(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
        rpcAction->ud.downloadReq.user = cmsMem_strdup(value);
    return XML_STS_OK;
}
static XML_STATUS fDownloadPwd(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
        rpcAction->ud.downloadReq.pwd = cmsMem_strdup(value);
    return XML_STS_OK;
}
static XML_STATUS fDownloadFileSize(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
        rpcAction->ud.downloadReq.fileSize = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fTargetFileName(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
        rpcAction->ud.downloadReq.fileName = cmsMem_strdup(value);
    return XML_STS_OK;
}
static XML_STATUS fDownloadDelaySec(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
        rpcAction->ud.downloadReq.delaySec = atoi(value);
    return XML_STS_OK;
}

/* <Upload>
*/

static XML_STATUS fUpload(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN)
        rpcAction->rpcMethod = rpcUpload;
    return XML_STS_OK;
}
static XML_STATUS fUploadFileType(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
    {
        if (value[0] == 'X')
        {
            rpcAction->ud.downloadReq.efileType = cmsUtl_getVendorFiletype(value);
        }
        else
        {
            int type = atoi(value);
            if ( type == 1 || type == 3 )
                rpcAction->ud.downloadReq.efileType = eVendorConfig;
            else if ( type == 2 || type == 4 )
                rpcAction->ud.downloadReq.efileType = eVendorLog;
        }
    }
    return XML_STS_OK;
}

/* <xxx:InformResponse>
*       <MaxEnvelopes>xx</MaxEnvelopes>
* Assumes that we only get this in an InformResponse msg.
*/

static XML_STATUS fMaxEnvelopes(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
        acsState.maxEnvelopes= atoi(value);
    return XML_STS_OK;
}
/* <Reboot>
*/

static XML_STATUS fCommandKey(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA){
        rpcAction->commandKey = cmsMem_strdup(value);
    }
    return XML_STS_OK;
}
static XML_STATUS fReboot(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN)
        rpcAction->rpcMethod = rpcReboot;
    return XML_STS_OK;
}
/* 
* <FactoryReset/> 
*/
static XML_STATUS fFactoryReset(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN)
        rpcAction->rpcMethod = rpcFactoryReset;
    return XML_STS_OK;
}

static XML_STATUS fInformResponse(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN){
        rpcAction->rpcMethod = rpcInformResponse;
        informState = eACSInformed;
    }
    return XML_STS_OK;
}
static XML_STATUS fTransferCompleteResponse(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN){
        rpcAction->rpcMethod = rpcTransferCompleteResponse;
    }
    return XML_STS_OK;
}
/* <AddObject>
*  <DeleteObject>
*
*/

static XML_STATUS fObjectName(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction){
        rpcAction->ud.addDelObjectReq.objectName = cmsMem_strdup(value);
    }
    return XML_STS_OK;
}

static XML_STATUS fAddObject(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction){
        rpcAction->rpcMethod = rpcAddObject;
    }
    return XML_STS_OK;
}
static XML_STATUS fDeleteObject(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction){
        rpcAction->rpcMethod = rpcDeleteObject;
    }
    return XML_STS_OK;
}


/*
 * Some customers want to test the case where ACS sends the wrong type
 * to our CPE.  So we need to store and pass up the type that was sent by the
 * ACS for later comparison with our MDM's type for this parameter.
 */
static XML_STATUS fParameterValueType(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);

    // value is xsd:string, but we only want the part after "xsd:"
    if (ttype == ATTRIBUTEVALUE && rpcAction){
        if(rpcAction->ud.pItem && (3 <= cmsUtl_strlen(value))) {
            char *s = NULL;
            s = strchr(value, ':');
            if (s) {
                rpcAction->ud.pItem->ptype = cmsMem_strdup(s+1);
            }
        }
    }
    return XML_STS_OK;
}

/* <SetParameterValue>
*   <ParameterList <ParameterValueStruct>
*                       <Name>xxxx</Name> <Value>yyyy</value>
*   </ParamterList>
*   <ParameterKey>kkkkk</ParameterKey>
*/
static XML_STATUS fParameterValue(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction){
        if(rpcAction->ud.pItem) {
            char *escapedStr = NULL;

            if (cmsXml_isUnescapeNeeded(value))
            {
               cmsXml_unescapeString(value, &escapedStr);
            }
            else
            {
               escapedStr = cmsMem_strdup(value);
            }

            rpcAction->ud.pItem->pvalue = escapedStr;
        }
    }
    return XML_STS_OK;
}
static XML_STATUS fSetParameterKey(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction){
        rpcAction->parameterKey = cmsMem_strdup(value);
    }
    return XML_STS_OK;
}
static XML_STATUS fSetParameterValues(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction){
        rpcAction->rpcMethod = rpcSetParameterValues;
    }
    return XML_STS_OK;
}
/* <ns:GetParameterValues ....../>
*
*/
static XML_STATUS fParameterNameArraySize(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
	
    #if !defined(SUPPRESS_SOAP_ARRAYTYPE)  /* use element cnt */
    if (ttype == ATTRIBUTEVALUE && rpcAction){
        char *s;
        s = strchr(value,'[');
        if (s)
            rpcAction->arrayItemCnt = atoi(s+1);
    }
	#endif
    return XML_STS_OK;
}

static XML_STATUS fParameterNames(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction){
        ParamItem *p = (ParamItem *)cmsMem_alloc(sizeof(ParamItem), ALLOC_ZEROIZE);
        if (p) {
			#ifdef SUPPRESS_SOAP_ARRAYTYPE
            ++rpcAction->arrayItemCnt;
			#endif
            p->next = rpcAction->ud.pItem;
            rpcAction->ud.pItem = p;
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    } else if (ttype==TAGDATA) {
        ParamItem *p = rpcAction->ud.pItem;
        p->pname = cmsMem_strdup(value);

    }
    return XML_STS_OK;
}

static XML_STATUS fGetParameterValues(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction)
        rpcAction->rpcMethod = rpcGetParameterValues;
    return XML_STS_OK;
}

/* <SetParameterAttributes>
*  <ParameterList <SetParameterAttributesStruct>
*               <Name>xxx</Name>  ....
*/
static XML_STATUS fAccessList(const char *name, TOKEN_TYPE ttype, const char *value)
{   /* there's only one access list item today */
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction){
        if (rpcAction->ud.aItem) {
            rpcAction->ud.aItem->subAccess= !strcasecmp(value,"subscriber");
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fAcclistChange(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction){
        if (rpcAction->ud.aItem) {
            rpcAction->ud.aItem->chgAccess= testBoolean(value);
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fNotificationChange(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction){
        if (rpcAction->ud.aItem) {
            rpcAction->ud.aItem->chgNotify= testBoolean(value);
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fNotification(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction){
        if (rpcAction->ud.aItem) {
            rpcAction->ud.aItem->notification = atoi(value);
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fSetAttParameterName(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction){
        if (rpcAction->ud.aItem) {
            rpcAction->ud.aItem->pname = cmsMem_strdup(value);
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}

static XML_STATUS fSetParameterAttributesList(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction){
        AttributeItem *p = (AttributeItem *)cmsMem_alloc(sizeof (AttributeItem), ALLOC_ZEROIZE);
        if (p) {
            #ifdef SUPPRESS_SOAP_ARRAYTYPE
			++rpcAction->arrayItemCnt;
			#endif
            p->next = rpcAction->ud.aItem;
            rpcAction->ud.aItem = p;
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}

static XML_STATUS fSetParameterAttributes(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction){
        rpcAction->rpcMethod = rpcSetParameterAttributes;
    }
    return XML_STS_OK;
}
/* <GetParameterAttributes>
*  <ParameterNames SOAP-ENC:arrayType="xsd:string[xx]"
*               <xsd:string>xxx</xsd:string>  ....
*/
static XML_STATUS fGetParameterAttributes(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction){
        rpcAction->rpcMethod = rpcGetParameterAttributes;
    }
    return XML_STS_OK;
}


/* <ns:GetParameterNames>
*     <ParameterPath>parameterpath</ParameterPath>
*/
static XML_STATUS fGetParameterNames(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction)
        rpcAction->rpcMethod = rpcGetParameterNames;
    return XML_STS_OK;
}
static XML_STATUS fParameterPath(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction)
        rpcAction->ud.paramNamesReq.parameterPath = cmsMem_strdup(value);

    return XML_STS_OK;
}
static XML_STATUS fNextLevel(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction)
        rpcAction->ud.paramNamesReq.nextLevel = testBoolean(value);
    return XML_STS_OK;
}
/* <SOAP_ENV:Body ..../> */

static XML_STATUS fDelaySeconds(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA && rpcAction)
    {
        ParamItem *p = (ParamItem *)cmsMem_alloc(sizeof (ParamItem), ALLOC_ZEROIZE);
        if (p)
        {
            #ifdef SUPPRESS_SOAP_ARRAYTYPE
                ++rpcAction->arrayItemCnt;
            #endif
            p->next = rpcAction->ud.pItem;
            rpcAction->ud.pItem = p;

            p->pname = cmsMem_strdup(name);
            p->pvalue = cmsMem_strdup(value);
        }
    }
    return XML_STS_OK;
}

static XML_STATUS fGetRPCMethods(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction)
        rpcAction->rpcMethod = rpcGetRPCMethods;
    return XML_STS_OK;
}

static XML_STATUS fScheduleInform(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction)
        rpcAction->rpcMethod = rpcScheduleInform;
    return XML_STS_OK;
}

static XML_STATUS fBody(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN) {
    }
    else {
    }
    return XML_STS_OK;
}


static XML_STATUS fGetRPCMethodsResponse(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction)
        rpcAction->rpcMethod = rpcGetRPCMethodsResponse;
    return XML_STS_OK;
}

static XML_STATUS fAutonomousTransferCompleteResponse(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN){
        rpcAction->rpcMethod = rpcAutonomousTransferCompleteResponse;
    }
    return XML_STS_OK;
}

static XML_STATUS fAutonomousDUStateChangeCompleteResponse(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN){
        rpcAction->rpcMethod = rpcAutonomousDUStateChangeCompleteResponse;
    }
    return XML_STS_OK;
}

/* <SOAP-ENV:Header ..../> */
static XML_STATUS fNoMoreRequests(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
        acsState.noMoreRequests = !strcmp(value,"1");
    return XML_STS_OK;
}
static XML_STATUS fHoldRequest(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA)
        acsState.holdRequests = !strcmp(value,"1");
        
    return XML_STS_OK;
}
static XML_STATUS fIDValue(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGDATA) {
        if (rpcAction)
            rpcAction->ID = cmsMem_strdup(value);
        else
            return XML_STS_ERR;
    }
    return XML_STS_OK;
}
/* <envelope .../>   */
static XML_STATUS fEnvelope(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN)
    {
      cmsLog_debug("TAGBEGIN ------");
      if (rpcAction)
      {
         freeRPCAction(rpcAction);
#ifdef SUPPORT_TR69C_VENDOR_RPC 
         freeVendorRPCAction(pVendorRpcAction);
#endif
      }
      rpcAction = newRPCAction();
    }
    else
    {
      cmsLog_debug("TAGEND -----");
      /* end of envelope -- now run RPC */
      /* */
      #ifdef DEBUG
      dumpAcsState();
      dumpRpcAction(rpcAction);
      #endif
    }
    return XML_STS_OK;
}

static XML_STATUS fGetQueuedTransfers(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction)
       rpcAction->rpcMethod = rpcGetQueuedTransfers;
    return XML_STS_OK;
}

static XML_STATUS fUnsupportedRPCMethod(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction)
       rpcAction->rpcMethod = rpcUnknown;
    return XML_STS_OK;
}


static XML_STATUS fDuStateChange(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
    if (ttype == TAGBEGIN && rpcAction)
    {
       rpcAction->rpcMethod = rpcChangeDuState;
    }
    return XML_STS_OK;
}

ChangeDuStateOp *duAlloc(void)
{
   ChangeDuStateOp *p;

   p = (ChangeDuStateOp *)cmsMem_alloc(sizeof(ChangeDuStateOp), ALLOC_ZEROIZE);
   if (p)
   {
      if (rpcAction->ud.opItem == NULL)
      {
         /* first one */
         rpcAction->ud.opItem = p;
      }
      else
      {
         p->next = rpcAction->ud.opItem;
         rpcAction->ud.opItem = p;
      }
   }
   return(p);
}
static XML_STATUS fSetDuOperation(const char *name, TOKEN_TYPE ttype, const char *value)
{
   ChangeDuStateOp *p;
   eOperationType opType;

   cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);

   if (ttype == ATTRIBUTEVALUE && rpcAction)
   {
      if (strstr(value, "UninstallOpStruct") != NULL)
      {
         opType = eDuUninstall;
      }
      else if (strstr(value, "InstallOpStruct") != NULL)
      {
         opType = eDuInstall;
      }
      else if (strstr(value, "UpdateOpStruct") != NULL)
      {
         opType = eDuUpdate;
      }
      else
      {
         return XML_STS_ERR;
      }

      if (opType != eDuNoop)
      {
         p = duAlloc();
         if (p)
         {            
            rpcAction->ud.opItem->operationType = opType;
         }
         else
         {
            return XML_STS_ERR;
         }
      } /* opType is valid */
   } /* attribute value */
   return XML_STS_OK;
}

static XML_STATUS fDuUrl(const char *name, TOKEN_TYPE ttype, const char *value)
{
   cmsLog_debug("name %s, Ttype %d\n",name,ttype);

    if (ttype == TAGDATA)
    {
       if (value != NULL)
       {
          rpcAction->ud.opItem->url = cmsMem_strdup(value);
       }
    }
    return XML_STS_OK;
}

static XML_STATUS fDuUuid(const char *name, TOKEN_TYPE ttype, const char *value)
{
   cmsLog_debug("name %s, Ttype %d, value %s\n",name,ttype,value);

    if (ttype == TAGDATA)
    {
       if (value != NULL)
       {
          if (rpcAction->ud.opItem != NULL)
          {
             rpcAction->ud.opItem->uuid = cmsMem_strdup(value);
          }
       }
    }
    return XML_STS_OK;
}

static XML_STATUS fDuUser(const char *name, TOKEN_TYPE ttype, const char *value)
{
    cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);

    if (ttype == TAGDATA)
    {
       if (value != NULL)
       {
          if (rpcAction->ud.opItem != NULL)
          {
             rpcAction->ud.opItem->user = cmsMem_strdup(value);
          }
       }
    }
    return XML_STS_OK;
}

static XML_STATUS fDuPwd(const char *name, TOKEN_TYPE ttype, const char *value)
{
   cmsLog_debug("name %s, Ttype %d\n",name,ttype);

    if (ttype == TAGDATA)
    {
       if (value != NULL)
       {
          if (rpcAction->ud.opItem != NULL)
          {
             rpcAction->ud.opItem->pwd = cmsMem_strdup(value);
          }
       }
    }
    return XML_STS_OK;
}

static XML_STATUS fDuExecEnvRef(const char *name, TOKEN_TYPE ttype, const char *value)
{
   cmsLog_debug("name %s, Ttype %d, value %s\n",name,ttype,value);
   
   if (ttype == TAGDATA) 
   {
      if (value !=NULL)
      {
         if (rpcAction->ud.opItem != NULL)
         {
            rpcAction->ud.opItem->exeEnvRef = cmsMem_strdup(value);
         }
      }
   }
   return XML_STS_OK;
}

static XML_STATUS fDuVersion(const char *name, TOKEN_TYPE ttype, const char *value)
{
   cmsLog_debug("name %s, Ttype %d, value %s\n",name,ttype,value);
   
   if (ttype == TAGDATA)
   {
      if (value != NULL)
      {
         if (rpcAction->ud.opItem != NULL)
         {
            rpcAction->ud.opItem->version = cmsMem_strdup(value);
         }
      }
   }
   return XML_STS_OK;
}

/* this function changes the NameSpace's nsURL of cwmp */
void changeNameSpaceCwmpVersionURL(int version)
{
   NameSpace *cwmp = iCWMP;

   switch (version)
   {
   case CWMP_VERSION_1_0:
      cwmp->nsURL = cwmp_url_str_v1_0;
      break;
   case CWMP_VERSION_1_1:
      cwmp->nsURL = cwmp_url_str_v1_1;
      break;
   case CWMP_VERSION_1_2:
      cwmp->nsURL = cwmp_url_str_v1_2;
      break;
   case CWMP_VERSION_1_3:
      cwmp->nsURL = cwmp_url_str_v1_3;
      break;
   default:
      cwmp->nsURL = cwmp_url_str_v1_0;
      break;
   }
   acsState.cwmpVersion = version;

   cmsLog_debug("current namespace %s, version 0x%x\n",cwmp->nsURL,version);
}

#ifdef SUPPORT_TR69C_VENDOR_RPC 
static XmlNodeDesc *findXmlNodeByName(char *tagName)
{
   int lastNode; 
   XmlNodeDesc *pNode;

   /* last most entry is {NULL, NULL, NULL, NULL} */
   /* keep search for the vendor specific entry in bodyDesc from bottom up; last entry is NULL */
   lastNode = (sizeof(bodyDesc)/sizeof(XmlNodeDesc))-2;
   while (lastNode >= 0)
   {
      pNode = &bodyDesc[lastNode];
      if (strcmp(pNode->tagName,tagName) == 0)
      {
         return (pNode);
      }
      lastNode -= 1;
   }
   return NULL;
}

static XML_STATUS fVendorSpecific(const char *name, TOKEN_TYPE ttype, const char *value)
{
   VendorSpecificRPCItem *pRpcItem;
   XmlNodeDesc *pXmlVendorNode=NULL;
   char *ptr=NULL;
   int prefixLen=0;

   cmsLog_debug("name=%s, ttype=%d, value=%s", name, ttype, value);
   if (ttype == TAGBEGIN && rpcAction)
   {
      /* check the value to see if this vendor specific RPC has been registered.
       * We keep track of the X_* RPC and the X_RPC_Desc.
       * change bodyDesc[X_]->leafNode = X_RPC_Desc.
       */
      pRpcItem = vendorSpecificRPCList;
      while (pRpcItem != NULL)
      {
         if (strstr(name,pRpcItem->rpcName) == NULL)
         {
            pRpcItem = pRpcItem->next;
         }
         else
         {
            /* found matching registered RPC */
            /* need to assign the xmlNodeDesc to the body's leafnode, so the xmlParser continues on */
            pXmlVendorNode = findXmlNodeByName(VENDOR_RPC_PREFIX);
            if (pXmlVendorNode != NULL)
            {
               pXmlVendorNode->leafNode = pRpcItem->rpcNodeDesc;
            }
            rpcAction->rpcMethod = rpcVendorSpecific; /* need to look at vendorRpcAction */
            /* name may be prefixed with cwmp:, need to remove that */
            ptr = strchr(name,':');
            if (ptr != NULL)
            {
               /* skipping : also */
               prefixLen=(ptr-name+1);
            }

            rpcAction->vendorRpcName = cmsMem_strdup((name+prefixLen));
            pVendorRpcAction= newVendorRPCAction();
            if (pVendorRpcAction != NULL)
            {
               pVendorRpcAction->rpcName = cmsMem_strdup((name+prefixLen));
            }
            break;
         }
      } /* while */
   } /* tagbegin */
   return XML_STS_OK;
}

void getCwmpPrefix(char *prefix)
{
   strcpy(prefix,(nsCWMP));
}
#endif /* SUPPORT_TR69C_VENDOR_RPC */
