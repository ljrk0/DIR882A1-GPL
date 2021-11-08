#ifndef _H_LOGMONITOR
#define _H_LOGMONITOR

#define LOGGER_FILE_KEY_LEN        8

/* ��־�ļ����ʱ���ʽ */
typedef struct
{
    unsigned long year : 6;
    unsigned long mon  : 4;
    unsigned long date : 5;
    unsigned long hour : 5;
    unsigned long minu : 6;
    unsigned long sec  : 6;
} ST_LOGGER_TIME;

/* MTD ������־�ļ���Ÿ�ʽ */
typedef struct
{
    unsigned long ulMagicNum;                   /* ħ�֣����ڱ�ʾѹ�������־�ļ� */
    unsigned long ulVer;                        /* �汾���ݲ�ʹ�� */
    ST_LOGGER_TIME   stTime;                       /* ��־�ļ�����ʱ�䣬�ݲ�ʹ�� */
    char          acKey[LOGGER_FILE_KEY_LEN];      /* �ڲ��ؼ��֣��ݲ�ʹ�� */
    unsigned long ulUncompLen;                  /* δѹ�������ݳ��� */
    unsigned long ulCompLen;                    /* ѹ��������ݳ��� */
    char          acData[0];                    /* ѹ��������� */
} ST_MTD_FILE;

#define LOGGER_MAX_MTD_LEN         0x1000              /* �����־�ļ��� MTD �豸������С, 4 KBytes */
/* ѹ������־���ݿ������ռ�, ���� MTD �ռ��С - �ṹռ�ÿռ� - CRC32 ���� */
#define LOGGER_COMP_MAX_LEN \
        (LOGGER_MAX_MTD_LEN - (sizeof(ST_MTD_FILE) + sizeof(unsigned long)))

/* δѹ������־�ļ���󳤶ȣ����մ˳��ȽضϺ���ļ�Ӧ�ܴﵽ 1000 ����־������ */
#define LOGGER_MAX_UNCOMP_FILE_LEN (2 * LOGGER_COMP_MAX_LEN)     /* 3 ���ڿ���ѹ���ռ� */

#define LOG_EMAIL_FILE "/var/log/log_email.txt"

#define TRACE_LOG_SIZE   0x100000    /* 1024 * 1024 = 1M */
#define TRACE_HTTP_LOG_FILE "/tmp/httpd_log.txt"
//#define TRACE_RC_LOG_FILE "/tmp/rc_log.txt"

#endif
