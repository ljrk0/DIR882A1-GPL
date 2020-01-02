/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_debug.h
  Description:      header file for thread operations
  History:
    1. zhonghai    2013-10-08         creation
        
*******************************************************************************/


#ifndef OSA_THREAD_H
#define OSA_THREAD_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/** System thread priority definition. */
typedef enum _OSA_THREAD_PRIO_E
{
    E_THREAD_PRIO_0 = 79,        /**< the min/lowest thread prio */
    E_THREAD_PRIO_1,        
    E_THREAD_PRIO_2,
    E_THREAD_PRIO_3,
    E_THREAD_PRIO_4,
    E_THREAD_PRIO_5,
    E_THREAD_PRIO_6,
    E_THREAD_PRIO_7,
    E_THREAD_PRIO_8,
    E_THREAD_PRIO_9,
    E_THREAD_PRIO_10,
    E_THREAD_PRIO_11,
    E_THREAD_PRIO_12,
    E_THREAD_PRIO_13,
    E_THREAD_PRIO_14,
    E_THREAD_PRIO_15,       /**< the max/highest thread prio */
    E_THREAD_PRIO_MAX
    
}OSA_THREAD_PRIO_E;

/** thread entry function */
typedef void    *(*OSA_THREAD_PFN)(UINT32 udwParam);

/** Thread ID Structure */
typedef struct _OSA_THREAD_T{
    pthread_t threadId;

#define OSA_THREAD_NAME_LEN 16
    INT8 name[OSA_THREAD_NAME_LEN+1];

    OSA_THREAD_PFN pfnEntry;
    UINT32 udwInitDat;

    INT32 policy;
    struct sched_param s_param;
}OSA_THREAD_T;

typedef enum _OSA_THREAD_PROP_E{
    OSA_THREAD_PROP_WAITABLE   = 0x00,            /**< Thread is joinable */
    OSA_THREAD_PROP_NOWAIT     = 0x01            /**< Thread needn't join */
}OSA_THREAD_PROP_E;

/** OSA thread schedule definition */
typedef enum _OSA_THREAD_SCHED_E{
    OSA_THREAD_SCHED_FIFO = 0,/**< when a SCHED_FIFO processes becomes runnable, it will always preempt immediately any currently running normal SCHED_OTHER process */
    OSA_THREAD_SCHED_RR = 1,/**< the same to SCHED_FIFO except that each process is only allowed to run for a maximum time quantum. Not supported yet. */
    OSA_THREAD_SCHED_OTHER = 2 /**< the default universal time-sharing scheduler policy */
}OSA_THREAD_SCHED_E;


/**
* start one thread.
*
* @param[out] pThread the struct of thread info.
* @param[in] pName the name for the new thread.
* @param[in] pfnEntry the function called by the new thread.
* @param[in] udwInitDat arguments for pfnEntry function.
* @param[in] udwStackSize stack size for the new thread. It must be more than PTHREAD_STACK_MIN.
* @param[in] eSched thread schedule.
* @param[in] ePri thread priority(applied only to SCHED_OTHER case).
* @param[in] eProp joinable or detached.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_create( OSA_THREAD_T *pThread, INT8 *pName, OSA_THREAD_PFN pfnEntry, UINT32 udwInitDat, UINT32 udwStackSize, OSA_THREAD_SCHED_E eSched, OSA_THREAD_PRIO_E ePri, OSA_THREAD_PROP_E eProp);


/**
* cancel one thread. The target thread may not quit directly until it walks into next cancel point.
*
* @param[in] pThread the struct of thread info.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_delete(OSA_THREAD_T *pThread);


/**
* try to cancel one thread if it is running.
*
* @param[in] pThread the struct of thread info.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32    osa_thread_try_delete(OSA_THREAD_T *pThread);


/**
* wait for one joinable thread quitting.
*
* @param[in] pThread the struct of thread info.
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_wait(OSA_THREAD_T *pThread);


/**
* let current thread sleep.
*
* @param[in] udwMs the time to sleep(millisecond).
* @return null.
*
*/
void  osa_thread_sleep(UINT32 udwMs);


/**
* get the pid of current thread.Note that it is different from pthread_self().
*
* @param[in] void
* @return the pid of the thread.
*
*/
pid_t osa_thread_gettid(void);


/**
* set a new name for current thread.
*
* @param[in] pName the name string
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_thread_set_name(const char *pName, ...);

INT32 osa_thread_set_priority(OSA_THREAD_T *pThread, UINT32 ulPriority);
INT32 osa_thread_increase_priority(pthread_t threadId);
INT32 osa_thread_decrease_priority(pthread_t threadId);

#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif


