/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa.h
  Description:      header file for osa libary
  History:
    1. zhonghai    2013-08-21         creation
        
*******************************************************************************/


#ifndef OSA_H
#define OSA_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <memory.h>

#include <pthread.h>
#include <sys/prctl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <cpio.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/resource.h>
#include <limits.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <netdb.h>

#include "common_type_def.h"

#include "osa_debug.h"
#include "osa_event.h"
#include "osa_resource.h"
#include "osa_mem.h"
#include "osa_mutex.h"
#include "osa_msg_queue.h"
#include "osa_sem.h"
#include "osa_shm.h"
#include "osa_thread.h"
#include "osa_thread_pool.h"
#include "osa_timer.h"
#include "osa_getaddrinfo.h"


/** the result definition of osa operations */
#define OSA_SUCCESS 0
#define OSA_FAILURE -1
#define OSA_QUEUE_EMPTY 1

/** the boolean definition of osa */
#define OSA_FALSE 0
#define OSA_TRUE  1

#define OSA_WAIT     0
#define OSA_NO_WAIT  1

#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif/* __cpluscplus */
    

#endif

