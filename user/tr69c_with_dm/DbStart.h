#ifndef _DB_START_H_
#define _DB_START_H_

#define SCM_SUCCESS 0
#define SCM_FAILURE -1
#define SCM_TRACE_H(fmt, args...) printf("%s:%d"fmt,  __FUNCTION__, __LINE__, ##args)
//#define SCM_ERROR(level, fmt, args...) do{ if(level)printf("%s:%d"fmt,  __FUNCTION__, __LINE__, ##args);}while(0)

/** the sock message header definition */
typedef struct _SOCK_MSG_HEADER_T{
    UINT32 msg_id;/**< the id of identify the message */
    UINT32 sender_id;/** the id to identify client or server */
    UINT32 receiver_id;/** the id to identify client or server */
    INT32  word_data;/** different msg has different usage.  */
    UINT32 msgLength;/** the bytes of the message body */
}SOCK_MSG_HEADER_T;


#endif
