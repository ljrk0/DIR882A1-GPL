cmd_arch/mips/kernel/r4k_fpu.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,arch/mips/kernel/.r4k_fpu.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff00000000+0x81001000 -DDATAOFFSET=0  -D__ASSEMBLY__  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic          -c -o arch/mips/kernel/r4k_fpu.o arch/mips/kernel/r4k_fpu.S

source_arch/mips/kernel/r4k_fpu.o := arch/mips/kernel/r4k_fpu.S

deps_arch/mips/kernel/r4k_fpu.o := \
    $(wildcard include/config/64bit.h) \
    $(wildcard include/config/mips32/r2.h) \
    $(wildcard include/config/mips32/compat.h) \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
    $(wildcard include/config/eva.h) \
    $(wildcard include/config/sgi/ip28.h) \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/errno.h \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/errno.h \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/errno-base.h \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/fpregdef.h \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mipsregs.h \
    $(wildcard include/config/cpu/vr41xx.h) \
    $(wildcard include/config/page/size/4kb.h) \
    $(wildcard include/config/page/size/8kb.h) \
    $(wildcard include/config/page/size/16kb.h) \
    $(wildcard include/config/page/size/32kb.h) \
    $(wildcard include/config/page/size/64kb.h) \
    $(wildcard include/config/mips/huge/tlb/support.h) \
    $(wildcard include/config/mips/mt/smtc.h) \
    $(wildcard include/config/cpu/micromips.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/linkage.h \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/hazards.h \
    $(wildcard include/config/cpu/mipsr2.h) \
    $(wildcard include/config/cpu/cavium/octeon.h) \
    $(wildcard include/config/cpu/mipsr1.h) \
    $(wildcard include/config/mips/alchemy.h) \
    $(wildcard include/config/cpu/bmips.h) \
    $(wildcard include/config/cpu/loongson2.h) \
    $(wildcard include/config/cpu/r10000.h) \
    $(wildcard include/config/cpu/r5500.h) \
    $(wildcard include/config/cpu/xlr.h) \
    $(wildcard include/config/cpu/sb1.h) \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/war.h \
    $(wildcard include/config/cpu/r4000/workarounds.h) \
    $(wildcard include/config/cpu/r4400/workarounds.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink/war.h \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /home/zhangtianchuan/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/regdef.h \

arch/mips/kernel/r4k_fpu.o: $(deps_arch/mips/kernel/r4k_fpu.o)

$(deps_arch/mips/kernel/r4k_fpu.o):
