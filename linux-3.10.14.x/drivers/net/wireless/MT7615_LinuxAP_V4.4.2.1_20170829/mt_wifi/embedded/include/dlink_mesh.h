/* MODULE NAME: meshd_common.h
 *
 * PURPOSE:
 *    {What is covered in this file ¨C function and scope.}
 *
 * NOTES:
 *    {Something must be known or noticed}
 *    How to use these functions - Give an example.}
 *    Any design limitation, Any performance limitation}
 *
 * CREATOR: Sam Chen
 *
 * Date: 2017.06.23
 *
 * Copyright(C) D-Link
 */
#ifndef _DLINK_MESH_H
#define _DLINK_MESH_H
/* INCLUDE FILE DECLARATIONS */



/* NAMING CONSTANT DECLARATIONS */
#define MESH_IE_MAX 255
#define BW20		(1 << 0)
#define BW40		(1 << 1)
#define BW80		(1 << 2)
#define BW160		(1 << 3)
#define MESH_WMODE_B	(1 << 0)
#define MESH_WMODE_G	(1 << 1)
#define MESH_WMODE_N	(1 << 2)
#define MESH_WMODE_A	(1 << 16)
#define MESH_WMODE_AN	(1 << 17)
#define MESH_WMODE_AC	(1 << 18)



/* MACRO FUNCTION DECLARATIONS */



/* DATA STRUCTURE DECLARATIONS */
struct drv_cfg_s {
	uint8_t antenna;
	uint8_t scan_interval;
	uint8_t weak_client_interval;
	uint8_t weak_client_threshold;	//SNR
	uint8_t nbr_aged_time;
	uint8_t nbr_max_cnt;
	char mesh_ul_if[IFNAMSIZ];
	char mesh_dl_if[IFNAMSIZ];
	char mesh_ssid[32];
};

struct ht_mcs_tx_s {
#ifdef RT_BIG_ENDIAN
	uint8_t reserved:3;
	uint8_t tx_uniqual_mod_support:1;
	uint8_t tx_max_ss_supported:2;
	uint8_t tx_rx_mcs_set_no_equal:1;
	uint8_t tx_mcs_set_defined:1;
#else
	uint8_t tx_mcs_set_defined:1;
	uint8_t tx_rx_mcs_set_no_equal:1;
	uint8_t tx_max_ss_supported:2;
	uint8_t tx_uniqual_mod_support:1;
	uint8_t reserved:3;
#endif
};



/* EXPORTED FUNCTION PROTOTYPE */
/*
 * DON'T ADD FUNCTION DECLARATION WHICH INPUT CONTAINS MTK DEFINED DATA SRTUCT!!!
 * IT WILL MAKE YOU IN FXXK WORD HELL DUE TO INCLUDE THOSE DATA STRUCT HEADER FILES.
 */
int mesh_module_init(void);
int mesh_module_exit(void);


#endif	/* _DLINK_MESH_H */