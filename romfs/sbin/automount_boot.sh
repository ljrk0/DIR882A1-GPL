#! /bin/sh
if [ "$1" == "" ]; then
echo "parameter is none" 
exit 1
else
	echo "***** $1 *****"
fi
mounted=`mount | grep $1 | wc -l`

if ! mkdir -p "/media/$1"; then
exit 1
fi

mount "/dev/$1" "/media/$1"
mounted=`mount | grep $1 | wc -l`
if [ $mounted -lt 1 ]; then
[ -e /sbin/chkexfat ] && chkexfat -f /dev/$1
[ -e /sbin/chkhfs ] && chkhfs -f /dev/$1
[ -e /sbin/chkntfs ] && chkntfs -f /dev/$1
num=3
if [ "$CONFIG_UFSD_FS" == "m" -o "$CONFIG_UFSD_FS" == "y" ]; then
sleep 1
fi
while [ -e /bin/mount -a $mounted -lt 1 -a $num -gt 0 ]
do
	mount "/dev/$1" "/media/$1" 2>/dev/null
	mounted=`mount | grep $1 | wc -l`
	num=`expr $num - 1`
done
mounted=`mount | grep $1 | wc -l`
while [ -e /bin/ntfs-3g -a $mounted -lt 1 -a $num -lt 3 ]
do
	ntfs-3g "/dev/$1" "/media/$1" -o force 2>/dev/null
	mounted=`mount | grep $1 | wc -l`
	num=`expr $num + 1`
done
fi

if [ $mounted -lt 1 ]; then
rm -r "/media/$1"
exit 1
fi

# Goahead need to know the event happened.
killall -SIGTTIN goahead
killall -SIGTTIN nvram_daemon
exit 0

