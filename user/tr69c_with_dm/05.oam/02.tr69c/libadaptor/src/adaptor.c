#include <ctype.h>
#include <wchar.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include "prc_ps_log.h"

#include "adaptor.h"
#include "tr69cdefs.h"
#include "event.h"
#include "bcmConfig.h"
#include "bcmWrapper.h"
#include "informer_public.h"
#include "osa.h"
#include "dm_interface.h"
#include "eme_sock_msg_def.h"
//#include "prc_perview.h"
//#include "debug_def.h"
//#include "swd_interface.h"
#include "device_model_def.h"
#include "internal_model_def.h"
#include "libopenssl.h"
//#include "devs_interface.h"
#include "tr069_db_cbs.h"


extern char* writeIp(struct sockaddr * ip);

extern eSessionState sessionState;
extern void *msgHandle;
extern ACSState acsState;
extern UBOOL8 Booting;
extern UBOOL8 swd_enabled;

static UINT32 emaEntityId = 0;
static UBOOL8 emaInSession = FALSE;

CmsRet updateEmaSession(UBOOL8 isAccuquire)
{
	CmsRet ret = CMSRET_INTERNAL_ERROR;

    INTERNAL_EMA_SESSIONSTATUS_T SessionStatus;
    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    DM_SET_PARA_C_VALUE_REQ_T setValue;
    char *pucPath = "Internal.Ema.SessionStatus.";

    cmsLog_debug("enter updateEmaSession");

	memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
	memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));
	memset(&setValue, 0, sizeof(DM_SET_PARA_C_VALUE_REQ_T));
	memset(&SessionStatus, 0, sizeof(INTERNAL_EMA_SESSIONSTATUS_T));

	strcpy(transReq.AccessEntity,"ROOT");
	strcpy(transReq.RootNodeToLock, pucPath);
	transReq.DataFlush = DM_TRUE;
	transReq.TransactionId = 0;

	INT32 dmRet = DmTransactionStartReq(&transReq);
	if (dmRet != DM_RET_SUCCESS)
	{
		cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave updateEmaSession",ret);
	}
	else
	{
		getValue.TransactionId = transReq.TransactionId;
		getValue.pucPath = pucPath;
		getValue.pValue = (void *)&SessionStatus;
		getValue.ValueSize = sizeof(INTERNAL_EMA_SESSIONSTATUS_T);

		dmRet = DmGetParaCValueReq(&getValue);
		if (dmRet != DM_RET_SUCCESS)
		{
			cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR, "DmGetParaCValueReq failed lRet=%d, pucPath=%s,leave updateEmaSession", ret, pucPath);
		}
		else
		{
			if(isAccuquire && SessionStatus.EntityId != 0 && SessionStatus.EntityId != emaEntityId)
			{
				cmsLog_notice("EME session is locked by other entity(id=%d).", SessionStatus.EntityId);
			}
			else if(!isAccuquire && SessionStatus.EntityId != emaEntityId)
			{
				cmsLog_error(TR069_ALARM_INTERNAL_ERROR, "Try to release a EME session locked by other entity(id=%d).", SessionStatus.EntityId);
			}
			else
			{
				SessionStatus.EntityId  = isAccuquire ? emaEntityId : 0;
				SessionStatus.UpdateTime = time(NULL);

				setValue.TransactionId = transReq.TransactionId;
				setValue.pucPath = pucPath;
				setValue.ulCnt = 1;
				setValue.pValue = (void *)&SessionStatus;
				setValue.ValueSize = sizeof(INTERNAL_EMA_SESSIONSTATUS_T);

				dmRet = DmSetParaCValueReq(&setValue);
				if (dmRet != DM_RET_SUCCESS)
				{
					cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmSetParaCValueReq failed lRet=%d, leave updateEmaSession",ret);
				}
				else
				{
					ret = CMSRET_SUCCESS;
				}
			}
		}

		dmRet = DmTransactionStopReq(transReq.TransactionId);
		if (dmRet != DM_RET_SUCCESS)
		{
			cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStopReq failed lRet=%d, leave updateEmaSession",ret);
		}
	}

	cmsLog_debug("release updateEmaSession");

    return ret;
}

UBOOL8 checkEmeSession(void)
{
    cmsLog_notice("enter checkEmeSession");

    if(!emaInSession)
    {
    	emaInSession = (CMSRET_SUCCESS == updateEmaSession(TRUE));
    	cmsLog_notice("TR69c accquried ema session (status=%d)", emaInSession);
    }
    else
    {
    	cmsLog_notice("TR69c has accquried ema session.");
    }

    cmsLog_notice("release checkEmeSession");

    return emaInSession;
}

void releaseEmeSessionIfAccquired(void)
{
    cmsLog_notice("enter releaseEmeSessionIfAccquired");

    if(emaInSession)
    {
    	cmsLog_notice("TR69c release ema session");
    	emaInSession=FALSE;

    	updateEmaSession(FALSE);
    }

    cmsLog_notice("release releaseEmeSessionIfAccquired");
}

static CmsLogLevel       logLevel = LOG_LEVEL_NOTICE;
static CmsLogDestination logDestination;

void log_log(CmsLogLevel level, const char *file, const char *func, UINT32 lineNum, UINT32 alarmid, const char *pFmt, ... )
{
    va_list      ap;
    char buf[MAX_LOG_LINE_LENGTH] = {0};

    if (logLevel<level)
    {
        return;
    }

    va_start(ap, pFmt);
    vsnprintf(buf, sizeof(buf), pFmt, ap);
    va_end(ap);

    printf("<TR69C>  %s(%d)  %s\n",func,lineNum,buf);
#if 0
    switch(level)
    {
        case LOG_LEVEL_LOGGER:
            DevicelogReportInd(REPORTER_ID_TR69,buf);
            break;
        case LOG_LEVEL_ERR:
            /* alarm raised */
            AlarmReportInd(alarmid,(void*)file,lineNum,buf);
            break;
        case LOG_LEVEL_NOTICE:
            ps_board_trace(GROUP_OAM_TR069,TRACE_LEVEL_HIGH,"<TR69C>  %s(%d)  %s",func,lineNum,buf);
            break;
        case LOG_LEVEL_DEBUG:
            ps_board_trace(GROUP_OAM_TR069,TRACE_LEVEL_LOW,"<TR69C>  %s(%d)  %s",func,lineNum,buf);
            break;
        default:
            break;
    }
#endif
    return;
}

static void cmsDmTraceHooker(const INT8 *file, INT32 line, UINT16 traceLevel, UINT32 errType, INT8 *pTraceContext)
{
    //UINT16 usLevel  = TRACE_LEVEL_LOW;

    if (pTraceContext != NULL)
    {
#if 0
        switch(traceLevel)
        {
            case DM_TRACE_LEVEL_LOW:
                usLevel  = TRACE_LEVEL_LOW;
                break;
            case DM_TRACE_LEVEL_MIDDLE:
                usLevel  = TRACE_LEVEL_MED;
                break;
            case DM_TRACE_LEVEL_HIGH:
                usLevel  = TRACE_LEVEL_HIGH;
                break;
            case DM_TRACE_LEVEL_WARNING:
                usLevel  = TRACE_LEVEL_WARNING;
                break;
            case DM_TRACE_LEVEL_ERROR:
                usLevel  = TRACE_LEVEL_ERROR;
                break;
            case DM_TRACE_LEVEL_NONE:
            default:
                usLevel  = TRACE_LEVEL_LOW;
                break;
        }

        /* only output high info, warning and error... */
        //if (usLevel<=PRC_TRACE_INFO_HIGH)
        {
            ps_board_trace(GROUP_OAM_DM,usLevel,"<TR69c> %s",pTraceContext);
        }
#endif
      //wwzh  printf("<TR69c> %s\n",pTraceContext);

    }

    return;
}

static VOID cmsOsaTraceHooker(INT32 traceLevel, const INT8 *file, INT32 line, INT8 *pTraceContext)
{
	//UINT16 usLevel  = TRACE_LEVEL_LOW;
    //UINT8 ucEventId = 0;
	//INT8 traces[256] = {0};

	if (pTraceContext != NULL)
	{
#if 0
		switch(traceLevel&OSA_DEBUG_LEVEL_MASK)
		{
			case OSA_DEBUG_LEVEL_DEBUG:
			{
				usLevel  = TRACE_LEVEL_LOW;
			}
			break;
			case OSA_DEBUG_LEVEL_TRACE:
			{
				usLevel  = TRACE_LEVEL_HIGH;
			}
			break;
			case OSA_DEBUG_LEVEL_WARNING:
			{
				usLevel  = TRACE_LEVEL_WARNING;
			}
			break;
			case OSA_DEBUG_LEVEL_ERROR:
			{
				usLevel  = TRACE_LEVEL_ERROR;/* here should raise an alarm instead later. */
			}
			break;
            case OSA_DEBUG_LEVEL_INFO:
            {
                ucEventId = traceLevel>>24;
                ps_board_log(GROUP_OSA_CPUH,ucEventId,pTraceContext,line,0xffff);
            }
            break;
			default:
				usLevel  = TRACE_LEVEL_LOW;
			break;
		}

		if ((traceLevel&OSA_DEBUG_LEVEL_MASK)<=OSA_DEBUG_LEVEL_ERROR)
		{
			snprintf(traces, sizeof(traces), "<Tr69c> OSA DBG --- f: %s l: %d %s", file, line, pTraceContext);
			ps_board_trace(GROUP_OSA_CPUH,usLevel,traces);
		}
#endif
       //wwzh printf("<Tr69c> OSA DBG --- f: %s l: %d %s\n", file, line, pTraceContext);
	}
	return;
}

void cmsLog_initWithName(CmsEntityId eid, const char *appName)
{
    //prc_trace_service_start(PRC_TRACE_SERVICE_AS_CLIENT,0);
    osa_debug_callback(cmsOsaTraceHooker);
    DmTraceHookerRegister(cmsDmTraceHooker);
    //AlarmReportStartReq("TR069");
    //DevicelogStartReq("TR069",NULL);
    return;
}

void cmsLog_cleanup(void)
{
    //AlarmReportStopReq();
    //prc_trace_service_stop();
    return;
}

void cmsLog_setLevel(CmsLogLevel level)
{
    logLevel = level;
    return;
}

void cmsLog_setDestination(CmsLogDestination dest)
{
    logDestination = dest;
    return;
}

SINT32 cmsLog_readPartial(SINT32 ptr, char* buffer)
{
    return 0;
}

/** Internal event timer structure
 */
typedef struct cms_timer_event
{
   struct cms_timer_event *next;      /**< pointer to the next timer. */
   CmsTimestamp            expireTms; /**< Timestamp (in the future) of when this
                                       *   timer event will expire. */
   CmsEventHandler         func;      /**< handler func to call when event expires. */
   void *                  ctxData;   /**< context data to pass to func */
   char name[CMS_EVENT_TIMER_NAME_LENGTH]; /**< name of this timer */
   CmsTimestamp            pausedTms; /**< Timestamp of when this timer was paused */
} CmsTimerEvent;


/** Internal timer handle. */
typedef struct
{
   CmsTimerEvent *events;     /**< Singly linked list of events */
   UINT32         numEvents;  /**< Number of events in this handle. */
} CmsTimerHandle;
#if 0
/*softdog*/
#define SWD_TR69C_FEED_DOG_TIMER_VALUE	1000*30

void  tr69FeedSoftdog(void *tmrHandle)
{
	SwdFeedDog();
	if (CMSRET_SUCCESS != cmsTmr_set(tmrHandle, 
                                     (CmsEventHandler)tr69FeedSoftdog, 
                                     tmrHandle, 
                                     SWD_TR69C_FEED_DOG_TIMER_VALUE, 
                                     "feedsoftdog"))
	{
		cmsLog_error(TR069_ALARM_TIMER_ERROR, "start Softdog feed timer for tr69c failed");
	}
	return;
}

static void tr69StartFeedSoftdog(void *tmrHandle)
{
    INT32 ret = SWD_FAILURE;
    SWD_START_REQ_T swdReq;

    memset(&swdReq,0,sizeof(SWD_START_REQ_T));

    snprintf(swdReq.name,sizeof(swdReq.name),"TR69C");
    swdReq.checkInterval = 0;
    swdReq.alarmId = 0;
    swdReq.sigNum = 0;
    swdReq.resetInterval_fle = 0;
    swdReq.options = SWD_OPTION__ENABLE_SW_FEED | SWD_OPTION__ENABLE_FLE_CHECK | SWD_OPTION__ABORT_IF_NO_USER_RESET;

	ret = SwdStartReq(&swdReq);
	
	if(ret==SWD_SUCCESS)
	{
        tr69FeedSoftdog(tmrHandle);
	}
	return;
}

static void tr69StopFeedSoftdog(void)
{
	SwdStopReq();
	return;
}
#endif
CmsRet cmsTmr_init(void **tmrHandle)
{
	(*tmrHandle) = cmsMem_alloc(sizeof(CmsTimerHandle), ALLOC_ZEROIZE);
	if ((*tmrHandle) == NULL)
	{
		cmsLog_error(TR069_ALARM_TIMER_ERROR,"could not malloc mem for tmrHandle");
		return CMSRET_RESOURCE_EXCEEDED;
	}

    if (swd_enabled==TRUE)
    {
	    //tr69StartFeedSoftdog(*tmrHandle);
    }
	return CMSRET_SUCCESS;
}

void cmsTmr_cleanup(void **handle)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *tmrEvent;

   while ((tmrEvent = tmrHandle->events) != NULL)
   {
      tmrHandle->events = tmrEvent->next;
      CMSMEM_FREE_BUF_AND_NULL_PTR(tmrEvent);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR((*handle));

   //tr69StopFeedSoftdog();

   return;
}

#define IS_EARLIER_THAN(a, b) (((a)->sec < (b)->sec) || \
                               (((a)->sec == (b)->sec) && ((a)->nsec < (b)->nsec)))


void _cmsTmr_insert(CmsTimerHandle *tmrHandle, CmsTimerEvent *newEvent)
{
   CmsTimerEvent *currEvent, *prevEvent;
   /* 
    * Now we just need to insert it in the correct place in the timer handle.
    * We just insert the events in absolute order, i.e. smallest expire timer
    * at the head of the queue, largest at the end of the queue.  If the
    * modem is up long enough where timestamp rollover is an issue (139 years!)
    * cmsTmr_executeExpiredEvents and cmsTmr_getTimeToNextEvent will have to
    * be careful about where they pick the next timer to expire.
    */
   if (tmrHandle->numEvents == 0)
   {
      tmrHandle->events = newEvent;
   }
   else 
   {
      currEvent = tmrHandle->events;

      if (IS_EARLIER_THAN(&(newEvent->expireTms), &(currEvent->expireTms)) ||
         !((currEvent->pausedTms.sec == 0) && (currEvent->pausedTms.nsec == 0)))
      {
         /* queue at the head */
         newEvent->next = currEvent;
         tmrHandle->events = newEvent;
      }
      else
      {
         UBOOL8 done = FALSE;

         while (!done)
         {
            prevEvent = currEvent;
            currEvent = currEvent->next;

            if ((currEvent == NULL) ||
                (IS_EARLIER_THAN(&(newEvent->expireTms), &(currEvent->expireTms))) ||
                !((currEvent->pausedTms.sec == 0) && (currEvent->pausedTms.nsec == 0)))
            {
               newEvent->next = prevEvent->next;
               prevEvent->next = newEvent;
               done = TRUE;
            }
         }
      }
   }

   tmrHandle->numEvents++;

   return;
}

CmsRet cmsTmr_set(void *handle, CmsEventHandler func, void *ctxData, UINT32 ms, const char *name)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *newEvent;

   /*
    * First verify there is not a duplicate event.
    * (The original code first deleted any existing timer,
    * which is a "side-effect", bad style, but maybe tr69c requires
    * that functionality?)
    */
   if (cmsTmr_isEventPresent(handle, func, ctxData))
   {
      cmsLog_error(TR069_ALARM_TIMER_ERROR,"There is already an event func 0x%x ctxData 0x%x",
                   func, ctxData);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* make sure name is not too long */
   if ((name != NULL) && (strlen(name) >= CMS_EVENT_TIMER_NAME_LENGTH))
   {
      cmsLog_error(TR069_ALARM_TIMER_ERROR,"name of timer event is too long, max %d", CMS_EVENT_TIMER_NAME_LENGTH);
      return CMSRET_INVALID_ARGUMENTS;
   }


   /*
    * Allocate a structure for the timer event.
    */
   newEvent = cmsMem_alloc(sizeof(CmsTimerEvent), ALLOC_ZEROIZE);
   if (newEvent == NULL)
   {
      cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"malloc of new timer event failed");
      return CMSRET_RESOURCE_EXCEEDED;
   }

   /* fill in fields of new event timer structure. */
   newEvent->func = func;
   newEvent->ctxData = ctxData;

   cmsTms_get(&(newEvent->expireTms));
   cmsTms_addMilliSeconds(&(newEvent->expireTms), ms);

   if (name != NULL)
   {
      sprintf(newEvent->name, "%s", name);
   }

   /* Insert the new event timer in the timer handle */
   _cmsTmr_insert(tmrHandle, newEvent);

   cmsLog_debug("added event %s, expires in %ums (at %u.%03u), func=0x%x data=%p numEvents=%d",
                newEvent->name,
                ms,
                newEvent->expireTms.sec,
                newEvent->expireTms.nsec/NSECS_IN_MSEC,
                func,
                ctxData,
                tmrHandle->numEvents);

   return CMSRET_SUCCESS;
}

CmsTimerEvent * _cmsTmr_remove(CmsTimerHandle *tmrHandle, CmsEventHandler func, void *ctxData)
{
   CmsTimerEvent *currEvent, *prevEvent;

   if ((currEvent = tmrHandle->events) == NULL)
   {
      return NULL;
   }

   if (currEvent->func == func && currEvent->ctxData == ctxData)
   {
      /* remove from head of the queue */
      tmrHandle->events = currEvent->next;
      currEvent->next = NULL;
   }
   else
   {
      UBOOL8 done = FALSE;

      while ((currEvent != NULL) && (!done))
      {
         prevEvent = currEvent;
         currEvent = currEvent->next;

         if (currEvent != NULL && currEvent->func == func && currEvent->ctxData == ctxData)
         {
            prevEvent->next = currEvent->next;
            currEvent->next = NULL;
            done = TRUE;
         }
      }
   }

   if (currEvent != NULL)
   {
      tmrHandle->numEvents--;
   }

   return currEvent;
}

void cmsTmr_cancel(void *handle, CmsEventHandler func, void *ctxData)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *currEvent;

   if ((currEvent = _cmsTmr_remove(tmrHandle, func, ctxData)) == NULL)
   {
      cmsLog_debug("no events to cancel (func=0x%x data=%p)", func, ctxData);
      return;
   }

   cmsLog_debug("canceled event %s, numEvents=%d", currEvent->name, tmrHandle->numEvents);

   CMSMEM_FREE_BUF_AND_NULL_PTR(currEvent);

   return;
}

CmsRet cmsTmr_replaceIfSooner(void *handle, CmsEventHandler func, void *ctxData, UINT32 ms, const char *name)
{
    CmsTimestamp nowTms;
    CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
    CmsTimerEvent *tmrEvent;
    UBOOL8 found=FALSE;
    
    tmrEvent = tmrHandle->events;
    
    while ((tmrEvent != NULL) && (!found))
    {
       if (tmrEvent->func == func && tmrEvent->ctxData == ctxData)
       {
          found = TRUE;
       }
       else
       {
          tmrEvent = tmrEvent->next;
       }
    }
    if (found)
    {
       /* find out the expire time of this event.  If it's sooner then the one in the 
        * timer list, then replace the one in list with this one.
        */
       cmsTms_get(&nowTms);
       cmsTms_addMilliSeconds(&nowTms, ms);
       if (IS_EARLIER_THAN(&nowTms, &(tmrEvent->expireTms)))
       {
          cmsTmr_cancel((void*)tmrHandle, func, (void*)NULL);
       }
       else
       {
          return CMSRET_SUCCESS;
       }
    } /* found */
    return(cmsTmr_set(handle, func, ctxData, ms, name));

}

CmsRet cmsTmr_getTimeToNextEvent(const void *handle, UINT32 *ms)
{
   const CmsTimerHandle *tmrHandle = (const CmsTimerHandle *) handle;
   CmsTimerEvent *currEvent;
   CmsTimestamp nowTms;

   cmsTms_get(&nowTms);
   currEvent = tmrHandle->events;

   if (currEvent == NULL)
   {
      *ms = MAX_UINT32;
      return CMSRET_NO_MORE_INSTANCES;
   }

   /* this is the same code as in dumpEvents, integrate? */
   if (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms))
   {
      /*
       * the next event is past due (nowTms is later than currEvent),
       * so time to next event is 0.
       */
      *ms = 0;
   }
   else
   {
      /*
       * nowTms is earlier than currEvent, so currEvent is still in
       * the future.  
       */
      (*ms) = cmsTms_deltaInMilliSeconds(&(currEvent->expireTms), &nowTms);
   }

   return CMSRET_SUCCESS;
}

UINT32 cmsTmr_getNumberOfEvents(const void *handle)
{
   const CmsTimerHandle *tmrHandle = (const CmsTimerHandle *) handle;

   return (tmrHandle->numEvents);
}

void cmsTmr_executeExpiredEvents(void *handle)
{
   CmsTimerHandle *tmrHandle = (CmsTimerHandle *) handle;
   CmsTimerEvent *currEvent;
   CmsTimestamp nowTms;

   cmsTms_get(&nowTms);
   currEvent = tmrHandle->events;

   while ((currEvent != NULL) && (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms)) &&
          (currEvent->pausedTms.sec == 0) && (currEvent->pausedTms.nsec == 0))
   {
      /*
       * first remove the currEvent from the tmrHandle because
       * when we execute the callback function, it might call the
       * cmsTmr API again.
       */
      tmrHandle->events = currEvent->next;
      currEvent->next = NULL;
      tmrHandle->numEvents--;

      cmsLog_notice("executing timer event %s func 0x%x data 0x%x expireTms %u.%03us nowTms %u.%03us numEvents=%d",
                   currEvent->name, currEvent->func, currEvent->ctxData, currEvent->expireTms.sec, currEvent->expireTms.nsec/NSECS_IN_MSEC, nowTms.sec, nowTms.nsec/NSECS_IN_MSEC, tmrHandle->numEvents);

      /* call the function */
      (*currEvent->func)(currEvent->ctxData);

      /* free the event struct */
      CMSMEM_FREE_BUF_AND_NULL_PTR(currEvent);

      currEvent = tmrHandle->events;
   }

   return;
}

UBOOL8 cmsTmr_isEventPresent(const void *handle, CmsEventHandler func, void *ctxData)
{
   const CmsTimerHandle *tmrHandle = (const CmsTimerHandle *) handle;
   CmsTimerEvent *tmrEvent;
   UBOOL8 found=FALSE;

   tmrEvent = tmrHandle->events;

   while ((tmrEvent != NULL) && (!found))
   {
      if (tmrEvent->func == func && tmrEvent->ctxData == ctxData)
      {
         found = TRUE;
      }
      else
      {
         tmrEvent = tmrEvent->next;
      }
   }

   return found;

}

void cmsTmr_dumpEvents(const void *handle)
{
#ifdef CMS_LOG3
       const CmsTimerHandle *tmrHandle = (const CmsTimerHandle *) handle;
       CmsTimerEvent *currEvent;
       CmsTimestamp nowTms;
       UINT32 expires;
    
       cmsLog_debug("dumping %d events", tmrHandle->numEvents);
       cmsTms_get(&nowTms);
    
       currEvent = tmrHandle->events;
    
       while (currEvent != NULL)
       {
    
          /* this is the same code as in getTimeToNextEvent, integrate? */
          if (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms))
          {
             /*
              * the currentevent is past due (nowTms is later than currEvent),
              * so expiry time is 0.
              */
             expires = 0;
          }
          else
          {
             /*
              * nowTms is earlier than currEvent, so currEvent is still in
              * the future.  
              */
             expires = cmsTms_deltaInMilliSeconds(&(currEvent->expireTms), &nowTms);
          }
    
    
          cmsLog_debug("event %s expires in %ums (at %u.%03u) func=0x%x data=%p",
                       currEvent->name,
                       expires,
                       currEvent->expireTms.sec,
                       currEvent->expireTms.nsec/NSECS_IN_MSEC,
                       currEvent->func,
                       currEvent->ctxData);
    
          currEvent = currEvent->next;
       }
#endif
       return;

}

#define CMS_MEM_HEADER_LENGTH   12
#define REAL_ALLOC_SIZE(s) (CMS_MEM_HEADER_LENGTH + (s))

void *cmsMem_alloc(UINT32 size, UINT32 allocFlags)
{
    void *buf = NULL;

    if (size <= 0)
    {
        return buf;
    }

    buf =osa_mem_alloc(REAL_ALLOC_SIZE(size));

    if (buf != NULL)
    {
        UINT32 *intBuf = (UINT32 *) buf;

        intBuf[0] = allocFlags;
        intBuf[1] = size;
        intBuf[2] = intBuf[1] ^ 0xffffffff;
        buf = &(intBuf[3]);

        if (allocFlags&ALLOC_ZEROIZE)
        {
            osa_mem_set(buf,0,size);
        }
    }

    return buf;
}

void *cmsMem_realloc(void *origBuf, UINT32 size)
{
    void *buf=NULL;
    UINT32 origSize=0, origAllocSize=0;
    UINT32 *intBuf=NULL;

    intBuf = (UINT32 *) (((UINT32) origBuf) - CMS_MEM_HEADER_LENGTH);

    origSize = intBuf[1];

    /* sanity check the original length */
    if (intBuf[1] != (intBuf[2] ^ 0xffffffff))
    {
        cmsLog_error(TR069_ALARM_INTERNAL_ERROR,"memory underflow detected, %d %d", intBuf[1], intBuf[2]);
        cmsAst_assert(0);
        return NULL;
    }

    if (REAL_ALLOC_SIZE(size) <= origAllocSize)
    {
        /* currently, I don't shrink buffers, but could in the future. */
        return origBuf;
    }

    buf = cmsMem_alloc(REAL_ALLOC_SIZE(size), origAllocSize);
    if (buf != NULL)
    {
       /* got new buffer, copy orig buffer to new buffer */
       memcpy(buf, origBuf, origSize);
       CMSMEM_FREE_BUF_AND_NULL_PTR(origBuf);
    }
    else
    {
       /*
        * We could not allocate a bigger buffer.
        * Return NULL but leave the original buffer untouched.
        */
    }

    return buf;
}

void cmsMem_free(void *buf)
{
    if (buf != NULL)
    {
        UINT32 *intBuf = (UINT32 *) (((UINT32) buf) - CMS_MEM_HEADER_LENGTH);

        if (intBuf[1] != (intBuf[2] ^ 0xffffffff))
        {
            cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"memory underflow detected, %d %d", intBuf[1], intBuf[2]);
            cmsAst_assert(0);
            return;
        }

        buf = intBuf;  /* buf points to real start of buffer */
        osa_mem_free(buf);
    }

    return;
}

char *cmsMem_strdup(const char *str)
{
    return cmsMem_strdupFlags(str, 0);
}

char *cmsMem_strdupFlags(const char *str, UINT32 flags)
{
    UINT32 len;
    void *buf;
    
    if (str == NULL)
    {
       return NULL;
    }
    
    /* this is somewhat dangerous because it depends on str being NULL
     * terminated.  Use strndup/strlen if not sure the length of the string.
     */
    len = strlen(str);
    
    buf = cmsMem_alloc(len+1, flags);
    if (buf == NULL)
    {
       return NULL;
    }
    
    strncpy((char *) buf, str, len+1);
    
    return ((char *) buf);
}

char *cmsMem_strndup(const char *str, UINT32 maxlen)
{
    return cmsMem_strndupFlags(str, maxlen, 0);
}

char *cmsMem_strndupFlags(const char *str, UINT32 maxlen, UINT32 flags)
{
    UINT32 len;
    char *buf;
    
    if (str == NULL)
    {
       return NULL;
    }
    
    len = cmsMem_strnlen(str, maxlen, NULL);
    
    buf = (char *) cmsMem_alloc(len+1, flags);
    if (buf == NULL)
    {
       return NULL;
    }
    
    strncpy(buf, str, len);
    buf[len] = 0;
    
    return buf;
}

UINT32 cmsMem_strnlen(const char *str, UINT32 maxlen, UBOOL8 *isTerminated)
{
   UINT32 len=0;

   while ((len < maxlen) && (str[len] != 0))
   {
      len++;
   }

   if (isTerminated != NULL)
   {
      *isTerminated = (str[len] == 0);
   }

   return len;
}

void cmsAst_assertFunc(const char *filename, UINT32 lineNumber, const char *exprString, SINT32 expr)
{
    if (expr == 0)
    {
        cmsLog_error(TR069_ALARM_INTERNAL_ERROR,"assertion \"%s\" failed at %s:%d", exprString, filename, lineNumber);
        abort();
    }

    return;
}

CmsRet cmsUtl_parseUrl(const char *url, UrlProto *proto, char **addr, UINT16 *port, char **path)
{
   int n = 0;
   const char *p = NULL;
   char protocol[BUFLEN_16];
   char host[BUFLEN_1024];
   char uri[BUFLEN_1024];

   if (url == NULL)
   {
      cmsLog_debug("url is NULL");
      return CMSRET_INVALID_ARGUMENTS;
   }

  *port = 0;
   protocol[0] = host[0]  = uri[0] = '\0';

   /* proto */
   p = (const char *) url;
   if ((p = strchr(url, ':')) == NULL) 
   {
      return CMSRET_INVALID_ARGUMENTS;
   }
   n = p - url;
   strncpy(protocol, url, n);
   protocol[n] = '\0';

   if (!strcmp(protocol, "http"))
   {
      *proto = URL_PROTO_HTTP;
   }
   else if (!strcmp(protocol, "https"))
   {
      *proto = URL_PROTO_HTTPS;
   }
   else if (!strcmp(protocol, "ftp"))
   {
      *proto = URL_PROTO_FTP;
   }
   else if (!strcmp(protocol, "tftp"))
   {
      *proto = URL_PROTO_TFTP;
   }
   else
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"unrecognized proto in URL %s", url);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* skip "://" */
   if (*p++ != ':') return CMSRET_INVALID_ARGUMENTS;
   if (*p++ != '/') return CMSRET_INVALID_ARGUMENTS;
   if (*p++ != '/') return CMSRET_INVALID_ARGUMENTS;

   /* host */
   {
      char *pHost = host;
      char endChar1 = ':';  // by default, host field ends if a colon is seen
      char endChar2 = '/';  // by default, host field ends if a / is seen

#ifdef SUPPORT_IPV6
      if (*p && *p == '[')
      {
         /*
          * Square brackets are used to surround IPv6 addresses in : notation.
          * So if a [ is detected, then keep scanning until the end bracket
          * is seen.
          */
         endChar1 = ']';
         endChar2 = ']';
         p++;  // advance past the [
      }
#endif

      while (*p && *p != endChar1 && *p != endChar2)
      {
         *pHost++ = *p++;
      }
      *pHost = '\0';

#ifdef SUPPORT_IPV6
      if (endChar1 == ']')
      {
         // if endChar is ], then it must be found
         if (*p != endChar1)
         {
            return CMSRET_INVALID_ARGUMENTS;
         }
         else
         {
            p++;  // advance past the ]
         }
      }
#endif
   }
   if (strlen(host) != 0)
   {
      *addr = cmsMem_strdup(host);
   }
   else
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"unrecognized host in URL %s", url);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* end */
   if (*p == '\0') 
   {
      *path = cmsMem_strdup("/");
       return CMSRET_SUCCESS;
   }

   /* port */
   if (*p == ':') 
   {
      char buf[BUFLEN_16];
      char *pBuf = buf;

      p++;
      while (isdigit(*p)) 
      {
         *pBuf++ = *p++;
      }
      *pBuf = '\0';
      if (strlen(buf) == 0)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(*addr);
         cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"unrecognized port in URL %s", url);
         return CMSRET_INVALID_ARGUMENTS;
      }
      *port = atoi(buf);
   }
  
   /* path */
   if (*p == '/') 
   {
      char *pUri = uri;

      while ((*pUri++ = *p++));
      *path = cmsMem_strdup(uri);  
   }
   else
   {
      *path = cmsMem_strdup("/");
   }

   return CMSRET_SUCCESS;
}

CmsRet cmsUtl_getBaseDir(char *pathBuf, UINT32 pathBufLen)
{
   UINT32 rc;

#ifdef DESKTOP_LINUX
   char pwd[CMS_MAX_FULLPATH_LENGTH]={0};
   UINT32 pwdLen = sizeof(pwd);
   char *str;
   char *envDir;
   struct stat statbuf;

   /*
    * If user has set an env var to tell us where the base dir is,
    * always use it.
    */
   if ((envDir = getenv("CMS_BASE_DIR")) != NULL)
   {
      rc = snprintf(pwd, sizeof(pwd), "%s/userspace", envDir);
      if (rc >= (SINT32) sizeof(pwd))
      {
         cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"CMS_MAX_FULLPATH_LENGTH (%d) exceeded on %s",
                      CMS_MAX_FULLPATH_LENGTH, pwd);
         return CMSRET_RESOURCE_EXCEEDED;
      }
      if ((rc = stat(pwd, &statbuf)) == 0)
      {
         /* env var is good, use it. */
         rc = snprintf(pathBuf, pathBufLen, "%s", envDir);
      }
      else
      {
         /* CMS_BASE_DIR is set, but points to bad location */
         return CMSRET_INVALID_ARGUMENTS;
      }
   }
   else
   {
      /*
       * User did not set env var, so try to figure out where base dir is
       * based on current directory.
       */
      if (NULL == getcwd(pwd, pwdLen) ||
          strlen(pwd) == pwdLen - 1)
      {
         cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"Buffer for getcwd is not big enough");
         return CMSRET_INTERNAL_ERROR;
      }

      str = strstr(pwd, "userspace");
      if (str == NULL)
      {
         str = strstr(pwd, "unittests");
      }
      if (str != NULL)
      {
         /*
          * OK, we are running from under a recognized top level directory
          * (userspace or unittests).
          * null terminate the string right before that directory and that
          * should give us the basedir.
          */
         str--;
         *str = 0;

         rc = snprintf(pathBuf, pathBufLen, "%s", pwd);
      }
      else
      {
         /* not running from under CommEngine and also no CMS_BASE_DIR */
         return CMSRET_INVALID_ARGUMENTS;
      }
   }
#else

   rc = snprintf(pathBuf, pathBufLen, "/var");

#endif /* DESKTOP_LINUX */

   if (rc >= pathBufLen)
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"pathBufLen %d is too small for buf %s", pathBufLen, pathBuf);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   return CMSRET_SUCCESS;
}

CmsRet cmsUtl_getRunTimeRootDir(char *pathBuf, UINT32 pathBufLen)
{
   UINT32 rc;

#ifdef DESKTOP_LINUX
   char baseDir[CMS_MAX_FULLPATH_LENGTH]={0};
   char tmpPath[CMS_MAX_FULLPATH_LENGTH]={0};
   char profileBuf[BUFLEN_64];
   UINT32 bufsize = sizeof(profileBuf);
   CmsRet ret;

   ret = cmsUtl_getBaseDir(baseDir, sizeof(baseDir));
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }

   rc = snprintf(tmpPath, sizeof(tmpPath), "%s/.last_profile", baseDir);
   if (rc >= sizeof(tmpPath))
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"CMS_MAX_FULLPATH_LENGTH (%d) exceeded", CMS_MAX_FULLPATH_LENGTH);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   ret = cmsFil_copyToBuffer(tmpPath, (UINT8 *) profileBuf, &bufsize);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"Error %d reading .last_profile file at %s (bufsize=%d)",
                   ret, tmpPath, bufsize);
      return ret;
   }

   /* truncate any newlines at the end of the buffer */
   {
      UINT32 i;

      for (i=0; i < bufsize; i++)
      {
         if (profileBuf[i] == '\n' || profileBuf[i] == '\r' || profileBuf[i] == '\t')
         {
            profileBuf[i] = '\0';
         }
      }
   }


   rc = snprintf(pathBuf, pathBufLen,
                 "%s/targets/%s/fs.install", baseDir, profileBuf);
   cmsLog_debug("returning %s", pathBuf);

#else

   rc = snprintf(pathBuf, pathBufLen, "/");

#endif /* DESKTOP_LINUX */

   if (rc >= pathBufLen)
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"pathBufLen %d is too small for buf %s", pathBufLen, pathBuf);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   return CMSRET_SUCCESS;
}

CmsRet cmsUtl_getRunTimePath(const char *target, char *pathBuf, UINT32 pathBufLen)
{
    CmsRet ret;
    
    ret = cmsUtl_getRunTimeRootDir(pathBuf, pathBufLen);
    if (ret != CMSRET_SUCCESS)
    {
       return ret;
    }
    
    /*
     * If the runtime root dir is just "/", and the target path already
     * begins with a "/", then just use target path to avoid beginning
     * the path with "//".
     */
    if (pathBufLen > 1 && pathBuf[0] == '/' && pathBuf[1] == '\0')
    {
       SINT32 rc;
       rc = snprintf(pathBuf, pathBufLen, "%s", target);
       if (rc >= (SINT32) pathBufLen)
       {
          cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"pathBufLen %d is too small for target %s",
                       pathBufLen, pathBuf);
          ret = CMSRET_RESOURCE_EXCEEDED;
       }
    }
    else
    {
       ret = cmsUtl_strncat(pathBuf, pathBufLen, target);
    }
    
    return ret;
}

CmsLogLevel cmsUtl_logLevelStringToEnum(const char *clogLevel)
{
    if (!strcmp(clogLevel, MDMVS_ERROR))
    {
       return LOG_LEVEL_ERR;
    }
    else if (!strcmp(clogLevel, MDMVS_NOTICE))
    {
       return LOG_LEVEL_NOTICE;
    }
    else if (!strcmp(clogLevel, MDMVS_DEBUG))
    {
       return LOG_LEVEL_DEBUG;
    }
    else
    {
       cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"unimplemented log level %s", clogLevel);
       return DEFAULT_LOG_LEVEL;
    }
}

CmsLogDestination cmsUtl_logDestinationStringToEnum(const char *logDest)
{
    if (!strcmp(logDest, MDMVS_STANDARD_ERROR))
    {
       return LOG_DEST_STDERR;
    }
    else if (!strcmp(logDest, MDMVS_SYSLOG))
    {
       return LOG_DEST_SYSLOG;
    }
    else if (!strcmp(logDest, MDMVS_TELNET))
    {
       return LOG_DEST_TELNET;
    }
    else
    {
       cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"unimplemented log dest %s", logDest);
       return DEFAULT_LOG_DESTINATION;
    }
}

UBOOL8 cmsUtl_isZeroIpvxAddress(UINT32 ipvx, const char *addr)
{
    if (IS_EMPTY_STRING(addr))
    {
       return TRUE;
    }
    
    if (((ipvx & CMS_AF_SELECT_IPV4) && !strcmp(addr, "0.0.0.0")) ||
        ((ipvx & CMS_AF_SELECT_IPV6) &&
            (!strcmp(addr, "0:0:0:0:0:0:0:0") || !strcmp(addr, "::") || !strcmp(addr, "::/0"))))
    {
       return TRUE;
    }
    
    return FALSE;
}

SINT32 cmsUtl_strcmp(const char *s1, const char *s2)
{
    char emptyStr = '\0';
    const char *str1 = (const char *) s1;
    const char *str2 = (const char *) s2;
    
    if (str1 == NULL)
    {
       str1 = &emptyStr;
    }
    if (str2 == NULL)
    {
       str2 = &emptyStr;
    }
    
    return strcmp(str1, str2);
}

SINT32 cmsUtl_strcasecmp(const char *s1, const char *s2)
{
    char emptyStr = '\0';
    const char *str1 = (const char *) s1;
    const char *str2 = (const char *) s2;
    
    if (str1 == NULL)
    {
       str1 = &emptyStr;
    }
    if (str2 == NULL)
    {
       str2 = &emptyStr;
    }
    
    return strcasecmp(str1, str2);
}

SINT32 cmsUtl_strncmp(const char *s1, const char *s2, SINT32 n)
{
    char emptyStr = '\0';
    const char *str1 = (const char *) s1;
    const char *str2 = (const char *) s2;
    
    if (str1 == NULL)
    {
       str1 = &emptyStr;
    }
    if (str2 == NULL)
    {
       str2 = &emptyStr;
    }
    
    return strncmp(str1, str2, n);
}

char *cmsUtl_strstr(const char *s1, const char *s2)
{
    char emptyStr = '\0';
    const char *str1 = (const char *)s1;
    const char *str2 = (const char *)s2;
    
    if (str1 == NULL)
    {
       str1 = &emptyStr;
    }
    if (str2 == NULL)
    {
       str2 = &emptyStr;
    }
    
    return strstr(str1, str2);
}

char *cmsUtl_strncpy(char *dest, const char *src, SINT32 dlen)
{
    /* if the dest ptr is NULL, we cannot copy at all.  Return now */
    if (dest == NULL)
    {
       cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"dest is NULL!");
       return NULL;
    }
    
    /* if src ptr is NULL, copy an empty string to dest */
    if (src == NULL)
    {
       strcpy(dest, "");
       return dest;
    }
    
    /* do a modified strncpy by making sure dest is NULL terminated */
    if( strlen(src)+1 > (UINT32) dlen )
    {
       cmsLog_notice("truncating:src string length > dest buffer");
       strncpy(dest,src,dlen-1);
       dest[dlen-1] ='\0';
    }
    else
    {
       strcpy(dest,src);
    }
    return dest;
}

CmsRet cmsUtl_strncat(char *prefix, UINT32 prefixLen, const char *suffix)
{
   CmsRet ret=CMSRET_SUCCESS;
   UINT32 copyLen;

   if((prefix == NULL) || (suffix == NULL))
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"null pointer reference src=%p dest =%p", prefix, suffix);
      return CMSRET_INVALID_ARGUMENTS;
   }

   if(strlen(prefix) + strlen(suffix) + 1 > prefixLen )
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"supplied prefix buffer (len=%d) is too small for %s + %s",
            prefixLen, prefix, suffix);
      ret = CMSRET_RESOURCE_EXCEEDED;  /* set error, but copy what we can */
   }

   copyLen = prefixLen - strlen(prefix) - 1;
   strncat(prefix, suffix, copyLen);

   return ret;
}

SINT32 cmsUtl_strlen(const char *src)
{
    char emptyStr[1] = {0};
    const char *str = (const char *)src;
    
    if(src == NULL)
    {
       str = emptyStr;
    }    
    
    return strlen(str);
}

static int get_random_fd(void)
{
    int fd;

    while (1) 
    {
        fd = ((int) random()) % MAXFDS;
        if (fd > 2)
        {
            return fd;
        }
    }
}

static void get_random_bytes(void *buf, int nbytes)
{
    int i, n = nbytes, fd = get_random_fd();
    int lose_counter = 0;
    unsigned char *cp = (unsigned char *) buf;

    if (fd >= 0) 
    {
        while (n > 0) 
        {
            i = read(fd, cp, n);
            if (i <= 0) 
            {
                if (lose_counter++ > 16)
                {
                    break;
                }
                continue;
            }
            n -= i;
            cp += i;
            lose_counter = 0;
        }
    }

    /*
     * We do this all the time, but this is the only source of
     * randomness if /dev/random/urandom is out to lunch.
     */
    for (cp = buf, i = 0; i < nbytes; i++)
    {
        *cp++ ^= (rand() >> 7) & 0xFF;
    }
    return;
}

static int get_clock(UINT32 *clock_high, UINT32 *clock_low, UINT16 *ret_clock_seq)
{
    static int              adjustment = 0;
    static struct timeval   last = {0, 0};
    static UINT16           clock_seq;
    struct timeval          tv;
    unsigned long long      clock_reg;

try_again:
    gettimeofday(&tv, 0);
    if ((last.tv_sec == 0) && (last.tv_usec == 0)) 
    {
        get_random_bytes(&clock_seq, sizeof(clock_seq));
        clock_seq &= 0x3FFF;
        last = tv;
        last.tv_sec--;
    }
    if ((tv.tv_sec < last.tv_sec) ||
        ((tv.tv_sec == last.tv_sec) &&
         (tv.tv_usec < last.tv_usec))) 
    {
        clock_seq = (clock_seq+1) & 0x3FFF;
        adjustment = 0;
        last = tv;
    } 
    else if ((tv.tv_sec == last.tv_sec) &&
             (tv.tv_usec == last.tv_usec)) 
    {
        if (adjustment >= MAX_ADJUSTMENT)
        {
            goto try_again;
        }
        adjustment++;
    } 
    else 
    {
        adjustment = 0;
        last = tv;
    }
  
    clock_reg = tv.tv_usec*10 + adjustment;
    clock_reg += ((unsigned long long) tv.tv_sec)*10000000;
    clock_reg += (((unsigned long long) 0x01B21DD2) << 32) + 0x13814000;

    *clock_high = clock_reg >> 32;
    *clock_low = clock_reg;
    *ret_clock_seq = clock_seq;
    return 0;
}

void uuid_pack(struct _uuid_t *uu, unsigned char *ptr)
{
   UINT32  tmp;
   unsigned char  *out = ptr;

   tmp = uu->time_low;
   out[3] = (unsigned char) tmp;
   tmp >>= 8;
   out[2] = (unsigned char) tmp;
   tmp >>= 8;
   out[1] = (unsigned char) tmp;
   tmp >>= 8;
   out[0] = (unsigned char) tmp;
   
   tmp = uu->time_mid;
   out[5] = (unsigned char) tmp;
   tmp >>= 8;
   out[4] = (unsigned char) tmp;
   
   tmp = uu->time_hi_and_version;
   out[7] = (unsigned char) tmp;
   tmp >>= 8;
   out[6] = (unsigned char) tmp;
   
   tmp = uu->clock_seq;
   out[9] = (unsigned char) tmp;
   tmp >>= 8;
   out[8] = (unsigned char) tmp;
   
   memcpy(out+10, uu->node, 6);
}

static void uuid_generate_time(unsigned char *out, unsigned char *macAddress)
{
   struct _uuid_t uu;
   UINT32  clock_mid;
   
   get_clock(&clock_mid, &uu.time_low, &uu.clock_seq);
   uu.clock_seq |= 0x8000;
   uu.time_mid = (UINT16) clock_mid;
   uu.time_hi_and_version = ((clock_mid >> 16) & 0x0FFF) | 0x1000;
   memcpy(uu.node, macAddress, 6);
   uuid_pack(&uu, out);
}

void cmsUtl_generateUuidStr(char *str, int len, unsigned char *macAddress)
{
    unsigned char d[16];
    uuid_generate_time(d,macAddress);
    snprintf(str, len, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             (UINT8)d[0], (UINT8)d[1], (UINT8)d[2], (UINT8)d[3], (UINT8)d[4], (UINT8)d[5], (UINT8)d[6], (UINT8)d[7], 
             (UINT8)d[8], (UINT8)d[9], (UINT8)d[10], (UINT8)d[11], (UINT8)d[12], (UINT8)d[13], (UINT8)d[14], (UINT8)d[15]);
}

void cmsTms_get(CmsTimestamp *tms)
{
    struct timespec ts;
    SINT32 rc;
    
    if (tms == NULL)
    {
       return;
    }
    
    rc = clock_gettime(CLOCK_MONOTONIC, &ts);
    if (rc == 0)
    {
       tms->sec = ts.tv_sec;
       tms->nsec = ts.tv_nsec;
    }
    else
    {
       cmsLog_error(TR069_ALARM_TIMER_ERROR,"clock_gettime failed, set timestamp to 0");
       tms->sec = 0;
       tms->nsec = 0;
    }

}

void cmsTms_delta(const CmsTimestamp *newTms,
                  const CmsTimestamp *oldTms,
                  CmsTimestamp *deltaTms)
{
   if (newTms->sec >= oldTms->sec)
   {
      if (newTms->nsec >= oldTms->nsec)
      {
         /* no roll-over in the sec and nsec fields, straight subtract */
         deltaTms->nsec = newTms->nsec - oldTms->nsec;
         deltaTms->sec = newTms->sec - oldTms->sec;
      }
      else
      {
         /* no roll-over in the sec field, but roll-over in nsec field */
         deltaTms->nsec = (NSECS_IN_SEC - oldTms->nsec) + newTms->nsec;
         deltaTms->sec = newTms->sec - oldTms->sec - 1;
      }
   }
   else
   {
      if (newTms->nsec >= oldTms->nsec)
      {
         /* roll-over in the sec field, but no roll-over in the nsec field */
         deltaTms->nsec = newTms->nsec - oldTms->nsec;
         deltaTms->sec = (MAX_UINT32 - oldTms->sec) + newTms->sec + 1; /* +1 to account for time spent during 0 sec */
      }
      else
      {
         /* roll-over in the sec and nsec fields */
         deltaTms->nsec = (NSECS_IN_SEC - oldTms->nsec) + newTms->nsec;
         deltaTms->sec = (MAX_UINT32 - oldTms->sec) + newTms->sec;
      }
   }
}

UINT32 cmsTms_deltaInMilliSeconds(const CmsTimestamp *newTms, const CmsTimestamp *oldTms)
{
    CmsTimestamp deltaTms;
    UINT32 ms;
    
    cmsTms_delta(newTms, oldTms, &deltaTms);
    
    if (deltaTms.sec > MAX_UINT32 / MSECS_IN_SEC)
    {
       /* the delta seconds is larger than the UINT32 return value, so return max value */
       ms = MAX_UINT32;
    }
    else
    {
       ms = deltaTms.sec * MSECS_IN_SEC;
    
       if ((MAX_UINT32 - ms) < (deltaTms.nsec / NSECS_IN_MSEC))
       {
          /* overflow will occur when adding the nsec, return max value */
          ms = MAX_UINT32;
       }
       else
       {
          ms += deltaTms.nsec / NSECS_IN_MSEC;
       }
    }
    
    return ms;
}

void cmsTms_addMilliSeconds(CmsTimestamp *tms, UINT32 ms)
{
   UINT32 addSeconds;
   UINT32 addNano;

   addSeconds = ms / MSECS_IN_SEC;
   addNano = (ms % MSECS_IN_SEC) * NSECS_IN_MSEC;

   tms->sec += addSeconds;
   tms->nsec += addNano;

   /* check for carry-over in nsec field */
   if (tms->nsec > NSECS_IN_SEC)
   {
      /* we can't have carried over by more than 1 second */
      tms->sec++;
      tms->nsec -= NSECS_IN_SEC;
   }

   return;
}

CmsRet cmsTms_getXSIDateTime(UINT32 t, char *buf, UINT32 bufLen)
{
    int          c;
    time_t       now;
    struct tm   tmpx;
    struct tm   *tmp=&tmpx;
    
    if (t == 0)
    {
       now = time(NULL);
    }
    else
    {
       now = t;
    }
    
    localtime_r(&now,tmp);
    memset(buf, 0, bufLen);
    c = strftime(buf, bufLen, "%Y-%m-%dT%H:%M:%S%z", tmp);
    if ((c == 0) || (c+1 > (int) bufLen))
    {
       /* buf was not long enough */
       return CMSRET_RESOURCE_EXCEEDED;
    }
    
     /* fix missing : in time-zone offset-- change -500 to -5:00 */
    buf[c+1] = '\0';
    buf[c] = buf[c-1];
    buf[c-1] = buf[c-2];
    buf[c-2]=':';
    
    return CMSRET_SUCCESS;
}

const char *cmsErr_getStringBuf(CmsRet ret, char *buf, UINT32 buflen)
{
   char *errorStr;

   switch(ret)
   {
   case CMSRET_SUCCESS:
      errorStr = "Success";
      break;

   case CMSRET_METHOD_NOT_SUPPORTED:
      errorStr = "Method not supported";
      break;

   case CMSRET_REQUEST_DENIED:
      errorStr = "Request denied";
      break;

   case CMSRET_INTERNAL_ERROR:
      errorStr = "Internal Error";
      break;

   case CMSRET_INVALID_ARGUMENTS:
      errorStr = "Invalid arguments";
      break;

   case CMSRET_RESOURCE_EXCEEDED:
      errorStr = "Resource exceeded";
      break;

   case CMSRET_INVALID_PARAM_NAME:
      errorStr = "Invalid Parameter Name";
      break;

   case CMSRET_INVALID_PARAM_TYPE:
      errorStr = "Invalid parameter type";
      break;

   case CMSRET_INVALID_PARAM_VALUE:
      errorStr = "Invalid parameter value";
      break;

   case CMSRET_SET_NON_WRITABLE_PARAM:
      errorStr = "Attempt to set a non-writeable parameter";
      break;

   case CMSRET_NOTIFICATION_REQ_REJECTED:
      errorStr = "Notification request rejected";
      break;

   case CMSRET_DOWNLOAD_FAILURE:
      errorStr = "Download failure";
      break;

   case CMSRET_UPLOAD_FAILURE:
      errorStr = "Upload failure";
      break;

   case CMSRET_FILE_TRANSFER_AUTH_FAILURE:
      errorStr = "File transfer server authentication failure";
      break;

   case CMSRET_UNSUPPORTED_FILE_TRANSFER_PROTOCOL:
      errorStr = "Unsupported protocol for file transfer";
      break;

   case CMSRET_FILE_TRANSFER_UNABLE_JOIN_MULTICAST:
      errorStr = "File transfer failure: unable to join multicast group";
      break;

   case CMSRET_FILE_TRANSFER_UNABLE_CONTACT_FILE_SERVER:
      errorStr = "File transfer failure: unable to contact file server";
      break;
   case CMSRET_FILE_TRANSFER_UNABLE_ACCESS_FILE:
      errorStr = "File transfer failure: unable to access file";
      break;

   case CMSRET_FILE_TRANSFER_UNABLE_COMPLETE:
      errorStr = "File transfer failure: unable to complete download";
      break;

   case CMSRET_FILE_TRANSFER_FILE_CORRUPTED:
      errorStr = "File transfer failure: file corrupted";
      break;

   case CMSRET_FILE_TRANSFER_FILE_AUTHENTICATION_ERROR:
      errorStr = "File transfer failure: file authentication error";
      break;

   case CMSRET_FILE_TRANSFER_FILE_TIMEOUT:
      errorStr = "File transfer failure: download timeout";
      break;

   case CMSRET_FILE_TRANSFER_FILE_CANCELLATION_NOT_ALLOW:
      errorStr = "File transfer failure: cancellation not permitted";
      break;

   case CMSRET_INVALID_UUID_FORMAT:
      errorStr = "Invalid UUID format";
      break;

   case CMSRET_UNKNOWN_EE:
      errorStr = "Unknown execution environment";
      break;

   case CMSRET_EE_DISABLED:
      errorStr = "Execution environment disabled";
      break;

   case CMSRET_DU_EE_MISMATCH:
      errorStr = "Execution environment and deployment unit mismatch";
      break;

   case CMSRET_DU_DUPLICATE:
      errorStr = "Duplicate deployment unit";
      break;

   case CMSRET_SW_MODULE_SYSTEM_RESOURCE_EXCEEDED:
      errorStr = "Software modules system resources exceeded";
      break;

   case CMSRET_DU_UNKNOWN:
      errorStr = "Unknown deployment unit";
      break;

   case CMSRET_DU_STATE_INVALID:
      errorStr = "Invalid deployment unit state";
      break;

   case CMSRET_DU_UPDATE_DOWNGRADE_NOT_ALLOWED:
      errorStr = "Invalid deployment unit update: downgrade not permitted";
      break;

   case CMSRET_DU_UPDATE_VERSION_NOT_SPECIFIED:
      errorStr = "Invalid deployment unit update: version not specified";
      break;

   case CMSRET_DU_UPDATE_VERSION_EXISTED:
      errorStr = "Invalid deployment unit update: version already exists";
      break;

   default:
      errorStr = NULL;
      break;
   }

   if (errorStr)
   {
      cmsUtl_strncpy(buf, errorStr, buflen);
   }
   else
   {
      int rc;
      if (ret >= 9800 && ret <= 9899)
      {
         rc = snprintf(buf, buflen, "vendor defined fault code %d", ret);
      }
      else
      {
         rc = snprintf(buf, buflen, "no string available for code %d", ret);
      }
      if (rc >= (int) buflen)
      {
         cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"buffer provided is too small (%d) for error string, truncated", buflen);
      }
   }

   return buf;
}

static char global_error_buf[BUFLEN_512];

const char *cmsErr_getString(CmsRet ret)
{
   return (cmsErr_getStringBuf(ret, global_error_buf, sizeof(global_error_buf)));
}

struct xml_esc_entry {
   char esc;  /**< character that needs to be escaped */
   char *seq; /**< escape sequence */
   UINT32 len;  /**< length of escape sequence */
};

struct xml_esc_entry xml_esc_table[] = {
      {'<', "&lt;", 4},
      {'>', "&gt;", 4},
      {'&', "&amp;", 5},
      {'%', "&#37;", 5},
      {' ', "&#32;", 5},
      {'\t', "&#09;", 5},
      {'\n', "&#10;", 5},
      {'\r', "&#13;", 5},
      {'"', "&quot;", 6},
};

#define NUM_XML_ESC_ENTRIES (sizeof(xml_esc_table)/sizeof(struct xml_esc_entry))



UBOOL8 cmsXml_isEscapeNeeded(const char *string)
{
   UINT32 len, i=0, e=0;
   UBOOL8 escapeNeeded = FALSE;

   if (string == NULL)
   {
      return FALSE;
   }

   len = strlen(string);

   /* look for characters which need to be escaped. */
   while (escapeNeeded == FALSE && i < len)
   {
      for (e=0; e < NUM_XML_ESC_ENTRIES; e++)
      {
         if (string[i] == xml_esc_table[e].esc)
         {
            escapeNeeded = TRUE;
            break;
         }
      }
      i++;
   }

   return escapeNeeded;
}


CmsRet cmsXml_escapeString(const char *string, char **escapedString)
{
   UINT32 len, len2, i=0, j=0, e=0, f=0;
   char *tmpStr;

   if (string == NULL)
   {
      return CMSRET_SUCCESS;
   }

   len = strlen(string);
   len2 = len;

   /* see how many characters need to be escaped and what the new length is */
   while (i < len)
   {
      for (e=0; e < NUM_XML_ESC_ENTRIES; e++)
      {
         if (string[i] == xml_esc_table[e].esc)
         {
            len2 += (xml_esc_table[e].len - 1);
            break;
         }
      }
      i++;
   }

   if ((tmpStr = cmsMem_alloc(len2+1, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to allocate %d bytes", len+1);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   i=0;
   while (i < len)
   {
      UBOOL8 found;

      found = FALSE;
      /* see if we need to replace any characters with esc sequences */
      for (e=0; e < NUM_XML_ESC_ENTRIES; e++)
      {
         if (string[i] == xml_esc_table[e].esc)
         {
            for (f=0; f<xml_esc_table[e].len; f++)
            {
               tmpStr[j++] = xml_esc_table[e].seq[f];
               found = TRUE;
            }
            break;
         }
      }

      /* no replacement, then just copy over the original string */
      if (!found)
      {
         tmpStr[j++] = string[i];
      }

      i++;
   }

   *escapedString = tmpStr;

   return CMSRET_SUCCESS;
}


UBOOL8 cmsXml_isUnescapeNeeded(const char *escapedString)
{
   UINT32 len, i=0, e=0, f=0;
   UBOOL8 unescapeNeeded = FALSE;
   UBOOL8 matched=FALSE;

   if (escapedString == NULL)
   {
      return FALSE;
   }

   len = strlen(escapedString);

   while (unescapeNeeded == FALSE && i < len)
   {
      /* all esc sequences begin with &, so look for that first */
      if (escapedString[i] == '&')
      {
         for (e=0; e < NUM_XML_ESC_ENTRIES && !matched; e++)
         {
            if (i+xml_esc_table[e].len-1 < len)
            {
               /* check for match against an individual sequence */
               matched = TRUE;
               for (f=1; f < xml_esc_table[e].len; f++)
               {
                  if (escapedString[i+f] != xml_esc_table[e].seq[f])
                  {
                     matched = FALSE;
                     break;
                  }
               }
            }
         }
      }

      i++;

      /* if we saw a match, then unescape is needed */
      unescapeNeeded = matched;
   }

   return unescapeNeeded;
}


CmsRet cmsXml_unescapeString(const char *escapedString, char **string)
{
   UINT32 len, i=0, j=0, e=0, f=0;
   char *tmpStr;
   UBOOL8 matched=FALSE;

   if (escapedString == NULL)
   {
      return CMSRET_SUCCESS;
   }

   len = strlen(escapedString);

   if ((tmpStr = cmsMem_alloc(len+1, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to allocate %d bytes", len+1);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   while (i < len)
   {
      /* all esc sequences begin with &, so look for that first */
      if (escapedString[i] == '&')
      {
         for (e=0; e < NUM_XML_ESC_ENTRIES && !matched; e++)
         {
            if (i+xml_esc_table[e].len-1 < len)
            {
               /* check for match against an individual sequence */
               matched = TRUE;
               for (f=1; f < xml_esc_table[e].len; f++)
               {
                  if (escapedString[i+f] != xml_esc_table[e].seq[f])
                  {
                     matched = FALSE;
                     break;
                  }
               }
            }

            if (matched)
            {
               tmpStr[j++] = xml_esc_table[e].esc;
               i += xml_esc_table[e].len;
            }
         }
      }

      if (!matched)
      {
         /* not a valid escape sequence, just copy it */
         tmpStr[j++] = escapedString[i++];
      }

      /* going on to next character, so reset matched */
      matched = FALSE;
   }

   *string = tmpStr;

   return CMSRET_SUCCESS;
}

UBOOL8 cmsUnicode_isUnescapeNeeded
   (const char *string)
{
   const char *pStart = NULL, *pChar = NULL;
   UBOOL8 found = FALSE;

   if (string == NULL)
      return found;

   pStart = (const char *)string;

   while (found == FALSE)
   {
      if ((pChar = strstr(pStart, "&#")) != NULL)
      {
         for (pChar += 2;
              found == FALSE && *pChar != '&' && *pChar != '\0';
              pChar++)
         {
            if (*pChar == ';')
               found = TRUE;
         }

         pStart = pChar;
      }
      else
      {
         break;
      }
   }

   return found;
}

static CmsRet consumeUnicode
   (const char *string, unsigned char *pConvertedChar, UINT32 *offset)
{
   char *tmpStr = NULL;
   char *pSemi = NULL, *pAmp = NULL;
   UINT16 num = 0;
   UINT32 i = 0;
   CmsRet ret = CMSRET_SUCCESS;
   
   if (string == NULL)
   {
      return CMSRET_INVALID_PARAM_VALUE;
   }

   // See valid format strings above
   tmpStr = cmsMem_strdup(string);

   *pConvertedChar = tmpStr[i];
   *offset = 1;

   if (tmpStr[i] == '&' && tmpStr[i+1] == '#')
   {
      // tmpStr = '&#x;' ==>  invalid number to convert
      if (tmpStr[i+2] == 'x' && tmpStr[i+3] == ';')
      {
         ret = CMSRET_INVALID_PARAM_VALUE;
         cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"invalid format, skipping data");
         *offset = 4;
      }
      // tmpStr = '&#;' ==> invalid number to convert
      else if (tmpStr[i+2] == ';')
      {
         ret = CMSRET_INVALID_PARAM_VALUE;
         cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"invalid format, skipping data");
         *offset = 3;
      }
      else
      {
         pSemi = strstr(&tmpStr[i+2], ";");
         if (pSemi != NULL)
         {
            pAmp = strstr(&tmpStr[i+2], "&");
            if (pAmp == NULL || pAmp > pSemi)
            {
               *pSemi = '\0';
               if (tmpStr[i+2] == 'x')
                  num = strtoul(&tmpStr[i+3], (char **)NULL, 16);
               else
                  num = strtoul(&tmpStr[i+2], (char **)NULL, 10);
               // TO-DO: need to take care unicode 16 (2 bytes)
               // right now only take care unicode 8 (1 byte)
               if (num > 127)
               {
                  cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"multi-byte unicode not supported. "
                              "expected num <= 127, got num %d", num);
               }
               *pConvertedChar = (unsigned char) btowc(num);
               *offset = (pSemi - tmpStr) + 1;
            }
         }
      }
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(tmpStr);

   return ret;
}

CmsRet cmsUnicode_unescapeString
   (const char *string, char **unicodedString)
{
   char *tmpStr = NULL;
   unsigned char convertedChar;
   UINT32 len = 0, i = 0, j = 0, offset = 0;
   CmsRet ret = CMSRET_SUCCESS;

   if (string == NULL)
   {
      return ret;
   }

   len = strlen(string);

   if ((tmpStr = cmsMem_alloc(len, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to allocate %d bytes", len);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   while (i < len)
   {
      ret = consumeUnicode(&string[i], &convertedChar, &offset);
      // note, only 8 bit unicode is suported right now, so we can only
      // get at most 1 byte convertedChar
      if (ret == CMSRET_SUCCESS)
         tmpStr[j++] = convertedChar;

      i += offset;
   }
   
   *unicodedString = tmpStr;

   return ret;
}

static UINT32 ConvertEntityToId(char *pName)
{
    char name[6] = {0};
    memset(name,0,sizeof(name));
    strncpy(name, pName, 5);
    return (name[0]<<24 | name[1]<<16 | name[2]<<8 | name[3]);
}

typedef struct
{
   CmsEntityId  eid;        /**< Entity id of the owner of this handle. */
   SINT32       commFd;     /**< communications fd */
   UBOOL8       standalone; /**< are we running without smd, for unittests */
   CmsMsgHeader *putBackQueue;  /**< Messages pushed back into the handle. */
} CmsMsgHandle;

#define SMD_MESSAGE_ADDR  "/tmp/sock_oam"

#define CMS_MSG_PUTBACK_Q_IS_EMPTY(h) (((CmsMsgHandle *) (h))->putBackQueue == NULL)

CmsRet cmsMsg_initWithFlags(CmsEntityId eid, UINT32 flags, void **msgHandler)
{
   CmsMsgHandle *handle;
   struct sockaddr_un serverAddr;
   SINT32 rc;

   
   if ((handle = (CmsMsgHandle *) cmsMem_alloc(sizeof(CmsMsgHandle), ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"could not allocate storage for msg handle");
      return CMSRET_RESOURCE_EXCEEDED;
   }

   /* store caller's eid */
   handle->eid = eid;

#ifdef DESKTOP_LINUX
   /*
    * Applications may be run without smd on desktop linux, so if we
    * don't see a socket for smd, don't bother connecting to it.
    */
   {
      struct stat statbuf;

      if ((rc = stat(SMD_MESSAGE_ADDR, &statbuf)) < 0)
      {
         handle->commFd = CMS_INVALID_FD;
         handle->standalone = TRUE;
         *msgHandler = (void *) handle;
         cmsLog_notice("no smd server socket detected, running in standalone mode.");
         return CMSRET_SUCCESS;
      }
   }
#endif /* DESKTOP_LINUX */


      /*
       * Create a unix domain socket.
       */
      handle->commFd = socket(AF_LOCAL, SOCK_STREAM, 0);
      if (handle->commFd < 0)
      {
         cmsLog_error(TR069_ALARM_SOCKET_ERROR,"Could not create socket");
         CMSMEM_FREE_BUF_AND_NULL_PTR(handle);
         return CMSRET_INTERNAL_ERROR;
      }


      /*
       * Set close-on-exec, even though all apps should close their
       * fd's before fork and exec.
       */
      if ((rc = fcntl(handle->commFd, F_SETFD, FD_CLOEXEC)) != 0)
      {
         cmsLog_error(TR069_ALARM_SOCKET_ERROR,"set close-on-exec failed, rc=%d errno=%d", rc, errno);
         close(handle->commFd);
         CMSMEM_FREE_BUF_AND_NULL_PTR(handle);
         return CMSRET_INTERNAL_ERROR;
      }


      /*
       * Connect to smd.
       */
      memset(&serverAddr, 0, sizeof(serverAddr));
      serverAddr.sun_family = AF_LOCAL;
      strncpy(serverAddr.sun_path, SMD_MESSAGE_ADDR, sizeof(serverAddr.sun_path));

      rc = connect(handle->commFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
      if (rc != 0)
      {
         cmsLog_error(TR069_ALARM_SOCKET_ERROR,"connect to %s failed, rc=%d errno=%d", SMD_MESSAGE_ADDR, rc, errno);
         close(handle->commFd);
         CMSMEM_FREE_BUF_AND_NULL_PTR(handle);
         return CMSRET_INTERNAL_ERROR;
      }
      else
      {
         cmsLog_notice("commFd=%d connected to smd", handle->commFd);
      }

      /* send a launched message to smd */
      {
          CmsRet ret;
          CmsMsgHeader launchMsg = EMPTY_MSG_HEADER;
          
          launchMsg.type = CMS_MSG_APP_LAUNCHED;
          launchMsg.src = EID_TR69C;
          launchMsg.dst = EID_SMD;
          launchMsg.flags_event = 1;
          launchMsg.wordData = getpid();
          
          if ((ret = cmsMsg_send(handle, &launchMsg)) != CMSRET_SUCCESS)
          {
             close(handle->commFd);
             CMSMEM_FREE_BUF_AND_NULL_PTR(handle);
             return CMSRET_INTERNAL_ERROR;
          }
          else
          {
             cmsLog_notice("sent LAUNCHED message to smd");
          }
      }

   /* successful, set handle pointer */
   *msgHandler = (void *) handle;
fprintf(stderr, "SSS :::  unixsockfd = %d\n", handle->commFd);

   return CMSRET_SUCCESS;
}

void cmsMsg_cleanup(void **msgHandler)
{
    CmsMsgHandle *handle = (CmsMsgHandle *) *msgHandler;
    CmsMsgHeader *msg;

    /* send a launched message to smd */
    {
        CmsRet ret;
        CmsMsgHeader launchMsg = EMPTY_MSG_HEADER;
        
        launchMsg.type = CMS_MSG_APP_STOPPED;
        launchMsg.src = EID_TR69C;
        launchMsg.dst = EID_SMD;
        launchMsg.flags_event = 1;
        launchMsg.wordData = getpid();
        
        if ((ret = cmsMsg_send(handle, &launchMsg)) != CMSRET_SUCCESS)
        {
           cmsLog_notice("sent STOPPED message to smd failed");
        }
        else
        {
           cmsLog_notice("sent STOPPED message to smd successfully");
        }
    }

    /* free any queued up messages */
    while ((msg = handle->putBackQueue) != NULL)
    {
        handle->putBackQueue = msg->next;
        CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
    }

    if (handle->commFd != CMS_INVALID_FD)
    {
        close(handle->commFd);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR((*msgHandler));

    return;
}

static SOCK_MSG_HEADER_T *cmsMsg_ConvertFromCmsMsg(const CmsMsgHeader *buf)
{
    SOCK_MSG_HEADER_T *pSockMsg = NULL;
    INT32 msgLength = 0;

    switch(buf->type)
    {
        case CMS_MSG_APP_LAUNCHED:
            msgLength = sizeof(SOCK_MSG_HEADER_T)+strlen(EME_ENTITY_TR69C_NAME)+1;
            pSockMsg = (SOCK_MSG_HEADER_T *)cmsMem_alloc(msgLength,ALLOC_ZEROIZE);

            pSockMsg->msg_id = EME_MSG_READY_IND;
        	pSockMsg->sender_id = ConvertEntityToId(EME_ENTITY_TR69C_NAME);
        	pSockMsg->receiver_id = ConvertEntityToId(EMA_ENTITY_NAME);
        	pSockMsg->word_data = getpid();
        	pSockMsg->msgLength = msgLength-sizeof(SOCK_MSG_HEADER_T);

        	strcpy((char*)(pSockMsg+1),EME_ENTITY_TR69C_NAME);
            break;
        case CMS_MSG_FILE_TRANSFER_REQ:
            msgLength = sizeof(SOCK_MSG_HEADER_T)+sizeof(EME_FILE_TRANSMISSION_T);
            pSockMsg = (SOCK_MSG_HEADER_T *)cmsMem_alloc(msgLength,ALLOC_ZEROIZE);

            pSockMsg->msg_id = EME_MSG_FILE_TRANS_REQ;
        	pSockMsg->sender_id = ConvertEntityToId(EME_ENTITY_TR69C_NAME);
        	pSockMsg->receiver_id = ConvertEntityToId(EMA_ENTITY_NAME);
        	pSockMsg->word_data = 0;
        	pSockMsg->msgLength = msgLength-sizeof(SOCK_MSG_HEADER_T);

            memcpy((pSockMsg+1),(buf+1),pSockMsg->msgLength);
            break;
        case CMS_MSG_REQUEST_DU_STATE_CHANGE:
            msgLength = sizeof(SOCK_MSG_HEADER_T)+sizeof(EME_FILE_TRANSMISSION_T)*buf->wordData;
            pSockMsg = (SOCK_MSG_HEADER_T *)cmsMem_alloc(msgLength,ALLOC_ZEROIZE);

            pSockMsg->msg_id = EME_MSG_FILE_TRANS_REQ;
        	pSockMsg->sender_id = ConvertEntityToId(EME_ENTITY_TR69C_NAME);
        	pSockMsg->receiver_id = ConvertEntityToId(EMA_ENTITY_NAME);
        	pSockMsg->word_data = buf->wordData;
        	pSockMsg->msgLength = msgLength-sizeof(SOCK_MSG_HEADER_T);

            memcpy((pSockMsg+1),(buf+1),pSockMsg->msgLength);
            break;
        case CMS_MSG_REBOOT_SYSTEM:
            msgLength = sizeof(SOCK_MSG_HEADER_T);
            pSockMsg = (SOCK_MSG_HEADER_T *)cmsMem_alloc(msgLength,ALLOC_ZEROIZE);

            if (buf->wordData == TRUE)
            {
                pSockMsg->msg_id = EME_MSG_FACTORY_RESET_REQ;
            }
            else
            {
                pSockMsg->msg_id = EME_MSG_SYSTEM_REBOOT_REQ;
            }

        	pSockMsg->sender_id = ConvertEntityToId(EME_ENTITY_TR69C_NAME);
        	pSockMsg->receiver_id = ConvertEntityToId(EMA_ENTITY_NAME);
        	pSockMsg->word_data = buf->wordData;
        	pSockMsg->msgLength = msgLength-sizeof(SOCK_MSG_HEADER_T);
            break;
        case CMS_MSG_REGISTERED_IND:
            msgLength = sizeof(SOCK_MSG_HEADER_T);
            pSockMsg = (SOCK_MSG_HEADER_T *)cmsMem_alloc(msgLength,ALLOC_ZEROIZE);

            pSockMsg->msg_id = EME_MSG_REGISTER_RSP;
        	pSockMsg->sender_id = ConvertEntityToId(EME_ENTITY_TR69C_NAME);
        	pSockMsg->receiver_id = ConvertEntityToId(EMA_ENTITY_NAME);
        	pSockMsg->word_data = 0;
        	pSockMsg->msgLength = 0;
            break;
        case CMS_MSG_APP_STOPPED:
            msgLength = sizeof(SOCK_MSG_HEADER_T)+strlen(EME_ENTITY_TR69C_NAME)+1;
            pSockMsg = (SOCK_MSG_HEADER_T *)cmsMem_alloc(msgLength,ALLOC_ZEROIZE);

            pSockMsg->msg_id = EME_MSG_DESTROY_IND;
        	pSockMsg->sender_id = ConvertEntityToId(EME_ENTITY_TR69C_NAME);
        	pSockMsg->receiver_id = ConvertEntityToId(EMA_ENTITY_NAME);
        	pSockMsg->word_data = 0;
        	pSockMsg->msgLength = msgLength-sizeof(SOCK_MSG_HEADER_T);

            strcpy((char*)(pSockMsg+1),EME_ENTITY_TR69C_NAME);
            break;
        default:
            break;
    }

    return pSockMsg;
}

static CmsMsgHeader *cmsMsg_ConvertToCmsMsg(const SOCK_MSG_HEADER_T *buf)
{
    CmsMsgHeader *pMsg = NULL;

    switch (buf->msg_id)
    {
        case EME_MSG_REGISTER_IND:
        {
            pMsg = cmsMem_alloc(sizeof(CmsMsgHeader)+buf->msgLength,ALLOC_ZEROIZE);

            if (pMsg)
            {
                pMsg->type = CMS_MSG_SYSTEM_BOOT;
                pMsg->src = EID_SMD;
                pMsg->dst = EID_TR69C;
                pMsg->wordData = buf->word_data;
                pMsg->dataLength = 0;
            }
        }
            break;
        case EME_MSG_TRANS_COMPLETE_IND:
        {
            const EME_FILE_TRANSFER_COMPLETE_IND_T *pCompleteInd = (const EME_FILE_TRANSFER_COMPLETE_IND_T *)(buf+1);

            if (buf->word_data <= 0)/* upload or download */
            {
                pMsg = cmsMem_alloc(sizeof(CmsMsgHeader)+sizeof(TransferCompleteInd),ALLOC_ZEROIZE);

                if (pMsg)
                {
                    TransferCompleteInd *pRcvInd = NULL;

                    pMsg->type = CMS_MSG_TRANSFER_COMPLETE_IND;
                    pMsg->src = EID_SMD;
                    pMsg->dst = EID_TR69C;
                    pMsg->wordData = 0;
                    pMsg->dataLength = sizeof(TransferCompleteInd);

                    pRcvInd = (TransferCompleteInd *)(pMsg+1);

                    pRcvInd->status = 0;
                    pRcvInd->startDLTime = pCompleteInd->file.startTime;/* pCompleteInd->fileNum should always be 1. */
                    pRcvInd->endDLTime = pCompleteInd->file.completeTime;
                    pRcvInd->dlFaultCode = pCompleteInd->file.result;
                    pRcvInd->dlFaultMsg = NULL;
                    pRcvInd->bDownload = (pCompleteInd->optype==EME_FILE_OP_DOWNLOAD_REQ)?TRUE:FALSE;
                    pRcvInd->bAutonomous = buf->word_data==-1?TRUE:FALSE;
                    snprintf(pRcvInd->filename,sizeof(pRcvInd->filename),"%s",pCompleteInd->file.fileName);
                    pRcvInd->filesize = pCompleteInd->file.fileSize;
                    pRcvInd->filetype = cmsUtl_ConvertToFiletype(pCompleteInd->filetype);
                    /* pRcvInd->announceURL */
                    snprintf(pRcvInd->transferURL,sizeof(pRcvInd->transferURL),"%s",pCompleteInd->file.url);
                }
            }
            else /* du change */
            {
                pMsg = cmsMem_alloc(sizeof(CmsMsgHeader)+sizeof(DUresponseStateChangedMsgBody)*buf->word_data,ALLOC_ZEROIZE);

                if (pMsg)
                {
                    INT32 cnt = 0;
                    DUresponseStateChangedMsgBody *pRcvInd = NULL;

                    pMsg->type = CMS_MSG_RESPONSE_DU_STATE_CHANGE;
                    pMsg->src = EID_SMD;
                    pMsg->dst = EID_TR69C;
                    pMsg->wordData = buf->word_data;
                    pMsg->dataLength = sizeof(DUresponseStateChangedMsgBody)*buf->word_data;

                    pRcvInd = (DUresponseStateChangedMsgBody *)(pMsg+1);

                    for (cnt=0;cnt<pMsg->wordData;cnt++)
                    {
                        snprintf(pRcvInd->operation,sizeof(pRcvInd->operation),"%s",pCompleteInd[cnt].du.operation);
                        snprintf(pRcvInd->URL,sizeof(pRcvInd->URL),"%s",pCompleteInd[cnt].du.URL);
                        snprintf(pRcvInd->UUID,sizeof(pRcvInd->UUID),"%s",pCompleteInd[cnt].du.UUID);
                        snprintf(pRcvInd->version,sizeof(pRcvInd->version),"%s",pCompleteInd[cnt].du.version);
                        snprintf(pRcvInd->EUlist,sizeof(pRcvInd->EUlist),"%s",pCompleteInd[cnt].du.EUlist);
                        snprintf(pRcvInd->DUlist,sizeof(pRcvInd->DUlist),"%s",pCompleteInd[cnt].du.DUlist);
                        pRcvInd->faultCode = pCompleteInd[cnt].du.faultCode;
                        pRcvInd->reqId = pCompleteInd[cnt].du.reqId;
                        snprintf(pRcvInd->startTime,sizeof(pRcvInd->startTime),"%s",pCompleteInd[cnt].du.startTime);
                        snprintf(pRcvInd->completeTime,sizeof(pRcvInd->completeTime),"%s",pCompleteInd[cnt].du.completeTime);
                        pRcvInd->currentState = pCompleteInd[cnt].du.currentState;
                        pRcvInd->resolved = pCompleteInd[cnt].du.resolved;

                        pRcvInd++;
                    }
                }
            }
        }
            break;
        default:
            break;
    }

    return pMsg;
}

CmsRet cmsMsg_send(void *msgHandler, const CmsMsgHeader *buf)
{
    UINT32 totalLen;
    SINT32 rc;
    CmsRet ret=CMSRET_SUCCESS;
    CmsMsgHandle *handle = (CmsMsgHandle *) msgHandler;
    SOCK_MSG_HEADER_T *realMsg = NULL;
    
#ifdef DESKTOP_LINUX
    if (handle->standalone)
    {
        /* just pretend to have sent the message */
        return CMSRET_SUCCESS;
    }
#endif   

    realMsg = cmsMsg_ConvertFromCmsMsg(buf);

    if (realMsg == NULL)
    {
        return CMSRET_INTERNAL_ERROR;
    }

    totalLen = sizeof(SOCK_MSG_HEADER_T) + realMsg->msgLength;

    rc = write(handle->commFd, realMsg, totalLen);
    if (rc < 0)
    {
       if (errno == EPIPE)
       {
          /*
           * This could happen when smd tries to write to an app that
           * has exited.  Don't print out a scary error message.
           * Just return an error code and let upper layer app handle it.
           */
          cmsLog_debug("got EPIPE, dest app is dead");
          ret = CMSRET_DISCONNECTED;
       }
       else
       {
          cmsLog_error(TR069_ALARM_SOCKET_ERROR,"write failed, errno=%d", errno);
          ret = CMSRET_INTERNAL_ERROR;
       }
    }
    else if (rc != (SINT32) totalLen)
    {
       cmsLog_error(TR069_ALARM_SOCKET_ERROR,"unexpected rc %d, expected %u", rc, totalLen);
       ret = CMSRET_INTERNAL_ERROR;
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(realMsg);


    return ret;
}

CmsRet cmsMsg_sendReply(void *msgHandler, const CmsMsgHeader *msg, CmsRet retCode)
{
    return CMSRET_SUCCESS;
}

CmsRet cmsMsg_sendAndGetReply(void *msgHandler, const CmsMsgHeader *buf)
{
   return CMSRET_SUCCESS;
}

static CmsRet getPutbackMsg(void *msgHandler, CmsMsgHeader **buf)
{
   CmsRet ret = CMSRET_NO_MORE_INSTANCES;

   if (buf == NULL)
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"buf is NULL!");
      return CMSRET_INVALID_ARGUMENTS;
   }
   else
   {
      *buf = NULL;
   }

   if (!CMS_MSG_PUTBACK_Q_IS_EMPTY(msgHandler))
   {
      CmsMsgHandle *handle = (CmsMsgHandle *) msgHandler;

      *buf = handle->putBackQueue;
      handle->putBackQueue = (*buf)->next;
      (*buf)->next = NULL;

      ret = CMSRET_SUCCESS;
   }

   return ret;
}

static CmsRet waitForDataAvailable(SINT32 fd, UINT32 timeout)
{
   struct timeval tv;
   fd_set readFds;
   SINT32 rc;

   FD_ZERO(&readFds);
   FD_SET(fd, &readFds);

   tv.tv_sec = timeout / MSECS_IN_SEC;
   tv.tv_usec = (timeout % MSECS_IN_SEC) * USECS_IN_MSEC;

   rc = select(fd+1, &readFds, NULL, NULL, &tv);
   if ((rc == 1) && (FD_ISSET(fd, &readFds)))
   {
      return CMSRET_SUCCESS;
   }
   else
   {
      return CMSRET_TIMED_OUT;
   }
}

CmsRet cmsMsg_receiveWithTimeout(void *msgHandler,
                                 CmsMsgHeader **buf,
                                 UINT32 timeoutMilliSeconds)
{
    CmsMsgHandle *handle = (CmsMsgHandle *) msgHandler;
    UINT32 timeout = timeoutMilliSeconds;
    CmsRet ret;
    SINT32 rc;
    SOCK_MSG_HEADER_T *realMsg;
    CmsMsgHeader *msg;
    
    if (buf == NULL)
    {
       cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"buf is NULL!");
       return CMSRET_INVALID_ARGUMENTS;
    }
    else
    {
       *buf = NULL;
    }

#ifdef DESKTOP_LINUX
    if (handle->standalone)
    {
        *buf = NULL;
        return CMSRET_TIMED_OUT;
    }
#endif   

    /*
    * First check for any messages queued in the putback queue.  If nothing
    * there, then go to the real socket to read.
    */
    ret = getPutbackMsg(msgHandler, buf);
    if (ret == CMSRET_SUCCESS || ret == CMSRET_INVALID_ARGUMENTS)
    {
        return ret;
    }

    if ((ret = waitForDataAvailable(handle->commFd, timeout)) != CMSRET_SUCCESS)
    {
        return ret;
    }

    /*
     * Read just the header in the first read.
     * Do not try to read more because we might get part of 
     * another message in the TCP socket.
     */
    realMsg = (SOCK_MSG_HEADER_T *) cmsMem_alloc(sizeof(SOCK_MSG_HEADER_T), ALLOC_ZEROIZE);
    if (realMsg == NULL)
    {
       cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"alloc of msg header failed");
       return CMSRET_RESOURCE_EXCEEDED;
    }
    
    rc = read(handle->commFd, realMsg, sizeof(SOCK_MSG_HEADER_T));
    if ((rc == 0) ||
        ((rc == -1) && (errno == 131)))  /* new 2.6.21 kernel seems to give us this before rc==0 */
    {
       /* broken connection */
       CMSMEM_FREE_BUF_AND_NULL_PTR(realMsg);
       return CMSRET_DISCONNECTED;
    }
    else if (rc < 0 || rc != sizeof(SOCK_MSG_HEADER_T))
    {
       cmsLog_error(TR069_ALARM_SOCKET_ERROR,"bad read, rc=%d errno=%d", rc, errno);
       CMSMEM_FREE_BUF_AND_NULL_PTR(realMsg);
       return CMSRET_INTERNAL_ERROR;
    }
    
    if (realMsg->msgLength > 0)
    {
       UINT32 totalReadSoFar=0;
       SINT32 totalRemaining=realMsg->msgLength;
       char *inBuf;
       SOCK_MSG_HEADER_T *newMsg;
    
       /* there is additional data in the message */
       newMsg = (SOCK_MSG_HEADER_T *) cmsMem_realloc(realMsg, sizeof(SOCK_MSG_HEADER_T) + realMsg->msgLength);
       if (newMsg == NULL)
       {
          cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"realloc to %d bytes failed", sizeof(SOCK_MSG_HEADER_T) + realMsg->msgLength);
          CMSMEM_FREE_BUF_AND_NULL_PTR(realMsg);
          return CMSRET_RESOURCE_EXCEEDED;
       }
    
       /* orig msg was freed by cmsMem_realloc, so now msg can point to newMsg */
       realMsg = newMsg;
       newMsg = NULL;
    
       inBuf = (char *) (realMsg + 1);
       while (totalReadSoFar < realMsg->msgLength)
       {
          cmsLog_debug("reading segment: soFar=%u total=%d", totalReadSoFar, totalRemaining);

          if ((ret = waitForDataAvailable(handle->commFd, timeout)) != CMSRET_SUCCESS)
          {
             CMSMEM_FREE_BUF_AND_NULL_PTR(realMsg);
             return ret;
          }
    
          rc = read(handle->commFd, inBuf, totalRemaining);
          if (rc <= 0)
          {
             cmsLog_error(TR069_ALARM_SOCKET_ERROR,"bad data read, rc=%d errno=%d readSoFar=%d remaining=%d", rc, errno, totalReadSoFar, totalRemaining);
             CMSMEM_FREE_BUF_AND_NULL_PTR(realMsg);
             return CMSRET_INTERNAL_ERROR;
          }
          else
          {
             inBuf += rc;
             totalReadSoFar += rc;
             totalRemaining -= rc;
          }
       }
    }

    msg = cmsMsg_ConvertToCmsMsg(realMsg);
    CMSMEM_FREE_BUF_AND_NULL_PTR(realMsg);

    if (msg==NULL)
    {
        cmsLog_error(TR069_ALARM_SOCKET_ERROR,"invalid msg(0x%x) received",realMsg->msg_id);
        return CMSRET_INTERNAL_ERROR;
    }
    else if(msg->type == CMS_MSG_SYSTEM_BOOT)
    {
    	emaEntityId = msg->wordData;
    }

    *buf = msg;
    
    return CMSRET_SUCCESS;
}

CmsRet cmsMsg_getEventHandle(const void *msgHandler, void *eventHandle)
{
    const CmsMsgHandle *handle = (const CmsMsgHandle *) msgHandler;
    SINT32 *fdPtr = (SINT32 *) eventHandle;

    *fdPtr = handle->commFd;

    return CMSRET_SUCCESS;
}

CmsEntityId cmsMsg_getHandleEid(const void *msgHandler)
{
    const CmsMsgHandle *handle = (const CmsMsgHandle *) msgHandler;

    return (handle == NULL ? 0 : handle->eid);
}

void cmsMsg_putBack(void *msgHandler, CmsMsgHeader **buf)
{
   CmsMsgHandle *handle = (CmsMsgHandle *) msgHandler;
   CmsMsgHeader *prevMsg;

   (*buf)->next = NULL;

   /* put the new message at the end of the putBackQueue */
   if (handle->putBackQueue == NULL)
   {
      handle->putBackQueue = (*buf);
   }
   else
   {
      prevMsg = handle->putBackQueue;
      while (prevMsg->next != NULL)
      {
         prevMsg = prevMsg->next;
      }

      prevMsg->next = (*buf);
   }

   /* we've taken ownership of this msg, so null out caller's pointer */
   *buf = NULL;

   return;
}


void cmsMsg_sendNoop(void *msgHandler)
{
   return;
}

SINT32 cmsFil_getSize(const char *filename)
{
   struct stat statbuf;
   SINT32 rc;

   rc = stat(filename, &statbuf);

   if (rc == 0)
   {
      return ((SINT32) statbuf.st_size);
   }
   else
   {
      return -1;
   }
}

CmsRet cmsFil_copyToBuffer(const char *filename, UINT8 *buf, UINT32 *bufSize)
{
   SINT32 actualFileSize;
   SINT32 fd, rc;

   if (-1 == (actualFileSize = cmsFil_getSize(filename)))
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"could not get filesize for %s", filename);
      return CMSRET_INTERNAL_ERROR;
   }

   if (*bufSize < (UINT32) actualFileSize)
   {
      cmsLog_error(TR069_ALARM_PARAMETER_ERROR,"user supplied buffer is %d, filename actual size is %d", *bufSize, actualFileSize);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   *bufSize = 0;
       
   fd = open(filename, 0);
   if (fd < 0)
   {
      cmsLog_error(TR069_ALARM_INTERNAL_ERROR,"could not open file %s, errno=%d", filename, errno);
      return CMSRET_INTERNAL_ERROR;
   }

   rc = read(fd, buf, actualFileSize);
   if (rc != actualFileSize)
   {
      cmsLog_error(TR069_ALARM_INTERNAL_ERROR,"read error, got %d, expected %d", rc, actualFileSize);
      close(fd);
      return CMSRET_INTERNAL_ERROR;
   }

   close(fd);

   /* let user know how many bytes was actually copied */
   *bufSize = (UINT32) actualFileSize;
   return CMSRET_SUCCESS;
}

/* handle db notify */
INT32 Tr69cDbNotifyFdr = 0;
INT32 Tr69cDbNotifyFdw = 0;

 
typedef struct {
#define DEVICE_IP_MAX   DEVICE_IP_INTERFACE_CNT

	INT8 ipv4Num;
	INT8 ipv6Num;

	INT8 ipv4mainIndex;
	INT8 ipv6mainIndex;

	struct {
		INT8 address[INET_ADDRSTRLEN];
		INT8 prefixLen;
	} ipv4 [DEVICE_IP_MAX];

	struct {
		INT8 address[INET6_ADDRSTRLEN];
		INT8 prefixLen;
	} ipv6[DEVICE_IP_MAX];
}DeviceIPTable_t;


typedef struct {
    INT32 ipType;/* 4: ipv4, 6 ipv6 */
    INT8 remoteTS[128];
    INT8 virtualIP[128];
}IPsecInfoNode_t;

typedef struct {
    INT32 acsFd;
    INT32 acsIpType;/* 0 : invalid 4: ipv4, 6 ipv6 */
    INT8  acsIp[INET6_ADDRSTRLEN];  /* ipv4 or ipv6 */

    DeviceIPTable_t localIPTable;  /* local ip table */

    INT32 tunnelNum;

#define IPSEC_INFO_NODE_MAX 5
    IPsecInfoNode_t tunnel[IPSEC_INFO_NODE_MAX];
}ConnReqInfo_t;

static ConnReqInfo_t connReqInfo;

#define TR69C_DB_NOTIFY_PIPE_FILE "/tmp/tr69c_db_notify"

#define TR69C_DB_NOTIFY_PARAM_CHANGED_IND            0x0001

#define TR069_NOTIFY_FLAG__CONFIG_CHANGE             0x0001
#define TR069_NOTIFY_FLAG__ACTIVE_CHANGE             0x0002
#define TR069_NOTIFY_FLAG__PASSIVE_CHANGE            0x0004
#define TR069_NOTIFY_FLAG__EXPEDITED_CHANGE          0x0008
#define TR069_NOTIFY_FLAG__QUEUED_CHANGE             0x0010
#define TR069_NOTIFY_FLAG__DIAGNOSTICS_COMPLETE      0x0020
#define TR069_NOTIFY_FLAG__IPSEC_CHANGE_IND          0x0040

static INT32 Tr69cDbNotify_open(BOOL8 bRead)
{
    INT32 fd = 0;

    if (bRead == TRUE)
    {
        if (access(TR69C_DB_NOTIFY_PIPE_FILE,F_OK)==0)
        {
            unlink(TR69C_DB_NOTIFY_PIPE_FILE);
        }

        if (mkfifo(TR69C_DB_NOTIFY_PIPE_FILE,0777) != 0)
		{
			cmsLog_error(TR069_ALARM_PROCESS_ERROR,"mkfifo error for DB notify %s (%d, %s)", 
						 TR69C_DB_NOTIFY_PIPE_FILE, errno, strerror(errno));
			return fd;
		}

        fd = open(TR69C_DB_NOTIFY_PIPE_FILE, O_RDONLY|O_NONBLOCK);
    }
    else
    {
        if (access(TR69C_DB_NOTIFY_PIPE_FILE,F_OK)==0)
        {
            fd = open(TR69C_DB_NOTIFY_PIPE_FILE, O_WRONLY|O_NONBLOCK);
        }
    }

    return fd;
}

static INT32 Tr69cDbNotify_close(INT32 fd)
{
    close(fd);
    return 0;
}

void Tr069ActiveNotifyInd(DM_NOTIFY_T *pPara)
{
    UINT32 cnt = 0;
    INT32 msgbody[3] = {0};
    INT32 lRet = 0;
    UINT32 flags = 0;
    MdmPathDescriptor *pParamList = NULL;
    MdmPathDescriptor *pParamLast = NULL;

    if (Tr69cDbNotifyFdw==0)
    {
        if ((Tr69cDbNotifyFdw = Tr69cDbNotify_open(FALSE))<=0)
        {
            cmsLog_notice("failed to open %s to write data",TR69C_DB_NOTIFY_PIPE_FILE);
            return;
        }
    }

    for (cnt=0;cnt<pPara->ulParaCount;cnt++)
    {
        cmsLog_notice("active changed... %s modified by %s",pPara->pCont[cnt].pucPath,pPara->Modifier);

        if (strcmp(pPara->Modifier,"TR069")!=0)/* not modified by acs */
        {
            flags |= TR069_NOTIFY_FLAG__ACTIVE_CHANGE;

            if (pParamList == NULL)
            {
                pParamLast = cmsMem_alloc(sizeof(MdmPathDescriptor),ALLOC_ZEROIZE);
        
                cmsMdm_fullPathToPathDescriptor(pPara->pCont[cnt].pucPath,pParamLast);
        
                if (pParamList == NULL)
                {
                    pParamList = pParamLast;
                }
                else
                {
                    /* impossible path */
                }
            }
            else
            {
                pParamLast->pNext = cmsMem_alloc(sizeof(MdmPathDescriptor),ALLOC_ZEROIZE);
        
                pParamLast = pParamLast->pNext;
                cmsMdm_fullPathToPathDescriptor(pPara->pCont[cnt].pucPath,pParamLast);
            }

        }
    }

    if (flags != 0)
    {
        memset(msgbody,0,sizeof(msgbody));

        msgbody[0] = TR69C_DB_NOTIFY_PARAM_CHANGED_IND;
        msgbody[1] = flags;
        msgbody[2] = (UINT32)pParamList;

        lRet = write(Tr69cDbNotifyFdw,msgbody,sizeof(msgbody));

        if (lRet < 0)
        {
            cmsLog_notice("write active-notify-params error %d %s to Tr69cDbNotifyFdw(%d)",errno,strerror(errno),Tr69cDbNotifyFdw);
        }
        else if (lRet != sizeof(msgbody))
        {
            cmsLog_notice("write active-notify-params %d bytes < %d bytes to Tr69cDbNotifyFdw(%d)",lRet,sizeof(msgbody),Tr69cDbNotifyFdw);
        }
        else
        {
            cmsLog_debug("TR69C_DB_NOTIFY_PARAM_CHANGED_IND (active-notify) sent by named-pipe");
        }
    }

    return;

}

void Tr069NotifyInd(DM_NOTIFY_T *pPara)
{
    UINT32 cnt = 0;
    INT32 msgbody[3] = {0};
    INT32 lRet = 0;
    UINT32 flags = 0;
    MdmPathDescriptor *pParamList = NULL;
    MdmPathDescriptor *pParamLast = NULL;

    if (Tr69cDbNotifyFdw==0)
    {
        if ((Tr69cDbNotifyFdw = Tr69cDbNotify_open(FALSE))<=0)
        {
            cmsLog_notice("failed to open %s to write data",TR69C_DB_NOTIFY_PIPE_FILE);
            return;
        }
    }

    for (cnt=0;cnt<pPara->ulParaCount;cnt++)
    {
        switch(pPara->pCont[cnt].ulParaId)
        {
        
            case DEVICE_IP_INTERFACE_ID:
		
            case DEVICE_MANAGEMENTSERVER_ENABLECWMP_ID:
            case DEVICE_MANAGEMENTSERVER_URL_ID:
            case DEVICE_MANAGEMENTSERVER_USERNAME_ID:
            case DEVICE_MANAGEMENTSERVER_PASSWORD_ID:
            case DEVICE_MANAGEMENTSERVER_DIFFSERV_ID:
            case DEVICE_MANAGEMENTSERVER_PERIODICINFORMENABLE_ID:
            case DEVICE_MANAGEMENTSERVER_PERIODICINFORMINTERVAL_ID:
            case DEVICE_MANAGEMENTSERVER_PERIODICINFORMTIME_ID:
            case DEVICE_MANAGEMENTSERVER_CONNECTIONREQUESTUSERNAME_ID:
            case DEVICE_MANAGEMENTSERVER_CONNECTIONREQUESTPASSWORD_ID:
            case DEVICE_MANAGEMENTSERVER_UPGRADESMANAGED_ID:
            case DEVICE_MANAGEMENTSERVER_DEFAULTACTIVENOTIFICATIONTHROTTLE_ID:
            case DEVICE_MANAGEMENTSERVER_CWMPRETRYMINIMUMWAITINTERVAL_ID:
            case DEVICE_MANAGEMENTSERVER_CWMPRETRYINTERVALMULTIPLIER_ID:
            case DEVICE_MANAGEMENTSERVER_STUNENABLE_ID:
            case DEVICE_MANAGEMENTSERVER_STUNSERVERADDRESS_ID:
            case DEVICE_MANAGEMENTSERVER_STUNSERVERPORT_ID:
            case DEVICE_MANAGEMENTSERVER_STUNUSERNAME_ID:
            case DEVICE_MANAGEMENTSERVER_STUNPASSWORD_ID:
            case DEVICE_MANAGEMENTSERVER_STUNMAXIMUMKEEPALIVEPERIOD_ID:
            case DEVICE_MANAGEMENTSERVER_STUNMINIMUMKEEPALIVEPERIOD_ID:
            case DEVICE_MANAGEMENTSERVER_INSTANCEMODE_ID:
            case DEVICE_MANAGEMENTSERVER_AUTOCREATEINSTANCES_ID:
		#ifdef OAM_SUPPORT			
            case DEVICE_MANAGEMENTSERVER_AUTONOMOUSTRANSFERCOMPLETEPOLICY_ENABLE_ID:
            case DEVICE_MANAGEMENTSERVER_AUTONOMOUSTRANSFERCOMPLETEPOLICY_TRANSFERTYPEFILTER_ID:
            case DEVICE_MANAGEMENTSERVER_AUTONOMOUSTRANSFERCOMPLETEPOLICY_RESULTTYPEFILTER_ID:
            case DEVICE_MANAGEMENTSERVER_AUTONOMOUSTRANSFERCOMPLETEPOLICY_FILETYPEFILTER_ID:
		#endif
            {
                cmsLog_notice("config changed... %s modified by %s",pPara->pCont[cnt].pucPath,pPara->Modifier);

                flags |= TR069_NOTIFY_FLAG__CONFIG_CHANGE;/* no matter who modify these params, tr69c should update its config */
            }
            break;
		#ifdef OAM_SUPPORT	
            case DEVICE_SERVICES_FAPSERVICE_FAPCONTROL_LTE_OPSTATE_ID:
            case DEVICE_SERVICES_FAPSERVICE_FAPCONTROL_LTE_RFTXSTATUS_ID:
            case DEVICE_SERVICES_FAPSERVICE_FAPCONTROL_UMTS_OPSTATE_ID:
            case DEVICE_SERVICES_FAPSERVICE_FAPCONTROL_UMTS_RFTXSTATUS_ID:
            case DEVICE_SERVICES_FAPSERVICE_FAPCONTROL_CDMA2000_ONEX_OPSTATE_ID:
            case DEVICE_SERVICES_FAPSERVICE_FAPCONTROL_CDMA2000_ONEX_RFTXSTATUS_ID:
            case DEVICE_SERVICES_FAPSERVICE_FAPCONTROL_CDMA2000_HRPD_OPSTATE_ID:
            case DEVICE_SERVICES_FAPSERVICE_FAPCONTROL_CDMA2000_HRPD_RFTXSTATUS_ID:
            case DEVICE_SERVICES_FAPSERVICE_REM_UMTS_WCDMA_SCANSTATUS_ID:
            case DEVICE_SERVICES_FAPSERVICE_REM_UMTS_WCDMA_LASTSCANTIME_ID:
            case DEVICE_SERVICES_FAPSERVICE_REM_UMTS_GSM_SCANSTATUS_ID:    
            case DEVICE_SERVICES_FAPSERVICE_REM_UMTS_GSM_LASTSCANTIME_ID:
            case DEVICE_SERVICES_FAPSERVICE_REM_LTE_SCANSTATUS_ID:
            case DEVICE_SERVICES_FAPSERVICE_REM_LTE_LASTSCANTIME_ID:
            case DEVICE_GATEWAYINFO_MANUFACTUREROUI_ID:
            case DEVICE_GATEWAYINFO_PRODUCTCLASS_ID:
            case DEVICE_GATEWAYINFO_SERIALNUMBER_ID:
            case DEVICE_FAP_GPS_LASTSCANTIME_ID:
            case DEVICE_FAP_GPS_CONTINUOUSGPSSTATUS_CURRENTFIX_ID:
		#endif
            case DEVICE_MANAGEMENTSERVER_CONNREQJABBERID_ID:
            case DEVICE_MANAGEMENTSERVER_CONNECTIONREQUESTURL_ID:
            case DEVICE_DEVICEINFO_SOFTWAREVERSION_ID:
            case DEVICE_DEVICEINFO_PROVISIONINGCODE_ID:
            {
                cmsLog_notice("active changed... %s modified by %s",pPara->pCont[cnt].pucPath,pPara->Modifier);

                if (strcmp(pPara->Modifier,"TR069")!=0)/* not modified by acs */
                {
                    flags |= TR069_NOTIFY_FLAG__ACTIVE_CHANGE;

                    if (pParamList == NULL)
                    {
                        pParamLast = cmsMem_alloc(sizeof(MdmPathDescriptor),ALLOC_ZEROIZE);
                
                        cmsMdm_fullPathToPathDescriptor(pPara->pCont[cnt].pucPath,pParamLast);
                
                        if (pParamList == NULL)
                        {
                            pParamList = pParamLast;
                        }
                        else
                        {
                            /* impossible path */
                        }
                    }
                    else
                    {
                        pParamLast->pNext = cmsMem_alloc(sizeof(MdmPathDescriptor),ALLOC_ZEROIZE);
                
                        pParamLast = pParamLast->pNext;
                        cmsMdm_fullPathToPathDescriptor(pPara->pCont[cnt].pucPath,pParamLast);
                    }

                }
            }
            break;
		#ifdef OAM_SUPPORT
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_ID:
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_EVENTTIME_ID:
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_ALARMIDENTIFIER_ID:
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_NOTIFICATIONTYPE_ID:
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_MANAGEDOBJECTINSTANCE_ID:
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_EVENTTYPE_ID:
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_PROBABLECAUSE_ID:
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_SPECIFICPROBLEM_ID:
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_PERCEIVEDSEVERITY_ID:
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_ADDITIONALTEXT_ID:
            case DEVICE_FAULTMGMT_EXPEDITEDEVENT_ADDITIONALINFORMATION_ID:
            {
                cmsLog_notice("expedited-alarm-table changed... %s modified by %s",pPara->pCont[cnt].pucPath,pPara->Modifier);

                if (strcmp(pPara->Modifier,"TR069")!=0)/* not modified by acs */
                {
                    flags |= TR069_NOTIFY_FLAG__EXPEDITED_CHANGE|TR069_NOTIFY_FLAG__ACTIVE_CHANGE;

                    if (pParamList == NULL)
                    {
                        pParamLast = cmsMem_alloc(sizeof(MdmPathDescriptor),ALLOC_ZEROIZE);
                
                        cmsMdm_fullPathToPathDescriptor(pPara->pCont[cnt].pucPath,pParamLast);
                
                        if (pParamList == NULL)
                        {
                            pParamList = pParamLast;
                        }
                        else
                        {
                            /* impossible path */
                        }
                    }
                    else
                    {
                        pParamLast->pNext = cmsMem_alloc(sizeof(MdmPathDescriptor),ALLOC_ZEROIZE);
                
                        pParamLast = pParamLast->pNext;
                        cmsMdm_fullPathToPathDescriptor(pPara->pCont[cnt].pucPath,pParamLast);
                    }

                }
            }
            break;
		
		
            case DEVICE_FAULTMGMT_QUEUEDEVENT_ID:
            case DEVICE_FAULTMGMT_QUEUEDEVENT_EVENTTIME_ID:
            case DEVICE_FAULTMGMT_QUEUEDEVENT_ALARMIDENTIFIER_ID:
            case DEVICE_FAULTMGMT_QUEUEDEVENT_NOTIFICATIONTYPE_ID:
            case DEVICE_FAULTMGMT_QUEUEDEVENT_MANAGEDOBJECTINSTANCE_ID:
            case DEVICE_FAULTMGMT_QUEUEDEVENT_EVENTTYPE_ID:
            case DEVICE_FAULTMGMT_QUEUEDEVENT_PROBABLECAUSE_ID:
            case DEVICE_FAULTMGMT_QUEUEDEVENT_SPECIFICPROBLEM_ID:
            case DEVICE_FAULTMGMT_QUEUEDEVENT_PERCEIVEDSEVERITY_ID:
            case DEVICE_FAULTMGMT_QUEUEDEVENT_ADDITIONALTEXT_ID:
            case DEVICE_FAULTMGMT_QUEUEDEVENT_ADDITIONALINFORMATION_ID:
            {
                cmsLog_notice("queued-alarm-table changed... %s modified by %s",pPara->pCont[cnt].pucPath,pPara->Modifier);

                if (strcmp(pPara->Modifier,"TR069")!=0)/* not modified by acs */
                {
                    flags |= TR069_NOTIFY_FLAG__QUEUED_CHANGE|TR069_NOTIFY_FLAG__PASSIVE_CHANGE;

                    if (pParamList == NULL)
                    {
                        pParamLast = cmsMem_alloc(sizeof(MdmPathDescriptor),ALLOC_ZEROIZE);
                
                        cmsMdm_fullPathToPathDescriptor(pPara->pCont[cnt].pucPath,pParamLast);
                
                        if (pParamList == NULL)
                        {
                            pParamList = pParamLast;
                        }
                        else
                        {
                            /* impossible path */
                        }
                    }
                    else
                    {
                        pParamLast->pNext = cmsMem_alloc(sizeof(MdmPathDescriptor),ALLOC_ZEROIZE);
                
                        pParamLast = pParamLast->pNext;
                        cmsMdm_fullPathToPathDescriptor(pPara->pCont[cnt].pucPath,pParamLast);
                    }

                }
            }
            break;
            case DEVICE_IPSEC_IKEV2SA_ID:
            {
                cmsLog_notice("ipsec status changed... %s modified by %s",pPara->pCont[cnt].pucPath,pPara->Modifier);
                flags |= TR069_NOTIFY_FLAG__IPSEC_CHANGE_IND;
            }
            break;
		#endif
            default:
            {
                cmsLog_notice("temperorily no handle for %s",pPara->pCont[cnt].pucPath);
            }
            break;
        }
    }

    if (flags != 0)
    {
        memset(msgbody,0,sizeof(msgbody));

        msgbody[0] = TR69C_DB_NOTIFY_PARAM_CHANGED_IND;
        msgbody[1] = flags;
        msgbody[2] = (UINT32)pParamList;

        lRet = write(Tr69cDbNotifyFdw,msgbody,sizeof(msgbody));

        if (lRet < 0)
        {
            cmsLog_notice("write changed-params error %d %s to Tr69cDbNotifyFdw(%d)",errno,strerror(errno),Tr69cDbNotifyFdw);
        }
        else if (lRet != sizeof(msgbody))
        {
            cmsLog_notice("write changed-params %d bytes < %d bytes to Tr69cDbNotifyFdw(%d)",lRet,sizeof(msgbody),Tr69cDbNotifyFdw);
        }
        else
        {
            cmsLog_debug("TR69C_DB_NOTIFY_PARAM_CHANGED_IND sent by named-pipe");
        }
    }

    return;
}

static PhlGetParamValue_t *tr69cNotifyParaFirst = NULL;
static PhlGetParamValue_t *tr69cNotifyParaLast = NULL;


static PhlGetParamValue_t *tr69cNotifyParaFirst2 = NULL; 
static PhlGetParamValue_t *tr69cNotifyParaLast2 = NULL;
static UINT32 tr69cNotifyParaCounter = 0;
static UINT32 tr69cNotifyPara2Counter = 0;


static UBOOL8 ActiveAlarmNeeded = 0;
static UBOOL8 ActiveAlarmNeeded2 = 0;

static UBOOL8 PassiveAlarmNeeded = 0;
static UBOOL8 PassiveAlarmNeeded2 = 0;

static UBOOL8 DiagnosticsComplete = 0;
static UBOOL8 DiagnosticsComplete2 = 0;

static BOOL8 IsSameSubnet(INT8 ipType, INT8* ip1, INT8* ip2, INT8 prefixLen)
{
	BOOL8 compareResult = TRUE;
	INT32 i;
	INT32 OctetSize = 0;
	UINT8 ip1Octet[16], ip2Octet[16];

	if(ipType == 4)
	{
		inet_pton(AF_INET, ip1, ip1Octet);
		inet_pton(AF_INET, ip2, ip2Octet);

		OctetSize = 4;
	}
	else
	{
		inet_pton(AF_INET6, ip1, ip1Octet);
		inet_pton(AF_INET6, ip2, ip2Octet);

		OctetSize = 16;
	}

	for(i = 0; i < OctetSize; i++)
	{
		UINT8 bytes = prefixLen / 8;
		UINT8 bits  = prefixLen % 8;
		UINT8 mask = (i < bytes) ? 0xFF : (UINT8)(0xFF << (8 - bits));

		if((ip1Octet[i] & mask) != (ip2Octet[i] & mask))
		{
			compareResult = FALSE;
			break;
		}
		if(mask != 0xFF)
		{
			break;
		}
	}

	return compareResult;
}


static BOOL8 cmsPhl_lookupPeerLocalIp(INT8 ipType, DeviceIPTable_t* pLocalIPTable, INT8* remoteIP, INT8* localIp)
{
	INT32 i, mainIpIndex;
	BOOL8 bFound = FALSE;

	if(ipType == 4)
	{
		for(i = 0; i < pLocalIPTable->ipv4Num; i++)
		{
			if(IsSameSubnet(ipType, pLocalIPTable->ipv4[i].address, remoteIP, pLocalIPTable->ipv4[i].prefixLen))
			{
				sprintf(localIp, "%s", pLocalIPTable->ipv4[i].address);
				bFound = TRUE;
				break;
			}
		}

		mainIpIndex = pLocalIPTable->ipv4mainIndex;
		if(!bFound && mainIpIndex < pLocalIPTable->ipv4Num)
		{
			sprintf(localIp, "%s", pLocalIPTable->ipv4[mainIpIndex].address);
			bFound = TRUE;
		}
	}
	else if(ipType == 6)
	{
		for(i = 0; i < pLocalIPTable->ipv6Num; i++)
		{
			if(IsSameSubnet(ipType, pLocalIPTable->ipv6[i].address, remoteIP, pLocalIPTable->ipv6[i].prefixLen))
			{
				sprintf(localIp, "%s", pLocalIPTable->ipv6[i].address);
				bFound = TRUE;
				break;
			}
		}

		mainIpIndex = pLocalIPTable->ipv6mainIndex;
		if(!bFound && mainIpIndex < pLocalIPTable->ipv6Num)
		{
			sprintf(localIp, "%s", pLocalIPTable->ipv6[mainIpIndex].address);
			bFound = TRUE;
		}
	}

    return bFound;
}



static void cmsPhl_AddLocalIpv4ToTable(DEVICE_IP_INTERFACE_T* ipInterface, DeviceIPTable_t* pLocalIPTable)
{
	UINT32 k, x, y;
	BOOL8  isFirstNewIndex = TRUE;

	DEVICE_IP_INTERFACE_IPV4ADDRESS_T* IPv4Address = ipInterface->IPv4Address;

	for(k = 0; k < ipInterface->IPv4Address_cnt; k++)
	{
		if (IPv4Address[k].AddressingType == DEVICE_IP_INTERFACE_IPV4ADDRESS_ADDRESSINGTYPE_DHCP ||
			IPv4Address[k].AddressingType == DEVICE_IP_INTERFACE_IPV4ADDRESS_ADDRESSINGTYPE_STATIC)
		{
			if(pLocalIPTable->ipv4Num >= DEVICE_IP_MAX)
			{
				cmsLog_notice("There are no enough space to store ipv4 address");
				break;
			}

			INT32 newIpIndex = pLocalIPTable->ipv4Num;
			pLocalIPTable->ipv4Num ++;

			inet_ntop(AF_INET, IPv4Address[k].IPAddress.u.aucIpv4Octet, pLocalIPTable->ipv4[newIpIndex].address, INET_ADDRSTRLEN);

			pLocalIPTable->ipv4[newIpIndex].prefixLen = 0;
			for(x = 0; x < sizeof(IPv4Address[k].SubnetMask.u.aucIpv4Octet)/sizeof(UINT8); x ++)
			{
				UINT8 netMaskByte = IPv4Address[k].SubnetMask.u.aucIpv4Octet[x];
				UINT8 bitMaskFlag = 0x80;
				for(y = 0; y < 8 && ( (netMaskByte & bitMaskFlag) == bitMaskFlag); y++)
				{
					pLocalIPTable->ipv4[newIpIndex].prefixLen ++;
					bitMaskFlag >>= 1;
				}

				if(netMaskByte != 0xFF)
				{
					break;
				}
			}

			if(ipInterface->MainInterface && isFirstNewIndex)
			{
				pLocalIPTable->ipv4mainIndex = newIpIndex;
				isFirstNewIndex = FALSE;
			}
		}
	}

	return;
}

static void cmsPhl_AddLocalIpv6ToTable(DEVICE_IP_INTERFACE_T* ipInterface, DeviceIPTable_t* pLocalIPTable)
{
	UINT32 k, x;
	BOOL8  isFirstNewIndex = TRUE;
	INT8   pucPath[256];

	DEVICE_IP_INTERFACE_IPV6ADDRESS_T* IPv6Address = ipInterface->IPv6Address;

	for(k = 0; k < ipInterface->IPv6Address_cnt; k++)
	{
		if (IPv6Address[k].Origin == DEVICE_IP_INTERFACE_IPV6ADDRESS_ORIGIN_DHCPV6 ||
			IPv6Address[k].Origin == DEVICE_IP_INTERFACE_IPV6ADDRESS_ORIGIN_STATIC)
		{
			if(pLocalIPTable->ipv6Num >= DEVICE_IP_MAX)
			{
				cmsLog_notice("There are no enough space to store ipv6 address");
				break;
			}

			INT32 newIpIndex = pLocalIPTable->ipv6Num;
			pLocalIPTable->ipv6Num ++;

			inet_ntop(AF_INET6, IPv6Address[k].IPAddress.u.aucIpv6Octet, pLocalIPTable->ipv6[newIpIndex].address, INET6_ADDRSTRLEN);

			pLocalIPTable->ipv6[newIpIndex].prefixLen = 64;

			snprintf(pucPath, sizeof(pucPath), "Device.IP.Interface.%d.IPv6Prefix.", ipInterface->instanceNum);
			UINT32 prefixInstanceNum = atoi(IPv6Address[k].Prefix + strlen(pucPath));
			for(x = 0; x < ipInterface->IPv6Prefix_cnt; x ++)
			{
				if(ipInterface->IPv6Prefix[x].instanceNum == prefixInstanceNum)
				{
					INT8* pToken = strchr(ipInterface->IPv6Prefix[x].Prefix, '/');
					if(pToken != NULL)
					{
						pLocalIPTable->ipv6[newIpIndex].prefixLen = atoi(pToken + 1);
					}
					break;
				}
			}

			if(ipInterface->MainInterface && isFirstNewIndex)
			{
				pLocalIPTable->ipv6mainIndex = newIpIndex;
				isFirstNewIndex = FALSE;
			}
		}
	}

	return;
}
#ifdef OAM_SUPPORT 

void cmsPhl_updateLocalIPs(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    UINT32 cnt = 0;
    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    DEVICE_IP_INTERFACE_T *ipInterafces = NULL;

    cmsLog_notice("enter cmsPhl_updateLocalIPs");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));

    ret = DmAllocateMemReq(sizeof(DEVICE_IP_INTERFACE_T)*DEVICE_IP_INTERFACE_CNT, (void**)&ipInterafces);

    if (ret || !ipInterafces)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for ipInterafces");
        return;
    }

    memset(ipInterafces,0,sizeof(DEVICE_IP_INTERFACE_T)*DEVICE_IP_INTERFACE_CNT);
    memset(&connReqInfo.localIPTable,0,sizeof(connReqInfo.localIPTable));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device.IP.Interface.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_updateLocalIPs",ret);

        ret = DmReleaseMemReq((void**)&ipInterafces);

        if (ret || !ipInterafces)
        {
            cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for ipInterafces");
        }
        return;
    }

    getValue.TransactionId = transReq.TransactionId;
    getValue.ulParaId = DEVICE_IP_INTERFACE_ID;
    getValue.pucPath = "Device.IP.Interface.";
    getValue.ulCnt = 0;
    getValue.pValue = ipInterafces;
    getValue.ValueSize = sizeof(DEVICE_IP_INTERFACE_T)*DEVICE_IP_INTERFACE_CNT;
    getValue.bShmWay = DM_TRUE;

    ret = DmGetParaCValueReq(&getValue);

    if (ret != 0)
    {
        cmsLog_notice("DmGetParaCValueReq failed lRet=%d, maybe no IP Interface instances, just ignored",ret);
    }
    else
    {
        for (cnt=0;cnt<getValue.ulCnt;cnt++)
        {
            if (ipInterafces[cnt].Type == DEVICE_IP_INTERFACE_TYPE_NORMAL)
            {
            	// ipv4
            	if (ipInterafces[cnt].IPv4Enable && ipInterafces[cnt].IPv4Address_cnt>=1)
            	{
            		cmsPhl_AddLocalIpv4ToTable(ipInterafces + cnt, &connReqInfo.localIPTable);
            	}

            	// ipv6
            	if (ipInterafces[cnt].IPv6Enable && ipInterafces[cnt].IPv6Address_cnt>=1)
            	{
            		cmsPhl_AddLocalIpv6ToTable(ipInterafces + cnt, &connReqInfo.localIPTable);
            	}
            }
        }

        cmsLog_notice("%d ipv4 interfaces and %d ipv6 is found", connReqInfo.localIPTable.ipv4Num, connReqInfo.localIPTable.ipv6Num);
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_updateLocalIPs",ret);
    }

    ret = DmReleaseMemReq((void**)&ipInterafces);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory of ipInterafces");
    }

    cmsLog_notice("leave cmsPhl_updateLocalIPs");

    return;
}

#endif
static void cmsPhl_setParamValueChanges(MdmPathDescriptor *pValueChanges, UINT32 saveId)
{
    INT32 lRet = 0;
    UINT32 cnt = 0;
    UINT32 paraCnt = 0;

    MdmPathDescriptor *pTmp = NULL;
    MdmPathDescriptor *pLst = NULL;

    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_VALUE_REQ_T getValue;
    PARAMETER_NAME_LIST_T nameList;

    UINT32 *notifyParaCounter = NULL;
    PhlGetParamValue_t *pValueNode = NULL;
	PhlGetParamValue_t **pFirst = NULL;
	PhlGetParamValue_t **pLast = NULL;
    INT8 *pRootNode = NULL;

    cmsLog_notice("enter cmsPhl_setParamValueChanges");

	switch(saveId)
	{
	case 0:
		pFirst = &tr69cNotifyParaFirst;
		pLast = &tr69cNotifyParaLast;
        notifyParaCounter = &tr69cNotifyParaCounter;
        cmsLog_notice("try to save params to tr69cNotifyParaFirst in cmsPhl_setParamValueChanges");
		break;
	case 1:
		pFirst = &tr69cNotifyParaFirst2;
		pLast = &tr69cNotifyParaLast2;
        notifyParaCounter = &tr69cNotifyPara2Counter;
        cmsLog_notice("try to save params to tr69cNotifyParaFirst2 in cmsPhl_setParamValueChanges");
		break;
	default:
		return;
	}

    pTmp = pValueChanges;
    paraCnt = 0;

    while (pTmp != NULL)
    {
        paraCnt++;
        pTmp = pTmp->pNext;
    }

    cmsLog_notice("paraCnt=%d",paraCnt);

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_VALUE_REQ_T));
    memset(&nameList, 0, sizeof(PARAMETER_NAME_LIST_T));


    pRootNode = CmsPhl_GetRootNode(pValueChanges->fullpath);

    strcpy(transReq.AccessEntity,"TR069");
    strcpy(transReq.RootNodeToLock,pRootNode);
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    CMSMEM_FREE_BUF_AND_NULL_PTR(pRootNode);

    lRet = DmTransactionStartReq(&transReq);

    if (lRet != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d",lRet);
        return;
    }    

    getValue.TransactionId = transReq.TransactionId;
    getValue.pNameList = &nameList;
    getValue.pNameList->udwCnt = paraCnt;
    getValue.pNameList->pucParamName = cmsMem_alloc(sizeof(PARAMETER_NAME_T)*paraCnt, ALLOC_ZEROIZE);

    if(getValue.pNameList->pucParamName==NULL)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"malloc() failed for getValue.pNameList->pucParamName");
        return;
    }

    pTmp = pValueChanges;
    cnt = 0;

    for (cnt=0;pTmp != NULL && cnt<paraCnt;cnt++)
    {
        getValue.pNameList->pucParamName[cnt].pucName = cmsMem_strdup(pTmp->fullpath);
        cmsLog_notice("[%d/%d] namelist %s",cnt,paraCnt,getValue.pNameList->pucParamName[cnt].pucName);

        pTmp = pTmp->pNext;
    }

    lRet = DmGetParaValueReq(&getValue);

    if (lRet != DM_RET_SUCCESS)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaValueReq failed, lRet=%d",lRet);
    }
    else
    {
        for (cnt=0;cnt<getValue.pvList->udwCnt;cnt++)
        {
            pValueNode = (PhlGetParamValue_t*)cmsMem_alloc(sizeof(PhlGetParamValue_t),ALLOC_ZEROIZE);
            if(pValueNode==NULL)
            {
                cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"malloc() failed for pValueNode");
                break;
            }
            (*notifyParaCounter)++;

            cmsMdm_fullPathToPathDescriptor(getValue.pvList->pstParamValue[cnt].pucName,&pValueNode->pathDesc);

            pValueNode->pParamType = cmsMem_strdup((INT8*)getValue.pvList->pstParamValue[cnt].pucType);
            pValueNode->isTr69Password = getValue.pvList->pstParamValue[cnt].bTr69Password;

            if (getValue.pvList->pstParamValue[cnt].pucValue==NULL 
                || strlen(getValue.pvList->pstParamValue[cnt].pucValue)<=0)
            {
                pValueNode->pValue = cmsMem_strdup("");
            }
            else
            {
                pValueNode->pValue = cmsMem_strdup(getValue.pvList->pstParamValue[cnt].pucValue);
            }

            cmsLog_notice("[%d/%d] valuelist %s=%s type=%s isTr69Password=%d",cnt,getValue.pvList->udwCnt,pValueNode->pathDesc.fullpath,pValueNode->pValue,pValueNode->pParamType,pValueNode->isTr69Password);

            if (*pFirst == NULL)
            {
                *pFirst = pValueNode;
            }
            else
            {
                (*pLast)->pNext = pValueNode;
            }

            (*pLast) = pValueNode;
        }

        getValue.pfnFree(&getValue);
    }

    for (cnt=0;cnt<paraCnt;cnt++)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(getValue.pNameList->pucParamName[cnt].pucName);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(getValue.pNameList->pucParamName);

    lRet = DmTransactionStopReq(transReq.TransactionId);

    if (lRet != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStopReq failed, lRet=%d",lRet);
    }

    pTmp = pValueChanges;
    while(pTmp != NULL)
    {
        pLst = pTmp;
        pTmp = pTmp->pNext;

        CMSMEM_FREE_BUF_AND_NULL_PTR(pLst);
    }

    cmsLog_notice("leave tr69cSaveNotifyMsg");

	return;
}

static void readMessageFromTr69cDbNotify(void *handle)
{
    INT32 lRet = 0;
    INT32 msg[3] = {0};/* msg[0] : msg_id, msg[1] : msg_flags, msg[2]: msg_body */

    stopListener(Tr69cDbNotifyFdr);

    while(1)
    {
        memset(msg,0,sizeof(msg));

        lRet = read(Tr69cDbNotifyFdr, &msg, sizeof(msg));

        if (lRet == 0)
        {
            cmsLog_debug("no msg available from Tr69cDbNotifyFdr(%d)",Tr69cDbNotifyFdr);
            break;
        }
        else if (lRet < 0)
        {
            cmsLog_debug("read failed from Tr69cDbNotifyFdr(%d), errno=%d %s",Tr69cDbNotifyFdr,errno,strerror(errno));
            break;
        }
        else if (lRet < sizeof(msg))
        {
            cmsLog_debug("read %d bytes < %d bytes from Tr69cDbNotifyFdr(%d)",Tr69cDbNotifyFdr, lRet, sizeof(msg));
            break;
        }

        if (msg[0] == TR69C_DB_NOTIFY_PARAM_CHANGED_IND)
        {
            if (msg[1]&TR069_NOTIFY_FLAG__CONFIG_CHANGE)
            {
                cmsLog_notice("config changed");
                updateTr69cCfgInfo();
            }

            if (msg[1]&TR069_NOTIFY_FLAG__IPSEC_CHANGE_IND)
            {
                cmsLog_notice("ipsec changed");
                //cmsPhl_updateIPsecInfo();
            }

            if (msg[1]&TR069_NOTIFY_FLAG__ACTIVE_CHANGE)
            {
                cmsLog_notice("active params changed acsState.enableSelfdefinedEventCode=%d",acsState.enableSelfdefinedEventCode);
                if(eSessionEnd == sessionState)
                {
                    cmsPhl_setParamValueChanges((void*)(msg[2]),0);
                    addInformEventToList(INFORM_EVENT_VALUE_CHANGE);

                    if (msg[1]&TR069_NOTIFY_FLAG__EXPEDITED_CHANGE
                        && acsState.enableSelfdefinedEventCode)
                    {
                        addInformEventToList(INFORM_EVENT_VENDOR_ALARM);
                        ActiveAlarmNeeded = 1;
                    }

                    if (msg[1]&TR069_NOTIFY_FLAG__DIAGNOSTICS_COMPLETE)
                    {
                        addInformEventToList(INFORM_EVENT_DIAGNOSTICS_COMPLETE);
                        DiagnosticsComplete = 1;
                    }

                    sendInform(NULL);
                }
                else
                {
                    cmsLog_notice("sessionState(%d), active params pending, acsState.enableSelfdefinedEventCode=%d",sessionState,acsState.enableSelfdefinedEventCode);
                    if (msg[1]&TR069_NOTIFY_FLAG__EXPEDITED_CHANGE
                        && acsState.enableSelfdefinedEventCode)
                    {
                        ActiveAlarmNeeded2 = 1;
                    }
                    if (msg[1]&TR069_NOTIFY_FLAG__DIAGNOSTICS_COMPLETE)
                    {
                        DiagnosticsComplete2 = 1;
                    }

                    cmsPhl_setParamValueChanges((void*)(msg[2]),1);
                }
            }

            if (msg[1]&TR069_NOTIFY_FLAG__PASSIVE_CHANGE)
            {
                cmsLog_notice("passive params changed acsState.enableSelfdefinedEventCode=%d",acsState.enableSelfdefinedEventCode);
                if(eSessionEnd == sessionState)
                {
                    cmsPhl_setParamValueChanges((void*)(msg[2]),0);
                    if (msg[1]&TR069_NOTIFY_FLAG__QUEUED_CHANGE
                        && acsState.enableSelfdefinedEventCode)
                    {
                        PassiveAlarmNeeded = 1;
                    }
                }
                else
                {
                    cmsLog_notice("sessionState(%d), active params pending, acsState.enableSelfdefinedEventCode=%d",sessionState,acsState.enableSelfdefinedEventCode);
                    cmsPhl_setParamValueChanges((void*)(msg[2]),1);

                    if (msg[1]&TR069_NOTIFY_FLAG__QUEUED_CHANGE
                        && acsState.enableSelfdefinedEventCode)
                    {
                        PassiveAlarmNeeded2 = 1;
                    }
                }
            }
        }
    }

    setListener(Tr69cDbNotifyFdr, readMessageFromTr69cDbNotify, NULL);

    return;
}

CmsRet cmsMdm_initWithAcc(CmsEntityId eid, UINT32 acc, void *msgHandler, SINT32 *shmId)
{
	INT32 ret;
    DM_START_REQ_T startReq;

    /* dm initialized here */
    osa_mem_set(&startReq,0,sizeof(DM_START_REQ_T));
    snprintf(startReq.entity,sizeof(startReq.entity),"%s",EME_ENTITY_TR69C_NAME);

    ret = DmStartReq(&startReq);

    if (ret != DM_RET_SUCCESS)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmStartReq failed, ret=%d", ret);
    }

    Tr69cDbNotifyFdr = Tr69cDbNotify_open(TRUE);
    setListener(Tr69cDbNotifyFdr, readMessageFromTr69cDbNotify, NULL);

    Tr069RegisterDbCbsHandler();/* register db-callback */
#ifdef OAM_SUPPORT
    cmsPhl_updateLocalIPs();
#endif
    //cmsPhl_updateIPsecInfo();

    //devs_start_req();

    return CMSRET_SUCCESS;
}

void cmsMdm_cleanup(void)
{
    DmStopReq(DM_FALSE);
    
    Tr69cDbNotify_close(Tr69cDbNotifyFdr);
    Tr69cDbNotify_close(Tr69cDbNotifyFdw);
    Tr69cDbNotifyFdr = 0;
    Tr69cDbNotifyFdw = 0;

    //devs_stop_req();

    return;
}

CmsRet cmsMdm_fullPathToPathDescriptor(const char *fullpath, MdmPathDescriptor *pathDesc)
{
    CmsRet ret = CMSRET_INTERNAL_ERROR;

    memset(pathDesc,0,sizeof(MdmPathDescriptor));

    if (fullpath && strlen(fullpath)>0)
    {
        snprintf(pathDesc->fullpath,sizeof(pathDesc->fullpath),"%s",fullpath);

        if (fullpath[strlen(fullpath)-1] != '.')
        {
            INT8 *pParam = strrchr(fullpath,'.');

            if (pParam)
            {
                pParam++;
                snprintf(pathDesc->paramName,sizeof(pathDesc->paramName),"%s",pParam);
            }
        }

        ret = CMSRET_SUCCESS;
    }

    return ret;
}

CmsRet cmsMdm_pathDescriptorToFullPath(const MdmPathDescriptor *pathDesc, char **fullpath)
{
    CmsRet ret = CMSRET_SUCCESS;

    *fullpath = cmsMem_strdup(pathDesc->fullpath);

    return ret;
}

UBOOL8 cmsMdm_isDataModelDevice2(void)
{
    return TRUE;
}

static char* ConvertIntToStr(INT32 Num)
{
    char acStr[50] = {0};

    snprintf(acStr, sizeof(acStr), "%d", Num);

    return cmsMem_strdup(acStr);
}

INT8* CmsPhl_GetRootNode(INT8    *pFullPathName)
{
    INT8       *pRootNode  = NULL;
    INT8        *pTemp      = NULL;

    pRootNode = cmsMem_strdup(pFullPathName);

    pTemp= strstr((INT8*)pRootNode, ".");

    if (pTemp == NULL)
    {
        cmsLog_notice("failed to find the root node of %s, in CmsPhl_GetRootNode",pFullPathName);
        CMSMEM_FREE_BUF_AND_NULL_PTR(pRootNode);
        return NULL;
    }
    else
    {
        pTemp++;
        *pTemp = '\0';
        cmsLog_notice("root node is '%s'",pRootNode);
    }

    return pRootNode;
}

CmsRet cmsPhl_getParameterNames(MdmPathDescriptor *pNamePath,
                                UBOOL8            nextLevelOnly,
                                PhlGetParamInfo_t **pParamInfoList,
                                SINT32            *pNumEntries)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    UINT32 cnt = 0;
    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_NAME_REQ_T getName;
    INT8 *pRootNode = NULL;

    cmsLog_notice("enter cmsPhl_getParameterNames");

    if(!checkEmeSession())   // check if Ema session is ready or not
    {
    	return CMSRET_REQUEST_DENIED;
    }

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getName, 0, sizeof(DM_GET_PARA_NAME_REQ_T));

    pRootNode = CmsPhl_GetRootNode(pNamePath->fullpath);

    strcpy(transReq.AccessEntity,"TR069");
    strcpy(transReq.RootNodeToLock,pRootNode);
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    CMSMEM_FREE_BUF_AND_NULL_PTR(pRootNode);

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getParameterNames",ret);

        return ret;
    }

    getName.TransactionId = transReq.TransactionId;
    getName.pucParamName = cmsMem_strdup(pNamePath->fullpath);
    getName.bNextLevel = nextLevelOnly;
    getName.pfnFree = NULL;
    getName.pNameList = NULL;

    cmsLog_notice("name-req %s",pNamePath->fullpath);

    ret = DmGetParaNameReq(&getName);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaNameReq failed lRet=%d, leave cmsPhl_getParameterNames",ret);

        result = ret;
    }
    else
    {
        *pNumEntries = getName.pNameList->udwCnt;

        if (*pNumEntries!=0)
        {
            *pParamInfoList = (PhlGetParamInfo_t*)cmsMem_alloc(sizeof(PhlGetParamInfo_t)*(getName.pNameList->udwCnt),ALLOC_ZEROIZE);

            if (*pParamInfoList==NULL)
            {
                DmTransactionStopReq(transReq.TransactionId);
                CMSMEM_FREE_BUF_AND_NULL_PTR(getName.pucParamName);
                getName.pfnFree(&getName);
                cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"malloc() failed, count=%d in cmsPhl_getParameterNames",*pNumEntries);
                return CMSRET_INTERNAL_ERROR;
            }
        }
        else
        {
             *pParamInfoList = NULL;
        }

        for (cnt=0;cnt<getName.pNameList->udwCnt;cnt++)
        {
            snprintf((*pParamInfoList)[cnt].pathDesc.fullpath,sizeof((*pParamInfoList)[cnt].pathDesc.fullpath),"%s",getName.pNameList->pucParamName[cnt].pucName);
            (*pParamInfoList)[cnt].writable = getName.pNameList->pucParamName[cnt].bWritable;

            cmsLog_notice("[%d/%d] ret-name %s writable=%d",cnt,getName.pNameList->udwCnt,(*pParamInfoList)[cnt].pathDesc.fullpath,(*pParamInfoList)[cnt].writable);
        }

        getName.pfnFree(&getName);
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStopReq failed, lRet=%d in cmsPhl_getParameterNames",ret);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(getName.pucParamName);

    cmsLog_notice("leave cmsPhl_getParameterNames");

    return result;
}

CmsRet cmsPhl_setParameterValues(PhlSetParamValue_t   *pSetParamValueList,
                              SINT32               numEntries)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    UINT32 cnt = 0;
    DM_TRANSACTION_REQ_T transReq;
    DM_SET_PARA_VALUE_REQ_T setValue;
    PARAMETER_VALUE_LIST_T valueList;
    INT8 *pRootNode = NULL;

    cmsLog_notice("enter cmsPhl_setParameterValues");

    if(!checkEmeSession())   // check if Ema session is ready or not
    {
    	return CMSRET_REQUEST_DENIED;
    }

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&setValue, 0, sizeof(DM_SET_PARA_VALUE_REQ_T));
    memset(&valueList, 0, sizeof(PARAMETER_VALUE_LIST_T));

    pRootNode = CmsPhl_GetRootNode(pSetParamValueList->pathDesc.fullpath);

    strcpy(transReq.AccessEntity,"TR069");
    strcpy(transReq.RootNodeToLock,pRootNode);
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_TRUE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    CMSMEM_FREE_BUF_AND_NULL_PTR(pRootNode);

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_setParameterValues",ret);

        return ret;
    }

    setValue.TransactionId = transReq.TransactionId;
    setValue.bCreateIfNotExit = DM_FALSE;
    setValue.pvList = &valueList;
    setValue.pvList->udwCnt = numEntries;
    setValue.pvList->pstParamValue = (PARAMETER_VALUE_T*)cmsMem_alloc(sizeof(PARAMETER_VALUE_T)*numEntries,ALLOC_ZEROIZE);
    setValue.pFaultCodeList = (UINT16*)cmsMem_alloc(sizeof(UINT16)*numEntries,ALLOC_ZEROIZE);

    if (!setValue.pvList->pstParamValue || !setValue.pFaultCodeList)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"memory allocation failed for setValue.pvList->pstParamValue(0x%x) setValue.pFaultCodeList(0x%x) in cmsPhl_setParameterValues",(UINT32)setValue.pvList->pstParamValue,(UINT32)setValue.pFaultCodeList);
        ret = CMSRET_INTERNAL_ERROR;
        DmTransactionStopReq(transReq.TransactionId);
        CMSMEM_FREE_BUF_AND_NULL_PTR(setValue.pvList->pstParamValue);
        CMSMEM_FREE_BUF_AND_NULL_PTR(setValue.pFaultCodeList);
        return ret;
    }

    for (cnt=0;cnt<numEntries;cnt++)
    {
        setValue.pvList->pstParamValue[cnt].pucName = cmsMem_strdup(pSetParamValueList[cnt].pathDesc.fullpath);
        setValue.pvList->pstParamValue[cnt].pucValue = cmsMem_strdup(pSetParamValueList[cnt].pValue);
        setValue.pvList->pstParamValue[cnt].pucType = (UINT8*)cmsMem_strdup(pSetParamValueList[cnt].pParamType);
        cmsLog_notice("[%d/%d] namelist '%s'='%s'",cnt,numEntries,setValue.pvList->pstParamValue[cnt].pucName,setValue.pvList->pstParamValue[cnt].pucValue);
    }

    ret = DmSetParaValueReq(&setValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmSetParaValueReq failed lRet=%d, leave cmsPhl_setParameterValues",ret);

        result = ret;
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStopReq failed, lRet=%d in cmsPhl_setParameterValues",ret);
    }

    for (cnt=0;cnt<numEntries;cnt++)
    {
        pSetParamValueList[cnt].status = setValue.pFaultCodeList[cnt];
        cmsLog_notice("[%d/%d] result '%s' FaultCode=%d",cnt,numEntries,setValue.pvList->pstParamValue[cnt].pucName,setValue.pFaultCodeList[cnt]);

        CMSMEM_FREE_BUF_AND_NULL_PTR(setValue.pvList->pstParamValue[cnt].pucName);
        CMSMEM_FREE_BUF_AND_NULL_PTR(setValue.pvList->pstParamValue[cnt].pucValue);
        CMSMEM_FREE_BUF_AND_NULL_PTR(setValue.pvList->pstParamValue[cnt].pucType);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(setValue.pvList->pstParamValue);
    CMSMEM_FREE_BUF_AND_NULL_PTR(setValue.pFaultCodeList);

    cmsLog_notice("leave cmsPhl_setParameterValues");

    return result;
}

CmsRet cmsPhl_getParameterValues(MdmPathDescriptor    *pNamePathList,
                                 SINT32               numNameEntries,
                                 PhlGetParamValue_t   **pParamValueList,
                                 SINT32               *pNumParamValueEntries,
								 UBOOL8               isAcsAction)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    UINT32 cnt = 0;
    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_VALUE_REQ_T getValue;
    PARAMETER_NAME_LIST_T nameList;
    INT8 *pRootNode = NULL;

    cmsLog_notice("enter cmsPhl_getParameterValues");

    if(isAcsAction && !checkEmeSession())   // check if Ema session is ready or not
    {
    	return CMSRET_REQUEST_DENIED;
    }

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_VALUE_REQ_T));
    memset(&nameList, 0, sizeof(PARAMETER_NAME_LIST_T));

    pRootNode = CmsPhl_GetRootNode(pNamePathList->fullpath);

    strcpy(transReq.AccessEntity,"TR069");
    strcpy(transReq.RootNodeToLock,pRootNode);
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    CMSMEM_FREE_BUF_AND_NULL_PTR(pRootNode);

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getParameterValues",ret);

        return ret;
    }

    getValue.TransactionId = transReq.TransactionId;
    getValue.pNameList = &nameList;
    getValue.pNameList->udwCnt = numNameEntries;
    getValue.pNameList->pucParamName = (PARAMETER_NAME_T*)cmsMem_alloc(sizeof(PARAMETER_NAME_T)*numNameEntries,ALLOC_ZEROIZE);
    getValue.pvList = NULL;
    getValue.pfnFree = NULL;

    if (!getValue.pNameList->pucParamName)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"memory allocation failed for getValue.pNameList->pucParamName in cmsPhl_setParameterValues");
        ret = CMSRET_INTERNAL_ERROR;
        DmTransactionStopReq(transReq.TransactionId);
        return ret;
    }

    for (cnt=0;cnt<numNameEntries;cnt++)
    {
        getValue.pNameList->pucParamName[cnt].pucName = cmsMem_strdup(pNamePathList[cnt].fullpath);
        getValue.pNameList->pucParamName[cnt].bWritable = DM_FALSE;
        cmsLog_notice("[%d/%d] namelist '%s'",cnt,numNameEntries,getValue.pNameList->pucParamName[cnt].pucName);
    }

    ret = DmGetParaValueReq(&getValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaValueReq failed lRet=%d, leave cmsPhl_getParameterValues",ret);

        result = ret;
    }
    else
    {
        *pNumParamValueEntries = getValue.pvList->udwCnt;

        if (getValue.pvList->udwCnt>0)
        {
            *pParamValueList = (PhlGetParamValue_t*)cmsMem_alloc(sizeof(PhlGetParamValue_t)*(getValue.pvList->udwCnt),ALLOC_ZEROIZE);

            if (*pParamValueList==NULL)
            {
                for (cnt=0;cnt<numNameEntries;cnt++)
                {
                    CMSMEM_FREE_BUF_AND_NULL_PTR(getValue.pNameList->pucParamName[cnt].pucName);
                }
                CMSMEM_FREE_BUF_AND_NULL_PTR(getValue.pNameList->pucParamName);
                getValue.pfnFree(&getValue);
                DmTransactionStopReq(transReq.TransactionId);
                cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"memory allocation failed for *pParamValueList in cmsPhl_setParameterValues");
                return CMSRET_INTERNAL_ERROR;
            }
        }

        for (cnt=0;cnt<getValue.pvList->udwCnt;cnt++)
        {
            snprintf((*pParamValueList)[cnt].pathDesc.fullpath,sizeof((*pParamValueList)[cnt].pathDesc.fullpath),"%s",getValue.pvList->pstParamValue[cnt].pucName);
            if (getValue.pvList->pstParamValue[cnt].pucValue)
            {
                (*pParamValueList)[cnt].pValue = cmsMem_strdup(getValue.pvList->pstParamValue[cnt].pucValue);
            }
            else
            {
                (*pParamValueList)[cnt].pValue = cmsMem_strdup("");
            }
            if (getValue.pvList->pstParamValue[cnt].pucType)
            {
                (*pParamValueList)[cnt].pParamType = cmsMem_strdup((INT8*)getValue.pvList->pstParamValue[cnt].pucType);
            }
            else
            {
                (*pParamValueList)[cnt].pParamType = cmsMem_strdup("");
            }
            (*pParamValueList)[cnt].isTr69Password = getValue.pvList->pstParamValue[cnt].bTr69Password;

            cmsLog_notice("[%d/%d] valuelist %s=%s type=%s isTr69Password=%d",cnt,getValue.pvList->udwCnt,(*pParamValueList)[cnt].pathDesc.fullpath,(*pParamValueList)[cnt].pValue,(*pParamValueList)[cnt].pParamType,(*pParamValueList)[cnt].isTr69Password);
        }

        getValue.pfnFree(&getValue);
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStopReq failed, lRet=%d in cmsPhl_getParameterValues",ret);
    }

    for (cnt=0;cnt<numNameEntries;cnt++)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(getValue.pNameList->pucParamName[cnt].pucName);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(getValue.pNameList->pucParamName);

    cmsLog_notice("leave cmsPhl_getParameterValues");

    return result;
}

CmsRet cmsPhl_setParameterAttributes(PhlSetParamAttr_t *pSetParamAttrList,
                                  SINT32            numEntries)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    UINT32 cnt = 0;
    DM_TRANSACTION_REQ_T transReq;
    DM_SET_PARA_ATTR_REQ_T setAttr;
    INT8 *pRootNode = NULL;

    cmsLog_notice("enter cmsPhl_setParameterAttributes");

    if(!checkEmeSession())   // check if Ema session is ready or not
    {
    	return CMSRET_REQUEST_DENIED;
    }

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&setAttr, 0, sizeof(DM_SET_PARA_ATTR_REQ_T));

    pRootNode = CmsPhl_GetRootNode(pSetParamAttrList->pathDesc.fullpath);

    strcpy(transReq.AccessEntity,"TR069");
    strcpy(transReq.RootNodeToLock,pRootNode);
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_TRUE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    CMSMEM_FREE_BUF_AND_NULL_PTR(pRootNode);

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_setParameterAttributes",ret);

        return ret;
    }

    setAttr.TransactionId = transReq.TransactionId;
    setAttr.ulParamCnt = numEntries;
    setAttr.pParamAttrList = (DM_PARAM_ATTR_T*)cmsMem_alloc(sizeof(DM_PARAM_ATTR_T)*numEntries,ALLOC_ZEROIZE);

    if (!setAttr.pParamAttrList)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"memory allocation failed for setAttr.pParamAttrList in cmsPhl_setParameterAttributes");
        ret = CMSRET_INTERNAL_ERROR;
        DmTransactionStopReq(transReq.TransactionId);
        return ret;
    }

    ret = CMSRET_SUCCESS;
    for (cnt=0;cnt<numEntries;cnt++)
    {
        setAttr.pParamAttrList[cnt].pucParamName = cmsMem_strdup(pSetParamAttrList[cnt].pathDesc.fullpath);

        if (pSetParamAttrList[cnt].attributes.notificationChange)
        {
            setAttr.pParamAttrList[cnt].ulAttrCnt++;
        }

        if (pSetParamAttrList[cnt].attributes.accessBitMaskChange)
        {
            setAttr.pParamAttrList[cnt].ulAttrCnt++;
        }

        if (setAttr.pParamAttrList[cnt].ulAttrCnt>0)
        {
            INT32 iCnt = 0;

            setAttr.pParamAttrList[cnt].pucAttrList = (DM_ATTR_NODE_T*)cmsMem_alloc(sizeof(DM_ATTR_NODE_T)*(setAttr.pParamAttrList[cnt].ulAttrCnt),ALLOC_ZEROIZE);

            if (!setAttr.pParamAttrList[cnt].pucAttrList)
            {
                cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"memory allocation failed for setAttr.pParamAttrList[cnt].pucAttrList in cmsPhl_setParameterAttributes");
                ret = CMSRET_INTERNAL_ERROR;
                break;
            }

            if (pSetParamAttrList[cnt].attributes.notificationChange)
            {
                setAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrName = cmsMem_strdup("Notification");
                setAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue = ConvertIntToStr(pSetParamAttrList[cnt].attributes.notification);
                setAttr.pParamAttrList[cnt].pucAttrList[iCnt].bCType = DM_FALSE;
                setAttr.pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueCnt = 1;
                setAttr.pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueSize = strlen(setAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue)+1;
                iCnt++;
            }

            if (pSetParamAttrList[cnt].attributes.accessBitMaskChange)
            {
                setAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrName = cmsMem_strdup("AccessList");

                if (pSetParamAttrList[cnt].attributes.accessBitMask & NDA_ACCESS_SUBSCRIBER)
                {
                    setAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue = cmsMem_strdup("Subscriber");
                }
                else
                {
                    setAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue = cmsMem_strdup("");
                }

                setAttr.pParamAttrList[cnt].pucAttrList[iCnt].bCType = DM_FALSE;
                setAttr.pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueCnt = 1;
                setAttr.pParamAttrList[cnt].pucAttrList[iCnt].ulAttrValueSize = strlen(setAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue)+1;
            }
        }
    }

    ret = DmSetParaAttributeReq(&setAttr);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmSetParaAttributeReq failed lRet=%d, leave cmsPhl_setParameterAttributes",ret);

        result = ret;
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStopReq failed, lRet=%d in cmsPhl_setParameterAttributes",ret);
    }

    for (cnt=0;cnt<numEntries;cnt++)
    {
        INT32 iCnt = 0;

        CMSMEM_FREE_BUF_AND_NULL_PTR(setAttr.pParamAttrList[cnt].pucParamName);

        for (iCnt=0;iCnt<setAttr.pParamAttrList[cnt].ulAttrCnt;iCnt++)
        {
            CMSMEM_FREE_BUF_AND_NULL_PTR(setAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrName);
            CMSMEM_FREE_BUF_AND_NULL_PTR(setAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue);
        }

        CMSMEM_FREE_BUF_AND_NULL_PTR(setAttr.pParamAttrList[cnt].pucAttrList);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(setAttr.pParamAttrList);

    cmsLog_notice("leave cmsPhl_setParameterAttributes");

    return result;
}

CmsRet cmsPhl_getParameterAttributes(MdmPathDescriptor *pNamePathList,
                                     SINT32            numNameEntries,
                                     PhlGetParamAttr_t **pParamAttrList,
                                     SINT32            *pNumParamAttrEntries)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    UINT32 cnt = 0;
    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_ATTR_REQ_T getAttr;
    PARAMETER_NAME_LIST_T nameList;
    INT8 *pRootNode = NULL;

    cmsLog_notice("enter cmsPhl_getParameterAttributes");

    if(!checkEmeSession())   // check if Ema session is ready or not
    {
    	return CMSRET_REQUEST_DENIED;
    }

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getAttr, 0, sizeof(DM_GET_PARA_ATTR_REQ_T));
    memset(&nameList, 0, sizeof(PARAMETER_NAME_LIST_T));

    pRootNode = CmsPhl_GetRootNode(pNamePathList->fullpath);

    strcpy(transReq.AccessEntity,"TR069");
    strcpy(transReq.RootNodeToLock,pRootNode);/* should check the root node to lock */
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    CMSMEM_FREE_BUF_AND_NULL_PTR(pRootNode);

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getParameterAttributes",ret);

        return ret;
    }

    getAttr.TransactionId = transReq.TransactionId;
    getAttr.pNameList = &nameList;
    getAttr.pNameList->udwCnt = numNameEntries;
    getAttr.pNameList->pucParamName = (PARAMETER_NAME_T*)cmsMem_alloc(sizeof(PARAMETER_NAME_T)*numNameEntries,ALLOC_ZEROIZE);
    getAttr.bCValue = DM_FALSE;
    getAttr.ulParamCnt = 0;
    getAttr.pParamAttrList = NULL;
    getAttr.pfnFree = NULL;

    if (!getAttr.pNameList->pucParamName)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"memory allocation failed for getValue.pNameList->pucParamName in cmsPhl_getParameterAttributes");

        ret = DmTransactionStopReq(transReq.TransactionId);

        if (ret != 0)
        {
            cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStopReq failed, lRet=%d in cmsPhl_getParameterAttributes",ret);
        }

        ret = CMSRET_INTERNAL_ERROR;
        return ret;
    }

    for (cnt=0;cnt<numNameEntries;cnt++)
    {
        getAttr.pNameList->pucParamName[cnt].pucName = cmsMem_strdup(pNamePathList[cnt].fullpath);
        getAttr.pNameList->pucParamName[cnt].bWritable = DM_FALSE;
    }

    ret = DmGetParaAttributeReq(&getAttr);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaAttributeReq failed lRet=%d, leave cmsPhl_getParameterAttributes",ret);

        result = ret;
    }
    else
    {
        *pNumParamAttrEntries = getAttr.ulParamCnt;
        *pParamAttrList = (PhlGetParamAttr_t*)cmsMem_alloc(sizeof(PhlGetParamAttr_t)*getAttr.ulParamCnt,ALLOC_ZEROIZE);

        for (cnt=0;cnt<getAttr.ulParamCnt;cnt++)
        {
            snprintf((*pParamAttrList)[cnt].pathDesc.fullpath,sizeof((*pParamAttrList)[cnt].pathDesc.fullpath),"%s",getAttr.pParamAttrList[cnt].pucParamName);

            if (getAttr.pParamAttrList[cnt].ulAttrCnt>0)
            {
                INT32 iCnt=0;
                for (iCnt=0;iCnt<getAttr.pParamAttrList[cnt].ulAttrCnt;iCnt++)
                {
                    if (getAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrName
                        && strcmp(getAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrName,"Notification")==0)
                    {
                        (*pParamAttrList)[cnt].attributes.notification = atoi(getAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrValue);
                    }
                    else if (getAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrName
                        && strcmp(getAttr.pParamAttrList[cnt].pucAttrList[iCnt].pAttrName,"AccessList")==0)
                    {
                        (*pParamAttrList)[cnt].attributes.accessBitMask = NDA_ACCESS_SUBSCRIBER;
                    }

                    (*pParamAttrList)[cnt].attributes.accessBitMaskChange = 0;
                    (*pParamAttrList)[cnt].attributes.notificationChange = 0;
                    (*pParamAttrList)[cnt].attributes.valueChanged = 0;
                    (*pParamAttrList)[cnt].attributes.reserved = 0;
                }
            }
        }

        getAttr.pfnFree(&getAttr);
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStopReq failed, lRet=%d in cmsPhl_getParameterAttributes",ret);
    }

    for (cnt=0;cnt<numNameEntries;cnt++)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(getAttr.pNameList->pucParamName[cnt].pucName);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(getAttr.pNameList->pucParamName);

    cmsLog_notice("leave cmsPhl_getParameterAttributes");

    return result;

}

CmsRet cmsPhl_addObjInstance(MdmPathDescriptor *pNamePath)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    DM_TRANSACTION_REQ_T transReq;
    DM_ADD_OBJECT_REQ_T addObj;
    INT8 *pRootNode = NULL;

    cmsLog_notice("enter cmsPhl_addObjInstance");

    if(!checkEmeSession())   // check if Ema session is ready or not
    {
    	return CMSRET_REQUEST_DENIED;
    }

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&addObj, 0, sizeof(DM_ADD_OBJECT_REQ_T));

    pRootNode = CmsPhl_GetRootNode(pNamePath->fullpath);

    strcpy(transReq.AccessEntity,"TR069");
    strcpy(transReq.RootNodeToLock,pRootNode);
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_TRUE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    CMSMEM_FREE_BUF_AND_NULL_PTR(pRootNode);

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_addObjInstance",ret);

        return ret;
    }

    addObj.TransactionId = transReq.TransactionId;
    addObj.pucObjName = cmsMem_strdup(pNamePath->fullpath);
    addObj.udwInstanceNum = 0;/* return value */

    cmsLog_notice("object path : %s",addObj.pucObjName);

    ret = DmAddObjectReq(&addObj);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmAddObjectReq failed lRet=%d, leave cmsPhl_addObjInstance",ret);

        result = ret;
    }
    else
    {
        pNamePath->iidStack.instance[pNamePath->iidStack.currentDepth] = addObj.udwInstanceNum;
        pNamePath->iidStack.currentDepth++;

        cmsLog_notice("object added : %s%d.",addObj.pucObjName,addObj.udwInstanceNum);
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStopReq failed, lRet=%d in cmsPhl_addObjInstance",ret);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(addObj.pucObjName);

    cmsLog_notice("leave cmsPhl_addObjInstance");

    return result;
}

CmsRet cmsPhl_delObjInstance(MdmPathDescriptor *pNamePath)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    DM_TRANSACTION_REQ_T transReq;
    DM_DELETE_OBJECT_REQ_T delObj;
    INT8 *pRootNode = NULL;

    cmsLog_notice("enter cmsPhl_delObjInstance");

    if(!checkEmeSession())   // check if Ema session is ready or not
    {
    	return CMSRET_REQUEST_DENIED;
    }

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&delObj, 0, sizeof(DM_DELETE_OBJECT_REQ_T));

    pRootNode = CmsPhl_GetRootNode(pNamePath->fullpath);

    strcpy(transReq.AccessEntity,"TR069");
    strcpy(transReq.RootNodeToLock,pRootNode);
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_TRUE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    CMSMEM_FREE_BUF_AND_NULL_PTR(pRootNode);

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_delObjInstance",ret);

        return ret;
    }

    delObj.TransactionId = transReq.TransactionId;
    delObj.pucObjName = cmsMem_strdup(pNamePath->fullpath);

    cmsLog_notice("object path : %s",delObj.pucObjName);

    ret = DmDeleteObjectReq(&delObj);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmDeleteObjectReq failed lRet=%d, leave cmsPhl_delObjInstance",ret);

        result = ret;
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStopReq failed, lRet=%d in cmsPhl_delObjInstance",ret);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(delObj.pucObjName);

    cmsLog_notice("leave cmsPhl_delObjInstance");

    return result;
}

void cmsPhl_freeGetParamValueBuf(PhlGetParamValue_t *pBuf,
                                SINT32             numEntries)
{
    UINT32 cnt = 0;

    for (cnt=0; cnt<numEntries;cnt++)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(pBuf[cnt].pValue);
        CMSMEM_FREE_BUF_AND_NULL_PTR(pBuf[cnt].pParamType);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(pBuf);
    return;
}

void cmsPhl_freeGetParamAttrBuf(PhlGetParamAttr_t *pBuf,
                                SINT32             numEntries)
{
    CMSMEM_FREE_BUF_AND_NULL_PTR(pBuf);

    return;
}

CmsRet cmsPhl_getAllParamValueChanges(PhlGetParamValue_t **pParamValue,
                                SINT32            *numParamValueChanges)
{
    *pParamValue = tr69cNotifyParaFirst;
    *numParamValueChanges = tr69cNotifyParaCounter;

    return CMSRET_SUCCESS;
}

BOOL8 cmsPhl_CheckIfAnyParamValueChangePending(void)
{
    if (tr69cNotifyPara2Counter
        && tr69cNotifyParaFirst2
        && tr69cNotifyParaLast2
        && !tr69cNotifyParaCounter
        && !tr69cNotifyParaFirst
        && !tr69cNotifyParaLast)
    {
        tr69cNotifyParaCounter = tr69cNotifyPara2Counter;
        tr69cNotifyParaFirst = tr69cNotifyParaFirst2;
        tr69cNotifyParaLast = tr69cNotifyParaLast2;
        ActiveAlarmNeeded = ActiveAlarmNeeded2;
        PassiveAlarmNeeded = PassiveAlarmNeeded2;
        DiagnosticsComplete = DiagnosticsComplete2;

        tr69cNotifyPara2Counter = 0;
        tr69cNotifyParaFirst2 = NULL;
        tr69cNotifyParaLast2 = NULL;
        ActiveAlarmNeeded2 = 0;
        PassiveAlarmNeeded2 = 0;
        DiagnosticsComplete2 = 0;

        if (ActiveAlarmNeeded
            && acsState.enableSelfdefinedEventCode)
        {
            addInformEventToList(INFORM_EVENT_VENDOR_ALARM);
        }

        addInformEventToList(INFORM_EVENT_VALUE_CHANGE);

        return TRUE;
    }

    return FALSE;
}

void cmsPhl_clearAllParamValueChanges(void)
{
    PhlGetParamValue_t *pParamValue = tr69cNotifyParaFirst;
    PhlGetParamValue_t *pParamLast = NULL;

    while(pParamValue)
    {
        pParamLast = pParamValue;
        pParamValue = pParamValue->pNext;

        CMSMEM_FREE_BUF_AND_NULL_PTR(pParamLast->pValue);
        CMSMEM_FREE_BUF_AND_NULL_PTR(pParamLast->pParamType);
        CMSMEM_FREE_BUF_AND_NULL_PTR(pParamLast);
    }

    tr69cNotifyParaCounter = 0;
    tr69cNotifyParaFirst = NULL;
    tr69cNotifyParaLast = NULL;

    return;
}

CmsRet cmsPhl_getDeviceInfo(Dev2DeviceInfoObject **deviceInfoObj)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;

    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    DEVICE_DEVICEINFO_T *deviceInfo = NULL;

    cmsLog_notice("enter cmsPhl_getDeviceInfo");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));

    ret = DmAllocateMemReq(sizeof(DEVICE_DEVICEINFO_T), (void**)&deviceInfo);

    if (ret || !deviceInfo)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for deviceInfo");
        return ret;
    }

    memset(deviceInfo, 0, sizeof(DEVICE_DEVICEINFO_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device.DeviceInfo.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getDeviceInfo",ret);

        return ret;
    }

    getValue.TransactionId = transReq.TransactionId;
    getValue.ulParaId = DEVICE_DEVICEINFO_ID;
    getValue.pucPath = "Device.DeviceInfo.";
    getValue.ulCnt = 0;
    getValue.pValue = deviceInfo;
    getValue.ValueSize = sizeof(DEVICE_DEVICEINFO_T);
    getValue.bShmWay = DM_TRUE;

    ret = DmGetParaCValueReq(&getValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaCValueReq failed lRet=%d, leave cmsPhl_getDeviceInfo",ret);

        result = ret;
    }
    else
    {
        *deviceInfoObj = (Dev2DeviceInfoObject*)cmsMem_alloc(sizeof(Dev2DeviceInfoObject),ALLOC_ZEROIZE);

        if (*deviceInfoObj)
        {
            (*deviceInfoObj)->_oid = MDMOID_DEV2_DEVICE_INFO;
            (*deviceInfoObj)->manufacturer = cmsMem_strdup(deviceInfo->Manufacturer);
            (*deviceInfoObj)->manufacturerOUI = cmsMem_strdup(deviceInfo->ManufacturerOUI);
            (*deviceInfoObj)->productClass = cmsMem_strdup(deviceInfo->ProductClass);
            (*deviceInfoObj)->serialNumber = cmsMem_strdup(deviceInfo->SerialNumber);
        }
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getDeviceInfo",ret);
    }

    ret = DmReleaseMemReq((void**)&deviceInfo);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for deviceInfo");
    }

    cmsLog_notice("leave cmsPhl_getDeviceInfo");

    return result;
}

CmsRet cmsPhl_getTr69cCfg(Tr69cCfgObject **tr69cCfg)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;

    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    INTERNAL_TR69C_CONFIG_T *tr69cConfig = NULL;

    cmsLog_notice("enter cmsPhl_getTr69cCfg");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));

    ret = DmAllocateMemReq(sizeof(INTERNAL_TR69C_CONFIG_T), (void**)&tr69cConfig);

    if (ret || !tr69cConfig)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for tr69cConfig");
        return ret;
    }

    memset(tr69cConfig, 0, sizeof(INTERNAL_TR69C_CONFIG_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Internal.Tr69c.Config.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getTr69cCfg",ret);
        DmReleaseMemReq((void**)&tr69cConfig);
        return ret;
    }

    getValue.TransactionId = transReq.TransactionId;
    getValue.ulParaId = INTERNAL_TR69C_CONFIG_ID;
    getValue.pucPath = "Internal.Tr69c.Config.";
    getValue.ulCnt = 0;
    getValue.pValue = tr69cConfig;
    getValue.ValueSize = sizeof(INTERNAL_TR69C_CONFIG_T);
    getValue.bShmWay = DM_TRUE;

    ret = DmGetParaCValueReq(&getValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaCValueReq failed lRet=%d, leave cmsPhl_getTr69cCfg",ret);

        result = ret;
    }
    else
    {
        *tr69cCfg = (Tr69cCfgObject*)cmsMem_alloc(sizeof(Tr69cCfgObject),ALLOC_ZEROIZE);

        if (*tr69cCfg)
        {
            (*tr69cCfg)->_oid = MDMOID_TR69C_CFG;
            (*tr69cCfg)->loggingLevel = cmsMem_strdup(tr69cConfig->loggingLevel);
            (*tr69cCfg)->loggingDestination = cmsMem_strdup(tr69cConfig->loggingDestination);
#ifdef DMP_X_BROADCOM_COM_DEBUG_1
            (*tr69cCfg)->loggingSOAP = tr69cConfig->loggingSOAP;
#endif
#ifdef DMP_X_BROADCOM_COM_SECURITY_1
            (*tr69cCfg)->connectionRequestAuthentication = tr69cConfig->connectionRequestAuthentication;
#endif
            (*tr69cCfg)->enableSelfdefinedEventCode = tr69cConfig->enableSelfdefinedEventCode;

            (*tr69cCfg)->CaCertRequired = tr69cConfig->TLS.CaCertRequired;
            (*tr69cCfg)->Ciphers = cmsMem_strdup(tr69cConfig->TLS.Ciphers);
            (*tr69cCfg)->CertDir = cmsMem_strdup(tr69cConfig->TLS.CertDir);
            (*tr69cCfg)->ClientCertFile = cmsMem_strdup(tr69cConfig->TLS.ClientCertFile);
            (*tr69cCfg)->ClientPrivateKeyFile = cmsMem_strdup(tr69cConfig->TLS.ClientPrivateKeyFile);
            (*tr69cCfg)->CaCertFile = cmsMem_strdup(tr69cConfig->TLS.CaCertFile);
        }
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getTr69cCfg",ret);
    }

    ret = DmReleaseMemReq((void**)&tr69cConfig);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for tr69cConfig");
    }

    cmsLog_notice("leave cmsPhl_getTr69cCfg");

    return result;
}

CmsRet cmsPhl_getGWStateData(void **GWState)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;

    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    INTERNAL_TR69C_GWSTATEDATA_T *gwData;
    GWStateData *pTmp = NULL;
    INT32 cnt = 0;

    cmsLog_notice("enter cmsPhl_getGWStateData");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));

    ret = DmAllocateMemReq(sizeof(INTERNAL_TR69C_GWSTATEDATA_T), (void**)&gwData);

    if (ret || !gwData)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for gwData");
        return ret;
    }

    memset(gwData, 0, sizeof(INTERNAL_TR69C_GWSTATEDATA_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Internal.Tr69c.GWStateData.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getGWStateData",ret);
        DmReleaseMemReq((void**)&gwData);
        return ret;
    }

    getValue.TransactionId = transReq.TransactionId;
    getValue.ulParaId = INTERNAL_TR69C_GWSTATEDATA_ID;
    getValue.pucPath = "Internal.Tr69c.GWStateData.";
    getValue.ulCnt = 0;
    getValue.pValue = gwData;
    getValue.ValueSize = sizeof(INTERNAL_TR69C_GWSTATEDATA_T);
    getValue.bShmWay = DM_TRUE;

    ret = DmGetParaCValueReq(&getValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaCValueReq failed lRet=%d, leave cmsPhl_getGWStateData",ret);

        result = ret;
    }
    else
    {
        pTmp = (GWStateData*)cmsMem_alloc(sizeof(GWStateData),ALLOC_ZEROIZE);

        if (pTmp)
        {
            snprintf(pTmp->rebootCommandKey,sizeof(pTmp->rebootCommandKey),"%s",gwData->rebootCommandKey);
            snprintf(pTmp->downloadCommandKey,sizeof(pTmp->downloadCommandKey),"%s",gwData->downloadCommandKey);
            snprintf(pTmp->newParameterKey_OBSOLETE,sizeof(pTmp->newParameterKey_OBSOLETE),"%s",gwData->newParameterKey);
            pTmp->contactedState = gwData->contactedState;
            pTmp->dlFaultStatus = gwData->dlFaultStatus;
            pTmp->retryCount = gwData->retryCount;
            pTmp->startDLTime = gwData->startDLTime;
            pTmp->endDLTime = gwData->endDLTime;
            snprintf(pTmp->dlFaultMsg,sizeof(pTmp->dlFaultMsg),"%s",gwData->dlFaultMsg);
            pTmp->informEvCount = gwData->informEvCount;
            for(cnt=0;cnt<pTmp->informEvCount;cnt++)
            {
                pTmp->informEvList[cnt] = gwData->informEvList[cnt];
            }
            pTmp->dataModel = gwData->dataModel;
            pTmp->cwmpVersion = gwData->cwmpVersion;

            *GWState = (void*)pTmp;
         }
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getGWStateData",ret);
    }

    ret = DmReleaseMemReq((void**)&gwData);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for gwData");
    }

    cmsLog_notice("leave cmsPhl_getGWStateData");

    return result;
}

CmsRet cmsPhl_setGWStateData(void *GWState)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    INT32 cnt = 0;

    DM_TRANSACTION_REQ_T transReq;
    DM_SET_PARA_C_VALUE_REQ_T setValue;
    INTERNAL_TR69C_GWSTATEDATA_T *gwData = NULL;
    GWStateData*pTmp = (GWStateData*)GWState;

    cmsLog_notice("enter cmsPhl_setGWStateData");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&setValue, 0, sizeof(DM_SET_PARA_C_VALUE_REQ_T));

    ret = DmAllocateMemReq(sizeof(INTERNAL_TR69C_GWSTATEDATA_T), (void**)&gwData);

    if (ret || !gwData)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"dm shared memory allocation failed for gwData",ret);
        return ret;
    }

    memset(gwData, 0, sizeof(INTERNAL_TR69C_GWSTATEDATA_T));

    if (pTmp)
    {
        snprintf(gwData->rebootCommandKey,sizeof(gwData->rebootCommandKey),"%s",pTmp->rebootCommandKey);
        snprintf(gwData->downloadCommandKey,sizeof(gwData->downloadCommandKey),"%s",pTmp->downloadCommandKey);
        snprintf(gwData->newParameterKey,sizeof(gwData->newParameterKey),"%s",pTmp->newParameterKey_OBSOLETE);
        gwData->contactedState = pTmp->contactedState;
        gwData->dlFaultStatus = pTmp->dlFaultStatus;
        gwData->retryCount = pTmp->retryCount;
        gwData->startDLTime = pTmp->startDLTime;
        gwData->endDLTime = pTmp->endDLTime;
        snprintf(gwData->dlFaultMsg,sizeof(gwData->dlFaultMsg),"%s",pTmp->dlFaultMsg);
        gwData->informEvCount = pTmp->informEvCount;
        gwData->informEvListCnt = pTmp->informEvCount;
        for(cnt=0;cnt<pTmp->informEvCount;cnt++)
        {
            gwData->informEvList[cnt] = pTmp->informEvList[cnt];
        }
        gwData->dataModel = pTmp->dataModel;
        gwData->cwmpVersion = pTmp->cwmpVersion;
    }

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Internal.Tr69c.GWStateData.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_setGWStateData",ret);
        DmReleaseMemReq((void**)&gwData);
        return ret;
    }

    setValue.TransactionId = transReq.TransactionId;
    setValue.ulParaId = INTERNAL_TR69C_GWSTATEDATA_ID;
    setValue.pucPath = "Internal.Tr69c.GWStateData.";
    setValue.ulCnt = 1;
    setValue.pValue = gwData;
    setValue.ValueSize = sizeof(INTERNAL_TR69C_GWSTATEDATA_T);
    setValue.bShmWay = DM_TRUE;

    ret = DmSetParaCValueReq(&setValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmSetParaCValueReq failed lRet=%d, leave cmsPhl_setGWStateData",ret);

        result = ret;
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_setGWStateData",ret);
    }

    ret = DmReleaseMemReq((void**)&gwData);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for gwData");
    }

    cmsLog_notice("leave cmsPhl_setGWStateData");

    return result;
}

CmsRet cmsPhl_getTransferList(void **pSaveList, UINT16 *saveEntryCount)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    UINT32 cnt = 0;
    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    DownloadReqInfo *pTmp = NULL;
    INTERNAL_TR69C_TRANSFERLIST_QUEUE_T *transferQueue = NULL;

    cmsLog_notice("enter cmsPhl_getTransferList");

    ret = DmAllocateMemReq(sizeof(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T)*INTERNAL_TR69C_TRANSFERLIST_QUEUE_CNT, (void**)&transferQueue);

    if (ret || !transferQueue)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for transferQueue");
        ret = CMSRET_INTERNAL_ERROR;
        return ret;
    }

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Internal.Tr69c.TransferList.Queue.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getTransferList",ret);
        DmReleaseMemReq((void**)&transferQueue);
        return ret;
    }

    getValue.TransactionId = transReq.TransactionId;
    getValue.ulParaId = INTERNAL_TR69C_TRANSFERLIST_QUEUE_ID;
    getValue.pucPath = "Internal.Tr69c.TransferList.Queue.";
    getValue.ulCnt = 0;
    getValue.pValue = transferQueue;
    getValue.ValueSize = sizeof(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T)*INTERNAL_TR69C_TRANSFERLIST_QUEUE_CNT;
    getValue.bShmWay = DM_TRUE;

    ret = DmGetParaCValueReq(&getValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaCValueReq failed lRet=%d, leave cmsPhl_getTransferList",ret);

        result = ret;
    }
    else
    {
        *saveEntryCount = getValue.ulCnt;

        pTmp = (DownloadReqInfo*)cmsMem_alloc(sizeof(DownloadReqInfo)*getValue.ulCnt,ALLOC_ZEROIZE);

        if (pTmp)
        {
            for (cnt=0;cnt<getValue.ulCnt;cnt++)
            {
                pTmp[cnt].efileType = transferQueue[cnt].efileType;
                snprintf(pTmp[cnt].commandKey, sizeof(pTmp[cnt].commandKey), "%s", transferQueue[cnt].commandKey);
                snprintf(pTmp[cnt].url, sizeof(pTmp[cnt].url), "%s", transferQueue[cnt].url);
                snprintf(pTmp[cnt].user, sizeof(pTmp[cnt].user), "%s", transferQueue[cnt].user);
                snprintf(pTmp[cnt].pwd, sizeof(pTmp[cnt].pwd), "%s", transferQueue[cnt].pwd);
                pTmp[cnt].fileSize = transferQueue[cnt].fileSize;
                snprintf(pTmp[cnt].fileName, sizeof(pTmp[cnt].fileName), "%s", transferQueue[cnt].fileName);
                pTmp[cnt].delaySec = transferQueue[cnt].delaySec;
                pTmp[cnt].state = transferQueue[cnt].state;
                pTmp[cnt].rpcMethod = transferQueue[cnt].rpcMethod;
            }

            *pSaveList = (void*)pTmp;
        }
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getTransferList",ret);
    }

    ret = DmReleaseMemReq((void**)&transferQueue);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for transferQueue");
    }

    cmsLog_notice("leave cmsPhl_getTransferList");

    return result;
}

CmsRet cmsPhl_setTransferList(void *pSaveList, UINT16 saveEntryCount)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    UINT32 cnt = 0;
    DM_TRANSACTION_REQ_T transReq;
    DM_ADD_OBJ_BY_ID_REQ_T addObj;
    DM_DEL_OBJ_BY_ID_REQ_T delObj;
    DownloadReqInfo *pTmp = (DownloadReqInfo *)pSaveList;
    INTERNAL_TR69C_TRANSFERLIST_QUEUE_T *transferQueue = NULL;

    cmsLog_notice("enter cmsPhl_setTransferList");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&addObj, 0, sizeof(DM_ADD_OBJ_BY_ID_REQ_T));
    memset(&delObj, 0, sizeof(DM_DEL_OBJ_BY_ID_REQ_T));

    ret = DmAllocateMemReq(sizeof(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T), (void**)&transferQueue);

    if (ret || !transferQueue)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for transferQueue");
        ret = CMSRET_INTERNAL_ERROR;
        return ret;
    }

    memset(transferQueue, 0, sizeof(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Internal.Tr69c.TransferList.Queue.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_setTransferList",ret);
        DmReleaseMemReq((void**)&transferQueue);
        return ret;
    }

    delObj.TransactionId = transReq.TransactionId;
    delObj.ulObjId = INTERNAL_TR69C_TRANSFERLIST_QUEUE_ID;
    delObj.pucPath = "Internal.Tr69c.TransferList.Queue.";

    ret = DmDelObjectByIdReq(&delObj);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmDelObjectByIdReq failed lRet=%d, leave cmsPhl_setTransferList",ret);

        result = ret;
    }
    else if (saveEntryCount>0)
    {
        addObj.TransactionId = transReq.TransactionId;
        addObj.ulObjId = INTERNAL_TR69C_TRANSFERLIST_QUEUE_ID;
        addObj.pucPath = "Internal.Tr69c.TransferList.Queue.";
        addObj.pObject = &transferQueue;
        addObj.objectSize = sizeof(INTERNAL_TR69C_TRANSFERLIST_QUEUE_T);
        addObj.bShmWay = DM_TRUE;

        for (cnt=0;cnt<saveEntryCount;cnt++)
        {
            transferQueue->efileType = pTmp[cnt].efileType;
            snprintf(transferQueue->commandKey, sizeof(transferQueue->commandKey), "%s", pTmp[cnt].commandKey);
            snprintf(transferQueue->url, sizeof(transferQueue->url), "%s", pTmp[cnt].url);
            snprintf(transferQueue->user, sizeof(transferQueue->user), "%s", pTmp[cnt].user);
            snprintf(transferQueue->pwd, sizeof(transferQueue->pwd), "%s", pTmp[cnt].pwd);
            transferQueue->fileSize = pTmp[cnt].fileSize;
            snprintf(transferQueue->fileName, sizeof(transferQueue->fileName), "%s", pTmp[cnt].fileName);
            transferQueue->delaySec = pTmp[cnt].delaySec;
            transferQueue->state = pTmp[cnt].state;
            transferQueue->rpcMethod = pTmp[cnt].rpcMethod;
        }

        ret = DmAddObjectByIdReq(&addObj);

        if (ret != 0)
        {
            cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmAddObjectById failed lRet=%d, leave cmsPhl_setTransferList",ret);

            result = ret;
        }
    }
 
    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_setTransferList",ret);
    }

    ret = DmReleaseMemReq((void**)&transferQueue);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for transferQueue");
    }

    cmsLog_notice("leave cmsPhl_setTransferList");

    return result;
}


CmsRet cmsPhl_getTimeObj(Dev2TimeObject **timeObj)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;

    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    DEVICE_TIME_T *timeInfo = NULL;

    cmsLog_notice("enter cmsPhl_getTimeObj");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));

    ret = DmAllocateMemReq(sizeof(DEVICE_TIME_T), (void**)&timeInfo);

    if (ret || !timeInfo)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for timeInfo");
        ret = CMSRET_INTERNAL_ERROR;
        return ret;
    }

    memset(timeInfo, 0, sizeof(DEVICE_TIME_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device.Time.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getTimeObj",ret);
        DmReleaseMemReq((void**)&timeInfo);
        return ret;
    }

    getValue.TransactionId = transReq.TransactionId;
    getValue.ulParaId = DEVICE_TIME_ID;
    getValue.pucPath = "Device.Time.";
    getValue.ulCnt = 0;
    getValue.pValue = timeInfo;
    getValue.ValueSize = sizeof(DEVICE_TIME_T);
    getValue.bShmWay = DM_TRUE;

    ret = DmGetParaCValueReq(&getValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaCValueReq failed lRet=%d, leave cmsPhl_getTimeObj",ret);

        result = ret;
    }
    else
    {
        *timeObj = (Dev2TimeObject*)cmsMem_alloc(sizeof(Dev2TimeObject),ALLOC_ZEROIZE);

        if (*timeObj)
        {
            (*timeObj)->_oid = MDMOID_DEV2_TIME;
            (*timeObj)->enable = timeInfo->Enable;
            (*timeObj)->status = cmsMem_strdup((timeInfo->Status==DEVICE_TIME_STATUS_SYNCHRONIZED)?MDMVS_SYNCHRONIZED:MDMVS_UNSYNCHRONIZED);
        }
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getTimeObj",ret);
    }

    ret = DmReleaseMemReq((void**)&timeInfo);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for timeInfo");
    }

    cmsLog_notice("leave cmsPhl_getTimeObj");

    return result;
}

CmsRet cmsPhl_getAcsCfg(Dev2ManagementServerObject **acsCfg)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;

    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    DEVICE_MANAGEMENTSERVER_T *managementServer = NULL;

    cmsLog_notice("enter cmsPhl_getAcsCfg");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));

    ret = DmAllocateMemReq(sizeof(DEVICE_MANAGEMENTSERVER_T), (void**)&managementServer);

    if (ret || !managementServer)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for managementServer");
        ret = CMSRET_INTERNAL_ERROR;
        return ret;
    }

    memset(managementServer, 0, sizeof(DEVICE_MANAGEMENTSERVER_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device.ManagementServer.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getAcsCfg",ret);
        DmReleaseMemReq((void**)&managementServer);
        return ret;
    }

    // first ACS IP type by ACS URL
    getValue.TransactionId = transReq.TransactionId;
    getValue.ulParaId = DEVICE_MANAGEMENTSERVER_ID;
    getValue.pucPath = "Device.ManagementServer.URL";
    getValue.ulCnt = 0;
    getValue.pValue = managementServer->URL;
    getValue.ValueSize = sizeof(INT8) * DEVICE_MANAGEMENTSERVER_URL_CNT;
    getValue.bShmWay = DM_FALSE;
    ret = DmGetParaCValueReq(&getValue);
    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaCValueReq failed lRet=%d, leave cmsPhl_getAcsCfg",ret);
        result = ret;
    }
    else
    {
    	CmsUtl_checkUrlIPtype(managementServer->URL, &connReqInfo.acsIpType, connReqInfo.acsIp);

		// get all configuration
		getValue.TransactionId = transReq.TransactionId;
		getValue.ulParaId = DEVICE_MANAGEMENTSERVER_ID;
		getValue.pucPath = "Device.ManagementServer.";
		getValue.ulCnt = 0;
		getValue.pValue = managementServer;
		getValue.ValueSize = sizeof(DEVICE_MANAGEMENTSERVER_T);
		getValue.bShmWay = DM_TRUE;

		ret = DmGetParaCValueReq(&getValue);

		if (ret != 0)
		{
			cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaCValueReq failed lRet=%d, leave cmsPhl_getAcsCfg",ret);

			result = ret;
		}
		else
		{
			*acsCfg = (Dev2ManagementServerObject*)cmsMem_alloc(sizeof(Dev2ManagementServerObject),ALLOC_ZEROIZE);

			if (*acsCfg)
			{
				(*acsCfg)->_oid = MDMOID_DEV2_MANAGEMENT_SERVER;
				(*acsCfg)->URL = cmsMem_strdup(managementServer->URL);
				(*acsCfg)->lastConnectedURL = cmsMem_strdup(managementServer->lastConnectedURL);
				(*acsCfg)->diffServ = managementServer->DiffServ;
				(*acsCfg)->username = cmsMem_strdup(managementServer->Username);
				(*acsCfg)->password = cmsMem_strdup(managementServer->Password);
				(*acsCfg)->X_BROADCOM_COM_BoundIfName = cmsMem_strdup(MDMVS_LAN);/* always LAN */
				(*acsCfg)->parameterKey = cmsMem_strdup(managementServer->ParameterKey);
				(*acsCfg)->connectionRequestURL = cmsMem_strdup(managementServer->ConnectionRequestURL);
				(*acsCfg)->connectionRequestUsername = cmsMem_strdup(managementServer->ConnectionRequestUsername);
				(*acsCfg)->connectionRequestPassword = cmsMem_strdup(managementServer->ConnectionRequestPassword);
				(*acsCfg)->periodicInformEnable = managementServer->PeriodicInformEnable;
				(*acsCfg)->periodicInformInterval = managementServer->PeriodicInformInterval;
				(*acsCfg)->CWMPRetryMinimumWaitInterval = managementServer->CWMPRetryMinimumWaitInterval;
				(*acsCfg)->CWMPRetryIntervalMultiplier = managementServer->CWMPRetryIntervalMultiplier;
				(*acsCfg)->tlsVersion = managementServer->TLSVersion;
				(*acsCfg)->tlsAuthenticationMethod = cmsMem_strdup(managementServer->TLSAuthenticationMethod);
				(*acsCfg)->tlsPeerAuthenticationMethod = cmsMem_strdup(managementServer->TLSPeerAuthenticationMethod);

			}
		}
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getAcsCfg",ret);
    }

    ret = DmReleaseMemReq((void**)&managementServer);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for managementServer");
    }

    cmsLog_notice("leave cmsPhl_getAcsCfg");

    return result;
}

CmsRet cmsPhl_setAcsCfg(Dev2ManagementServerObject *acsCfg)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;

    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    DM_SET_PARA_C_VALUE_REQ_T setValue;
    DEVICE_MANAGEMENTSERVER_T *managementServer = NULL;

    cmsLog_notice("enter cmsPhl_setAcsCfg");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));
    memset(&setValue, 0, sizeof(DM_SET_PARA_C_VALUE_REQ_T));

    ret = DmAllocateMemReq(sizeof(DEVICE_MANAGEMENTSERVER_T), (void**)&managementServer);

    if (ret || !managementServer)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for managementServer-set");
        ret = CMSRET_INTERNAL_ERROR;
        return ret;
    }

    memset(managementServer, 0, sizeof(DEVICE_MANAGEMENTSERVER_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device.ManagementServer.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_setAcsCfg",ret);
        DmReleaseMemReq((void**)&managementServer);
        return ret;
    }

    getValue.TransactionId = transReq.TransactionId;
    getValue.ulParaId = DEVICE_MANAGEMENTSERVER_ID;
    getValue.pucPath = "Device.ManagementServer.";
    getValue.ulCnt = 0;
    getValue.pValue = managementServer;
    getValue.ValueSize = sizeof(DEVICE_MANAGEMENTSERVER_T);
    getValue.bShmWay = DM_TRUE;

    ret = DmGetParaCValueReq(&getValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaCValueReq failed lRet=%d, leave cmsPhl_setAcsCfg",ret);

        result = ret;
    }
    else
    {

        if (acsCfg)
        {
            if (cmsUtl_strcmp(acsCfg->lastConnectedURL,managementServer->lastConnectedURL)!=0)
            {
                if (acsCfg->lastConnectedURL)
                {
                    snprintf(managementServer->lastConnectedURL,sizeof(managementServer->lastConnectedURL),"%s",acsCfg->lastConnectedURL);
                }
                else
                {
                    memset(managementServer->lastConnectedURL,0,sizeof(managementServer->lastConnectedURL));
                }

                setValue.TransactionId = transReq.TransactionId;
                setValue.ulParaId = DEVICE_MANAGEMENTSERVER_LASTCONNECTEDURL_ID;
                setValue.pucPath = "Device.ManagementServer."DM_SELFDEFINED_PREFIX"lastConnectedURL";
                setValue.ulCnt = 1;
                setValue.pValue = managementServer->URL;
                setValue.ValueSize = sizeof(managementServer->URL);
                setValue.bShmWay = DM_FALSE;

                ret = DmSetParaCValueReq(&setValue);

                if (ret != 0)
                {
                    cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmSetParaCValueReq failed path=%s lRet=%d, leave cmsPhl_setAcsCfg",setValue.pucPath,ret);

                    result = ret;
                }
            }

            if (cmsUtl_strcmp(acsCfg->connectionRequestURL,managementServer->ConnectionRequestURL)!=0)
            {
                if (acsCfg->connectionRequestURL)
                {
                    snprintf(managementServer->ConnectionRequestURL,sizeof(managementServer->ConnectionRequestURL),"%s",acsCfg->connectionRequestURL);
                }
                else
                {
                    memset(managementServer->ConnectionRequestURL,0,sizeof(managementServer->ConnectionRequestURL));
                }

                setValue.TransactionId = transReq.TransactionId;
                setValue.ulParaId = DEVICE_MANAGEMENTSERVER_CONNECTIONREQUESTURL_ID;
                setValue.pucPath = "Device.ManagementServer.ConnectionRequestURL";
                setValue.ulCnt = 1;
                setValue.pValue = managementServer->ConnectionRequestURL;
                setValue.ValueSize = sizeof(managementServer->ConnectionRequestURL);
                setValue.bShmWay = DM_FALSE;

                ret = DmSetParaCValueReq(&setValue);

                if (ret != 0)
                {
                    cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmSetParaCValueReq failed path=%s lRet=%d, leave cmsPhl_setAcsCfg",setValue.pucPath,ret);

                    result = ret;
                }
            }

            if (cmsUtl_strcmp(acsCfg->parameterKey,managementServer->ParameterKey)!=0)
            {
                if (acsCfg->parameterKey)
                {
                    snprintf(managementServer->ParameterKey,sizeof(managementServer->ParameterKey),"%s",acsCfg->parameterKey);
                }
                else
                {
                    memset(managementServer->ParameterKey,0,sizeof(managementServer->ParameterKey));
                }

                setValue.TransactionId = transReq.TransactionId;
                setValue.ulParaId = DEVICE_MANAGEMENTSERVER_PARAMETERKEY_ID;
                setValue.pucPath = "Device.ManagementServer.ParameterKey";
                setValue.ulCnt = 1;
                setValue.pValue = managementServer->ParameterKey;
                setValue.ValueSize = sizeof(managementServer->ParameterKey);
                setValue.bShmWay = DM_FALSE;

                ret = DmSetParaCValueReq(&setValue);

                if (ret != 0)
                {
                    cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmSetParaCValueReq failed path=%s lRet=%d, leave cmsPhl_setAcsCfg",setValue.pucPath,ret);

                    result = ret;
                }
            }
        }
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_setAcsCfg",ret);
    }

    ret = DmReleaseMemReq((void**)&managementServer);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for managementServer");
    }

    cmsLog_notice("leave cmsPhl_setAcsCfg");

    return result;
}

CmsRet cmsPhl_getDUObject(DUObject **pDuObj, SINT32 *numEntries)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;
    UINT32 cnt = 0;
    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    DEVICE_SOFTWAREMODULES_DEPLOYMENTUNIT_T *duObjects = NULL;

    cmsLog_notice("enter cmsPhl_getDUObject");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));

    ret = DmAllocateMemReq(sizeof(DEVICE_SOFTWAREMODULES_DEPLOYMENTUNIT_T)*DEVICE_SOFTWAREMODULES_DEPLOYMENTUNIT_CNT, (void**)&duObjects);

    if (ret || !duObjects)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for duObjects");
        ret = CMSRET_INTERNAL_ERROR;
        return ret;
    }

    memset(duObjects,0,sizeof(DEVICE_SOFTWAREMODULES_DEPLOYMENTUNIT_T)*DEVICE_SOFTWAREMODULES_DEPLOYMENTUNIT_CNT);

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device.SoftwareModules.DeploymentUnit.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getDUObject",ret);
        DmReleaseMemReq((void**)&duObjects);
        return ret;
    }

    getValue.TransactionId = transReq.TransactionId;
    getValue.ulParaId = DEVICE_SOFTWAREMODULES_DEPLOYMENTUNIT_ID;
    getValue.pucPath = "Device.SoftwareModules.DeploymentUnit.";
    getValue.ulCnt = 0;
    getValue.pValue = duObjects;
    getValue.ValueSize = sizeof(DEVICE_SOFTWAREMODULES_DEPLOYMENTUNIT_T)*DEVICE_SOFTWAREMODULES_DEPLOYMENTUNIT_CNT;
    getValue.bShmWay = DM_TRUE;

    ret = DmGetParaCValueReq(&getValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaCValueReq failed lRet=%d, leave cmsPhl_getDUObject",ret);

        result = ret;
    }
    else
    {
        *numEntries = getValue.ulCnt;

        *pDuObj = (DUObject*)cmsMem_alloc(sizeof(DUObject)*getValue.ulCnt,ALLOC_ZEROIZE);

        if (*pDuObj)
        {
            for (cnt=0;cnt<getValue.ulCnt;cnt++)
            {
                (*pDuObj)[cnt]._oid = MDMOID_DU;
                (*pDuObj)[cnt].UUID = cmsMem_strdup(duObjects[cnt].UUID);
                (*pDuObj)[cnt].DUID = cmsMem_strdup(duObjects[cnt].DUID);
                (*pDuObj)[cnt].alias = cmsMem_strdup(duObjects[cnt].Alias);
                (*pDuObj)[cnt].name = cmsMem_strdup(duObjects[cnt].Name);
                (*pDuObj)[cnt].status = cmsMem_strdup(duObjects[cnt].Status);
                (*pDuObj)[cnt].resolved = duObjects[cnt].Resolved;
                (*pDuObj)[cnt].URL = cmsMem_strdup(duObjects[cnt].URL);
                (*pDuObj)[cnt].description = cmsMem_strdup(duObjects[cnt].Description);
                (*pDuObj)[cnt].vendor = cmsMem_strdup(duObjects[cnt].Vendor);
                (*pDuObj)[cnt].version = cmsMem_strdup(duObjects[cnt].Version);
                (*pDuObj)[cnt].executionUnitList = cmsMem_strdup(duObjects[cnt].ExecutionUnitList);
                (*pDuObj)[cnt].executionEnvref = cmsMem_strdup(duObjects[cnt].ExecutionEnvRef);
                (*pDuObj)[cnt].X_BROADCOM_COM_startTime = NULL;
                (*pDuObj)[cnt].X_BROADCOM_COM_completeTime = NULL;
            }
        }
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsPhl_getDUObject",ret);
    }

    ret = DmReleaseMemReq((void**)&duObjects);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for duObjects");
    }

    cmsLog_notice("leave cmsPhl_getDUObject");

    return result;
}

void cmsPhl_free(void **mdmObj, SINT32 numEntries)
{
    UINT32 cnt = 0;
    MdmObjectId oid = *((MdmObjectId*)(*mdmObj));

    switch(oid)
    {
        case MDMOID_DEV2_DEVICE_INFO:
        {
            Dev2DeviceInfoObject *deviceInfoObj = (Dev2DeviceInfoObject *)(*mdmObj);

            CMSMEM_FREE_BUF_AND_NULL_PTR(deviceInfoObj->manufacturer);
            CMSMEM_FREE_BUF_AND_NULL_PTR(deviceInfoObj->manufacturerOUI);
            CMSMEM_FREE_BUF_AND_NULL_PTR(deviceInfoObj->productClass);
            CMSMEM_FREE_BUF_AND_NULL_PTR(deviceInfoObj->serialNumber);
            CMSMEM_FREE_BUF_AND_NULL_PTR(deviceInfoObj);
        }
            break;
        case MDMOID_TR69C_CFG:
        {
            Tr69cCfgObject *tr69cCfg = (Tr69cCfgObject *)(*mdmObj);

            CMSMEM_FREE_BUF_AND_NULL_PTR(tr69cCfg->loggingLevel);
            CMSMEM_FREE_BUF_AND_NULL_PTR(tr69cCfg->loggingDestination);
            CMSMEM_FREE_BUF_AND_NULL_PTR(tr69cCfg);
        }
            break;
        case MDMOID_DEV2_MANAGEMENT_SERVER:
        {
            Dev2ManagementServerObject *acsCfg = (Dev2ManagementServerObject *)(*mdmObj);

            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->URL);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->lastConnectedURL);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->username);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->password);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->X_BROADCOM_COM_BoundIfName);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->parameterKey);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->connectionRequestURL);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->connectionRequestUsername);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->connectionRequestPassword);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->tlsAuthenticationMethod);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->tlsPeerAuthenticationMethod);
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg);
        }
            break;
        case MDMOID_DU:
        {
            DUObject *pDuObj = (DUObject *)(*mdmObj);

            for (cnt=0;cnt<numEntries;cnt++)
            {
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].UUID);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].DUID);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].alias);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].name);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].status);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].URL);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].description);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].vendor);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].version);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].executionUnitList);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].executionEnvref);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].X_BROADCOM_COM_startTime);
                CMSMEM_FREE_BUF_AND_NULL_PTR(pDuObj[cnt].X_BROADCOM_COM_completeTime);
            }
        }
            break;
        default:
            break;
    }

    return;
}

CmsRet SendDownloadReqToOam(void *req)
{
	CmsRet ret = CMSRET_INTERNAL_ERROR;
    DownloadReq *dlreq = (DownloadReq *)req;
	CmsMsgHeader	*pMsg	= NULL;
	EME_FILE_TRANSMISSION_T	*pMsgBody		= NULL;

    cmsLog_notice("enter SendDownloadReqToOam");
    cmsLog_notice("efileType is %d", dlreq->efileType);
    cmsLog_notice("url is %s", dlreq->url);
    cmsLog_notice("user is %s", dlreq->user);
    cmsLog_notice("pwd is %s", dlreq->pwd);
    cmsLog_notice("fileSize is %d", dlreq->fileSize);
    cmsLog_notice("fileName is %s", dlreq->fileName);
    cmsLog_notice("delaySec is %d", dlreq->delaySec);

    if(!checkEmeSession())   // check if Ema session is ready or not
    {
    	return CMSRET_REQUEST_DENIED;
    }

	pMsg = (CmsMsgHeader*)cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(EME_FILE_TRANSMISSION_T), ALLOC_ZEROIZE);

	if(NULL == pMsg)
	{
		cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"SendDownloadReqToOam: malloc failed\n");
		return ret;
	}

	/* set message header */
    pMsg->type = CMS_MSG_FILE_TRANSFER_REQ;
    pMsg->src = EID_TR69C;
    pMsg->dst = EID_SMD;
    pMsg->wordData = 0;
    pMsg->dataLength = sizeof(EME_FILE_TRANSMISSION_T);

	/* set message body */
    pMsgBody = (EME_FILE_TRANSMISSION_T*)(pMsg + 1);
    pMsgBody->filetype = cmsUtl_ConvertFromFiletype(dlreq->efileType);
    pMsgBody->optype = EME_FILE_OP_DOWNLOAD_REQ;

    if (dlreq->url)
    {
        snprintf(pMsgBody->download.url,sizeof(pMsgBody->download.url),"%s",dlreq->url);
    }

    pMsgBody->download.updateMode = eForceUpdate;

    pMsgBody->download.delaySecondsToUpdate = 0;

    pMsgBody->download.extension.delaySecondsToDownload = dlreq->delaySec;

    if (dlreq->user)
    {
        snprintf(pMsgBody->download.extension.usrname,sizeof(pMsgBody->download.extension.usrname),"%s",dlreq->user);
    }

    if (dlreq->pwd)
    {
        snprintf(pMsgBody->download.extension.passwd,sizeof(pMsgBody->download.extension.passwd),"%s",dlreq->pwd);
    }

	/* send message to OAM */
	ret = cmsMsg_send(msgHandle, pMsg);
	if (ret != CMSRET_SUCCESS)
	{
	   cmsLog_error(TR069_ALARM_SOCKET_ERROR,"send of EME_MSG_FILE_TRANS_REQ msg failed, ret=%d", ret);
	   ret = CMSRET_INTERNAL_ERROR;
	}
	else
	{
	   cmsLog_debug("EME_MSG_FILE_TRANS_REQ msg sent");
	}

    CMSMEM_FREE_BUF_AND_NULL_PTR(pMsg);

	return ret;
}

void SendUploadReqToOam(void *req)
{
	CmsRet ret;
    DownloadReq *dlreq = (DownloadReq *)req;
	CmsMsgHeader	*pMsg	= NULL;
	EME_FILE_TRANSMISSION_T	*pMsgBody		= NULL;


    cmsLog_notice("enter SendUploadReqToOam");
    cmsLog_notice("efileType is %d", dlreq->efileType);
    cmsLog_notice("url is %s", dlreq->url);
    cmsLog_notice("user is %s", dlreq->user);
    cmsLog_notice("pwd is %s", dlreq->pwd);
    cmsLog_notice("fileSize is %d", dlreq->fileSize);
    cmsLog_notice("fileName is %s", dlreq->fileName);
    cmsLog_notice("delaySec is %d", dlreq->delaySec);


	pMsg = (CmsMsgHeader*)cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(EME_FILE_TRANSMISSION_T), ALLOC_ZEROIZE);

	if(NULL == pMsg)
	{
		cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"SendUploadReqToOam: malloc failed\n");
		return;
	}

	/* set message header */
    pMsg->type = CMS_MSG_FILE_TRANSFER_REQ;
    pMsg->src = EID_TR69C;
    pMsg->dst = EID_SMD;
    pMsg->wordData = 0;
    pMsg->dataLength = sizeof(EME_FILE_TRANSMISSION_T);

	/* set message body */
    pMsgBody = (EME_FILE_TRANSMISSION_T*)(pMsg + 1);
    pMsgBody->filetype = cmsUtl_ConvertFromFiletype(dlreq->efileType);
    pMsgBody->optype = EME_FILE_OP_UPLOAD_REQ;

    snprintf(pMsgBody->upload.url,sizeof(pMsgBody->upload.url),"%s",dlreq->url);

    pMsgBody->upload.extension.delaySecondsToUpload = dlreq->delaySec;

    if (dlreq->user)
    {
        snprintf(pMsgBody->upload.extension.usrname,sizeof(pMsgBody->upload.extension.usrname),"%s",dlreq->user);
    }

    if (dlreq->pwd)
    {
        snprintf(pMsgBody->upload.extension.passwd,sizeof(pMsgBody->upload.extension.passwd),"%s",dlreq->pwd);
    }

	/* send message to OAM */
	ret = cmsMsg_send(msgHandle, pMsg);
	if (ret != CMSRET_SUCCESS)
	{
	   cmsLog_error(TR069_ALARM_SOCKET_ERROR,"send of CME_MSG_UPLOAD_REQ msg failed, ret=%d", ret);
	}
	else
	{
	   cmsLog_debug("CME_MSG_UPLOAD_REQ msg sent");
	}
    CMSMEM_FREE_BUF_AND_NULL_PTR(pMsg);

	return;
}

eFileType cmsUtl_ConvertToFiletype(E_FILE_TYPE_T fileType)
{
    eFileType ret = 0;

    switch (fileType)
    {
        case EME_FILE_TYPE_FIRMWARE:
            ret = eFirmwareUpgrade;
            break;
        case EME_FILE_TYPE_WEBCONTENT:
            ret = eWebContent;
            break;
        case EME_FILE_TYPE_VENDOR_CONFIG:
            ret = eVendorConfig;
            break;
        case EME_FILE_TYPE_VENDOR_LOG:
            ret = eVendorLog;
            break;
        case EME_FILE_TYPE_VENDOR_ALARMREPORT:
            ret = eAlarmReport;
            break;
        case EME_FILE_TYPE_VENDOR_PMREPORT:
            ret = ePerformanceReport;
            break;
        case EME_FILE_TYPE_VENDOR_TRACES:
            ret = eVendorTraces;
            break;
        case EME_FILE_TYPE_VENDOR_CERTREQ:
            ret = eVendorCertReq;
            break;
        case EME_FILE_TYPE_VENDOR_CERTIFICATE:
            ret = eVendorClCert;
            break;
        case EME_FILE_TYPE_VENDOR_CA_CERTIFICATE:
            ret = eVendorCaCert;
            break;
        case EME_FILE_TYPE_VENDOR_DEFAULTVALUE:
            ret = eVendorDbDefaultFile;
            break;
        case EME_FILE_TYPE_VENDOR_COREFILE:
            ret = eVendorCorefiles;
            break;
        case EME_FILE_TYPE_VENDOR_DBRAWFILE:
            ret = eVendorDbRawFiles;
            break;
        case EME_FILE_TYPE_VENDOR_INNER_DBRAWFILE:
            ret = eVendorDbRawInner;
            break;
        case EME_FILE_TYPE_VENDOR_INNER_DEFAULTVALUE:
            ret = eVendorDbDefaultInner;
            break;
        case EME_FILE_TYPE_VENDOR_TCPDUMP_FILE:
            ret = eVendorTcpdumpFiles;
            break;
        case EME_FILE_TYPE_VENDOR_KERNELLOG_FILE:
            ret = eVendorKernelLog;
            break;
        case EME_FILE_TYPE_VENDOR_FIRMWARE:
            ret = eVendorFirmware;
            break;
        case EME_FILE_TYPE_VENDOR_DB_CNDATA:
            ret = eVendorDbCnData;
            break;
        case EME_FILE_TYPE_VENDOR_DB_CNDATA_DEFAULT:
            ret = eVendorDbCnDataDefault;
            break;
        case EME_FILE_TYPE_VENDOR_TRACE_MDT_FILE:
            ret = eVendorTraceMdtFile;
            break;
        default:
            ret = 0;
            break;
    }

    return ret;
}

E_FILE_TYPE_T cmsUtl_ConvertFromFiletype(eFileType fileType)
{
    E_FILE_TYPE_T ret = 0;

    switch (fileType)
    {
        case eFirmwareUpgrade:
            ret = EME_FILE_TYPE_FIRMWARE;
            break;
        case eWebContent:
            ret = EME_FILE_TYPE_WEBCONTENT;
            break;
        case eVendorConfig:
            ret = EME_FILE_TYPE_VENDOR_CONFIG;
            break;
        case eVendorLog:
            ret = EME_FILE_TYPE_VENDOR_LOG;
            break;
        case eAlarmReport:
            ret = EME_FILE_TYPE_VENDOR_ALARMREPORT;
            break;
        case ePerformanceReport:
            ret = EME_FILE_TYPE_VENDOR_PMREPORT;
            break;
        case eVendorTraces:
            ret = EME_FILE_TYPE_VENDOR_TRACES;
            break;
        case eVendorCertReq:
            ret = EME_FILE_TYPE_VENDOR_CERTREQ;
            break;
        case eVendorClCert:
            ret = EME_FILE_TYPE_VENDOR_CERTIFICATE;
            break;
        case eVendorCaCert:
            ret = EME_FILE_TYPE_VENDOR_CA_CERTIFICATE;
            break;
        case eVendorDbDefaultFile:
            ret = EME_FILE_TYPE_VENDOR_DEFAULTVALUE;
            break;
        case eVendorCorefiles:
            ret = EME_FILE_TYPE_VENDOR_COREFILE;
            break;
        case eVendorDbRawFiles:
            ret = EME_FILE_TYPE_VENDOR_DBRAWFILE;
            break;
        case eVendorDbRawInner:
            ret = EME_FILE_TYPE_VENDOR_INNER_DBRAWFILE;
            break;
        case eVendorDbDefaultInner:
            ret = EME_FILE_TYPE_VENDOR_INNER_DEFAULTVALUE;
            break;
        case eVendorTcpdumpFiles:
            ret = EME_FILE_TYPE_VENDOR_TCPDUMP_FILE;
            break;
        case eVendorKernelLog:
            ret = EME_FILE_TYPE_VENDOR_KERNELLOG_FILE;
            break;
        case eVendorFirmware:
            ret = EME_FILE_TYPE_VENDOR_FIRMWARE;
            break;
        case eVendorDeviceParams:
            ret = EME_FILE_TYPE_VENDOR_DEVICE_PARAMS;
            break;
        case eVendorDbCnData:
            ret = EME_FILE_TYPE_VENDOR_DB_CNDATA;
            break;
        case eVendorDbCnDataDefault:
            ret = EME_FILE_TYPE_VENDOR_DB_CNDATA_DEFAULT;
            break;
        case eVendorTraceMdtFile:
            ret = EME_FILE_TYPE_VENDOR_TRACE_MDT_FILE;
            break;
        default:
            ret = EME_FILE_TYPE_NULL;
            break;
    }

    return ret;
}

eFileType cmsUtl_getVendorFiletype(const char *filetype_str)
{
    const char *pFiletype = NULL;
    char vendorFiletype_str[128] = {0};
    eFileType filetype = 0;

    snprintf(vendorFiletype_str,sizeof(vendorFiletype_str),"X %s ", acsState.manufacturerOUI);

    if (cmsUtl_strncmp(vendorFiletype_str,filetype_str,strlen(vendorFiletype_str))==0)
    {
        pFiletype = (const char*)filetype_str + strlen(vendorFiletype_str);
        if (cmsUtl_strcmp(pFiletype,"AlarmReport")==0)
        {
            filetype = eAlarmReport;
        }
        else if (cmsUtl_strcmp(pFiletype,"PerformaceReport")==0)
        {
            filetype = ePerformanceReport;
        }
        else if (cmsUtl_strcmp(pFiletype,"VendorTraces")==0)
        {
            filetype = eVendorTraces;
        }
        else if (cmsUtl_strcmp(pFiletype,"CertReq")==0)
        {
            filetype = eVendorCertReq;
        }
        else if (cmsUtl_strcmp(pFiletype,"ClCert")==0)
        {
            filetype = eVendorClCert;
        }
        else if (cmsUtl_strcmp(pFiletype,"CaCert")==0)
        {
            filetype = eVendorCaCert;
        }
        else if (cmsUtl_strcmp(pFiletype,"DbDefaultFile")==0)
        {
            filetype = eVendorDbDefaultFile;
        }
        else if (cmsUtl_strcmp(pFiletype,"CoreFile")==0)
        {
            filetype = eVendorCorefiles;
        }
        else if (cmsUtl_strcmp(pFiletype,"DbRawFile")==0)
        {
            filetype = eVendorDbRawFiles;
        }
        else if (cmsUtl_strcmp(pFiletype,"DbRawInner")==0)
        {
            filetype = eVendorDbRawInner;
        }
        else if (cmsUtl_strcmp(pFiletype,"DbDefaultInner")==0)
        {
            filetype = eVendorDbDefaultInner;
        }
        else if (cmsUtl_strcmp(pFiletype,"TcpdumpFiles")==0)
        {
            filetype = eVendorTcpdumpFiles;
        }
        else if (cmsUtl_strcmp(pFiletype,"KernelLog")==0)
        {
            filetype = eVendorKernelLog;
        }
        else if (cmsUtl_strcmp(pFiletype,"VendorFirmware")==0)
        {
            filetype = eVendorFirmware;
        }
        else if (cmsUtl_strcmp(pFiletype,"DeviceParams")==0)
        {
            filetype = eVendorDeviceParams;
        }
        else if (cmsUtl_strcmp(pFiletype,"DbCnData")==0)
        {
            filetype = eVendorDbCnData;
        }
        else if (cmsUtl_strcmp(pFiletype,"DbCnDataDefault")==0)
        {
            filetype = eVendorDbCnDataDefault;
        }

    }

    return filetype;
}

const char *cmsUtl_getFiletypeString(eFileType filetype)
{
    char filetype_str[128] = {0};
    static const char *pRet = NULL;

    switch(filetype)
    {
        case eFirmwareUpgrade:
            snprintf((char*)filetype_str, sizeof(filetype_str), "1 Firmware Upgrade Image");
            break;
        case eWebContent:
            snprintf((char*)filetype_str, sizeof(filetype_str), "2 Web Content");
            break;
        case eVendorConfig:
            snprintf((char*)filetype_str, sizeof(filetype_str), "3 Vendor Configuration File");
            break;
        case eVendorLog:
            snprintf((char*)filetype_str, sizeof(filetype_str), "4 Vendor Log File");
            break;
        case eAlarmReport:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s AlarmReport", acsState.manufacturerOUI);
            break;
        case ePerformanceReport:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s PerformaceReport", acsState.manufacturerOUI);
            break;
        case eVendorTraces:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s VendorTraces", acsState.manufacturerOUI);
            break;
        case eVendorCertReq:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s CertReq", acsState.manufacturerOUI);
            break;
        case eVendorClCert:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s ClCert", acsState.manufacturerOUI);
            break;
        case eVendorCaCert:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s CaCert", acsState.manufacturerOUI);
            break;
        case eVendorDbDefaultFile:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s DbDefaultFile", acsState.manufacturerOUI);
            break;
        case eVendorCorefiles:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s CoreFiles", acsState.manufacturerOUI);
            break;
        case eVendorDbRawFiles:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s DbRawFile", acsState.manufacturerOUI);
            break;
        case eVendorDbRawInner:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s DbRawInner", acsState.manufacturerOUI);
            break;
        case eVendorDbDefaultInner:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s DbDefaultInner", acsState.manufacturerOUI);
            break;
        case eVendorTcpdumpFiles:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s TcpdumpFiles", acsState.manufacturerOUI);
            break;
        case eVendorKernelLog:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s KernelLog", acsState.manufacturerOUI);
            break;
        case eVendorFirmware:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s VendorFirmware", acsState.manufacturerOUI);
            break;
        case eVendorDeviceParams:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s DeviceParams", acsState.manufacturerOUI);
            break;
        case eVendorDbCnData:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s DbCnData", acsState.manufacturerOUI);
            break;
        case eVendorDbCnDataDefault:
            snprintf((char*)filetype_str, sizeof(filetype_str), "X %s DbCnDataDefault", acsState.manufacturerOUI);
            break;
        default:
            memset((char*)filetype_str,0,sizeof(filetype_str));
            break;
    }

    if (pRet)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(pRet);
    }
    pRet = cmsMem_strdup(filetype_str);

    return pRet;
}

void cmsUtil_sendRequestRebootMsg(void *handle, UBOOL8 bReset)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret;

   msg.type = CMS_MSG_REBOOT_SYSTEM;
   msg.src = cmsMsg_getHandleEid(handle);
   msg.dst = EID_SMD;
   msg.flags_request = 1;
   msg.wordData = bReset;/* factory reset */

   ret = cmsMsg_send(handle, &msg);
   
   if (ret != CMSRET_SUCCESS)
   {
	  cmsLog_error(TR069_ALARM_SOCKET_ERROR,"send of CMS_MSG_REBOOT_SYSTEM msg failed, ret=%d", ret);
   }
   else
   {
	  cmsLog_debug("CMS_MSG_REBOOT_SYSTEM msg sent");
   }
}

CmsRet devCtl_getBaseMacAddress(UINT8 *macAddrNum)
{
    CmsRet ret = CMSRET_INTERNAL_ERROR;
    int                 sockfd;
    struct ifreq        ifr;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        cmsLog_error(TR069_ALARM_SOCKET_ERROR,"socket error");
        return ret;
    }
    
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);      //Interface name
    
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == 0) {  //SIOCGIFHWADDR hardware address
        memcpy(macAddrNum, ifr.ifr_hwaddr.sa_data, 6);
        cmsLog_notice("mac: %02x:%02x:%02x:%02x:%02x:%02x", macAddrNum[0], macAddrNum[1], macAddrNum[2], macAddrNum[3], macAddrNum[4], macAddrNum[5]);
        ret = CMSRET_SUCCESS;
    }

    close(sockfd);
    return ret;
}

CmsRet qdmModsw_getExecEnvFullPathByNameLocked(const char *name,
                                      char *fullPathBuf, UINT32 bufLen)
{
    CmsRet ret = CMSRET_SUCCESS;

    /* only one instance, and number should always be 1 */
    snprintf(fullPathBuf,bufLen,"Device.SoftwareModules.ExecEnv.1.");

    return ret;
}

INT32 Tr69cGetConnReqUrlReq(DM_GET_VALUE_T * pGetValue)
{
    INT32 lRet = DM_FAILURE;
    INT8 localIPAddr[INET6_ADDRSTRLEN]={0};
    INT8 accessIPAddr[INET6_ADDRSTRLEN]={0};

    switch(pGetValue->ulParaId)
	{
        case DEVICE_MANAGEMENTSERVER_CONNECTIONREQUESTURL_ID:
        {
            if (connReqInfo.acsFd>0)
            {
                /* first we should use getsockname() to fetch bind-ip */
                struct sockaddr_storage local_addr;
                struct sockaddr *paddr = (struct sockaddr *)&local_addr;
                socklen_t addr_size = sizeof(local_addr);

                lRet = getsockname(connReqInfo.acsFd,(struct sockaddr *)&local_addr,&addr_size);

                if(lRet<0)  
                {  
                    cmsLog_notice("getsockname() failed, ret=%d errno=%d %s in Tr69cGetConnReqUrlReq",lRet,errno,strerror(errno));  
                }
                else
                {
                    switch (paddr->sa_family)
                    {
                        case AF_INET:
                            inet_ntop(paddr->sa_family, &((struct sockaddr_in *) paddr)->sin_addr, accessIPAddr, sizeof(accessIPAddr));
                            cmsLog_notice("get ipv4 bind-ip %s from getsockname()",accessIPAddr);
                            break;
                        case AF_INET6:
                        {
                            INT8 tmpIPAddrBuf[INET6_ADDRSTRLEN]={0};

                            inet_ntop(paddr->sa_family, &((struct sockaddr_in6 *) paddr)->sin6_addr, tmpIPAddrBuf, sizeof(tmpIPAddrBuf));
                            cmsLog_notice("get ipv6 bind-ip %s from getsockname()", tmpIPAddrBuf);
                            snprintf(accessIPAddr, sizeof(accessIPAddr), "[%s]", tmpIPAddrBuf);
                            break;
                        }
                        default:
                            cmsLog_notice("invalid sa_family=%d",paddr->sa_family);
                            break;
                    }
                }
            }

            /* if failed, we use local ip instead */
            if (strlen(accessIPAddr)<=0)
            {
            	/* try ipv6 first, just same as startACScallback function */
                if (connReqInfo.acsIpType == 6 && cmsPhl_lookupPeerLocalIp(6, &connReqInfo.localIPTable, connReqInfo.acsIp, localIPAddr))
                {
                    cmsLog_notice("no virtual ip found, use local ipv6 %s", localIPAddr);
                    snprintf(accessIPAddr, sizeof(accessIPAddr), "[%s]", localIPAddr);
                }
                else if (cmsPhl_lookupPeerLocalIp(4, &connReqInfo.localIPTable, connReqInfo.acsIp, localIPAddr))
                {
                    cmsLog_notice("no virtual ip found, use local ipv4 %s", localIPAddr);
                    snprintf(accessIPAddr, sizeof(accessIPAddr), "%s", localIPAddr);
                }
                else
                {
                    cmsLog_notice("no virtual ip found, and no local ip found");
                }
            }

            sprintf(pGetValue->pValue, "http://%s:30005/", accessIPAddr);

            cmsLog_notice("connectionRequestUrl is %s",pGetValue->pValue);
            lRet = DM_SUCCESS;
        }
            break;
        default:
            break;
    }

    return lRet;
}

void sendEmeBootComplete(void)
{
    CmsMsgHeader *msg=NULL;
    INT32 msglen = sizeof(CmsMsgHeader);
	CmsRet ret;

	msg = (CmsMsgHeader *)cmsMem_alloc(msglen, ALLOC_ZEROIZE);

    if(msg==NULL)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"malloc() failed for msg");
        return;
    }

    msg->type = CMS_MSG_REGISTERED_IND;
    msg->src = EID_TR69C;
    msg->dst = EID_SMD;
    msg->wordData = 0;    //reserved
    msg->dataLength =0;

	ret = cmsMsg_send(msgHandle, msg);
	if (ret != CMSRET_SUCCESS)
	{
	   cmsLog_error(TR069_ALARM_SOCKET_ERROR,"send of CMS_MSG_REGISTERED_IND msg failed, ret=%d", ret);
	}
	else
	{
	   cmsLog_debug("CMS_MSG_REGISTERED_IND msg sent");
	}
	CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
	return;
}

void updateAcsSocket(int fd)
{
    connReqInfo.acsFd = fd;
    return;
}

const char *getInformVendorEvtString(UINT32 evt)
{
    char evtStrBuf[128] = {0};
    const char *evtString = NULL;

    switch(evt)
    {
        case INFORM_EVENT_VENDOR_ALARM:
            snprintf(evtStrBuf,sizeof(evtStrBuf),"X %s Alarm",acsState.manufacturerOUI);
            evtString = cmsMem_strdup(evtStrBuf);
            break;
        default:
            break;
    }

    return evtString;
}
#ifdef OAM_SUPPORT
CmsRet cmsUtl_get_certifcate_file(char *certificate_path, char *certificate_file, int certificate_file_length, char *certificate_passwd, int certificate_passwd_length)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsRet result = CMSRET_SUCCESS;

    DM_TRANSACTION_REQ_T transReq;
    DM_GET_PARA_C_VALUE_REQ_T getValue;
    DEVICE_SECURITY_CERTIFICATE_T *theCertificate = NULL;

    cmsLog_notice("enter cmsUtl_get_certifcate_file");

    memset(&transReq, 0, sizeof(DM_TRANSACTION_REQ_T));
    memset(&getValue, 0, sizeof(DM_GET_PARA_C_VALUE_REQ_T));

    ret = DmAllocateMemReq(sizeof(DEVICE_SECURITY_CERTIFICATE_T), (void**)&theCertificate);

    if (ret || !theCertificate)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"dm shared memory allocation failed for theCertificate");
        ret = CMSRET_INTERNAL_ERROR;
        return ret;
    }

    memset(theCertificate, 0, sizeof(DEVICE_SECURITY_CERTIFICATE_T));

    strcpy(transReq.AccessEntity,"ROOT");
    strcpy(transReq.RootNodeToLock,"Device.Security.Certificate.");
    transReq.TransactionId = 0;
    transReq.DataFlush = DM_FALSE;
    transReq.bNotCallNotify = DM_FALSE;
    transReq.timeout = 0;

    ret = DmTransactionStartReq(&transReq);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsUtl_get_certifcate_file",ret);
        DmReleaseMemReq((void**)&theCertificate);
        return ret;
    }

    getValue.TransactionId = transReq.TransactionId;
    getValue.ulParaId = DEVICE_SECURITY_CERTIFICATE_ID;
    getValue.pucPath = certificate_path;
    getValue.ulCnt = 0;
    getValue.pValue = theCertificate;
    getValue.ValueSize = sizeof(DEVICE_SECURITY_CERTIFICATE_T);
    getValue.bShmWay = DM_TRUE;

    ret = DmGetParaCValueReq(&getValue);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmGetParaCValueReq failed lRet=%d, leave cmsUtl_get_certifcate_file",ret);
        result = ret;
    }
    else
    {
		if(strncmp(theCertificate->Name, CERT_DOWNLOAD_CERT_PREFIX, strlen(CERT_DOWNLOAD_CERT_PREFIX)) == 0)
		{
			snprintf(certificate_file,certificate_file_length,"%s%s",CERT_DOWNLOADED_LOCATION_DIR,theCertificate->Name);
		}
		else
		{
			snprintf(certificate_file,certificate_file_length,"%s%s",CERT_PRESET_LOCATION_DIR,theCertificate->Name);
		}
        snprintf(certificate_passwd,certificate_passwd_length,"%s",theCertificate->Password);
        cmsLog_notice("find %s (pw=%s), in cmsUtl_get_certifcate_file",certificate_file,certificate_passwd);
    }

    ret = DmTransactionStopReq(transReq.TransactionId);

    if (ret != 0)
    {
        cmsLog_error(TR069_ALARM_DB_ACCESS_ERROR,"DmTransactionStartReq failed lRet=%d, leave cmsUtl_get_certifcate_file",ret);
    }

    ret = DmReleaseMemReq((void**)&theCertificate);

    if (ret)
    {
        cmsLog_error(TR069_ALARM_MEMORY_ALLOC_FAILED,"failed to release dm shared memory for theCertificate");
    }

    cmsLog_notice("leave cmsUtl_get_certifcate_file");

    return result;

}

#endif

void deploy_ssl_certificates(char *prvkeyPath, char *clcertPath, char *caPath)
{
#if 0
    CmsRet ret = CMSRET_SUCCESS;
    char cert_file[128] = {0};
    char cert_passwd[128] = {0};
	char decrypt_cert_file[128] = {0};
    struct stat filestat;

    /* client cert */
    if (acsState.tlsAuthenticationMethod
        && strlen(acsState.tlsAuthenticationMethod)>0
        && strncmp(acsState.tlsAuthenticationMethod,"Device.Security.Certificate.",strlen("Device.Security.Certificate."))==0)
    {
        memset(cert_file,0,sizeof(cert_file));
        memset(cert_passwd,0,sizeof(cert_passwd));
		memset(decrypt_cert_file,0,sizeof(decrypt_cert_file));
        ret = cmsUtl_get_certifcate_file(acsState.tlsAuthenticationMethod,cert_file,sizeof(cert_file),cert_passwd,sizeof(cert_passwd));
        if (ret == CMSRET_SUCCESS)
        {
            if (access(cert_file,F_OK)==0)
            {
                snprintf(decrypt_cert_file,sizeof(decrypt_cert_file),"/tmp/tlsCert.p12");

                if (devs_file_security_copy(cert_file,decrypt_cert_file, DEVS_FILE_SEC_COPY_decrypt)!=CMSRET_SUCCESS
                    || access(decrypt_cert_file,F_OK)!=0)
                {
                    cmsLog_notice("decrypted-certfile(%s) not exist, certfile(%s)",decrypt_cert_file,cert_file);
                }
                else
                {
                    /* extract clcert from p12 file */
                    ret = libopenssl_pkcs12_output_clcerts_with_name(decrypt_cert_file,cert_passwd,clcertPath);

                    if (ret != 0)
                    {
                        cmsLog_notice("extract client certificate %s failed, in deploy_ssl_certificates",clcertPath);
                    }
                    else if (access(clcertPath,F_OK)!=0)
                    {
                        cmsLog_notice("extract client certificate %s failed, file not exists, in deploy_ssl_certificates",clcertPath);
                    }
                    else if(stat(clcertPath, &filestat)!=0 || filestat.st_size<=(off_t)0)/* cert file exists, but size is 0 */
                    {
						cmsLog_notice("extract client certificate %s failed, file is empty, in deploy_ssl_certificates",clcertPath);
                    	if (remove(clcertPath) != 0)
						{
							cmsLog_notice("extract client certificate %s failed, failed to remove empty file in deploy_ssl_certificates",clcertPath);
						}
					}
                    else
                    {
                        cmsLog_notice("extract %s from %s successfully, in deploy_ssl_certificates",clcertPath,cert_file);
                    }
                    
                    /* extract privatekey from p12 file */
                    ret = libopenssl_pkcs12_output_keys_with_name(decrypt_cert_file, cert_passwd, prvkeyPath, NULL);
                    
                    if (ret != 0)
                    {
                        cmsLog_notice("extract private key failed, in deploy_ssl_certificates",prvkeyPath);
                    }
                    else if (access(prvkeyPath,F_OK)!=0)
                    {
                        cmsLog_notice("extract private key failed, file not exists, in deploy_ssl_certificates",prvkeyPath);
                    }
                    else if(stat(prvkeyPath, &filestat)!=0 || filestat.st_size<=(off_t)0)/* key file exists, but size is 0 */
                    {
					    cmsLog_notice("extract private key  %s failed, file is empty, in deploy_ssl_certificates",prvkeyPath);
                        if (remove(prvkeyPath) != 0)
					    {
						    cmsLog_notice("extract private key %s failed, failed to remove empty file in deploy_ssl_certificates", prvkeyPath);
					    }
                    }
                    else
                    {
                        cmsLog_notice("extract %s from %s successfully, in deploy_ssl_certificates",prvkeyPath,cert_file);
					}
					
					remove(decrypt_cert_file);  /* remove tmp file */
                }
            }
            else
            {
                cmsLog_notice("client certificate %s not exists, skip it, in deploy_ssl_certificates",cert_file);
            }
        }
        else
        {
            cmsLog_notice("client certificate instance %s not exists, skip it, in deploy_ssl_certificates",acsState.tlsAuthenticationMethod);
        }
    }
    else
    {
        cmsLog_notice("no client certificate configured, in deploy_ssl_certificates");
    }

    /* ca cert */
    if (acsState.tlsPeerAuthenticationMethod
        && strlen(acsState.tlsPeerAuthenticationMethod)>0
        && strncmp(acsState.tlsPeerAuthenticationMethod,"Device.Security.Certificate.",strlen("Device.Security.Certificate."))==0)
    {
        memset(cert_file,0,sizeof(cert_file));
        memset(cert_passwd,0,sizeof(cert_passwd));
        ret = cmsUtl_get_certifcate_file(acsState.tlsPeerAuthenticationMethod,cert_file,sizeof(cert_file),cert_passwd,sizeof(cert_passwd));
        if (ret == CMSRET_SUCCESS)
        {
            if (access(cert_file,F_OK)==0)
            {
                ret = devs_file_security_copy(cert_file, caPath, DEVS_FILE_SEC_COPY_decrypt);
				if (ret != CMSRET_SUCCESS || access(caPath, F_OK) != 0)
                {
                    cmsLog_notice("decrypted CA certfile(%s) not exist, cert_file(%s)",caPath,cert_file);
                }
                else
                {
                    cmsLog_notice("copy %s to %s done in deploy_ssl_certificates",cert_file,caPath);
                }
            }
            else
            {
                cmsLog_notice("ca certificate %s not exists, skip it, in deploy_ssl_certificates",cert_file);
            }
        }
        else
        {
            cmsLog_notice("ca certificate instance %s not exists, skip it, in deploy_ssl_certificates",acsState.tlsPeerAuthenticationMethod);
        }
    }
    else
    {
        cmsLog_notice("no ca certificate configured, in deploy_ssl_certificates");
    }
#endif
    return;
}

/** dns lookup that uses getaddrinfo
 */
int dns_lookup(const char *name, struct sockaddr_storage *addr)
{
	struct addrinfo hints, *res, *res0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (osa_getaddrinfo(name, NULL, &hints, &res0, TR069_DNS_RESOLV_TIMEOUT_MS) != 0)
	{
		cmsLog_notice("dns_lookup(%s) failed", name);
	}
	else
	{
		for (res=res0;res;res=res->ai_next)
		{
			cmsLog_debug("AF<%d> addr<%s>", res->ai_family, writeIp(res->ai_addr));
		}

		/* pick the first available address */
		if (res0->ai_family == AF_INET)
		{
			memcpy(addr, res0->ai_addr, sizeof(struct sockaddr_in));
		}
		else
		{
			memcpy(addr, res0->ai_addr, sizeof(struct sockaddr_in6));
		}

		freeaddrinfo(res0);
	}

	return 1;
}

CmsRet CmsUtl_checkUrlIPtype(INT8* Url, INT32* ipType, INT8* ipAddress)
{
	CmsRet ret = CMSRET_INTERNAL_ERROR;

	UrlProto proto;
	char *tmpHost, *tmpPath;
	UINT16 port;

	struct sockaddr_storage  host_addr;

	*ipType = 0;
	ipAddress[0] = '\0';

	if(Url != NULL && strlen(Url) > 0)
	{
	   if(cmsUtl_parseUrl(Url, &proto, &tmpHost, &port, &tmpPath) == CMSRET_SUCCESS)
		{
			if (dns_lookup(tmpHost, &host_addr))
			{
				*ipType = (host_addr.ss_family == AF_INET6) ? 6 :4;
				if(*ipType == 6)
				{
					inet_ntop(AF_INET6, &((struct sockaddr_in6* )&host_addr)->sin6_addr, ipAddress, INET6_ADDRSTRLEN);
				}
				else
				{
					inet_ntop(AF_INET, &((struct sockaddr_in* )&host_addr)->sin_addr, ipAddress, INET_ADDRSTRLEN);
				}
				ret = CMSRET_SUCCESS;
			}
		}
	}
	else
	{
		ret = CMSRET_SUCCESS;
	}

	CMSMEM_FREE_BUF_AND_NULL_PTR(tmpHost);
	CMSMEM_FREE_BUF_AND_NULL_PTR(tmpPath);

	return ret;
}


//wwzh
INT32 get_ipv4_IPAddress(DM_GET_VALUE_T * pGetValue)
{
	fprintf(stderr, "PPP:::GGG id == %d path = %s\n", pGetValue->ulParaId, pGetValue->pucPath);
	return DM_SUCCESS;
}

INT32 set_ipv4_IPAddress(DM_SET_VALUE_T * pSetValue)
{
	int iii;
	fprintf(stderr, "PPP:::SSS id == %d path = %s value=%s\n", 
		pSetValue->ulParaId, pSetValue->pucPath, pSetValue->pValue);
	fprintf(stderr, "PPP value = ");
	for (iii = 0; iii < pSetValue->ValueSize; iii++)
	{
		fprintf(stderr, "%02x ", *((char *)pSetValue->pValue + iii));
	}
	fprintf(stderr, "\n\n");
	return DM_SUCCESS;
}



