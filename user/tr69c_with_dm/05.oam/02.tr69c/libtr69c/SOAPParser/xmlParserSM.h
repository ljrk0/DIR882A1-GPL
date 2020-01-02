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
 * File Name  : xmlParserSM.h
 *
 * Description: xmlParser state machine 
 *   
 *  
 * $Revision: 1.4 $
 * $Id: xmlParserSM.h,v 1.4 2005/05/12 19:34:31 dmounday Exp $
 *----------------------------------------------------------------------*/
/* *
 * The callbacks are only called if they are found in the correct state
 * as defined by the XmlNodeDesc tables below. 
 * Each node contains a tag and attribute list that is valid for the node.
 * Associated with
 * each tag or attribute name is the function pointer. Functions associated
 * with tag names are called on the start of the tag with a ttype of TAGBEGIN and
 * when the tag end is found with a ttype of TAGEND. Data found in between the
 * state and end of a tag causes a callback with ttype of TAGDATA.
 * Functions associated with attributes are called when the value is determined.
 * If the XML structure is syntacly valid then unknown tags are quitely parsed
 * and ignored. 
 * The scanSink function variable in parseState may be set to cause the callbacks
 * to write the parsed XML to a file or else where for processing. This parsed
 * XML has blanks removed and the line breaks altered.  The nodeFlags must also be 
 * set to XML_SCAN_F in order for the scanSink function to be called. A typical
 * use of this function is to enable the sink function in the TAGBEGIN and disable
 * it in the TAGEND function. All of the XML within the TAG will be written 
 * to the sink function.
 * 
 * The parse_error routine function variable defined in parseState is called
 * when there are XML structure errors or unknow attributes within a know tag.
 * Unknow tags and their nested tags and attributes are ignored.
* Example fragment of xml tree definition:
*
*       XmlNodeDesc mgmtDesc[] = {
*           {"response", setResponse,&responseDesc},
*            {"version", setEscVersion, NULL},
*            {"gwapp", NULL,NULL},
*            {NULL, NULL, NULL},
*        };
*        
*        XmlNodeDesc topLevel[] = {
*            {"escmgmt", setEscMgmt, &mgmtDesc},
*            {"escconf", setEscconf, &escconfDesc}, 
*            {"system", setSystem, &systemDesc},
*            {NULL,NULL,NULL}
*        };
* The first element of the structure is the tagName or attrName, the second is a function pointer.
* The third element is NULL if the entry describes an attribute, or a pointer to the next
* level node descriptor if the entry describes a tag.
* If the parser state machine finds a tag it will call the function once at the beginning of the tag
* and once at the end. If the parserSM finds an attribute the function is called when the
* value has been collected. The function pointer may be set to NULL. 
* In the above each node is defined by a XmlNodeDesc structure array that contains an
* entry for each item at the node. For example topLevel would represent the tag escmgmt as:
*
*       <ecsmgmt version="0.1" gwapp="trx">
*           <response ....... 
*           </response>
*       </escmgmt>
* */
/* The more argument in xmlData and xmlValue indicate that more data is pending */
/* If the more flag is set the callback will be called again with data that is */
/* to be appended to the current data. Mutlitple callbacks can occur befor the */
/* more flag is reset. This is caused if a data item spans multiple write_xml calls */

#include "nanoxml.h"

/*****
static void xmlTagBegin( nxml_t handle, const char *tagName, unsigned lth);

static void xmlTagEnd ( nxml_t handle, const char *tagName, unsigned lth);
static void xmlData( nxml_t handle, const char *dataValue, unsigned lth, int more);

static void xmlAttr( nxml_t handle, const char *attrName, unsigned lth);

static void xmlValue( nxml_t handle, const char *attrValue, unsigned lth, int more);
*****/

typedef enum {
    NO_ERROR = 0,
    PARSE_ERROR
} eParseStatus;

typedef struct ParseHow {
    XmlNodeDesc *topLevel;
    NameSpace   *nameSpace;
} ParseHow;

#define BUFSIZE 4096
eParseStatus parseGeneric(char *path, char *memory, int size, ParseHow *parseHow );
