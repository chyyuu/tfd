/* 
 *  mappings for target-i386 internals
 *
 *  Copyright (C) 2009-2013 Juan Caballero <juan.caballero@imdea.org>
 *  Copyright (C) 2009-2010 Zhenkai Liang <liangzk@comp.nus.edu.sg>
 *  Copyright (C) 2009-2010 Heng Yin <heyin@syr.edu>
 *  All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _DECAF_TARGET_H_
#define _DECAF_TARGET_H_

#define DECAF_CPU_REGS cpu_single_env->regs
#define DECAF_CPU_EIP (&cpu_single_env->eip)
#define DECAF_CPU_EFLAGS (&cpu_single_env->eflags)
#define DECAF_CPU_HFLAGS (&cpu_single_env->hflags)
#define DECAF_CPU_SEGS cpu_single_env->segs
#define DECAF_CPU_LDT (&cpu_single_env->ldt)
#define DECAF_CPU_GDT (&cpu_single_env->gdt)
#define DECAF_CPU_IDT (&cpu_single_env->idt)
#define DECAF_CPU_CR cpu_single_env->cr
#define DECAF_CPU_DF (&cpu_single_env->df)
#define DECAF_CC_OP (&cpu_single_env->cc_op)
#define DECAF_CPU_XMM_REGS ((XMMReg *)(&(cpu_single_env->xmm_regs)))
#define DECAF_CPU_FP_REGS ((FPReg *)(&(cpu_single_env->fpregs)))
#define DECAF_CPU_FP_STT (&cpu_single_env->fpstt)
#define DECAF_CPU_FPUS (&cpu_single_env->fpus)
#define DECAF_CPU_FPUC (&cpu_single_env->fpuc)
#define DECAF_CPU_FPTAGS cpu_single_env->fptags
#define DECAF_CPU_BRANCH_CNT (&cpu_single_env->branch_cnt)

#endif // _DECAF_TARGET_H_
