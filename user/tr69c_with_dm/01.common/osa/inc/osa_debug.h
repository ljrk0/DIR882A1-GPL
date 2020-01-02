/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_debug.h
  Description:      header file for osa debugging
  History:
    1. zhonghai    2013-10-08         creation
        
*******************************************************************************/


#ifndef OSA_DEBUG_H
#define OSA_DEBUG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/** osa debug definition */
#define OSA_DEBUG_LEVEL_DEBUG      0x00000000
#define OSA_DEBUG_LEVEL_TRACE      0x00000001
#define OSA_DEBUG_LEVEL_WARNING    0x00000002
#define OSA_DEBUG_LEVEL_ERROR      0x00000003
#define OSA_DEBUG_LEVEL_INFO       0x00000004

#define OSA_DEBUG_LEVEL_MASK       0x000000ff

#define OSA_DEBUG_INFO_TIMER_STATUS  (OSA_DEBUG_LEVEL_INFO|0x28000000)

typedef void (*OSA_DBG_FUN)(INT32 level, const INT8 *file, INT32 line, INT8 *detail);

/**
* register a callback and the callback will catch all osa internal traces.
*
* @param[in] osaDbgCb the callback function.
* @return null.
*
*/
extern void osa_debug_callback(OSA_DBG_FUN osaDbgCb);

/**
* output osa traces.
*
* @param[in] level trace level.
* @param[in] file the src file.
* @param[in] line the line number in the src file.
* @param[in] format the trace string.
* @return null.
*
*/
extern void osa_debug_trace(INT32 level, const char *file, int line, const char *format, ...);

/**
* output osa info.
*
* @param[in] event event id.
* @param[in] data the data.
* @param[in] length the length of data.
* @return null.
*
*/
extern void osa_debug_info(INT32 event, void *data, UINT16 length);

#define OSA_DBG_DEBUG(...)                         osa_debug_trace(OSA_DEBUG_LEVEL_DEBUG,__FILE__,__LINE__,__VA_ARGS__)
#define OSA_DBG_TRACE(...)                         osa_debug_trace(OSA_DEBUG_LEVEL_TRACE,__FILE__,__LINE__,__VA_ARGS__)
#define OSA_DBG_WARNING(...)                       osa_debug_trace(OSA_DEBUG_LEVEL_WARNING,__FILE__,__LINE__,__VA_ARGS__)
#define OSA_DBG_ERROR(...)                         osa_debug_trace(OSA_DEBUG_LEVEL_ERROR,__FILE__,__LINE__,__VA_ARGS__)
#define OSA_DBG_INFO(event,data,length)            osa_debug_info(event,data,length)


#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif


