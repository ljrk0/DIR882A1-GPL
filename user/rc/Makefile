EXEC = rc

all: $(EXEC)

romfs:
	$(ROMFSINST) /bin/$(EXEC)
	$(RM) -rf $(ROOTDIR)/romfs/sbin/preinit
	ln -s /bin/rc $(ROOTDIR)/romfs/sbin/preinit
	$(RM) -rf $(ROOTDIR)/romfs/sbin/hotplug
	ln -s /bin/rc $(ROOTDIR)/romfs/sbin/hotplug

