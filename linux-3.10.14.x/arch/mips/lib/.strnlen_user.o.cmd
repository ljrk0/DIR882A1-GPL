cmd_arch/mips/lib/strnlen_user.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,arch/mips/lib/.strnlen_user.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff80001000 -DDATAOFFSET=0  -D__ASSEMBLY__  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic         -c -o arch/mips/lib/strnlen_user.o arch/mips/lib/strnlen_user.S

source_arch/mips/lib/strnlen_user.o := arch/mips/lib/strnlen_user.S

deps_arch/mips/lib/strnlen_user.o := \
    $(wildcard include/config/eva.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
    $(wildcard include/config/sgi/ip28.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/regdef.h \

arch/mips/lib/strnlen_user.o: $(deps_arch/mips/lib/strnlen_user.o)

$(deps_arch/mips/lib/strnlen_user.o):
