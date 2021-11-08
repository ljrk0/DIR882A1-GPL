#ifndef _NVRAM_H
#define _NVRAM_H 	1

#include <linux/autoconf.h>

#ifdef CONFIG_DUAL_IMAGE

#define UBOOT_NVRAM	0
#define RT2860_NVRAM    1
#define RTDEV_NVRAM    	2
#define WIFI3_NVRAM    	3
#define CERT_NVRAM    	4
#else
#define RT2860_NVRAM    0
#define RTDEV_NVRAM    	1
#define WIFI3_NVRAM    	2
#define CERT_NVRAM    	3
#endif

#define FB_2860_BLOCK_NAME "2860"
#define FB_CONFIG2_BLOCK_NAME "config2"

#if defined CONFIG_EXTEND_NVRAM
#define RALINK_NVRAM2_MTDNAME "Config2"
#define EXTEND_BLOCK_NUM 2
#define CONFIG2_NVRAM 		CERT_NVRAM+1
#if defined CONFIG_CONFIG_SHRINK
#define VOIP_NVRAM			RT2860_NVRAM
#else
#define VOIP_NVRAM			CONFIG2_NVRAM
#endif
#if defined CONFIG_WAPI_SUPPORT
#define WAPI_NVRAM    	CONFIG2_NVRAM+1
#else
#define TR069CERT_NVRAM    	CONFIG2_NVRAM+1
#endif
#else
#define VOIP_NVRAM			RT2860__NVRAM
#define EXTEND_BLOCK_NUM 0
#define CONFIG2_NVRAM           RT2860_NVRAM
#endif


#define NV_DEV "/dev/apsoc_nvram"
#define RALINK_NVRAM_IOCTL		0x1000
#define RALINK_NVRAM_IOCTL_GET		RALINK_NVRAM_IOCTL + 0x01
#define RALINK_NVRAM_IOCTL_GETALL	RALINK_NVRAM_IOCTL + 0x02
#define RALINK_NVRAM_IOCTL_SET		RALINK_NVRAM_IOCTL + 0x03
#define RALINK_NVRAM_IOCTL_COMMIT	RALINK_NVRAM_IOCTL + 0x04
#define RALINK_NVRAM_IOCTL_CLEAR	RALINK_NVRAM_IOCTL + 0x05

typedef struct environment_s {
	unsigned long crc;		//CRC32 over data bytes
	char *data;
} env_t;

typedef struct cache_environment_s {
	char *name;
	char *value;
} cache_t;

#define MAX_CACHE_ENTRY 2000
typedef struct block_s {
	char *name;
	env_t env;			//env block
	cache_t	cache[MAX_CACHE_ENTRY];	//env cache entry by entry
	unsigned long flash_offset;
	unsigned long flash_max_len;	//ENV_BLK_SIZE

	char valid;
	char dirty;
} block_t;

#define MAX_NAME_LEN 128
#define MAX_VALUE_LEN (ENV_BLK_SIZE * 5)
typedef struct nvram_ioctl_s {
	int index;
	int ret;
	char *name;
	char *value;
} nvram_ioctl_t;


#ifdef CONFIG_DUAL_IMAGE
#define FLASH_BLOCK_NUM	5
#else
#define FLASH_BLOCK_NUM	4
#endif


void nvram_init(int index);
void nvram_close(int index);

int nvram_set(int index, char *name, char *value);
const char *nvram_get(int index, char *name);
int nvram_bufset(int index, char *name, char *value);
char const *nvram_bufget(int index, char *name);

void nvram_buflist(int index);
int nvram_commit(int index);
int nvram_clear(int index);
int nvram_erase(int index);

int getNvramNum(void);
unsigned int getNvramOffset(int index);
unsigned int getNvramBlockSize(int index);
char *getNvramName(int index);
unsigned int getNvramIndex(char *name);
void toggleNvramDebug(void);

//new added
const char *nvram_safe_get(char *name);
int nvram_safe_set(char *name, char *value);
int nvram_unset(char *name);
int nvram_match(char *name, char *match);
int nvram_invmatch(const char *name, const char *invmatch);
int nvram_get_int(char *name);
#endif
