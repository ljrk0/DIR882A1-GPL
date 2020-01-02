#ra_dir_y	+= button
#ra_dir_y	+= downloader
#ra_dir_y	+= logmonitor
#ra_dir_y	+= pcmonitor
ra_dir_y	+= protest
#ra_dir_y	+= rssi
ra_dir_y	+= timer


all romfs:
	for i in $(ra_dir_y) ; do make -C $$i $@ || exit $?; done

clean:
	for i in $(ra_dir_y) ; do make -C $$i clean ; done

