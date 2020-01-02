/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : om.h
 文件描述 : OM子系统公共头文件


 函数列表 :


 修订记录 :
          1 创建 : 刘泽群
            日期 : 2007-9-18
            描述 :

**********************************************************************/


#ifndef __OM_H__
#define __OM_H__

#include "tbsmsg.h"
#include "tbstype.h"
#include "tbserror.h"
/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/
#define OM_USER_NAME_LEN       32       // 用户名长度
#define OM_PASSWORD_LEN        32       // 密码长度
#define OM_ACTION_LEN          16       // 操作命令长度
#define OM_SESSION_ID_LEN      16       // sessionID长度


#define OM_PATH_MAX             128     // 路径最大长度



#define OM_NODE_MAX             16      // 最大节点个数，如: aaa.bbbb.ccccc的个数就是3
#define OM_NODE_LEN             64      // 单个最大长度，如: 上面ccccc的节点长度是5
#define OM_NODE_TOKEN           '.'
#define RESPONSE_FLAG           0x8000  // 消息类型最高位标识回应消息
typedef const char * PCSTR;
typedef char *       PSTR;



#define OM_ASSERT(express)//  assert(express)

#define OM_FAILED(ret)   (ret != TBS_SUCCESS)
#define OM_SUCCEED(ret)   (ret == TBS_SUCCESS)
/******************************************************************************
 *                                 ENUM                                       *
 ******************************************************************************/






/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/

// 用于对查询消息的反馈，反馈一系列的值
typedef struct tag_ST_OM_VAL_RESP
{
    unsigned char   ucObjNum;           // 对象的数量
    unsigned char   ucValNum;           // 每个对象中值的个数

    char    szObject[0];            // 其的内容为连续的 ST_OM_MSG_VAL
} ST_OM_VAL_RESP;

// 用于对查询消息的反馈的值单元
typedef struct tag_ST_OMMSG_VAL
{
    unsigned short usType;  // 类型
    unsigned short usLen;   // 这个长度是ucBuffer内容的长度
    char   ucBuffer[0];     // 字符串型的值
} ST_OM_MSG_VAL;




/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/

/*=========================================================================*/
/*  函数名称: Web_MsgRequestParse                                          */
/*  函数功能: 来自web模块的请求消息的解析                                  */
/*  输入参数: pstMsg: 接收到的消息                                         */
/*  输出参数: pszName、pszVal, 解析后的name和val的指针                     */
/*  创  建  : 刘泽群 / 2007-9-29                                           */
/*  说  明  : 消息体形如"name=val, 本接口将name和val的指针分别赋予pszName和*/
/*            pszVal， 并将'='改为'\0'                                     */
/*=========================================================================*/
int WEB_MsgRequestParse(ST_MSG *pstMsg, char **pszName, char **pszVal);

/*=========================================================================*/
/*  函数名称: WEB_MsgRespAddOne                                            */
/*  函数功能: 在发往WEB模块的响应消息中增加一个值                          */
/*  输入参数: nObjNum/nVarNum: 响应消息中对象个数和值的个数                */
/*            strVal         : 响应消息中新加入的值                        */
/*  输出参数: pstMsg         : 增加值后的消息包                            */
/*  返回值  : 成功/失败                                                    */
/*  创  建  : 刘泽群 / 2007-9-29                                           */
/*  注  意  : 本函数不检查pstMsg消息体的内存长度，调用者需自行检查         */
/*=========================================================================*/
int WEB_MsgRespAddOne(ST_MSG *pstMsg, unsigned char nObjNum, unsigned char nVarNum, PCSTR strVal);

/*=========================================================================*/
/*  函数名称: WEB_MsgRespSetOne                                            */
/*  函数功能: 在发往WEB模块的响应消息中设置一个单值                        */
/*  输入参数: strVal         : 响应消息中设置的值                          */
/*  输出参数: pstMsg         : 增加值后的消息包                            */
/*  返回值  : 成功/失败                                                    */
/*  创  建  : 刘泽群 / 2007-9-29                                           */
/*  注  意  : 本函数不检查pstMsg消息体的内存长度，调用者需自行检查         */
/*=========================================================================*/
int WEB_MsgRespSetOne(ST_MSG *pstMsg, PCSTR strVal);





/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/


#endif /* __OM_H__ */



