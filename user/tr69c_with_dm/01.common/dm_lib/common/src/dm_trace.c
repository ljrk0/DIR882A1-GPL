
#include "dm_internal.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

#define DM_MAX_LOG_LEN 1024
static DM_TRACE_HOOKER_FUN pDmHooker = NULL;

void DmTraceHookerRegister(DM_TRACE_HOOKER_FUN pHooker)
{
	/* If multi-call, only first is valid */
	if (NULL == pDmHooker)
	{
		pDmHooker = pHooker;
	}
    return;
}

void DmTrace(const INT8 *file, INT32 line, UINT16 usLevel, UINT32 errType, const char *string,...)
{
	INT8 pMsg[DM_MAX_LOG_LEN] = {0};
	va_list pArg;

    memset(pMsg, 0, DM_MAX_LOG_LEN);

	va_start(pArg,string);
    vsnprintf(pMsg, DM_MAX_LOG_LEN, string, pArg);
	va_end(pArg);

    if (pDmHooker && usLevel > DM_TRACE_LEVEL_MIDDLE)
    {
        pDmHooker(file, line, usLevel, errType, pMsg);
    }

	return;
}


