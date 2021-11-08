
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
 ��������    : STB_StrNiCmp()
 ��������    : ���Դ�Сд�ĵ��ַ����Ƚ�
 �²㺯��    : ��.
 �ϲ㺯��    :
 ����ȫ�ֱ���: ��.
 �޸�ȫ�ֱ���: ��.
 ���ʵ��ı�  : ��.
 �޸ĵ��ı�  : ��.
 �������    : szSrc, �ַ���1����ʼ��ַ.
               szDst, �ַ���2����ʼ��ַ.
 �������    : ��.
 ����ֵ      : ��ֵ,�� szSrc > szDst
               ��ֵ,�� szSrc > szDst
               0   ,�� szSrc = szDst
 ��ע        : ��.
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
 ��������    : STB_StrLen()
 ��������    : ȡ�ַ�������
 �²㺯��    : ��.
 �ϲ㺯��    :
 ����ȫ�ֱ���: ��.
 �޸�ȫ�ֱ���: ��.
 ���ʵ��ı�  : ��.
 �޸ĵ��ı�  : ��.
 �������    : szSrc �ַ�����ʼ��ַ
 �������    : ��.
 ����ֵ      : �ַ����ĳ���
 ��ע        : ��.
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
 ��������    : STB_StrToIPAddr()
 ��������    : �ַ���ת���ɳ����͵�IP��ַ
 �²㺯��    : ��.
 �ϲ㺯��    :
 ����ȫ�ֱ���: ��.
 �޸�ȫ�ֱ���: ��.
 ���ʵ��ı�  : ��.
 �޸ĵ��ı�  : ��.
 �������    : string �ַ�����ַ
 �������    : ulRet  ת�����
 ����ֵ      : SUCESS,  �ɹ�;
               ����,    ʧ��.
 ��ע        : ��.
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
 ��������    : STB_IPAddrToStr()
 ��������    : ������IP��ַת�����ַ���
 �²㺯��    : ��.
 �ϲ㺯��    :
 ����ȫ�ֱ���: ��.
 �޸�ȫ�ֱ���: ��.
 ���ʵ��ı�  : ��.
 �޸ĵ��ı�  : ��.
 �������    : ulIPAddr �����͵�ַ
               string   ת�����
 �������    : ��.
 ����ֵ      : SUCESS,  �ɹ�;
               ����,    ʧ��.
 ��ע        : ��.
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
 ��������    : STB_atoul()
 ��������    : �ַ���ת�����޷��ų�����
 �²㺯��    : ��.
 �ϲ㺯��    :
 ����ȫ�ֱ���: ��.
 �޸�ȫ�ֱ���: ��.
 ���ʵ��ı�  : ��.
 �޸ĵ��ı�  : ��.
 �������    : Sour �ַ�����ַ
 �������    : pTemp ת�����
 ����ֵ      : SUCESS,  �ɹ�;
               ����,    ʧ��.
 ��ע        : ��.
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
 ��������    : STB_atol()
 ��������    : �ַ���ת���ɳ�����
 �²㺯��    : ��.
 �ϲ㺯��    :
 ����ȫ�ֱ���: ��.
 �޸�ȫ�ֱ���: ��.
 ���ʵ��ı�  : ��.
 �޸ĵ��ı�  : ��.
 �������    : szSrc �ַ�����ַ
 �������    : pTemp ת�����
 ����ֵ      : SUCESS,  �ɹ�;
               ����,    ʧ��.
 ��ע        : ��.
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
 ��������    : STB_atou()
 ��������    : �ַ���ת�����޷�������
 �²㺯��    : ��.
 �ϲ㺯��    :
 ����ȫ�ֱ���: ��.
 �޸�ȫ�ֱ���: ��.
 ���ʵ��ı�  : ��.
 �޸ĵ��ı�  : ��.
 �������    : Sour �ַ�����ַ
 �������    : pTemp ת�����
 ����ֵ      : SUCESS,  �ɹ�;
               ����,    ʧ��.
 ��ע        : ��.
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
 ��������    : STB_atoulx()
 ��������    : �ַ���ת����ʮ����������
 �²㺯��    : ��.
 �ϲ㺯��    :
 ����ȫ�ֱ���: ��.
 �޸�ȫ�ֱ���: ��.
 ���ʵ��ı�  : ��.
 �޸ĵ��ı�  : ��.
 �������    : Sour �ַ�����ַ
 �������    : pTemp ת�����
 ����ֵ      : SUCESS,  �ɹ�;
               ����,    ʧ��.
 ��ע        : ��.
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
 ��������    : STB_IsChar()
 ��������    : �ж��ַ��Ƿ�Ϊ��ĸ
 �²㺯��    : ��.
 �ϲ㺯��    :
 ����ȫ�ֱ���: ��.
 �޸�ȫ�ֱ���: ��.
 ���ʵ��ı�  : ��.
 �޸ĵ��ı�  : ��.
 �������    : c ���ж��ַ�
 �������    : ��.
 ����ֵ      : SUCESS,  ��;
               ����,    ��
 ��ע        : ��.
******************************************************************************/
ULONG  STB_IsChar(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return TBS_SUCCESS;
    else
        return TBS_FAILED;
}

/*****************************************************************************
 ��������    : STB_sprintf
 ��������    : ��ʽ������֯
 �²㺯��    : ��.
 �ϲ㺯��    :
 ����ȫ�ֱ���: ��.
 �޸�ȫ�ֱ���: ��.
 ���ʵ��ı�  : ��.
 �޸ĵ��ı�  : ��.
 �������    : szFormat: ��ʽ����ԭ��
 �������    : szDst   : ��ʽ�������
 ����ֵ      : ���ظ�ʽ��������ַ�������
 ��ע        : �ú���ʹ����Ҫ��֤��ʽ����ĳ��Ȳ��ܳ���1024�ַ�
******************************************************************************/
#define MAX_PRINT_LENGTH  1024
ULONG  STB_Sprintf(char  *szDst, const char  *szFormat, ...)
{
    va_list     ArgList;
    ULONG         ulStrlen;

    /* �ַ������������, ��ֹ�ڴ�Խ�� */
    if(szDst == NULL || strlen( szFormat ) >= ( MAX_PRINT_LENGTH * 3 / 4 ) )
    {
        return 0;
    }

    va_start ( ArgList, szFormat );
    (_VOID   )vsprintf ( szDst, szFormat, ArgList );
    va_end ( ArgList );

    /* ��ӡ���ַ����Ƿ���� */
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



