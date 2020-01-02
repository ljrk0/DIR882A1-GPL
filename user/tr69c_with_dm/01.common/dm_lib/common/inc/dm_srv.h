/*******************************************************************************
*
*   Copyright (c) 2009-2015 Broadcom Corporation
*   All rights reserved
*
*   FILE NAME     : dm_srv.h
*
*   PROGRAMMER(S) :  
*
*   DATE CREATED  :  24-Sep-2013
*
*   DESCRIPTION   :  Interface only for dmsrv process
*
*   CAUTIONS      :  none
*
*******************************************************************************/

#ifndef _DM_SRV_INTERFACE_H_
#define _DM_SRV_INTERFACE_H_
#ifdef __cplusplus
    extern "C" {
#endif

#include "dm_interface.h"

/**
* initialize DM module. Initialize dm lib. 
*   called by dmsrv
*
* @param encryptFunc encrypt/decrypt callback
*
* @return DM_SUCCESS or DM_FAILURE.
*/
INT32 DmServiceStartReq(DM_DBFILE_ENCRYPT_FUNC encryptFunc);

/**
* Release relative resources. 
* Save xml files for server type.
*   called by dmsrv
*
* @return DM_SUCCESS or DM_FAILURE.
*
*/
INT32 DmServiceStopReq();

/**
* Check pid file 
* Check/create PID file, return TRUE if already running
*   called by dmsrv
*
* @param[in] isServer  caller is server
* @retval DM_TRUE   Dmsrv PID file has been created
* @retval DM_FALSE  Dmsrv PID file has not been created
*
*/
BOOL8 DmSrvCheckPidFile(BOOL8 isServer);

/**
* Delete/truncate the PID file
*   called by dmsrv
*/
VOID DmSrvUnlinkPidFile();

/**
*  notify service state, called by dmsrv
*
* @param[in] pStateMonitor @see DM_SRV_STATE_NOTIFY_T
* @return the result (refer to DM_RET_E enum).
*/
INT32 DmSrvServiceStateNotify(DM_SRV_STATE_NOTIFY_T* pStateMonitor);


#ifdef __cplusplus
}
#endif

#endif /** _DM_SRV_INTERFACE_H_ */