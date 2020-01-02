#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include "../../config/autoconf.h"
#include <linux/autoconf.h>

#if defined (CONFIG_RALINK_RT6855A) || defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
#define NO_WIFI_SOC
#endif

#if ! defined (CONFIG_MTK_EMMC_SUPPORT)
struct mtd_info_user {
	u_char type;
	u_int32_t flags;
	u_int32_t size;
	u_int32_t erasesize;
	u_int32_t oobblock;
	u_int32_t oobsize;
	u_int32_t ecctype;
	u_int32_t eccsize;
};

struct erase_info_user {
	u_int32_t start;
	u_int32_t length;
};

#define MEMGETINFO	_IOR('M', 1, struct mtd_info_user)
#define MEMERASE	_IOW('M', 2, struct erase_info_user)
#if defined CONFIG_NVRAM_RW_FILE
#define NVRAM_PATH "/etc/nvram"
#endif

int mtd_open(const char *name, int flags)
{
	FILE *fp;
	char dev[80];
	int i, ret;

	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, name)) {
				snprintf(dev, sizeof(dev), "/dev/mtd/%d", i);
				if ((ret = open(dev, flags)) < 0) {
					snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
					ret = open(dev, flags);
				}
				fclose(fp);
				return ret;
			}
		}
		fclose(fp);
	}
	return -1;
}

int flash_read_mac(char *buf)
{
#if defined CONFIG_NVRAM_RW_FILE
	memset(buf, 0, 6);
	return 0;
#else
	int fd, ret;

	if (!buf)
		return -1;
	fd = mtd_open("Factory", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
#if ! defined (NO_WIFI_SOC)
	lseek(fd, 0x2E, SEEK_SET);
#else
	lseek(fd, 0xE006, SEEK_SET);
#endif
	ret = read(fd, buf, 6);
	close(fd);
	return ret;
#endif
}

int flash_read_NicConf(char *buf)
{
	int fd, ret;

	if (!buf)
		return -1;
	fd = mtd_open("Factory", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
	lseek(fd, 0x34, SEEK_SET);
	ret = read(fd, buf, 6);
	close(fd);
	return ret;
}

int flash_read(char *buf, off_t from, size_t len)
{
	int fd, ret;
#if defined CONFIG_NVRAM_RW_FILE
	FILE *fp = fopen(NVRAM_PATH, "r");
	
	if (fp == NULL) {
		perror(__FUNCTION__);
		return -1;
	}
	rewind(fp);
	fseek(fp, from, SEEK_SET);
	ret = (int) fread(buf, 1, len, fp);
	fclose(fp);
#else
	struct mtd_info_user info;

	fd = mtd_open("Config", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes - %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	lseek(fd, from, SEEK_SET);
	ret = read(fd, buf, len);
	if (ret == -1) {
		fprintf(stderr, "Reading from mtd failed\n");
		close(fd);
		return -1;
	}

	close(fd);
#endif
	return ret;
}

#define min(x,y) ({ typeof(x) _x = (x); typeof(y) _y = (y); (void) (&_x == &_y); _x < _y ? _x : _y; })

int flash_write(char *buf, off_t to, size_t len)
{
	int fd, ret = 0;
#if defined CONFIG_NVRAM_RW_FILE
	FILE *fp;
	
	if ((fp = fopen(NVRAM_PATH, "r+")) == NULL && 
	    (fp = fopen(NVRAM_PATH, "w+")) == NULL) {
		perror(__FUNCTION__);
		return -1;
	}
	rewind(fp);
	fseek(fp, to, SEEK_SET);
	ret = (int) fwrite(buf, 1, len, fp);
	fclose(fp);
#else
	char *bak = NULL;
	struct mtd_info_user info;
	struct erase_info_user ei;

	fd = mtd_open("Config", O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes: %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	while (len > 0) {
		if ((len & (info.erasesize-1)) || (len < info.erasesize)) {
			int piece_size;
			unsigned int piece, bakaddr;

			bak = (char *)malloc(info.erasesize);
			if (bak == NULL) {
				fprintf(stderr, "Not enough memory\n");
				close(fd);
				return -1;
			}

			bakaddr = to & ~(info.erasesize - 1);
			lseek(fd, bakaddr, SEEK_SET);

			ret = read(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Reading from mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			piece = to & (info.erasesize - 1);
			piece_size = min(len, info.erasesize - piece);
			memcpy(bak + piece, buf, piece_size);

			ei.start = bakaddr;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			lseek(fd, bakaddr, SEEK_SET);
			ret = write(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			free(bak);
			buf += piece_size;
			to += piece_size;
			len -= piece_size;
		}
		else {
			ei.start = to;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				return -1;
			}

			ret = write(fd, buf, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			buf += info.erasesize;
			to += info.erasesize;
			len -= info.erasesize;
		}
	}

	close(fd);
#endif
	return ret;
}

#else
static int get_start_address(long long *size, long long *start_addr)
{
	FILE *fp = fopen("/proc/emmc_partition", "r");
	char p_name[20];
	char p_size[20];
	char p_start_addr[20];

	if (fp == NULL) {
		fprintf(stderr, "Can't open %s\n", "/proc/emmc_partition");
		return -1;
	}
	memset(p_name, 0, 20);
	while (EOF != fscanf(fp, "\n%s\t%s\t%s\t%*s\t%*s",
				p_name, p_size, p_start_addr)) {
		if (!strcmp(p_name, "factory"))
			break;
		memset(p_name, 0, 20);
		memset(p_size, 0, 20);
		memset(p_start_addr, 0, 20);
	}
	*size = strtoll(p_size, NULL, 16);
	*start_addr = strtoull(p_start_addr, NULL, 16);
	if (size == 0) {
		fprintf(stderr, "not found \"factory\" partition\n");
		fclose(fp);
		return -1;
	}
	fclose(fp);

	return 0;
}

int flash_read_mac(char *buf)
{
	int fd, ret;
	long long size, start_addr;

	if (!buf)
		return -1;
	fd = open("/dev/mmcblk0", O_RDWR | O_SYNC);
	if(fd < 0) {
		fprintf(stderr, "Could not open emmc device: %s\n", "/dev/mmcblk0");
		exit(1);
		return -1;
	}
	if (get_start_address(&size, &start_addr) < 0) {
		close(fd);
		exit(1);
	}
#if ! defined (NO_WIFI_SOC)
	lseek(fd, start_addr+0x2E, SEEK_SET);
#else
	lseek(fd, start_addr+0xE006, SEEK_SET);
#endif
	ret = read(fd, buf, 6);
	//printf("%02x:%02x:%02x:%02x:%02x:%02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
	close(fd);
	return ret;
}
#endif
