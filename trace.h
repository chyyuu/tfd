/* 
 *  generation of trace files v60
 *
 *  Copyright (C) 2009-2013 Juan Caballero <juan.caballero@imdea.org>
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

#ifndef _TRACE_H_
#define _TRACE_H_

#include <inttypes.h>
#include "disasm.h"
#undef INLINE
#include "DECAF_main.h"

/* Size of buffer to store instructions */
#define FILEBUFSIZE 104857600

/* Trace header values */
#define VERSION_NUMBER 60
#define MAGIC_NUMBER 0xFFFFFFFF
#define TRAILER_BEGIN 0xFFFFFFFF
#define TRAILER_END 0x41AA42BB

/* Taint origins */
#define TAINT_SOURCE_NIC_IN 0
#define TAINT_SOURCE_KEYBOARD_IN 1
#define TAINT_SOURCE_FILE_IN 2
#define TAINT_SOURCE_NETWORK_OUT 3
#define TAINT_SOURCE_API_TIME_IN 4
#define TAINT_SOURCE_API_FILE_IN 5
#define TAINT_SOURCE_API_REGISTRY_IN 6
#define TAINT_SOURCE_API_HOSTNAME_IN 7
#define TAINT_SOURCE_API_FILE_INFO_IN 8
#define TAINT_SOURCE_API_SOCK_INFO_IN 9
#define TAINT_SOURCE_API_STR_IN 10
#define TAINT_SOURCE_API_SYS_IN 11
#define TAINT_SOURCE_HOOKAPI 12
#define TAINT_SOURCE_MODULE 13

/* Starting origin for network connections */
#define TAINT_ORIGIN_START_TCP_NIC_IN 10000
#define TAINT_ORIGIN_START_UDP_NIC_IN 11000
#define TAINT_ORIGIN_MODULE           20000

/* Taint propagation definitions */
#define TP_NONE 0           // No taint propagation
#define TP_SRC 1            // Taint propagated from SRC to DST
#define TP_CJMP 2           // Cjmp using tainted EFLAG
#define TP_MEMREAD_INDEX 3  // Memory read with tainted index
#define TP_MEMWRITE_INDEX 4 // Memory write with tainted index
#define TP_REP_COUNTER 5    // Instruction with REP prefix and tainted counter
#define TP_SYSENTER 6       // Sysenter

/* Trace format definitions */
// FXSAVE has a memory operand of 512 bytes, but we only save 30*4=120 bytes
// Anyway, XED does not return the read operands for FXSAVE
#define MAX_NUM_OPERANDS 30 // FNSAVE has a memory operand of 108 bytes
#define MAX_NUM_MEMREGS 7  /* Max number of memregs per memory operand */
#define MAX_NUM_TAINTBYTE_RECORDS 3
#define MAX_STRING_LEN 32
#define MAX_OPERAND_LEN 16 /* Max length of an operand in bytes */
#define MAX_INSN_BYTES 15 /* Maximum number of bytes in a x86 instruction */

/* Macros to access register ids / values */
#define REGNUM(regid) (regmapping[(regid) - 100])
#define MMXVAL(regid) (DECAF_cpu_fp_regs[(regid) - 164].mmx.q)
#define XMMVAL(regid,idx) (DECAF_cpu_xmm_regs[(regid) - 172]._q[(idx)])
#define FLOATVAL(regid) (DECAF_cpu_fp_regs[(regid) - 188].d)

enum OpUsage { unknown = 0, esp, counter, membase, memindex, memsegment,
  memsegent0, memsegent1, memdisplacement, memscale, eflags };


typedef struct _taint_byte_record {
  uint32_t source;              // Tainted data source (network,keyboard...)
  uint32_t origin;              // Identifies a network flow
  uint32_t offset;              // Offset in tainted data buffer (network)
} TaintByteRecord;

#define TAINT_RECORD_FIXED_SIZE 1

typedef struct _taint_record {
  uint8_t numRecords;          // How many TaintByteRecord currently used
  TaintByteRecord taintBytes[MAX_NUM_TAINTBYTE_RECORDS];
} taint_record_t;

#define OPERAND_VAL_FIXED_SIZE 4
#define OPERAND_VAL_ENUMS_REAL_SIZE 2

/* Operand value union, check operand type to know which entry to use */
typedef union _opval {
  uint32_t val32;
  uint64_t val64;
  XMMReg xmm_val;
  floatx80 float_val;
} opval;

/* Operand address union, check operand type to know which entry to use */
/*   Immediates have no address */
typedef union _opaddr {
  uint8_t reg_addr;
  uint32_t mem32_addr;
  uint64_t mem64_addr;
} opaddr;

/* OperandVal description
  access:        Operand access: read,written,etc --> xed_operand_action_enum_t
  length:        Operand length in bytes
  tainted:       Taint mask
  addr:          Operand address
  value:         Operand value
  type:          Operand type
  usage:         Used for serialization
  records[]:     Taint records for the operand
*/
typedef struct _operand_val {
  uint8_t access;
  uint8_t length;
  uint16_t tainted;
  enum OpType type;
  enum OpUsage usage;
  opaddr addr;
  opval value;
  taint_record_t records[MAX_OPERAND_LEN];
} OperandVal;

#define ENTRY_HEADER_FIXED_SIZE 24

/* Entry header description
  address:       Address where instruction is loaded in memory
  pid:           Process identifier
  tid:           Thread identifier
  inst_size:     Number of bytes in x86 instruction
  num_operands:  Number of operands (includes all except ESP)
  tp:            Taint propagation value. See above.
  df:            Direction flag. Has to be -1 (x86_df=1) or 1 (x86_df = 0)
                    COULD BE DERIVED FROM eflags
  eflags:        Value of the EFLAGS register
  cc_op:         Determines operation performed by QEMU on CC_SRC,CC_DST.
                   ONLY REQUIRES 8-bit
  operand[]:     Operands accessed by instruction
  memregs[][idx]:   Operands used for indirect addressing
    idx == 0 -> Segment register
    idx == 1 -> Base register
    idx == 2 -> Index register
    idx == 3 -> Segent0
    idx == 4 -> Segent1
    idx == 5 -> Displacement
    idx == 6 -> Scale
  rawbytes[]:   Rawbytes of the x86 instruction
*/
typedef struct _entry_header {
  uint32_t address;
  uint32_t pid;
  uint32_t tid;
  uint8_t inst_size;
  uint8_t num_operands;
  uint8_t tp;
  uint8_t df;
  uint32_t eflags;
  uint32_t cc_op;
  unsigned char rawbytes[MAX_INSN_BYTES];
  OperandVal operand[MAX_NUM_OPERANDS + 1];
  OperandVal memregs[MAX_NUM_OPERANDS + 1][MAX_NUM_MEMREGS];
} EntryHeader;

typedef struct _proc_record {
  char name[MAX_STRING_LEN];
  uint32_t pid;
  int n_mods;
  uint32_t ldt_base;
} ProcRecord;

typedef struct _module_record {
  char name[MAX_STRING_LEN];
  uint32_t base;
  uint32_t size;
} ModuleRecord;

typedef struct _trace_header {
  int magicnumber;
  int version;
  int n_procs;
  uint32_t gdt_base;
  uint32_t idt_base;
} TraceHeader;

/* Structure to hold trace statistics */
struct trace_stats {
  uint64_t insn_counter_decoded; // Number of instructions decoded
  uint64_t insn_counter_traced; // Number of instructions written to trace
  uint64_t insn_counter_traced_tainted; // Number of tainted instructions written to trace
  uint64_t operand_counter;      // Number of operands decoded
};

/* Exported variables */
extern int received_tainted_data;
extern int access_user_mem;
extern int insn_already_written;
extern int regmapping[];
extern char filebuf[FILEBUFSIZE];
extern unsigned int tid_to_trace;
extern int header_already_written;
extern struct trace_stats tstats;
extern int eflags_idx;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* Exported Functions */
int get_regnum(OperandVal op);
int getOperandOffset (OperandVal *op);
void xed2_init(void);   // Initialize disassembler
void decode_address(uint32_t address, EntryHeader *eh, int ignore_taint);
unsigned int write_insn(FILE *stream, EntryHeader *eh); // Write insn to trace
int close_trace(FILE *stream); // Write trailer to trace and close it
void print_trace_stats(void); // Print trace statistics
void clear_trace_stats(void); // Clear trace statistics

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _TRACE_H_

