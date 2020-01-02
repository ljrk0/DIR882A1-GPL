#!/bin/sh

PRODUCT_NAME=`cat ../../config/autoconf.h | grep CONFIG_CUSTOM_PRODUCT |cut -d ' ' -f 3 | sed 's/\"//g'`
PRODUCT_NAME_EXT=`cat ../../config/autoconf.h | grep CONFIG_PRODUCT |cut -d ' ' -f 3 | sed 's/\"//g'`
if [ "$PRODUCT_NAME" = "DIR-867" -o "$PRODUCT_NAME" = "DIR-1935" ]; then
	PRODUCT_ID="AP-MSM001-0001-dlink-sdk-"	
else
	if [ "$PRODUCT_NAME_EXT" = "DIR_853_A1" -o "$PRODUCT_NAME_EXT" = "DIR_853_A2" ]; then
		if [ "$PRODUCT_NAME_EXT" = "DIR_853_A1" ]; then
		PRODUCT_ID="DIR-853_AX-"
		else
		PRODUCT_ID="DIR-853_A2-"
		fi
	else
	PRODUCT_ID="AP-MTKH7-0002-dlink-sdk-"
	fi
fi
BUILD_TIME=`date "+%Y-%m-%d %T"`
SOFTWARE_VER_FILE="version.h"
echo $BUILD_TIME
PRJ_SVN_REVISION=`svn info $1 | grep "Revision:"| cut -d ' ' -f  2 `
LAST_CHANGED_AUTHOR=`svn info $2 | grep "Last Changed Author:"| cut -d ' ' -f  4 `
PWD_AUTHOR=`pwd | cut -d '/' -f 3`
#echo 'svn last changed rev: '$PRJ_SVN_REVISION > $SOFTWARE_VER_FILE
echo '#define TW_SW_VERSION' \"$PRODUCT_ID$PRJ_SVN_REVISION\" > $SOFTWARE_VER_FILE
echo '#define SW_BUILD_TIME' \"$BUILD_TIME\" >> $SOFTWARE_VER_FILE
mv $SOFTWARE_VER_FILE include/
