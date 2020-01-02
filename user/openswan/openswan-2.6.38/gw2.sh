#!/bin/sh

. /sbin/config.sh

echo 0 > /sys/class/net/eth3/queues/rx-0/rps_cpus
echo 5 > /sys/class/net/eth2/queues/rx-0/rps_cpus

if [ "$1" == "1" -o "$1" == "ip4trans" ]; then
ipsec_add_main_in_conf.sh CONN2 psk 10.10.20.254 "10.10.20.254" "10.10.20.254" single 10.10.20.253 "10.10.20.253" "10.10.20.253" yes 123456789 transport
fi

if [ "$1" == "0" -o "$1" == "ip4tun" ]; then
route add default gw 10.10.20.253
if [ "$2" != "ah+esp" ]; then
ipsec_add_main_in_conf.sh CONN2 psk @RIGHT "10.10.20.254" "10.10.10.0/24" subnet @LEFT "10.10.20.253" "10.10.40.0/24" no 123456789 tunnel
fi
fi

if [ "$1" == "5" -o "$1" == "pc" ]; then
ipsec_add_main_in_conf.sh CONN2 psk "10.10.20.254" "10.10.20.254" "10.10.10.0/24" subnet "10.10.20.252" "10.10.20.252" "10.10.20.252/32" yes 123456789 tunnel
route add default gw 10.10.20.254
fi

if [ "$1" == "4in6tun" ]; then
net_gw2.sh
cp /sbin/ipsec_4in6_gw2.conf /etc/ipsec.conf
cp /sbin/ipsec_4in6_gw2.secrets /etc/ipsec.secrets
fi

if [ "$1" == "ip6trans" ]; then
ip -6 addr add 2001:bbb:6401:8000::2/64 dev eth3
route -A inet6 add default gw 2001:bbb:6401:8000::1
echo 1 >/proc/sys/net/ipv6/conf/all/forwarding
cp /sbin/ipsec_ip6_gw2.conf /etc/ipsec.conf
cp /sbin/ipsec_4in6_gw2.secrets /etc/ipsec.secrets
fi

if [ "$1" != "4in6tun" -a "$1" != "ip6trans" -a "$2" != "ah+esp" ]; then
if [ ! -z "$2" ]; then
if [ "$2" == "aes" ]; then
ipsec_add_ikekey_in_conf.sh psk aes128 sha1 $3 modp1024 $3 no
fi
if [ "$2" == "3des" ]; then
ipsec_add_ikekey_in_conf.sh psk 3des sha1 $3 modp1024 $3 no
fi
else
ipsec_add_ikekey_in_conf.sh psk aes128 sha1  86400 modp1024 86400 no
fi
fi

if [ "$1" == "0" -o "$1" == "ip4tun" ]; then
iptables -I FORWARD -s 10.10.10.0/24 -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1452
iptables -I FORWARD -d 10.10.40.0/24 -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1452
iptables -I FORWARD -s 10.10.10.0/24 -p tcp --tcp-flags SYN,RST SYN  -j TCPMSS --clamp-mss-to-pmtu
iptables -I FORWARD -d 10.10.40.0/24 -p tcp --tcp-flags SYN,RST SYN  -j TCPMSS --clamp-mss-to-pmtu
#iptables -t nat -F
iptables -t nat -I POSTROUTING -o eth3 -s 10.10.10.0/24 -j ACCEPT
ifconfig br0 mtu 1452
echo 1 >/proc/sys/net/ipv4/ip_forward
fi

if [ "$1" == "ip4tun" -a "$2" == "ah+esp" ]; then
cp /sbin/ipsec_gw2.conf /etc/ipsec.conf
cp /sbin/ipsec_gw2.secrets /etc/ipsec.secrets
fi




if [ "$CONFIG_ARCH_MT7623" == "y" ]; then
insmod /lib/modules/3.10.20/kernel/drivers/net/cryptoDriver_eip97/crypto_eip97_k.ko
# set irq230 and irq231 to different CPU
echo 1 > /proc/irq/231/smp_affinity
echo 2 > /proc/irq/230/smp_affinity
#
echo 4 > /proc/irq/129/smp_affinity
echo 4 > /proc/irq/114/smp_affinity
echo 8 > /proc/irq/115/smp_affinity
echo 4 > /proc/irq/116/smp_affinity
echo 8 > /proc/irq/123/smp_affinity
echo 1 > /proc/irq/225/smp_affinity
else
insmod crypto_k
fi



if [ "$1" != "help" -a "$1" != "-h" -a "$1" != "?" -a "$1" != "-?" ]; then
ipsec_connect.sh ike CONN2&
else
echo "=== openswan config ==="
echo "gw2.sh ip4tun  : ipv4 tunnel mode "
echo "gw2.sh ip4trans : ipv4 transport mode"
echo "gw2.sh 4in6tun : 4in6 tunnel mode"
echo "gw2.sh ip6tun : ipv6 tunnel mode"
#echo "gw2.sh ip6trans : ipv6 transport mode"
echo "======================="
fi
