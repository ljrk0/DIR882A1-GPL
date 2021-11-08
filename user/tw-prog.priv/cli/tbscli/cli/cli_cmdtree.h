/****************************************************************************
 �ļ���  : CLI_Io.h
 ����    : liuzequn
 �汾    :
 �������:
 �ļ�����:  ���ļ��ṩ������������е�ע��ӿ��Լ�ע�����������Ҫ
            ���õĽṹ���壬����Ҳ����ִ�к����еĲ�����ȡ�ӿ�
 ��ע   :
 �����б�:
            (1);
            (2);
            (3);
            ......

�ṩ���ⲿ�Ľӿ�:   ���ļ��ṩ���ⲿ�Ľӿ�
            (1) ULONG   CLI_CmdAutoRegist
            (2)
            (3)
            (4)
            (5)
            (6)
            (7)
            (8)
            (9)
            (10)

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
#ifndef _CLI_CMDTREE_INC_
#define _CLI_CMDTREE_INC_

#ifdef  __cplusplus
extern "C" {
#endif
#include "cli_private.h"

/* -------------------------------- ���Ͷ���------------------------------ */


/* -------------------------------- �����ꡢö�ٶ���------------------------------ */

#define CLI_MAX_MODE_NAME    48          /* ģʽ������       */
#define CLI_MAX_MODE_PROMPT  36          /* ģʽ��ʾ���ĳ��� */
#define CLI_MAX_MODE_VERB    256         /* ����ĳģʽ�����ֳ��� */
#define CLI_MAX_CMDLEN       1024
#define MAX_TOKEN_SIZE      512          /* ������������󳤶�*/
#define CLI_MAX_PARAMLEN     MAX_TOKEN_SIZE/* ��������󳤶�*/


/* ����������λ���� */
#define CLI_PRMPROP_NULL        0x00        /* ������   */
#define CLI_PRMPROP_NAME        0x01        /* ��Ҫ���� */
#define CLI_PRMPROP_MUTEX       0x02        /* ��ֵ���� */
#define CLI_PRMPROP_INDEX       0x04        /* ����     */
#define CLI_PRMPROP_OPTIONAL    0x08        /* ��ѡ     */

/* ֵ������λ���壬�Զ�ע�᲻ʹ�ã�һ��ΪCLI_VALPROP_CHECK  */
#define CLI_VALPROP_NULL        0x00        /* ������       */
#define CLI_VALPROP_OPTIONAL    0x01        /* ��ѡ         */
#define CLI_VALPROP_MUTEX       0x02        /* ����(��ѡһ) */
#define CLI_VALPROP_INDEX       0x04        /* λ����       */
#define CLI_VALPROP_CHECK       0x08        /* �������ͼ�� */
#define CLI_VALPROP_DEFAULT     0x10        /* ȱʡ         */


/* ģʽת������ */
typedef enum  CLI_MODE_CONV_E
{
    CLI_MC_NULL = 0      ,   /*��ģʽת��             */
    CLI_MC_NEW           ,   /*ת�����µ�ģʽ         */
    CLI_MC_EXIT          ,   /*�˳�����һģʽ         */
    CLI_MC_END               /*�˳���CTM_GENL_EXECģʽ*/
} CLI_MODE_CHANGE_T;

/* ����ģʽö�ٶ��� */
enum CLI_MODE_ID_E {
    /* ����ģʽ */
    CTM_GLOBAL = 0      ,  /* ȫ��ģʽ����ģʽ�µ��������κ�ģʽ�¿���ִ��    (ʹ�ü���: ��ѯ��)*/
    CTM_GENL            ,  /* ��Ȩģʽ������һ�������     (ʹ�ü���:������)*/
    CTM_CONFIG          ,  /* ȫ������ģʽ����ҵ������Ϊ�� (ʹ�ü���:������)*/
    CTM_DIAG            ,  /* ���ģʽ��ϵͳ״̬��������� (ʹ�ü���:���Լ�)*/
    CTM_DBG             ,  /* ����ģʽ����������ʹ�ã�release�汾�����ڸ�ģʽ (ʹ�ü���: super��)*/
    /* ��չģʽ */
    CTM_IF              ,  /* �ӿ�ģʽ������ģʽ����ģʽ */

    /* ��̬ģʽ */
    CTM_DYNAMIC_START   ,  /* ��̬ģʽ��ʼ��־���˴�����һ�������ģʽ */

    CTM_TOTAL              /* ����ģʽ�Ľ�����������һ�������ģʽ */
} ;
#define END_MODE        CTM_GENL
#define ROOT_MODE       CTM_GENL
#define MAX_MODE_NUM    (CTM_TOTAL + 1)
#define NULL_MODE       CTM_TOTAL


/*  ����ֵ���Ͷ��� */
#define CLI_DATA_MAX_TYPE    32
typedef enum  CLI_DATATYPES_E
{
    CLI_DATA_NULL,                       /* ��                   */
    CLI_DATA_VI,                         /* ����                 */
    CLI_DATA_VIK,                        /* ���� + ����          */
    CLI_DATA_VU,                         /* �޷�������           */
    CLI_DATA_VUK,                        /* �޷������� + ����    */
    CLI_DATA_VH,                         /* 16��������           */
    CLI_DATA_VHK,                        /* 16�������� +����     */
    CLI_DATA_VS,                         /* �ַ���               */
    CLI_DATA_K,                          /* ����                 */
    CLI_DATA_IP,                         /* ����ip               */
    CLI_DATA_IPK,                        /* ����ip + ����        */
    CLI_DATA_MASK,                       /* ip����               */
    CLI_DATA_MAC,                        /* MAC��ַ              */
    CLI_DATA_MACK,                       /* MAC��ַ + ����       */
    CLI_DATA_DATE,                       /* �������� y-m-d       */
    CLI_DATA_TIME,                       /* ʱ������ h:m:s       */

    CLI_DATA_USER,                       /* �û������Զ�����ޣ���
                                           ���ͼ��Ժ�ľ�Ϊ�û��Զ�������   */
    CLI_DATA_BOTTOM = CLI_DATA_MAX_TYPE

} CLI_DATATYPES_T;



/* -------------------------------- ��������� ------------------------------ */

/* �ж����������Ƿ�ͬ�� */
#define CLI_IS_SAME_KEY(p1,p2) ( strcmp((p1)->szName,(p2)->szName) == 0 )
/* �ж������϶���1�Ƿ����ڶ���2*/
#define CLI_KEY_BIGGER_THAN(p1,p2)  ( strcmp((p1)->szName,(p2)->szName) > 0  )


/* -------------------------------- �ṹ������������ ------------------------------ */
/*------------------------------------*/
/* ��1������ֵ�Ķ��忪ʼ              */
/*------------------------------------*/
typedef struct tag_ST_CLI_VALTYPE
{
    UCHAR ucAttrib;    /* ����ֵ����    */
    UCHAR ucDataType;  /* ����ֵ����    */
    ULONG ulDataLen;   /* ����ֵ����(����ַ�������)*/
    UCHAR ucIndex;     /* ����          */
}ST_CLI_VALTYPE, *PST_CLI_VALTYPE;


/* Ϊ�ؼ��ֶ�������ݽṹ  {{*/
typedef struct tag_ST_CLI_ENUM
{
    char    *pK;                   /* enumö�ٴ�        */
    struct tag_ST_CLI_ENUM *pPrevious;   /* ǰһ��ö�� */
    struct tag_ST_CLI_ENUM *pNext;       /* ��һ��ö�� */
}ST_CLI_ENUM,*PST_CLI_ENUM;

typedef struct tag_ST_CLI_ENUMS
{
    ULONG    ulNumber;             /* ö�����еĸ��� */
    PST_CLI_ENUM pKeyword;            /* ö�����е����� */
}ST_CLI_ENUMS;
/*                             }}*/

/* �Զ���Ĳ���ֵ��麯��*/
typedef ULONG  (*FUN_USER_CHECK)(char  *szValue,char  *szShow);
typedef struct tag_ST_CLI_VALUE
{
    char  *szName;                      /* ����ֵ��           */
    ST_CLI_VALTYPE valueType;           /* ����ֵ����         */
    ST_CLI_ENUMS keywords;              /* ��ѡһ�ؼ����б�   */
    int    lIntFilter[2];             /* ������ֵ��������   */
    struct tag_ST_CLI_VALUE *pPrevious; /* ����ֵǰ��         */
    struct tag_ST_CLI_VALUE *pNext;     /* ����ֵ�ĺ��       */
    ULONG    ulAbsoluteIndex;           /* ����λ������       */
    char    *szDefault;                 /* ����ֵ��ȱʡֵ     */
    FUN_USER_CHECK pCheckFunc;          /* �û��ṩ�ļ�麯�� */
}ST_CLI_VALUE, *PST_CLI_VALUE;

/* ����ֵ����Ķ��� */
typedef struct tag_ST_CLI_VALUELINK
{
    ULONG  ulNumber;           /* Total number of Values in the Link   */
    PST_CLI_VALUE pValue;      /* Head Pointer to the Value Link       */
}ST_CLI_VALUELINK, *PST_CLI_VALUELINK;

/*�û��Զ������ֵ����*/
#define CLI_MAX_USERTYPES    ((ULONG )CLI_DATA_BOTTOM - (ULONG )CLI_DATA_USER)

typedef ULONG  (*CLI_TYPECHECK_FUNC_T) (char  *szValue,char  *szShow);
typedef struct  tag_ST_CLI_USERTYPE
{
    UCHAR ucTypeID;                        /* ��������     */
    char  szTypeName[24];                  /* ������������ */
    CLI_TYPECHECK_FUNC_T pCheckFunc;      /* ��麯��     */
} ST_CLI_USERTYPE;
/*------------------------------------*/
/* ��1������ֵ�Ķ������              */
/*------------------------------------*/




/*------------------------------------*/
/* ��2���������忪ʼ                  */
/*------------------------------------*/
typedef struct tag_ST_CLI_PARAM_TYPE
{
    UCHAR ucAttrib;     /* ����λ   */
    UCHAR ucIndex;      /* λ������ */
}ST_CLI_PARAM_TYPE, *PST_CLI_PARAM_TYPE;

typedef struct tag_ST_CLI_PARAMETER
{
    char  *szName;                      /* ������            */
    ST_CLI_PARAM_TYPE stParamType;           /* ��������          */
    UCHAR ucAttrib;                     /* ��������          */
    struct tag_ST_CLI_PARAMETER *subParam;    /* ��һ���Ӳ�����ָ��*/
    struct tag_ST_CLI_PARAMETER *pPrevious;   /* ������ǰ��        */
    struct tag_ST_CLI_PARAMETER *pNext;       /* �����ĺ��        */
    struct tag_ST_CLI_PARAMETER *pParent;     /* ������ָ��        */
    ULONG  ulAbsoluteIndex;             /* ��������          */
    ST_CLI_VALUELINK valueList;          /* ����ֵ�б�        */
}ST_CLI_PARAMETER, *PST_CLI_PARAMETER;

/*��������ṹ����*/
typedef struct tag_ST_CLI_PARAMLINK
{
    ULONG  ulNumber;          /* ����������������   */
    PST_CLI_PARAMETER pParam;    /* �������Ĳ���ͷ�� */
}ST_CLI_PARAMLINK, *PST_CLI_PARAMLINK;
/*------------------------------------*/
/* ��2�������������                  */
/*------------------------------------*/




/*------------------------------------*/
/* ��3���ؼ��ֵĽṹ���忪ʼ          */
/*------------------------------------*/

/* ����ִ�в��������Խṹ*/
typedef struct tag_ST_CLI_ACTION
{
    ULONG  ulMode;                    /* �������ģʽ       */
    CLI_OPERAT_LEVEL_T rightLevel;   /* �������Ȩ��       */
    PFUN_CLI_CALLBACK_EXEC pFunction; /* �����ִ�к�����ַ */
    PCSTR   strObjHelp;                  /* ������ϢID         */
    CLI_MODE_CHANGE_T modeConv;        /* ģʽת����ʽ       */
    PST_CLI_PARAMLINK pParamLink;      /* ����������         */
}ST_CLI_ACTION;

typedef struct tag_ST_CLI_CMDKEY
{
    char  *szName;                    /* ������               */
    ULONG  ulObjectCount;             /* �Ӷ������           */
    struct tag_ST_CLI_CMDKEY *pSubCmdKey; /* ��һ���Ӷ����ָ��   */
    struct tag_ST_CLI_CMDKEY *pPrevious;  /* �����ǰ��           */
    struct tag_ST_CLI_CMDKEY *pNext;      /* ����ĺ��           */
    struct tag_ST_CLI_CMDKEY *pParent;    /* ������ָ��           */
    ST_CLI_ACTION action;             /* ����Ĳ������Խṹ   */
    ST_CLI_ACTION noAction;           /* �����no�������Խṹ */
    ULONG   ulNoFlag;                 /* �����Ƿ��з���ʽ   */
}ST_CLI_CMDKEY, *PST_CLI_CMDKEY;

/*------------------------------------*/
/* ��3������Ľṹ�������            */
/*------------------------------------*/



/*------------------------------------*/
/* ��4������ģʽ���忪ʼ              */
/*------------------------------------*/
typedef struct tag_ST_CLI_MODE
{
    ULONG   ulMode;                           /* ģʽID         */
    ULONG   ulParent;                         /* ��ģʽID       */
    CLI_OPERAT_LEVEL_T rightLevel;            /* ģʽȨ��       */
    char    szModeName[CLI_MAX_MODE_NAME];    /* ģʽ����       */
    char    szModePrompt[CLI_MAX_MODE_PROMPT];/* ģʽ��ʾ��     */

    ULONG   ulHierarchy;                      /* ģʽ���       */
    ULONG   ulParamCount;                     /* ��������       */
    PST_CLI_CMDKEY pCmdKey;                   /* ģʽ����       */
}ST_CLI_MODE;

typedef struct tag_ST_CMD_MODE_INFO
{
    ULONG   ulMode;                           /* ģʽID         */
    ULONG   ulParent;                         /* ��ģʽID       */
    CLI_OPERAT_LEVEL_T rightLevel;            /* ģʽȨ��       */
    char    szModeName[CLI_MAX_MODE_NAME];    /* ģʽ����       */
    char    szModePrompt[CLI_MAX_MODE_PROMPT];/* ģʽ��ʾ��     */
    PFUN_CLI_CALLBACK_EXEC pFunction;         /* �����ִ�к�����ַ */
    PCSTR   strObjHelp;                       /* ������ϢID         */
}ST_CMD_MODE_INFO;



/*------------------------------------*/
/* ��4������ģʽ�������              */
/*------------------------------------*/





/* -------------------------------- ����ӿں����ⲿ���� ------------------------------ */

_VOID CLI_InitUserType(_VOID);
_VOID CLI_InitSysModes(_VOID);
ULONG  CLI_InitModeObject(_VOID);
ULONG   CLI_InitCmdTree(ST_CMD_MODE_INFO *pModeInfo);
ULONG  CLI_RegisterUserType(ULONG  *ulID,char  *szName,CLI_TYPECHECK_FUNC_T pFunc);
ULONG  CLI_FreeValue( PST_CLI_VALUE pValue );
ULONG  CLI_FreeKeyword(PST_CLI_ENUM pKW);
PST_CLI_ENUM CLI_NewKeywords(char  *szKeys, ULONG  *ulCount);
ULONG  CLI_FreeParameter(PST_CLI_PARAMETER pParam);
ULONG  CLI_FreeParamlink(PST_CLI_PARAMLINK pParamLink);
ULONG  CLI_FreeObject(PST_CLI_CMDKEY pCmdKey);
PST_CLI_VALUE CLI_NewValue(_VOID);
PST_CLI_VALUELINK CLI_NewValuelink(_VOID);
PST_CLI_ENUM CLI_NewKeyword(_VOID);
PST_CLI_CMDKEY CLI_NewObject(_VOID);
PST_CLI_PARAMETER CLI_NewParameter(_VOID);
PST_CLI_PARAMLINK CLI_NewParamlink(_VOID);
PST_CLI_CMDKEY CLI_CreateObject(char  *szName, ULONG  ulMode,
            CLI_OPERAT_LEVEL_T level, PFUN_CLI_CALLBACK_EXEC pFunc,
            BOOL bChangeMode, CLI_MODE_CHANGE_T modeConv,
            PFUN_CLI_CALLBACK_EXEC pNoFunc);
PST_CLI_PARAMETER CLI_CreateParameter( char  *szNameStr, UCHAR ucTypeAttrib,
                                   UCHAR ucIndex, ULONG  ulIndex );
PST_CLI_PARAMLINK CLI_CreateParamLink(_VOID);
PST_CLI_VALUE CLI_CreateValue(char  *szName, UCHAR ucDataType,
                          ULONG ulDataLen, char  *szKeys,  int    iLow, int    iHigh,
                          ULONG  ulIndex);
PST_CLI_VALUE CLI_CreateValueWithCheck(char  *szName,
                                UCHAR ucAttrib, UCHAR ucDataType, ULONG ulDataLen,
                                char  *szKeys,  int    iLow, int    iHigh,
                                ULONG  ulIndex,
                                FUN_USER_CHECK pFunc);
ULONG  CLI_DefineValueCheck(PST_CLI_VALUE pValue,FUN_USER_CHECK pFunc);
ULONG  CLI_SetValueDefault(PST_CLI_VALUE pInValue, char  *szDefault);
ULONG  CLI_SetMode(ULONG  ulMode, char  *szName, char  *szPrompt,
              CLI_OPERAT_LEVEL_T level,
              ULONG  ulParentMode, PST_CLI_CMDKEY pCmdKey);
ULONG  CLI_RegMode(ST_CLI_MODE *pMode);
ULONG  CLI_RegModeObject(ULONG  ulMode,PST_CLI_CMDKEY pCmdKey, ULONG  ulParamCount);
ULONG  CLI_RegModeObjectExt( ULONG  ulMode, PST_CLI_CMDKEY pCmdKey,
                        ULONG  ulParamCount, PFUN_CLI_CALLBACK_EXEC pFunc);
ULONG  CLI_RegObject(PST_CLI_CMDKEY pParent, PST_CLI_CMDKEY pCmdKey);
ULONG  CLI_RegObjToMode(ULONG  ulMode, PST_CLI_CMDKEY pCmdKey);
ULONG  CLI_RegCmdHelp(PST_CLI_CMDKEY pCmdKey, PCSTR szHelpInfoID);
ULONG  CLI_RegParamlink(PST_CLI_CMDKEY pCmdKey, BOOL bNoform,
                         PST_CLI_PARAMLINK pParamLink);
ULONG  CLI_RegValueToParam(PST_CLI_PARAMETER pParam,
                                PST_CLI_VALUE pValue);
ULONG  CLI_RegParamAsHeir(PST_CLI_PARAMETER pOldParam,
                     PST_CLI_PARAMETER pNewParam);
ULONG  CLI_RegParamToLink(PST_CLI_PARAMLINK pParamLink,
                                PST_CLI_PARAMETER pNewParam);
ULONG  CLI_GlobalCmdLink(_VOID);


#ifdef  __cplusplus
}
#endif

#endif /*end of #ifndef _CLI_CMDTREE_H_*/
