/****************************************************************************
 �ļ���  : CLI_Term.h
 ����    : liuzequn
 �汾    :
 �������:
 �ļ�����: ������ģ��󲿷��ӹ�����Ҫ�õ��Ķ�����������˽�У�������ģ�鲻�ṩ
 �޸���ʷ:
        1. �޸���   :
           ʱ��     :
           �汾     :
           �޸�ԭ�� :
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

/* ��ʱ����λΪ1/100�� */
#define CLI_DELAY(s)   usleep(s*100000)


/* -------------------------------- ���Ͷ���------------------------------ */


/* -------------------------------- �����ꡢö�ٶ���------------------------------ */

#define HISTORY_SIZE        10      /* ��ʷ�������      */
#define USERNAME_LEN        32      /* �û�����󳤶�    */
#define PASSWORD_LEN        USERNAME_LEN
#define HOSTNAME_LENGTH     16      /* ��������󳤶�    */
#define CLI_MAX_CMDBUF       256     /* �������󳤶�  */
#define TERM_TIMEOUT        300     /* ���յĳ�ʱ���ȶ���*/
#define TELNET_ACL_NUM      10      /* telnet���ʿ��Ʊ��С*/

#define FIRST_TELNET_DATA   2       /* ��һ��telnet���ݵ�λ��*/
#define MAX_USER_NUM        16      /* �����û�������  */
#define MAX_CLIENT_NUM      0

#define MAX_ONLINE_USER     (MAX_CLIENT_NUM + 2)


#define TERM_IO_BUFFER      1024    /* �ն˷�������ջ����С */
#define TERM_OUT_FOR_RESPONSE 24    /* Ԥ����������ִ�з����Ŀռ�*/

//support special char for telefonica spain
#define TELEFONICA_SPAIN_TEST
#define SPECIAL_CHAR_BEGIN 0x80

#define CLI_INVALID         (ULONG )(-1)

/* ����������Դ��� */
#define MAX_FAILED_LOG_TIME     3

/* ��ʱ���ض���     */
#define TIMEOUT_ON     0
#define TIMEOUT_OFF    1

/* ����ģʽ����     */
#define ENGLISH_L        0
#define CHINESE_L        1

/* ����ģʽ���ض��� */
#define CTHELPMODEON     0
#define CTHELPMODEOFF    1

/* �Զ����ֶ��������� */
#define SCROLL_MANUAL    1
#define SCROLL_AUTO      0

/* ���������ض��� */
#define SMART_ON         0
#define SMART_OFF        1

/* telnet �˿ںŶ���            */
#define TELNETPORT      23
/* �ն�socket�ڷ�telnet״̬�µ�����   */
#define CLI_TERM_FOR_SERIAL       -1
#define CLI_TERMT_FOR_MML          -2
#define CLI_TERM_NOT_INITIAL      -3

/* �ն����������״̬����        */
enum  Term_Status_E
{
    TERM_SLEEPING = 0,
    TERM_ACTIVE,
    TERM_LOGED,
    TERM_EXECCMD,
    TERM_WAIT_RESPONSE,

    TELNET_MAX_STATUS
} ;

/* -------------------------------- ��������� ------------------------------ */

/* �н�����ֵ��1����1�궨�� */
#define CLI_INCCYCLE(idx, max)   ( ((idx) >= ((max) - 1)) ? 0 : ((idx) + 1) )
#define CLI_DECCYCLE(idx, max)   ( ((idx) == 0) ? (max - 1) : ((idx) - 1) )

/* �ж�ָ���ն˵�״̬�Ƿ����� */
#define IS_TERM_NORMAL(lp) ((lp).nTermStatus >= TERM_ACTIVE && (lp).ulTimeLeft != 0)

/* �ж��ַ�c�Ƿ�ΪTab��         */
#define ISTAB( c )       (( c == '\t' ) ? 1 : 0)
/* �ж��ַ�c�Ƿ�ΪCTL + �ַ���  */
#define ISCTRLCHAR( c )  (( c >= 0 && c <= 32 ) ? ('@' + c) : 0)
/* �ж��ַ�c�Ƿ�ΪBackSpace��   */
#define ISBACKSPACE( c ) (( c == '\b' ) ? 1 : 0)
/* �ж��ַ�c�Ƿ�ΪDelete��      */
#define ISDELETE( c )    (( c == 127 ) ? 1  : 0)
/* �ж��ַ�c�Ƿ�ΪESC��         */
#define ISESC( c )       (( c == '\x1b' ) ? 1  : 0)
/* �ж��ַ�c�Ƿ�Ϊ[��           */
#define ISESC2( c )      (( c == '[' ) ? 1 : 0 )
/* �ж��ַ�c�Ƿ�Ϊo��           */
#define ISESC3( c )      (( c == 'O' ) ? 1 : 0 )
/* �ж��ַ�c�Ƿ�Ϊ�����        */
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



/* -------------------------------- �ṹ������������ ------------------------------ */

/* �û��ն��������ݽṹ���忪ʼ                             { */

typedef struct tag_ST_CLI_TERM
{
    int         nTermType ;                     /* socket���ӱ�ʶ           */
    int         nTermStatus;                  /* �ն�����״̬             */

    char        szCurUserName[USERNAME_LEN];  /* ��ǰ�û���               */

    char        szRecvBuf[TERM_IO_BUFFER] ;   /* ���ջ�����               */
    int         iRecvLen ;                    /* ʵ�ʽ��յ������ݳ���     */

    char        szSendBuf[TERM_IO_BUFFER] ;   /* ���ͻ�����               */
    int         iSendLen ;                    /* ���ͻ�������ʵ�����ݳ��� */

    char        szHistory[HISTORY_SIZE][CLI_MAX_CMDBUF] ;/* ��ʷ������� */
    int         iHistoryPos ;                 /* ��ǰ��ʷ�������ڵ�λ��   */

    char        szEditBuf[CLI_MAX_CMDBUF] ;    /* ���̱༭������           */
    int         iCurrentPos ;                 /* ��ǰ�Ĺ��λ��           */
    int         iPromptLen ;                  /* ��ǰ�������ʾ������     */

    char        szCommandBuf[CLI_MAX_CMDBUF] ; /* �������               */
    char        szInputCmd[CLI_MAX_CMDBUF] ;   /* �Ѿ����������           */
    int         iEditStatus ;                 /* �ַ����ı༭״̬         */
    int         iMaxLen ;                     /* ��ǰ�����б༭����󳤶� */

    BOOL        bCommandExecOk ;              /* ����ִ�б�־             */
    BOOL        bRecurCmd;                    /* ���������б�־           */
    ULONG       ulTimeLeft;                   /* �ն�ʣ��ʱ��             */
    ULONG       ulLastInputTime;              /* ��һ�������ʱ�䣬ֵΪ��һ������ʱ��time(0)����ֵ*/
    ULONG       ulDispLineCount;              /* �Ѿ���ʾ������           */

    UCHAR       ucUserLevel ;                 /* ��ǰ�û���Ȩ��           */
    UCHAR       ucInputStatus;                /* �ַ�������״̬           */
    UCHAR       ucScroll;                     /* �����������             */

} ST_CLI_TERM;


/* -------------------------------- ����ӿں����ⲿ���� ------------------------------ */

//extern struct TD_S      m_TDTable  ;
extern ST_CLI_TERM  m_stCliTerm;
extern char  m_szHostName[32];
extern ST_CLI_WS  m_stCliWS;                          /* ״̬�ṹ����             */


/* ��2��Cli_Term.c�ĺ������� */

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


/* ��3��Cli_lib.c�ĺ������� */
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

