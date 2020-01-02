/****************************************************************************
 文件名  : CLI_Term.h
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述: 命令行模块大部分子功能需要用到的定义与声明。私有，对其它模块不提供
 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
 ****************************************************************************/
#ifndef _CLI_TERM_H_
#define _CLI_TERM_H_
#include "../cli_config.h"
#include "cli_private.h"
#include "cli_cmdreg.h"
#include "cli_interpret.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define CLI_ML_NULL     0

#define CLI_PID_NULL        0xffff      /*ddddd*/

/* 延时，单位为1/100秒 */
#define CLI_DELAY(s)   usleep(s*100000)


/* -------------------------------- 类型定义------------------------------ */


/* -------------------------------- 常量宏、枚举定义------------------------------ */

#define HISTORY_SIZE        10      /* 历史命令个数      */
#define USERNAME_LEN        32      /* 用户名最大长度    */
#define PASSWORD_LEN        USERNAME_LEN
#define HOSTNAME_LENGTH     16      /* 主机名最大长度    */
#define CLI_MAX_CMDBUF       256     /* 命令缓冲最大长度  */
#define TERM_TIMEOUT        300     /* 接收的超时长度定义*/
#define TELNET_ACL_NUM      10      /* telnet访问控制表大小*/

#define FIRST_TELNET_DATA   2       /* 第一个telnet数据的位置*/
#define MAX_USER_NUM        16      /* 操作用户最大个数  */
#define MAX_CLIENT_NUM      0

#define MAX_ONLINE_USER     (MAX_CLIENT_NUM + 2)


#define TERM_IO_BUFFER      1024    /* 终端发送与接收缓冲大小 */
#define TERM_OUT_FOR_RESPONSE 24    /* 预留给从命令执行反馈的空间*/

//support special char for telefonica spain
#define TELEFONICA_SPAIN_TEST
#define SPECIAL_CHAR_BEGIN 0x80

#define CLI_INVALID         (ULONG )(-1)

/* 定义最大重试次数 */
#define MAX_FAILED_LOG_TIME     3

/* 超时开关定义     */
#define TIMEOUT_ON     0
#define TIMEOUT_OFF    1

/* 语言模式定义     */
#define ENGLISH_L        0
#define CHINESE_L        1

/* 帮助模式开关定义 */
#define CTHELPMODEON     0
#define CTHELPMODEOFF    1

/* 自动、手动分屏开关 */
#define SCROLL_MANUAL    1
#define SCROLL_AUTO      0

/* 交互允许开关定义 */
#define SMART_ON         0
#define SMART_OFF        1

/* telnet 端口号定义            */
#define TELNETPORT      23
/* 终端socket在非telnet状态下的属性   */
#define CLI_TERM_FOR_SERIAL       -1
#define CLI_TERMT_FOR_MML          -2
#define CLI_TERM_NOT_INITIAL      -3

/* 终端任务的运行状态定义        */
enum  Term_Status_E
{
    TERM_SLEEPING = 0,
    TERM_ACTIVE,
    TERM_LOGED,
    TERM_EXECCMD,
    TERM_WAIT_RESPONSE,

    TELNET_MAX_STATUS
} ;

/* -------------------------------- 宏操作定义 ------------------------------ */

/* 有界索引值加1、减1宏定义 */
#define CLI_INCCYCLE(idx, max)   ( ((idx) >= ((max) - 1)) ? 0 : ((idx) + 1) )
#define CLI_DECCYCLE(idx, max)   ( ((idx) == 0) ? (max - 1) : ((idx) - 1) )

/* 判断指定终端的状态是否正常 */
#define IS_TERM_NORMAL(lp) ((lp).nTermStatus >= TERM_ACTIVE && (lp).ulTimeLeft != 0)

/* 判断字符c是否为Tab键         */
#define ISTAB( c )       (( c == '\t' ) ? 1 : 0)
/* 判断字符c是否为CTL + 字符键  */
#define ISCTRLCHAR( c )  (( c >= 0 && c <= 32 ) ? ('@' + c) : 0)
/* 判断字符c是否为BackSpace键   */
#define ISBACKSPACE( c ) (( c == '\b' ) ? 1 : 0)
/* 判断字符c是否为Delete键      */
#define ISDELETE( c )    (( c == 127 ) ? 1  : 0)
/* 判断字符c是否为ESC键         */
#define ISESC( c )       (( c == '\x1b' ) ? 1  : 0)
/* 判断字符c是否为[键           */
#define ISESC2( c )      (( c == '[' ) ? 1 : 0 )
/* 判断字符c是否为o键           */
#define ISESC3( c )      (( c == 'O' ) ? 1 : 0 )
/* 判断字符c是否为方向键        */
#define ISARROW( c )     (( c >= 'A' && c <= 'D' ) ? (c - 'A' + 1) : 0 )
#define ISSPACEBAR(c)    (((c) == ' ') ? 1 : 0)

#define CLI_ASSURE_OR_FAIL(exp)    if (!(exp)) return TBS_FAILED;
#if 0
{                               \
    if (!(exp))                 \
    {                           \
        DBG_Out( \
                "\r\n CLI FATAL: filename:%s, line:%d", __FILE__, __LINE__); \
        MT_ERRLOG(0);          \
        return TBS_FAILED;       \
    }                           \
}
#endif
#define CLI_ASSURE_OR_NULL(exp)    if (!(exp))  return NULL;
#if 0
{                               \
    if (!(exp))                 \
    {                           \
        DBG_Out( \
                "\r\n CLI FATAL: filename:%s, line:%d", __FILE__, __LINE__); \
        MT_ERRLOG(0);          \
        return NULL;          \
    }                           \
}
#endif



/* -------------------------------- 结构、类描述定义 ------------------------------ */

/* 用户终端任务数据结构定义开始                             { */

typedef struct tag_ST_CLI_TERM
{
    int         nTermType ;                     /* socket连接标识           */
    int         nTermStatus;                  /* 终端运行状态             */

    char        szCurUserName[USERNAME_LEN];  /* 当前用户名               */

    char        szRecvBuf[TERM_IO_BUFFER] ;   /* 接收缓冲区               */
    int         iRecvLen ;                    /* 实际接收到的数据长度     */

    char        szSendBuf[TERM_IO_BUFFER] ;   /* 发送缓冲区               */
    int         iSendLen ;                    /* 发送缓冲区的实际数据长度 */

    char        szHistory[HISTORY_SIZE][CLI_MAX_CMDBUF] ;/* 历史命令缓冲区 */
    int         iHistoryPos ;                 /* 当前历史命令所在的位置   */

    char        szEditBuf[CLI_MAX_CMDBUF] ;    /* 键盘编辑缓冲区           */
    int         iCurrentPos ;                 /* 当前的光标位置           */
    int         iPromptLen ;                  /* 当前命令的提示符长度     */

    char        szCommandBuf[CLI_MAX_CMDBUF] ; /* 命令缓冲区               */
    char        szInputCmd[CLI_MAX_CMDBUF] ;   /* 已经输入的命令           */
    int         iEditStatus ;                 /* 字符串的编辑状态         */
    int         iMaxLen ;                     /* 当前命令行编辑的最大长度 */

    BOOL        bCommandExecOk ;              /* 命令执行标志             */
    BOOL        bRecurCmd;                    /* 重现命令行标志           */
    ULONG       ulTimeLeft;                   /* 终端剩余时间             */
    ULONG       ulLastInputTime;              /* 上一次输入的时间，值为上一次输入时的time(0)返回值*/
    ULONG       ulDispLineCount;              /* 已经显示的行数           */

    UCHAR       ucUserLevel ;                 /* 当前用户的权级           */
    UCHAR       ucInputStatus;                /* 字符串输入状态           */
    UCHAR       ucScroll;                     /* 多行输出控制             */

} ST_CLI_TERM;


/* -------------------------------- 对外接口函数外部引用 ------------------------------ */

//extern struct TD_S      m_TDTable  ;
extern ST_CLI_TERM  m_stCliTerm;
extern char  m_szHostName[32];
extern ST_CLI_WS  m_stCliWS;                          /* 状态结构变量             */


/* 【2】Cli_Term.c的函数声明 */

void  CLI_TermEchoOn();
void  CLI_TermEchoOff();
ULONG   CLI_SendToTerm ( );
_VOID    CLI_WinMoveToCurrentPos ( int    iTmp, char  cFlag );
int     Cli_StdGetCh();
ULONG   CLI_ReceiveFromTerm(ULONG  ulSecond);
_VOID    CLI_TermTaskProc();
_VOID    CLI_TermTask( ULONG  ulArg1, ULONG  ulArg2);
_VOID    CLI_PeriodProc(_VOID   );
_VOID    CLI_TermLogin();
_VOID    CLI_TermLogout();
ULONG   CLI_ReturnRootMode();
_VOID    CLI_SetTermStatus(UCHAR nTermStatus);
ULONG   CLI_EditString( );
ULONG   CLI_CharProcess ( UCHAR ucTmp );
_VOID    CLI_CharInsert( char  ch );
_VOID    CLI_EditSpecialKey( char  ch );
_VOID    CLI_DealWithBackSpace ( );
_VOID    CLI_DealWithDelete ( );
_VOID    CLI_PrevCmdDisp ( );
_VOID    CLI_NextCmdDisp ( );
_VOID    CLI_MoveToCurrentPos ( ULONG  iPos );
_VOID    CLI_Bell ( );
ULONG   CLI_PatchCommand();
ULONG   CLI_PatchCmdWord();
ULONG   CLI_DealWitchCommand();
ULONG   CLI_CmdBufProcess (  );
_VOID    CLI_DisplayPrompt ( );
ULONG   CLI_SetPromptLen(ULONG  ulStrLen);
ULONG   CLI_GetTermLevel( );
BOOL    CLI_IsTermBusy();
ST_CLI_TERM* CLI_GetTermData ( _VOID    );
_VOID    CLI_TermDataInit( );
_VOID    CLI_CmdRecord ( );
_VOID    CLI_TermDestroy( );


/* 【3】Cli_lib.c的函数声明 */
ULONG   _isfloatstr(char  * pStr);
ULONG   _isintstr(char  * pStr);
char * _btrim (char  * pStr);
char * _ltrim(char  *pStr);
char * _rtrim(char  *pStr);
_VOID    _TrimString( char * pDst, char  * pSrc );
ULONG   strmatch(char  *s8Src, char  *s8Dest);
ULONG  _isempty(char  * pStr);
ULONG  _VerifyFixedString(char  *pStr, ULONG  ulLen);
ULONG  _VerifyAndCvtDwordI(char  * szNum, int    dwLow, int    dwHigh, int    * pdwValue);
ULONG  _VerifyAndCvtDword (char  * szNum, ULONG  dwLow, ULONG  dwHigh, ULONG  *
pdwValue);
ULONG  _VerifyAndCvtDwordH (char  * szNum, ULONG  dwLow, ULONG  dwHigh, ULONG  *
pdwValue);
ULONG  _isdecstr (char  * pStr);
ULONG  _isxdecstr (char  * pStr);
ULONG  _DateOrTimeToUl(char  *szDate, ULONG  *pulDate);
ULONG  _VerifyDate (char  *szDate1);
ULONG  _VerifyTime (char  *szTime1);
ULONG  _VerifyHostIPAddress(char  *szIPAdd);
ULONG  _VerifyGeneralIPAddress(char  *szIPAdd);
ULONG  _VerifyMASKAddress(char  *szMaskAdd);
ULONG  _VerifyMACAddress(char  *szMACAdd);
ULONG  _AffirmStrBeLower(char  *szStr);
int  _Split(char  **Sour,char  *Dest,char  flag,int Len);
ULONG  _StrToIPAddr(char  *string, ULONG  *ulRet);
char * _IPAddrToStr(ULONG  ulIPAddr, char  *string);
int  _atoulx(register char  *Sour,unsigned long * pTemp);
int  _atoul(register char  *Sour,unsigned long * pTemp);
int _atol(char  *szSrc, int    *ulVal);
int  _atou(char  *Sour,ULONG  * pTemp);
ULONG  _strtol(const char  *str, ULONG  base);
int  _atoux(register char  *Sour,unsigned short * pTemp);
ULONG   _IsChar(char  c);
ULONG  _IsCmdChar(char  c);
void CLI_ProcForCtrlC(int n);
UCHAR CLI_MapCp850ToIso8859(UCHAR ucChar);
UCHAR CLI_MapIso8859ToCp850(UCHAR ucChar);

#ifdef  __cplusplus
}
#endif

#endif /*end of #ifndef _CLI_PRIVATE_INC_*/

