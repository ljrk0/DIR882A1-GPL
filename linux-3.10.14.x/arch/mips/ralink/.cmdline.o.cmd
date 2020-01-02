cmd_arch/mips/ralink/cmdline.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,arch/mips/ralink/.cmdline.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff00000000+0x81001000 -DDATAOFFSET=0 -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -Werror -Werror    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(cmdline)"  -D"KBUILD_MODNAME=KBUILD_STR(cmdline)" -c -o arch/mips/ralink/cmdline.o arch/mips/ralink/cmdline.c

source_arch/mips/ralink/cmdline.o := arch/mips/ralink/cmdline.c

deps_arch/mips/ralink/cmdline.o := \
    $(wildcard include/config/rt2880/rootfs/in/flash.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/rt2880/rootfs/in/ram.h) \
    $(wildcard include/config/uboot/cmdline.h) \
  include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
    $(wildcard include/config/modules.h) \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/uapi/linux/types.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/types.h \
    $(wildcard include/config/64bit/phys/addr.h) \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/types.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/posix_types.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/posix_types.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include/stdarg.h \
  include/uapi/linux/string.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/string.h \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/cpu/r3000.h) \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/bootinfo.h \
    $(wildcard include/config/swiotlb.h) \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/setup.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/setup.h \

arch/mips/ralink/cmdline.o: $(deps_arch/mips/ralink/cmdline.o)

$(deps_arch/mips/ralink/cmdline.o):
