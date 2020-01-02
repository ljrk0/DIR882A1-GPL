#ifdef __cplusplus
extern "C" {
#endif
#include <ctype.h>
#include "cli_private.h"
#include "cli_term.h"

ULONG  _isfloatstr(char  * pStr)
{
    char  * p;
    char  dot;  /* dot already found */

    if(pStr == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    dot = 0;
    p = pStr;

    /* Check the first character */
    if(!isdigit(*p))
    {
        if(*p == '-')
        {
            p++;
        }
        else if((*p++) == '.')
        {
            dot = 1;
        }
        else
        {
            return TBS_FAILED;
        }
    }

    /* Check the left part of the string */
    while(*p)
    {
        if( !isdigit(*p) )
        {
            if(*p == '.')
            {
                if(dot == 0)
                {
                    dot = 1;
                }
                else
                {
                    return TBS_FAILED;
                }
            }
            else
            {
                return TBS_FAILED;
            }
        }
        p++;
    }

    return TBS_SUCCESS;

} /* End of function _isfloatstr() */

ULONG  _isintstr(char  * pStr)
{
    char  * p;

    if(pStr == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    p = pStr;
    if((!isdigit(*p)) && (*p++ != '-'))
        return TBS_FAILED;
    while(*p)
    {
        if( !isdigit(*p) )
            return TBS_FAILED;
        p++;
    }

    return TBS_SUCCESS;
} /* End of function _isintstr() */

char  * _btrim (char  * pStr)
{
    char * p = _ltrim(pStr);

    return _rtrim(p);
}

char * _ltrim(char  *pStr)
{
    char * p;

    if((pStr == NULL) || (*pStr == 0))
    {
        return pStr;
    }

    p = pStr;

    while (*p == ' ' || *p == '\t')
        p++;

    return p;
}

char * _rtrim(char  *pStr)
{
    char * p;

    if((pStr == NULL) || (*pStr == 0))
    {
        return pStr;
    }

    p = &pStr[strlen(pStr) - 1];

    while (*p == ' ' || *p == '\t')
        *p-- = '\0';

    return pStr;
}

_VOID    _TrimString( char * pDst, char  * pSrc )
{
    ULONG  ulFlag = 0;           /* 串标志,0:前,1:中,2:尾 */
    ULONG  ulPreMidBlock = 1;    /* 前一字符第一个中间空格,0:no,1:yes */

    if (pDst == NULL || pSrc == NULL)
    {
        MT_ERRLOG(0);
        return ;
    }

    for( ; 0 != *pSrc; )
    {
        if( 0 == ulFlag )
        {
            if( 0x20 != *pSrc )
            {
                *pDst++ = *pSrc;
                ulFlag = 1;
                ulPreMidBlock = 0;
            }
            pSrc ++;
        }
        else
        {
            if( 0 == ulPreMidBlock )
            {
                if( 0x20 == *pSrc )
                {
                    ulPreMidBlock = 1;
                }
                *pDst++ = *pSrc++;
            }
            else
            {
                if( 0x20 != *pSrc )
                {
                    ulPreMidBlock = 0;
                    *pDst++ = *pSrc;
                }
                pSrc ++;
            }
        }
    }

    *pDst = 0 ;
    return ;
}

ULONG  strmatch(char  *s8Src, char  *s8Dest)
{
    char  *src,*dest;

    if((s8Src == NULL) || (s8Dest == NULL))
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    src = s8Src;
    dest = s8Dest;

    /* Empty source string and longer source string cause an error */
    if((*src == 0) || (strlen(src) > strlen(dest)))
    {
        return TBS_FAILED;
    }

    while(*src!=0)
    {
        if(toupper(*src)!=toupper(*dest))
        {
            return TBS_FAILED;
        }
        src++;
        dest++;
    }

    /* The return value 0 stands for matching successfully */
    return TBS_SUCCESS;

} /* End of function strmatch() */

ULONG  _isempty(char  * pStr)
{

    if(pStr == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    if(*pStr == 0)
    {
        return TBS_SUCCESS;
    }
    else
    {
        return TBS_FAILED;
    }

} /* End of function _isempty() */


ULONG  _VerifyFixedString(char  *pStr, ULONG  ulLen)
{
    if(pStr == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    if((ULONG )strlen(pStr) == ulLen)
    {
        return TBS_SUCCESS;
    }
    else
    {
        return TBS_FAILED;
    }
} /* End of function _VerifyFixedString() */


ULONG  _VerifyAndCvtDwordI(char  * szNum, int    dwLow, int    dwHigh, int    * pdwValue)
{
    int    dwData;
    char  *pTemp, buf[32];

    if((szNum == NULL) || (pdwValue == NULL))
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    pTemp = _btrim(szNum);

    if (!strlen (pTemp) || _isintstr (pTemp) != 0)
        return TBS_FAILED;

    dwData = atol (pTemp);
    STB_Sprintf (buf, "%d", dwData);
    while (*pTemp == '0' && *(pTemp + 1) != 0)
        pTemp ++;
    if (strcmp (pTemp, buf))
        return TBS_FAILED;

    if ((dwData < dwLow) || (dwData > dwHigh))
         return TBS_FAILED;
    *pdwValue = dwData;

    return TBS_SUCCESS;

} /* End of function _VerifyAndCvtDwordI() */

ULONG  _VerifyAndCvtDword (char  * szNum, ULONG  dwLow, ULONG  dwHigh, ULONG  *
pdwValue)
{
    ULONG  dwData;
    char  *pTemp, buf[32];

    if((szNum == NULL) || (pdwValue == NULL))
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    pTemp = _btrim(szNum);

    if (!strlen (pTemp) || _isdecstr (pTemp) != 0)
        return TBS_FAILED;

    dwData = (ULONG ) atol (pTemp);
    STB_Sprintf (buf, "%u", dwData);
    while (*pTemp == '0' && *(pTemp + 1) != 0)
        pTemp ++;
    if (strcmp (pTemp, buf))
        return TBS_FAILED;

    if ((dwData < dwLow) || (dwData > dwHigh))
         return TBS_FAILED;
    *pdwValue = dwData;

    return TBS_SUCCESS;
}

ULONG  _VerifyAndCvtDwordH (char  * szNum, ULONG  dwLow, ULONG  dwHigh, ULONG  *
pdwValue)
{
    ULONG  dwData;
    char  *pTemp;

    if((szNum == NULL) || (pdwValue == NULL))
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    pTemp = _btrim(szNum);

    if (!strlen (pTemp) || _isxdecstr (pTemp) != 0)
        return TBS_FAILED;

    //如果范围越界，直接返回失败
    if ((strlen(pTemp) > 10)
     || (*(pTemp+1) != 'x' && (strlen(pTemp) > 8)))
        return TBS_FAILED;

    dwData = (ULONG ) _strtol (pTemp, 16);

    if ((dwData < dwLow) || (dwData > dwHigh))
         return TBS_FAILED;
    *pdwValue = dwData;

    return TBS_SUCCESS;
}

ULONG  _isdecstr (char  * pStr)
{
    char  * p;

    if(pStr == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    for (p = pStr; *p; p++)
        if (!isdigit (*p)) return TBS_FAILED;

    return TBS_SUCCESS;
}

ULONG  _isxdecstr (char  * pStr)
{
    char  * p;

    if(pStr == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    if ( (*pStr == '0') && (tolower(*(pStr+1)) == 'x') )
        pStr += 2;

    for (p = pStr; *p; p++)
    {
        if (!isxdigit (*p))
            return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

ULONG  _DateOrTimeToUl(char  *szDate, ULONG  *pulDate)
{
    char  *pcDst, *pcSrc ;
    UCHAR  ucType;
    char  szSrc[20];
    ULONG  ulTime[3];

    if (szDate == NULL || pulDate == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    strcpy(szSrc, szDate);

    pcSrc = szSrc;
    pcDst = szSrc;
    ucType = 0;
    for (; ;)
    {
        if (*pcSrc <= '9' && *pcSrc >= '0')
            pcSrc++;

        else if (*pcSrc == '-' || *pcSrc == ':'|| *pcSrc == '\0')
        {
            if (ucType > 2)
                return TBS_FAILED;

            if (*pcSrc == '\0')
            {
                if ((ULONG )_atoul(pcDst, &ulTime[ucType]) != TBS_SUCCESS)
                   return TBS_FAILED;
                break;
            }
            *pcSrc = 0;
            if ((ULONG )_atoul(pcDst, &ulTime[ucType]) != TBS_SUCCESS)
                return TBS_FAILED;

            pcSrc++;
            pcDst = pcSrc;
            ucType++;
        }
        else
        {
            return TBS_FAILED;
        }
    }
    *pulDate = (ulTime[0] << 16) | (ulTime[1] << 8) | (ulTime[2]);
    return TBS_SUCCESS;
}

ULONG  _VerifyDate (char  *szDate1)
{
    char  *pcTmp, *pcDate ;
    char  szTmp[10] = "", szDate[20];
    UCHAR  ucType;
    ULONG  ulNum[3];
    ULONG  ulLimit[3][2] = {{2007, 2099},
                          {1,    12},
                          {1,    31}};
    ULONG  ulMaxDay[12] = {31, 29, 31, 30,
                         31, 30, 31, 31,
                         30, 31, 30, 31};

    if (szDate1 == NULL || strlen(szDate1) > 10 || szDate1[0] == 0)
        return TBS_FAILED;

    strcpy(szDate, szDate1);

    pcTmp  = szTmp;
    pcDate = szDate;
    ucType = 0;
    for (;;)
    {
        if (*pcDate <= '9' && *pcDate >= '0')
            *pcTmp++ = *pcDate++;

        else if (*pcDate == '-' || *pcDate == '\0')
        {
            if (ucType > 2)
                return TBS_FAILED;

            *pcTmp = 0;
            if ((ULONG )_atoul(szTmp, &ulNum[ucType]) != TBS_SUCCESS)
                return TBS_FAILED;

            if (ulNum[ucType] > ulLimit[ucType][1]|| ulNum[ucType] < ulLimit[ucType][0])
                return TBS_FAILED;

            if (*pcDate == '\0')
                break;

            pcDate++;
            pcTmp = szTmp;
            *szTmp = '\0';
            ucType++;
        }
        else
        {
            return TBS_FAILED;
        }
    }
    if (ucType < 2)
        return TBS_FAILED;

    if (ulNum[0] % 4 != 0)
        ulMaxDay[1] = 28;

    if (ulNum[2] > ulMaxDay[ulNum[1] - 1])
        return TBS_FAILED;

    return TBS_SUCCESS;
}

ULONG  _VerifyTime (char  *szTime1)
{
    char  *pcTmp, *pcTime ;
    char  szTmp[10] = "", szTime[20];
    UCHAR  ucType;
    ULONG  ulNum;
    ULONG  ulLimit[3] = {23, 59, 59};

    if (szTime1 == NULL || strlen(szTime1) > 8 || szTime1[0] == '\0')
        return TBS_FAILED;

    strcpy(szTime, szTime1);

    pcTmp  = szTmp;
    pcTime = szTime;
    ucType = 0;
    for(; ;)
    {
        if (*pcTime <= '9' && *pcTime >= '0')
            *pcTmp++ = *pcTime++;

        else if (*pcTime == ':' || *pcTime == '\0')
        {
            if (ucType > 2 || szTmp[0] == '\0')
                return TBS_FAILED;
            *pcTmp = 0;
            if ((ULONG )_atoul(szTmp, &ulNum) != TBS_SUCCESS)
                return TBS_FAILED;
            if (ulNum > ulLimit[ucType])
                return TBS_FAILED;
            if (*pcTime == '\0')
                break;
            pcTime++;
            pcTmp = szTmp;
            *szTmp = '\0';
            ucType++;
        }
        else
        {
            return TBS_FAILED;
        }
    }

    if (ucType < 2)
        return TBS_FAILED;
    return TBS_SUCCESS;
}


ULONG  _VerifyHostIPAddress(char  *szIPAdd)
{
    char  *pData;
    ULONG  ulCount = 0, ulIpByte = 0;
    char  pTmp[4];

    pData = szIPAdd;
    pTmp[0] = pTmp[1] = pTmp[2] = pTmp[3] = '\0';
    if(pData == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    for(;;)
    {
        if(isdigit(*pData))
        {
            if(ulIpByte > 2)
                return TBS_FAILED;
            pTmp[ulIpByte] = *pData;
        }
        else if((*pData == '.') || (*pData == '\0'))
        {
            ulCount ++;
            ulIpByte = (ULONG )-1;
            if(ulCount > 4)
                return TBS_FAILED;
            if((atoi(pTmp) > 255) || (pTmp[0] == '\0'))
                return TBS_FAILED;
            if((pTmp[0] == '0') && (pTmp[1] == '\0')
               && (ulCount == 1))
                return TBS_FAILED;
            pTmp[0] = pTmp[1] = pTmp[2] = pTmp[3] = '\0';
            if(*pData == '\0')
            {
                if(ulCount < 4)
                    return TBS_FAILED;
                return TBS_SUCCESS;
            }
        }
        else return TBS_FAILED;
        pData ++;
        ulIpByte ++;
    }
}

ULONG  _VerifyGeneralIPAddress(char  *szIPAdd)
{
    char  *pData;
    ULONG  ulCount = 0, ulIpByte = 0;
    char  pTmp[4];

    pData = szIPAdd;
    pTmp[0] = pTmp[1] = pTmp[2] = pTmp[3] = '\0';
    if(pData == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    for(;;)
    {
        if(isdigit(*pData))
        {
            if(ulIpByte > 2)
                return TBS_FAILED;
            pTmp[ulIpByte] = *pData;
        }
        else if((*pData == '.') || (*pData == '\0'))
        {
            ulCount ++;
            ulIpByte = (ULONG )-1;
            if(ulCount > 4)
                return TBS_FAILED;
            if((atoi(pTmp) > 255) || (pTmp[0] == '\0'))
                return TBS_FAILED;
            pTmp[0] = pTmp[1] = pTmp[2] = pTmp[3] = '\0';
            if(*pData == '\0')
            {
                if(ulCount < 4)
                    return TBS_FAILED;
                return TBS_SUCCESS;
            }
        }
        else return TBS_FAILED;
        pData ++;
        ulIpByte ++;
    }
}



ULONG  _VerifyMASKAddress(char  *szMaskAdd)
{
    char  *pData;
    UCHAR ucMask = 0xff;
    UCHAR ucTmpMask;
    UCHAR ucByte;
    ULONG  ulCount = 0, ulIpByte = 0;
    char  pTmp[4];
    ULONG  i, ulAllOne = 1;

    pData = szMaskAdd;
    pTmp[0] = pTmp[1] = pTmp[2] = pTmp[3] = '\0';
    if(pData == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    for(;;)
    {
        if(isdigit(*pData))
        {
            if(ulIpByte > 2)
                return TBS_FAILED;
            pTmp[ulIpByte] = *pData;
        }
        else if((*pData == '.') || (*pData == '\0'))
        {
            ulCount ++;
            ulIpByte = (ULONG )-1;
            if (atoi(pTmp) > 255)
                return TBS_FAILED;
            ucByte = (UCHAR)atoi(pTmp);
            if((ulCount > 4) || (pTmp[0] == '\0') )
                return TBS_FAILED;
            ucTmpMask = ucMask;
            for(i = 0; i < 8; i++)
            {
                if(ucByte == ucTmpMask)
                    break;
                ucTmpMask = (char )(ucTmpMask << 1);
            }
            if((i == 8) && (ucByte != 0))
                return TBS_FAILED;
            if((ucTmpMask != 0xff) && (ulAllOne == 1))
                ulAllOne = 0;
            else if((ucTmpMask != 0x00) && (ulAllOne == 0))
                return TBS_FAILED;
            pTmp[0] = pTmp[1] = pTmp[2] = pTmp[3] = '\0';
            if(*pData == '\0')
            {
                if(ulCount < 4)
                    return TBS_FAILED;
                return TBS_SUCCESS;
            }
        }
        else return TBS_FAILED;
        pData ++;
        ulIpByte ++;
    }
}


ULONG  _VerifyMACAddress(char  *szMACAdd)
{
    char  *pData;
    ULONG  ulCount = 0, ulIpByte = 0;
    char  pTmp[3];

    pData = szMACAdd;
    pTmp[0] = pTmp[1] = pTmp[2] = '\0';
    if(pData == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    for(;;)
    {
        if (((*pData >= '0') && (*pData <= '9'))
          ||((*pData >= 'a') && (*pData <= 'f'))
          ||((*pData >= 'A') && (*pData <= 'F')))
        {
            if(ulIpByte > 1)
                return TBS_FAILED;
            pTmp[ulIpByte] = *pData;
        }
        else if((*pData == '-') || (*pData == '\0'))
        {
            ulCount ++;
            if(ulIpByte != 2)
                return TBS_FAILED;
            ulIpByte = (ULONG )-1;
            if((ulCount > 6) || (pTmp[0] == '\0'))
                return TBS_FAILED;
            pTmp[0] = pTmp[1] = pTmp[2] = '\0';
            if(*pData == '\0')
            {
                if(ulCount < 6)
                    return TBS_FAILED;
                return TBS_SUCCESS;
            }
        }
        else return TBS_FAILED;
        pData ++;
        ulIpByte ++;
    }

}

ULONG  _AffirmStrBeLower(char  *szStr)
{
    ULONG  ulTail, ulCur;

    if (szStr == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    ulTail = strlen(szStr);
    for(ulCur = 0; ulCur < ulTail; ulCur++)
    {
        if (*(szStr + ulCur) <= 'Z' && *(szStr + ulCur) >= 'A')
        {
            *(szStr + ulCur) += 'a' - 'A';
        }
    }
    return TBS_SUCCESS;
}

int _Split(char  **Sour,char  *Dest,char  flag,int Len)
{
  register char  *temp;
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

ULONG  _StrToIPAddr(char  *string, ULONG  *ulRet)
{
    char  stemp[300];
    ULONG  a,b,c,d;

    if (_Split(&string,stemp,'.',300)!=1)
        return TBS_FAILED;

    if (!*stemp)
        return TBS_FAILED;

    if (_atou(stemp,&a))
        return TBS_FAILED;

    if (a>255)
        return TBS_FAILED;

    if (_Split(&string,stemp,'.',300)!=1)
        return TBS_FAILED;

    if (!*stemp)
        return TBS_FAILED;

    if (_atou(stemp,&b))
        return TBS_FAILED;

    if (b>255)
        return TBS_FAILED;

    if (_Split(&string,stemp,'.',300)!=1)
        return TBS_FAILED;

    if (!*stemp)
        return TBS_FAILED;

    if (_atou(stemp,&c))
        return TBS_FAILED;

    if (c>255)
        return TBS_FAILED;

    if (_Split(&string,stemp,'\0',300)!=1)
        return TBS_FAILED;

    if (!*stemp)
        return TBS_FAILED;

    if (_atou(stemp,&d))
        return TBS_FAILED;

    if (d>255)
        return TBS_FAILED;

    *ulRet =htonl((a<<24)+(b<<16)+(c<<8)+d);

    return TBS_SUCCESS;
}

char  *_IPAddrToStr(ULONG  ulIPAddr, char  *string)
{
    if (NULL == string)
    {
        MT_ERRLOG(0);
        return 0;//for 代码审查
    }
    /* IPAddr都是网络字节顺序 */
    ulIPAddr=ntohl(ulIPAddr);
    STB_Sprintf((char  *)string,"%d.%d.%d.%d",
        ulIPAddr>>24,(ulIPAddr>>16)&0xff,
        (ulIPAddr>>8)&0xff,ulIPAddr&0xff);
    return string;
}

int _atoulx(register char  *Sour,unsigned long * pTemp)
{
  register unsigned long ulResult=0;
  register unsigned char ch;
  if (Sour == NULL || pTemp == NULL)
  {
      MT_ERRLOG(0);
      return 1;
  }

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
              return 1;
          }
      }
      else
      {
          *pTemp=ulResult;
          return 1;
      }
      Sour++;
  }
  *pTemp=ulResult;
  return 0;
}

int _atoul(register char  *Sour,ULONG  * pTemp)
/* 0:success 1:failure */
{
    ULONG  ulResult=0;

    if (Sour == NULL || pTemp == NULL)
    {
        MT_ERRLOG(0);
        return 1;
    }

    while (*Sour)
    {
        /* count */
        if (isdigit(*Sour))
        {
            if ((ulResult<429496729) || ((ulResult==429496729) && (*Sour<'6')))
                ulResult = (ULONG )(ulResult*10 + (UCHAR)((*Sour)-48));
            else
            {
                *pTemp=ulResult;
                return 1;
            }
        }
        else
        {
            *pTemp=ulResult;
            return 1;
        }
        Sour++;
    }
    *pTemp=ulResult;
    return 0;
}

int _atol(char  *szSrc, int    *ulVal)
{
    ULONG  ulFlag = 0;
    ULONG  ulTmp;

    if (szSrc == NULL || ulVal == NULL)
    {
        MT_ERRLOG(0);
        return 1;
    }
    if (*szSrc == '-')
    {
        ulFlag = 1;
        szSrc++;
    }

    if (_atoul(szSrc, &ulTmp) != 0)
    {
        return 1;
    }

    if (ulFlag == 1)
        *ulVal = 0 - (char )ulTmp;
    else
        *ulVal = (char )ulTmp;
    return 0;
}

int _atou(char  *Sour,ULONG  * pTemp)
/* 0:success 1:failure */
{
  ULONG  uResult=0;

  if (Sour == NULL || pTemp == NULL)
  {
      MT_ERRLOG(0);
      return 1;
  }

  while (*Sour)
  {
      /* count */
      if (isdigit(*Sour))
      {
          if ((uResult<6553) || ((uResult==6553) && (*Sour<'6')))
              uResult = (ULONG )(uResult * 10 + (UCHAR)(*Sour - 48));
          else
          {
              *pTemp=uResult;
              return 1;
          }
      }
      else
      {
          *pTemp=uResult;
          return 1;
      }
      Sour++;
  }
  *pTemp=uResult;
  return 0;
}

int _atoux(register char  *Sour,unsigned short * pTemp)
/* 0:success 1:failure */
{
  register unsigned short uResult=0;
  register unsigned char ch;

  if (Sour == NULL || pTemp == NULL)
  {
      MT_ERRLOG(0);
      return 1;
  }

  while (*Sour)
  {
      /* count */
      ch=(UCHAR)toupper(*Sour);
      if (isdigit(ch) || ((ch >= 'A') && (ch <= 'F')))
      {
          if (uResult<0x1000)
              uResult=(USHORT)(uResult*16+((ch<='9')?(ch-48):(ch-55)));
          else
          {
              *pTemp=uResult;
              return 1;
          }
      }
      else
      {
          *pTemp=uResult;
          return 1;
      }
      Sour++;
  }
  *pTemp=uResult;
  return 0;
}

ULONG  _strtol(const char  *str, ULONG  base)
{
    ULONG  val = 0 ;
    char  *ptr ;

    if (str == NULL || base == 0)
    {
        MT_ERRLOG(0);
        return 1;
    }

    while (isspace(*str))
        str++ ;

    ptr = (char  *)str ;
    if ( (*ptr == '0') && (tolower(*(ptr+1)) == 'x') )
    {
        if (!base)
            base = 16 ;
        ptr += 2 ;
    }
    if (!base)
        base = 10 ;

    if (strlen(ptr) > 8)
        return 0 ;

    while (*ptr)
    {
        if ( (base == 16 && !isxdigit(*ptr)) || (base == 10 && !isdigit(*ptr)))
        {
            return 0 ;
        }

        val = val*base + (isdigit(*ptr) ? (ULONG)(int)(*ptr-'0') : \
                            (islower(*ptr) ? (ULONG)(int)(*ptr-'a'+10) : \
                            (ULONG)(int)(*ptr-'A'+10))) ;
        ptr++ ;
    }

    return val ;
} /* End of function _strtol() */

ULONG  _IsChar(char  c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return 0;
    else
        return 1;
}

ULONG  _IsCmdChar(char  c)
{
    if (_IsChar(c) == TBS_SUCCESS || c == '-' || isdigit(c))
        return TBS_SUCCESS;
    else
        return TBS_FAILED;
}

#ifdef __cplusplus
}
#endif

