/******************************************************************************
  Copyright (c), 1991-2008, T&W ELECTRONICS(SHENTHEN) Co., Ltd.

  �ļ�����: tbsflash.h
  �ļ�����: ��flash�洢��������ص����ݽṹ����
 ******************************************************************************/
 
#ifndef __TBS_FLASH_H__
#define __TBS_FLASH_H__

#include "warnlog.h"

#define MAX_FLASH_LEN       0xffffUL  /*original_data flash�洢���Ŀռ�Ϊ64KB*/

#define ORIGINAL_DATA_PATH  "/proc/llconfig/original_data"

/*����дflashʱ�ı�־*/
#define FLAG_APPEND   1   /*��д������׷�ӵ��������ݵ�β��*/
#define FLAG_TRUNC    2   /*д���������ȫ������ǰ������*/

#ifdef  FLASH_DEBUG
#define FLASH_TRACE(fmt, args...) TBS_TRACE(0, fmt, ##args)
#else
#define FLASH_TRACE(str, args...)  do { ; } while(0)
#endif


/* ����original_data flash�ļ�����ͷ����Ϣ�ṹ */
typedef struct tag_ST_FLASH_DATA_INFO {
    int iObjNum;               /* ��flash�д�����ݵĶ�����Ŀ */
    unsigned long ulDataLen;   /* �ܵ����ݳ��� */
    char acData[0];
} ST_FLASH_DATA_INFO;   


/* ������ʶ���ID */
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


/* ������ʶ���������Ϣͷ���ṹ */
typedef struct tag_ST_OBJ_DATA_INFO {
    EN_OBJ_ID     enObjID;     /* ����ID */
    unsigned long ulDataLen;   /* �������ݳ��� */  
    char          acData[0];
} ST_OBJ_DATA_INFO;

/************************** ����ԭ������ **************************/

/* ��ȡ������original_data flash�ļ��д洢������ */
int ReadOrigiData(EN_OBJ_ID enObjId, char *pcBuf, unsigned long ulCount);

/* ������original_data flash �ļ���д������*/
int WriteOrigiData(EN_OBJ_ID enObjId, const char *pcBuf, unsigned long ulCount, int iFlag);


#endif /*__TBS_FLASH_H__ */

