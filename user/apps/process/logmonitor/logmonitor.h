#ifndef _H_LOGMONITOR
#define _H_LOGMONITOR

#define LOGGER_FILE_KEY_LEN        8

/* 日志文件存放时间格式 */
typedef struct
{
    unsigned long year : 6;
    unsigned long mon  : 4;
    unsigned long date : 5;
    unsigned long hour : 5;
    unsigned long minu : 6;
    unsigned long sec  : 6;
} ST_LOGGER_TIME;

/* MTD 分区日志文件存放格式 */
typedef struct
{
    unsigned long ulMagicNum;                   /* 魔字，用于表示压缩后的日志文件 */
    unsigned long ulVer;                        /* 版本，暂不使用 */
    ST_LOGGER_TIME   stTime;                       /* 日志文件保存时间，暂不使用 */
    char          acKey[LOGGER_FILE_KEY_LEN];      /* 内部关键字，暂不使用 */
    unsigned long ulUncompLen;                  /* 未压缩的数据长度 */
    unsigned long ulCompLen;                    /* 压缩后的数据长度 */
    char          acData[0];                    /* 压缩后的数据 */
} ST_MTD_FILE;

#define LOGGER_MAX_MTD_LEN         0x1000              /* 存放日志文件的 MTD 设备分区大小, 4 KBytes */
/* 压缩的日志数据可用最大空间, 可用 MTD 空间大小 - 结构占用空间 - CRC32 长度 */
#define LOGGER_COMP_MAX_LEN \
        (LOGGER_MAX_MTD_LEN - (sizeof(ST_MTD_FILE) + sizeof(unsigned long)))

/* 未压缩的日志文件最大长度，按照此长度截断后的文件应能达到 1000 行日志的需求 */
#define LOGGER_MAX_UNCOMP_FILE_LEN (2 * LOGGER_COMP_MAX_LEN)     /* 3 倍于可用压缩空间 */

#define LOG_EMAIL_FILE "/var/log/log_email.txt"

#define TRACE_LOG_SIZE   0x100000    /* 1024 * 1024 = 1M */
#define TRACE_HTTP_LOG_FILE "/tmp/httpd_log.txt"
//#define TRACE_RC_LOG_FILE "/tmp/rc_log.txt"

#endif
