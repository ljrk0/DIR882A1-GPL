/*
 * container_list_sl.h
 * $Id: //WIFI_SOC/MP/SDK_5_0_0_0/RT288x_SDK/source/user/net-snmp-5.5.pre2/include/net-snmp/library/container_list_ssll.h#1 $
 *
 */
#ifndef NETSNMP_CONTAINER_SSLL_H
#define NETSNMP_CONTAINER_SSLL_H


#include <net-snmp/library/container.h>

#ifdef  __cplusplus
extern "C" {
#endif

    netsnmp_container *netsnmp_container_get_sorted_singly_linked_list(void);
    netsnmp_container *netsnmp_container_get_singly_linked_list(int fifo);

    void netsnmp_container_ssll_init(void);


#ifdef  __cplusplus
}
#endif

#endif /** NETSNMP_CONTAINER_SSLL_H */
