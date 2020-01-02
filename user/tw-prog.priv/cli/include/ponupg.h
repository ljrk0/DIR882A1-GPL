
#ifndef __PONUPG_H__
#define __PONUPG_H__

#define DL_PATH                     "/var/"
#define DL_IMAGE_FILE       "image.img"
#define DL_GATEWAY_IMAGE "gwimage"
#define DL_PON_IMAGE    "ponimage"


/*********************************************************************
*  功能: 将整个文件读取到一个缓冲区
*  参数:
*  返回: char *
*  创建: huangjidong
*  备注: 返回的字符串需要用户自行释放
*********************************************************************/
char *tbsReadFileToBufWithMalloc(const char *pcFileName, int *piLen);
/*********************************************************************
*  功能: 公共的系统升级函数
*  参数: pcPath     执行升级的路径，比如/var
         pcPack     升级包的名称，比如fullpack
         pcGwImage  升级包中网关升级文件的名称，比如image.img
         pcPonImage 升级包中PON升级文件的名称，比如firmware.uue
*  返回: int
         -4 --- 无文件可升级
         -3 --- PON升级失败
         -2 --- 网关升级失败
         -1 --- 参数检查错误
         0  --- 升级成功
*  创建: huangjidong
*  备注: 将两个文件分割开来升级---实现PON和网关的同时升级
*********************************************************************/
int tbsCommonUpgrader(const char *pcPath, const char *pcPack, const char *pcGwImage, const char *pcPonImage,
    const char *pcUser, const char *pcClearFlag);

#endif

