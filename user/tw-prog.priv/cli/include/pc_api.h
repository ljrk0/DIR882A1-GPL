/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : pc_api.h
 文件描述 : pc模块提供的api接口头文件
 函数列表 :

 修订记录 :
          1 创建 : 陈跃东
            日期 : 2007-11-15
            描述 :

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

/* 信号值定义 */
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


/* 重启进程时的特殊意义: 保持不变 */
#define PC_NO_CHANGE           0xff




/* pc接口返回值类型定义 */
typedef int   PC_RET;


#define PC_OK             TBS_SUCCESS
#define PC_FAIL           TBS_FAILED

#define ERR_PC_PARA       ERR_PARA_INVALID
#define ERR_PC_MALLOC     ERR_MALLOC_FAILED



/* 模块所提供的进程操作接口的类型 */
typedef int (*FUNC_PROC_CTRL)(const char *pcName, unsigned long ulInstNo,
                              unsigned char ucProcOpt);

/* 状态机的所有状态,
   其中模块一般只关注 PC_STATE_STOPPED 和 PC_STATE_STARTED */
enum
{
    PC_STATE_NULL,

    PC_STATE_STOPPED,
    PC_STATE_WAIT_START,
    PC_STATE_STARTED,
    PC_STATE_WAIT_STOP,
    PC_STATE_RESTART
};


/* 状态机事件,
PC_EVENT_RESTART 和 PC_EVENT_STOP 是由模块发起
其他事件都是通过分析pc发送来的消息产生
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

/* 模块所提供的进程接口的三种操作方式 */
enum
{
    PC_OPT_NULL,

    PC_OPT_START,
    PC_OPT_STOP,
    PC_OPT_RESTART
};




#define PC_PROC_NAME_LEN   16

/* 启动 或 重启进程的参数结构 */
typedef struct
{
    char acName[PC_PROC_NAME_LEN]; /* 进程名 */
    unsigned long ulInstNo;        /* 实例号 */

    unsigned char ucSignal;        /* 信号值, 为TBS_SIGSTART 时
                                      表示新启动一个进程 */
    unsigned char ucNeedRestart;   /* 用在重启时, 表示进程退出后是否重启 */
    unsigned char  ucNeedSteady;   /* 进程是否需要持久化运行 */
    unsigned char  ucExecCount;    /* 执行文件名和参数 个数, 用在重启时,
                                      如果为0xff,表示保持原参数不变 */
    char **ppcExecInfo;            /* 执行信息 */
    unsigned char  ucMonCount;     /* 监控条件个数, 用在重启时,如果为0xff,
                                      表示保持原监控条件不变 */
    unsigned char  aucSpace[3];    /* 四字节对齐 */
    char **ppcMonVals;             /* 监控条件 */

} ST_PC_PROC_CTRL_INFO;


/* 向PC进程请求的消息格式 */
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


/* pc进程应答的消息格式 */
typedef struct
{
    char acName[PC_PROC_NAME_LEN];
    unsigned long ulInstNo;
    unsigned char ucSignal;
    unsigned char ucNeedRestart;
    unsigned char aucSpace[2];
    unsigned long ulRet;

} ST_PC_REPLY_MSG;


/* pc进程报告进程状态的消息格式 */
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


/* 进程实例结构 */
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
Description:   给进程发信号
Calls:
Data Accessed:
Data Updated:
Input:         ulProcID, 要访问的ProcID
               ucSignal, 信号值
               ucNeedReatart, 是否需要重启
               pstStartInfo, 重启参数结构, 具体参考结构定义处注释
Output:        无
Return:        0,成功;
               其它, 失败
Others:
*************************************************************************/
PC_RET PC_ProcCtrl(unsigned short usMID,
                            const ST_PC_PROC_CTRL_INFO *pstProcCtrlInfo);

/* 进程状态机函数 */
PC_RET PC_StateMachine(const ST_MSG *pstPcMsg,
                    const char *pcName, unsigned long ulInstNo,
                    FUNC_PROC_CTRL pfnProcCtrl, unsigned char ucEvent,
                    unsigned char *pucState);

PC_RET PC_GetProcInstState(const char *pcName, unsigned long ulInstNo,
                           unsigned char *pucState);

/* 打印进程启动参数信息 */
void PC_PrintExecInfo(char *apszExecInfo[], int iExecInfoLen);


#ifdef _PC_VER_R1

/* 最大进程id */
#define PC_MAX_PROC_ID         0xfe


/* 启动 或 重启进程的参数结构 */
typedef struct
{
    unsigned long  ulMsgID;
    unsigned short usMID;          /* 模块id */
    unsigned char  ucNeedSteady;   /* 进程是否需要持久化运行 */
    unsigned char  ucExecCount;    /* 执行文件名和参数 个数, 用在重启时,
                                      如果为0xff,表示保持原参数不变 */
    char **ppcExecInfo;            /* 执行信息 */
    unsigned char  ucMonCount;     /* 监控条件个数, 用在重启时,如果为0xff,
                                      表示保持原监控条件不变 */
    unsigned char  aucSpace[3];    /* 四字节对齐 */
    char **ppcMonVals;             /* 监控条件 */

} ST_PC_PROC_START;



/*************************************************************************
Function:      PC_RET PC_StartProcess(const ST_PC_PROC_START *pstStartInfo)
Description:   通过PC启动进程的API
Calls:         MSG_SendMessage
Data Accessed:
Data Updated:
Input:         pstStartInfo, 启动参数结构, 具体参考结构定义处的注释
Output:        无
Return:        0,成功;
               其它, 失败
Others:
*************************************************************************/
PC_RET PC_StartProcess(const ST_PC_PROC_START *pstStartInfo);

/*************************************************************************
Function:      PC_RET PC_SendProcessSignal(unsigned long ulProcID,
                     unsigned char ucSignal, unsigned char ucNeedReatart,
                     const ST_PC_PROC_START *pstStartInfo)
Description:   给进程发信号
Calls:
Data Accessed:
Data Updated:
Input:         ulProcID, 要访问的ProcID
               ucSignal, 信号值
               ucNeedReatart, 是否需要重启
               pstStartInfo, 重启参数结构, 具体参考结构定义处注释
Output:        无
Return:        0,成功;
               其它, 失败
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
