#!/bin/sh
#
# $Id: //WIFI_SOC/MP/SDK_5_0_0_0/RT288x_SDK/source/user/rt2880_app/scripts/ddnsv6.sh#1 $
#
# usage: ddns.sh
#

# TODO:
# set ddnstate as disconnected
# echo 0 > /tmp/ddnstate

# TODO:
# clean the inadyn cache
# rm -f /tmp/inadyn_ip.cache

srv=`nvram_get 2860 DDNSProvider`
u=`nvram_get 2860 DDNSAccount`
pw=`nvram_get 2860 DDNSPassword`
#timeout=`nvram_get 2860 DDNSTimeout`
#timeout=`expr $timeout \* 3600`

# killall -q -9 inadyn

echo "srv=$srv"
echo "u=$u"
echo "pw=$pw"
#echo "timeout=$timeout"

for i in 0 1 2 3 4 5 6 7 8 9
do
enabled=`nvram_get 2860 DDNSEnabledv6_${i}`
ddns=`nvram_get 2860 DDNSv6_${i}`
ip=`nvram_get 2860 DDNSIPv6_${i}`

if [ "$enabled" = "1" ]; then
echo "${i}: ${enabled} ${ddns} ${ip}"
if [ "$srv" = "dyndns.org" -o "$srv" = "dyndns.com" ]; then
    wget -s "http://"${u}":"${pw}"@members.dyndns.org/nic/update?hostname="${ddns}"&myip="${ip}"&wildcard=NOCHG&mx=NOCHG&backmx=NOCHG"
else
	echo "$0: unknown DDNS provider: $srv"
	exit 1
fi
fi
done

# set ddnstate as connecting
# echo 1 > /tmp/ddnstate

