
#include <ctype.h>
#include  "cli_private.h"
#include  <stdarg.h>

int    STB_StriCmp(const char  *szSrc, const char  *szDst)
{
    char  *pcSrc, *pcDst;
    int    cTmp;

    if (szSrc == NULL || szDst == NULL)
    {
        STB_ASSERT(0);
        return 1;
    }

    pcSrc = (char  *)szSrc;
    pcDst = (char  *)szDst;

    while (*pcSrc != '\0' || *pcDst != '\0')
    {
        cTmp = (int   )(tolower(*pcSrc) - tolower(*pcDst));
        if (cTmp != 0)
        {
            return cTmp;
        }
        pcSrc++;
        pcDst++;
    }
    return 0;
}


/*****************************************************************************
 函数名称    : STB_StrNiCmp()
 函数功能    : 忽略大小写的的字符串比较
 下层函数    : 无.
 上层函数    :
 访问全局变量: 无.
 修改全局变量: 无.
 访问到的表  : 无.
 修改到的表  : 无.
 输入参数    : szSrc, 字符串1的起始地址.
               szDst, 字符串2的起始地址.
 输出参数    : 无.
 返回值      : 正值,当 szSrc > szDst
               负值,当 szSrc > szDst
               0   ,当 szSrc = szDst
 备注        : 无.
******************************************************************************/
int STB_StrNiCmp(const char *szSrc, const char *szDst, int nCount)
{
    char *pcSrc, *pcDst;
    long cTmp;
    int i;

    if (szSrc == NULL || szDst == NULL)
    {
        STB_ASSERT(0);
        return 1;
    }

    pcSrc = (char *)szSrc;
    pcDst = (char *)szDst;

    for (i = 0; i < nCount && (*pcSrc != '\0' || *pcDst != '\0'); i++)
    {
        cTmp = (long)(tolower(*pcSrc) - tolower(*pcDst));
        if (cTmp != 0)
        {
            return cTmp;
        }
        pcSrc++;
        pcDst++;
    }
    return 0;
}

/*****************************************************************************
 函数名称    : STB_StrLen()
 函数功能    : 取字符串长度
 下层函数    : 无.
 上层函数    :
 访问全局变量: 无.
 修改全局变量: 无.
 访问到的表  : 无.
 修改到的表  : 无.
 输入参数    : szSrc 字符串起始地址
 输出参数    : 无.
 返回值      : 字符串的长度
 备注        : 无.
******************************************************************************/
int STB_Split(char **Sour,char *Dest,char flag,int Len)
{
  register char *temp;
  register int i=0;

  temp=*Sour;
  for (; ;)
  {
      if (*temp==flag)
      {
          *Sour=temp+1;
          *Dest='\0';
          return 1;
      }
      else
      if (!(*temp))
      {
          *Sour=temp;
          *Dest='\0';
          return 0;
      }
      else
      if (i==Len)
      {
          *Sour=temp;
          *Dest='\0';
          return 2;
      }
      else
      {
          *Dest++=*temp++;
          i++;
      }
  }
}


/*****************************************************************************
 函数名称    : STB_StrToIPAddr()
 函数功能    : 字符串转换成长整型的IP地址
 下层函数    : 无.
 上层函数    :
 访问全局变量: 无.
 修改全局变量: 无.
 访问到的表  : 无.
 修改到的表  : 无.
 输入参数    : string 字符串地址
 输出参数    : ulRet  转换结果
 返回值      : SUCESS,  成功;
               其他,    失败.
 备注        : 无.
******************************************************************************/
ULONG  STB_StrToIPAddr(char  *string, ULONG  *ulRet)
{
    char  stemp[300];
    ULONG  a,b,c,d;

    if (string == NULL || ulRet == NULL)
        return TBS_FAILED;

    if (STB_Split(&string,stemp,'.',300)!=1)
        return TBS_FAILED;

    if (!*stemp)
        return TBS_FAILED;

    if (STB_atou(stemp,&a))
        return TBS_FAILED;

    if (a>255)
        return TBS_FAILED;

    if (STB_Split(&string,stemp,'.',300)!=1)
        return TBS_FAILED;

    if (!*stemp)
        return TBS_FAILED;

    if (STB_atou(stemp,&b))
        return TBS_FAILED;

    if (b>255)
        return TBS_FAILED;

    if (STB_Split(&string,stemp,'.',300)!=1)
        return TBS_FAILED;

    if (!*stemp)
        return TBS_FAILED;

    if (STB_atou(stemp,&c))
        return TBS_FAILED;

    if (c>255)
        return TBS_FAILED;

    if (STB_Split(&string,stemp,'\0',300)!=1)
        return TBS_FAILED;

    if (!*stemp)
        return TBS_FAILED;

    if (STB_atou(stemp,&d))
        return TBS_FAILED;

    if (d>255)
        return TBS_FAILED;

    *ulRet =(a<<24)+(b<<16)+(c<<8)+d;

    return TBS_SUCCESS;
}
/*****************************************************************************
 函数名称    : STB_IPAddrToStr()
 函数功能    : 长整型IP地址转换成字符串
 下层函数    : 无.
 上层函数    :
 访问全局变量: 无.
 修改全局变量: 无.
 访问到的表  : 无.
 修改到的表  : 无.
 输入参数    : ulIPAddr 长整型地址
               string   转换结果
 输出参数    : 无.
 返回值      : SUCESS,  成功;
               其他,    失败.
 备注        : 无.
******************************************************************************/
char  *STB_IPAddrToStr(ULONG  ulIPAddr, char  *string)
{
    if (NULL == string)
        return NULL;
    STB_Sprintf((char *)string,"%d.%d.%d.%d",
        ulIPAddr>>24,(ulIPAddr>>16)&0xff,
		(ulIPAddr>>8)&0xff,ulIPAddr&0xff);
    return string;
}
/*****************************************************************************
 函数名称    : STB_atoul()
 函数功能    : 字符串转换成无符号长整型
 下层函数    : 无.
 上层函数    :
 访问全局变量: 无.
 修改全局变量: 无.
 访问到的表  : 无.
 修改到的表  : 无.
 输入参数    : Sour 字符串地址
 输出参数    : pTemp 转换结果
 返回值      : SUCESS,  成功;
               其他,    失败.
 备注        : 无.
******************************************************************************/
ULONG  STB_atoul(register char *Sour,ULONG  * pTemp)
{
  register unsigned long ulResult=0;

  while (*Sour)
  {
      /* count */
      if (isdigit(*Sour))
      {
          if ((ulResult<429496729) || ((ulResult==429496729) && (*Sour<'6')))
              ulResult = (ulResult*10+(*Sour)-48);
          else
          {
              *pTemp=ulResult;
              return TBS_FAILED;
          }
      }
      else
      {
          *pTemp=ulResult;
          return TBS_FAILED;
      }
      Sour++;
  }
  *pTemp=ulResult;
  return 0;
}

/*****************************************************************************
 函数名称    : STB_atol()
 函数功能    : 字符串转换成长整型
 下层函数    : 无.
 上层函数    :
 访问全局变量: 无.
 修改全局变量: 无.
 访问到的表  : 无.
 修改到的表  : 无.
 输入参数    : szSrc 字符串地址
 输出参数    : pTemp 转换结果
 返回值      : SUCESS,  成功;
               其他,    失败.
 备注        : 无.
******************************************************************************/
ULONG  STB_atol(char  *szSrc, int    *ulVal)
{
    ULONG  ulFlag = 0;
    ULONG  ulTmp;

    if (*szSrc == '-')
    {
        ulFlag = 1;
        szSrc++;
    }

    if (STB_atoul(szSrc, &ulTmp) != 0)
    {
        return TBS_FAILED;
    }

    if (ulFlag == 1)
        *ulVal = 0 - (int   )ulTmp;
    else
        *ulVal = (int   )ulTmp;
    return TBS_SUCCESS;
}

/*****************************************************************************
 函数名称    : STB_atou()
 函数功能    : 字符串转换成无符号整型
 下层函数    : 无.
 上层函数    :
 访问全局变量: 无.
 修改全局变量: 无.
 访问到的表  : 无.
 修改到的表  : 无.
 输入参数    : Sour 字符串地址
 输出参数    : pTemp 转换结果
 返回值      : SUCESS,  成功;
               其他,    失败.
 备注        : 无.
******************************************************************************/
ULONG  STB_atou(char  *Sour,ULONG  * pTemp)
{
  ULONG  uResult = 0;

  while (*Sour)
  {
      /* count */
      if (isdigit(*Sour))
      {
          if ((uResult<6553) || ((uResult==6553) && (*Sour<'6')))
              uResult=(uResult*10+(*Sour)-48);
          else
          {
              *pTemp=uResult;
              return TBS_FAILED;
          }
      }
      else
      {
          *pTemp=uResult;
          return TBS_FAILED;
      }
      Sour++;
  }
  *pTemp=uResult;
  return TBS_SUCCESS;
}

/*****************************************************************************
 函数名称    : STB_atoulx()
 函数功能    : 字符串转换成十六进制整型
 下层函数    : 无.
 上层函数    :
 访问全局变量: 无.
 修改全局变量: 无.
 访问到的表  : 无.
 修改到的表  : 无.
 输入参数    : Sour 字符串地址
 输出参数    : pTemp 转换结果
 返回值      : SUCESS,  成功;
               其他,    失败.
 备注        : 无.
******************************************************************************/
ULONG  STB_atoulx(register char *Sour,ULONG  * pTemp)
{
  register unsigned long ulResult=TBS_SUCCESS;
  register unsigned char ch;

  if (*Sour == '0' && *(Sour + 1) == 'x')
    Sour += 2;

  while (*Sour)
  {
      /* count */
      ch=(UCHAR)toupper(*Sour);
      if (isdigit(ch) || ((ch>='A') && (ch<='F')))
      {
          if (ulResult<0x10000000)
              ulResult=ulResult*16+((ch<='9')?(ch-48):(ch-55));
          else
          {
              *pTemp=ulResult;
              return TBS_FAILED;
          }
      }
      else
      {
          *pTemp=ulResult;
          return TBS_FAILED;
      }
      Sour++;
  }
  *pTemp=ulResult;
  return TBS_SUCCESS;
}


/*****************************************************************************
 函数名称    : STB_IsChar()
 函数功能    : 判断字符是否为字母
 下层函数    : 无.
 上层函数    :
 访问全局变量: 无.
 修改全局变量: 无.
 访问到的表  : 无.
 修改到的表  : 无.
 输入参数    : c 待判断字符
 输出参数    : 无.
 返回值      : SUCESS,  是;
               其他,    否
 备注        : 无.
******************************************************************************/
ULONG  STB_IsChar(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return TBS_SUCCESS;
    else
        return TBS_FAILED;
}

/*****************************************************************************
 函数名称    : STB_sprintf
 函数功能    : 格式化串组织
 下层函数    : 无.
 上层函数    :
 访问全局变量: 无.
 修改全局变量: 无.
 访问到的表  : 无.
 修改到的表  : 无.
 输入参数    : szFormat: 格式化串原型
 输出参数    : szDst   : 格式化结果串
 返回值      : 返回格式化结果的字符串长度
 备注        : 该函数使用需要保证格式化后的长度不能超过1024字符
******************************************************************************/
#define MAX_PRINT_LENGTH  1024
ULONG  STB_Sprintf(char  *szDst, const char  *szFormat, ...)
{
    va_list     ArgList;
    ULONG         ulStrlen;

    /* 字符串不允许过长, 防止内存越界 */
    if(szDst == NULL || strlen( szFormat ) >= ( MAX_PRINT_LENGTH * 3 / 4 ) )
    {
        return 0;
    }

    va_start ( ArgList, szFormat );
    (_VOID   )vsprintf ( szDst, szFormat, ArgList );
    va_end ( ArgList );

    /* 打印的字符串是否过长 */
    ulStrlen = STB_StrLen(szDst );
    if( ulStrlen >=  MAX_PRINT_LENGTH )
        return 0;

    return ulStrlen;

}




char *strchrR(char	*s,	int	c)
{
	char	*fs;

	for (fs = 0;; ++s) {
	    if (*s == c) fs = s;
	    if (!*s) return fs;
	}
}







/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/



