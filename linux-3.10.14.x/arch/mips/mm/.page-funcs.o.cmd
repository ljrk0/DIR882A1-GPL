cmd_arch/mips/mm/page-funcs.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,arch/mips/mm/.page-funcs.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/baifan/dlink_882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/baifan/dlink_882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/baifan/dlink_882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/baifan/dlink_882/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff00000000+0x81001000 -DDATAOFFSET=0  -D__ASSEMBLY__  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/baifan/dlink_882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/baifan/dlink_882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic          -c -o arch/mips/mm/page-funcs.o arch/mips/mm/page-funcs.S

source_arch/mips/mm/page-funcs.o := arch/mips/mm/page-funcs.S

deps_arch/mips/mm/page-funcs.o := \
    $(wildcard include/config/sibyte/dma/pageops.h) \
  /home/baifan/dlink_882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
    $(wildcard include/config/eva.h) \
    $(wildcard include/config/sgi/ip28.h) \
  /home/baifan/dlink_882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \
  /home/baifan/dlink_882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/regdef.h \

arch/mips/mm/page-funcs.o: $(deps_arch/mips/mm/page-funcs.o)

$(deps_arch/mips/mm/page-funcs.o):
