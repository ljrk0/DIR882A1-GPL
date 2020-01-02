/******************************************************************************
  Copyright (c), 1991-2008, T&W ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: tbsflash.h
  文件描述: 与flash存储区操作相关的数据结构定义
 ******************************************************************************/
 
#ifndef __TBS_FLASH_H__
#define __TBS_FLASH_H__

#include "warnlog.h"

#define MAX_FLASH_LEN       0xffffUL  /*original_data flash存储区的空间为64KB*/

#define ORIGINAL_DATA_PATH  "/proc/llconfig/original_data"

/*定义写flash时的标志*/
#define FLAG_APPEND   1   /*将写入数据追加到现有数据的尾端*/
#define FLAG_TRUNC    2   /*写入的数据完全覆盖以前的数据*/

#ifdef  FLASH_DEBUG
#define FLASH_TRACE(fmt, args...) TBS_TRACE(0, fmt, ##args)
#else
#define FLASH_TRACE(str, args...)  do { ; } while(0)
#endif


/* 定义original_data flash文件数据头部信息结构 */
typedef struct tag_ST_FLASH_DATA_INFO {
    int iObjNum;               /* 在flash中存放数据的对象数目 */
    unsigned long ulDataLen;   /* 总的数据长度 */
    char acData[0];
} ST_FLASH_DATA_INFO;   


/* 定义访问对象ID */
typedef enum tag_EN_OBJ_ID {
    OBJ_LOGGER = 1,
    OBJ_PPPOE,
    OBJ_LAN,
    OBJ_ALG,
    OBJ_TR069,
    OBJ_ETHWAN,
    OBJ_ETHLAN,
    OBJ_SNMPA,
    OBJ_UPGRADE
       
} EN_OBJ_ID;


/* 定义访问对象数据信息头部结构 */
typedef struct tag_ST_OBJ_DATA_INFO {
    EN_OBJ_ID     enObjID;     /* 对象ID */
    unsigned long ulDataLen;   /* 对象数据长度 */  
    char          acData[0];
} ST_OBJ_DATA_INFO;

/************************** 函数原型声明 **************************/

/* 读取对象在original_data flash文件中存储的数据 */
int ReadOrigiData(EN_OBJ_ID enObjId, char *pcBuf, unsigned long ulCount);

/* 对象向original_data flash 文件中写入数据*/
int WriteOrigiData(EN_OBJ_ID enObjId, const char *pcBuf, unsigned long ulCount, int iFlag);


#endif /*__TBS_FLASH_H__ */

