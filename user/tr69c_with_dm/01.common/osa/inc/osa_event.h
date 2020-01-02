/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_event.h
  Description:      header file for event operations
  History:
    1. zhonghai    2013-10-08         creation
        
*******************************************************************************/


#ifndef OSA_EVENT_H
#define OSA_EVENT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/** event option definition */
typedef enum _OSA_EVENT_OPTION_E {
    OSA_EVENT_CLEAR_REQUIRED            = 0x01,  /**< clear required events */
    OSA_EVENT_ANY_RECEIVED              = 0x02,  /**< some events received */
    OSA_EVENT_ALL_RECEIVED              = 0x04   /**< all events received */
}OSA_EVENT_OPTION_E;

/** OSA event group definition */
typedef struct _OSA_EVENT_GRP_T
{
    UINT32			  eventBits;
    UINT8             eventGrpDeleted;
    pthread_condattr_t condAttr;
	pthread_cond_t    cv;
	pthread_mutex_t   lock;
}OSA_EVENT_GRP_T;

/**
* create an event group.
*
* @param[out] pEvent the struct of the event group.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_event_group_create(OSA_EVENT_GRP_T *pEvent);


/**
* destroy the event group.
*
* @param[in] pEvent the struct of the event group.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_event_group_delete(OSA_EVENT_GRP_T *pEvent);


/**
* wait for the specified event on the event group.
*
* @param[in] pEvent the struct of the event group.
* @param[in] udwReqEvents the events required
* @param[in] eOption the option for required events, 
* @param[out] pudwRealEvt return the actual events received.
* @param[in] udwWaitMS the time(millsecond) to wait for the required event. 0 means always blocked until the event arrived.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_event_get(OSA_EVENT_GRP_T *pEvent, UINT32 udwReqEvents, OSA_EVENT_OPTION_E eOption, UINT32 *pudwRealEvt, UINT32 udwWaitMS);


/**
* fill the events into the event group.
* if any one blocked at osa_event_get(), it will catch the changes.
*
* @param[in] pEvent the struct of the event group.
* @param[in] udwEvents the events to fill
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_event_set(OSA_EVENT_GRP_T *pEvent, UINT32 udwEvents);


/**
* delete events from the event group.
* if any one blocked at osa_event_get(), it will catch the changes.
*
* @param[in] pEvent the struct of the event group.
* @param[in] udwEvents the events to delete
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_event_clear(OSA_EVENT_GRP_T *pEvent, UINT32 udwEvents);


#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif



