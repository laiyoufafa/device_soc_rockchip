/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MMU_CONTEXT_H
#define _LINUX_MMU_CONTEXT_H

#include <asm/mmu_context.h>
#include <asm/mmu.h>

/* Architectures that care about IRQ state in switch_mm can override this. */
#ifndef switch_mm_irqs_off
#define switch_mm_irqs_off switch_mm
#endif

#ifndef leave_mm
static inline void leave_mm(int cpu)
{
}
#endif

/*
 * CPUs that are capable of running task @p. By default, we assume a sane,
 * homogeneous system. Must contain at least one active CPU.
 */
#ifndef task_cpu_possible_mask
#define task_cpu_possible_mask(p) cpu_possible_mask
#endif

#endif
