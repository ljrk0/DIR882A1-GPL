cmd_kernel/sys_ni.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,kernel/.sys_ni.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff00000000+0x81001000 -DDATAOFFSET=0 -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(sys_ni)"  -D"KBUILD_MODNAME=KBUILD_STR(sys_ni)" -c -o kernel/sys_ni.o kernel/sys_ni.c

source_kernel/sys_ni.o := kernel/sys_ni.c

deps_kernel/sys_ni.o := \
  include/linux/linkage.h \
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
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/linkage.h \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/errno.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/errno.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/errno-base.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/unistd.h \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/mips32/o32.h) \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/unistd.h \
  /home/xieshijing/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \

kernel/sys_ni.o: $(deps_kernel/sys_ni.o)

$(deps_kernel/sys_ni.o):
