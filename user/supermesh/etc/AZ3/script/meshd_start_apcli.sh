#!/bin/sh
brif=${1}
ifname=${2}
security=${3}
encrypt=${4}
ssid=${5}
bssid=${6}
channel=${7}

ifconfig ${ifname} > /dev/null 2>&1
[ $? -eq 0 ] && {
	ifconfig ${ifname}|grep UP > /dev/null 2>&1
	[ $? -ne 0 ] && {
		ifconfig ${ifname} up
		sleep 1
	}

	iwpriv ${ifname} set ApCliEnable=0
	iwpriv ${ifname} set ApCliAuthMode=${security}
	iwpriv ${ifname} set ApCliEncrypType=${encrypt}
	iwpriv ${ifname} set ApCliBssid=${bssid}
	iwpriv ${ifname} set ApCliSsid=${ssid}
	iwpriv ${ifname} set Channel=${channel}
	iwpriv ${ifname} set ApCliMWDS=1
	iwpriv ${ifname} set ApCliEnable=1

	brctl show |grep ${ifname} > /dev/null 2>&1
	[ $? -ne 0 ] && brctl addif ${brif} ${ifname}
}

