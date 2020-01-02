cmd_arch/mips/kernel/cevt-r4k.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,arch/mips/kernel/.cevt-r4k.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff00000000+0x81001000 -DDATAOFFSET=0 -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -Werror    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(cevt_r4k)"  -D"KBUILD_MODNAME=KBUILD_STR(cevt_r4k)" -c -o arch/mips/kernel/cevt-r4k.o arch/mips/kernel/cevt-r4k.c

source_arch/mips/kernel/cevt-r4k.o := arch/mips/kernel/cevt-r4k.c

deps_arch/mips/kernel/cevt-r4k.o := \
    $(wildcard include/config/mips/mt/smtc.h) \
    $(wildcard include/config/ralink/systick.h) \
    $(wildcard include/config/ralink/mt7621.h) \
    $(wildcard include/config/cevt/gic.h) \
    $(wildcard include/config/irq/gic.h) \
    $(wildcard include/config/kvm/guest.h) \
  include/linux/clockchips.h \
    $(wildcard include/config/generic/clockevents/build.h) \
    $(wildcard include/config/generic/clockevents/broadcast.h) \
    $(wildcard include/config/arch/has/tick/broadcast.h) \
    $(wildcard include/config/tick/oneshot.h) \
    $(wildcard include/config/generic/clockevents.h) \
  include/linux/clocksource.h \
    $(wildcard include/config/arch/clocksource/data.h) \
    $(wildcard include/config/clocksource/watchdog.h) \
    $(wildcard include/config/clksrc/of.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/uapi/linux/types.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/types.h \
    $(wildcard include/config/64bit/phys/addr.h) \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/types.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
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
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/posix_types.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/posix_types.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/cache.h \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/linkage.h \
  include/linux/bitops.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/bitops.h \
    $(wildcard include/config/cpu/mipsr2.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/barrier.h \
    $(wildcard include/config/cpu/has/sync.h) \
    $(wildcard include/config/cpu/cavium/octeon.h) \
    $(wildcard include/config/sgi/ip28.h) \
    $(wildcard include/config/cpu/has/wb.h) \
    $(wildcard include/config/weak/ordering.h) \
    $(wildcard include/config/weak/reordering/beyond/llsc.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/addrspace.h \
    $(wildcard include/config/cpu/r8000.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/spaces.h \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/dma/noncoherent.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/const.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cpu-features.h \
    $(wildcard include/config/sys/supports/micromips.h) \
    $(wildcard include/config/cpu/mipsr2/irq/vi.h) \
    $(wildcard include/config/cpu/mipsr2/irq/ei.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cpu.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cpu-info.h \
    $(wildcard include/config/mips/mt/smp.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cache.h \
    $(wildcard include/config/mips/l1/cache/shift.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/kmalloc.h \
    $(wildcard include/config/dma/coherent.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/cpu-feature-overrides.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/war.h \
    $(wildcard include/config/cpu/r4000/workarounds.h) \
    $(wildcard include/config/cpu/r4400/workarounds.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink/war.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/ffz.h \
  include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  include/asm-generic/bitops/sched.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/arch_hweight.h \
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
  include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
  include/linux/kern_levels.h \
  include/linux/dynamic_debug.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  include/uapi/linux/string.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/string.h \
    $(wildcard include/config/cpu/r3000.h) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/errno.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/errno.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/errno-base.h \
  include/uapi/linux/kernel.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/sysinfo.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/div64.h \
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
    $(wildcard include/config/generic/bug.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/break.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/break.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/thread_info.h \
    $(wildcard include/config/page/size/4kb.h) \
    $(wildcard include/config/page/size/8kb.h) \
    $(wildcard include/config/page/size/16kb.h) \
    $(wildcard include/config/page/size/32kb.h) \
    $(wildcard include/config/page/size/64kb.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/processor.h \
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
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/cachectl.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mipsregs.h \
    $(wildcard include/config/cpu/vr41xx.h) \
    $(wildcard include/config/mips/huge/tlb/support.h) \
    $(wildcard include/config/cpu/micromips.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/hazards.h \
    $(wildcard include/config/cpu/mipsr1.h) \
    $(wildcard include/config/mips/alchemy.h) \
    $(wildcard include/config/cpu/bmips.h) \
    $(wildcard include/config/cpu/loongson2.h) \
    $(wildcard include/config/cpu/r10000.h) \
    $(wildcard include/config/cpu/r5500.h) \
    $(wildcard include/config/cpu/xlr.h) \
    $(wildcard include/config/cpu/sb1.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/prefetch.h \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/irqflags.h \
    $(wildcard include/config/irq/cpu.h) \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/spinlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/prove/rcu.h) \
  include/linux/rwlock_types.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/spinlock.h \
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
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/atomic.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cmpxchg.h \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/atomic-long.h \
  include/asm-generic/atomic64.h \
  include/linux/math64.h \
  include/uapi/linux/time.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/param.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  include/uapi/asm-generic/param.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/timex.h \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/jiffies.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/io.h \
    $(wildcard include/config/pci.h) \
  include/asm-generic/iomap.h \
    $(wildcard include/config/has/ioport.h) \
    $(wildcard include/config/generic/iomap.h) \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/no/generic/pci/ioport/map.h) \
    $(wildcard include/config/generic/pci/iomap.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/page.h \
    $(wildcard include/config/cpu/mips32.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
  include/linux/pfn.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  include/asm-generic/getorder.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/pgtable-bits.h \
    $(wildcard include/config/cpu/tx39xx.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/ioremap.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/mangle-port.h \
    $(wildcard include/config/swap/io/space.h) \
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
  include/linux/wait.h \
  arch/mips/include/generated/asm/current.h \
  include/asm-generic/current.h \
  include/uapi/linux/wait.h \
  include/linux/rcutree.h \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
    $(wildcard include/config/sysfs.h) \
  include/linux/interrupt.h \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/irq/forced/threading.h) \
    $(wildcard include/config/generic/irq/probe.h) \
    $(wildcard include/config/proc/fs.h) \
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
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/hardirq.h \
  include/asm-generic/hardirq.h \
  include/linux/irq_cpustat.h \
  include/linux/irq.h \
    $(wildcard include/config/generic/pending/irq.h) \
    $(wildcard include/config/hardirqs/sw/resend.h) \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/smp.h \
    $(wildcard include/config/kexec.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/smp-ops.h \
    $(wildcard include/config/smp/up.h) \
    $(wildcard include/config/mips/cmp.h) \
  arch/mips/include/generated/asm/percpu.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  include/linux/gfp.h \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/cma.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/isolation.h) \
    $(wildcard include/config/memcg.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/have/memblock/node/map.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/numa/balancing.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/nodemask.h \
    $(wildcard include/config/movable/node.h) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/linux/page-flags-layout.h \
  include/generated/bounds.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/have/bootmem/info/node.h) \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/sched/book.h) \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/topology.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/topology.h \
  include/asm-generic/topology.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/irq.h \
    $(wildcard include/config/i8259.h) \
    $(wildcard include/config/mips/mt/smtc/irqaff.h) \
    $(wildcard include/config/mips/mt/smtc/im/backstop.h) \
  include/linux/irqdomain.h \
    $(wildcard include/config/irq/domain.h) \
    $(wildcard include/config/of/irq.h) \
  include/linux/radix-tree.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mipsmtregs.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/irq.h \
    $(wildcard include/config/irq/cpu/rm7k.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/ptrace.h \
    $(wildcard include/config/cpu/has/smartmips.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/isadep.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/ptrace.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/irq_regs.h \
  include/linux/irqdesc.h \
    $(wildcard include/config/irq/preflow/fasteoi.h) \
    $(wildcard include/config/sparse/irq.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/hw_irq.h \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/timerfd.h) \
  include/linux/rbtree.h \
  include/linux/timerqueue.h \
  include/linux/kref.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/smtc_ipi.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/time.h \
    $(wildcard include/config/cevt/r4k.h) \
    $(wildcard include/config/csrc/r4k.h) \
    $(wildcard include/config/met.h) \
  include/linux/rtc.h \
    $(wildcard include/config/rtc/intf/dev/uie/emul.h) \
    $(wildcard include/config/rtc/hctosys/device.h) \
  include/uapi/linux/rtc.h \
  include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/acpi.h) \
    $(wildcard include/config/pinctrl.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
  include/linux/ioport.h \
  include/linux/kobject.h \
    $(wildcard include/config/support/openwrt.h) \
  include/linux/sysfs.h \
  include/linux/kobject_ns.h \
  include/linux/klist.h \
  include/linux/pinctrl/devinfo.h \
  include/linux/pm.h \
    $(wildcard include/config/vt/console/sleep.h) \
    $(wildcard include/config/pm.h) \
    $(wildcard include/config/pm/runtime.h) \
    $(wildcard include/config/pm/clk.h) \
    $(wildcard include/config/pm/generic/domains.h) \
  include/linux/ratelimit.h \
  include/linux/uidgid.h \
    $(wildcard include/config/uidgid/strict/type/checks.h) \
    $(wildcard include/config/user/ns.h) \
  include/linux/highuid.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/seq_file.h \
  include/linux/cdev.h \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/poll.h \
  include/linux/fs.h \
    $(wildcard include/config/splice/net/support.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  include/linux/dcache.h \
  include/linux/rculist.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/path.h \
  include/linux/stat.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/pid.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/fiemap.h \
  include/linux/shrinker.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/blk_types.h \
    $(wildcard include/config/blk/cgroup.h) \
    $(wildcard include/config/blk/dev/integrity.h) \
  include/uapi/linux/fs.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/limits.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/ioctl.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  include/linux/percpu_counter.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/fcntl.h \
  include/uapi/linux/fcntl.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/fcntl.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/fcntl.h \
  include/linux/err.h \
  include/linux/sysctl.h \
    $(wildcard include/config/sysctl.h) \
  include/uapi/linux/sysctl.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/uaccess.h \
    $(wildcard include/config/eva.h) \
  include/uapi/linux/poll.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/poll.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/poll.h \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/gic.h \
    $(wildcard include/config/ofs.h) \
    $(wildcard include/config/countstop/shf.h) \
    $(wildcard include/config/countstop/msk.h) \
    $(wildcard include/config/countbits/shf.h) \
    $(wildcard include/config/countbits/msk.h) \
    $(wildcard include/config/numintrs/shf.h) \
    $(wildcard include/config/numintrs/msk.h) \
    $(wildcard include/config/numvpes/shf.h) \
    $(wildcard include/config/numvpes/msk.h) \
  /home/zhangtianchuan/882_89018/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/cevt-r4k.h \

arch/mips/kernel/cevt-r4k.o: $(deps_arch/mips/kernel/cevt-r4k.o)

$(deps_arch/mips/kernel/cevt-r4k.o):
