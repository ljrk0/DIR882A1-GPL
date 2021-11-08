/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 �ļ����� : pc_api.h
 �ļ����� : pcģ���ṩ��api�ӿ�ͷ�ļ�
 �����б� :

 �޶���¼ :
          1 ���� : ��Ծ��
            ���� : 2007-11-15
            ���� :

**********************************************************************/

#ifndef __PC_API_H__
#define __PC_API_H__

#include <signal.h>
#include "tbstype.h"
#include "tbsmsg.h"
#include "tbserror.h"

#ifdef _cplusplus
    #if _cplusplus
        extern "C" {
    #endif
#endif

#define _PC_VER_R1

/* �ź�ֵ���� */
#define TBS_SIGSTART  (unsigned char)0
#define TBS_SIGHUP    (unsigned char)SIGHUP
#define TBS_SIGINT    (unsigned char)SIGINT
#define TBS_SIGQUIT   (unsigned char)SIGQUIT
#define TBS_SIGILL    (unsigned char)SIGILL
#define TBS_SIGTRAP   (unsigned char)SIGTRAP
#define TBS_SIGABRT   (unsigned char)SIGABRT
#define TBS_SIGEMT    (unsigned char)SIGEMT
#define TBS_SIGFPE    (unsigned char)SIGFPE
#define TBS_SIGKILL   (unsigned char)SIGKILL
#define TBS_SIGBUS    (unsigned char)SIGBUS
#define TBS_SIGSEGV   (unsigned char)SIGSEGV
#define TBS_SIGSYS    (unsigned char)SIGSYS
#define TBS_SIGPIPE   (unsigned char)SIGPIPE
#define TBS_SIGALRM   (unsigned char)SIGALRM
#define TBS_SIGTERM   (unsigned char)SIGTERM
#define TBS_SIGURG    (unsigned char)SIGURG
#define TBS_SIGSTOP   (unsigned char)SIGSTOP
#define TBS_SIGTSTP   (unsigned char)SIGTSTP
#define TBS_SIGCONT   (unsigned char)SIGCONT
#define TBS_SIGCHLD   (unsigned char)SIGCHLD
#define TBS_SIGTTIN   (unsigned char)SIGTTIN
#define TBS_SIGTTOU   (unsigned char)SIGTTOU
#define TBS_SIGIO     (unsigned char)SIGIO
#define TBS_SIGXCPU   (unsigned char)SIGXCPU
#define TBS_SIGXFSZ   (unsigned char)SIGXFSZ
#define TBS_SIGVTALRM (unsigned char)SIGVTALRM
#define TBS_SIGPROF   (unsigned char)SIGPROF
#define TBS_SIGWINCH  (unsigned char)SIGWINCH
#define TBS_SIGINFO   (unsigned char)SIGINFO
#define TBS_SIGUSR1   (unsigned char)SIGUSR1
#define TBS_SIGUSR2   (unsigned char)SIGUSR2

#define TBS_SIGNULL   (unsigned char)(0xff)

#define PC_IS_KILL_SIG(sig) (TBS_SIGKILL == (sig) || TBS_SIGTERM == (sig))


/* ��������ʱ����������: ���ֲ��� */
#define PC_NO_CHANGE           0xff




/* pc�ӿڷ���ֵ���Ͷ��� */
typedef int   PC_RET;


#define PC_OK             TBS_SUCCESS
#define PC_FAIL           TBS_FAILED

#define ERR_PC_PARA       ERR_PARA_INVALID
#define ERR_PC_MALLOC     ERR_MALLOC_FAILED



/* ģ�����ṩ�Ľ��̲����ӿڵ����� */
typedef int (*FUNC_PROC_CTRL)(const char *pcName, unsigned long ulInstNo,
                              unsigned char ucProcOpt);

/* ״̬��������״̬,
   ����ģ��һ��ֻ��ע PC_STATE_STOPPED �� PC_STATE_STARTED */
enum
{
    PC_STATE_NULL,

    PC_STATE_STOPPED,
    PC_STATE_WAIT_START,
    PC_STATE_STARTED,
    PC_STATE_WAIT_STOP,
    PC_STATE_RESTART
};


/* ״̬���¼�,
PC_EVENT_RESTART �� PC_EVENT_STOP ����ģ�鷢��
�����¼�����ͨ������pc����������Ϣ����
*/
enum
{
   PC_EVENT_NULL,

   PC_EVENT_RESTART,
   PC_EVENT_STOP,

   PC_EVENT_START_OK,
   PC_EVENT_START_FAIL,

   PC_EVENT_STOP_OPT_OK,
   PC_EVENT_STOP_OPT_FAIL,

   PC_EVENT_RESTART_OPT_OK,
   PC_EVENT_RESTART_OPT_FAIL,

   PC_EVENT_RESTART_OK,
   PC_EVENT_RESTART_FAIL,

   PC_EVENT_EXIT
};

/* ģ�����ṩ�Ľ��̽ӿڵ����ֲ�����ʽ */
enum
{
    PC_OPT_NULL,

    PC_OPT_START,
    PC_OPT_STOP,
    PC_OPT_RESTART
};




#define PC_PROC_NAME_LEN   16

/* ���� �� �������̵Ĳ����ṹ */
typedef struct
{
    char acName[PC_PROC_NAME_LEN]; /* ������ */
    unsigned long ulInstNo;        /* ʵ���� */

    unsigned char ucSignal;        /* �ź�ֵ, ΪTBS_SIGSTART ʱ
                                      ��ʾ������һ������ */
    unsigned char ucNeedRestart;   /* ��������ʱ, ��ʾ�����˳����Ƿ����� */
    unsigned char  ucNeedSteady;   /* �����Ƿ���Ҫ�־û����� */
    unsigned char  ucExecCount;    /* ִ���ļ����Ͳ��� ����, ��������ʱ,
                                      ���Ϊ0xff,��ʾ����ԭ�������� */
    char **ppcExecInfo;            /* ִ����Ϣ */
    unsigned char  ucMonCount;     /* �����������, ��������ʱ,���Ϊ0xff,
                                      ��ʾ����ԭ����������� */
    unsigned char  aucSpace[3];    /* ���ֽڶ��� */
    char **ppcMonVals;             /* ������� */

} ST_PC_PROC_CTRL_INFO;


/* ��PC�����������Ϣ��ʽ */
typedef struct
{
    char acName[PC_PROC_NAME_LEN];
    unsigned long ulInstNo;
    unsigned char ucSignal;
    unsigned char ucNeedRestart;
    unsigned char ucNeedSteady;
    unsigned char ucExecCount;
    /* unsigned char ucMonCount; */

} ST_PC_PROC_CTRL_MSG;


/* pc����Ӧ�����Ϣ��ʽ */
typedef struct
{
    char acName[PC_PROC_NAME_LEN];
    unsigned long ulInstNo;
    unsigned char ucSignal;
    unsigned char ucNeedRestart;
    unsigned char aucSpace[2];
    unsigned long ulRet;

} ST_PC_REPLY_MSG;


/* pc���̱������״̬����Ϣ��ʽ */
/*lint -save -e46*/
typedef struct
{
    char acName[PC_PROC_NAME_LEN];
    unsigned long ulInstNo;

    unsigned char ucProcState;
    unsigned char ucExitSignal;
    unsigned char ucLastSignal;
    unsigned char ucNeedSteady : 4;
    unsigned char ucWillRestart : 4;

    unsigned long ulRestartRet;

} ST_PC_STATE_MSG;
/*lint -restore*/


/* ����ʵ���ṹ */
typedef struct tag_ST_PC_PROC_STATE
{
    struct tag_ST_PC_PROC_STATE *pstNext;

    char acName[PC_PROC_NAME_LEN];
    unsigned long  ulInstNo;
    unsigned char  ucState;
    unsigned char  aucSpace[3];

} ST_PC_PROC_INST;



#define PC_PROC_ID_LEN  (PC_PROC_NAME_LEN + sizeof(unsigned long))


/*************************************************************************
Function:      PC_RET PC_SendProcessSignal(unsigned long ulProcID,
                     unsigned char ucSignal, unsigned char ucNeedReatart,
                     const ST_PC_PROC_START *pstStartInfo)
Description:   �����̷��ź�
Calls:
Data Accessed:
Data Updated:
Input:         ulProcID, Ҫ���ʵ�ProcID
               ucSignal, �ź�ֵ
               ucNeedReatart, �Ƿ���Ҫ����
               pstStartInfo, ���������ṹ, ����ο��ṹ���崦ע��
Output:        ��
Return:        0,�ɹ�;
               ����, ʧ��
Others:
*************************************************************************/
PC_RET PC_ProcCtrl(unsigned short usMID,
                            const ST_PC_PROC_CTRL_INFO *pstProcCtrlInfo);

/* ����״̬������ */
PC_RET PC_StateMachine(const ST_MSG *pstPcMsg,
                    const char *pcName, unsigned long ulInstNo,
                    FUNC_PROC_CTRL pfnProcCtrl, unsigned char ucEvent,
                    unsigned char *pucState);

PC_RET PC_GetProcInstState(const char *pcName, unsigned long ulInstNo,
                           unsigned char *pucState);

/* ��ӡ��������������Ϣ */
void PC_PrintExecInfo(char *apszExecInfo[], int iExecInfoLen);


#ifdef _PC_VER_R1

/* ������id */
#define PC_MAX_PROC_ID         0xfe


/* ���� �� �������̵Ĳ����ṹ */
typedef struct
{
    unsigned long  ulMsgID;
    unsigned short usMID;          /* ģ��id */
    unsigned char  ucNeedSteady;   /* �����Ƿ���Ҫ�־û����� */
    unsigned char  ucExecCount;    /* ִ���ļ����Ͳ��� ����, ��������ʱ,
                                      ���Ϊ0xff,��ʾ����ԭ�������� */
    char **ppcExecInfo;            /* ִ����Ϣ */
    unsigned char  ucMonCount;     /* �����������, ��������ʱ,���Ϊ0xff,
                                      ��ʾ����ԭ����������� */
    unsigned char  aucSpace[3];    /* ���ֽڶ��� */
    char **ppcMonVals;             /* ������� */

} ST_PC_PROC_START;



/*************************************************************************
Function:      PC_RET PC_StartProcess(const ST_PC_PROC_START *pstStartInfo)
Description:   ͨ��PC�������̵�API
Calls:         MSG_SendMessage
Data Accessed:
Data Updated:
Input:         pstStartInfo, ���������ṹ, ����ο��ṹ���崦��ע��
Output:        ��
Return:        0,�ɹ�;
               ����, ʧ��
Others:
*************************************************************************/
PC_RET PC_StartProcess(const ST_PC_PROC_START *pstStartInfo);

/*************************************************************************
Function:      PC_RET PC_SendProcessSignal(unsigned long ulProcID,
                     unsigned char ucSignal, unsigned char ucNeedReatart,
                     const ST_PC_PROC_START *pstStartInfo)
Description:   �����̷��ź�
Calls:
Data Accessed:
Data Updated:
Input:         ulProcID, Ҫ���ʵ�ProcID
               ucSignal, �ź�ֵ
               ucNeedReatart, �Ƿ���Ҫ����
               pstStartInfo, ���������ṹ, ����ο��ṹ���崦ע��
Output:        ��
Return:        0,�ɹ�;
               ����, ʧ��
Others:
*************************************************************************/
PC_RET PC_SendProcessSignal(unsigned long ulProcID, unsigned char ucSignal,
        unsigned char ucNeedReatart, const ST_PC_PROC_START *pstStartInfo);



#endif









#ifdef _cplusplus
    #if _cplusplus
        }
    #endif
#endif


#endif /* __PC_API_H__ */
