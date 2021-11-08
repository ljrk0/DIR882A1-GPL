
/******************************************************************************
  Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.

  �ļ�����: devctl_api.h
  �ļ�����: ���ն������ں�ģ��API
  �����б�:
  �޶���¼:
         1. ����: ������
            ����: 2009-3-31
            ����: �����ļ�
 ******************************************************************************/

#ifndef __TBS_DEVCTL_API_H__
#define __TBS_DEVCTL_API_H__

#include <devctl_types.h>


/* �����豸��Ϣ */
int dc_add_devinfo(const char *pszIP, const char *pszMac, const char *pszDevType);

/* ɾ���豸��Ϣ */
int dc_del_devinfo(EN_DC_FilterType enFilterType, const void *pData);

/* ��ѯ�豸��Ϣ */
int dc_query_devinfo(EN_DC_FilterType enFilterType, const void *pData,
    ST_DC_DeviceInfo *pstDevList, ulong *pnCount);

/* ���ö��ն�����ģ����� */
int dc_config(ST_DC_Config *pstConfig);


#endif /*__TBS_DEVCTL_API_H__*/

