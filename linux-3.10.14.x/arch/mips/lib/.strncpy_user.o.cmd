cmd_arch/mips/lib/strncpy_user.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,arch/mips/lib/.strncpy_user.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff00000000+0x81001000 -DDATAOFFSET=0  -D__ASSEMBLY__  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic         -c -o arch/mips/lib/strncpy_user.o arch/mips/lib/strncpy_user.S

source_arch/mips/lib/strncpy_user.o := arch/mips/lib/strncpy_user.S

deps_arch/mips/lib/strncpy_user.o := \
    $(wildcard include/config/eva.h) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/errno.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/errno.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/errno-base.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
    $(wildcard include/config/sgi/ip28.h) \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/regdef.h \

arch/mips/lib/strncpy_user.o: $(deps_arch/mips/lib/strncpy_user.o)

$(deps_arch/mips/lib/strncpy_user.o):
