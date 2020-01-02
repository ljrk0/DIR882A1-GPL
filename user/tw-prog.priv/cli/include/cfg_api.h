/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : cfgapi.h
 文件描述 : cfg对外头文件, 包括参数结构定义和函数声明


 修订记录 :
          1 创建 : 陈跃东
            日期 : 2007-10-23
            描述 :

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


/* 配置参数和值的结构 */
typedef struct tagST_PARA_VAL
{
    struct tagST_PARA_VAL *pstNext;
    char *pcType;         /* 指向参数值的类型, 当结构用于获取属性列表时,
                             此字段不起作用 */
    char *pcName;         /* 指向参数名称 */
    char *pcVal;          /* 指向参数值
                             当此结构用于获取属性时, 这里表示属性列表,
                             按如下个格式: attr1="aaa" attr2="abc def"
                             当此结构用于获取参数名称时, 这里表示是否可写,
                             格式为: Writable="0" 或者 Writable="1"
                          */
} ST_PARA_VAL;




/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/


/* 报告可以获取通知的接口 */
typedef CFG_RET (*FUNC_NOTI_INFORM)(unsigned char ucNotiType);



/***********************************************************
接口:   释放配置模块分配的内存
参数:   pvMem, 通过配置提供的接口取得的内存
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_MemFree(void *pvMem);


/***********************************************************
接口:   获取通知
参数:   ppstParaVal: 输出通知的链表头地址, 由配置模块分配,
                      使用完请用 CFG_MemFree 释放
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_GetNotification(ST_PARA_VAL **ppstParaVal);


/* 清除通知 */
CFG_RET CFG_ClearNoti(void);


/***********************************************************
接口:   检查节点值
参数:   pcPath: 节点路径,
        pcVal: 节点值,
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_CheckNodeVal(const char *pcPath, const char *pcVal);

/***********************************************************
接口:   设置节点内容
参数:   pcPath: 节点路径
        pcVal: 要设置成的值
       pcAccessor: 访问者, 业务代理模块调用时直接填写NULL
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_SetNodeVal(const char *pcPath, const char *pcVal, const char *pcAccessor);


/***********************************************************
接口:   获取节点内容
参数:   pcPath: 节点路径
        ppstParaVal: 输出节点值链表头
       pcAccessor: 访问者, 业务代理模块调用时直接填写NULL
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_GetNodeVal(const char *pcPath, ST_PARA_VAL **ppstParaVal,
                       const char *pcAccessor);





/***********************************************************
接口:   获取叶节点内容和类型
参数:   pcPath: 节点路径,
        pcVal: 输出节点状态值, 内存由调用者分配,
        pulLen: 输入pcVal的有效长度,
                如果不够, 将输出实际需要多长.
        pcType 和 pulTypeLen 的意义同上
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_GetLeafValAndType(const char *pcPath,
                char *pcVal, unsigned long *pulValLen,
                char *pcType, unsigned long *pulTypeLen);


/***********************************************************
接口:   设置节点内容
参数:   pcPath: 节点路径
        pcType: 节点类型, 可以为空
        pcVal: 节点值, 可以为空
        ucIsStandard, 是否标准节点, 0为非标准节点, 1为标准节点
        ucNeedSave, 是否需要保存, 0为不需要保存, 1为需要保存
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_AddNode(const char *pcPath, const char *pcType, const char *pcVal,
                 unsigned char ucIsStandard, unsigned char ucNeedSave,
                 unsigned char ucWritable);

CFG_RET CFG_AddNodeEx(const char *pcPath, const char *pcType, const char *pcVal,
                 unsigned char ucIsStandard, unsigned char ucNeedSave,
                 unsigned char ucWritable, unsigned char ucNotify);





/***********************************************************
接口:   设置节点内容
参数:   pcPath: 节点路径
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_DelNode(const char *pcPath);


/***********************************************************
接口:   获取节点名称
参数:   pcPath: 节点路径,
        ucNextLevel: 参考Tr069
        ppstParaInfo: 输出节点信息链表头
       pcAccessor: 访问者, 业务代理模块调用时直接填写NULL
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_GetNodeName(const char *pcPath, unsigned char ucNextLevel,
                         ST_PARA_VAL **ppstParaInfo, const char *pcAccessor);


/***********************************************************
接口:   设置节点属性
参数:   pstAttVal: 路径和属性值结构,
       pcAccessor: 访问者, 业务代理模块调用时直接填写NULL
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_SetNodeAttr(const ST_PARA_VAL *pstAttSet, const char *pcAccessor);

CFG_RET CFG_CheckNodeAttrSet(const ST_PARA_VAL *pstAttSet, const char *pcAccessor);

/***********************************************************
接口:   设置节点属性
参数:   pstAttVal: 路径和属性值结构,
       pcAccessor: 访问者, 业务代理模块调用时直接填写NULL
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_GetNodeAttr(const char *pcPath, ST_PARA_VAL **ppstAttVal,
                        const char *pcAccessor);


/***********************************************************
接口:   查找节点属性
参数:   pcPath: 路径
                pszAttrName: 节点属性名
返回值: 成功 节点属性值指针(不需要释放)
                  :失败     NULL
***********************************************************/
const char * CFG_GetAttrValByName(const char *pcPath, const char * pszAttrName);

/***********************************************************
接口:   更新配置文件
参数:   pcFileContent: 配置文件内容
        ulLen: 内容长度
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_SetCfgFile(const char *pcFile, unsigned long ulLen);

#ifdef CONFIG_LAST_SAVE_CFG
/***********************************************************
接口:   读取压缩文件更新配置文件
参数:   pcFile: 压缩文件路径
        ulLen: 内容长度
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_SetCompressCfgFile(const char *pcFile, unsigned long ulLen);
#endif

/***********************************************************
接口:   备份配置文件
参数:   pcFileContent: 缓冲区
        pulLen: 输入缓冲区长度, 如果长度不够, 这里会输出实际需要多大的长度
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_GetCfgFile(char *pcFile, unsigned long *pulLen,
                       unsigned long ulCompress, unsigned long ulOverWrite,
                       const char *pcAccessor);

/*************************************************************************
接口:   写将指定内容写入文件
参数:         pcFile, 要写入的文件名
               pcBuf, 要写入的内容
               ulLen, 要写入的长度
返回值:        0,成功;
               其它, 失败
*************************************************************************/
CFG_RET CFG_WriteFile(const char *pcFile, const char *pcBuf, unsigned long ulLen);




/***********************************************************
接口:   写配置
参数:   无
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_SaveCfg(void);



/***********************************************************
接口:   配置库初始化
参数:   无
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_LibInit(void);


long CFG_LibFinal(void);



/***********************************************************
接口:   注册节点的MID
参数:   pcPath, 节点路径
        usMID: 模块id
返回值: 0:成功
        其它:失败
备注:
1．模块的管理范围默认为子树,比如注册xxx.1由模块1来管理,则xxx.1.yyy将自动纳入模
块1的管理范围
2．如果同一个路径被注册多次,以最后一次的为准.
3．节点路径支持通配符, 比如指明xxx.{i}.yyy.1属于模块1,表示对xxx
下所有的实例都由模块1
来管理,但xxx.{i}.yyy下只有实例1属于模块1. 如果同时指明了xxx.2属于模块2,则模块1
将失去
对xxx.2的管理作用.

***********************************************************/
CFG_RET CFG_RegisterMID(const char *pcPath, unsigned short usMID);




/***********************************************************
接口:   注册关心此节点的MID
参数:   pcPath, 节点路径
        usMID: 模块id
返回值: 0:成功
        其它:失败
备注:
1．pcPath必须到叶子节点, 也就是说CareMID的作用范围为指定节点.
2．如果同一个路径被注册多次, 每个都有效.
3．节点路径支持通配符.在通配符的基础上若再指明特定实例属于被另一个模块关心,
则该实
例对两个模块都有效.

***********************************************************/
CFG_RET CFG_RegisterCareMID(const char *pcPath, unsigned short usMID);


/***********************************************************
接口:   获取节点所属的MID
参数:   pcPath, 节点路径
       pusMid, 输出MID
返回值: 0:成功
        其它:失败
备注:
***********************************************************/
CFG_RET CFG_GetNodeMID(const char *pcPath, unsigned short *pusMID);



/***********************************************************
接口:   获取节点所属的MID
参数:   pcPath, 节点路径
       pcAccessor, 访问者。业务代理模块使用时可直接传NULL
       pusMid, 输出MID
       pusCareMIDList, 关心的MID列表, 以0作为结束符.
       ppvNode, 输出子树. 模块使用时可直接传NULL
返回值: 0:成功
        其它:失败, 含无权限访问的情况
备注:
***********************************************************/
CFG_RET CFG_GetNodeMIDList(const char *pcPath, const char *pcAccessor,
                    unsigned short *pusMID, unsigned short * pusCareMIDList,
                    unsigned long *pulCareMIDCount,
                    void **ppvNode, const char *pcVal);






/* 取得节点最大的已经使用的实例序号 */
/***********************************************************
接口:   获取当前对象集合下已使用过的最大的实例编号
参数:   pcPath, 节点路径
       pulMaxIdx, 输出最大编号
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_GetNodeMaxIdx(const char *pcPath, unsigned long *pulMaxIdx);


/* 设置节点最大的已经使用的实例序号 */
/***********************************************************
接口:   获取当前对象集合下已使用过的最大的实例编号
参数:   pcPath, 节点路径
        ulMaxIdx, 要设置成的最大编号
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_SetNodeMaxIdx(const char *pcPath, unsigned long ulMaxIdx);


/* 取得节点子树个数 */
/***********************************************************
接口:   获取节点下一层子节点个数
参数:   pcPath, 节点路径
       pulCount, 输出个数
返回值: 0:成功
        其它:失败
***********************************************************/
CFG_RET CFG_GetNodeChildCount(const char *pcPath, unsigned int *pulCount);




/* 非递归遍历接口, 取下一个节点 */
/*************************************************************************
接口: 非递归遍历下一个节点
参数:    pvPath, 当前节点指针
        pusMID, 输出节点所属的mid
返回值:   非NULL表示下一个节点的指针
          NULL表示便历结束
备注:
*************************************************************************/
void *CFG_GetNextNode(void *pvPath, unsigned short *pusMID);


/* 恢复出厂设置 */
CFG_RET CFG_RecoverFactSet(const char *pcAccessor);

/* 清除当前配置 */
CFG_RET CFG_ClearCfg(void);

#ifdef CONFIG_APPS_LOGIC_WANSELECT
CFG_RET CFG_ClearAllCfg(void);
#endif

/* 注册通知报告的接口 */
CFG_RET CFG_RegNotiInformIntf(FUNC_NOTI_INFORM pfnNotiInform);

/* 设置非递归遍历MID的基本信息 */
CFG_RET CFG_SetMIDAccessInfo(const char *pcPath, const void **ppvTree,
           unsigned short *pusMID, const char *pcAccessor,
           char **ppcPathSave);

/* 非递归遍历MID, 取下一个MID */
const void *CFG_GetNextMID(const void *pvPrev, unsigned short *pusMID,
                                    unsigned char *pucLen);

/* 检查删除节点的合法性 */
CFG_RET CFG_CheckAndSetNodeDeleting(const char *pcPath, const char *pcAccessor);

/* 设置节点属性值的接口 */
CFG_RET CFG_SetNodeAttrVal(const char *pcPath,
         const char *pcAttr, const char *pcVal);

#define CFG_DATA_TREE 0
#define CFG_LOGIC_TREE 1




#ifdef _CFG_DEBUG
CFG_RET CFG_PrintNode(const void *pvTree, const char *pcPath,
                      unsigned long ucLen, void *pvFile);
#endif






/* 将配置树实例直接转化成一个结构 */
CFG_RET CFG_GetValToStruct(const char *pcPath, void *pvStruct,
                           char * const *ppcLeafList,
                           unsigned long ulLeafNum);

/* 通过节点路径获取一个节点列表值到一个结构体中, 所有节点值按string类型输出 */
CFG_RET CFG_GetValToStrArr(const char *pcPath, void *pvStrArr,
                           char * const *ppcLeafList,
                           unsigned long ulLeafNum);


/* 将结构体中的值设置到配置树实例中 */
CFG_RET CFG_SetValFromStruct(const char *pcPath, const void *pvStruct,
                           char * const *ppcLeafList,
                           unsigned long ulLeafNum);

/***********************************************************
接口:   通过节点路径设置一个节点列表值, 值来源于一个字符数组
参数:   pcPath, 节点路径
        pvStrArr, 设置值的来源
        ppcLeafList, 节点列表, 节点名为"" 的将被忽略
        ulLeafNum, 节点个数, 实际上是指 ppcLeafList 中的元素个数
返回值: 0:成功
        其它: 失败
备注:
***********************************************************/
CFG_RET CFG_SetValFromStrArr(const char *pcPath, void *pvStrArr,
                           char * const *ppcLeafList,
                           unsigned long ulLeafNum);

/* 列出配置树中指定MID的所有节点 */
CFG_RET CFG_ListMIDAllNodes(unsigned short usMID, ST_PARA_VAL **ppstParaVal);

/* 新建一个对象实例 */
CFG_RET CFG_AddObjInst(const char *pcPath, unsigned long *pulIndex);

/* 获取一个节点的值的地址, 和类型地址 */
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

/* 获取一个通配路径下的所有实例的结构链表 */
CFG_RET CFG_GetPathInstList(const char *pcMatchPath, char * const *ppcLeafList,
                           unsigned long ulLeafNum,
                           ST_CFG_INST_NODE **ppstInstList);

/* 通过通配路径和index列表获取一个对象下叶子节点值到一个结构体中 */
CFG_RET CFG_GetValToStructEx(const char *pcMatchPath,
                           const unsigned long *pulIndexList,
                           void *pvStruct, char * const *ppcLeafList,
                           unsigned long ulLeafNum);

/* 通过节点通配路径设置一个节点列表值, 值来源于一个结构体 */
CFG_RET CFG_SetValFromStructEx(const char *pcMatchPath,
                           const unsigned long *pulIndexList,
                           const void *pvStruct, char * const *ppcLeafList,
                           unsigned long ulLeafNum);

/* 获取对象下的mid列表, 该函数是前面两个函数的封装 */
CFG_RET CFG_GetObjMIDList(unsigned char ucTree, const char *pcPath,
                unsigned short *pusMIDList, unsigned long *pulCount);

CFG_RET CFG_GetNodeMIDFromFullTree(const char *pcPath, unsigned short *pusMIDList,
                                   unsigned long  *pulCount);

/* 设置节点名称 */
CFG_RET CFG_SetNodeName(const char *pcPath, const char *pcNewName);

/* 通过节点地址设置节点名称 */
CFG_RET CFG_SetNodeNameByPtr(void *pvNode, const char *pcNewName);

/* 将节点路径对应的子树切下来 */
CFG_RET CFG_DelNodeToPtr(const char *pcPath, void **ppvNode);

/* 将子树添加到指定的路径下 */
CFG_RET CFG_AddNodeFromPtr(const char *pcPath, void *pvNode);

/* 为列举指定mid的所有节点而做的准备工作 */
void CFG_MIDInstListBegin(unsigned short usMID);

/* 为列举指定mid的所有节点而做的准备工作, 该函数可以指定路径 */
void CFG_MIDInstListBeginEx(unsigned short usMID, const char *pcPath);

/* 遍历函数, 获取下一个对象下 属于指定mid的节点列表 */
CFG_RET CFG_ListMIDNextInstNodes(ST_PARA_VAL **ppstParaVal);

/* 获取通配路径的叶子节点列表 */
CFG_RET CFG_GetMatchNodeName(const char *pcMatchPath,
                             unsigned char ucNextLevel,
                             ST_PARA_VAL **ppstParaInfo, const char *pcAccessor);

/* 获取节点属性值指针 */
CFG_RET CFG_GetNodeAttrValPtr(const char *pcPath, const char *pcAttr,
                              const char **ppcVal);

/* 对节点值进行增加指定的值, 只对int类节点有意义 */
CFG_RET CFG_IncIntNodeVal(const char *pcPath, long lVal);


#define CFG_LAST_NODE  0xfffffffe

/* 根据节点顺序获取节点名称 */
CFG_RET CFG_GetNodeOrderName(const char *pcPath, unsigned long ulOrder,
                             ST_PARA_VAL **pstPara);

/* 清除通配路径对象节点下的实例 */
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

