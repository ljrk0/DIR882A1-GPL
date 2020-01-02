/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : cli_private.h
 文件描述 :


 函数列表 :


 修订记录 :
          1 创建 : liuzequn
            日期 : 2007-10-30
            描述 :

**********************************************************************/


#ifndef __CLI_PRIVATE_H__
#define __CLI_PRIVATE_H__
#include "../cli_pub.h"

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/
/*这里无需定义，在include/netinet/in.h已经定义*/
#if 0
    #define htonl(addr)     ( (((unsigned long)(addr) & 0x000000FF)<<24) | \
                             (((unsigned long)(addr) & 0x0000FF00)<<8)  | \
                             (((unsigned long)(addr) & 0x00FF0000)>>8)  | \
                             (((unsigned long)(addr) & 0xFF000000)>>24))

    #define ntohl(addr)     htonl(addr)

    #define htons(addr)     ( (((unsigned short)(addr) & 0x000000FF)<<8)  | \
                            (((unsigned short)(addr) & 0x0000FF00)>>8))

    #define ntohs(addr)     htons(addr)
#endif

int getch();

#define CLI_LOGFILE	"/dev/console"
#define CLI_MSG(fmt, arg...)	do {	FILE *log_fp = fopen(CLI_LOGFILE, "w"); \
						fprintf(log_fp, "%s:%s:%d:%ld:" fmt "\n", __FILE__, __func__, __LINE__, time(NULL),##arg); \
						fclose(log_fp); \
					} while(0)


/******************************************************************************
 *                                 ENUM                                       *
 ******************************************************************************/



/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/

#define DBG_Out     printf
//#define DBG_Trace  printf  
#define DBG_Trace(...) 
#define MT_ERRLOG(n) (void)(n);
#define DBG_ASSERT(exp) (void)(exp)
#define STB_ASSERT(exp) (void)(exp)
#define REG_OUT     printf

#define CLI_StaticAlloc malloc
#define CLI_Malloc malloc
#define CLI_Free free
#define CLI_MemSet   memset

#define STB_Printf printf

#define STB_StrCpy  strcpy
#define STB_StrNCpy strncpy
#define STB_StrCat  strcat

#define STB_StrNCat strncat
#define STB_StrCmp  strcmp
#define STB_StrNCmp strncmp
#define STB_StrLen  strlen

int    STB_StriCmp(const char  *szSrc, const char  *szDst);
int    STB_StrNiCmp(const char *szSrc, const char *szDst, int nCount);
ULONG  STB_StrToIPAddr(char  *string, ULONG  *ulRet);
char  *STB_IPAddrToStr(ULONG  ulIPAddr, char  *string);
ULONG  STB_atoul(register char *Sour, ULONG  * pTemp);
ULONG  STB_atol(char  *szSrc, int    *ulVal);
ULONG  STB_atou(char  *Sour,ULONG  * pTemp);
ULONG  STB_atoulx(register char  *Sour, ULONG  * pTemp);
ULONG  STB_IsChar(char  c);
ULONG  STB_Sprintf(char  *szDst, const char  *szFormat, ...);
char  *strchrR(char	*s,	int	c);



#define CLI_DBGOUT_ERR 0
#define CLI_DBGOUT_NOTE 1


#define CLI_ML_NULL     0







/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/










/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/


#endif /* __CLI_PRIVATE_H__ */



