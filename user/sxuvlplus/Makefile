all:

romfs:
	$(ROMFSINST)  -S -p 755 sxuptp.ko /lib/modules/sxuptp.ko
	$(ROMFSINST)  -S -p 755 sxuptp_driver.ko /lib/modules/sxuptp_driver.ko
	$(ROMFSINST)  -S -p 755 sxuptp_wq.ko /lib/modules/sxuptp_wq.ko
	$(ROMFSINST)  -S -p 755 jcpd /sbin/jcpd
	$(ROMFSINST)  -S -p 755 jcpd.conf /etc/jcpd.conf