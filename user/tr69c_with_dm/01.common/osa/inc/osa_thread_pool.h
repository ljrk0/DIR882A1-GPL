/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_thread_pool.h
  Description:      header file for thread pool operations
  History:
    1. zhonghai    2016-04-28         creation
        
*******************************************************************************/


#ifndef OSA_THREAD_POOL_H
#define OSA_THREAD_POOL_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

#define OSA_TREAD_POOL_NAME_LEN 16

typedef struct _OSA_THREAD_POOL_T
{
    INT8 prefix[OSA_TREAD_POOL_NAME_LEN+1];
    UINT32 threads;
    OSA_THREAD_SCHED_E eSched;
    OSA_THREAD_PRIO_E ePri;
    BOOL8 bSchedSupported;/* support task schedulely */
    BOOL8 bStopped;/* indicate thread pool stopped */
    void *ctrl;/* to hide the control info */
}OSA_THREAD_POOL_T;

typedef enum _OSA_TP_TASK_PRIO_E
{
    OSA_TP_TASK_PRIO__LOW=0,/* low tasks */
    OSA_TP_TASK_PRIO__MED=1,/* default priority */
    OSA_TP_TASK_PRIO__HIGH=2/* will serve first */
}OSA_TP_TASK_PRIO_E;

typedef enum _OSA_TP_TASK_SCHED_E
{
    OSA_TP_TASK_SCHED__NONE,/* only once, no need to requeue the task */
    OSA_TP_TASK_SCHED__DIRECT,/* execute the task without requeueing */
    OSA_TP_TASK_SCHED__SCHED/* requeue the task schedulely with some interval */
}OSA_TP_TASK_SCHED_E;

/** thread entry function */
typedef BOOL8    (*OSA_TP_TASK_CB_FUN)(void *data);

typedef struct _OSA_TP_TASK_T
{
    UINT32 task_id;/* return value */
    OSA_TP_TASK_CB_FUN task_cb;
    void *data;
    OSA_TP_TASK_PRIO_E prio;
    OSA_TP_TASK_SCHED_E sched_type;
    UINT32 sched_interval;/* if requeued schedulely, here you can set the interval(unit: ms)*/
}OSA_TP_TASK_T;



/**
* create the thread pool.
* if to support task schedulely, make sure timer service started in advance.
*
* @param[out] pThreadPool the struct of thread pool info.
* @param[in] pName the name prefix for each thread.
* @param[in] bSchedSupported support task schedulely.
* @param[in] threads the count of threads to create.
* @param[in] eSched thread schedule.
* @param[in] ePri thread priority(applied only to SCHED_OTHER case).
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_pool_create( OSA_THREAD_POOL_T *pThreadPool, INT8 *pName, BOOL8 bSchedSupported, UINT32 threads, OSA_THREAD_SCHED_E eSched, OSA_THREAD_PRIO_E ePrio);

/**
* delete the thread pool.
*
* @param[in] pThreadPool the struct of thread pool info.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_pool_delete( OSA_THREAD_POOL_T *pThreadPool);

/**
* queue new task for the thread pool.
*
* @param[in] pThreadPool the struct of thread pool info.
* @param[in] pTask the task to queue
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_pool_queue_task( OSA_THREAD_POOL_T *pThreadPool, OSA_TP_TASK_T *pTask);

/**
* cancel one task before it running.
*
* @param[in] pThreadPool the struct of thread pool info.
* @param[in] pTask the task to cancel.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_pool_cancel_task( OSA_THREAD_POOL_T *pThreadPool, OSA_TP_TASK_T *pTask);



#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif


