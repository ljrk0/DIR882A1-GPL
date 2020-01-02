#!/bin/sh
brif=${1}
ifname=${2}
security=${3}
encrypt=${4}
ssid=${5}

ifconfig ${ifname} > /dev/null 2>&1
[ $? -eq 0 ] && {
	ifconfig ${ifname}|grep UP > /dev/null 2>&1
	[ $? -ne 0 ] && {
		ifconfig ${ifname} up
		sleep 1
	}

	iwpriv ${ifname} set AuthMode=${security}
	iwpriv ${ifname} set EncrypType=${encrypt}
	iwpriv ${ifname} set HideSSID=1
	iwpriv ${ifname} set ApMWDS=1
	iwpriv ${ifname} set SSID=${ssid}

	brctl show |grep ${ifname} > /dev/null 2>&1
	[ $? -ne 0 ] && brctl addif ${brif} ${ifname}
}

