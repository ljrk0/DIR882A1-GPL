
.PHONY: romfs
romfs:
	$(ROMFSINST) cacert.pem /etc_ro/cacert.pem
	make -C common romfs
	make -C fota romfs
	make -C fota_config romfs
	make -C fota_output romfs
