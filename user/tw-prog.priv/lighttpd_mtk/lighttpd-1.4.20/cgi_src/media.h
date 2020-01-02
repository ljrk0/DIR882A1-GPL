/*
 *	wireless.h -- Wireless Configuration Header 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: //WIFI_SOC/DEV/COMPUTEX-2014/RT288x_SDK/source/user/goahead/src/media.h#1 $
 */

//void formDefineMedia(void);

void cleanup_musiclist(void);
struct status {
	char operate[10];
	char service_mode[2];
	char option[10];
	char target[50];
	// char volume[4];
	// char mute[2];
	char connected[5];
};

struct list {
	char name[230];
	char path[1024];
	int	 selected;
}; 
char track_path[512];
