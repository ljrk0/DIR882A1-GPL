cmd_arch/mips/lib/memset.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,arch/mips/lib/.memset.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff00000000+0x81001000 -DDATAOFFSET=0  -D__ASSEMBLY__  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic         -c -o arch/mips/lib/memset.o arch/mips/lib/memset.S

source_arch/mips/lib/memset.o := arch/mips/lib/memset.S

deps_arch/mips/lib/memset.o := \
    $(wildcard include/config/cpu/micromips.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
    $(wildcard include/config/eva.h) \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
    $(wildcard include/config/sgi/ip28.h) \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/regdef.h \

arch/mips/lib/memset.o: $(deps_arch/mips/lib/memset.o)

$(deps_arch/mips/lib/memset.o):
