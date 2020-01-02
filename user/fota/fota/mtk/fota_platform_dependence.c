
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <netinet/in.h>

#include "../fota.h"
#include "../fota_platform_dependence.h"

#ifdef CONFIG_USE_NAND_FLASH
#define SIZE_MAX_FILE (32*1024*1024)
#else
#define SIZE_MAX_FILE (16*1024*1024)
#endif

/* This define is from trunk/Uboot/include/image.h */
#define IH_MAGIC	0x27051956	/* Image Magic Number		*/
#define IH_NMLEN	(32-4)		/* Image Name Length		*/
#define PRODUCT_LEN  64
#define VERSION_LEN 16
typedef struct image_header {
	unsigned int	ih_magic;	/* Image Header Magic Number	*/
	unsigned int	ih_hcrc;	/* Image Header CRC Checksum	*/
	unsigned int	ih_time;	/* Image Creation Timestamp	*/
	unsigned int	ih_size;	/* Image Data Size		*/
	unsigned int	ih_load;	/* Data	 Load  Address		*/
	unsigned int	ih_ep;		/* Entry Point Address		*/
	unsigned int	ih_dcrc;	/* Image Data CRC Checksum	*/
	unsigned char		ih_os;		/* Operating System		*/
	unsigned char		ih_arch;	/* CPU architecture		*/
	unsigned char		ih_type;	/* Image Type			*/
	unsigned char		ih_comp;	/* Compression Type		*/
	unsigned char		ih_name[IH_NMLEN];	/* Image Name		*/
	unsigned int	ih_ksz;		/* Kernel Part Size		*/
	unsigned char     product[PRODUCT_LEN];
	unsigned char     sw_version[VERSION_LEN];
	unsigned char     hw_version[VERSION_LEN];
} image_header_t;

unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);

static int check_fw_header(const char *fw_path,
						const char * pModel_name)
{
    unsigned char *buf_fw = 0;
    unsigned int udSize = 0;
    image_header_t *kernel_header = 0;
    unsigned int kernel_size = 0;
    //unsigned long hcrc = 0;
    unsigned long hcrc_read = 0;
    //unsigned long dcrc = 0;
    int ret = 0; // default Fail
    //unsigned int hcrc_bkup = 0;

    FILE *file = fopen(fw_path, "rb");
    if (file == 0)
        goto end;

    buf_fw = (unsigned char*)malloc(SIZE_MAX_FILE);
    if (buf_fw == 0)
        goto end;
        
    fseek(file, 0, SEEK_END);
    udSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    fread(buf_fw, udSize, 1, file);

    kernel_header = (image_header_t*)(buf_fw);
	kernel_size = ntohl(kernel_header->ih_size);

    // check magic number
    if ( IH_MAGIC != ntohl(kernel_header->ih_magic) )
        goto end;

    // check header crc
    //hcrc_bkup = kernel_header->ih_hcrc;
    hcrc_read = ntohl(kernel_header->ih_hcrc); // backup hcrc
    kernel_header->ih_hcrc = 0;

	if(strcmp(pModel_name, (char *)kernel_header->product))
		goto end;
	
    if (crc32( 0, buf_fw, sizeof(image_header_t) ) != hcrc_read)
        goto end;
    //kernel_header->ih_hcrc = hcrc_bkup; // if you want to write buf_fw, this step should not be ignored

    // check data crc
    if( crc32 (0, buf_fw + sizeof(image_header_t), kernel_size) != ntohl(kernel_header->ih_dcrc) )
        goto end;

    ret = 1; // Pass

end:
    if (file != 0) {fclose(file);}
    if (buf_fw != 0) {free(buf_fw);}
    return ret;
}

int md5_check(const char * pFw_name, const char * pChecksum)
{
	char sCmd_buf[128] = { 0 };
	char sChecksum[64] = { 0 };
	int iItem_num = 0;
	int iResult_code = -1;
	FILE * fp = NULL;

	RETURN_IF_NULL(pFw_name, -1);
	RETURN_IF_NULL(pChecksum, -1);

	snprintf(sCmd_buf, sizeof (sCmd_buf), "md5sum %s", pFw_name);

	fp = popen(sCmd_buf, "r");
	if (!fp)
	{
		DEBUG("popen fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	iItem_num = fscanf(fp, " %32s ", sChecksum);
	if (iItem_num != 1)
	{
		DEBUG("Get md5 checksum of firmware fail.\n");
		if (iItem_num < 0)
		{
			DEBUG("fscanf fail, %s, errno=%d.\n", strerror(errno), errno);
		}
		goto done;
	}
	else
	{
		if (0 != strcasecmp(sChecksum, pChecksum))
		{
			DEBUG("Check md5 checksum of firmware fail.\n");
			goto done;
		}
	}

	DEBUG("Check md5 checksum of firmware ok.\n");

	iResult_code = 0;

done:

	if (pclose(fp) < 0)
	{
		DEBUG("pclose fail, %s, errno=%d.\n", strerror(errno), errno);
	}

	return iResult_code;
}

int verify_firmware(const char * pFw_name,
					struct fota_fw_info * pFw)
{
	char sCmd_buf[128] = { 0 };
	int iRet = 0;
	extern struct fota_config fota_config_cmd;

	RETURN_IF_NULL(pFw_name, -1);
	RETURN_IF_NULL(pFw, -1);
	RETURN_IF_NULL(pFw->pChecksum, -1);
	RETURN_IF_NULL(fota_config_cmd.pFw_path, -1);

	DEBUG("Now verify the firmware.\n");

	iRet = md5_check(pFw_name, pFw->pChecksum);
	if (iRet < 0)
	{
		DEBUG("Check md5 checksum of firmware fail.\n");
		return -1;
	}

	if (1 == fota_config_cmd.iFw_encrypt_flag)
	{
		snprintf(sCmd_buf, sizeof (sCmd_buf),
			"/bin/imgdecrypt %s", fota_config_cmd.pFw_path);

		iRet = system(sCmd_buf);
		if (0 == WIFEXITED(iRet) || 0 != WEXITSTATUS(iRet))
		{
			DEBUG("Decrypt firmware fail.\n");
			return -1;
		}
	}

	iRet = check_fw_header(pFw_name,
		fota_config_cmd.client_info.sModel_name);
	if (iRet != 1)
	{
		DEBUG("check_fw_header fail.\n");
		return -1;
	}

	DEBUG("Verify firmware ok.\n");

	return 0;
}

int clear_firmware(const char * pFw_name)
{
	extern struct fota_config fota_config_cmd;

	RETURN_IF_NULL(pFw_name, -1);

	unlink(pFw_name);

	if (1 == fota_config_cmd.iFw_encrypt_flag)
	{
		unlink("/tmp/.firmware.orig");
	}

	return 0;
}

int change_led_color(void)
{
	DEBUG("Now change the color of the LED light.\n");
	// Set WAN LED to "Orange Blinking" when start to firmware upgrade
    system("killall -USR1 timer"); // Disable WanLedControl in timer
#if defined(PRODUCT_DIR853_A2) \
	|| defined(PRODUCT_DIR853_A1) \
	|| defined(PRODUCT_DIR1360)

	system("gpio l 16 0 4000 0 1 4000");//internet ok
	system("gpio l 15 1 1 1 1 4000");//internet ng

#else

	system("gpio l 3 0 4000 0 1 4000");
	system("gpio l 4 1 1 1 1 4000");

#endif

	return 0;
}

int save_firmware_to_flash(const char * pFw_name)
{
	char sCmd_buf[128] = { 0 };

	DEBUG("Now save the firmware into flash memory.\n");

	RETURN_IF_NULL(pFw_name, -1);

	// Set WAN LED to "Orange Blinking" when start to firmware upgrade
	system("killall -USR1 timer"); // Disable WanLedControl in timer

	snprintf(sCmd_buf, sizeof (sCmd_buf),
		"mtd_write -r -w write %s Kernel &", pFw_name);

	system(sCmd_buf);

	return 0;
}

int reboot(void)
{
	// Set Power LED to "Orange ON" when start to reboot
#if defined(PRODUCT_DIR853_A2) \
	|| defined(PRODUCT_DIR853_A1) \
	|| defined(PRODUCT_DIR1360)

	system("gpio l 14 0 4000 0 1 4000");
	system("gpio l 13 4000 0 1 0 4000");

#else

	system("gpio l 16 0 4000 0 1 4000");
	system("gpio l 8 4000 0 1 0 4000");

#endif

	system("echo \"Reboot\" > /dev/console");

	kill(1, SIGTERM);

	return 0;
}

int detect_process_num(const char * pProcess_name)
{
	FILE *ptr;
	char buff[512];
	char ps[128];
	char sPsName[128] = { 0 };
	int iProcess_num = 0;

	sprintf(ps,"ps | grep %s",pProcess_name);
	if((ptr=popen(ps, "r")) != NULL)
	{
		while (fgets(buff, 512, ptr) != NULL)
		{
			*sPsName = '\0';
			sscanf(buff, " %*s %*s %*s %*s %s ", sPsName);
			/*
			  * 用ps检测进程是否存在,应取ps命令
			  * 输出格式的进程名字段,并完全匹配,
			  * 否则可能出现误判.
			  * 2017-03-08 --liushenghui
			*/
			if (!strcmp(sPsName, pProcess_name))
			{
				pclose(ptr);
				++iProcess_num;
			}
		}
	}
	
	pclose(ptr);   
	return iProcess_num;
}

