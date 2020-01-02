
/*
*/

#ifndef __FOTA_PLATFORM_DEPENDENCE_H__
#define __FOTA_PLATFORM_DEPENDENCE_H__

int md5_check(const char * pFw_name, const char * pChecksum);
int verify_firmware(const char * pFw_name,
					struct fota_fw_info * pFw);
int clear_firmware(const char * pFw_name);
int change_led_color(void);
int save_firmware_to_flash(const char * pFw_name);
int reboot(void);
int detect_process_num(const char * pProcess_name);

#endif

