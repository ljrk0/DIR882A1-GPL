cmd_net/ipv6/ip6_fib.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,net/ipv6/.ip6_fib.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi -Iinclude/generated/uapi -include /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0xffffffff80001000 -DDATAOFFSET=0 -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-ralink -I/home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(ip6_fib)"  -D"KBUILD_MODNAME=KBUILD_STR(ipv6)" -c -o net/ipv6/ip6_fib.o net/ipv6/ip6_fib.c

source_net/ipv6/ip6_fib.o := net/ipv6/ip6_fib.c

deps_net/ipv6/ip6_fib.o := \
    $(wildcard include/config/ipv6/subtrees.h) \
    $(wildcard include/config/ipv6/multiple/tables.h) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/errno.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/errno.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/errno-base.h \
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
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/posix_types.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sgidefs.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/posix_types.h \
  include/linux/net.h \
    $(wildcard include/config/compat.h) \
  include/linux/stringify.h \
  include/linux/random.h \
    $(wildcard include/config/arch/random.h) \
  include/uapi/linux/random.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/ioctl.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/irqnr.h \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/smp.h) \
  include/uapi/linux/irqnr.h \
  include/linux/wait.h \
    $(wildcard include/config/lockdep.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/const.h \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/typecheck.h \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/context/tracking.h) \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/debug/stack/usage.h) \
  include/linux/bug.h \
    $(wildcard include/config/generic/bug.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/break.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/break.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
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
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/string.h \
    $(wildcard include/config/cpu/r3000.h) \
  include/uapi/linux/kernel.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/sysinfo.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/div64.h \
  include/asm-generic/div64.h \
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
  arch/mips/include/generated/asm/current.h \
  include/asm-generic/current.h \
  include/uapi/linux/wait.h \
  include/linux/fcntl.h \
  include/uapi/linux/fcntl.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/fcntl.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/fcntl.h \
  include/linux/kmemcheck.h \
    $(wildcard include/config/kmemcheck.h) \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/have/cmpxchg/double.h) \
    $(wildcard include/config/have/aligned/struct/page.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mm/owner.h) \
    $(wildcard include/config/mmu/notifier.h) \
    $(wildcard include/config/transparent/hugepage.h) \
    $(wildcard include/config/numa/balancing.h) \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/mips/include/generated/uapi/asm/auxvec.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/auxvec.h \
  include/linux/rbtree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/completion.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/guard.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  include/linux/uprobes.h \
    $(wildcard include/config/arch/supports/uprobes.h) \
    $(wildcard include/config/uprobes.h) \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/generated/bounds.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/page.h \
    $(wildcard include/config/cpu/mips32.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
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
    $(wildcard include/config/discontigmem.h) \
  include/asm-generic/getorder.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mmu.h \
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
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/seqlock.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/rcutree.h \
  include/uapi/linux/net.h \
  include/linux/socket.h \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/splice/net/support.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/socket.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/socket.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/sockios.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/sockios.h \
  include/linux/uio.h \
  include/uapi/linux/uio.h \
  include/uapi/linux/socket.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/route.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/if.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/hdlc/ioctl.h \
  include/linux/netdevice.h \
    $(wildcard include/config/dcb.h) \
    $(wildcard include/config/wlan.h) \
    $(wildcard include/config/ax25.h) \
    $(wildcard include/config/mac80211/mesh.h) \
    $(wildcard include/config/net/ipip.h) \
    $(wildcard include/config/net/ipgre.h) \
    $(wildcard include/config/ipv6/sit.h) \
    $(wildcard include/config/ipv6/tunnel.h) \
    $(wildcard include/config/rps.h) \
    $(wildcard include/config/netpoll.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/xps.h) \
    $(wildcard include/config/bql.h) \
    $(wildcard include/config/rfs/accel.h) \
    $(wildcard include/config/fcoe.h) \
    $(wildcard include/config/net/poll/controller.h) \
    $(wildcard include/config/libfcoe.h) \
    $(wildcard include/config/wireless/ext.h) \
    $(wildcard include/config/vlan/8021q.h) \
    $(wildcard include/config/net/dsa.h) \
    $(wildcard include/config/net/ns.h) \
    $(wildcard include/config/netprio/cgroup.h) \
    $(wildcard include/config/net/dsa/tag/dsa.h) \
    $(wildcard include/config/net/dsa/tag/trailer.h) \
    $(wildcard include/config/netpoll/trap.h) \
  include/linux/pm_qos.h \
    $(wildcard include/config/pm.h) \
    $(wildcard include/config/pm/runtime.h) \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/mutex/spin/on/owner.h) \
    $(wildcard include/config/have/arch/mutex/cpu/relax.h) \
  include/linux/srcu.h \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/math64.h \
  include/uapi/linux/time.h \
  include/linux/jiffies.h \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/param.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  include/uapi/asm-generic/param.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/timex.h \
  include/linux/miscdevice.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/major.h \
  include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/acpi.h) \
    $(wildcard include/config/pinctrl.h) \
    $(wildcard include/config/cma.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
  include/linux/ioport.h \
    $(wildcard include/config/memory/hotremove.h) \
  include/linux/kobject.h \
    $(wildcard include/config/support/openwrt.h) \
  include/linux/sysfs.h \
  include/linux/kobject_ns.h \
  include/linux/kref.h \
  include/linux/klist.h \
  include/linux/pinctrl/devinfo.h \
  include/linux/pm.h \
    $(wildcard include/config/vt/console/sleep.h) \
    $(wildcard include/config/pm/clk.h) \
    $(wildcard include/config/pm/generic/domains.h) \
  include/linux/ratelimit.h \
  include/linux/uidgid.h \
    $(wildcard include/config/uidgid/strict/type/checks.h) \
    $(wildcard include/config/user/ns.h) \
  include/linux/highuid.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/device.h \
  include/linux/pm_wakeup.h \
  include/linux/delay.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/delay.h \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/smp.h \
    $(wildcard include/config/kexec.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/smp-ops.h \
    $(wildcard include/config/smp/up.h) \
    $(wildcard include/config/mips/cmp.h) \
  arch/mips/include/generated/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  include/linux/rculist.h \
  include/linux/dmaengine.h \
    $(wildcard include/config/async/tx/enable/channel/switch.h) \
    $(wildcard include/config/rapidio/dma/engine.h) \
    $(wildcard include/config/dma/engine.h) \
    $(wildcard include/config/net/dma.h) \
    $(wildcard include/config/async/tx/dma.h) \
  include/linux/scatterlist.h \
    $(wildcard include/config/debug/sg.h) \
  include/linux/mm.h \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/x86.h) \
    $(wildcard include/config/ppc.h) \
    $(wildcard include/config/parisc.h) \
    $(wildcard include/config/metag.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/ksm.h) \
    $(wildcard include/config/have/memblock/node/map.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/debug/vm/rb.h) \
    $(wildcard include/config/arch/uses/numa/prot/none.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/hugetlbfs.h) \
  include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/isolation.h) \
    $(wildcard include/config/memcg.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/nodemask.h \
    $(wildcard include/config/movable/node.h) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/have/bootmem/info/node.h) \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/sched/book.h) \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/topology.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/topology.h \
  include/asm-generic/topology.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
  include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  include/linux/range.h \
  include/linux/bit_spinlock.h \
  include/linux/shrinker.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/pgtable.h \
    $(wildcard include/config/cpu/supports/uncached/accelerated.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/pgtable-32.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/fixmap.h \
    $(wildcard include/config/eva/3gb.h) \
  include/asm-generic/pgtable-nopmd.h \
  include/asm-generic/pgtable-nopud.h \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/swap.h) \
  include/linux/huge_mm.h \
  include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/migration.h) \
  arch/mips/include/generated/asm/scatterlist.h \
  include/asm-generic/scatterlist.h \
    $(wildcard include/config/need/sg/dma/length.h) \
  include/linux/dynamic_queue_limits.h \
  include/linux/ethtool.h \
  include/linux/compat.h \
    $(wildcard include/config/compat/old/sigaction.h) \
    $(wildcard include/config/odd/rt/sigaction.h) \
  include/uapi/linux/ethtool.h \
  include/linux/if_ether.h \
  include/linux/skbuff.h \
    $(wildcard include/config/imq.h) \
    $(wildcard include/config/nf/conntrack.h) \
    $(wildcard include/config/bridge/netfilter.h) \
    $(wildcard include/config/nf/defrag/ipv4.h) \
    $(wildcard include/config/nf/defrag/ipv6.h) \
    $(wildcard include/config/xfrm.h) \
    $(wildcard include/config/raeth/skb/recycle/2k.h) \
    $(wildcard include/config/net/sched.h) \
    $(wildcard include/config/net/cls/act.h) \
    $(wildcard include/config/ipv6/ndisc/nodetype.h) \
    $(wildcard include/config/network/secmark.h) \
    $(wildcard include/config/pppopptp.h) \
    $(wildcard include/config/pppol2tp.h) \
    $(wildcard include/config/network/phy/timestamping.h) \
    $(wildcard include/config/netfilter/xt/target/trace.h) \
  include/linux/textsearch.h \
  include/linux/err.h \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/failslab.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/debug/slab.h) \
  include/linux/slab_def.h \
    $(wildcard include/config/memcg/kmem.h) \
  include/net/checksum.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/uaccess.h \
    $(wildcard include/config/eva.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/checksum.h \
  include/linux/in6.h \
  include/uapi/linux/in6.h \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/timerfd.h) \
  include/linux/timerqueue.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/arch/has/dma/set/coherent/mask.h) \
    $(wildcard include/config/have/dma/attrs.h) \
    $(wildcard include/config/need/dma/map/state.h) \
  include/linux/dma-attrs.h \
  include/linux/dma-direction.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/dma-mapping.h \
    $(wildcard include/config/sgi/ip27.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/dma-coherence.h \
  include/asm-generic/dma-coherent.h \
    $(wildcard include/config/have/generic/dma/coherent.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/dma-coherence.h \
  include/asm-generic/dma-mapping-common.h \
  include/linux/dma-debug.h \
    $(wildcard include/config/dma/api/debug.h) \
  include/linux/netdev_features.h \
  include/net/flow_keys.h \
  include/linux/imq.h \
  include/uapi/linux/if_ether.h \
  include/net/net_namespace.h \
    $(wildcard include/config/ipv6.h) \
    $(wildcard include/config/ip/sctp.h) \
    $(wildcard include/config/ip/dccp.h) \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/wext/core.h) \
  include/linux/sysctl.h \
  include/uapi/linux/sysctl.h \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/xfrm/statistics.h) \
  include/net/snmp.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/snmp.h \
  include/linux/u64_stats_sync.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/ip/multiple/tables.h) \
    $(wildcard include/config/ip/route/classid.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/ip/mroute.h) \
    $(wildcard include/config/ip/mroute/multiple/tables.h) \
  include/net/inet_frag.h \
  include/linux/percpu_counter.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/ipv6/mroute.h) \
    $(wildcard include/config/ipv6/mroute/multiple/tables.h) \
  include/net/dst_ops.h \
  include/net/netns/sctp.h \
  include/net/netns/dccp.h \
  include/net/netns/netfilter.h \
  include/linux/proc_fs.h \
  include/linux/fs.h \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/fs/xip.h) \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/path.h \
  include/linux/stat.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/stat.h \
  include/uapi/linux/stat.h \
  include/linux/radix-tree.h \
  include/linux/pid.h \
  include/linux/capability.h \
  include/uapi/linux/capability.h \
  include/linux/semaphore.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/fiemap.h \
  include/linux/migrate_mode.h \
  include/linux/percpu-rwsem.h \
  include/linux/blk_types.h \
    $(wildcard include/config/blk/cgroup.h) \
    $(wildcard include/config/blk/dev/integrity.h) \
  include/uapi/linux/fs.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/limits.h \
  include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/netfilter.h \
    $(wildcard include/config/jump/label.h) \
    $(wildcard include/config/nf/nat/needed.h) \
  include/linux/in.h \
  include/uapi/linux/in.h \
    $(wildcard include/config/inet/ipsec/saref.h) \
  include/uapi/linux/netfilter.h \
  include/net/flow.h \
  include/net/netns/x_tables.h \
    $(wildcard include/config/bridge/nf/ebtables.h) \
  include/net/netns/conntrack.h \
    $(wildcard include/config/nf/conntrack/proc/compat.h) \
    $(wildcard include/config/nf/conntrack/labels.h) \
  include/linux/list_nulls.h \
  include/linux/netfilter/nf_conntrack_tcp.h \
  include/uapi/linux/netfilter/nf_conntrack_tcp.h \
  include/net/netns/xfrm.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/xfrm.h \
  include/linux/seq_file_net.h \
  include/linux/seq_file.h \
  include/net/dsa.h \
  include/net/netprio_cgroup.h \
  include/linux/cgroup.h \
    $(wildcard include/config/cgroups.h) \
  include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/no/hz/common.h) \
    $(wildcard include/config/lockup/detector.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/virt/cpu/accounting/native.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/inotify/user.h) \
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
    $(wildcard include/config/virt/cpu/accounting/gen.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/bcache.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/irq/time/accounting.h) \
    $(wildcard include/config/no/hz/full.h) \
  include/uapi/linux/sched.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/ptrace.h \
    $(wildcard include/config/cpu/has/smartmips.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/isadep.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/ptrace.h \
  arch/mips/include/generated/asm/cputime.h \
  include/asm-generic/cputime.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
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
    $(wildcard include/config/trad/signals.h) \
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
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/seccomp/filter.h) \
  include/uapi/linux/seccomp.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/seccomp.h \
    $(wildcard include/config/mips32/o32.h) \
    $(wildcard include/config/mips32/n32.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/unistd.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/unistd.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/unistd.h \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
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
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/cgroupstats.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/taskstats.h \
  include/linux/prio_heap.h \
  include/linux/idr.h \
  include/linux/xattr.h \
  include/uapi/linux/xattr.h \
  include/linux/hardirq.h \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  include/linux/vtime.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/hardirq.h \
  include/asm-generic/hardirq.h \
  include/linux/irq_cpustat.h \
  include/linux/irq.h \
    $(wildcard include/config/generic/pending/irq.h) \
    $(wildcard include/config/hardirqs/sw/resend.h) \
  include/linux/irqreturn.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/irq.h \
    $(wildcard include/config/i8259.h) \
    $(wildcard include/config/mips/mt/smtc/irqaff.h) \
    $(wildcard include/config/mips/mt/smtc/im/backstop.h) \
  include/linux/irqdomain.h \
    $(wildcard include/config/irq/domain.h) \
    $(wildcard include/config/of/irq.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mipsmtregs.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/mach-generic/irq.h \
    $(wildcard include/config/irq/cpu/rm7k.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/irq_regs.h \
  include/linux/irqdesc.h \
    $(wildcard include/config/irq/preflow/fasteoi.h) \
    $(wildcard include/config/sparse/irq.h) \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/asm/hw_irq.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/neighbour.h \
  include/linux/netlink.h \
  include/net/scm.h \
    $(wildcard include/config/security/network.h) \
  include/linux/security.h \
    $(wildcard include/config/security/path.h) \
    $(wildcard include/config/security/network/xfrm.h) \
    $(wildcard include/config/securityfs.h) \
    $(wildcard include/config/security/yama.h) \
  include/linux/nsproxy.h \
  include/uapi/linux/netlink.h \
  include/uapi/linux/netdevice.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/if_packet.h \
  include/linux/if_link.h \
  include/uapi/linux/if_link.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
  include/net/ipv6.h \
    $(wildcard include/config/have/efficient/unaligned/access.h) \
  include/linux/ipv6.h \
    $(wildcard include/config/ipv6/privacy.h) \
    $(wildcard include/config/ipv6/router/pref.h) \
    $(wildcard include/config/ipv6/route/info.h) \
    $(wildcard include/config/ipv6/optimistic/dad.h) \
    $(wildcard include/config/ipv6/mip6.h) \
  include/uapi/linux/ipv6.h \
  include/linux/icmpv6.h \
  include/uapi/linux/icmpv6.h \
  include/linux/tcp.h \
    $(wildcard include/config/tcp/md5sig.h) \
  include/net/sock.h \
    $(wildcard include/config/net.h) \
  include/linux/uaccess.h \
  include/linux/memcontrol.h \
    $(wildcard include/config/memcg/swap.h) \
    $(wildcard include/config/inet.h) \
  include/linux/res_counter.h \
  include/linux/aio.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/aio_abi.h \
  include/linux/filter.h \
    $(wildcard include/config/bpf/jit.h) \
  include/uapi/linux/filter.h \
  include/linux/rculist_nulls.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/arch/mips/include/uapi/asm/poll.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/asm-generic/poll.h \
  include/net/dst.h \
  include/linux/rtnetlink.h \
  include/uapi/linux/rtnetlink.h \
  /home/guohongliang/work/new882/trunk/GPL_Code/source/linux-3.10.14.x/include/uapi/linux/if_addr.h \
  include/net/neighbour.h \
  include/net/rtnetlink.h \
  include/net/netlink.h \
  include/net/inet_connection_sock.h \
  include/net/inet_sock.h \
  include/linux/jhash.h \
  include/linux/unaligned/packed_struct.h \
  include/net/request_sock.h \
  include/net/netns/hash.h \
  include/net/inet_timewait_sock.h \
  include/net/tcp_states.h \
  include/net/timewait_sock.h \
  include/uapi/linux/tcp.h \
  include/linux/udp.h \
  include/uapi/linux/udp.h \
  include/net/if_inet6.h \
  include/net/ndisc.h \
  include/linux/if_arp.h \
    $(wildcard include/config/firewire/net.h) \
  include/uapi/linux/if_arp.h \
  include/linux/hash.h \
  include/net/addrconf.h \
  include/net/ip6_fib.h \
  include/linux/ipv6_route.h \
  include/uapi/linux/ipv6_route.h \
  include/net/inetpeer.h \
  include/net/ip6_route.h \
  include/linux/ip.h \
  include/uapi/linux/ip.h \

net/ipv6/ip6_fib.o: $(deps_net/ipv6/ip6_fib.o)

$(deps_net/ipv6/ip6_fib.o):