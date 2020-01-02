.PHONY: all romfs clean

CURXMLDIR  = $(shell pwd)

all:
#	@cd libxml2-2.9.2 && touch *.* && touch * && make && cp -rf .libs/libxml2.a ../mips_lib
	./CompileLibxml2.sh $(ROOTDIR)/$(LINUXDIR) '' LTE_L1 $(CROSS_COMPILE)

romfs:
	
clean:
	@cd libxml2-2.9.2 && make clean
