/*******************************************************************************
  Copyright (C) 2013, Broadcom.
  File name:        osa_getaddrinfo.h
  Description:      header file for event operations
  History:
    1. zhonghai    2015-08-18         creation
        
*******************************************************************************/


#ifndef OSA_GETADDRINFO_H
#define OSA_GETADDRINFO_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

/**
* async getaddrinfo().
*
* @param[out] node
* @param[out] service
* @param[out] hints
* @param[out] res
* @param[out] milliseconds
* @return the result.(OSA_SUCCESS or OSA_FAILURE)
*
*/
INT32 osa_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res, int milliseconds);


#ifdef __cplusplus
#if __cplusplus
            }
#endif 
#endif /* __cpluscplus */
    

#endif /* OSA_GETADDRINFO_H */



