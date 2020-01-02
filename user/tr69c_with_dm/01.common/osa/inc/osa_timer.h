/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_debug.h
  Description:      header file for timer operations
  History:
    1. zhonghai    2013-10-08         creation
        
*******************************************************************************/


#ifndef OSA_TIMER_H
#define OSA_TIMER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 


/** timer property definition */
typedef enum _OSA_TIMER_PROP_E{
    OSA_TIMER_PROP_ONCE             = 1,               /**< Timer will run once when it is expired */
    OSA_TIMER_PROP_AUTOLOAD         = 2,               /**< Timer will re-load automatically */
}OSA_TIMER_PROP_E;

/** the callback function which will be called when timer expired. */
typedef void (*OSA_TIMER_TMRCB_PFN)(UINT32 udwData);

/** the structure of timer */
typedef struct  _OSA_TIMER_T
{
	UINT32 timerID;

    BOOL8 isRunning;

#define OSA_TIMER_NAME_LEN 16
    INT8 name[OSA_TIMER_NAME_LEN+1];
}OSA_TIMER_T;

/**
* start timer service.
*
* @param[in] pTmrName give timer-thread a name.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_timer_service_start(INT8 *pTmrName);


/**
* stop timer service.
*
* @param[in] void
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_timer_service_stop(void);


/**
* start one timer.
* if the timer does not exist, it will first be created and then started.
*
* @param[out] pTimer the timer structure.
* @param[in] pName the name for the new timer.
* @param[in] pfnCallBack the function called when the timer expired.
* @param[in] udwCBData the argument for the pfnCallBack function.
* @param[in] udwMS timer interval
* @param[in] eProp timer will run only once or reloaded automatically.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_timer_start(OSA_TIMER_T *pTimer, INT8 *pName, OSA_TIMER_TMRCB_PFN pfnCallBack, UINT32 udwCBData, UINT32 udwMS, OSA_TIMER_PROP_E eProp);


/**
* stop the timer.
*
* @param[in] pTimer the timer structure.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_timer_stop(OSA_TIMER_T *pTimer);




#define OSA_SYSTIMER_LEN             5000

//the callback function which will be called when timer expire.
typedef void (*PFNTMRCB)(UINT32 udwDat);

typedef struct  _OSA_SYSTIMER_T
{
	void	(*PFNTMRCB)(UINT32 value);
	UINT32	value;
	UINT32  mode;
	timer_t timerID;
	pthread_attr_t pAttr;
#define     OSA_TIMER_CREATED             0xFF
#define     OSA_TIMER_NO_CREATED          0x0
	UINT8 ucTimerFlg;
}OSA_SYSTIMER_T;

INT32 osa_systimer_create(OSA_SYSTIMER_T *pTimer,            //Timer
                          PFNTMRCB        pfnCallBack,       //Callback function for this timer
                          UINT32          udwCBDat,          //Callback parameter for this timer
                          UINT32          udwProp            //property for this timer.It can be TMR_PROP_ONCE or TMR_PROP_AUTOLOAD.
                          );

INT32 osa_systimer_delete(OSA_SYSTIMER_T *pTimer);

INT32 osa_systimer_start(OSA_SYSTIMER_T *pTimer,             //Timer
                         UINT32          udwMS               //timer will overflowed after udwMS(mil-second)
                         );

INT32 osa_systimer_stop(OSA_SYSTIMER_T *pTimer);


#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif


