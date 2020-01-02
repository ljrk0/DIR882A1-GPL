#!/bin/sh

write_action()
{
	COUNTER=`expr $COUNTER + 1`
	nvram_set TC $COUNTER
	cp /sbin/smbd /media/jffs2/smbd$COUNTER
	cp /sbin/smbd /media/ubi/smbd$COUNTER
	echo $COUNTER >> /media/jffs2/test
	echo $COUNTER >> /media/ubi/test
	sync
	sleep 5 
}

sleep 20
COUNTER=`nvram_get TC`
jffs2_mounted=`mount | grep jffs2 | wc -l`
if [ $jffs2_mounted -lt 1 ]; then
	mkdir -p /media/jffs2
	mount -t jffs2 /dev/mtdblock5 /media/jffs2
fi
ubifs_mounted=`mount | grep ubifs | wc -l`
if [ $ubifs_mounted -lt 1 ]; then
	ubiattach -p /dev/mtd6
	mkdir -p /media/ubi
	mount -t ubifs /dev/ubi0_0 /media/ubi
fi
jffs2_mounted=`mount | grep jffs2 | wc -l`
ubifs_mounted=`mount | grep ubifs | wc -l`
if [ $jffs2_mounted -lt 1 -o $ubifs_mounted -lt 1 ]; then
	echo "!!! FAIL to mount JFFS2 or UBIFS !!!"
	echo "!!! Test STOP !!!"
	exit
fi
if [ "$COUNTER" == "" ]; then
	COUNTER=0
	write_action
	echo "Initial Test OK!"
	reboot
fi
SMBDCRC=`md5sum /sbin/smbd | sed 's/\/sbin\/.*//'`
TestFile=/media/jffs2/smbd$COUNTER
JFFS2FileCRC=`md5sum $TestFile | sed 's/\/media\/.*//'`
TestFile=/media/ubi/smbd$COUNTER
UBIFileCRC=`md5sum $TestFile | sed 's/\/media\/.*//'`
JFFS2_CHK=`sed -n '$p' /media/jffs2/test`
UBI_CHK=`sed -n '$p' /media/ubi/test`
echo $SMBDCRC
echo $JFFS2FileCRC
echo $UBIFileCRC
echo $JFFS2_CHK
echo $UBI_CHK
if [ $COUNTER == $JFFS2_CHK -a $COUNTER == $UBI_CHK -a $SMBDCRC == $JFFS2FileCRC -a $SMBDCRC == $UBIFileCRC ]; then
	rm -f /media/jffs2/smbd*
	rm -f /media/ubi/smbd*
	echo "##### $COUNTER Test OK! #####"
	write_action
	reboot
else
	echo "!!! Check FAIL !!!"
	echo "!!! Test STOP !!!"
fi
