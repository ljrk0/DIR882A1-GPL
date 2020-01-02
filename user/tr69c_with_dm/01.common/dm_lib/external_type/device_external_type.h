

#ifndef _EXTERNAL_TYPE_DEF_H_
#define _EXTERNAL_TYPE_DEF_H_



#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    INT8        TimeZoneHour;
    INT8        TimeZoneMinute;
}TIME_ZONE_T;

typedef struct
{
    UINT16      Year;
    UINT8       Month;
    UINT16      Day;
    UINT8       Hour;
    UINT8       Minute;
    UINT8       Second;
    UINT32      MicroSecond;
    TIME_ZONE_T TimeZone;
}DATE_TIME_T;

typedef struct
{
#define INVALID_ADDR  0	
#define IPV4_ADDR     1
#define IPV6_ADDR     2
#define HOSTNAME_ADDR 3
    UINT32      choice;
    union
    {
        UINT8  aucIpv4Octet[4];
        UINT8  aucIpv6Octet[16];
        INT8   HostName[65];
    }u;
}CM_HOST_NAME_OR_IP_ADDR_T;

typedef struct
{
#define INVALID_ADDR  0
#define IPV4_ADDR     1
#define IPV6_ADDR     2
    UINT32      choice;
    union
    {
        UINT8   aucIpv4Octet[4];
        UINT8   aucIpv6Octet[16];
    }u;
}CM_IP_ADDR_T;

typedef struct
{
    UINT8       ucMncCnt;           /**< 2..3, MNC count */
    UINT8       aucMncDigit[3];     /**< 0..9 */
    UINT8       aucMccDigit[3];     /**< 0..9 */

}CM_PLMN_ID_T;

typedef struct
{
#define CM_IMSI_MAX_SIZE   8
    UINT8   ucLength;				               /**< 3..8 */
    UINT8   aucImsi[CM_IMSI_MAX_SIZE];             /**< International Mobile Subscriber Identity of the UE */
}CM_IMSI_T;

INT8* DmConvertIntToStr(INT32 lNum);
BOOL8 DmConvertStringToCmHostNameOrIp(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
BOOL8 DmConvertCmHostNameOrIpToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
BOOL8 DmConvertImsiToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
BOOL8 DmConvertStringToCmImsi(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
BOOL8 DmConvertImsiArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
BOOL8 DmConvertStringToCmImsiArray(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
BOOL8 DmConvertStringToCmIpAddr(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
BOOL8 DmConvertCmIpAddrToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
BOOL8 DmConvertCmPlmnIdToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
BOOL8 DmConvertStringToCmPlmnId(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
BOOL8 DmConvertCmPlmnIdArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
BOOL8 DmConvertStringToCmPlmnIdArray(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
BOOL8 DmConvertDateTimeToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
BOOL8 DmConvertStringToDateTime(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
BOOL8 DmConvertStringToCmHostNameOrIpArray(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
BOOL8 DmConvertCmHostNameOrIpArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);
BOOL8 DmConvertStringToCmIpAddrArray(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);
BOOL8 DmConvertCmIpAddrArrayToString(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);

BOOL8 DmDateTimeStringValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmCmIpAddrStringValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmCmCmHostNameOrIpStringValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmCmPlmnIdStringValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmCmImsiStringValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmCmPlmnIdArrayStringValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmCmImsiArrayStringValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmCmCmHostNameOrIpArrayStringValidCheck(INT8 *pValueRange, INT8 *pValue);
BOOL8 DmCmIpAddrArrayStringValidCheck(INT8 *pValueRange, INT8 *pValue);

#ifdef __cplusplus
}
#endif

#endif /* _EXTERNAL_TYPE_DEF_H_ */
