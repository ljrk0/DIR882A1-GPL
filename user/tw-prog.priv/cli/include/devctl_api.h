
/******************************************************************************
  Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: devctl_api.h
  文件描述: 多终端上网内核模块API
  函数列表:
  修订记录:
         1. 作者: 匡素文
            日期: 2009-3-31
            内容: 创建文件
 ******************************************************************************/

#ifndef __TBS_DEVCTL_API_H__
#define __TBS_DEVCTL_API_H__

#include <devctl_types.h>


/* 更新设备信息 */
int dc_add_devinfo(const char *pszIP, const char *pszMac, const char *pszDevType);

/* 删除设备信息 */
int dc_del_devinfo(EN_DC_FilterType enFilterType, const void *pData);

/* 查询设备信息 */
int dc_query_devinfo(EN_DC_FilterType enFilterType, const void *pData,
    ST_DC_DeviceInfo *pstDevList, ulong *pnCount);

/* 配置多终端上网模块参数 */
int dc_config(ST_DC_Config *pstConfig);


#endif /*__TBS_DEVCTL_API_H__*/

