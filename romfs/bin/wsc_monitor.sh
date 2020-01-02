timeOut=$1

STATUS_WSC_USER_IDEL=0
STATUS_WSC_USER_SUCCESS=1
STATUS_WSC_USER_TIMEOUT=2
STATUS_WSC_USER_OVERLAP=3

if [ "$1" == "" ]; then
#default time is 120 sec.
timeOut=120
fi
while [ "$timeOut" -ne "0" ]
do
statusWsc=`cat /proc/wsc/wsc_status`
echo "WSC status is $statusWsc"
if [ "$statusWsc" == "$STATUS_WSC_USER_SUCCESS" ]; then
/etc_ro/lighttpd/www/web/wireless.cgi wps_success 0
echo 0 > /proc/wsc/wsc_status
break
fi
if [ "$statusWsc" == "$STATUS_WSC_USER_TIMEOUT" ]; then
/etc_ro/lighttpd/www/web/wireless.cgi wps_timeout 0
echo 0 > /proc/wsc/wsc_status
break
fi
if [ "$statusWsc" == "$STATUS_WSC_USER_OVERLAP" ]; then
/etc_ro/lighttpd/www/web/wireless.cgi wps_overlap 0
echo 0 > /proc/wsc/wsc_status
break
fi
timeOut=`expr $timeOut - 1`
sleep 1
done
if [ "$timeOut" -eq "0" ]; then
/etc_ro/lighttpd/www/web/wireless.cgi wps_timeout 0
echo 0 > /proc/wsc/wsc_status
fi
