ra_dir_y	+= notify_rc
ra_dir_y	+= rcm
ra_dir_y	+= process

all romfs:
	for i in $(ra_dir_y) ; do make -C $$i $@ || exit $?; done


