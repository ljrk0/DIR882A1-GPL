/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:         osa_debug.c
  Description: the operations for debugging osa.
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

OSA_DBG_FUN g_OsaDbgCb = NULL;

void osa_debug_callback(OSA_DBG_FUN osaDbgCb)
{
	/* If multi-call, only first is valid */
	if (NULL == g_OsaDbgCb)
	{
		g_OsaDbgCb = osaDbgCb;
	}
}

void osa_debug_trace(INT32 level, const char *file, int line, const char *format, ...)
{
    INT8 details[128] = {0};
    va_list ap;

	if(g_OsaDbgCb != NULL)
    {
        va_start(ap, format);
        vsnprintf(details, sizeof(details), format, ap);
        va_end(ap);

		g_OsaDbgCb(level, file, line, details);
	}

	return;
}

void osa_debug_info(INT32 event, void *data, UINT16 length)
{
	if(g_OsaDbgCb != NULL)
    {
		g_OsaDbgCb(event, 0, length, data);
	}

	return;
}

