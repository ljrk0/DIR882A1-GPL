
. /sbin/config.sh

usage()
{
	echo "Usage:"
	echo "  $0 0 - Disable long_loop"
	echo "  $0 1 - Enable long_loop: 100M + 100M extend + 10M"
	echo "  $0 2 - Enable long_loop: 100M + 100M extend"
	echo "  $0 3 - Enable long_loop: 100M + 10M"
	exit 0
}

disable_long_loop()
{
	#stop long_loop
	killall -9 long_loop
	long_loop 0


}

enable_long_loop_mode1()
{
	#enable long_loop mode 1
	killall -9 long_loop
	long_loop 1&
}

enable_long_loop_mode2()
{
	#enable long_loop mode 2
	killall -9 long_loop
	long_loop 2&
}

enable_long_loop_mode3()
{
	#enable long_loop mode 3
	killall -9 long_loop
	long_loop 3&
}


if [ "$CONFIG_RALINK_MT7628" = "y" ]; then
	if [ "$1" = "0" ]; then
		disable_long_loop
	elif [ "$1" = "1" ]; then
		enable_long_loop_mode1
	elif [ "$1" = "2" ]; then
		enable_long_loop_mode2
	elif [ "$1" = "3" ]; then
		enable_long_loop_mode3
	else
		usage $0
	fi
else
	echo 'long_loop not supported'
fi
