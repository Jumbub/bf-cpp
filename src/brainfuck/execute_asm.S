/*
 * brainfuck execute, rewritten in x86-64 assembly.
 *
 * Replaces the computed-goto interpreter loop in execute.cpp.
 * C++ side still handles:
 *   - allocation of the data tape,
 *   - setupInstructionAddresses (which patches each Instruction's jump/next),
 *   - I/O via bf_output_c / bf_input_c shims.
 *
 * Instruction layout (must match instruction.h, sizeof == 24):
 *   offset 0  : jump    (void*)      -- address of handler label in this file
 *   offset 8  : value / next         -- int64 operand or Instruction*
 *   offset 16 : move                 -- int64 data-pointer delta
 *
 * Register allocation (System V AMD64 ABI):
 *   rbx -> current Instruction*      (callee-saved)
 *   r12 -> current data pointer      (callee-saved)
 *
 * Each handler inlines the "advance to next instruction" sequence so the CPU
 * frontend doesn't eat an extra unconditional branch per dispatched op. This
 * matches what g++ produces for the computed-goto interpreter in execute.cpp
 * and is the single largest perf lever on this interpreter.
 *
 * Exposed symbols:
 *   brainfuck_jump_table  : array of 9 handler addresses, indexed by Type
 *   brainfuck_execute_asm : void(Instruction* begin, int8_t* data)
 */

	.intel_syntax noprefix

/* -------------------------------------------------------------------------- */
/* Jump table exposed to C++ so setupInstructionAddresses can patch each      */
/* instruction's jump field with the real handler address.                    */
/* -------------------------------------------------------------------------- */
	/* Relocatable data (shared library), local label addresses -> need writes at load. */
	.section .data.rel.ro, "aw", @progbits
	.globl brainfuck_jump_table
	.align 8
brainfuck_jump_table:
	.quad .L_NOOP              /* NOOP                                 */
	.quad .L_DONE              /* DONE                                 */
	.quad .L_DATA_ADD          /* DATA_ADD                             */
	.quad .L_DATA_SET_FROM_INPUT /* DATA_SET_FROM_INPUT                */
	.quad .L_DATA_PRINT        /* DATA_PRINT                           */
	.quad 0                    /* DATA_POINTER_ADD (squashed; unused)  */
	.quad .L_IP_SET_IF_ZERO    /* INSTRUCTION_POINTER_SET_IF_ZERO      */
	.quad .L_IP_SET_IF_NOT_ZERO /* INSTRUCTION_POINTER_SET_IF_NOT_ZERO */
	.quad .L_DATA_TRANSFER     /* DATA_TRANSFER                        */

/* -------------------------------------------------------------------------- */
/* void brainfuck_execute_asm(Instruction* begin, int8_t* data);              */
/*   rdi = begin                                                              */
/*   rsi = data                                                               */
/* -------------------------------------------------------------------------- */
	.text
	.globl brainfuck_execute_asm
	.type  brainfuck_execute_asm, @function
	.align 16
brainfuck_execute_asm:
	/* 3 pushes -> rsp 16-byte-aligned for calls inside I/O handlers. */
	push rbp
	push rbx
	push r12

	mov rbx, rdi               /* instruction = begin                  */
	mov r12, rsi               /* data        = datas                  */

	/* Enter the dispatch loop: apply move for the first instruction, jump. */
	add r12, [rbx + 16]
	jmp [rbx]

/* -------------------------------------------------------------------------- */
/* `+`/`-` -- *data += (int8_t)instruction->value;                            */
/*                                                                            */
/* Hot path. The load of value and the instruction-pointer bump are           */
/* independent, so putting `add rbx,24` between the load and the store        */
/* lets the frontend start fetching the next instruction while the byte       */
/* write retires. NEXT is inlined to save an unconditional branch.            */
/* -------------------------------------------------------------------------- */
	.align 16
.L_DATA_ADD:
	mov rax, [rbx + 8]         /* value                                */
	add rbx, 24                /* advance instruction pointer          */
	add [r12], al              /* *data += (int8_t)value               */
	add r12, [rbx + 16]        /* apply next instruction's move        */
	jmp [rbx]

/* -------------------------------------------------------------------------- */
/* NOOP -- just advance. Parser leaves these in for a couple of reasons;     */
/* they must still dispatch correctly.                                        */
/* -------------------------------------------------------------------------- */
	.align 16
.L_NOOP:
	add rbx, 24
	add r12, [rbx + 16]
	jmp [rbx]

/* -------------------------------------------------------------------------- */
/* Transfer loop: parser turned e.g. `[->++>+++<<]` into a DATA_TRANSFER      */
/* followed by DATA_TRANSFER_META entries; instruction->next points at the    */
/* last META entry. Semantics (see execute.cpp):                              */
/*   multiplier = *data & 255                                                 */
/*   for each meta: *(data + meta.move) += (int8_t)(multiplier * meta.value)  */
/*   *data = 0                                                                */
/* -------------------------------------------------------------------------- */
	.align 16
.L_DATA_TRANSFER:
	movzx ecx, byte ptr [r12]  /* multiplier = *data & 255             */
	mov rdx, [rbx + 8]         /* last = instruction->next             */
	cmp rbx, rdx
	jae .L_xfer_done
.L_xfer_body:
	add rbx, 24                /* ++instruction                        */
	mov rax, [rbx + 16]        /* meta.move                            */
	mov rdi, [rbx + 8]         /* meta.value                           */
	imul rdi, rcx              /* multiplier * value                   */
	add [r12 + rax], dil       /* *(data + move) += (int8_t)...        */
	cmp rbx, rdx
	jb .L_xfer_body
.L_xfer_done:
	mov byte ptr [r12], 0      /* *data = 0                            */
	add rbx, 24                /* advance past last META               */
	add r12, [rbx + 16]
	jmp [rbx]

/* -------------------------------------------------------------------------- */
/* `[` -- if *data == 0, jump to matching `]`; else fall through.             */
/* -------------------------------------------------------------------------- */
	.align 16
.L_IP_SET_IF_ZERO:
	cmp byte ptr [r12], 0
	je  .L_ipz_taken
	add rbx, 24
	add r12, [rbx + 16]
	jmp [rbx]
.L_ipz_taken:
	mov rbx, [rbx + 8]         /* instruction = instruction->next      */
	add r12, [rbx + 16]
	jmp [rbx]

/* -------------------------------------------------------------------------- */
/* `]` -- if *data != 0, jump back to matching `[`; else fall through.        */
/* Also handles the scan-loop optimisation: when instruction->next == self,   */
/* scan the tape by `move` until we hit a zero cell.                          */
/* -------------------------------------------------------------------------- */
	.align 16
.L_IP_SET_IF_NOT_ZERO:
	mov rax, [rbx + 8]         /* next                                 */
	cmp rax, rbx
	je  .L_ipnz_scan           /* self-loop -> scan path               */

	/* Normal ] */
	cmp byte ptr [r12], 0
	je  .L_ipnz_fallthrough
	mov rbx, rax               /* instruction = instruction->next      */
	add r12, [rbx + 16]
	jmp [rbx]

.L_ipnz_fallthrough:
	add rbx, 24
	add r12, [rbx + 16]
	jmp [rbx]

.L_ipnz_scan:
	/* Scan loop: while (*data != 0) data += move; then fall through.
	   move is cached in rdx across iterations since the instruction itself
	   doesn't change. */
	mov rdx, [rbx + 16]
.L_scan:
	cmp byte ptr [r12], 0
	je  .L_ipnz_fallthrough    /* *data==0 -> exit scan -> NEXT        */
	add r12, rdx
	jmp .L_scan

/* -------------------------------------------------------------------------- */
/* `.` -- output(*data, instruction->value);                                  */
/* Cold path (I/O). PLT call is fine here.                                    */
/* -------------------------------------------------------------------------- */
	.align 16
.L_DATA_PRINT:
	movsx edi, byte ptr [r12]  /* arg1: *data as int                   */
	mov rsi, [rbx + 8]         /* arg2: times                          */
	call bf_output_c
	add rbx, 24
	add r12, [rbx + 16]
	jmp [rbx]

/* -------------------------------------------------------------------------- */
/* `,` -- input(data, instruction->value); cold path.                         */
/* -------------------------------------------------------------------------- */
	.align 16
.L_DATA_SET_FROM_INPUT:
	mov rdi, r12               /* arg1: data                           */
	mov rsi, [rbx + 8]         /* arg2: times                          */
	call bf_input_c
	add rbx, 24
	add r12, [rbx + 16]
	jmp [rbx]

/* -------------------------------------------------------------------------- */
/* Program termination.                                                       */
/* -------------------------------------------------------------------------- */
.L_DONE:
	pop r12
	pop rbx
	pop rbp
	ret

	.size brainfuck_execute_asm, .-brainfuck_execute_asm

/* Non-executable stack marker (required on Linux for .S files). */
	.section .note.GNU-stack, "", @progbits
