#!/bin/sh

PRODUCT_ID="AP-MTKH7-0002-dlink-sdk-"
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
