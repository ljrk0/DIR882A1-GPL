#!/bin/sh

. /sbin/config.sh

IRQ="$1"
CPU="$2"

if [ $CONFIG_RALINK_MT7621 == "y" ]; then

if [ ! -n "$2" ]; then
  echo ""
  echo "insufficient arguments!"
  echo "Usage: $0 <irq:1~31> <cpu:1~4>"
  echo ""
  exit 0
fi

if [ $IRQ -lt "1" -o $IRQ -gt "31" ]; then
  echo ""
  echo " WARNING!! irq=$IRQ is invalid (should be 1 ~ 31)"
  echo ""
  exit 0
fi

if [ $CPU != "1" -a $CPU != "2" -a $CPU != "3" -a $CPU != "4" ]; then
  echo ""
  echo " WARNING!! cpu=$CPU is invalid (should be 1 ~ 4)"
  echo ""
  exit 0
fi

if [ $CPU == "1" ]; then
	echo 1 > /proc/irq/$IRQ/smp_affinity
elif [ $CPU == "2" ]; then
	echo 2 > /proc/irq/$IRQ/smp_affinity
elif [ $CPU == "3" ]; then
	echo 4 > /proc/irq/$IRQ/smp_affinity
elif [ $CPU == "4" ]; then
	echo 8 > /proc/irq/$IRQ/smp_affinity
fi

else #MT7623

if [ ! -n "$2" ]; then
  echo ""
  echo "insufficient arguments!"
  echo "Usage: $0 <irq:1~FF HEX> <cpu:1~2>"
  echo ""
  exit 0
fi

if [ $IRQ -lt "1" -o $IRQ -gt "255" ]; then
  echo ""
  echo " WARNING!! irq=$IRQ is invalid (should be 1 ~ 255)"
  echo ""
  exit 0
fi

if [ $CPU != "1" -a $CPU != "2" ]; then
  echo ""
  echo " WARNING!! cpu=$CPU is invalid (should be 1 ~ 2)"
  echo ""
  exit 0
fi

	reg s f0211800
	reg w $IRQ $CPU
	echo "set IRQ$IRQ to CPU$CPU"

fi

