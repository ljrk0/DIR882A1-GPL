/****************************************************************************
 �ļ���  : CLI_Config.h
 ����    : liuzequn
 �汾    :
 �������:
 �ļ�����: ������ģ���������ѡ��
 �޸���ʷ:
        1. �޸���   :
           ʱ��     :
           �汾     :
           �޸�ԭ�� :
 ****************************************************************************/
#ifndef _CLI_CONFIG_H_
#define _CLI_CONFIG_H_
//#include "cli_private.h"

#define CLI_YES     1
#define CLI_NO      0

#define CLI_DEBUG_ON            CLI_YES   //�Ƿ���԰汾
//#define CLI_DEBUG_ON            CLI_NO

#define LOG_SAVE_FLASH          CLI_NO    //�Ƿ�ʹ��FLASH������־�洢

#define DBG_USER_PASSWORD       "root"            //�����û�����

#define UNLOGON_USER_NAME       "unlogon user"

/*��Գ�ʱ�������õ����ֳ�ʱʱ�� ��λ:��*/
#define DEADLINE_LONG           7200
#define DEADLINE_SHORT          300

#endif

