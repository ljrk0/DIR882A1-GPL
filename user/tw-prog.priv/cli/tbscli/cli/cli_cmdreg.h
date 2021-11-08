/****************************************************************************
 �ļ���  :  CLI_Io.h
 ����    :  liuzequn
 �汾    :
 �������:  2003-2-14
 �ļ�����:  ���ļ��ṩ������������е�ע��ӿ��Լ�ע�����������Ҫ
            ���õĽṹ���壬����Ҳ����ִ�к����еĲ�����ȡ�ӿ�
 ��ע   :
 �����б�:
            (1);
            (2);
            (3);
            ......

�ṩ���ⲿ�Ľӿ�:  ���ļ��ṩ���ⲿ�Ľӿ�
            (1) CLI_CmdAutoRegist
            (2) CLI_GetParamByName
            (3)

��Ҫ�ⲿ�ṩ�Ľӿ�:    ���ļ���Ҫ�ⲿ�ṩ�Ľӿ�
            (1)
            (2)
 �޸���ʷ:
        1. �޸���   :
           ʱ��     :
           �汾     :
           �޸�ԭ�� :
        2. ...
 ****************************************************************************/
#ifndef _CLI_REG_H_
#define _CLI_REG_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "cli_private.h"
#include "cli_cmdtree.h"

/* -------------------------------- ���Ͷ���------------------------------ */


/* -------------------------------- �����ꡢö�ٶ���------------------------------ */


/* ���������Ϣ�Ķ���������*/
/*#define HELP_FUN_E              "\r\n  Function: "
#define HELP_USG_E              "\r\n  Usage   : "
#define HELP_OPT_E              "\r\n  Options : "
#define HELP_EXP_E              "\r\n  e.g.    : "
#define HELP_SPC_E              "\r\n            "
#define HELP_FUN_C              "\r\n  �����: "
#define HELP_USG_C              "\r\n  �����ʽ: "
#define HELP_OPT_C              "\r\n  ����˵��: "
#define HELP_EXP_C              "\r\n  ��    ��: "
#define HELP_SPC_C              "\r\n            "
*/

#define HELP_FUN                "\r\n\r\n  "
#define HELP_FMT                "\r\n\r\n  "
#define HELP_OPT                "\r\n    "
#define HELP_SPC                "\r\n    "
#define HELP_EXP                "\r\n\r\n  "



#define CMD_CM              "cm "
#define CLI_MAX_CMD_FORMAT  1500        /* �����Զ�ע��ĸ�ʽ�������Ȳ��ܳ���1000 */
#define CLI_MAX_KEY_NUM     3           /* ����һ�������е���������� */
#define CLI_MAX_PARAM_NUM   40          /* ���������������*/


/* �Զ�ע����ڲ�ʹ�õĴ����붨�� */
enum AutoReg_ErrNo
{
    TOKEN_GET_ONLYSPACE = 0, /* ֻ��ȡ���ո���� */
    TOKEN_GET_GROUPBEGIN,    /* �����鿪ʼ */
    TOKEN_GET_MUTEXBEGIN,    /* �����鿪ʼ */
    TOKEN_GET_GROUPEND,      /* ��������� */
    TOKEN_GET_MUTEXEND,      /* ��������� */
    TOKEN_GET_OPTIONBGIN ,   /* ��ѡ�鿪ʼ */
    TOKEN_GET_OPTIONEND ,    /* ��ѡ����� */
    TOKEN_GET_PROPBEGIN,     /* ����ֵ���Կ�ʼ */
    TOKEN_GET_PROPEND,       /* ����ֵ���Խ��� */
    TOKEN_GET_VALBEGIN,      /* ����ֵ��ʼ */
    TOKEN_GET_VALEND,        /* ����ֵ���� */
    TOKEN_GET_MUTEXLINE,     /* ����ָ��� */
    TOKEN_GET_SPACEEND,      /* �ո�ָ��� */
    TOKEN_GET_DEFAULT,       /* ȱʡ�ָ��� */
    TOKEN_GET_LASTOBJ,       /* ���һ������ */
    TOKEN_GET_CMDNULL        /* ȡ�� */
};



/* -------------------------------- �ṹ������������ ------------------------------ */



/* ����ע��Ļ�����Ϣ�ṹ����*/
typedef struct CmdDefine
{
    const char  *szCmd;         /*�����ʽ����*/
    PCSTR  strHelp1;       /*��һ������İ�����Ϣ*/
    PCSTR  strHelp2;       /*�ڶ�������İ�����Ϣ(�������������2�����ò���Ϊ��)*/
    PCSTR  strHelp3;       /*����������İ�����Ϣ(�������������3�����ò���Ϊ��)*/
    PFUN_CLI_CALLBACK_EXEC pFunc; /*����ִ�к���*/
    UCHAR   ucLevel;       /*����ʹ�õȼ� */
    UCHAR   ucMode;        /*��������ģʽ*/
} ST_CLI_CMD_REG, *PST_CLI_CMD_REG;


/*------------------------------------*/
/*��5���Զ�ע�ᶨ�忪ʼ               */
/*------------------------------------*/

/* �﷨�������̱����ṹ����*/
typedef struct PointStack
{
    PST_CLI_PARAMLINK pParamLink;
    PST_CLI_PARAMETER pParam;
    PST_CLI_VALUE  pValue;
    UCHAR Number;
    UCHAR ucMaxIndex;
    UCHAR ucMutexIndex;
    struct PointStack *pPre;
}ST_POINT_STACK;

/*------------------------------------*/
/*��5���Զ�ע�ᶨ�����               */
/*------------------------------------*/

/* -------------------------------- ����ӿں����ⲿ���� ------------------------------ */



ULONG  CLI_CmdAutoRegist(const char  *szCmd,  UCHAR  ucMode,
                CLI_OPERAT_LEVEL_T   ucLevel,  PFUN_CLI_CALLBACK_EXEC pFunc,
                UCHAR  ucModeChange, UCHAR  ucNewMode,
                PCSTR  strHelp1, PCSTR  strHelp2, PCSTR  strHelp3);
ULONG  CLI_IsCmdValid(char  * szCmd);
PST_CLI_CMDKEY CLI_GetSubObjByName(PST_CLI_CMDKEY pCmdKey, char  *szName);
PST_CLI_PARAMLINK CLI_CreatGroup(char  * szParmGroup);
ULONG  CLI_GetObj(char  *pCurPos, char  *szObj);
ULONG  CLI_GetRegistToken(char  * szSrc, char  * szDst);
_VOID    CLI_SetProperty(UCHAR *ucState, UCHAR usProp);
ULONG  CLI_IsProp(UCHAR ucState, UCHAR usProp);
char  *CLI_GetEndSignal(char  *pCurPos);
_VOID    CLI_GetValType(char  *szSrc, UCHAR *ucType, int    *piLow,
                   int    *piHigh, int    *piStrLen);
ST_POINT_STACK *CLI_NewPointStack(_VOID   );
PST_CLI_PARAMETER CLI_AutoCreatParam(ST_POINT_STACK *pPointStack, char  *szName, UCHAR *ucStat);
PST_CLI_VALUE CLI_AutoCreatVal(ST_POINT_STACK *pPointStack, char  *szName, UCHAR *ucStat, char  *szType);
ULONG  CLI_GetRangeStr(char  *szSrc, char  *szLow, char  *szHigh);
ULONG  CLI_CheckForValue(PST_CLI_VALUE pValue, char  *szToken);


#ifdef  __cplusplus
}
#endif

#endif /*end of #ifndef _CLI_REG_H_*/
