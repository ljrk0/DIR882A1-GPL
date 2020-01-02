
all:Makefile
	make -f Makefile all
	
Makefile:	
	set CC=mipsel-linux-gcc 
	./configure --host=mipsel-linux --prefix=/opt/buildroot-gdb/bin --enable-static --disable-shared --enable-speed --disable-debugging --enable-sso
	
all-recursive:
	make -f Makefile all-recursive
	
all-am:Makefile
	make -f Makefile all-am
		
clean:Makefile
	make -f Makefile clean

clean-am:Makefile
	make -f Makefile clean-am
	
distclean:Makefile
	make -f Makefile distclean
	rm -f Makefile
		
distclean-am:Makefile
	make -f Makefile distclean-am
	
am--refresh:Makefile
	make -f Makefile am--refresh