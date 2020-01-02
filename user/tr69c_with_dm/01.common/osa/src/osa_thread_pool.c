/*******************************************************************************
  Copyright (C) 2016, Broadcom.
  File name:         osa_thread_pool.c
  Description: thread pool operations
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

typedef enum _OSA_TP_TASK_STATUS_E
{
    OSA_TP_TASK_STATUS__QUEUED,/* in the queue */
    OSA_TP_TASK_STATUS__RUNNING,/* running in one thread */
    OSA_TP_TASK_STATUS__CANCELED,/* canceled before running */
    OSA_TP_TASK_STATUS__SCHEDULED,/* in the schedule interval */
    OSA_TP_TASK_STATUS__DONE/* done and no need to schedule */
}OSA_TP_TASK_STATUS_E;

typedef struct _OSA_TP_TASK_NODE_T
{
    OSA_TP_TASK_T task;/* backup */

    OSA_TIMER_T timer;
    OSA_TP_TASK_STATUS_E status;

    void *pThreadPool;/* to find taskGroup and poolCtrl */

    struct _OSA_TP_TASK_NODE_T *prev;
    struct _OSA_TP_TASK_NODE_T *next;
}OSA_TP_TASK_NODE_T;

typedef enum _OSA_TP_THREAD_STATUS_E
{
    OSA_TP_TASK_THREAD_STATUS__IDLE,
    OSA_TP_TASK_THREAD_STATUS__RUNNING,
    OSA_TP_TASK_THREAD_STATUS__STOPPED
}OSA_TP_THREAD_STATUS_E;

typedef enum _OSA_TP_THREAD_SERV_PRIO_E
{
    OSA_TP_THREAD_SERV_PRIO__LOW=1,/* only one thread serve for low tasks */
    OSA_TP_THREAD_SERV_PRIO__MED=2,/* default priority */
    OSA_TP_THREAD_SERV_PRIO__HIGH=4/* will serve first */
}OSA_TP_THREAD_SERV_PRIO_E;

typedef struct _OSA_TP_THREAD_T
{
    OSA_THREAD_T thread;
    OSA_TP_THREAD_SERV_PRIO_E serviceLevel;
    OSA_TP_THREAD_STATUS_E status;
    OSA_TP_TASK_NODE_T *currentTask;

    void *pThreadPool;/* to find taskGroup and poolCtrl */
}OSA_TP_THREAD_T;

typedef struct _OSA_TP_CTRL_T
{
    UINT32 max_task_id;

    OSA_THREAD_MUTEX_T apiMutex;

    OSA_TP_THREAD_T *threadPool;
    OSA_RESOURCE_GRP_T taskGroup;

    OSA_TP_TASK_NODE_T *workingTasks;
    OSA_TP_TASK_NODE_T *schedTasks;

    OSA_TP_TASK_NODE_T *highQueue;
    OSA_TP_TASK_NODE_T *medQueue;
    OSA_TP_TASK_NODE_T *lowQueue;
}OSA_TP_CTRL_T;

typedef struct _OSA_TP_SRV_LEVEL_NODE_T
{
    UINT32 srvlevel[10];
}OSA_TP_SRV_LEVEL_NODE_T;

#define OSA_TP_TASK_TIMEOUT_MS  60000

static OSA_TP_SRV_LEVEL_NODE_T srvLevelTable[10]=
{
    {{7,0,0,0,0,0,0,0,0,0}},
    {{6,7,0,0,0,0,0,0,0,0}},
    {{4,6,7,0,0,0,0,0,0,0}},
    {{1,2,4,7,0,0,0,0,0,0}},
    {{1,2,4,6,7,0,0,0,0,0}},
    {{1,2,4,6,6,7,0,0,0,0}},
    {{1,2,4,4,6,6,7,0,0,0}},
    {{1,2,4,4,6,6,7,7,0,0}},
    {{1,1,2,2,4,4,6,6,7,0}},
    {{1,1,2,2,4,4,6,6,6,7}}
};

static INT32 osa_tp_queue_append(OSA_TP_TASK_NODE_T **queue, OSA_TP_TASK_NODE_T *task)
{
    INT32 lRet = OSA_SUCCESS;

    if (queue==NULL || task==NULL)
    {
        OSA_DBG_ERROR("null arguments in osa_tp_queue_append()");
        lRet = OSA_FAILURE;
        return lRet;
    }

    if (*queue==NULL)
    {
        *queue = task;
        task->prev = task;
        task->next = task;
    }
    else
    {
        (*queue)->prev->next = task;
        task->prev = (*queue)->prev;
        task->next = *queue;
        (*queue)->prev = task;
    }

    return lRet;
}

static INT32 osa_tp_queue_remove_task_by_id(OSA_TP_TASK_NODE_T **queue, OSA_TP_TASK_NODE_T **task, UINT32 taskId)
{
    INT32 lRet = OSA_SUCCESS;

    if (queue==NULL || *queue==NULL || task==NULL || taskId==0)
    {
        OSA_DBG_ERROR("null arguments in osa_tp_queue_remove_task_by_id()");
        lRet = OSA_FAILURE;
        return lRet;
    }

    *task = *queue;

    if ((*queue)->prev==(*queue) && (*queue)->next==(*queue) && (*task)->task.task_id == taskId)
    {
        /* only one */
        *queue = NULL;
    }
    else
    {
        (*queue)->prev->next = NULL;/* break the loop */
        while(*task && (*task)->task.task_id != taskId)
        {
            *task = (*task)->next;
        }
        (*queue)->prev->next = *queue;/* recover the loop */

        if (*task == *queue)
        {
            /* if the first to remove, queue header forward */
            *queue = (*queue)->next;
        }

        if (*task)
        {
            (*task)->next->prev = (*task)->prev;
            (*task)->prev->next = (*task)->next;
        }
    }

    if (lRet == OSA_FAILURE)
    {
        *task = NULL;
    }

    return lRet;
}

static INT32 osa_tp_queue_remove_first_task(OSA_TP_TASK_NODE_T **queue, OSA_TP_TASK_NODE_T **task)
{
    INT32 lRet = OSA_SUCCESS;

    if (queue==NULL || *queue==NULL || task==NULL)
    {
        OSA_DBG_ERROR("null arguments in osa_tp_queue_remove_first_task()");
        lRet = OSA_FAILURE;
        return lRet;
    }

    *task = *queue;

    /* remove it */
    if ((*queue)->prev==(*queue) && (*queue)->next==(*queue))
    {
        /* only one */
        *queue = NULL;
    }
    else
    {
        /* in the middle */
        *queue = (*queue)->next;;
        (*task)->next->prev = (*task)->prev;
        (*task)->prev->next = (*task)->next;
    }

    return lRet;
}

static INT32 osa_tp_queue_clear(OSA_TP_TASK_NODE_T **queue)
{
    INT32 lRet = OSA_SUCCESS;
    OSA_TP_TASK_NODE_T *task = NULL;
    OSA_TP_TASK_NODE_T *taskToFree = NULL;

    if (queue==NULL || *queue==NULL)
    {
        OSA_DBG_ERROR("null arguments in osa_tp_queue_clear()");
        lRet = OSA_FAILURE;
        return lRet;
    }

    if ((*queue)->prev==(*queue) && (*queue)->next==(*queue))
    {
        /* only one */
        task = *queue;
        *queue = NULL;
        /* stop time if necessary */
        if (task->task.sched_type==OSA_TP_TASK_SCHED__SCHED
            && task->task.sched_interval>0)
        {
            osa_timer_stop(&task->timer);
        }
        OSA_MEM_FREE(task);
    }
    else
    {
        (*queue)->prev->next = NULL;/* break the loop */
        task = *queue;
        *queue = NULL;

        while(task)
        {
            taskToFree = task;
            task = task->next;

            /* stop time if necessary */
            if (taskToFree->task.sched_type==OSA_TP_TASK_SCHED__SCHED
                && taskToFree->task.sched_interval>0)
            {
                osa_timer_stop(&taskToFree->timer);
            }
            OSA_MEM_FREE(taskToFree);
        }
    }

    return lRet;
}

static INT32 osa_tp_get_service_level(UINT32 ulIndex, UINT32 threads)
{
    /* here we define the policy  */
    return srvLevelTable[threads-1].srvlevel[ulIndex];
}

static INT32 osa_tp_resource_handler(OSA_THREAD_POOL_T *pThreadPool, OSA_RESOURCE_GRP_STATUS_E status, void *pExtraData)
{
    INT32 lRet = OSA_FAILURE;
    OSA_TP_CTRL_T *pTpCtrl = (OSA_TP_CTRL_T *)pThreadPool->ctrl;

    switch(status)
    {
        case OSA_RESOURCE_WAIT:
        {
            /* try to find the task to run */
            OSA_TP_THREAD_T *threadInstance = (OSA_TP_THREAD_T *)pExtraData;

            if (pThreadPool->bStopped==OSA_TRUE)
            {
                threadInstance->status = OSA_TP_TASK_THREAD_STATUS__STOPPED;
                threadInstance->currentTask = NULL;
                lRet = OSA_FAILURE;
                break;
            }

            threadInstance->currentTask = NULL;

            /* try to find the first high task */
            if (threadInstance->serviceLevel&OSA_TP_THREAD_SERV_PRIO__HIGH
                && pTpCtrl->highQueue)
            {
                osa_tp_queue_remove_first_task(&pTpCtrl->highQueue,&threadInstance->currentTask);
            }

            /* try to find the first med task */
            if (threadInstance->serviceLevel&OSA_TP_THREAD_SERV_PRIO__MED
                && pTpCtrl->medQueue
                && !threadInstance->currentTask)
            {
                osa_tp_queue_remove_first_task(&pTpCtrl->medQueue,&threadInstance->currentTask);
            }

            /* try to find the first low task */
            if (threadInstance->serviceLevel&OSA_TP_THREAD_SERV_PRIO__LOW
                && pTpCtrl->lowQueue
                && !threadInstance->currentTask)
            {
                osa_tp_queue_remove_first_task(&pTpCtrl->lowQueue,&threadInstance->currentTask);
            }

            if (threadInstance->currentTask)
            {
                /* find the task */
                threadInstance->status = OSA_TP_TASK_THREAD_STATUS__RUNNING;
                threadInstance->currentTask->status = OSA_TP_TASK_STATUS__RUNNING;
                lRet = OSA_SUCCESS;
            }
            else
            {
                /* no task found */
                threadInstance->status = OSA_TP_TASK_THREAD_STATUS__IDLE;
                lRet = OSA_FAILURE;
            }
        }
            break;
        case OSA_RESOURCE_RELEASE:
        {
            /* insert the new task to the queue */
            OSA_TP_TASK_T *taskReq = (OSA_TP_TASK_T *)pExtraData;
            OSA_TP_TASK_NODE_T *taskNode = NULL;

            if (pThreadPool->bStopped==OSA_TRUE)
            {
                lRet = OSA_FAILURE;
                break;
            }

            taskNode = (OSA_TP_TASK_NODE_T *)OSA_MEM_ALLOC(sizeof(OSA_TP_TASK_NODE_T));

            if (taskNode==NULL)
            {
                OSA_DBG_ERROR("memory allocation failed for taskNode in osa_tp_resource_handler()");
                lRet = OSA_FAILURE;
                break;
            }

            osa_mem_set(taskNode, 0, sizeof(OSA_TP_TASK_NODE_T));

            /* assign task_id here */
            if (taskReq->task_id==0)
            {
                pTpCtrl->max_task_id++;
                taskReq->task_id = pTpCtrl->max_task_id;
            }

            taskNode->task = *taskReq;
            taskNode->status = OSA_TP_TASK_STATUS__QUEUED;
            taskNode->pThreadPool = pThreadPool;
            taskNode->prev = NULL;
            taskNode->next = NULL;

            switch (taskReq->prio)
            {
                case OSA_TP_TASK_PRIO__HIGH:
                {
                    lRet = osa_tp_queue_append(&pTpCtrl->highQueue,taskNode);
                }
                    break;
                case OSA_TP_TASK_PRIO__MED:
                {
                    lRet = osa_tp_queue_append(&pTpCtrl->medQueue,taskNode);
                }
                    break;
                case OSA_TP_TASK_PRIO__LOW:
                {
                    lRet = osa_tp_queue_append(&pTpCtrl->lowQueue,taskNode);
                }
                default:
                    break;
            }

            if (lRet == OSA_FAILURE)
            {
                OSA_MEM_FREE(taskNode);
            }
        }
            break;
        case OSA_RESOURCE_CANCEL:
        {
            UINT32 taskId = (UINT32)pExtraData;
            OSA_TP_TASK_NODE_T *pTaskNode = NULL;

            if (pThreadPool->bStopped==OSA_TRUE)
            {
                lRet = OSA_FAILURE;
                break;
            }

            if (pTpCtrl->highQueue
                && OSA_SUCCESS==osa_tp_queue_remove_task_by_id(&pTpCtrl->highQueue,&pTaskNode,taskId)
                && pTaskNode!=NULL)
            {
                /* stop time if necessary */
                if (pTaskNode->task.sched_type==OSA_TP_TASK_SCHED__SCHED
                    && pTaskNode->task.sched_interval>0)
                {
                    osa_timer_stop(&pTaskNode->timer);
                }
                OSA_MEM_FREE(pTaskNode);
                lRet = OSA_SUCCESS;
            }
            else if (pTpCtrl->medQueue
                && OSA_SUCCESS==osa_tp_queue_remove_task_by_id(&pTpCtrl->medQueue,&pTaskNode,taskId)
                && pTaskNode!=NULL)
            {
                if (pTaskNode->task.sched_type==OSA_TP_TASK_SCHED__SCHED
                    && pTaskNode->task.sched_interval>0)
                {
                    osa_timer_stop(&pTaskNode->timer);
                }
                OSA_MEM_FREE(pTaskNode);
                lRet = OSA_SUCCESS;
            }
            else if (pTpCtrl->lowQueue
                && OSA_SUCCESS==osa_tp_queue_remove_task_by_id(&pTpCtrl->lowQueue,&pTaskNode,taskId)
                && pTaskNode!=NULL)
            {
                if (pTaskNode->task.sched_type==OSA_TP_TASK_SCHED__SCHED
                    && pTaskNode->task.sched_interval>0)
                {
                    osa_timer_stop(&pTaskNode->timer);
                }
                OSA_MEM_FREE(pTaskNode);
                lRet = OSA_SUCCESS;
            }
        }
            break;
        case OSA_RESOURCE_GRP_DELETED:
        {
            UINT32 cnt = 0;

            if (pThreadPool->bStopped==OSA_TRUE)
            {
                lRet = OSA_FAILURE;
                break;
            }

            /* notify all threads to stop */
            for (cnt=0;cnt<pThreadPool->threads;cnt++)
            {
                /*if (pTpCtrl->threadPool[cnt].status == OSA_TP_TASK_THREAD_STATUS__RUNNING)
                {
                    osa_thread_delete(&pTpCtrl->threadPool[cnt].thread);
                }*/

                pTpCtrl->threadPool[cnt].status = OSA_TP_TASK_THREAD_STATUS__STOPPED;
            }

            pThreadPool->bStopped = OSA_TRUE;

            lRet = OSA_SUCCESS;
        }
            break;
        default:
            break;
    }

    return lRet;
}

static void osa_tp_timer_handler(OSA_TP_TASK_NODE_T *pTaskNode)
{
    INT32 lRet = OSA_FAILURE;
    OSA_THREAD_POOL_T *pThreadPool = (OSA_THREAD_POOL_T *)pTaskNode->pThreadPool;

    /* just call osa_thread_pool_queue_task() again */
    lRet = osa_thread_pool_queue_task(pThreadPool, &pTaskNode->task);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to requeue the task");
    }
    else
    {
        OSA_DBG_TRACE("the task requeued");
    }

    OSA_MEM_FREE(pTaskNode);

    return;
}

static void osa_tp_thread_handler(OSA_TP_THREAD_T *threadInstance)
{
    INT32 lRet = OSA_FAILURE;
    OSA_THREAD_POOL_T *pThreadPool = (OSA_THREAD_POOL_T *)threadInstance->pThreadPool;
    OSA_TP_CTRL_T *pTpCtrl = (OSA_TP_CTRL_T *)pThreadPool->ctrl;
    BOOL8 bDirectRequeue = OSA_FALSE;
    BOOL8 bSchedStopped = OSA_FALSE;

    while(threadInstance && threadInstance->status!=OSA_TP_TASK_THREAD_STATUS__STOPPED)
    {
        if (bDirectRequeue==OSA_FALSE)
        {
            lRet = osa_resource_wait(&pTpCtrl->taskGroup,(void *)threadInstance,OSA_TP_TASK_TIMEOUT_MS);
        }

        if (lRet==OSA_SUCCESS
            && threadInstance->currentTask
            && threadInstance->status!=OSA_TP_TASK_THREAD_STATUS__STOPPED)
        {
            /* find the task, so here to execute it and not check the result */
            bSchedStopped = threadInstance->currentTask->task.task_cb(threadInstance->currentTask->task.data);

            switch (threadInstance->currentTask->task.sched_type)
            {
                case OSA_TP_TASK_SCHED__DIRECT:
                {
                    if (bSchedStopped == OSA_TRUE)
                    {
                        /* stop the schedule */
                        bDirectRequeue = OSA_FALSE;
                        OSA_MEM_FREE(threadInstance->currentTask);
                    }
                    else
                    {
                        /* continue with this task node */
                        bDirectRequeue = OSA_TRUE;
                    }
                }
                    break;
                case OSA_TP_TASK_SCHED__SCHED:
                {
                    if (bSchedStopped == OSA_TRUE)
                    {
                        /* stop the schedule */
                        OSA_MEM_FREE(threadInstance->currentTask);
                    }
                    else if (threadInstance->currentTask->task.sched_interval == 0)
                    {
                        /* just call osa_thread_pool_queue_task() again */
                        lRet = osa_thread_pool_queue_task(pThreadPool, &threadInstance->currentTask->task);

                        if (lRet != OSA_SUCCESS)
                        {
                            OSA_DBG_ERROR("failed to requeue the task");
                        }

                        OSA_MEM_FREE(threadInstance->currentTask);
                    }
                    else
                    {
                        /* start a timer to schedule the task */
                        lRet = osa_timer_start(&threadInstance->currentTask->timer,
                                            "pltmr",
                                            (OSA_TIMER_TMRCB_PFN)osa_tp_timer_handler,
                                            (UINT32)threadInstance->currentTask,
                                            threadInstance->currentTask->task.sched_interval,
                                            OSA_TIMER_PROP_ONCE);

                        if (lRet != OSA_SUCCESS)
                        {
                            OSA_DBG_ERROR("failed to start timer to schedule the task");
                            OSA_MEM_FREE(threadInstance->currentTask);
                        }

                        /* not release pTaskNode, it will be freed when timeout */
                    }

                    bDirectRequeue = OSA_FALSE;
                }
                    break;
                case OSA_TP_TASK_SCHED__NONE:
                default:
                    /* nothing to do, just leave */
                    OSA_MEM_FREE(threadInstance->currentTask);
                    bDirectRequeue = OSA_FALSE;
                    break;
            }
        }
        else
        {
            OSA_DBG_DEBUG("nothing found, continue to wait");
        }
    }

    return;
}

INT32 osa_thread_pool_create( OSA_THREAD_POOL_T *pThreadPool, INT8 *pName, BOOL8 bSchedSupported, UINT32 threads, OSA_THREAD_SCHED_E eSched, OSA_THREAD_PRIO_E ePrio)
{
    INT32 lRet = OSA_FAILURE;
    UINT32 cnt = 0;
    OSA_TP_CTRL_T *pTpCtrl = NULL;

    if (pThreadPool == NULL)
    {
        OSA_DBG_ERROR("null arguments in osa_thread_pool_create()");
        return lRet;
    }

    if (threads <= 0 || threads >10)
    {
        OSA_DBG_ERROR("invalid threads %u not in [0,10], in osa_thread_pool_create()",threads);
        return lRet;
    }

    /* backup arguments */
    if (!pName)
    {
        pName = "threadpool";
    }

    snprintf(pThreadPool->prefix,sizeof(pThreadPool->prefix),"%s",pName);
    pThreadPool->threads = threads;
    pThreadPool->eSched = eSched;
    pThreadPool->ePri = ePrio;
    pThreadPool->bSchedSupported = bSchedSupported;

    pTpCtrl = (OSA_TP_CTRL_T *)OSA_MEM_ALLOC(sizeof(OSA_TP_CTRL_T));

    if (pTpCtrl==NULL)
    {
        OSA_DBG_ERROR("memory allocated for pThreadPoolCtrl failed, in osa_thread_pool_create()");
        return lRet;
    }

    osa_mem_set(pTpCtrl, 0, sizeof(OSA_TP_CTRL_T));

    pTpCtrl->threadPool = (OSA_TP_THREAD_T *)OSA_MEM_ALLOC(sizeof(OSA_TP_THREAD_T)*pThreadPool->threads);

    if (pTpCtrl->threadPool==NULL)
    {
        OSA_DBG_ERROR("memory allocated for pTpCtrl->threadPool failed, in osa_thread_pool_create()");
        OSA_MEM_FREE(pTpCtrl);
        return lRet;
    }

    osa_mem_set(pTpCtrl->threadPool, 0, sizeof(OSA_TP_THREAD_T)*pThreadPool->threads);

    /* create resource-group */
    lRet = osa_resource_group_create(&pTpCtrl->taskGroup, pThreadPool, (OSA_RESOURCE_CB_FUN)osa_tp_resource_handler);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to create event group, in osa_thread_pool_create()");
        OSA_MEM_FREE(pTpCtrl->threadPool);
        OSA_MEM_FREE(pTpCtrl);
        return lRet;
    }

    /* create resource-group */
    lRet = osa_thread_mutex_create(&pTpCtrl->apiMutex);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to create event group, in osa_thread_pool_create()");
        osa_resource_group_delete(&pTpCtrl->taskGroup);
        OSA_MEM_FREE(pTpCtrl->threadPool);
        OSA_MEM_FREE(pTpCtrl);
        return lRet;
    }

    pThreadPool->ctrl = (void *)pTpCtrl;

    /* create threads */
    for (cnt=0;cnt<pThreadPool->threads;cnt++)
    {
        INT8 threadName[OSA_TREAD_POOL_NAME_LEN+1] = {0};

        snprintf(threadName,sizeof(threadName),"%s%u",pThreadPool->prefix,cnt);

        pTpCtrl->threadPool[cnt].status = OSA_TP_TASK_THREAD_STATUS__IDLE;
        pTpCtrl->threadPool[cnt].currentTask = NULL;
        pTpCtrl->threadPool[cnt].serviceLevel = osa_tp_get_service_level(cnt,pThreadPool->threads);
        pTpCtrl->threadPool[cnt].pThreadPool = pThreadPool;

        lRet = osa_thread_create(&pTpCtrl->threadPool[cnt].thread,
                                threadName,
                                (OSA_THREAD_PFN)osa_tp_thread_handler,
                                (UINT32)&pTpCtrl->threadPool[cnt],
                                0xf000,
                                pThreadPool->eSched,
                                pThreadPool->ePri,
                                OSA_THREAD_PROP_WAITABLE);

        if (lRet != OSA_SUCCESS)
        {
            OSA_DBG_ERROR("failed to create thread%u in pool, in osa_thread_pool_create()",cnt);
            break;
        }
        else
        {
            OSA_DBG_DEBUG("create thread%u in pool, in osa_thread_pool_create()",cnt);
        }
    }

    if (lRet != OSA_SUCCESS)
    {
        UINT32 iCnt = 0;
        /* release resources */
        for (iCnt=0;iCnt<cnt;iCnt++)
        {
            osa_thread_delete(&pTpCtrl->threadPool[iCnt].thread);
        }

        osa_resource_group_delete(&pTpCtrl->taskGroup);
        osa_thread_mutex_delete(&pTpCtrl->apiMutex);

        OSA_MEM_FREE(pTpCtrl->threadPool);
        OSA_MEM_FREE(pTpCtrl);
    }

    return lRet;
}

INT32 osa_thread_pool_delete( OSA_THREAD_POOL_T *pThreadPool)
{
    INT32 lRet = OSA_FAILURE;
    OSA_TP_CTRL_T *pTpCtrl = NULL;
    UINT32 cnt = 0;

    if (pThreadPool==NULL || pThreadPool->ctrl==NULL)
    {
        OSA_DBG_ERROR("null arguments in osa_thread_pool_delete()");
        return lRet;
    }

    pTpCtrl = (OSA_TP_CTRL_T *)pThreadPool->ctrl;

    lRet = osa_thread_mutex_lock(&pTpCtrl->apiMutex);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to lock thread pool, in osa_thread_pool_delete()");
        return lRet;
    }

    if (pThreadPool->bStopped==OSA_TRUE)
    {
        OSA_DBG_ERROR("thread pool stopped in osa_thread_pool_delete()");
        lRet = osa_thread_mutex_unlock(&pTpCtrl->apiMutex);

        if (lRet != OSA_SUCCESS)
        {
            OSA_DBG_ERROR("failed to unlock thread pool, in osa_thread_pool_delete()");
        }

        return lRet;
    }

    lRet = osa_resource_group_delete(&pTpCtrl->taskGroup);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to queue the task in osa_thread_pool_delete()");
    }

    /* wait for all threads stopped */
    for (cnt=0;cnt<pThreadPool->threads;cnt++)
    {
        osa_thread_wait(&pTpCtrl->threadPool[cnt].thread);
    }

    /* remove all tasks and timers later in case thread is using task */
    if (pTpCtrl->highQueue)
    {
        osa_tp_queue_clear(&pTpCtrl->highQueue);
    }
    
    if (pTpCtrl->medQueue)
    {
        osa_tp_queue_clear(&pTpCtrl->medQueue);
    }
    
    if (pTpCtrl->lowQueue)
    {
        osa_tp_queue_clear(&pTpCtrl->lowQueue);
    }

    lRet = osa_thread_mutex_unlock(&pTpCtrl->apiMutex);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to unlock thread pool, in osa_thread_pool_delete()");
        return lRet;
    }

    /* release resources */
    //osa_thread_mutex_delete(&pTpCtrl->apiMutex);
    OSA_MEM_FREE(pTpCtrl->threadPool);
    OSA_MEM_FREE(pTpCtrl);
    pThreadPool->ctrl = NULL;
    
    return lRet;
}

INT32 osa_thread_pool_queue_task( OSA_THREAD_POOL_T *pThreadPool, OSA_TP_TASK_T *pTask)
{
    INT32 lRet = OSA_FAILURE;
    OSA_TP_CTRL_T *pTpCtrl = NULL;

    if (pThreadPool==NULL || pThreadPool->ctrl==NULL || pTask==NULL || pTask->task_cb==NULL)
    {
        OSA_DBG_ERROR("null arguments in osa_thread_pool_queue_task()");
        return lRet;
    }

    if (pTask->sched_type!=OSA_TP_TASK_SCHED__NONE
        && pTask->sched_type!=OSA_TP_TASK_SCHED__DIRECT
        && pTask->sched_type!=OSA_TP_TASK_SCHED__SCHED)
    {
        OSA_DBG_ERROR("invalid sched_type in osa_thread_pool_queue_task()");
        return lRet;
    }

    if (pTask->prio!=OSA_TP_TASK_PRIO__LOW
        && pTask->prio!=OSA_TP_TASK_PRIO__MED
        && pTask->prio!=OSA_TP_TASK_PRIO__HIGH)
    {
        OSA_DBG_ERROR("invalid prio in osa_thread_pool_queue_task()");
        return lRet;
    }

    pTpCtrl = (OSA_TP_CTRL_T *)pThreadPool->ctrl;

    lRet = osa_thread_mutex_lock(&pTpCtrl->apiMutex);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to lock thread pool, in osa_thread_pool_queue_task()");
        return lRet;
    }

    if (pThreadPool->bStopped==OSA_TRUE)
    {
        OSA_DBG_ERROR("thread pool stopped in osa_thread_pool_queue_task()");
        return lRet;
    }

    pTpCtrl = (OSA_TP_CTRL_T *)pThreadPool->ctrl;

    lRet = osa_resource_release(&pTpCtrl->taskGroup, pTask);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to queue the task in osa_thread_pool_queue_task()");
    }

    lRet = osa_thread_mutex_unlock(&pTpCtrl->apiMutex);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to unlock thread pool, in osa_thread_pool_queue_task()");
        return lRet;
    }

    return lRet;
}

INT32 osa_thread_pool_cancel_task( OSA_THREAD_POOL_T *pThreadPool, OSA_TP_TASK_T *pTask)
{
    INT32 lRet = OSA_FAILURE;
    OSA_TP_CTRL_T *pTpCtrl = NULL;

    if (pThreadPool==NULL || pThreadPool->ctrl==NULL || pTask==NULL || pTask->task_id==0)
    {
        OSA_DBG_ERROR("null arguments in osa_thread_pool_cancel_task()");
        return lRet;
    }

    lRet = osa_thread_mutex_lock(&pTpCtrl->apiMutex);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to lock thread pool, in osa_thread_pool_cancel_task()");
        return lRet;
    }

    if (pThreadPool->bStopped==OSA_TRUE)
    {
        OSA_DBG_ERROR("thread pool stopped in osa_thread_pool_cancel_task()");
        return lRet;
    }

    pTpCtrl = (OSA_TP_CTRL_T *)pThreadPool->ctrl;

    if (pTask->task_id > pTpCtrl->max_task_id)
    {
        OSA_DBG_ERROR("invalid taskId in osa_thread_pool_cancel_task()");
        return lRet;
    }

    lRet = osa_resource_cancel(&pTpCtrl->taskGroup,(void*)pTask->task_id);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to cancel the task in osa_thread_pool_cancel_task()");
    }

    lRet = osa_thread_mutex_unlock(&pTpCtrl->apiMutex);

    if (lRet != OSA_SUCCESS)
    {
        OSA_DBG_ERROR("failed to unlock thread pool, in osa_thread_pool_cancel_task()");
        return lRet;
    }

    return lRet;
}


#ifdef __cplusplus
#if __cplusplus
        }
#endif 
#endif/* __cpluscplus */


