// Made by Sean Pesce


#pragma once


#ifndef ASM_INJECT_X86_HPP
    #define ASM_INJECT_X86_HPP

#include <cstdint>  // uint8_t, uint32_t, uint16_t, int8_t, int32_t
#include <cstring>  // size_t, memset


//////////// Constants ////////////
const	uint8_t	NOP_INSTR_OPCODE = 0x90,   		// NOP instruction byte value
				JMP_REL8_INSTR_OPCODE = 0xEB,	// JMP rel8 opcode byte value
				JMP_REL32_INSTR_OPCODE = 0xE9,	// JMP rel32 opcode byte value
				CALL_REL32_INSTR_OPCODE = 0xE8, // CALL rel32 opcode byte value
				RET_INSTR_OPCODE = 0xC3,		// Near return (RET) instruction byte value
				RET_FAR_INSTR_OPCODE = 0xCB,	// Far return (RET) instruction byte value
				RET_IMM16_INSTR_OPCODE = 0xC2,		// Near return, pop imm16 bytes from stack (RET imm16) opcode value
				RET_FAR_IMM16_INSTR_OPCODE = 0xCA;	// Far return, pop imm16 bytes from stack (RET imm16) opcode value
//
const	int		JMP_REL8_INSTR_LENGTH  = 2,		// The length of a 'JMP rel8' instruction (in bytes)
				JMP_REL32_INSTR_LENGTH = 5,		// The length of a 'JMP rel32' instruction (in bytes)
				CALL_REL32_INSTR_LENGTH = 5,	// CALL rel32 instruction
				RET_INSTR_LENGTH = 1,			// RET instruction
				RET_IMM16_INSTR_LENGTH = 3;		// RET imm16 instruction



//////////// Global Variables ////////////
//
//
// Offset from function start address when jumping to assembly code:
// (can be changed to accomodate different compilers and/or calling conventions)
#ifdef _MSC_VER
	#define SP_ASM_FUNC_START_OFFSET 0; // Using a Microsoft compiler; jump straight to the injected function
#else
	#define SP_ASM_FUNC_START_OFFSET 3; // Using non-MS compiler; GCC in-line ASM starts +3 bytes from asm_code
#endif // _MSC_VER




//////////// Function prototypes and descriptions ////////////


/* inject_jmp_5b
 *  Injects assembly code at the specified location in memory.
 *		NOTE:	Memory at the injection point must be writable before calling this function (use
 *				SET_MEM_PROTECTION).
 *		NOTE:	There must be AT LEAST 5 bytes of memory at the injection point that is either
 *				unused or that can be overwritten without disrupting program logic; this space
 *				is necessary to write the JMP instruction that directs execution to the injected
 *				assembly code.
 *  @param inject_at    The location in memory where the assembly code will be injected. A
 *                          JMP instruction will be written at this location, containing the
 *                          address of the assembly function referenced by the asm_code parameter.
 *  @param return_jmp_addr  The location in memory where the code cave should return to
 *                          after execution. returnJumpAddr should point to a uin32_t that
 *                          is referenced in a JMP instruction at the end of the assembly
 *                          function referenced by the asm_code parameter.
 *  @param nops         The number of NOP instructions to be written after the initial JMP
 *                          instruction. These NOPs will erase any remaining garbage bytes
 *                          that resulted from overwriting existing instructions at the injection
 *                          location.
 *  @param asm_code  A pointer to an in-line assembly function (to be used as a code cave).
 */
void inject_jmp_5b(uint8_t *inject_at, uint32_t *return_jmp_addr, int nops, void *asm_code);


/* write_jmp_rel8
 *  Writes a JMP rel8 instruction at the specified write_to address.
 *
 *  @param write_to  The address where the JMP rel8 instruction will be written.
 *  @param jmp_to    The address where the JMP rel8 instruction will jump to. After the JMP rel8
 *                  instruction executes, %eip = jmp_to.
 *                  NOTE: jmp_to must be in the range [write_to-128,write_to+127]
 *  @param nops The number of NOP instructions to be written after the JMP rel8 instruction.
 */
void write_jmp_rel8(void *write_to, void *jmp_to, int nops);


/* write_jmp_rel8
 *  Writes a JMP rel8 instruction at the specified write_to address using the given offset.
 *
 *  @param write_to  The address where the JMP rel8 instruction will be written.
 *  @param offset   The offset that the JMP rel8 instruction will jump to. After the JMP rel8
 *                  instruction executes, %eip = write_to+offset+2.
 *                  NOTE: offset must be in the range [write_to-128,write_to+127]
 *  @param nops The number of NOP instructions to be written after the JMP rel8 instruction.
 */
void write_jmp_rel8(void *write_to, int8_t offset, int nops);


/* write_jmp_rel32
 *  Writes a JMP rel32 instruction at the specified write_to address.
 *
 *  @param write_to  The address where the JMP rel32 instruction will be written.
 *  @param jmp_to    The address where the JMP rel32 instruction will jump to. After the JMP rel32
 *                  instruction executes, %eip = jmp_to.
 *                  NOTE: jmp_to must be in the range [write_to-2³¹,write_to+2³¹-1]
 *  @param nops The number of NOP instructions to be written after the JMP rel32 instruction.
 */
void write_jmp_rel32(void *write_to, void *jmp_to, int nops);


/* write_jmp_rel32
 *  Writes a JMP rel32 instruction at the specified write_to address using the given offset.
 *
 *  @param write_to  The address where the JMP rel32 instruction will be written.
 *  @param offset   The offset that the JMP rel32 instruction will jump to. After the JMP rel32
 *                  instruction executes, %eip = write_to+offset+5.
 *                  NOTE: offset must be in the range [write_to-2³¹,write_to+2³¹-1]
 *  @param nops The number of NOP instructions to be written after the JMP rel32 instruction.
 */
void write_jmp_rel32(void *write_to, int32_t offset, int nops);


/* write_call_rel32
 *  Writes a CALL rel32 instruction at the specified write_to address.
 *
 *  @param write_to      The address where the CALL rel32 instruction will be written.
 *  @param procedure    The address of the called procedure. After the CALL rel32
 *                      instruction executes, %eip = procedure.
 *                      NOTE: procedure must be in the range [write_to-2³¹,write_to+2³¹-1]
 *  @param nops     The number of NOP instructions to be written after the CALL rel32 instruction.
 */
void write_call_rel32(void *write_to, void *procedure, int nops);


/* write_call_rel32
 *  Writes a CALL rel32 instruction at the specified write_to address using the given offset.
 *
 *  @param write_to  The address where the CALL rel32 instruction will be written.
 *  @param offset   The offset of the called procedure. After the CALL rel32
 *                  instruction executes, %eip = write_to+offset+5.
 *                  NOTE: offset must be in the range [write_to-2³¹,write_to+2³¹-1]
 *  @param nops The number of NOP instructions to be written after the CALL rel32 instruction.
 */
void write_call_rel32(void *write_to, int32_t offset, int nops);


/* write_ret
 *  Writes a "near return" (RET) instruction at the specified write_to address.
 *
 *  @param write_to  The address where the RET instruction will be written.
 *  @param nops The number of NOP instructions to be written after the RET instruction.
 */
void write_ret(void *write_to, int nops);


/* write_ret_far
 *  Writes a "far return" (RET) instruction at the specified write_to address.
 *
 *  @param write_to  The address where the RET instruction will be written.
 *  @param nops The number of NOP instructions to be written after the RET instruction.
 */
void write_ret_far(void *write_to, int nops);


/* write_ret_imm16
 *  Writes a "near return, pop imm16 bytes from stack" (RET imm16) instruction at the
 *	 specified write_to address.
 *
 *  @param write_to  The address where the RET imm16 instruction will be written.
 *  @param pop_bytes The number of bytes to pop from the stack before returning to the calling
 *					procedure.
 *  @param nops The number of NOP instructions to be written after the RET imm16 instruction.
 */
void write_ret_imm16(void *write_to, uint16_t pop_bytes, int nops);


/* write_ret_far_imm16
 *  Writes a "far return, pop imm16 bytes from stack" (RET imm16) instruction at the
 *	 specified write_to address.
 *
 *  @param write_to  The address where the RET imm16 instruction will be written.
 *  @param pop_bytes The number of bytes to pop from the stack before returning to the calling
 *					procedure.
 *  @param nops The number of NOP instructions to be written after the RET imm16 instruction.
 */
void write_ret_far_imm16(void *write_to, uint16_t pop_bytes, int nops);


/* calculate_jmp_offset
 *  Calculates the offset between a 'JMP rel' instruction and a target address.
 *
 *  @param from  	The address of the JMP instruction.
 *  @param to    	The address of the assembly code cave.
 *	@param instr_length   The length (in bytes) of the variation of JMP instruction being used
 */
uint32_t calculate_jmp_offset(void *from, void *to, int instr_length);


// Removes a previous injection entry
void forget_injection(uint32_t address);


#endif // ASM_INJECT_X86_HPP
