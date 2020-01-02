#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "md5.h"
#include "mkconfig.h"


//#define DEBUG_MODE
#ifdef DEBUG_MODE 
#define DEBUG(fmt, args...) printf(fmt,##args)
#else
#define DEBUG(fmt, args...) {do { ; } while(0);}
#endif

#define PROGNAME			"mkconfig"
#define VERSION				"1.00"
#define SEAMA_DE_ENCA_OUTPUT_FILE "output_seama" //调用seama解封装的时候，产生的文件名称
#define MAX_META_DATA		64
#define MAX_FILE_PATH_LENGTH 128

extern int optind;
extern char * optarg;

static char *	o_action = NULL;           //action encapsulate or de-encapsulate
static char *	o_input_file = NULL;	   // input file,the file you want to encapsulate or de-encapsulate
static char *	o_output_file = NULL;	   // the output file is that you encapsulated or de-encapsulated
static char *	o_meta_data = NULL;	       // meta data is used to identify the uniqueness of the product

static size_t calculate_digest(FILE * fh, size_t size, uint8_t * digest);
static size_t write_config_md5(FILE * fh, uint8_t * checksum);



static void cleanup_exit(int exit_code)
{
	DEBUG("%s: exit with code %d\n", PROGNAME, exit_code);
	exit(exit_code);
}

static void show_usage(int exit_code)
{
	printf(	PROGNAME " version " VERSION "\n"
			"this program is used to encapsulate a config file, or de-encapsulate a encapsulated config file\n"
			"=================================================================================================\n"
			"usage: "PROGNAME" -a [enca|de-enca] -m <meta data> -i <input-file> -o <output-file>\n"
			"  -a action.     encapsulate a config file, or de-encapsulate a encapsulated config file.\n"
			"  -m meta data.  such as <board_info=hardware_software_product-model>\n"
			"  -i input file.    the file you want to encapsulate or de-encapsulate.\n"
			"  -o output file.   the output file is that you encapsulated or de-encapsulated\n"
			"  -h show this help message.\n"
			"=================================================================================================\n\n\n"
			);
	cleanup_exit(exit_code);
}

static int parse_args(int argc, char * argv[])
{
	int opt;
	if (argc != 9) {
		show_usage(-1);
		return 0;
	}

	while ((opt = getopt(argc, argv, "ha:i:m:o:")) > 0)
	{
		switch (opt)
		{
		default:	show_usage(-1); break;
		case 'h':	show_usage(0); break;
		case 'a':	o_action = optarg; break;
		case 'i':	o_input_file = optarg; break;
		case 'o':	o_output_file = optarg; break;
		case 'm':   o_meta_data = optarg; break;
		}
	}

	if (strncmp(o_action,"enca",strlen("enca")) != 0 && strncmp(o_action,"de-enca",strlen("de-enca")) != 0) {
		show_usage(-1);
		return 0;	
	}

	if (o_meta_data != NULL && strlen(o_meta_data) > MAX_META_DATA) {
		printf("meta data length is too long, must be less than %d\n",MAX_META_DATA);
		cleanup_exit(-1);
	}

	if (strlen(o_input_file) > MAX_FILE_PATH_LENGTH || strlen(o_output_file) > MAX_FILE_PATH_LENGTH) {
		printf("input or output file path length is too long, must be less than %d\n",MAX_FILE_PATH_LENGTH);
		cleanup_exit(-1);
	}
	return 0;
}

int calculate_config_md5(char *file,uint8_t *config_md5)
{
	FILE * ifh;
	/* Open the input file. */
	ifh = fopen(file, "r+");
	if (ifh){
			calculate_digest(ifh, 0, config_md5);
			fclose(ifh);
			return 0;
	} else {
			printf("Unable to open input file '%s'\n",file);
			return -1;
	}
}

void calculate_meta_data_md5(char *meta_data,uint8_t *meta_data_md5)
{
	MD5_CTX ctx;
	
	MD5_Init(&ctx);
	MD5_Update(&ctx, meta_data, strlen(meta_data));
	MD5_Final(meta_data_md5, &ctx);
}

void combine_file_and_md5(uint8_t *file_name,uint8_t *config_md5)
{
	FILE * fh;

	/* Open the  file. */
	fh = fopen(file_name, "ab");
	if (fh) {
		write_config_md5(fh, config_md5);
		fclose(fh);
	}else{
		printf("Unable to open file '%s'\n",file_name);
	}
}

int System(const char *command)
{
	int ret = system(command);
	
	if (-1 == ret) {      //创建子进程等准备工作。如果失败，返回-1。
		//printf("de-encapsulate file fail!\n");	
		return -1;
	} else {  
       //printf("exit status value = [0x%x]\n", ret);   //如果拉起失败或者shell未正常执行结束（参见备注1），原因值被写入到status的低8~15比特位中。
		if (WIFEXITED(ret)) {  
			if (0 == WEXITSTATUS(ret)) {         
				return 0;
			} else {  
				return -1;
			}  
		} else { 
			return -1;
		}  
    } 		
}

void enca_files(void)
{
	uint8_t config_md5[16];
	uint8_t meta_data_md5[16];
	int ret;
	uint8_t cmd[MAX_META_DATA + MAX_FILE_PATH_LENGTH * 2 + 64] = {0}; 
	int i = 0;
	uint8_t key[64] = {0};
	uint8_t file_name[MAX_FILE_PATH_LENGTH + 10] = {0};

	//计算配置文件的MD5
	ret = calculate_config_md5(o_input_file,config_md5);
	if (ret < 0) {
		return;
	}
	
	DEBUG("config md5 =");
	for(i = 0; i < 16; i++) {
		DEBUG("%02X",config_md5[i]);
	}
	DEBUG("\n\n");

	//计算meta data的MD5,此MD5传递给openssl程序作为加密时候的key值
	calculate_meta_data_md5(o_meta_data,meta_data_md5);

	DEBUG("meta data md5 =");
	for(i = 0; i < 16; i++) {
		DEBUG("%02X",meta_data_md5[i]);
		sprintf(key + i * 2,"%02X",meta_data_md5[i]);
	}
	DEBUG("\n\n");
	DEBUG("key = %s\n",key);

	//加密配置文件
	sprintf(cmd,"openssl enc -e -aes-256-ecb -k %s -in %s -out %s.enc >/dev/null 2>&1",key,o_input_file,o_input_file);
	DEBUG("cmd = %s\n",cmd);
	ret = System(cmd);

	if (ret < 0) {
		printf("encapsulate file fail!\n");
		return;
	}

	//将加密之后的配置文件和没加密之前的配置文件的MD5组成一个文件,即给输入的文件后面加上MD5
	sprintf(file_name,"%s.enc",o_input_file);
	combine_file_and_md5(file_name,config_md5);

	//生成最终的配置文件
	sprintf(cmd,"seama -i %s.enc -m %s >/dev/null 2>&1",o_input_file,o_meta_data);
	DEBUG("cmd = %s\n",cmd);
	ret = System(cmd);

	if (ret < 0) {
		printf("encapsulate file fail!\n");
		return;
	}

	sprintf(cmd,"mv %s.enc.seama %s >/dev/null 2>&1",o_input_file,o_output_file);
	DEBUG("cmd = %s\n",cmd);
	ret = System(cmd);

	if (ret < 0) {
		printf("encapsulate file fail!\n");
	}

	sprintf(cmd,"rm -rf  %s.enc >/dev/null 2>&1",o_input_file);
	DEBUG("cmd = %s\n",cmd);
	ret = System(cmd);

	if (ret < 0) {
		printf("encapsulate file fail!\n");
		return;
	}
	printf("encapsulate file success!\n");

}


void de_enca_files(void)
{
	uint8_t cmd[MAX_META_DATA + MAX_FILE_PATH_LENGTH * 2 + 64] = {0};
	int ret = 0;
	FILE * fh = NULL;
	uint8_t meta_data_md5[16];
	uint8_t config_md5[64] = {0};
	uint8_t config_md51[64] = {0};
	int i,n;
	long config_len;
	int fd = 0;
	uint8_t key[64] = {0};
	uint8_t buf[64] = {0};

	sprintf(cmd,"seama -x %s -i  %s -m %s >/dev/null 2>&1",SEAMA_DE_ENCA_OUTPUT_FILE,o_input_file,o_meta_data);
	DEBUG("cmd = %s\n",cmd);
	ret = System(cmd);
	if (ret < 0) {
		printf("de-encapsulate file fail!\n");
		return;
	} 
	ret = access(SEAMA_DE_ENCA_OUTPUT_FILE,F_OK);
	if (ret < 0) {
		printf("de-encapsulate file fail!\n");
		return;
	}

	//获取配置文件的原始MD5值
	fh  = fopen(SEAMA_DE_ENCA_OUTPUT_FILE, "r");
	if (fh == NULL) {
		printf("de-encapsulate file fail!\n");	
		return;
	}
	ret = fseek(fh,-16L,SEEK_END);
	if (ret != 0) {
		printf("de-encapsulate file fail!\n");	
		return;
	}
	config_len = ftell(fh);
	fread(config_md5, sizeof(uint8_t), 16, fh);
	//printf("n = %d\n",n);
	DEBUG("ori md5 = ");
	for (i=0; i<16; i++) DEBUG("%02X", config_md5[i]);
	DEBUG("\n\n");
	fclose(fh);

	fd = open(SEAMA_DE_ENCA_OUTPUT_FILE, O_RDWR);
	if (fd < 0) {
		printf("de-encapsulate file fail!\n");	
		return;
	}
	ftruncate(fd,config_len);
    close(fd);

	//计算meta data的MD5,此MD5传递给openssl程序作为解密时候的key值
	calculate_meta_data_md5(o_meta_data,meta_data_md5);

	DEBUG("meta data md5 =");
	for(i = 0; i < 16; i++) {
		DEBUG("%02X",meta_data_md5[i]);
		sprintf(key + i * 2,"%02X",meta_data_md5[i]);
	}
	DEBUG("\n\n");
	DEBUG("key = %s\n",key);
	
	//解密配置文件
	sprintf(cmd,"openssl enc -d -aes-256-ecb -k %s -in %s -out %s.dec >/dev/null 2>&1",key,SEAMA_DE_ENCA_OUTPUT_FILE,SEAMA_DE_ENCA_OUTPUT_FILE);
	DEBUG("cmd = %s\n",cmd);
	//openssl如果解密失败的话，会返回1，在这里需要检测一下，如果返回0，则认为解密成功
	ret = System(cmd);

	if (ret < 0) {
		printf("de-encapsulate file fail!\n");
		return;
	} 

	//计算解密出来的配置文件MD5值
	sprintf(buf,"%s.dec",SEAMA_DE_ENCA_OUTPUT_FILE);
	ret = calculate_config_md5(buf,config_md51);
	if (ret < 0) {
		printf("de-encapsulate file fail!\n");	
		return;
	}
	DEBUG("config file md5 = ");
	for (i=0; i<16; i++) DEBUG("%02X", config_md51[i]);
	DEBUG("\n\n");

	//比较解密出的MD5值是否和原始值一致
	if(strncmp(config_md5,config_md51,strlen(config_md5)) != 0) {
		printf("de-encapsulate file fail!\n");	
		return;
	}

	
	//生成最原始的配置文件
	sprintf(cmd,"mv %s.dec %s >/dev/null 2>&1",SEAMA_DE_ENCA_OUTPUT_FILE,o_output_file);
	ret = System(cmd);
	if (ret < 0) {
		printf("de-encapsulate file fail!\n");
		return;
	} 
	sprintf(cmd,"rm -rf %s >/dev/null 2>&1",SEAMA_DE_ENCA_OUTPUT_FILE);
	ret = System(cmd);
	if (ret < 0) {
		printf("de-encapsulate file fail!\n");
		return;
	} 
	
	printf("de-encapsulate file success!\n");	
	return;
}



/*******************************************************************/

static size_t calculate_digest(FILE * fh, size_t size, uint8_t * digest)
{
	MD5_CTX ctx;
	size_t bytes_left, bytes_read, i;
	uint8_t buf[1024];

	bytes_left = size ? size : sizeof(buf);
	bytes_read = 0;

	MD5_Init(&ctx);
	while (!feof(fh) && !ferror(fh) && bytes_left > 0)
	{
		i = bytes_left < sizeof(buf) ? bytes_left : sizeof(buf);
		i = fread(buf, sizeof(char), i, fh);
		if (i > 0)
		{
			MD5_Update(&ctx, buf, i);
			bytes_read += i;
		}
		if (size) bytes_left -= i;
	}
	MD5_Final(digest, &ctx);
	return bytes_read;
}

static size_t write_config_md5(FILE * fh, uint8_t * checksum)
{
	return fwrite(checksum, sizeof(uint8_t), 16, fh);
}

/*******************************************************************/
int main(int argc, char * argv[], char * env[])
{
	//printf("mkconfig version " VERSION "\n");

	/* parse the arguments */
	if (parse_args(argc, argv) < 0) show_usage(9);

	/* Do the works */
	if (strncmp(o_action,"enca",strlen("enca")) == 0) {
		enca_files();
	} else if (strncmp(o_action,"de-enca",strlen("de-enca")) == 0){
		de_enca_files();
	} 
	cleanup_exit(0);
	return 0;
}

