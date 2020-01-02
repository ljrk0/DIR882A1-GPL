#!/bin/sh
#
# $Id: //WIFI_SOC/MP/SDK_5_0_0_0/RT288x_SDK/source/user/rt2880_app/scripts/ddns.sh#1 $
#
# usage: ddns.sh
#

# set ddnstate as disconnected
echo 0 > /tmp/ddnstate

# clean the inadyn cache
rm -f /tmp/inadyn_ip.cache

enabled=`nvram_get 2860 DDNSEnabled`
srv=`nvram_get 2860 DDNSProvider`
ddns=`nvram_get 2860 DDNS`
u=`nvram_get 2860 DDNSAccount`
pw=`nvram_get 2860 DDNSPassword`
timeout=`nvram_get 2860 DDNSTimeout`
timeout=`expr $timeout \* 3600`

killall -q -9 inadyn

if [ "$enabled" = "" -o "$enabled" = "none" -o "$enabled" = "0" ]; then
        exit 0
fi
if [ "$srv" = "" -o "$srv" = "none" ]; then
	exit 0
fi
if [ "$ddns" = "" -o "$u" = "" -o "$pw" = "" -o "$timeout" = "" ]; then
	exit 0
fi

# debug
echo "enabled=$enabled"
echo "srv=$srv"
echo "ddns=$ddns"
echo "u=$u"
echo "pw=$pw"
echo "timeout=$timeout"

if [ "$srv" = "dyndns.org" -o "$srv" = "dyndns.com" -o "$srv" = "dlinkddns.org" -o "$srv" = "dlinkddns.com" ]; then
	inadyn -u $u -p $pw -a $ddns --dyndns_system default@dyndns.org --update_period_sec $timeout &
elif [ "$srv" = "freedns.afraid.org" ]; then
	inadyn -u $u -p $pw -a $ddns --dyndns_system default@$srv --update_period_sec $timeout &
elif [ "$srv" = "zoneedit.com" ]; then
	inadyn -u $u -p $pw -a $ddns --dyndns_system default@$srv --update_period_sec $timeout &
elif [ "$srv" = "no-ip.com" ]; then
	inadyn -u $u -p $pw -a $ddns --dyndns_system default@$srv --update_period_sec $timeout &
elif [ "$srv" = "oray.cn" ]; then
	inadyn -u $u -p $pw --dyndns_server_name ddns.oray.com --dyndns_server_url /ph/update? -a $ddns --update_period_sec $timeout &
else
	echo "$0: unknown DDNS provider: $srv"
	exit 1
fi

# set ddnstate as connecting
echo 1 > /tmp/ddnstate

