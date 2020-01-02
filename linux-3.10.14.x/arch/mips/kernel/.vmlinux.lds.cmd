cmd_arch/mips/kernel/vmlinux.lds := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -E -Wp,-MD,arch/mips/kernel/.vmlinux.lds.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff80001000 -DDATAOFFSET=0    -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024  -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO  -P -C -Umips -D__ASSEMBLY__ -DLINKER_SCRIPT -o arch/mips/kernel/vmlinux.lds arch/mips/kernel/vmlinux.lds.S

source_arch/mips/kernel/vmlinux.lds := arch/mips/kernel/vmlinux.lds.S

deps_arch/mips/kernel/vmlinux.lds := \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/cpu/little/endian.h) \
    $(wildcard include/config/boot/elf64.h) \
    $(wildcard include/config/ralink/mt7628.h) \
    $(wildcard include/config/ralink/mt7620.h) \
    $(wildcard include/config/mapped/kernel.h) \
    $(wildcard include/config/mips/l1/cache/shift.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/thread_info.h \
    $(wildcard include/config/page/size/4kb.h) \
    $(wildcard include/config/64bit.h) \
    $(wildcard include/config/page/size/8kb.h) \
    $(wildcard include/config/page/size/16kb.h) \
    $(wildcard include/config/page/size/32kb.h) \
    $(wildcard include/config/page/size/64kb.h) \
  include/asm-generic/vmlinux.lds.h \
    $(wildcard include/config/hotplug.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/ftrace/syscalls.h) \
    $(wildcard include/config/clksrc/of.h) \
    $(wildcard include/config/irqchip.h) \
    $(wildcard include/config/common/clk.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/pm/trace.h) \
    $(wildcard include/config/blk/dev/initrd.h) \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \

arch/mips/kernel/vmlinux.lds: $(deps_arch/mips/kernel/vmlinux.lds)

$(deps_arch/mips/kernel/vmlinux.lds):
