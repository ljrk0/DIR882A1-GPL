
/*
*/

#ifndef __FOTA_COMMON_H__
#define __FOTA_COMMON_H__

#define FOTA_AES_KEY "b7931c8343c8a1c23f6ef1d4240660f7"

#define cfg_get(pName) \
	nvram_safe_get(pName)

#define cfg_get_int(pName) \
	nvram_get_int(pName)

#define cfg_set(pName, pValue) \
	nvram_safe_set(pName, pValue)

#define cfg_save() \
	nvram_commit(RT2860_NVRAM)

/*
  * Must not be changed when restoring to the factory.
*/
#define cfg_get_permanent(pName) \
	nvram_get(CONFIG2_NVRAM, pName)

#define cfg_set_permanent(pName, pValue) \
	nvram_bufset(CONFIG2_NVRAM, pName, pValue)

#define cfg_save_permanent() \
	nvram_commit(CONFIG2_NVRAM)

#define FREE_POINTER(p) \
	if (p) \
	{ \
		free(p); \
		p = NULL; \
	}

int fota_output_to_file(const char * pFile_name,
						const void * pAddr,
						size_t iLen);

int fota_get_file_content(const char * pFile_name,
							void * * pBuf_addr,
							size_t * iLen);

int aes_encrypt(
					char * pKey_hex,
					const char * pInput_data,
					char * * pOutput_data_hex);

int aes_decrypt(
					char * pKey_hex,
					const char * pInput_data_hex,
					char * * pOutput_data);

int execute_action(const char * pFile_name, char * const argv[],
					int * pExit_code);

int execute_pause(int iSeconds);

int check_network_status(const char * pDomain, int iFamily);
int wait_network_accessible(const char * pDomain, int iFamily);

int fota_init_lock(void);
int fota_lock(void);
int fota_unlock(void);

#endif

