#!/bin/sh

TOP="/supermesh"

if [ "$(grep localhost /etc/hosts)" == "" ]; then
	echo -n "Please add '127.0.0.1 localhost' in /etc/hosts"
	echo "127.0.0.1 localhost" >> /etc/hosts
	#exit
fi

if [ ! -d /etc/mosquitto ]; then
	cp -rf /etc_ro/mosquitto /etc/
fi

if [ ! -d /etc/AZ3 ]; then
	cp -rf /etc_ro/AZ3 /etc/
fi

mkdir -p /etc/config 
#if [ "$(nvram_get 2860 wan_wan0_proto)" == "Bridge" ]; then
#	cp -rf /etc_ro/config/apson.ap   /etc/config/apson 
#else
#	cp -rf /etc_ro/config/apson   /etc/config/apson 
#fi

insmod /lib/modules/3.10.14+/meshm.ko
sleep 1

if [ -e /etc/mosquitto/mosquitto.conf ]; then	
	mosquitto -c /etc/mosquitto/mosquitto.conf -d
fi

sbd
sleep 1
apsond
sleep 1
meshd
