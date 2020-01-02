
#include "dm_internal.h"
#include "device_external_type.h"

#ifdef DMALLOC_CHECK_ENABLED
#warning ***** compilation with dmalloc *****
#include "dmalloc.h"
#endif

INT8* DmConvertIntToStr(INT32 lNum)
{
    INT8 *pucStr = (INT8 *)malloc(sizeof(INT8)*16);
    if(NULL == pucStr)
    {
        return NULL;
    }
    memset(pucStr,0,sizeof(INT8)*16);
    snprintf(pucStr, sizeof(INT8)*16, "%d", lNum);

    return pucStr;
}

BOOL8 DmConvertStringToCmHostNameOrIp(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{

    UINT8 *pucStr = pucStringValue;
    CM_HOST_NAME_OR_IP_ADDR_T *pValue = (CM_HOST_NAME_OR_IP_ADDR_T *)pStructValue;
    BOOL8  bDotFlag = 0;
    BOOL8  bDigitFlag = 0;

    /*judge host, ipv4, ipv6*/
    UINT32 i = 0;
    pValue->choice = INVALID_ADDR; /* ipv4 */
    for(i=0; i< strlen((char*)(pucStr)); i++)
    {
        if(pucStr[i] == ':')
        {
            pValue->choice = IPV6_ADDR; /* ipv6 */
            break;
        }
        else if(isalpha(pucStr[i]))
        {
            pValue->choice = HOSTNAME_ADDR; /* host name */
            break;
        }
        else if(pucStr[i] == '.')
        {
            bDotFlag = 1;
        }
        else if(isdigit(pucStr[i]))
        {
            bDigitFlag = 1;
        }
    }

    if((INVALID_ADDR == pValue->choice)&& bDotFlag && bDigitFlag)
    {
        pValue->choice = IPV4_ADDR;
    }
    

    /* call the convert func */
    if(IPV4_ADDR == pValue->choice)   /*ipv4*/
    {
        inet_pton(AF_INET, (char*)(pucStr), pValue->u.aucIpv4Octet);
    }
    else if(IPV6_ADDR == pValue->choice) /*ipv6*/
    {
        inet_pton(AF_INET6, (char*)(pucStr), pValue->u.aucIpv6Octet);
    }
    else if(HOSTNAME_ADDR == pValue->choice)
    {
        strncpy((char*)(pValue->u.HostName), (char*)(pucStr), sizeof(pValue->u.HostName)-1);
    }
    

    return DM_TRUE;
}

BOOL8 DmConvertCmHostNameOrIpToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT8 *pucStr = pucStringValue;
    CM_HOST_NAME_OR_IP_ADDR_T *pValue = pStructValue;
    INT8 aucIPAddr[160] = {0}; 

    /* ipv4 */
    if(IPV4_ADDR == pValue->choice)
    {
        inet_ntop(AF_INET, pValue->u.aucIpv4Octet, (char*)aucIPAddr, 160); 
        strcpy(pucStr, aucIPAddr);
    }
    /* ipv6 */
    else if(IPV6_ADDR == pValue->choice)
    {
        inet_ntop(AF_INET6, pValue->u.aucIpv6Octet, (char*)aucIPAddr, 160); 
        strcpy(pucStr, aucIPAddr); 
    }
    /* url */
    else if(HOSTNAME_ADDR == pValue->choice)
    {
        strcpy(pucStr, pValue->u.HostName);
    }
    
    *plStringCnt = strlen(pucStr);


    return DM_TRUE;
}

BOOL8 DmConvertImsiToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{

    UINT32 i = 0;
    UINT32 k = 0;
    INT8 *pucStr = pucStringValue;
    CM_IMSI_T *pImsi = pStructValue;
    *plStringCnt = 0;
    
    for(i = 0; i < pImsi->ucLength; i++)
    {
        pucStr[k++] = (pImsi->aucImsi[i] & 0x0F) + 0x30;
        pucStr[k++] = ((pImsi->aucImsi[i] & 0xF0)>>4) + 0x30;
    }
    if(k > 0)
    {
        if(0x3F == pucStr[k-1])
        {
            pucStr[k-1] = 0;
        }
        else
        {
            pucStr[k] = 0;
        }
        *plStringCnt = strlen(pucStr);
    }
    return DM_TRUE;
}

BOOL8 DmConvertStringToCmImsi(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{

    UINT8 *pucStr = pucStringValue;
    CM_IMSI_T *pValue = (CM_IMSI_T *)pStructValue;

    UINT32 i = 0;
    UINT8 i1;
    UINT8 i2;

    /* IMSI length is even number */
    if (0 != strlen((char*)(pucStr)))
    {
        if (0 == (strlen((char*)(pucStr)) % 2))
        {
            pValue->ucLength = strlen((char*)(pucStr)) / 2;
            for(i = 0; i < pValue->ucLength;  i++)
            {
                i1 = pucStr[i*2+1] - 48;
                i2 = pucStr[i*2] - 48;
                pValue->aucImsi[i] = (i1 << 4) | i2;
            }
        }
        /* IMSI length is odd number */
        else
        {
            pValue->ucLength = strlen((char*)(pucStr)) / 2 + 1;
            for(i = 0; i < (pValue->ucLength - (UINT32)1);  i++)
            {
                i1 = pucStr[i*2+1] - 48;
                i2 = pucStr[i*2] - 48;
                pValue->aucImsi[i] = (i1 << 4) | i2;
            }
            i2 = pucStr[i*2] - 48;
            pValue->aucImsi[i] = i2 | 0xF0;
        }
    }
    return DM_TRUE;
}

BOOL8 DmConvertImsiArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{

    UINT32 i = 0;
    INT32 j = 0;
    UINT32 k = 0;
    INT8 *pucStr = pucStringValue;
    CM_IMSI_T *pImsi = pStructValue;
    *plStringCnt = 0;
    

    for(j = 0; j < lStructCnt; j++)
    {
        for(i = 0; i < pImsi[j].ucLength; i++)
        {
            pucStr[k++] = (pImsi[j].aucImsi[i] & 0x0F) + 0x30;
            pucStr[k++] = ((pImsi[j].aucImsi[i] & 0xF0)>>4) + 0x30;
        }
        if(0x3F == pucStr[k-1])
        {
            pucStr[k-1] = ',';
        }
        else
        {
            pucStr[k++] = ',';
        }
    }

    if(k > 0)
    {
        pucStr[k-1] = 0;
        *plStringCnt = strlen(pucStr);
    }
    
    return DM_TRUE;
}

BOOL8 DmConvertStringToCmImsiArray(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    INT8 *pucTmp = NULL;
    UINT8 *pucStr = pucStringValue;
    CM_IMSI_T *pValue = (CM_IMSI_T *)pStructValue;

    

    /* call the convert func */
    UINT8 *token = (UINT8*)strtok_r((char*)(pucStr), ",", &pucTmp);
    UINT32 i = 0;
    INT32 cnt = 0;
    while(token != NULL && cnt < lStringCnt)
    {
        /* IMSI length is even number */
        UINT8 i1;
        UINT8 i2;
        if (0 == (strlen((char*)token) % 2))
        {
            pValue->ucLength = strlen((char*)token) / 2;
            for(i = 0; i < pValue->ucLength;  i++)
            {
                i1 = token[i*2+1] - 48;
                i2 = token[i*2] - 48;
                pValue->aucImsi[i] = (i1 << 4) | i2;
            }
        }
        /* IMSI length is odd number */
        else
        {
            pValue->ucLength = strlen((char*)token) / 2 + 1;
            for(i = 0; i < (pValue->ucLength - (UINT32)1);  i++)
            {
                i1 = token[i*2+1] - 48;
                i2 = token[i*2] - 48;
                pValue->aucImsi[i] = (i1 << 4) | i2;
            }
            i2 = token[i*2] - 48;
            pValue->aucImsi[i] = i2 | 0xF0;
        }
        token = (UINT8*)strtok_r(NULL, ",", &pucTmp);
        pValue++;
        cnt++;
    }

    *pulStructCnt = (UINT32)cnt;

    return DM_TRUE;
}

BOOL8 DmConvertStringToCmIpAddr(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{

    UINT8 *pucStr = pucStringValue;
    CM_IP_ADDR_T *pValue = (CM_IP_ADDR_T *)pStructValue;

    /* IPV4 */
    if(strstr((char*)(pucStr),".") != NULL)
    {
        pValue->choice = IPV4_ADDR;
        inet_pton(AF_INET, (char*)(pucStr), pValue->u.aucIpv4Octet);
    }
    /* IPV6 */
    else if(strstr((char*)(pucStr),":") != NULL)
    {
       pValue->choice = IPV6_ADDR;
       inet_pton(AF_INET6, (char*)(pucStr), pValue->u.aucIpv6Octet);
    }
    else
    {
        pValue->choice = INVALID_ADDR;
    }
    
    return DM_TRUE;
}

BOOL8 DmConvertCmIpAddrToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{

    INT8 *pucStr = pucStringValue;
    CM_IP_ADDR_T *pValue = pStructValue;

    if(pValue->choice == IPV4_ADDR)
    {
        inet_ntop(AF_INET, pValue->u.aucIpv4Octet, pucStr, INET_ADDRSTRLEN);
    }
    else if(pValue->choice == IPV6_ADDR)
    {
        inet_ntop(AF_INET6, pValue->u.aucIpv6Octet, pucStr, INET6_ADDRSTRLEN);
    }

    *plStringCnt = strlen(pucStr);

    return DM_TRUE;
}

BOOL8 DmConvertCmPlmnIdToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{


    INT8 *pucStr = pucStringValue;
    CM_PLMN_ID_T *pValue = pStructValue;

    INT8* tmpValue = NULL;
    UINT32 i = 0;
    for(i=0; i<3; i++)
    {
        tmpValue = DmConvertIntToStr(pValue->aucMccDigit[i]);
        strcat((char*)(pucStr), (char*)tmpValue);
        free(tmpValue);
    }

    for(i=0; i<pValue->ucMncCnt; i++)
    {
        tmpValue = DmConvertIntToStr(pValue->aucMncDigit[i]);
        strcat((char*)(pucStr), (char*)tmpValue);
        free(tmpValue);
    }

    *plStringCnt = strlen(pucStr);
    return DM_TRUE;
}

BOOL8 DmConvertStringToCmPlmnId(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    UINT8 *pucStr = pucStringValue;
    CM_PLMN_ID_T *pValue = (CM_PLMN_ID_T *)pStructValue;

    UINT32 i = 0;
    if(0 != strlen((char*)(pucStringValue)))
    {
        for(i=0; i<3; i++)
        {
            pValue->aucMccDigit[i] = pucStr[i] - 48;
        }

        if(strlen((char*)(pucStr)) == 5)
        {
            pValue->ucMncCnt = 2;
        }
        else if (strlen((char*)(pucStr)) == 6)
        {
            pValue->ucMncCnt = 3;
        }
        else
        {
            pValue->ucMncCnt = 0;
        }
        for(i=0; i<pValue->ucMncCnt; i++)
        {
            pValue->aucMncDigit[i] = pucStr[i+3] - 48;
        }
    }
    return DM_TRUE;
}

BOOL8 DmConvertCmPlmnIdArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{


    INT8 *pucStr = pucStringValue;
    CM_PLMN_ID_T *pValue = pStructValue;

    INT8* tmpValue = NULL;
    UINT32 i = 0;
    INT32 j = 0;
    for(j=0; j < lStructCnt; j++)
    {
        for(i=0; i<3; i++)
        {
            tmpValue = DmConvertIntToStr(pValue->aucMccDigit[i]);
            strcat((char*)(pucStr), (char*)tmpValue);
            free(tmpValue);
        }

        for(i=0; i<pValue->ucMncCnt; i++)
        {
            tmpValue = DmConvertIntToStr(pValue->aucMncDigit[i]);
            strcat((char*)(pucStr), (char*)tmpValue);
            free(tmpValue);
        }
        if(j != lStructCnt-1)
        {
            strcat((char*)(pucStr), ",");
        }
    }
    
    *plStringCnt = strlen(pucStr);
    return DM_TRUE;
}

BOOL8 DmConvertStringToCmPlmnIdArray(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    INT8 *pucTmp = NULL;
    UINT8 *pucStr = pucStringValue;
    CM_PLMN_ID_T *pValue = (CM_PLMN_ID_T *)pStructValue;
    CM_PLMN_ID_T tmpValue={0};

    /* call the convert func */
     UINT8 *token = (UINT8*)strtok_r((char*)(pucStr), ",", &pucTmp);
     UINT32 i = 0;

     while(token != NULL)
     {
        strncpy((char*)(tmpValue.aucMccDigit), (char*)token, 3);
        tmpValue.aucMccDigit[0] -= 48;
        tmpValue.aucMccDigit[1] -= 48;
        tmpValue.aucMccDigit[2] -= 48;
        if(strlen((char*)token) == 5)
        {
            strncpy((char*)(tmpValue.aucMncDigit), (char*)(token+3), 2);
            tmpValue.ucMncCnt = 2;
        }
        else
        {
            strncpy((char*)(tmpValue.aucMncDigit), (char*)(token+3), 3);
            tmpValue.ucMncCnt = 3;
            tmpValue.aucMncDigit[2] -= 48;
        }
        tmpValue.aucMncDigit[0] -= 48;
        tmpValue.aucMncDigit[1] -= 48;

        memcpy(&(pValue[i]), &tmpValue, sizeof(CM_PLMN_ID_T));
        token = (UINT8*)strtok_r(NULL, ",", &pucTmp);
        i++;
     }

     *pulStructCnt = i;

    return DM_TRUE;
}

BOOL8 DmConvertDateTimeToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{

    INT8 *pucStr = pucStringValue;
    DATE_TIME_T *pValue = pStructValue;

    INT8 tmpValue[8] = {0};

    if(0 == pValue->Year)
    {
        pValue->Year = 1;
    }

    if(0 == pValue->Month)
    {
        pValue->Month = 1;
    }

    if(0 == pValue->Day)
    {
        pValue->Day = 1;
    }

    /* "yyyy-mm-ddThh:mm:ssZ+hh:mm" */
    memset(tmpValue,0,sizeof(UINT8)*8);
    snprintf(tmpValue,sizeof(UINT8)*8,"%04d",pValue->Year);
    strcat((char*)(pucStr), (char*)tmpValue);
    strcat((char*)(pucStr), "-");

    memset(tmpValue,0,sizeof(UINT8)*8);
    snprintf(tmpValue,sizeof(UINT8)*8,"%02d",pValue->Month);
    strcat((char*)(pucStr), (char*)tmpValue);
    strcat((char*)(pucStr), "-");

    memset(tmpValue,0,sizeof(UINT8)*8);
    snprintf(tmpValue,sizeof(UINT8)*8,"%02d",pValue->Day);
    strcat((char*)(pucStr), (char*)tmpValue);
    strcat((char*)(pucStr), "T");

    memset(tmpValue,0,sizeof(UINT8)*8);
    snprintf(tmpValue,sizeof(UINT8)*8,"%02d",pValue->Hour);
    strcat((char*)(pucStr), (char*)tmpValue);
    strcat((char*)(pucStr), ":");

    memset(tmpValue,0,sizeof(UINT8)*8);
    snprintf(tmpValue,sizeof(UINT8)*8,"%02d",pValue->Minute);
    strcat((char*)(pucStr), (char*)tmpValue);
    strcat((char*)(pucStr), ":");

    memset(tmpValue,0,sizeof(UINT8)*8);
    snprintf(tmpValue,sizeof(UINT8)*8,"%02d",pValue->Second);
    strcat((char*)(pucStr), (char*)tmpValue);

    if(0 != pValue->MicroSecond)
    {
        memset(tmpValue,0,sizeof(UINT8)*8);
        snprintf(tmpValue,sizeof(UINT8)*8,".%03d",pValue->MicroSecond);
        strcat((char*)(pucStr), (char*)tmpValue);
    }

    /*format for timezone*/
    if((0 == pValue->TimeZone.TimeZoneHour)&&(0 == pValue->TimeZone.TimeZoneMinute))
    {
           strcat((char*)(pucStr), "Z");
    }
    else
    {
        memset(tmpValue,0,sizeof(UINT8)*8);
        snprintf(tmpValue,sizeof(UINT8)*8,"%02d",abs(pValue->TimeZone.TimeZoneHour));
        if(pValue->TimeZone.TimeZoneHour >= 0)
        {
               strcat((char*)(pucStr), "+");
        }
        else
        {
               strcat((char*)(pucStr), "-");    
        }
        strcat((char*)(pucStr), (char*)tmpValue);
        strcat((char*)(pucStr), ":");

        memset(tmpValue,0,sizeof(UINT8)*8);
        snprintf(tmpValue,sizeof(UINT8)*8,"%02d",abs(pValue->TimeZone.TimeZoneMinute));
        strcat((char*)(pucStr), (char*)tmpValue);
    }

    *plStringCnt = strlen(pucStr);

    return DM_TRUE;
}

BOOL8 DmConvertStringToDateTime(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    INT8 *pucTmpStr = NULL;
    DATE_TIME_T *pValue = (DATE_TIME_T *)pStructValue;

    BOOL8 bZonePositive = 0;

    memset(pValue, 0, sizeof(DATE_TIME_T));

    /* call the convert func */
    if(strstr((char*)pucStringValue, "+"))
    {
        bZonePositive = 1;
    }
    else
    {
        bZonePositive = 0;
    }

     UINT8 *token = (UINT8*)strtok_r((char*)(pucStringValue), "+-T:Z", &pucTmpStr);
     UINT32 i = 0;
     while(token != NULL)
     {
        switch (i)
        {
            /* year */
            case 0:
                pValue->Year = atoi((char*)token);
                break;
            /* month */
            case 1:
                pValue->Month = atoi((char*)token);
                break;
            /* day */
            case 2:
                pValue->Day = atoi((char*)token);
                break;
            /* hour */
            case 3:
                pValue->Hour = atoi((char*)token);
                break;
            /* minute */
            case 4:
                pValue->Minute = atoi((char*)token);
                break;
            /* second */
            case 5:
                pValue->Second = atoi((char*)token);
                INT8 *pMicroS = strstr((char*)token,".");
                if(NULL != pMicroS)
                {
                    if(strlen(pMicroS) < 4)
                    {
                        INT8 aucTemp[4] = {0};
                        snprintf(aucTemp,sizeof(aucTemp),"%s","000");
                        memcpy(aucTemp,pMicroS+1,strlen(pMicroS)-1);
                        pValue->MicroSecond = atoi((char*)aucTemp);
                    }
                    else
                    {
                        pValue->MicroSecond = atoi((char*)pMicroS+1);
                    }
                }
                else
                {
                    pValue->MicroSecond = 0;
                }
                break;
            /* zone hour */
            case 6:
                if(bZonePositive)
                {
                    pValue->TimeZone.TimeZoneHour = atoi((char*)token);
                }
                else
                {
                    pValue->TimeZone.TimeZoneHour = -atoi((char*)token);
                }
                break;
            /* zone minute */
            case 7:
                if(bZonePositive)
                {
                    pValue->TimeZone.TimeZoneMinute = atoi((char*)token);
                }
                else
                {
                    pValue->TimeZone.TimeZoneMinute = -atoi((char*)token);
                }
                break;
            default:
                break;

        }
        token = (UINT8*)strtok_r(NULL, "-T:Z+", &pucTmpStr);
        i++;
     }

    return DM_TRUE;
}


BOOL8 DmConvertStringToCmHostNameOrIpArray(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    INT8 *pucTmp = NULL;
    UINT8 *pucStr = pucStringValue;
    CM_HOST_NAME_OR_IP_ADDR_T *pValue = (CM_HOST_NAME_OR_IP_ADDR_T *)pStructValue;
    CM_HOST_NAME_OR_IP_ADDR_T tmpValue={0};
    BOOL8  bDotFlag = 0;
    BOOL8  bDigitFlag = 0;

    /* call the convert func */
    UINT8 *token = (UINT8*)strtok_r((char*)(pucStr), ",", &pucTmp);
    UINT32 cnt = 0;

    while(token != NULL)
    {
        /*judge host, ipv4, ipv6*/
        UINT32 i = 0;

        osa_mem_set(&tmpValue, 0, sizeof(CM_HOST_NAME_OR_IP_ADDR_T));
        tmpValue.choice = INVALID_ADDR; /* ipv4 */

        for(i=0; i< strlen((char*)(token)); i++)
        {
            if(token[i] == ':')
            {
                tmpValue.choice = IPV6_ADDR; /* ipv6 */
                break;
            }
            else if(isalpha(token[i]))
            {
                tmpValue.choice = HOSTNAME_ADDR; /* host name */
                break;
            }
            else if(token[i] == '.')
            {
                bDotFlag = 1;
            }
            else if(isdigit(token[i]))
            {
                bDigitFlag = 1;
            }
        }

        if((INVALID_ADDR == tmpValue.choice)&& bDotFlag && bDigitFlag)
        {
            tmpValue.choice = IPV4_ADDR;
        }

        /* call the convert func */
        if(IPV4_ADDR == tmpValue.choice)   /*ipv4*/
        {
            inet_pton(AF_INET, (char*)(token), tmpValue.u.aucIpv4Octet);
        }
        else if(IPV6_ADDR == tmpValue.choice) /*ipv6*/
        {
            inet_pton(AF_INET6, (char*)(token), tmpValue.u.aucIpv6Octet);
        }
        else if(HOSTNAME_ADDR == tmpValue.choice)
        {
            strcpy((char*)(tmpValue.u.HostName), (char*)(token));
        }

        osa_mem_copy(&(pValue[cnt]), &tmpValue, sizeof(CM_HOST_NAME_OR_IP_ADDR_T));
        cnt++;

        token = (UINT8*)strtok_r(NULL, ",", &pucTmp);
    }

    *pulStructCnt = cnt;

    return DM_TRUE;
}

BOOL8 DmConvertCmHostNameOrIpArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT32 j = 0;
    INT8 *pucStr = pucStringValue;
    CM_HOST_NAME_OR_IP_ADDR_T *pValue = pStructValue;

    *plStringCnt = 0;

    for(j = 0; j < lStructCnt; j++)
    {
        INT8 aucIPAddr[160] = {0}; 

        /* ipv4 */
        if(IPV4_ADDR == pValue[j].choice)
        {
            inet_ntop(AF_INET, pValue[j].u.aucIpv4Octet, (char*)aucIPAddr, 160); 
            strcat(pucStr, aucIPAddr);
        }
        /* ipv6 */
        else if(IPV6_ADDR == pValue[j].choice)
        {
            inet_ntop(AF_INET6, pValue[j].u.aucIpv6Octet, (char*)aucIPAddr, 160); 
            strcat(pucStr, aucIPAddr); 
        }
        /* url */
        else if(HOSTNAME_ADDR == pValue[j].choice)
        {
            strcat(pucStr, pValue[j].u.HostName);
        }

        if ((j+1)<lStructCnt)
        {
            strcat(pucStr, ",");
        }
    }

    if (lStructCnt>0)
    {
        *(pucStr + strlen(pucStr)) = '\0';
    }

    *plStringCnt = strlen(pucStr);

    return DM_TRUE;
}

BOOL8 DmConvertStringToCmIpAddrArray(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange)
{
    INT8 *pucTmp = NULL;
    UINT8 *pucStr = pucStringValue;
    CM_IP_ADDR_T *pValue = (CM_IP_ADDR_T *)pStructValue;
    CM_IP_ADDR_T tmpValue = {0};

    /* call the convert func */
    UINT8 *token = (UINT8*)strtok_r((char*)(pucStr), ",", &pucTmp);
    UINT32 cnt = 0;

    while(token != NULL)
    {
        osa_mem_set(&tmpValue,0,sizeof(CM_IP_ADDR_T));

        /* IPV4 */
        if(strstr((char*)(token),".") != NULL)
        {
            tmpValue.choice = IPV4_ADDR;
            inet_pton(AF_INET, (char*)(token), tmpValue.u.aucIpv4Octet);
        }
        /* IPV6 */
        else if(strstr((char*)(token),":") != NULL)
        {
           tmpValue.choice = IPV6_ADDR;
           inet_pton(AF_INET6, (char*)(token), tmpValue.u.aucIpv6Octet);
        }
        else
        {
            tmpValue.choice = INVALID_ADDR;
        }

        osa_mem_copy(&(pValue[cnt]), &tmpValue, sizeof(CM_IP_ADDR_T));
        cnt++;

        token = (UINT8*)strtok_r(NULL, ",", &pucTmp);
    }

    *pulStructCnt = cnt;

    return DM_TRUE;
}

BOOL8 DmConvertCmIpAddrArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange)
{
    INT32 j = 0;
    INT8 *pucStr = pucStringValue;
    CM_IP_ADDR_T *pValue = pStructValue;

    *plStringCnt = 0;

    for(j = 0; j < lStructCnt; j++)
    {
        
        if(pValue[j].choice == IPV4_ADDR)
        {
            inet_ntop(AF_INET, pValue[j].u.aucIpv4Octet, pucStr, INET_ADDRSTRLEN);
        }
        else if(pValue[j].choice == IPV6_ADDR)
        {
            inet_ntop(AF_INET6, pValue[j].u.aucIpv6Octet, pucStr, INET6_ADDRSTRLEN);
        }

        if ((j+1)<lStructCnt)
        {
            strcat(pucStr, ",");
        }
    }

    if (lStructCnt>0)
    {
        *(pucStr + strlen(pucStr)) = '\0';
    }

    *plStringCnt = strlen(pucStr);


    return DM_TRUE;
}

BOOL8 DmDateTimeStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_TRUE;
	INT8 *pChar = pValue;
    INT32 length = 0;
    INT32 length_type1 = strlen("YYYY-MM-DDTHH:mm:SS.FFFZ");
    INT32 length_type2 = strlen("YYYY-MM-DDTHH:mm:SSZ");

    /* datetime should have UTC format according to tr106,  YYYY-MM-DDTHH:MM:SS[.FFF]Z */

	if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmDateTimeStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

    length = strlen(pValue);
	if (length != length_type1
        && length != length_type2)
	{
        DM_WARNING("Invalid length in DmDateTimeStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
	}

    /* here we do the char comparation one by one since the format is fix */
    if (!isdigit(pChar[0]) || !isdigit(pChar[1]) || !isdigit(pChar[2]) || !isdigit(pChar[3]))/* year */
    {
        DM_WARNING("Invalid year value in DmDateTimeStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
    }

    if (pChar[4]!='-' || pChar[7]!='-' || pChar[10]!='T' || pChar[13]!=':' || pChar[16]!=':' || pChar[length-1]!='Z')
    {
        DM_WARNING("Invalid symbol value in DmDateTimeStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
    }

    if (!((pChar[5]=='0'&&pChar[6]>='0'&&pChar[6]<='9') || (pChar[5]=='1'&&pChar[6]>='0'&&pChar[6]<='2')))/* month */
    {
        DM_WARNING("Invalid month value in DmDateTimeStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
    }
        
    if (!((pChar[8]>='0'&&pChar[8]<='2'&&pChar[9]>='0'&&pChar[9]<='9') || (pChar[8]=='3'&&pChar[9]>='0'&&pChar[9]<='1')))/* day */
    {
        DM_WARNING("Invalid day value in format in DmDateTimeStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
    }
    else
    {
        /* check the day */
        INT32 year = strtol(&(pChar[0]),NULL,10);
        INT32 mouth = strtol(&(pChar[5]),NULL,10);
        INT32 mday = strtol(&(pChar[8]),NULL,10);

        if (!mouth || !mday)
        {
            /* 0 is invalid */
            bRet = DM_FALSE;
        }

        if ((mouth==4 || mouth==6 || mouth==9 || mouth==11) && mday>30)
        {
            bRet = DM_FALSE;
        }

        if ((mouth==1 || mouth==3 || mouth==5 || mouth==7 || mouth==8 || mouth==10 || mouth==12) && mday>31)
        {
            bRet = DM_FALSE;
        }

        if ((!(year%4)&&(year%100)) || (!(year%100)&&!(year%400)))
        {
            /* leap year */
            if (mouth==2 && mday>29)
            {
                bRet = DM_FALSE;
            }
        }
        else
        {
            if (mouth==2 && mday>28)
            {
                bRet = DM_FALSE;
            }
        }

        if (bRet == DM_FALSE)
        {
            DM_WARNING("Invalid day value in DmDateTimeStringValidCheck");
    		bRet = DM_FALSE;
    		return bRet;
        }
    }

    if (!((pChar[11]>='0'&&pChar[11]<='1'&&pChar[12]>='0'&&pChar[12]<='9') || (pChar[11]=='2'&&pChar[12]>='0'&&pChar[12]<='3')))/* hour */
    {
        DM_WARNING("Invalid hour value in DmDateTimeStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
    }
        
    if (!(pChar[14]>='0'&&pChar[14]<='5'&&pChar[15]>='0'&&pChar[15]<='9'))/* minute */
    {
        DM_WARNING("Invalid minute value in DmDateTimeStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
    }

    if (!(pChar[17]>='0'&&pChar[17]<='5'&&pChar[18]>='0'&&pChar[18]<='9'))/* second */
    {
        DM_WARNING("Invalid second value in DmDateTimeStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
    }

    if (length==length_type1 && (pChar[19]!='.' || !isdigit(pChar[20]) || !isdigit(pChar[21]) || !isdigit(pChar[22])))/* micro-second */
    {
        DM_WARNING("Invalid micro-second value in DmDateTimeStringValidCheck");
		bRet = DM_FALSE;
		return bRet;
    }

    return bRet;
}

BOOL8 DmCmIpAddrStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
    UINT8 aucIpOctet[sizeof(struct in6_addr)];

    if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmCmIpAddrStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

    if (strlen(pValue)<=0)
    {
        bRet = DM_TRUE;
		return bRet;
    }

    if (strchr(pValue,'.')!=NULL && inet_pton(AF_INET, pValue, aucIpOctet)==1)
    {
        bRet = DM_TRUE;
	}
    else if (strchr(pValue,':')!=NULL && inet_pton(AF_INET6, pValue, aucIpOctet)==1)
    {
        bRet = DM_TRUE;
    }
    else
    {
        DM_WARNING("Invalid ip string in DmCmIpAddrStringValidCheck");
    }

    return bRet;
}

BOOL8 DmCmCmHostNameOrIpStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
    BOOL8 bRet = DM_FALSE;
    UINT8 aucIpOctet[sizeof(struct in6_addr)];
    INT8 *pucStr = NULL;
    BOOL8  bDotFlag = 0;
    BOOL8  bDigitFlag = 0;
    UINT32 i = 0;
    UINT8 choice = INVALID_ADDR;
    INT32 length = 0;

    if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmCmCmHostNameOrIpStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

    length = strlen(pValue);
    if (length>=65)
    {
        DM_WARNING("Invalid length (>=65) in DmCmCmHostNameOrIpStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
    }

    if (strlen(pValue)<=0)
    {
        bRet = DM_TRUE;
		return bRet;
    }

    /*judge host, ipv4, ipv6*/
    pucStr = pValue;
    choice = INVALID_ADDR; /* ipv4 */
    for(i=0; i< length; i++)
    {
        if(pucStr[i] == ':')
        {
            choice = IPV6_ADDR; /* ipv6 */
            break;
        }
        else if(isalpha(pucStr[i]))
        {
            choice = HOSTNAME_ADDR; /* host name */
            break;
        }
        else if(pucStr[i] == '.')
        {
            bDotFlag = 1;
        }
        else if(isdigit(pucStr[i]))
        {
            bDigitFlag = 1;
        }
    }

    if((INVALID_ADDR == choice)&& bDotFlag && bDigitFlag)
    {
        choice = IPV4_ADDR;
    }

    /* call the convert func */
    if(IPV4_ADDR == choice && inet_pton(AF_INET, pucStr, aucIpOctet)==1)/*ipv4*/
    {
        bRet = DM_TRUE;
    }
    else if(IPV6_ADDR == choice && inet_pton(AF_INET6, pucStr, aucIpOctet)==1)/*ipv6*/
    {
        bRet = DM_TRUE;
    }
    else if(HOSTNAME_ADDR == choice)
    {
        if (strstr(pValue,"..")!=NULL)
        {
            DM_WARNING("'..' found in hostname in DmCmCmHostNameOrIpStringValidCheck");
            bRet = DM_FALSE;
            return bRet;
        }

        if (!isdigit(pValue[0]) && !isalpha(pValue[0]))
        {
            DM_WARNING("invalid prefix of hostname in DmCmCmHostNameOrIpStringValidCheck");
            bRet = DM_FALSE;
            return bRet;
        }

        if (!isdigit(pValue[length-1]) && !isalpha(pValue[length-1]))
        {
            DM_WARNING("invalid suffix of hostname in DmCmCmHostNameOrIpStringValidCheck");
            bRet = DM_FALSE;
            return bRet;
        }

        bRet = DM_TRUE;
        for(i=0; i< length; i++)
        {
            if((pucStr[i]>='0' && pucStr[i]<='9')
                || (pucStr[i]>='a' && pucStr[i]<='z')
                || (pucStr[i]>='A' && pucStr[i]<='Z')
                || pucStr[i]=='-'
                || pucStr[i]=='.')
            {
                continue;
            }
            else
            {
                DM_WARNING("Invalid hostname in DmCmCmHostNameOrIpStringValidCheck");
                bRet = DM_FALSE;
                break;
            }
        }
    }
    else if (IPV4_ADDR == choice || IPV6_ADDR == choice)
    {
        DM_WARNING("Invalid ip addr in DmCmCmHostNameOrIpStringValidCheck");
        bRet = DM_FALSE;
    }

    return bRet;
}

BOOL8 DmCmPlmnIdStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
    INT8 *pChar = pValue;
    INT32 length = 0;
    INT32 cnt = 0;

    if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmCmPlmnIdStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

    /* all digital numbers, its length is 0 or 5 or 6 */
    length = strlen(pValue);
    if (length!=0 && length!=5 && length!=6)
    {
        DM_WARNING("Invalid length in DmCmPlmnIdStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
    }

    bRet = DM_TRUE;
    for (cnt=0; cnt<length; cnt++)
    {
        if (!isdigit(pChar[cnt]))
        {
            DM_WARNING("Invalid chars found in DmCmPlmnIdStringValidCheck");
            bRet = DM_FALSE;
            break;
        }
    }

    return bRet;
}

BOOL8 DmCmImsiStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
    INT8 *pChar = pValue;
    INT32 length = 0;
    INT32 cnt = 0;

    if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmCmImsiStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

    /* all digital numbers, its length is 0 or 15 */
    length = strlen(pValue);
    if (length!=0 && length!=15)
    {
        DM_WARNING("Invalid length in DmCmImsiStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
    }

    bRet = DM_TRUE;
    for (cnt=0; cnt<length; cnt++)
    {
        if (!isdigit(pChar[cnt]))
        {
            DM_WARNING("Invalid chars found in DmCmImsiStringValidCheck");
            bRet = DM_FALSE;
            break;
        }
    }

    return bRet;
}

BOOL8 DmCmPlmnIdArrayStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
    INT8 *pValueTmp = NULL;
    INT8 *pToken = NULL;
    INT8 *pTmp = NULL;

    if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmCmPlmnIdArrayStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

    if (strlen(pValue)<=0)
    {
        bRet = DM_TRUE;
		return bRet;
    }

    if (strchr(pValue,',')==NULL)
    {
        if (DmCmPlmnIdStringValidCheck(pValueRange,pValue)==DM_FALSE)
        {
            DM_WARNING("Invalid PLMNID in DmCmPlmnIdArrayStringValidCheck");
            return bRet;
        }
    }
    else
    {
        pValueTmp = strdup(pValue);

        if (pValueTmp==NULL)
        {
            return bRet;
        }

        pToken = strtok_r(pValueTmp,",",&pTmp);
        while(pToken)
        {
            if (DmCmPlmnIdStringValidCheck(pValueRange,pToken)==DM_FALSE)
            {
                DM_WARNING("Invalid PLMNID list in DmCmPlmnIdArrayStringValidCheck");
                DM_FREE(pValueTmp);
                return bRet;
            }
            pToken = strtok_r(NULL,",",&pTmp);
        }
        DM_FREE(pValueTmp);
    }

    bRet = DM_TRUE;
    return bRet;
}

BOOL8 DmCmImsiArrayStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
    INT8 *pValueTmp = NULL;
    INT8 *pToken = NULL;
    INT8 *pTmp = NULL;

    if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmCmImsiArrayStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

    if (strlen(pValue)<=0)
    {
        bRet = DM_TRUE;
		return bRet;
    }

    if (strchr(pValue,',')==NULL)
    {
        if (DmCmImsiStringValidCheck(pValueRange,pValue)==DM_FALSE)
        {
            DM_WARNING("Invalid IMSI in DmCmImsiArrayStringValidCheck");
            return bRet;
        }
    }
    else
    {
        pValueTmp = strdup(pValue);

        if (pValueTmp==NULL)
        {
            return bRet;
        }

        pToken = strtok_r(pValueTmp,",",&pTmp);
        while(pToken)
        {
            if (DmCmImsiStringValidCheck(pValueRange,pToken)==DM_FALSE)
            {
                DM_WARNING("Invalid IMSI list in DmCmImsiArrayStringValidCheck");
                DM_FREE(pValueTmp);
                return bRet;
            }
            pToken = strtok_r(NULL,",",&pTmp);
        }
        DM_FREE(pValueTmp);
    }

    bRet = DM_TRUE;
    return bRet;
}

BOOL8 DmCmCmHostNameOrIpArrayStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
    INT8 *pValueTmp = NULL;
    INT8 *pToken = NULL;
    INT8 *pTmp = NULL;

    if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmCmCmHostNameOrIpArrayStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

    if (strlen(pValue)<=0)
    {
        bRet = DM_TRUE;
		return bRet;
    }

    if (strchr(pValue,',')==NULL)
    {
        if (DmCmCmHostNameOrIpStringValidCheck(pValueRange,pValue)==DM_FALSE)
        {
            DM_WARNING("Invalid hostname/ipaddr in DmCmCmHostNameOrIpArrayStringValidCheck");
            return bRet;
        }
    }
    else
    {
        pValueTmp = strdup(pValue);

        if (pValueTmp==NULL)
        {
            return bRet;
        }

        pToken = strtok_r(pValueTmp,",",&pTmp);
        while(pToken)
        {
            if (DmCmCmHostNameOrIpStringValidCheck(pValueRange,pToken)==DM_FALSE)
            {
                DM_WARNING("Invalid hostname/ipaddr list in DmCmCmHostNameOrIpArrayStringValidCheck");
                DM_FREE(pValueTmp);
                return bRet;
            }
            pToken = strtok_r(NULL,",",&pTmp);
        }
        DM_FREE(pValueTmp);
    }

    bRet = DM_TRUE;
    return bRet;
}

BOOL8 DmCmIpAddrArrayStringValidCheck(INT8 *pValueRange, INT8 *pValue)
{
	BOOL8 bRet = DM_FALSE;
    INT8 *pValueTmp = NULL;
    INT8 *pToken = NULL;
    INT8 *pTmp = NULL;

    if (pValueRange == NULL || pValue == NULL)
	{
        DM_WARNING("Invalid parameters(null) in DmCmIpAddrArrayStringValidCheck");
        bRet = DM_FALSE;
		return bRet;
	}

    if (strlen(pValue)<=0)
    {
        bRet = DM_TRUE;
		return bRet;
    }

    if (strchr(pValue,',')==NULL)
    {
        if (DmCmIpAddrStringValidCheck(pValueRange,pValue)==DM_FALSE)
        {
            DM_WARNING("Invalid ip addr in DmCmIpAddrArrayStringValidCheck");
            return bRet;
        }
    }
    else
    {
        pValueTmp = strdup(pValue);

        if (pValueTmp==NULL)
        {
            return bRet;
        }

        pToken = strtok_r(pValueTmp,",",&pTmp);
        while(pToken)
        {
            if (DmCmIpAddrStringValidCheck(pValueRange,pToken)==DM_FALSE)
            {
                DM_WARNING("Invalid ip addr list in DmCmIpAddrArrayStringValidCheck");
                DM_FREE(pValueTmp);
                return bRet;
            }
            pToken = strtok_r(NULL,",",&pTmp);
        }
        DM_FREE(pValueTmp);
    }

    bRet = DM_TRUE;
    return bRet;
}

