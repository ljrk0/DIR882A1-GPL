romfs:
	$(ROMFSINST) agent/snmpd /bin/snmpd
	$(ROMFSINST) ralink/snmpd.conf /etc_ro/snmpd.conf
