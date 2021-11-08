#include <sys/types.h>
#include <sys/wait.h>
#include "../shared/shutils.h"

#define TASK_STATUS_FOLDER	"/var/tmp"
#define PID_DIR	"/var/run"
#define DISK_NOT_ENOUGHT	88

/*record division mark*/
#define TASK_MARK		"#task#"
#define TASK_VALUE_MARK "!sub!"

/*status info */
#define STATUS_FINISH_PERCENT 	"finish_percent"
#define STATUS_DOWNLOAD_SPEED	"down_speed"
#define STATUS_TASK_STATUS		"task_status"
#define STATUS_SPEEDER_NUM		"seeder_number"

/*file info*/
#define FILE_NAME 		"name"
#define FILE_SIZE		"size"
#define SAVE_PATH	    "save_path"
#define DONWLOAD_TYPE   "type"
#define DONWLOAD_TARGET "target"
#define USRNAME         "usrname"
#define PASSWORD        "password"

/*path*/
#define DOWNLOADER_FOLDER			    "downloader"
#define DOWNLOADER_FILE_FOLDER		    "download"
#define BT_DOWNLOADER_CONF_FOLDER		"conf"
#define BT_DOWNLOADER_TORRENT_FOLDER	"torrent"
#define TASKS_FILE                      "tasks_file"
#define STATUS                          "status"
#define BT_DOWNLOADER_CONF_PATH			DOWNLOADER_FOLDER"/"BT_DOWNLOADER_CONF_FOLDER
#define BT_DOWNLOADER_TORRENT_PATH		DOWNLOADER_FOLDER"/"BT_DOWNLOADER_TORRENT_FOLDER
#define DOWNLOADER_FILE_PATH			DOWNLOADER_FOLDER"/"DOWNLOADER_FILE_FOLDER

/*BT torrent file's key mark*/
#define BT_TORRENT_LENGTH_START		":length"
#define BT_TORRENT_LENGTH_END 		"e4"
#define BT_TORRENT_NAME				":name"


void tw_Downloader_test();
int tw_Downloader_get_mnt_path(char *mntpath);
int tw_Downloader_add_BT_info(char *torrentPath, char *savePath);
int tw_Downloader_Init_File_list(char *tmpConfpath, int type);
int tw_Downloader_start_Transmission(char* savePath, char* confPath, char* torrentPath, int speed);
int tw_Downloader_Wakeup_New_task();
int tw_Downloader_pause(char *task);
int tw_Downloader_delete(char* confPath, char *task);
int tw_Downloader_Start_Manage_Task(char *tasks,int speed);
int tw_Downloader_clean_finish_task();
int tw_Downloader_Pause_Manage_Task(char *tasks);
int tw_Downloader_Delete_Manage_Task(char *tasks);
char * separate_tasks( char *tasks,char *task);
FILE * get_file_name(FILE *fp, char *name);
void empty_downloader_data();
int check_pid_running(char *name);
int check_num_of_tw_downloader_pid();
int tw_downloader_add_http_ftp_info(char *url);
int tw_Downloader_start_wget(char* savePath, char* url, int type,int speed);
void get_http_ftp_file_name(char *url, char *name);
int add_all_tasks_info(char *file_name, char *task_name);
void init_folder(char *pathname);
int check_task_existent(char *Pstr, char *Dstr, char *flag);
int tw_Downloader_String2HexStr(char *pSrc, char *pDest);
void  remove_out_0x00_dat(char* in_buff,int data_len);

