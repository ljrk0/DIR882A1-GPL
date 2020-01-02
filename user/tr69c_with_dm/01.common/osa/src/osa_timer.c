/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         osa_timer.c
  Description: the timer operations
  History:
*******************************************************************************/


#include "osa.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/** osa timer task definition */
typedef struct _OSA_TM_TASK_T
{
    OSA_TIMER_T task;                               /**< task */
    struct timeval tv_added;                   /**< the time the task added. */
    struct timeval tv_expired;                   /**< how long the task will run since added. */
    UINT32 interval_ms;                           /**< the interval of the task */
    OSA_TIMER_TMRCB_PFN cb;                           /**< the callback function to called when task expired. */
    UINT32 cb_data;                               /**< the argument for callback function */
    OSA_TIMER_PROP_E prop;                         /**< run once or periodically */

    struct _OSA_TM_TASK_T *next;
}OSA_TM_TASK_T;

/** osa timer status definition */
typedef struct _OSA_TIMER_STATUS_T
{
    INT8 name[OSA_TIMER_NAME_LEN+1];

    struct timeval now_mono;
    struct timeval now_real;

    INT32 count;
#define OSA_TASK_DUMP_MAX 20
    OSA_TM_TASK_T timers[OSA_TASK_DUMP_MAX];
}OSA_TIMER_STATUS_T;

typedef struct _OSA_TMR_WAKEUP_T
{
    INT32 index;
    struct timeval wakeup;
    INT32 reason;
}OSA_TMR_WAKEUP_T;

typedef struct _OSA_TMR_EXP_T
{
    INT32 index;
    OSA_TIMER_T task;
    struct timeval before_call;
    struct timeval after_call;
    struct timeval real_time;
    UINT32 call_ptr;
}OSA_TMR_EXP_T;

typedef struct _OSA_TMR_CHG_T
{
    INT32 index;
    OSA_TIMER_T task;
    UINT32 is_added;
    struct timeval change_time;
}OSA_TMR_CHG_T;

/** the timer ctrl definition */
typedef struct _OSA_TIMER_CTRL_T
{
    INT8 name[OSA_TIMER_NAME_LEN+1];

    OSA_TM_TASK_T *task_queue;
    UINT32 max_task_id;
    UINT32 task_count;

    pthread_condattr_t condAttr;
    pthread_cond_t    cv;

    OSA_THREAD_MUTEX_T   mutex;

    OSA_THREAD_T thread_id;
    BOOL8 srv_stopped;

#define OSA_WAKEUP_LIST_LENGTH 1024
    UINT32 wakeupCounter;
    OSA_TMR_WAKEUP_T wakeupList[OSA_WAKEUP_LIST_LENGTH];

#define OSA_TMR_CHG_LIST_LENGTH 1024
    UINT32 tmrChgCounter;
    OSA_TMR_CHG_T tmrChgList[OSA_TMR_CHG_LIST_LENGTH];

#define OSA_TMR_EXPIRED_LIST_LENGTH 1024
    UINT32 tmrExpiredCounter;
    OSA_TMR_EXP_T tmrExpiredList[OSA_TMR_EXPIRED_LIST_LENGTH];

    OSA_TIMER_T status_timer;
    OSA_TIMER_STATUS_T status;
}OSA_TIMER_CTRL_T;

static OSA_TIMER_CTRL_T g_OsaTimerCtrl;

#ifdef OSA_TMR_DBG
static void osa_dump_task_status_without_lock(OSA_TIMER_CTRL_T *pScheduler, UINT32 timerId, BOOL8 bAdded);
#endif

static struct timeval * osa_time_realtime(struct timeval *tv)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) == 0)
    {
        if (tv)
        {
            tv->tv_sec = ts.tv_sec;
            tv->tv_usec = ts.tv_nsec / 1000;
        }
    }

    return tv;

}

static struct timeval * osa_time_monotonic(struct timeval *tv)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
    {
        if (tv)
        {
            tv->tv_sec = ts.tv_sec;
            tv->tv_usec = ts.tv_nsec / 1000;
        }
    }

    return tv;

}

static INT32 osa_timeval_cmp(struct timeval *a, struct timeval *b)
{
    if (a->tv_sec > b->tv_sec)
    {
        return 1;
    }
    if (a->tv_sec < b->tv_sec)
    {
        return -1;
    }
    if (a->tv_usec > b->tv_usec)
    {
        return 1;
    }
    if (a->tv_usec < b->tv_usec)
    {
        return -1;
    }
    return 0;
}

static INT32 osa_add_task(OSA_TIMER_CTRL_T *pScheduler, OSA_TM_TASK_T *pTask)
{
    OSA_TM_TASK_T *cur_task = pScheduler->task_queue;
    OSA_TM_TASK_T *last_task = NULL;

    pScheduler->task_count++;

    if (pTask->task.timerID==0)
    {
        pScheduler->max_task_id++;
        pTask->task.timerID = pScheduler->max_task_id;
    }

    /* update change list */
    g_OsaTimerCtrl.tmrChgList[g_OsaTimerCtrl.tmrChgCounter].index = g_OsaTimerCtrl.tmrChgCounter;
    g_OsaTimerCtrl.tmrChgList[g_OsaTimerCtrl.tmrChgCounter].task = pTask->task;
    g_OsaTimerCtrl.tmrChgList[g_OsaTimerCtrl.tmrChgCounter].is_added = 1;
    g_OsaTimerCtrl.tmrChgList[g_OsaTimerCtrl.tmrChgCounter].change_time = pTask->tv_added;
    g_OsaTimerCtrl.tmrChgCounter++;
    g_OsaTimerCtrl.tmrChgCounter = g_OsaTimerCtrl.tmrChgCounter%OSA_TMR_CHG_LIST_LENGTH;

    if (cur_task==NULL)
    {
        /* the first task */
        pScheduler->task_queue = pTask;
        pTask->next = NULL;
    }
    else
    {
        while(cur_task)
        {
            if (osa_timeval_cmp(&pTask->tv_expired, &cur_task->tv_expired) < 0)
            {
                /* find the point to insert new task */
                if (last_task==NULL)
                {
                    pTask->next = cur_task;
                    pScheduler->task_queue = pTask;
                }
                else
                {
                    pTask->next = cur_task;
                    last_task->next = pTask;
                }
                break;
            }
            else if (cur_task->next==NULL)
            {
                /* to the end, so append it */
                cur_task->next = pTask;
                pTask->next = NULL;
                break;
            }

            last_task = cur_task;
            cur_task = cur_task->next;
        }
    }
#ifdef OSA_TMR_DBG
    osa_dump_task_status_without_lock(pScheduler,pTask->task.timerID,OSA_TRUE);
#endif
    return OSA_SUCCESS;
}

static OSA_TM_TASK_T *osa_peek_task(OSA_TIMER_CTRL_T *pScheduler)
{
    return pScheduler->task_queue;
}

static OSA_TM_TASK_T *osa_remove_task(OSA_TIMER_CTRL_T *pScheduler, UINT32 timerID)
{
	OSA_TM_TASK_T *cur_task = pScheduler->task_queue;
    OSA_TM_TASK_T *last_task = NULL;

    if (!pScheduler->task_count || !timerID)
    {
        return NULL;
    }

    while(cur_task)
    {
        if (cur_task->task.timerID == timerID)
        {
            if (last_task==NULL)
            {
                /* remove the header task */
                pScheduler->task_queue = pScheduler->task_queue->next;
            }
            else
            {
                /* remove the task */
                last_task->next = cur_task->next;
            }

            break;
        }

        last_task = cur_task;
        cur_task = cur_task->next;
    }

    if (cur_task)
    {
        pScheduler->task_count--;

        /* update change list */
        g_OsaTimerCtrl.tmrChgList[g_OsaTimerCtrl.tmrChgCounter].index = g_OsaTimerCtrl.tmrChgCounter;
        g_OsaTimerCtrl.tmrChgList[g_OsaTimerCtrl.tmrChgCounter].task = cur_task->task;
        g_OsaTimerCtrl.tmrChgList[g_OsaTimerCtrl.tmrChgCounter].is_added = 0;
        g_OsaTimerCtrl.tmrChgList[g_OsaTimerCtrl.tmrChgCounter].change_time = cur_task->tv_added;
        g_OsaTimerCtrl.tmrChgCounter++;
        g_OsaTimerCtrl.tmrChgCounter = g_OsaTimerCtrl.tmrChgCounter%OSA_TMR_CHG_LIST_LENGTH;
    }
#ifdef OSA_TMR_DBG
    osa_dump_task_status_without_lock(pScheduler,timerID,OSA_FALSE);
#endif
    return cur_task;
}

static void osa_schedule_tasks(OSA_TIMER_CTRL_T *pScheduler)
{
    struct timeval now;
    struct timeval now_Real;
    struct timespec ts;
    struct timeval tv_added;
    struct timeval tv_expired;
	OSA_TM_TASK_T *task;
    OSA_TM_TASK_T tmp;
	BOOL8 timed = OSA_FALSE;
    INT32 ret = 0;

    OSA_DBG_TRACE("+++++++++++++++++osa timer thread, tid:%d.\n", osa_thread_gettid());

    while(pScheduler->srv_stopped != OSA_TRUE)
    {
        timed = OSA_FALSE;
        osa_thread_mutex_lock(&(pScheduler->mutex));

        osa_time_monotonic(&now);
        osa_time_realtime(&now_Real);

        g_OsaTimerCtrl.wakeupList[g_OsaTimerCtrl.wakeupCounter].index=g_OsaTimerCtrl.wakeupCounter;
        g_OsaTimerCtrl.wakeupList[g_OsaTimerCtrl.wakeupCounter].wakeup=now;
        g_OsaTimerCtrl.wakeupList[g_OsaTimerCtrl.wakeupCounter].reason=ret;
        g_OsaTimerCtrl.wakeupCounter++;
        g_OsaTimerCtrl.wakeupCounter=g_OsaTimerCtrl.wakeupCounter%OSA_WAKEUP_LIST_LENGTH;

        if ((task = osa_peek_task(pScheduler)) != NULL)
        {
            if (osa_timeval_cmp(&now, &task->tv_expired) >= 0)
            {
                osa_remove_task(pScheduler, task->task.timerID);

                osa_mem_set(&tmp, 0, sizeof(OSA_TM_TASK_T));
                osa_mem_copy(&tmp, task, sizeof(OSA_TM_TASK_T));

                if (task->prop == OSA_TIMER_PROP_AUTOLOAD)
                {
                    /* add it to task heap again. */
                    task->tv_added.tv_sec = now.tv_sec;
                    task->tv_added.tv_usec = now.tv_usec;

                    tv_added.tv_sec = task->interval_ms / 1000;
                    tv_added.tv_usec = (task->interval_ms % 1000) * 1000;

                    timeradd(&(task->tv_added), &tv_added, &(task->tv_expired));

                    if(osa_add_task(pScheduler, task) != OSA_SUCCESS)
                    {
                        /* reload task failed */
                        OSA_DBG_ERROR("reload timer id=%d(%s) failed in osa_schedule_tasks()",task->task.timerID,task->task.name);
                        OSA_MEM_FREE(task);
                    }
                    else
                    {
                        OSA_DBG_DEBUG("timer(id=%d %s) reload(%ld.%06lds) now(%ld.%06lds) added(%ld.%06lds) interval(%dms) left_in_queue(%d)",tmp.task.timerID,tmp.task.name,tmp.tv_expired.tv_sec,tmp.tv_expired.tv_usec,now.tv_sec,now.tv_usec,tmp.tv_added.tv_sec,tmp.tv_added.tv_usec,tmp.interval_ms, pScheduler->task_count);
                    }
                }
                else
                {
                    /* delete this task */
                    OSA_MEM_FREE(task);

                    OSA_DBG_DEBUG("timer(id=%d %s) expired(%ld.%06lds) now(%ld.%06lds) added(%ld.%06lds) interval(%dms) left_in_queue(%d)",tmp.task.timerID,tmp.task.name,tmp.tv_expired.tv_sec,tmp.tv_expired.tv_usec,now.tv_sec,now.tv_usec,tmp.tv_added.tv_sec,tmp.tv_added.tv_usec,tmp.interval_ms, pScheduler->task_count);
                }

                osa_thread_mutex_unlock(&(pScheduler->mutex));

                /* run the callback */
                if (tmp.cb != NULL)
                {
                    struct timeval before_call;
                    struct timeval after_call;
                    osa_time_monotonic(&before_call);

                    g_OsaTimerCtrl.tmrExpiredList[g_OsaTimerCtrl.tmrExpiredCounter].index = g_OsaTimerCtrl.tmrExpiredCounter;
                    g_OsaTimerCtrl.tmrExpiredList[g_OsaTimerCtrl.tmrExpiredCounter].task = tmp.task;
                    g_OsaTimerCtrl.tmrExpiredList[g_OsaTimerCtrl.tmrExpiredCounter].real_time = now_Real;
                    g_OsaTimerCtrl.tmrExpiredList[g_OsaTimerCtrl.tmrExpiredCounter].before_call = before_call;
                    g_OsaTimerCtrl.tmrExpiredList[g_OsaTimerCtrl.tmrExpiredCounter].after_call.tv_sec = 0;
                    g_OsaTimerCtrl.tmrExpiredList[g_OsaTimerCtrl.tmrExpiredCounter].after_call.tv_usec = 0;

                    OSA_DBG_DEBUG("timer(id=%d %s) cb(0x%x)-enter-calling interval(%dms) left_in_queue(%d)",tmp.task.timerID,tmp.task.name,tmp.cb,tmp.interval_ms, pScheduler->task_count);
                    tmp.cb(tmp.cb_data);
                    osa_time_monotonic(&after_call);

                    g_OsaTimerCtrl.tmrExpiredList[g_OsaTimerCtrl.tmrExpiredCounter].after_call = after_call;
                    g_OsaTimerCtrl.tmrExpiredList[g_OsaTimerCtrl.tmrExpiredCounter].call_ptr = (UINT32)tmp.cb;
                    g_OsaTimerCtrl.tmrExpiredCounter++;
                    g_OsaTimerCtrl.tmrExpiredCounter = g_OsaTimerCtrl.tmrExpiredCounter%OSA_TMR_EXPIRED_LIST_LENGTH;

                    OSA_DBG_DEBUG("timer(id=%d %s) cb(0x%x)-leave-calling interval(%dms) left_in_queue(%d) time_cost(%ld.%06lds->%ld.%06lds)",tmp.task.timerID,tmp.task.name,tmp.cb,tmp.interval_ms, pScheduler->task_count,before_call.tv_sec,before_call.tv_usec,after_call.tv_sec,after_call.tv_usec);
                }

                continue;
            }

            timed = OSA_TRUE;
        }

        if (timed)
        {
            tv_added.tv_sec = 0;
            tv_added.tv_usec = 0;
            tv_expired.tv_sec = 0;
            tv_expired.tv_usec = 0;

            timersub(&task->tv_expired, &now, &tv_added);
            timeradd(&now_Real, &tv_added, &tv_expired);

            ts.tv_sec = tv_expired.tv_sec;
            ts.tv_nsec = tv_expired.tv_usec*1000;

            /* No matter if timeout or interrupted, we just take next loop. */
            ret = pthread_cond_timedwait(&(pScheduler->cv), &(pScheduler->mutex.mutex), &ts);
        }
        else
        {
            /* no task, so wait for new task */
            ret = pthread_cond_wait(&(pScheduler->cv), &(pScheduler->mutex.mutex));
        }

        osa_thread_mutex_unlock(&(pScheduler->mutex));
    }

    return;
}
#ifdef OSA_TMR_DBG
static void osa_dump_task_status_without_lock(OSA_TIMER_CTRL_T *pScheduler, UINT32 timerId, BOOL8 bAdded)
{
    OSA_TM_TASK_T *cur_task = pScheduler->task_queue;
    INT8 task_ids[1024] = {0};
    struct timeval now;

    memset(task_ids,0,sizeof(task_ids));

    while (cur_task)
    {
        snprintf(task_ids+strlen(task_ids),sizeof(task_ids)-strlen(task_ids),"%d(%s %ld.%06lds) ",cur_task->task.timerID,cur_task->task.name,cur_task->tv_expired.tv_sec,cur_task->tv_expired.tv_usec);
        cur_task = cur_task->next;
    }

    osa_time_monotonic(&now);

    OSA_DBG_TRACE("osa_timer %d %s (time %ld.%06lds) : %d timers in queue, ID(expired) : %s",timerId,bAdded?"add":"del",now.tv_sec,now.tv_usec,pScheduler->task_count,task_ids);
    return;
}
#endif
static void osa_dump_task_status(OSA_TIMER_CTRL_T *pScheduler)
{
    OSA_TM_TASK_T *cur_task = NULL;

    memset(&pScheduler->status,0,sizeof(pScheduler->status));

    osa_thread_mutex_lock(&(pScheduler->mutex));
    cur_task = pScheduler->task_queue;

    /* the number of tasks may be more than OSA_TASK_DUMP_MAX, here we just dump first ones */

    while (cur_task)
    {
        if (pScheduler->status.count < OSA_TASK_DUMP_MAX)
        {
            memcpy(&pScheduler->status.timers[pScheduler->status.count],cur_task,sizeof(OSA_TM_TASK_T));
            pScheduler->status.count++;
        }
        else
        {
            break;
        }

        cur_task = cur_task->next;
    }

    pScheduler->status.count = pScheduler->task_count;

    osa_thread_mutex_unlock(&(pScheduler->mutex));

    snprintf(pScheduler->status.name,sizeof(pScheduler->status.name),"%s",pScheduler->name);
    osa_time_realtime(&pScheduler->status.now_real);
    osa_time_monotonic(&pScheduler->status.now_mono);

    OSA_DBG_INFO(OSA_DEBUG_INFO_TIMER_STATUS,&pScheduler->status,sizeof(pScheduler->status));
    return;
}

/*
 ************************************************************************************************
 INT32 osa_timer_service_start(INT8 *pTmrName)
 This func will start  timer function. 
 ************************************************************************************************
*/
INT32 osa_timer_service_start(INT8 *pTmrName)
{
    OSA_TIMER_CTRL_T *pTimerCtrl = &g_OsaTimerCtrl;
    INT32   lRet = OSA_SUCCESS;

    if(pTimerCtrl)
    {
        osa_mem_set(pTimerCtrl,0,sizeof(OSA_TIMER_CTRL_T));

        pTimerCtrl->srv_stopped = OSA_FALSE;
        pTimerCtrl->task_count = 0;
        pTimerCtrl->max_task_id = 0;
        pTimerCtrl->thread_id.threadId = 0;
        pTimerCtrl->task_queue = NULL;

        pthread_condattr_init(&(pTimerCtrl->condAttr));
        //pthread_condattr_setclock(&(pTimerCtrl->condAttr), CLOCK_REALTIME);

        lRet = pthread_cond_init(&pTimerCtrl->cv,NULL);
        if ( lRet != OSA_SUCCESS )
        {
            OSA_DBG_ERROR("pthread_cond_init failed err-code=%d %s in osa_timer_service_start()",lRet,strerror(lRet));
            lRet = OSA_FAILURE;
            return lRet;
        }

        lRet = osa_thread_mutex_create(&pTimerCtrl->mutex);
        if ( lRet != OSA_SUCCESS )
        {
            OSA_DBG_ERROR("osa_thread_mutex_create failed in osa_timer_service_start()");
            lRet = OSA_FAILURE;
            return lRet;
        }

        if (pTmrName==NULL)
        {
            pTmrName = "osa_tmrs";
        }

        snprintf(pTimerCtrl->name,sizeof(pTimerCtrl->name),"%s",pTmrName);

        lRet = osa_thread_create(&(pTimerCtrl->thread_id),
                                    pTmrName,
                                    (OSA_THREAD_PFN)osa_schedule_tasks,
                                    (UINT32)pTimerCtrl,
                                    0xF000,
                                    OSA_THREAD_SCHED_FIFO,
                                    E_THREAD_PRIO_5,
                                    OSA_THREAD_PROP_NOWAIT);

        if (lRet != OSA_SUCCESS)
        {
            OSA_DBG_ERROR("osa_thread_create failed in osa_timer_service_start()");
            return lRet;
        }

        /* here we start a timer to print timer current status. */
        lRet = osa_timer_start(&(pTimerCtrl->status_timer),
                                "TmrStatusTmr",
                                (OSA_TIMER_TMRCB_PFN)osa_dump_task_status,
                                (UINT32)pTimerCtrl,
                                60*1000,
                                OSA_TIMER_PROP_AUTOLOAD);

        if (lRet != OSA_SUCCESS)
        {
            OSA_DBG_ERROR("osa_timer_start failed in osa_timer_service_start()");
            return lRet;
        }
        else
        {
            OSA_DBG_DEBUG("osa_timer status started...");
        }

    }
    else
    {
        lRet = OSA_FAILURE;
    }

    return lRet;
}

/*
 ************************************************************************************************
 INT32 osa_timer_service_stop(void)
 This func will stop  timer function. 
 ************************************************************************************************
*/

INT32 osa_timer_service_stop(void)
{
    OSA_TIMER_CTRL_T *pTimerCtrl = &g_OsaTimerCtrl;
    INT32 lRet = OSA_SUCCESS;


    if (pTimerCtrl)
    {
        /* stop status timer */
        osa_timer_stop(&pTimerCtrl->status_timer);

        pTimerCtrl->srv_stopped = OSA_TRUE;
        /* just stop timer-thread running, not release resources in case that any other thread is using it */
    }

    return lRet;

}

/*
 ************************************************************************************************
  INT32 osa_timer_start(      OSA_TIMER_T *pTimer,
                              OSA_TIMER_TMRCB_PFN     pfnCallBack,
                              UINT32       udwCBDat,
                              UINT32       udwMS,
                              OSA_TIMER_PROP_E       eProp
                        );

   This function will start a timer. timer struct buffer should be alloc by user. If start successfully, SUCCESS will be 
   returned, or Error Code returned.User can use this function as this:
 ************************************************************************************************
*/
INT32 osa_timer_start(   OSA_TIMER_T *pTimer,            /* Timer ID */
                              INT8 *pName,                      /* Timer name */
                              OSA_TIMER_TMRCB_PFN     pfnCallBack,       /* Callback function for this timer */
                              UINT32       udwCBDat,          /* Callback parameter for this timer */
                              UINT32       udwMS,             /* timer will overflowed after udwMS(mill-second) */
                              OSA_TIMER_PROP_E       eProp            /* property for this timer.It can be TMR_PROP_ONCE or TMR_PROP_AUTOLOAD. */
                        )
{
    OSA_TIMER_CTRL_T *pTimerCtrl = &g_OsaTimerCtrl;
    OSA_TM_TASK_T *pTask = NULL;
    INT32 lRet = OSA_FAILURE;
    struct timeval tv_added;

    if (pTimer == NULL)
    {
        OSA_DBG_ERROR("invalid arguments in osa_timer_start()");
        return lRet;
    }

    if (udwMS <= 0)
    {
        OSA_DBG_ERROR("udwMS<=0, invalid arguments in osa_timer_start()");
        return lRet;
    }

    if(pTimerCtrl && !pTimerCtrl->srv_stopped)
    {
        snprintf(pTimer->name,sizeof(pTimer->name),"%s",(pName?pName:"null"));

        osa_thread_mutex_lock(&(pTimerCtrl->mutex));

        if (pTimer->timerID != 0)
        {
            pTask = osa_remove_task(pTimerCtrl, pTimer->timerID);

            OSA_MEM_FREE(pTask);
        }

        pTask = (OSA_TM_TASK_T *)OSA_MEM_ALLOC(sizeof(OSA_TM_TASK_T));

        if (pTask == NULL)
        {
            osa_thread_mutex_unlock(&(pTimerCtrl->mutex));
            OSA_DBG_ERROR("malloc failed in osa_timer_start()");
            return OSA_FAILURE;
        }

        osa_mem_set(pTask, 0, sizeof(OSA_TM_TASK_T));

        pTask->task = *pTimer;/* re-use last timer-id */
        pTask->cb = pfnCallBack;
        pTask->cb_data = udwCBDat;
        pTask->prop = eProp;
        pTask->interval_ms = udwMS;

        osa_time_monotonic(&(pTask->tv_added));

        tv_added.tv_sec = udwMS / 1000;
        tv_added.tv_usec = (udwMS % 1000) * 1000;

        timeradd(&(pTask->tv_added), &(tv_added), &(pTask->tv_expired));

        if (osa_add_task(pTimerCtrl, pTask) == OSA_SUCCESS)
        {
            pTimer->timerID = pTask->task.timerID;
            pTimer->isRunning = OSA_TRUE;
            pthread_cond_signal(&(pTimerCtrl->cv));
        }
        else
        {
            pTimer->timerID = 0;
            pTimer->isRunning = OSA_FALSE;
            OSA_MEM_FREE(pTask);
        }

        osa_thread_mutex_unlock(&(pTimerCtrl->mutex));
    }
    else
    {
        pTimer->timerID = 0;
        pTimer->isRunning = OSA_FALSE;
    }

    if (pTimer->timerID > 0)
    {
        OSA_DBG_DEBUG("timer id=%d(%s) added(%ld.%06lds) interval(%dms) cb(0x%x) left_in_queue(%d)",pTimer->timerID,pTimer->name,pTask->tv_added.tv_sec,pTask->tv_added.tv_usec,udwMS,pfnCallBack,pTimerCtrl->task_count);
        lRet = OSA_SUCCESS;
    }
    else
    {
        OSA_DBG_DEBUG("timer (%s) adding failed interval(%dms) cb(0x%x) left_in_queue(%d)",pTimer->name,udwMS,pfnCallBack,pTimerCtrl->task_count);
    }

    return lRet;
}

/*
 ************************************************************************************************
 INT32 osa_timer_stop(OSA_TIMER_T *pTmr)
 This func will stop  pTmr. 
 If stop a timer successfully, SUCCESS will be returned or Error code returned.
 ************************************************************************************************
*/
INT32 osa_timer_stop(OSA_TIMER_T *pTimer)
{
    OSA_TIMER_CTRL_T *pTimerCtrl = &g_OsaTimerCtrl;
    OSA_TM_TASK_T *pTask = NULL;
	INT32 lRet = OSA_FAILURE;

    if (pTimer == NULL)
    {
        OSA_DBG_ERROR("invalid arguments in osa_timer_stop()");
        return lRet;
    }

    if (pTimer->timerID == 0)
    {
        OSA_DBG_WARNING("invalid arguments : timerID = 0 in osa_timer_stop()");
        return lRet;
    }

    if(pTimerCtrl && !pTimerCtrl->srv_stopped)
    {
        osa_thread_mutex_lock(&(pTimerCtrl->mutex));

        pTask = osa_remove_task(pTimerCtrl, pTimer->timerID);

        pTimer->isRunning = OSA_FALSE;

        osa_thread_mutex_unlock(&(pTimerCtrl->mutex));

        if (pTask != NULL)
        {
            OSA_DBG_DEBUG("timer id=%d(%s) deleted added_time(%ld.%06lds) interval(%dms) cb(0x%x) left_in_queue(%d)",pTimer->timerID,pTimer->name,pTask->tv_added.tv_sec,pTask->tv_added.tv_usec,pTask->interval_ms,pTask->cb,pTimerCtrl->task_count);
            OSA_MEM_FREE(pTask);
        }
        else
        {
            OSA_DBG_DEBUG("timer id=%d(%s) not found, maybe expired and deleted, left_in_queue(%d)",pTimer->timerID,pTimer->name,pTimerCtrl->task_count);
        }

        lRet = OSA_SUCCESS;
    }

    return lRet;
}



void osa_systimer_handler(union sigval sigev_value)
{
     OSA_SYSTIMER_T *pTimer = sigev_value.sival_ptr;

     printf("osa_systimer_handler, SIGRTMIN, timerID=%d\n", (INT32)pTimer->timerID); 
     
     OSA_DBG_TRACE("osa_systimer_handler, SIGRTMIN, timerID=%d", (INT32)pTimer->timerID);

     abort();
     
     return;
}

INT32 osa_systimer_create(OSA_SYSTIMER_T *pTimer,            //Timer
                          PFNTMRCB        pfnCallBack,       //Callback function for this timer
                          UINT32          udwCBDat,          //Callback parameter for this timer
                          UINT32          udwProp            //property for this timer.It can be TMR_PROP_ONCE or TMR_PROP_AUTOLOAD.
                          )
{
    struct sigevent se;
    INT32  lRet = 0;

    if (pTimer == NULL)
    {
        OSA_DBG_WARNING("osa_systimer_create, pTimer is NULL");
        return -1;
    }
    
    if (pTimer->ucTimerFlg != OSA_TIMER_CREATED)
    {
        pTimer->PFNTMRCB = pfnCallBack;
        pTimer->mode = udwProp;
        pTimer->value = udwCBDat;

        memset(&se, 0, sizeof(struct sigevent));
        se.sigev_notify = SIGEV_THREAD;
        se.sigev_value.sival_ptr = pTimer;
        se.sigev_notify_function = osa_systimer_handler;
        se.sigev_notify_attributes = NULL;

        lRet = timer_create(CLOCK_THREAD_CPUTIME_ID, &se, &pTimer->timerID);
        if (lRet != 0)
        {
            OSA_DBG_WARNING("osa_systimer_create, timer_create failed %d, errno=%d (%s)", lRet, errno, strerror(errno));
            pTimer->ucTimerFlg = OSA_TIMER_NO_CREATED;
        }
        else
        {
            pTimer->ucTimerFlg = OSA_TIMER_CREATED;
        }
    }
    else
    {
        OSA_DBG_TRACE("osa_systimer_create, pTimer->ucTimerFlg is already OSA_TIMER_CREATED");
    }

    return lRet;
}

INT32 osa_systimer_delete(OSA_SYSTIMER_T *pTimer)
{
    struct itimerspec ts;
    INT32  lRet = -1;

    if (pTimer == NULL)
    {
        OSA_DBG_WARNING("osa_systimer_delete, pTimer is NULL");
        return -1;
    }

    if (pTimer->ucTimerFlg == OSA_TIMER_CREATED) 
    {
        memset(&ts, 0, sizeof(ts));
        lRet = timer_settime(pTimer->timerID, 0, &ts, NULL);
        if (lRet != 0)
        {
            OSA_DBG_WARNING("osa_systimer_delete, timer_settime failed %d, errno=%d (%s)", lRet, errno, strerror(errno));
        }

        lRet = timer_delete(pTimer->timerID);
        if (lRet != 0)
        {
            OSA_DBG_WARNING("osa_systimer_delete, timer_delete failed %d, errno=%d (%s)", lRet, errno, strerror(errno));
        }
        pTimer->ucTimerFlg = OSA_TIMER_NO_CREATED;
    }
    else
    {
        OSA_DBG_WARNING("osa_systimer_delete, pTimer->ucTimerFlg is OSA_TIMER_NO_CREATED");
    }
   
    return lRet;
}

INT32 osa_systimer_start(OSA_SYSTIMER_T *pTimer,            //Timer ID
                         UINT32          udwMS              //timer will overflowed after udwMS(mil-second)
                         )
{
    struct itimerspec ts;
    INT32  lRet = -1;

    if (pTimer == NULL)
    {
        OSA_DBG_WARNING("osa_systimer_start, pTimer is NULL");
        return -1;
    }

    if (pTimer->ucTimerFlg == OSA_TIMER_CREATED)
    {
        if (pTimer->mode == OSA_TIMER_PROP_ONCE)
        {
            ts.it_value.tv_nsec    = (udwMS%1000ULL)*1000000ULL;
            ts.it_value.tv_sec     = udwMS/1000ULL;
            ts.it_interval.tv_nsec = 0;
            ts.it_interval.tv_sec  = 0;
        }
        else  
        {
            ts.it_value.tv_nsec    = (udwMS%1000ULL)*1000000ULL;
            ts.it_value.tv_sec     = udwMS/1000ULL;
            ts.it_interval.tv_nsec = (udwMS%1000ULL)*1000000ULL;
            ts.it_interval.tv_sec  = udwMS/1000ULL;
        }
        
        lRet = timer_settime(pTimer->timerID, 0, &ts, NULL);
        if (lRet != 0)
        {
            OSA_DBG_WARNING("osa_systimer_start, timer_settime failed %d, errno=%d (%s)", lRet, errno, strerror(errno));
        }
    }
    else
    {
        OSA_DBG_WARNING("osa_systimer_start, pTimer->ucTimerFlg is OSA_TIMER_NO_CREATED");
    }
    
    return lRet;
}

INT32 osa_systimer_stop(OSA_SYSTIMER_T *pTimer)
{
    struct itimerspec ts;
    INT32  lRet = -1;

    if (pTimer == NULL)
    {
        OSA_DBG_WARNING("osa_systimer_stop, pTimer is NULL");
        return -1;
    }

    if (pTimer->ucTimerFlg == OSA_TIMER_CREATED)
    {
        memset(&ts, 0, sizeof(ts));
        lRet = timer_settime(pTimer->timerID, 0, &ts, NULL);
        if (lRet != 0)
        {
            OSA_DBG_WARNING("osa_systimer_stop, timer_settime failed %d, errno=%d (%s)", lRet, errno, strerror(errno));
        }
    }
    else
    {
        OSA_DBG_WARNING("osa_systimer_stop, pTimer->ucTimerFlg is OSA_TIMER_NO_CREATED");
    }

    return lRet;
}


#ifdef __cplusplus
#if __cplusplus
    }
#endif 
#endif/* __cpluscplus */


