/****************************************************************************
 �ļ���  : CLI_Init.c
 ����    : liuzequn
 �汾    :
 �������:
 �ļ�����:  ���ļ�ʵ��������ģ����Ҫ���ֵĳ�ʼ������
 �޸���ʷ:
        1. �޸���   :
           ʱ��     :
           �汾     :
           �޸�ԭ�� :
 ****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "cli_private.h"
#include "cli_term.h"
#include "cli_interpret.h"
#include "cli_io.h"
#include "../app/cli_cmd.inc"
#include "cli_cmdreg.h"
#include "../cli_comm.h"
#include <signal.h>

char               m_szHostName[32] = "TBS";             /* �����豸��     */

// ģʽע����Ϣ
ST_CMD_MODE_INFO m_stCmdModeInfo[] =
  /* ģʽ       ��ģʽ       Ȩ��           ģʽ��      ��ʾ��       ��������ִ�в����������Ϣ */
{{CTM_GLOBAL,  NULL_MODE,  CLI_AL_QUERY,   "Global",   ">>"       , NULL, CLI_ML_NULL},
 {CTM_GENL,    NULL_MODE,  CLI_AL_QUERY,   "Root",     ">>"       , NULL, CLI_ML_NULL},
 {CTM_CONFIG,  CTM_GENL,   CLI_AL_QUERY,   "Config",   ">>config>", NULL, CMDHELP_GENL_CM_CONFIG},

 // TODO: �˴���Ӹ�������ģʽ

 {NULL_MODE}
};

/*********************************************************************/
/* �������� : CLI_ProcessInit()                                      */
/* �������� : ģ��ֶγ�ʼ������                                     */
/* ������� :                                                        */
/* ������� :                                                        */
/* ����     :                                                        */
/* �ϲ㺯�� :                                                        */
/* ������   :                                                        */
/* �޸ļ�¼ :                                                        */
/*********************************************************************/
ULONG  CLI_ProcessInit()
{
    ULONG  ulRet = TBS_SUCCESS;

    signal(SIGINT, CLI_ProcForCtrlC);

    //ulRet += CLI_CommInit();

    /* ģʽ�ṹ��ʼ�� */
    CLI_InitSysModes();
    /* �ն�������ʼ��*/
    CLI_TermDataInit();
    /* ����������ʼ��  */
    ulRet += CLI_ResetEnviroment();

    /* ע������ģʽ��ģʽ���� */
    ulRet += CLI_InitCmdTree(m_stCmdModeInfo);

    /* --------��ģ������ע�Ὺʼλ��--------{                  */

	/* ע�᱾ģ������ */
    ulRet += CLI_SysCmdReg();

    // TODO: ��ģ������ע��

    /* --------��ģ������ע�����λ��--------}                  */


    /* ��ȫ���������ӵ�����ģʽ */
    ulRet += CLI_GlobalCmdLink();

    return ulRet;
}


void CLI_ProcessDestory()
{
    CLI_TermDestroy();
}

/*********************************************************************/
/* �������� : CLI_GetHostName()                                      */
/* �������� : �����ṩ����������ȡ����                               */
/* ������� :                                                        */
/* ������� :                                                        */
/* ����     : �������ַ���                                           */
/*********************************************************************/
char  *CLI_GetHostName(_VOID   )
{
    return m_szHostName;
}


#ifdef __cplusplus
}
#endif

