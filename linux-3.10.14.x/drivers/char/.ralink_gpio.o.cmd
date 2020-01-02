cmd_drivers/char/ralink_gpio.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,drivers/char/.ralink_gpio.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff80001000 -DDATAOFFSET=0 -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(ralink_gpio)"  -D"KBUILD_MODNAME=KBUILD_STR(ralink_gpio)" -c -o drivers/char/ralink_gpio.o drivers/char/ralink_gpio.c

source_drivers/char/ralink_gpio.o := drivers/char/ralink_gpio.c

deps_drivers/char/ralink_gpio.o := \
    $(wildcard include/config/ralink/gpio/led.h) \
    $(wildcard include/config/devfs/fs.h) \
    $(wildcard include/config/ralink/mt7621.h) \
    $(wildcard include/config/ralink/mt7628.h) \
    $(wildcard include/config/fb/mediatek/ilitek.h) \
    $(wildcard include/config/fb/mediatek/truly.h) \
    $(wildcard include/config/ralink/rt2880.h) \
    $(wildcard include/config/ralink/rt3052.h) \
    $(wildcard include/config/ralink/rt2883.h) \
    $(wildcard include/config/ralink/mt7620.h) \
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
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/types.h \
    $(wildcard include/config/64bit/phys/addr.h) \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/types.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/posix_types.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/posix_types.h \
  include/generated/uapi/linux/version.h \
  include/linux/module.h \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/module/sig.h) \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/debug/set/module/ronx.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/const.h \
  include/linux/stat.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
  /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modversions.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/linkage.h \
  include/linux/bitops.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/bitops.h \
    $(wildcard include/config/cpu/mipsr2.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/barrier.h \
    $(wildcard include/config/cpu/has/sync.h) \
    $(wildcard include/config/cpu/cavium/octeon.h) \
    $(wildcard include/config/sgi/ip28.h) \
    $(wildcard include/config/cpu/has/wb.h) \
    $(wildcard include/config/weak/ordering.h) \
    $(wildcard include/config/weak/reordering/beyond/llsc.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/addrspace.h \
    $(wildcard include/config/cpu/r8000.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/spaces.h \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/kvm/guest.h) \
    $(wildcard include/config/dma/noncoherent.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cpu-features.h \
    $(wildcard include/config/sys/supports/micromips.h) \
    $(wildcard include/config/cpu/mipsr2/irq/vi.h) \
    $(wildcard include/config/cpu/mipsr2/irq/ei.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cpu.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cpu-info.h \
    $(wildcard include/config/mips/mt/smp.h) \
    $(wildcard include/config/mips/mt/smtc.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cache.h \
    $(wildcard include/config/mips/l1/cache/shift.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/kmalloc.h \
    $(wildcard include/config/dma/coherent.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/cpu-feature-overrides.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/war.h \
    $(wildcard include/config/cpu/r4000/workarounds.h) \
    $(wildcard include/config/cpu/r4400/workarounds.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink/war.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/ffz.h \
  include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  include/asm-generic/bitops/sched.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/arch_hweight.h \
  include/asm-generic/bitops/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/le.h \
  include/asm-generic/bitops/ext2-atomic.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/typecheck.h \
  include/linux/printk.h \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  include/linux/kern_levels.h \
  include/linux/dynamic_debug.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  include/uapi/linux/string.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/string.h \
    $(wildcard include/config/cpu/r3000.h) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/errno.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/errno.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/errno-base.h \
  include/uapi/linux/kernel.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/sysinfo.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/div64.h \
  include/asm-generic/div64.h \
  include/linux/seqlock.h \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/context/tracking.h) \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
    $(wildcard include/config/debug/stack/usage.h) \
  include/linux/bug.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/break.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/break.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/thread_info.h \
    $(wildcard include/config/page/size/4kb.h) \
    $(wildcard include/config/page/size/8kb.h) \
    $(wildcard include/config/page/size/16kb.h) \
    $(wildcard include/config/page/size/32kb.h) \
    $(wildcard include/config/page/size/64kb.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/processor.h \
    $(wildcard include/config/cavium/octeon/cvmseg/size.h) \
    $(wildcard include/config/mips/mt/fpaff.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/bitmap.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/cachectl.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mipsregs.h \
    $(wildcard include/config/cpu/vr41xx.h) \
    $(wildcard include/config/mips/huge/tlb/support.h) \
    $(wildcard include/config/cpu/micromips.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/hazards.h \
    $(wildcard include/config/cpu/mipsr1.h) \
    $(wildcard include/config/mips/alchemy.h) \
    $(wildcard include/config/cpu/bmips.h) \
    $(wildcard include/config/cpu/loongson2.h) \
    $(wildcard include/config/cpu/r10000.h) \
    $(wildcard include/config/cpu/r5500.h) \
    $(wildcard include/config/cpu/xlr.h) \
    $(wildcard include/config/cpu/sb1.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/prefetch.h \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/irqflags.h \
    $(wildcard include/config/irq/cpu.h) \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/spinlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/prove/rcu.h) \
  include/linux/rwlock_types.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/spinlock.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/uninline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  include/linux/atomic.h \
    $(wildcard include/config/arch/has/atomic/or.h) \
    $(wildcard include/config/generic/atomic64.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/atomic.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cmpxchg.h \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/atomic-long.h \
  include/asm-generic/atomic64.h \
  include/linux/math64.h \
  include/uapi/linux/time.h \
  include/linux/uidgid.h \
    $(wildcard include/config/uidgid/strict/type/checks.h) \
    $(wildcard include/config/user/ns.h) \
  include/linux/highuid.h \
  include/linux/kmod.h \
  include/linux/gfp.h \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/cma.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/isolation.h) \
    $(wildcard include/config/memcg.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/have/memblock/node/map.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/numa/balancing.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/wait.h \
  arch/mips/include/generated/asm/current.h \
  include/asm-generic/current.h \
  include/uapi/linux/wait.h \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/nodemask.h \
    $(wildcard include/config/movable/node.h) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  include/generated/bounds.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/page.h \
    $(wildcard include/config/cpu/mips32.h) \
  include/linux/pfn.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/io.h \
    $(wildcard include/config/pci.h) \
  include/asm-generic/iomap.h \
    $(wildcard include/config/has/ioport.h) \
    $(wildcard include/config/generic/iomap.h) \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/no/generic/pci/ioport/map.h) \
    $(wildcard include/config/generic/pci/iomap.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/pgtable-bits.h \
    $(wildcard include/config/cpu/tx39xx.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/ioremap.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/mangle-port.h \
    $(wildcard include/config/swap/io/space.h) \
  include/asm-generic/memory_model.h \
  include/asm-generic/getorder.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/have/bootmem/info/node.h) \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/mutex/spin/on/owner.h) \
    $(wildcard include/config/have/arch/mutex/cpu/relax.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/srcu.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/rcu/torture/test.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/rcu/trace.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/rcu/user/qs.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/tiny/preempt/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/rcu/nocb/cpu.h) \
  include/linux/completion.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/rcutree.h \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/jiffies.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/param.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  include/uapi/asm-generic/param.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/timex.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/sched/book.h) \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/smp.h \
    $(wildcard include/config/kexec.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/smp-ops.h \
    $(wildcard include/config/smp/up.h) \
    $(wildcard include/config/mips/cmp.h) \
  arch/mips/include/generated/asm/percpu.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/topology.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/topology.h \
  include/asm-generic/topology.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
  include/linux/sysctl.h \
    $(wildcard include/config/sysctl.h) \
  include/linux/rbtree.h \
  include/uapi/linux/sysctl.h \
  include/linux/elf.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/elf.h \
    $(wildcard include/config/mips32/n32.h) \
    $(wildcard include/config/mips32/o32.h) \
    $(wildcard include/config/mips32/compat.h) \
  include/uapi/linux/elf.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/support/openwrt.h) \
  include/linux/sysfs.h \
  include/linux/kobject_ns.h \
  include/linux/kref.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  include/linux/tracepoint.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/jump/label.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/module.h \
    $(wildcard include/config/cpu/mips32/r1.h) \
    $(wildcard include/config/cpu/mips32/r2.h) \
    $(wildcard include/config/cpu/mips64/r1.h) \
    $(wildcard include/config/cpu/mips64/r2.h) \
    $(wildcard include/config/cpu/r4300.h) \
    $(wildcard include/config/cpu/r4x00.h) \
    $(wildcard include/config/cpu/tx49xx.h) \
    $(wildcard include/config/cpu/r5000.h) \
    $(wildcard include/config/cpu/r5432.h) \
    $(wildcard include/config/cpu/r6000.h) \
    $(wildcard include/config/cpu/nevada.h) \
    $(wildcard include/config/cpu/rm7000.h) \
    $(wildcard include/config/cpu/loongson1.h) \
    $(wildcard include/config/cpu/xlp.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/uaccess.h \
    $(wildcard include/config/eva.h) \
  include/linux/config.h \
    $(wildcard include/config/h.h) \
  include/linux/autoconf.h \
    $(wildcard include/config/nf/conntrack/h323.h) \
    $(wildcard include/config/scsi/dma.h) \
    $(wildcard include/config/rfs/accel.h) \
    $(wildcard include/config/crc32.h) \
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/vlan/8021q.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/hdr/trans/tx/support.h) \
    $(wildcard include/config/ppp/sync/tty.h) \
    $(wildcard include/config/ssb/possible.h) \
    $(wildcard include/config/nf/nat/sip.h) \
    $(wildcard include/config/net/sch/fifo.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/second/if/eeprom/flash.h) \
    $(wildcard include/config/stp.h) \
    $(wildcard include/config/arch/flatmem/enable.h) \
    $(wildcard include/config/inet6/tunnel.h) \
    $(wildcard include/config/nf/conntrack/sip.h) \
    $(wildcard include/config/crypto/manager/disable/tests.h) \
    $(wildcard include/config/arch/want/ipc/parse/version.h) \
    $(wildcard include/config/generic/smp/idle/thread.h) \
    $(wildcard include/config/default/security/dac.h) \
    $(wildcard include/config/ip6/nf/mangle.h) \
    $(wildcard include/config/ipv6.h) \
    $(wildcard include/config/bql.h) \
    $(wildcard include/config/default/tcp/cong.h) \
    $(wildcard include/config/uevent/helper/path.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/net/cls/flow.h) \
    $(wildcard include/config/nf/nat/proto/gre.h) \
    $(wildcard include/config/ip6/nf/target/reject.h) \
    $(wildcard include/config/wan/at/p4.h) \
    $(wildcard include/config/vow/support.h) \
    $(wildcard include/config/sys/supports/smp.h) \
    $(wildcard include/config/wlan.h) \
    $(wildcard include/config/namespaces.h) \
    $(wildcard include/config/default/message/loglevel.h) \
    $(wildcard include/config/wsc/included.h) \
    $(wildcard include/config/ralink/ram/size.h) \
    $(wildcard include/config/sync/r4k.h) \
    $(wildcard include/config/ra/hw/nat/udp/ka.h) \
    $(wildcard include/config/legacy/ptys.h) \
    $(wildcard include/config/crypto/rng2.h) \
    $(wildcard include/config/netfilter/netlink/queue.h) \
    $(wildcard include/config/serial/8250.h) \
    $(wildcard include/config/usb/ehci/root/hub/tt.h) \
    $(wildcard include/config/mt7621/asic.h) \
    $(wildcard include/config/rd/lzma.h) \
    $(wildcard include/config/passpoint/r2.h) \
    $(wildcard include/config/usb.h) \
    $(wildcard include/config/irq/gic.h) \
    $(wildcard include/config/ethernet.h) \
    $(wildcard include/config/background/scan/support.h) \
    $(wildcard include/config/sched/omit/frame/pointer.h) \
    $(wildcard include/config/dql.h) \
    $(wildcard include/config/ip/nf/arptables.h) \
    $(wildcard include/config/bcma/possible.h) \
    $(wildcard include/config/vga/arb.h) \
    $(wildcard include/config/modules/use/elf/rel.h) \
    $(wildcard include/config/net/ipgre/demux.h) \
    $(wildcard include/config/rt/3052/esw.h) \
    $(wildcard include/config/nf/conntrack/proc/compat.h) \
    $(wildcard include/config/timerfd.h) \
    $(wildcard include/config/mtd/cfi/i2.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/mtd.h) \
    $(wildcard include/config/have/arch/jump/label.h) \
    $(wildcard include/config/decompress/lzma.h) \
    $(wildcard include/config/devtmpfs/mount.h) \
    $(wildcard include/config/dnotify.h) \
    $(wildcard include/config/ralink/gpio.h) \
    $(wildcard include/config/nls/codepage/437.h) \
    $(wildcard include/config/mtd/nand/ids.h) \
    $(wildcard include/config/net/cls/u32.h) \
    $(wildcard include/config/raeth/scatter/gather/rx/dma.h) \
    $(wildcard include/config/rtmp/flash/support.h) \
    $(wildcard include/config/serio.h) \
    $(wildcard include/config/blk/dev/initrd.h) \
    $(wildcard include/config/zlib/inflate.h) \
    $(wildcard include/config/ralink/rdm.h) \
    $(wildcard include/config/nf/nat/rtsp.h) \
    $(wildcard include/config/sys/supports/sched/smt.h) \
    $(wildcard include/config/stacktrace/support.h) \
    $(wildcard include/config/jffs2/fs.h) \
    $(wildcard include/config/board/scache.h) \
    $(wildcard include/config/mtd/cfi/util.h) \
    $(wildcard include/config/netfilter/xt/match/string.h) \
    $(wildcard include/config/hw/has/pci.h) \
    $(wildcard include/config/mips/mt.h) \
    $(wildcard include/config/usb/storage.h) \
    $(wildcard include/config/mips.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/have/ide.h) \
    $(wildcard include/config/init/env/arg/limit.h) \
    $(wildcard include/config/ip/nf/arp/mangle.h) \
    $(wildcard include/config/nf/conntrack/pptp.h) \
    $(wildcard include/config/rt2880/dram/128m.h) \
    $(wildcard include/config/textsearch/kmp.h) \
    $(wildcard include/config/smart/carrier/sense/support.h) \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/ra/hw/nat/tcp/dlta.h) \
    $(wildcard include/config/mips/cpu/scache.h) \
    $(wildcard include/config/ge1/trgmii/force/1200.h) \
    $(wildcard include/config/cpu/r4k/fpu.h) \
    $(wildcard include/config/ppe/mcast.h) \
    $(wildcard include/config/wext/core.h) \
    $(wildcard include/config/nls.h) \
    $(wildcard include/config/nf/conntrack/timeout.h) \
    $(wildcard include/config/rtdev.h) \
    $(wildcard include/config/pppoe.h) \
    $(wildcard include/config/first/if/mt7615e.h) \
    $(wildcard include/config/irq/work.h) \
    $(wildcard include/config/usb/ehci/pci.h) \
    $(wildcard include/config/usb/common.h) \
    $(wildcard include/config/ip6/nf/iptables.h) \
    $(wildcard include/config/hdr/trans/rx/support.h) \
    $(wildcard include/config/vga/arb/max/gpus.h) \
    $(wildcard include/config/sys/supports/little/endian.h) \
    $(wildcard include/config/nls/iso8859/1.h) \
    $(wildcard include/config/crypto/workqueue.h) \
    $(wildcard include/config/net/dma.h) \
    $(wildcard include/config/usb/ehci/hcd.h) \
    $(wildcard include/config/textsearch/bm.h) \
    $(wildcard include/config/nf/conntrack/procfs.h) \
    $(wildcard include/config/nat/linux.h) \
    $(wildcard include/config/ppp/mppe.h) \
    $(wildcard include/config/netdevices.h) \
    $(wildcard include/config/initramfs/root/gid.h) \
    $(wildcard include/config/eventfd.h) \
    $(wildcard include/config/ipv6/sit.h) \
    $(wildcard include/config/xfrm.h) \
    $(wildcard include/config/defconfig/list.h) \
    $(wildcard include/config/list.h) \
    $(wildcard include/config/async/tx/dma.h) \
    $(wildcard include/config/ra/hw/nat/fin/dlta.h) \
    $(wildcard include/config/serial/8250/console.h) \
    $(wildcard include/config/ip/nf/target/masquerade.h) \
    $(wildcard include/config/rcu/fanout/leaf.h) \
    $(wildcard include/config/raeth/tso.h) \
    $(wildcard include/config/ra/hw/nat/tbl/4k.h) \
    $(wildcard include/config/band/steering.h) \
    $(wildcard include/config/nf/defrag/ipv4.h) \
    $(wildcard include/config/netfilter/advanced.h) \
    $(wildcard include/config/mtd/cfi.h) \
    $(wildcard include/config/txbf/support.h) \
    $(wildcard include/config/jffs2/fs/debug.h) \
    $(wildcard include/config/wds/support.h) \
    $(wildcard include/config/have/dynamic/ftrace.h) \
    $(wildcard include/config/magic/sysrq.h) \
    $(wildcard include/config/ipv6/sit/6rd.h) \
    $(wildcard include/config/cevt/r4k.h) \
    $(wildcard include/config/netfilter/xt/match/mark.h) \
    $(wildcard include/config/ip/nf/mangle.h) \
    $(wildcard include/config/rcu/stall/common.h) \
    $(wildcard include/config/inet6/xfrm/mode/tunnel.h) \
    $(wildcard include/config/ip/nf/filter.h) \
    $(wildcard include/config/fat/fs.h) \
    $(wildcard include/config/textsearch/fsm.h) \
    $(wildcard include/config/mcast/rate/specific.h) \
    $(wildcard include/config/sys/has/cpu/mips32/r1.h) \
    $(wildcard include/config/inet/tunnel.h) \
    $(wildcard include/config/generic/clockevents.h) \
    $(wildcard include/config/esw/double/vlan/tag.h) \
    $(wildcard include/config/mtd/cfi/staa.h) \
    $(wildcard include/config/stop/machine.h) \
    $(wildcard include/config/cls/u32/mark.h) \
    $(wildcard include/config/module/force/load.h) \
    $(wildcard include/config/mbss/support.h) \
    $(wildcard include/config/nls/ascii.h) \
    $(wildcard include/config/mtd/physmap.h) \
    $(wildcard include/config/hardware/watchpoints.h) \
    $(wildcard include/config/have/kvm.h) \
    $(wildcard include/config/have/generic/hardirqs.h) \
    $(wildcard include/config/binfmt/elf.h) \
    $(wildcard include/config/scsi/proc/fs.h) \
    $(wildcard include/config/hotplug.h) \
    $(wildcard include/config/ip/pimsm/v1.h) \
    $(wildcard include/config/usb/mon.h) \
    $(wildcard include/config/apcli/support.h) \
    $(wildcard include/config/netfilter/xt/mark.h) \
    $(wildcard include/config/netfilter/xtables.h) \
    $(wildcard include/config/mt/wifi/path.h) \
    $(wildcard include/config/slabinfo.h) \
    $(wildcard include/config/pppopptp.h) \
    $(wildcard include/config/jffs2/fs/writebuffer.h) \
    $(wildcard include/config/first/if/ipailna.h) \
    $(wildcard include/config/crc16.h) \
    $(wildcard include/config/generic/calibrate/delay.h) \
    $(wildcard include/config/hz/periodic.h) \
    $(wildcard include/config/net/cls.h) \
    $(wildcard include/config/wifi/mode/ap.h) \
    $(wildcard include/config/anon/inodes.h) \
    $(wildcard include/config/nls/codepage/936.h) \
    $(wildcard include/config/greenap/support.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/multi/inf/support.h) \
    $(wildcard include/config/cpu/supports/32bit/kernel.h) \
    $(wildcard include/config/serial/core/console.h) \
    $(wildcard include/config/sys/has/cpu/mips32/r2.h) \
    $(wildcard include/config/usb/mt7621/xhci/platform.h) \
    $(wildcard include/config/ra/network/tasklet/bh.h) \
    $(wildcard include/config/dot11k/rrm/support.h) \
    $(wildcard include/config/dmadevices.h) \
    $(wildcard include/config/nf/conntrack/events.h) \
    $(wildcard include/config/ipv6/ndisc/nodetype.h) \
    $(wildcard include/config/mt/dfs/support.h) \
    $(wildcard include/config/wifi/driver.h) \
    $(wildcard include/config/pcie/port0.h) \
    $(wildcard include/config/crypto/pcomp2.h) \
    $(wildcard include/config/have/debug/kmemleak.h) \
    $(wildcard include/config/nf/conntrack/ftp.h) \
    $(wildcard include/config/raeth.h) \
    $(wildcard include/config/unix.h) \
    $(wildcard include/config/first/if/eeprom/flash.h) \
    $(wildcard include/config/crypto/hash2.h) \
    $(wildcard include/config/default/hostname.h) \
    $(wildcard include/config/xps.h) \
    $(wildcard include/config/nf/conntrack/ipv6.h) \
    $(wildcard include/config/crypto/algapi.h) \
    $(wildcard include/config/bridge.h) \
    $(wildcard include/config/ra/hw/nat.h) \
    $(wildcard include/config/ra/hw/nat/udp/dlta.h) \
    $(wildcard include/config/net/ip/tunnel.h) \
    $(wildcard include/config/mtd/cfi/i1.h) \
    $(wildcard include/config/nf/nat.h) \
    $(wildcard include/config/arch/has/atomic64/dec/if/positive.h) \
    $(wildcard include/config/crypto/hash.h) \
    $(wildcard include/config/efi/partition.h) \
    $(wildcard include/config/mtd/any/ralink.h) \
    $(wildcard include/config/log/buf/shift.h) \
    $(wildcard include/config/extra/firmware.h) \
    $(wildcard include/config/ipv6/mroute.h) \
    $(wildcard include/config/virt/to/bus.h) \
    $(wildcard include/config/vfat/fs.h) \
    $(wildcard include/config/pid/ns.h) \
    $(wildcard include/config/ra/hw/nat/wifi/new/arch.h) \
    $(wildcard include/config/crc32/sliceby8.h) \
    $(wildcard include/config/cpu/rmap.h) \
    $(wildcard include/config/trad/signals.h) \
    $(wildcard include/config/mtd/nand/ecc.h) \
    $(wildcard include/config/ip6/nf/match/rt.h) \
    $(wildcard include/config/serial/8250/runtime/uarts.h) \
    $(wildcard include/config/ra/nat/hw.h) \
    $(wildcard include/config/fs/mbcache.h) \
    $(wildcard include/config/initramfs/root/uid.h) \
    $(wildcard include/config/wsc/v2/support.h) \
    $(wildcard include/config/raeth/qdma.h) \
    $(wildcard include/config/have/function/tracer.h) \
    $(wildcard include/config/nf/nat/tftp.h) \
    $(wildcard include/config/netfilter/xt/match/multiport.h) \
    $(wildcard include/config/con/wps/support.h) \
    $(wildcard include/config/crypto/manager2.h) \
    $(wildcard include/config/serial/8250/deprecated/options.h) \
    $(wildcard include/config/raeth/gmac2.h) \
    $(wildcard include/config/netfilter/xt/target/log.h) \
    $(wildcard include/config/jffs2/zlib.h) \
    $(wildcard include/config/binfmt/script.h) \
    $(wildcard include/config/tick/cpu/accounting.h) \
    $(wildcard include/config/vm/event/counters.h) \
    $(wildcard include/config/crypto/ecb.h) \
    $(wildcard include/config/rt/second/card/eeprom.h) \
    $(wildcard include/config/base/full.h) \
    $(wildcard include/config/zlib/deflate.h) \
    $(wildcard include/config/arch/discard/memblock.h) \
    $(wildcard include/config/fw/loader.h) \
    $(wildcard include/config/pci/quirks.h) \
    $(wildcard include/config/mii.h) \
    $(wildcard include/config/ra/hw/nat/wifi.h) \
    $(wildcard include/config/signalfd.h) \
    $(wildcard include/config/net/core.h) \
    $(wildcard include/config/ext4/fs.h) \
    $(wildcard include/config/uidgid/converted.h) \
    $(wildcard include/config/crypto/sha1.h) \
    $(wildcard include/config/csrc/r4k.h) \
    $(wildcard include/config/has/iomem.h) \
    $(wildcard include/config/gpio/devres.h) \
    $(wildcard include/config/generic/irq/probe.h) \
    $(wildcard include/config/proc/kcore.h) \
    $(wildcard include/config/mtd/map/bank/width/1.h) \
    $(wildcard include/config/net/sch/prio.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/partition/advanced.h) \
    $(wildcard include/config/have/net/dsa.h) \
    $(wildcard include/config/cpu/supports/highmem.h) \
    $(wildcard include/config/net.h) \
    $(wildcard include/config/netfilter/xt/target/tcpmss.h) \
    $(wildcard include/config/second/if/mt7615e.h) \
    $(wildcard include/config/eth/wifi/oom/debug.h) \
    $(wildcard include/config/mtd/gen/probe.h) \
    $(wildcard include/config/igmp/snoop/support.h) \
    $(wildcard include/config/packet.h) \
    $(wildcard include/config/arch/binfmt/elf/randomize/pie.h) \
    $(wildcard include/config/nf/conntrack/timestamp.h) \
    $(wildcard include/config/netfilter/xt/match/iprange.h) \
    $(wildcard include/config/usb/xhci/platform.h) \
    $(wildcard include/config/nf/conntrack/tftp.h) \
    $(wildcard include/config/inet.h) \
    $(wildcard include/config/mt/mac.h) \
    $(wildcard include/config/pci/domains.h) \
    $(wildcard include/config/ra/hw/nat/binding/threshold.h) \
    $(wildcard include/config/net/sch/sfq.h) \
    $(wildcard include/config/rtc/lib.h) \
    $(wildcard include/config/have/kprobes.h) \
    $(wildcard include/config/crypto/aes.h) \
    $(wildcard include/config/imq.h) \
    $(wildcard include/config/raeth/lro.h) \
    $(wildcard include/config/nf/conntrack/mark.h) \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/serio/serport.h) \
    $(wildcard include/config/nls/codepage/950.h) \
    $(wildcard include/config/clone/backwards.h) \
    $(wildcard include/config/blk/dev/ram/count.h) \
    $(wildcard include/config/ip/mroute.h) \
    $(wildcard include/config/nf/nat/needed.h) \
    $(wildcard include/config/raeth/hw/vlan/tx.h) \
    $(wildcard include/config/lockdep/support.h) \
    $(wildcard include/config/usb/arch/has/ehci.h) \
    $(wildcard include/config/mtd/blkdevs.h) \
    $(wildcard include/config/hnat/v2.h) \
    $(wildcard include/config/sysctl/syscall.h) \
    $(wildcard include/config/nf/conntrack/rtsp.h) \
    $(wildcard include/config/ip6/nf/filter.h) \
    $(wildcard include/config/uapsd.h) \
    $(wildcard include/config/need/dma/map/state.h) \
    $(wildcard include/config/ra/hw/nat/ntu/ka.h) \
    $(wildcard include/config/preempt/none.h) \
    $(wildcard include/config/tty.h) \
    $(wildcard include/config/netfilter/xt/match/time.h) \
    $(wildcard include/config/cpu/generic/dump/tlb.h) \
    $(wildcard include/config/netfilter/xt/match/mac.h) \
    $(wildcard include/config/l2tp.h) \
    $(wildcard include/config/generic/io.h) \
    $(wildcard include/config/have/ftrace/mcount/record.h) \
    $(wildcard include/config/ra/hw/nat/tcp/ka.h) \
    $(wildcard include/config/ra/hw/nat/unb/dlta.h) \
    $(wildcard include/config/iosched/noop.h) \
    $(wildcard include/config/cpu/r4k/cache/tlb.h) \
    $(wildcard include/config/debug/kernel.h) \
    $(wildcard include/config/localversion.h) \
    $(wildcard include/config/crypto.h) \
    $(wildcard include/config/default/mmap/min/addr.h) \
    $(wildcard include/config/ip/nf/iptables.h) \
    $(wildcard include/config/pdma/new.h) \
    $(wildcard include/config/usb/xhci/hcd.h) \
    $(wildcard include/config/have/dma/api/debug.h) \
    $(wildcard include/config/initramfs/compression/lzma.h) \
    $(wildcard include/config/ip/pimsm/v2.h) \
    $(wildcard include/config/usb/arch/has/hcd.h) \
    $(wildcard include/config/generic/irq/show.h) \
    $(wildcard include/config/dot11/n/support.h) \
    $(wildcard include/config/mtd/physmap/bankwidth.h) \
    $(wildcard include/config/panic/on/oops/value.h) \
    $(wildcard include/config/scsi/mod.h) \
    $(wildcard include/config/wifi/basic/func.h) \
    $(wildcard include/config/crypto/crc32c.h) \
    $(wildcard include/config/serial/core.h) \
    $(wildcard include/config/fuse/fs.h) \
    $(wildcard include/config/buildtime/extable/sort.h) \
    $(wildcard include/config/embedded.h) \
    $(wildcard include/config/have/kretprobes.h) \
    $(wildcard include/config/nf/defrag/ipv6.h) \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/lan/wan/support.h) \
    $(wildcard include/config/ra/hw/nat/lan/vlanid.h) \
    $(wildcard include/config/scsi.h) \
    $(wildcard include/config/nf/nat/pptp.h) \
    $(wildcard include/config/jbd2.h) \
    $(wildcard include/config/phylib.h) \
    $(wildcard include/config/ipv6/tunnel.h) \
    $(wildcard include/config/jffs2/rtime.h) \
    $(wildcard include/config/misc/filesystems.h) \
    $(wildcard include/config/ip/nf/arpfilter.h) \
    $(wildcard include/config/pppol2tp.h) \
    $(wildcard include/config/ksm.h) \
    $(wildcard include/config/nf/nat/h323.h) \
    $(wildcard include/config/netfilter/xt/target/imq.h) \
    $(wildcard include/config/pcie/port1.h) \
    $(wildcard include/config/rcu/cpu/stall/timeout.h) \
    $(wildcard include/config/nf/conntrack/ipsec.h) \
    $(wildcard include/config/ip6/nf/match/ipv6header.h) \
    $(wildcard include/config/rt/second/card.h) \
    $(wildcard include/config/tpc/support.h) \
    $(wildcard include/config/inet6/xfrm/mode/transport.h) \
    $(wildcard include/config/crypto/arc4.h) \
    $(wildcard include/config/boot/elf32.h) \
    $(wildcard include/config/slhc.h) \
    $(wildcard include/config/wifi/pkt/fwd.h) \
    $(wildcard include/config/crypto/manager.h) \
    $(wildcard include/config/net/sch/htb.h) \
    $(wildcard include/config/ppp/bsdcomp.h) \
    $(wildcard include/config/mtd/nand.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/netfilter/xt/target/mark.h) \
    $(wildcard include/config/mu/ra/support.h) \
    $(wildcard include/config/led/control/support.h) \
    $(wildcard include/config/net/cls/fw.h) \
    $(wildcard include/config/expert.h) \
    $(wildcard include/config/wireless.h) \
    $(wildcard include/config/wext/proc.h) \
    $(wildcard include/config/hz/250.h) \
    $(wildcard include/config/net/sch/tbf.h) \
    $(wildcard include/config/perf/use/vmalloc.h) \
    $(wildcard include/config/fat/default/iocharset.h) \
    $(wildcard include/config/mac/to/gigaphy/mode/addr2.h) \
    $(wildcard include/config/frame/warn.h) \
    $(wildcard include/config/generic/hweight.h) \
    $(wildcard include/config/initramfs/source.h) \
    $(wildcard include/config/sys/supports/highmem.h) \
    $(wildcard include/config/nf/nat/ipsec.h) \
    $(wildcard include/config/serial/8250/nr/uarts.h) \
    $(wildcard include/config/dot11w/pmf/support.h) \
    $(wildcard include/config/default/iosched.h) \
    $(wildcard include/config/nlattr.h) \
    $(wildcard include/config/tcp/cong/cubic.h) \
    $(wildcard include/config/ra/hw/nat/unb/mnp.h) \
    $(wildcard include/config/usb/default/persist.h) \
    $(wildcard include/config/have/function/trace/mcount/test.h) \
    $(wildcard include/config/netfilter/xt/match/limit.h) \
    $(wildcard include/config/mtd/physmap/len.h) \
    $(wildcard include/config/raeth/checksum/offload.h) \
    $(wildcard include/config/mtd/complex/mappings.h) \
    $(wildcard include/config/dot11r/ft/support.h) \
    $(wildcard include/config/msdos/partition.h) \
    $(wildcard include/config/icap/support.h) \
    $(wildcard include/config/g/band/256qam/support.h) \
    $(wildcard include/config/have/oprofile.h) \
    $(wildcard include/config/have/generic/dma/coherent.h) \
    $(wildcard include/config/arch/have/custom/gpio/h.h) \
    $(wildcard include/config/mumimo/support.h) \
    $(wildcard include/config/red/support.h) \
    $(wildcard include/config/have/arch/kgdb.h) \
    $(wildcard include/config/sys/supports/multithreading.h) \
    $(wildcard include/config/usb/arch/has/ohci.h) \
    $(wildcard include/config/nf/conntrack/ipv4.h) \
    $(wildcard include/config/zone/dma/flag.h) \
    $(wildcard include/config/extend/nvram.h) \
    $(wildcard include/config/rps.h) \
    $(wildcard include/config/ip/nf/target/trigger.h) \
    $(wildcard include/config/legacy/pty/count.h) \
    $(wildcard include/config/ra/hw/nat/hash1.h) \
    $(wildcard include/config/mtd/map/bank/width/2.h) \
    $(wildcard include/config/ra/hw/nat/ipv6.h) \
    $(wildcard include/config/ip/multicast.h) \
    $(wildcard include/config/generic/cmos/update.h) \
    $(wildcard include/config/default/security.h) \
    $(wildcard include/config/rt/first/if/rf/offset.h) \
    $(wildcard include/config/wireless/ext.h) \
    $(wildcard include/config/have/dma/attrs.h) \
    $(wildcard include/config/default/noop.h) \
    $(wildcard include/config/nf/nat/ipv4.h) \
    $(wildcard include/config/have/function/graph/tracer.h) \
    $(wildcard include/config/serial/8250/dma.h) \
    $(wildcard include/config/crypto/blkcipher2.h) \
    $(wildcard include/config/mips/perf/shared/tc/counters.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/mtd/block.h) \
    $(wildcard include/config/wext/priv.h) \
    $(wildcard include/config/scsi/lowlevel.h) \
    $(wildcard include/config/sys/supports/arbit/hz.h) \
    $(wildcard include/config/kernel/arp/spoofing/protect.h) \
    $(wildcard include/config/irq/forced/threading.h) \
    $(wildcard include/config/ate/support.h) \
    $(wildcard include/config/second/if/ipailna.h) \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/mtd/physmap/compat.h) \
    $(wildcard include/config/kernel/nvram.h) \
    $(wildcard include/config/ikconfig.h) \
    $(wildcard include/config/bridge/igmp/snooping.h) \
    $(wildcard include/config/mtd/block2mtd.h) \
    $(wildcard include/config/have/c/recordmcount.h) \
    $(wildcard include/config/rt/first/card.h) \
    $(wildcard include/config/ge2/internal/gphy.h) \
    $(wildcard include/config/net/ns.h) \
    $(wildcard include/config/have/perf/events.h) \
    $(wildcard include/config/ppp/async.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/rt/second/if/rf/offset.h) \
    $(wildcard include/config/usb/ehci/tt/newsched.h) \
    $(wildcard include/config/fat/default/codepage.h) \
    $(wildcard include/config/blk/dev.h) \
    $(wildcard include/config/rt/first/card/eeprom.h) \
    $(wildcard include/config/tracing/support.h) \
    $(wildcard include/config/unix98/ptys.h) \
    $(wildcard include/config/rt/dma.h) \
    $(wildcard include/config/net/sched.h) \
    $(wildcard include/config/ppp.h) \
    $(wildcard include/config/nf/nat/ftp.h) \
    $(wildcard include/config/textsearch.h) \
    $(wildcard include/config/rt2880/rootfs/in/ram.h) \
    $(wildcard include/config/mtd/jedecprobe.h) \
    $(wildcard include/config/usb/support.h) \
    $(wildcard include/config/inet/ipsec/saref.h) \
    $(wildcard include/config/mtd/physmap/buswidth.h) \
    $(wildcard include/config/inet/lro.h) \
    $(wildcard include/config/cpu/supports/hr/sched/clock.h) \
    $(wildcard include/config/ra/hw/nat/ntu/dlta.h) \
    $(wildcard include/config/blk/dev/ram.h) \
    $(wildcard include/config/netfilter/xt/match/state.h) \
    $(wildcard include/config/mt/wifi.h) \
    $(wildcard include/config/inet6/xfrm/mode/beet.h) \
    $(wildcard include/config/generic/clockevents/build.h) \
    $(wildcard include/config/mtd/cfi/amdstd.h) \
    $(wildcard include/config/sys/supports/32bit/kernel.h) \
    $(wildcard include/config/sys/supports/mips/cmp.h) \
    $(wildcard include/config/ra/hw/nat/wan/vlanid.h) \
    $(wildcard include/config/mtd/physmap/start.h) \
    $(wildcard include/config/llc.h) \
    $(wildcard include/config/cross/compile.h) \
    $(wildcard include/config/serial/8250/pci.h) \
    $(wildcard include/config/ge/rgmii/internal/p4/an.h) \
    $(wildcard include/config/nls/utf8.h) \
    $(wildcard include/config/have/mod/arch/specific.h) \
    $(wildcard include/config/mac/repeater/support.h) \
    $(wildcard include/config/imq/num/devs.h) \
    $(wildcard include/config/crc/ccitt.h) \
    $(wildcard include/config/blk/dev/sd.h) \
    $(wildcard include/config/netfilter/netlink.h) \
    $(wildcard include/config/usb/arch/has/xhci.h) \
    $(wildcard include/config/imq/behavior/ba.h) \
    $(wildcard include/config/rcu/fanout.h) \
    $(wildcard include/config/bitreverse.h) \
    $(wildcard include/config/devport.h) \
    $(wildcard include/config/rt/dma/hsdma.h) \
    $(wildcard include/config/blk/dev/ram/size.h) \
    $(wildcard include/config/third/if/none.h) \
    $(wildcard include/config/crypto/blkcipher.h) \
    $(wildcard include/config/nf/conntrack.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/ip/nf/target/reject.h) \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/dot11/vht/ac.h) \
    $(wildcard include/config/mtd/map/bank/width/4.h) \
    $(wildcard include/config/cpu/little/endian.h) \
    $(wildcard include/config/nls/default.h) \
    $(wildcard include/config/uts/ns.h) \
    $(wildcard include/config/nf/ct/proto/gre.h) \
    $(wildcard include/config/nf/ct/netlink.h) \
    $(wildcard include/config/chip/mt7615e.h) \
    $(wildcard include/config/dma/engine.h) \
    $(wildcard include/config/crypto/aead2.h) \
    $(wildcard include/config/apcli/cert/support.h) \
    $(wildcard include/config/crypto/algapi2.h) \
    $(wildcard include/config/wext/spy.h) \
    $(wildcard include/config/mt/ap/support.h) \
    $(wildcard include/config/have/memblock.h) \
    $(wildcard include/config/proc/sysctl.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/pcie/port2.h) \
  include/linux/interrupt.h \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  include/linux/vtime.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
    $(wildcard include/config/virt/cpu/accounting/native.h) \
    $(wildcard include/config/virt/cpu/accounting/gen.h) \
    $(wildcard include/config/irq/time/accounting.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/hardirq.h \
  include/asm-generic/hardirq.h \
  include/linux/irq_cpustat.h \
  include/linux/irq.h \
    $(wildcard include/config/generic/pending/irq.h) \
    $(wildcard include/config/hardirqs/sw/resend.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/irq.h \
    $(wildcard include/config/i8259.h) \
    $(wildcard include/config/mips/mt/smtc/irqaff.h) \
    $(wildcard include/config/mips/mt/smtc/im/backstop.h) \
  include/linux/irqdomain.h \
    $(wildcard include/config/irq/domain.h) \
    $(wildcard include/config/of/irq.h) \
  include/linux/radix-tree.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mipsmtregs.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/irq.h \
    $(wildcard include/config/irq/cpu/rm7k.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/ptrace.h \
    $(wildcard include/config/cpu/has/smartmips.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/isadep.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/ptrace.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/irq_regs.h \
  include/linux/irqdesc.h \
    $(wildcard include/config/irq/preflow/fasteoi.h) \
    $(wildcard include/config/sparse/irq.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/hw_irq.h \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
  include/linux/timerqueue.h \
  include/linux/fs.h \
    $(wildcard include/config/splice/net/support.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/path.h \
  include/linux/pid.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/fiemap.h \
  include/linux/shrinker.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/blk_types.h \
    $(wildcard include/config/blk/cgroup.h) \
    $(wildcard include/config/blk/dev/integrity.h) \
  include/uapi/linux/fs.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/limits.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/ioctl.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  include/linux/percpu_counter.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/fcntl.h \
  include/uapi/linux/fcntl.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/fcntl.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/fcntl.h \
  include/linux/err.h \
  include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/no/hz/common.h) \
    $(wildcard include/config/lockup/detector.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/fanotify.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/cgroup/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/uprobes.h) \
    $(wildcard include/config/bcache.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/no/hz/full.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/mm/owner.h) \
  include/uapi/linux/sched.h \
  include/linux/mm_types.h \
    $(wildcard include/config/have/cmpxchg/double.h) \
    $(wildcard include/config/have/aligned/struct/page.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/mmu/notifier.h) \
    $(wildcard include/config/transparent/hugepage.h) \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/mips/include/generated/uapi/asm/auxvec.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/auxvec.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/guard.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  include/linux/uprobes.h \
    $(wildcard include/config/arch/supports/uprobes.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mmu.h \
  arch/mips/include/generated/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/asm-generic/cputime_jiffies.h \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/uapi/linux/ipc.h \
  arch/mips/include/generated/uapi/asm/ipcbuf.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/ipcbuf.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sembuf.h \
  include/linux/signal.h \
    $(wildcard include/config/old/sigaction.h) \
  include/uapi/linux/signal.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/signal.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/signal.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/signal-defs.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/sigcontext.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sigcontext.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/siginfo.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/proportions.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp/filter.h) \
  include/uapi/linux/seccomp.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/seccomp.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/unistd.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/unistd.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/unistd.h \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/latencytop.h \
  include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
  include/linux/key.h \
  include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  drivers/char/ralink_gpio.h \
    $(wildcard include/config/ralink/rt3883.h) \
    $(wildcard include/config/ralink/rt3352.h) \
    $(wildcard include/config/ralink/rt6855.h) \
    $(wildcard include/config/ralink/rt5350.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/rt2880/rt_mmap.h \
    $(wildcard include/config/ralink/rt2880/shuttle.h) \
    $(wildcard include/config/ralink/rt2880/mp.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/rt2880/surfboardint.h \

drivers/char/ralink_gpio.o: $(deps_drivers/char/ralink_gpio.o)

$(deps_drivers/char/ralink_gpio.o):
