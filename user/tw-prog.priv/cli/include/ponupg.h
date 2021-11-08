
#ifndef __PONUPG_H__
#define __PONUPG_H__

#define DL_PATH                     "/var/"
#define DL_IMAGE_FILE       "image.img"
#define DL_GATEWAY_IMAGE "gwimage"
#define DL_PON_IMAGE    "ponimage"


/*********************************************************************
*  ����: �������ļ���ȡ��һ��������
*  ����:
*  ����: char *
*  ����: huangjidong
*  ��ע: ���ص��ַ�����Ҫ�û������ͷ�
*********************************************************************/
char *tbsReadFileToBufWithMalloc(const char *pcFileName, int *piLen);
/*********************************************************************
*  ����: ������ϵͳ��������
*  ����: pcPath     ִ��������·��������/var
         pcPack     �����������ƣ�����fullpack
         pcGwImage  �����������������ļ������ƣ�����image.img
         pcPonImage ��������PON�����ļ������ƣ�����firmware.uue
*  ����: int
         -4 --- ���ļ�������
         -3 --- PON����ʧ��
         -2 --- ��������ʧ��
         -1 --- ����������
         0  --- �����ɹ�
*  ����: huangjidong
*  ��ע: �������ļ��ָ������---ʵ��PON�����ص�ͬʱ����
*********************************************************************/
int tbsCommonUpgrader(const char *pcPath, const char *pcPack, const char *pcGwImage, const char *pcPonImage,
    const char *pcUser, const char *pcClearFlag);

#endif

