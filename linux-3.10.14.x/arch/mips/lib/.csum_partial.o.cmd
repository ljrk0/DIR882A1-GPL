cmd_arch/mips/lib/csum_partial.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,arch/mips/lib/.csum_partial.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff00000000+0x81001000 -DDATAOFFSET=0  -D__ASSEMBLY__  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic         -c -o arch/mips/lib/csum_partial.o arch/mips/lib/csum_partial.S

source_arch/mips/lib/csum_partial.o := arch/mips/lib/csum_partial.S

deps_arch/mips/lib/csum_partial.o := \
    $(wildcard include/config/dma/noncoherent.h) \
    $(wildcard include/config/mips/malta.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
    $(wildcard include/config/64bit.h) \
    $(wildcard include/config/cpu/mipsr2.h) \
    $(wildcard include/config/cpu/little/endian.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
    $(wildcard include/config/cpu/sb1.h) \
    $(wildcard include/config/eva.h) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  /home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/errno.h \
  /home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/errno.h \
  /home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/errno-base.h \
  /home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/sgi/ip28.h) \
  /home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \
  /home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /home/xieshijing/SDK_5_0_2_0/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/regdef.h \

arch/mips/lib/csum_partial.o: $(deps_arch/mips/lib/csum_partial.o)

$(deps_arch/mips/lib/csum_partial.o):
