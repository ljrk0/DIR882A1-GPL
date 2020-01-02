/**@file common_type_def.h
 * @brief common type definition used by LTE stack in CPU-L
 *
 * 
*/


#ifndef _COMMON_TYPE_DEF_H_
#define _COMMON_TYPE_DEF_H_
    
#ifdef __cplusplus
    extern "C" {
#endif
 

/* 
 * basic type used in layer1
 */
#ifndef _UINT8
#define _UINT8
    typedef unsigned char       UINT8;
#endif
    
#ifndef _INT8
#define _INT8
    typedef char                INT8;
#endif
    
#ifndef _UINT16
#define _UINT16
    typedef unsigned short      UINT16;
#endif
    
#ifndef _INT16
#define _INT16
    typedef short               INT16;
#endif
    
#ifndef _UINT32
#define _UINT32
    typedef unsigned int        UINT32;
#endif
    
#ifndef _INT32
#define _INT32
    typedef int                 INT32;
#endif
    
#ifndef _UINT64
#define _UINT64
    typedef unsigned long long  UINT64;
#endif
    
#ifndef _INT64
#define _INT64
    typedef long long           INT64;
#endif
    
#ifndef _BOOL8
#define _BOOL8
    typedef unsigned char       BOOL8;
#endif
    
    
/* typedef unsigned char       UINT8;   */
/* typedef signed  char        INT8;    */
/* typedef unsigned short      UINT16;  */
/* typedef signed   short      INT16;   */
/* typedef unsigned int        UINT32;  */
/* typedef signed   int        INT32;   */

#ifndef VOID
#define VOID                void
#endif

#ifndef NULL
#define NULL                ((void*)0)
#endif
    
#ifndef	CONST
#define CONST               const
#endif
    

#ifdef __cplusplus
    }   /* __cplusplus */
#endif



#endif /* _COMMON_TYPE_DEF_H_ */
