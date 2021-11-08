/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 �ļ����� : cfgapi.h
 �ļ����� : cfg����ͷ�ļ�, ���������ṹ����ͺ�������


 �޶���¼ :
          1 ���� : ��Ծ��
            ���� : 2007-10-23
            ���� :

**********************************************************************/
#ifndef __CFGAPI_H__
#define __CFGAPI_H__

#include "tbstype.h"
#include "tbsutil.h"
#include "tbserror.h"


#ifdef _cplusplus
    #if _cplusplus
        extern "C" {
    #endif
#endif


/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/



#define CFG_OK              TBS_SUCCESS
#define CFG_FAIL            TBS_FAILED

#define ERR_CFG_MALLOC_FAIL     ERR_MALLOC_FAILED
#define ERR_CFG_BUF_NOT_ENOUGH  ERR_BUF_NOT_ENOUGH
#define ERR_CFG_INTERNAL_ERR    ERR_INTERNAL_ERR
#define ERR_CFG_PARA_INVALID    ERR_PARA_INVALID

#define ERR_CFG_FILE_OPEN       ERR_FILE_OPEN
#define ERR_CFG_FILE_READ       ERR_FILE_READ
#define ERR_CFG_FILE_WRITE      ERR_FILE_WRITE
#define ERR_CFG_FILE_CHKSUM     ERR_FILE_CHKSUM
#define ERR_CFG_NOT_SUPPORT     ERR_NOT_SUPPORT_FUNCTION



#define CFG_PASSIVE_NOTI  1
#define CFG_ACTIVE_NOTI   2

#define CFG_NO_WRITABLE  0
#define CFG_WRITABLE     1
#define CFG_ADDABLE      2
#define CFG_DELETABLE    3

#define MAX_CONFIG_SIZE 		(unsigned long)512*1024   	    //512K  bytes


typedef long CFG_RET;


/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/


/* ���ò�����ֵ�Ľṹ */
typedef struct tagST_PARA_VAL
{
    struct tagST_PARA_VAL *pstNext;
    char *pcType;         /* ָ�����ֵ������, ���ṹ���ڻ�ȡ�����б�ʱ,
                             ���ֶβ������� */
    char *pcName;         /* ָ��������� */
    char *pcVal;          /* ָ�����ֵ
                             ���˽ṹ���ڻ�ȡ����ʱ, �����ʾ�����б�,
                             �����¸���ʽ: attr1="aaa" attr2="abc def"
                             ���˽ṹ���ڻ�ȡ��������ʱ, �����ʾ�Ƿ��д,
                             ��ʽΪ: Writable="0" ���� Writable="1"
                          */
} ST_PARA_VAL;




/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/


/* ������Ի�ȡ֪ͨ�Ľӿ� */
typedef CFG_RET (*FUNC_NOTI_INFORM)(unsigned char ucNotiType);



/***********************************************************
�ӿ�:   �ͷ�����ģ�������ڴ�
����:   pvMem, ͨ�������ṩ�Ľӿ�ȡ�õ��ڴ�
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_MemFree(void *pvMem);


/***********************************************************
�ӿ�:   ��ȡ֪ͨ
����:   ppstParaVal: ���֪ͨ������ͷ��ַ, ������ģ�����,
                      ʹ�������� CFG_MemFree �ͷ�
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_GetNotification(ST_PARA_VAL **ppstParaVal);


/* ���֪ͨ */
CFG_RET CFG_ClearNoti(void);


/***********************************************************
�ӿ�:   ���ڵ�ֵ
����:   pcPath: �ڵ�·��,
        pcVal: �ڵ�ֵ,
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_CheckNodeVal(const char *pcPath, const char *pcVal);

/***********************************************************
�ӿ�:   ���ýڵ�����
����:   pcPath: �ڵ�·��
        pcVal: Ҫ���óɵ�ֵ
       pcAccessor: ������, ҵ�����ģ�����ʱֱ����дNULL
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_SetNodeVal(const char *pcPath, const char *pcVal, const char *pcAccessor);


/***********************************************************
�ӿ�:   ��ȡ�ڵ�����
����:   pcPath: �ڵ�·��
        ppstParaVal: ����ڵ�ֵ����ͷ
       pcAccessor: ������, ҵ�����ģ�����ʱֱ����дNULL
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_GetNodeVal(const char *pcPath, ST_PARA_VAL **ppstParaVal,
                       const char *pcAccessor);





/***********************************************************
�ӿ�:   ��ȡҶ�ڵ����ݺ�����
����:   pcPath: �ڵ�·��,
        pcVal: ����ڵ�״ֵ̬, �ڴ��ɵ����߷���,
        pulLen: ����pcVal����Ч����,
                �������, �����ʵ����Ҫ�೤.
        pcType �� pulTypeLen ������ͬ��
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_GetLeafValAndType(const char *pcPath,
                char *pcVal, unsigned long *pulValLen,
                char *pcType, unsigned long *pulTypeLen);


/***********************************************************
�ӿ�:   ���ýڵ�����
����:   pcPath: �ڵ�·��
        pcType: �ڵ�����, ����Ϊ��
        pcVal: �ڵ�ֵ, ����Ϊ��
        ucIsStandard, �Ƿ��׼�ڵ�, 0Ϊ�Ǳ�׼�ڵ�, 1Ϊ��׼�ڵ�
        ucNeedSave, �Ƿ���Ҫ����, 0Ϊ����Ҫ����, 1Ϊ��Ҫ����
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_AddNode(const char *pcPath, const char *pcType, const char *pcVal,
                 unsigned char ucIsStandard, unsigned char ucNeedSave,
                 unsigned char ucWritable);

CFG_RET CFG_AddNodeEx(const char *pcPath, const char *pcType, const char *pcVal,
                 unsigned char ucIsStandard, unsigned char ucNeedSave,
                 unsigned char ucWritable, unsigned char ucNotify);





/***********************************************************
�ӿ�:   ���ýڵ�����
����:   pcPath: �ڵ�·��
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_DelNode(const char *pcPath);


/***********************************************************
�ӿ�:   ��ȡ�ڵ�����
����:   pcPath: �ڵ�·��,
        ucNextLevel: �ο�Tr069
        ppstParaInfo: ����ڵ���Ϣ����ͷ
       pcAccessor: ������, ҵ�����ģ�����ʱֱ����дNULL
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_GetNodeName(const char *pcPath, unsigned char ucNextLevel,
                         ST_PARA_VAL **ppstParaInfo, const char *pcAccessor);


/***********************************************************
�ӿ�:   ���ýڵ�����
����:   pstAttVal: ·��������ֵ�ṹ,
       pcAccessor: ������, ҵ�����ģ�����ʱֱ����дNULL
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_SetNodeAttr(const ST_PARA_VAL *pstAttSet, const char *pcAccessor);

CFG_RET CFG_CheckNodeAttrSet(const ST_PARA_VAL *pstAttSet, const char *pcAccessor);

/***********************************************************
�ӿ�:   ���ýڵ�����
����:   pstAttVal: ·��������ֵ�ṹ,
       pcAccessor: ������, ҵ�����ģ�����ʱֱ����дNULL
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_GetNodeAttr(const char *pcPath, ST_PARA_VAL **ppstAttVal,
                        const char *pcAccessor);


/***********************************************************
�ӿ�:   ���ҽڵ�����
����:   pcPath: ·��
                pszAttrName: �ڵ�������
����ֵ: �ɹ� �ڵ�����ֵָ��(����Ҫ�ͷ�)
                  :ʧ��     NULL
***********************************************************/
const char * CFG_GetAttrValByName(const char *pcPath, const char * pszAttrName);

/***********************************************************
�ӿ�:   ���������ļ�
����:   pcFileContent: �����ļ�����
        ulLen: ���ݳ���
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_SetCfgFile(const char *pcFile, unsigned long ulLen);

#ifdef CONFIG_LAST_SAVE_CFG
/***********************************************************
�ӿ�:   ��ȡѹ���ļ����������ļ�
����:   pcFile: ѹ���ļ�·��
        ulLen: ���ݳ���
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_SetCompressCfgFile(const char *pcFile, unsigned long ulLen);
#endif

/***********************************************************
�ӿ�:   ���������ļ�
����:   pcFileContent: ������
        pulLen: ���뻺��������, ������Ȳ���, ��������ʵ����Ҫ���ĳ���
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_GetCfgFile(char *pcFile, unsigned long *pulLen,
                       unsigned long ulCompress, unsigned long ulOverWrite,
                       const char *pcAccessor);

/*************************************************************************
�ӿ�:   д��ָ������д���ļ�
����:         pcFile, Ҫд����ļ���
               pcBuf, Ҫд�������
               ulLen, Ҫд��ĳ���
����ֵ:        0,�ɹ�;
               ����, ʧ��
*************************************************************************/
CFG_RET CFG_WriteFile(const char *pcFile, const char *pcBuf, unsigned long ulLen);




/***********************************************************
�ӿ�:   д����
����:   ��
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_SaveCfg(void);



/***********************************************************
�ӿ�:   ���ÿ��ʼ��
����:   ��
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_LibInit(void);


long CFG_LibFinal(void);



/***********************************************************
�ӿ�:   ע��ڵ��MID
����:   pcPath, �ڵ�·��
        usMID: ģ��id
����ֵ: 0:�ɹ�
        ����:ʧ��
��ע:
1��ģ��Ĺ���ΧĬ��Ϊ����,����ע��xxx.1��ģ��1������,��xxx.1.yyy���Զ�����ģ
��1�Ĺ���Χ
2�����ͬһ��·����ע����,�����һ�ε�Ϊ׼.
3���ڵ�·��֧��ͨ���, ����ָ��xxx.{i}.yyy.1����ģ��1,��ʾ��xxx
�����е�ʵ������ģ��1
������,��xxx.{i}.yyy��ֻ��ʵ��1����ģ��1. ���ͬʱָ����xxx.2����ģ��2,��ģ��1
��ʧȥ
��xxx.2�Ĺ�������.

***********************************************************/
CFG_RET CFG_RegisterMID(const char *pcPath, unsigned short usMID);




/***********************************************************
�ӿ�:   ע����Ĵ˽ڵ��MID
����:   pcPath, �ڵ�·��
        usMID: ģ��id
����ֵ: 0:�ɹ�
        ����:ʧ��
��ע:
1��pcPath���뵽Ҷ�ӽڵ�, Ҳ����˵CareMID�����÷�ΧΪָ���ڵ�.
2�����ͬһ��·����ע����, ÿ������Ч.
3���ڵ�·��֧��ͨ���.��ͨ����Ļ���������ָ���ض�ʵ�����ڱ���һ��ģ�����,
���ʵ
��������ģ�鶼��Ч.

***********************************************************/
CFG_RET CFG_RegisterCareMID(const char *pcPath, unsigned short usMID);


/***********************************************************
�ӿ�:   ��ȡ�ڵ�������MID
����:   pcPath, �ڵ�·��
       pusMid, ���MID
����ֵ: 0:�ɹ�
        ����:ʧ��
��ע:
***********************************************************/
CFG_RET CFG_GetNodeMID(const char *pcPath, unsigned short *pusMID);



/***********************************************************
�ӿ�:   ��ȡ�ڵ�������MID
����:   pcPath, �ڵ�·��
       pcAccessor, �����ߡ�ҵ�����ģ��ʹ��ʱ��ֱ�Ӵ�NULL
       pusMid, ���MID
       pusCareMIDList, ���ĵ�MID�б�, ��0��Ϊ������.
       ppvNode, �������. ģ��ʹ��ʱ��ֱ�Ӵ�NULL
����ֵ: 0:�ɹ�
        ����:ʧ��, ����Ȩ�޷��ʵ����
��ע:
***********************************************************/
CFG_RET CFG_GetNodeMIDList(const char *pcPath, const char *pcAccessor,
                    unsigned short *pusMID, unsigned short * pusCareMIDList,
                    unsigned long *pulCareMIDCount,
                    void **ppvNode, const char *pcVal);






/* ȡ�ýڵ������Ѿ�ʹ�õ�ʵ����� */
/***********************************************************
�ӿ�:   ��ȡ��ǰ���󼯺�����ʹ�ù�������ʵ�����
����:   pcPath, �ڵ�·��
       pulMaxIdx, ��������
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_GetNodeMaxIdx(const char *pcPath, unsigned long *pulMaxIdx);


/* ���ýڵ������Ѿ�ʹ�õ�ʵ����� */
/***********************************************************
�ӿ�:   ��ȡ��ǰ���󼯺�����ʹ�ù�������ʵ�����
����:   pcPath, �ڵ�·��
        ulMaxIdx, Ҫ���óɵ������
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_SetNodeMaxIdx(const char *pcPath, unsigned long ulMaxIdx);


/* ȡ�ýڵ��������� */
/***********************************************************
�ӿ�:   ��ȡ�ڵ���һ���ӽڵ����
����:   pcPath, �ڵ�·��
       pulCount, �������
����ֵ: 0:�ɹ�
        ����:ʧ��
***********************************************************/
CFG_RET CFG_GetNodeChildCount(const char *pcPath, unsigned int *pulCount);




/* �ǵݹ�����ӿ�, ȡ��һ���ڵ� */
/*************************************************************************
�ӿ�: �ǵݹ������һ���ڵ�
����:    pvPath, ��ǰ�ڵ�ָ��
        pusMID, ����ڵ�������mid
����ֵ:   ��NULL��ʾ��һ���ڵ��ָ��
          NULL��ʾ��������
��ע:
*************************************************************************/
void *CFG_GetNextNode(void *pvPath, unsigned short *pusMID);


/* �ָ��������� */
CFG_RET CFG_RecoverFactSet(const char *pcAccessor);

/* �����ǰ���� */
CFG_RET CFG_ClearCfg(void);

#ifdef CONFIG_APPS_LOGIC_WANSELECT
CFG_RET CFG_ClearAllCfg(void);
#endif

/* ע��֪ͨ����Ľӿ� */
CFG_RET CFG_RegNotiInformIntf(FUNC_NOTI_INFORM pfnNotiInform);

/* ���÷ǵݹ����MID�Ļ�����Ϣ */
CFG_RET CFG_SetMIDAccessInfo(const char *pcPath, const void **ppvTree,
           unsigned short *pusMID, const char *pcAccessor,
           char **ppcPathSave);

/* �ǵݹ����MID, ȡ��һ��MID */
const void *CFG_GetNextMID(const void *pvPrev, unsigned short *pusMID,
                                    unsigned char *pucLen);

/* ���ɾ���ڵ�ĺϷ��� */
CFG_RET CFG_CheckAndSetNodeDeleting(const char *pcPath, const char *pcAccessor);

/* ���ýڵ�����ֵ�Ľӿ� */
CFG_RET CFG_SetNodeAttrVal(const char *pcPath,
         const char *pcAttr, const char *pcVal);

#define CFG_DATA_TREE 0
#define CFG_LOGIC_TREE 1




#ifdef _CFG_DEBUG
CFG_RET CFG_PrintNode(const void *pvTree, const char *pcPath,
                      unsigned long ucLen, void *pvFile);
#endif






/* ��������ʵ��ֱ��ת����һ���ṹ */
CFG_RET CFG_GetValToStruct(const char *pcPath, void *pvStruct,
                           char * const *ppcLeafList,
                           unsigned long ulLeafNum);

/* ͨ���ڵ�·����ȡһ���ڵ��б�ֵ��һ���ṹ����, ���нڵ�ֵ��string������� */
CFG_RET CFG_GetValToStrArr(const char *pcPath, void *pvStrArr,
                           char * const *ppcLeafList,
                           unsigned long ulLeafNum);


/* ���ṹ���е�ֵ���õ�������ʵ���� */
CFG_RET CFG_SetValFromStruct(const char *pcPath, const void *pvStruct,
                           char * const *ppcLeafList,
                           unsigned long ulLeafNum);

/***********************************************************
�ӿ�:   ͨ���ڵ�·������һ���ڵ��б�ֵ, ֵ��Դ��һ���ַ�����
����:   pcPath, �ڵ�·��
        pvStrArr, ����ֵ����Դ
        ppcLeafList, �ڵ��б�, �ڵ���Ϊ"" �Ľ�������
        ulLeafNum, �ڵ����, ʵ������ָ ppcLeafList �е�Ԫ�ظ���
����ֵ: 0:�ɹ�
        ����: ʧ��
��ע:
***********************************************************/
CFG_RET CFG_SetValFromStrArr(const char *pcPath, void *pvStrArr,
                           char * const *ppcLeafList,
                           unsigned long ulLeafNum);

/* �г���������ָ��MID�����нڵ� */
CFG_RET CFG_ListMIDAllNodes(unsigned short usMID, ST_PARA_VAL **ppstParaVal);

/* �½�һ������ʵ�� */
CFG_RET CFG_AddObjInst(const char *pcPath, unsigned long *pulIndex);

/* ��ȡһ���ڵ��ֵ�ĵ�ַ, �����͵�ַ */
CFG_RET CFG_GetNodeValPtr(const char *pcPath, const char **ppcVal,
                          const char **ppcType);

#define CFG_MAX_DEPTH         64

#define CFG_MAX_INDEX_DEPTH   16

typedef struct tag_ST_CFG_INST
{
    struct tag_ST_CFG_INST *pstNext;
    unsigned long aulIndexList[CFG_MAX_INDEX_DEPTH];
    char acData[0];

} ST_CFG_INST_NODE;

/* ��ȡһ��ͨ��·���µ�����ʵ���Ľṹ���� */
CFG_RET CFG_GetPathInstList(const char *pcMatchPath, char * const *ppcLeafList,
                           unsigned long ulLeafNum,
                           ST_CFG_INST_NODE **ppstInstList);

/* ͨ��ͨ��·����index�б��ȡһ��������Ҷ�ӽڵ�ֵ��һ���ṹ���� */
CFG_RET CFG_GetValToStructEx(const char *pcMatchPath,
                           const unsigned long *pulIndexList,
                           void *pvStruct, char * const *ppcLeafList,
                           unsigned long ulLeafNum);

/* ͨ���ڵ�ͨ��·������һ���ڵ��б�ֵ, ֵ��Դ��һ���ṹ�� */
CFG_RET CFG_SetValFromStructEx(const char *pcMatchPath,
                           const unsigned long *pulIndexList,
                           const void *pvStruct, char * const *ppcLeafList,
                           unsigned long ulLeafNum);

/* ��ȡ�����µ�mid�б�, �ú�����ǰ�����������ķ�װ */
CFG_RET CFG_GetObjMIDList(unsigned char ucTree, const char *pcPath,
                unsigned short *pusMIDList, unsigned long *pulCount);

CFG_RET CFG_GetNodeMIDFromFullTree(const char *pcPath, unsigned short *pusMIDList,
                                   unsigned long  *pulCount);

/* ���ýڵ����� */
CFG_RET CFG_SetNodeName(const char *pcPath, const char *pcNewName);

/* ͨ���ڵ��ַ���ýڵ����� */
CFG_RET CFG_SetNodeNameByPtr(void *pvNode, const char *pcNewName);

/* ���ڵ�·����Ӧ������������ */
CFG_RET CFG_DelNodeToPtr(const char *pcPath, void **ppvNode);

/* ��������ӵ�ָ����·���� */
CFG_RET CFG_AddNodeFromPtr(const char *pcPath, void *pvNode);

/* Ϊ�о�ָ��mid�����нڵ������׼������ */
void CFG_MIDInstListBegin(unsigned short usMID);

/* Ϊ�о�ָ��mid�����нڵ������׼������, �ú�������ָ��·�� */
void CFG_MIDInstListBeginEx(unsigned short usMID, const char *pcPath);

/* ��������, ��ȡ��һ�������� ����ָ��mid�Ľڵ��б� */
CFG_RET CFG_ListMIDNextInstNodes(ST_PARA_VAL **ppstParaVal);

/* ��ȡͨ��·����Ҷ�ӽڵ��б� */
CFG_RET CFG_GetMatchNodeName(const char *pcMatchPath,
                             unsigned char ucNextLevel,
                             ST_PARA_VAL **ppstParaInfo, const char *pcAccessor);

/* ��ȡ�ڵ�����ֵָ�� */
CFG_RET CFG_GetNodeAttrValPtr(const char *pcPath, const char *pcAttr,
                              const char **ppcVal);

/* �Խڵ�ֵ��������ָ����ֵ, ֻ��int��ڵ������� */
CFG_RET CFG_IncIntNodeVal(const char *pcPath, long lVal);


#define CFG_LAST_NODE  0xfffffffe

/* ���ݽڵ�˳���ȡ�ڵ����� */
CFG_RET CFG_GetNodeOrderName(const char *pcPath, unsigned long ulOrder,
                             ST_PARA_VAL **pstPara);

/* ���ͨ��·������ڵ��µ�ʵ�� */
CFG_RET CFG_ClearObjInstEx(const char *pcMatchPath, unsigned char ucRestIdx);


CFG_RET CFG_GetMwNoti(unsigned char ucDst, ST_PARA_VAL **ppstParaVal);
CFG_RET CFG_GetNodeMwAttr(const char *pcPath, ST_PARA_VAL **ppstAttVal,
                                                 const char *pcAccessor);

#ifdef  CONFIG_APPS_LOGIC_DEVCONFIG
CFG_RET CFG_CheckConfigFile(const char *pszValue);
CFG_RET CFG_SaveCurrentCfgToConfigNode(char *pszValue);
CFG_RET CFG_SaveCurrentCfgToConfigNodeFromWeb();
CFG_RET CFG_EffectDeviceConfigNode();
#endif


#ifdef _cplusplus
    #if _cplusplus
        }
    #endif
#endif


#endif /* __CFGAPI_H__ */

