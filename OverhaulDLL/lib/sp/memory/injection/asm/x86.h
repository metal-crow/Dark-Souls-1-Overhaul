/*
    CONTRIBUTORS:
        Sean Pesce

    WARNING: This code is extremely old and probably has many issues.

    @TODO: Fix up this sub-library with abstractions etc.
*/

#pragma once

#ifndef SP_UTILITY_ASSEMBLY_INJECTION_X86_H_
#define SP_UTILITY_ASSEMBLY_INJECTION_X86_H_

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4311 4302) // Disable truncation warnings when compiling for x64
#endif // _MSC_VER

#include "sp/sp.h"
#include "sp/memory.h"


__SP_NAMESPACE
namespace mem  {
namespace code {
namespace x86  {


const uint8_t NOP_INSTR_OPCODE = 0x90,           // NOP instruction byte value
              JMP_REL8_INSTR_OPCODE = 0xEB,      // JMP rel8 opcode byte value
              JMP_REL32_INSTR_OPCODE = 0xE9,     // JMP rel32 opcode byte value
              CALL_REL32_INSTR_OPCODE = 0xE8,    // CALL rel32 opcode byte value
              RET_INSTR_OPCODE = 0xC3,           // Near return (RET) instruction byte value
              RET_FAR_INSTR_OPCODE = 0xCB,       // Far return (RET) instruction byte value
              RET_IMM16_INSTR_OPCODE = 0xC2,     // Near return, pop imm16 bytes from stack (RET imm16) opcode value
              RET_FAR_IMM16_INSTR_OPCODE = 0xCA; // Far return, pop imm16 bytes from stack (RET imm16) opcode value
//
const int     JMP_REL8_INSTR_LENGTH = 2,         // Length (in bytes) of 'JMP rel8' instruction
              JMP_REL32_INSTR_LENGTH = 5,        // Length (in bytes) of 'JMP rel32' instruction
              CALL_REL32_INSTR_LENGTH = 5,       // CALL rel32 instruction
              RET_INSTR_LENGTH = 1,              // RET instruction
              RET_IMM16_INSTR_LENGTH = 3;        // RET imm16 instruction



// Offset from function start address when jumping to assembly code:
// (can be changed to accommodate different compilers and/or calling conventions)
#ifdef _MSC_VER
#define SP_ASM_FUNC_START_OFFSET_X86 0 // Microsoft compiler; jump straight to injected function
#else
#define SP_ASM_FUNC_START_OFFSET_X86 3 // Non-MS compiler; GCC in-line ASM starts +3 bytes from asm_code
#endif // _MSC_VER



/* calculate_jmp_offset
 *  Calculates the offset between a 'JMP rel' instruction and a target address.
 *
 *  @param from          Address of the JMP instruction.
 *  @param to            Address of the assembly code cave.
 *  @param instr_length  Length (in bytes) of the variation of JMP instruction being used
 */
inline uint32_t calculate_jmp_offset(void* from, void* to, unsigned int instr_length)
{
    return ((uint32_t)to - (uint32_t)from - instr_length);
}



/* write_jmp_rel8
 *  Writes a JMP rel8 instruction at the specified write_to address.
 *
 *  @param write_to  The address where the JMP rel8 instruction will be written.
 *  @param jmp_to    The address where the JMP rel8 instruction will jump to. After the JMP rel8
 *                  instruction executes, %eip = jmp_to.
 *                  NOTE: jmp_to must be in the range [write_to-128,write_to+127]
 *  @param nops The number of NOP instructions to be written after the JMP rel8 instruction.
 */
inline void write_jmp_rel8(void* write_to, void *jmp_to, int nops)
{
    *(uint8_t*)write_to = JMP_REL8_INSTR_OPCODE; // Write opcode byte
    *((uint8_t*)write_to + 1) = (int8_t)calculate_jmp_offset(write_to, jmp_to, JMP_REL8_INSTR_LENGTH); // Write operand byte

    // Erase trailing garbage bytes from overwritten instruction(s) at write address:
    memset((void*)((uint8_t*)write_to + JMP_REL8_INSTR_LENGTH), NOP_INSTR_OPCODE, nops);
}



/* write_jmp_rel8
 *  Writes a JMP rel8 instruction at the specified write_to address using the given offset.
 *
 *  @param write_to  The address where the JMP rel8 instruction will be written.
 *  @param offset   The offset that the JMP rel8 instruction will jump to. After the JMP rel8
 *                  instruction executes, %eip = write_to+offset+2.
 *                  NOTE: offset must be in the range [write_to-128,write_to+127]
 *  @param nops The number of NOP instructions to be written after the JMP rel8 instruction.
 */
inline void write_jmp_rel8(void* write_to, int8_t offset, int nops)
{
    *(uint8_t*)write_to = JMP_REL8_INSTR_OPCODE; // Write opcode byte
    *((int8_t*)write_to + 1) = offset; // Write operand byte

    // Erase trailing garbage bytes from overwritten instruction(s) at write address:
    memset((void*)((uint8_t*)write_to + JMP_REL8_INSTR_LENGTH), NOP_INSTR_OPCODE, nops);
}



/* write_jmp_rel32
 *  Writes a JMP rel32 instruction at the specified write_to address.
 *
 *  @param write_to  The address where the JMP rel32 instruction will be written.
 *  @param jmp_to    The address where the JMP rel32 instruction will jump to. After the JMP rel32
 *                  instruction executes, %eip = jmp_to.
 *                  NOTE: jmp_to must be in the range [write_to-2³¹,write_to+2³¹-1]
 *  @param nops The number of NOP instructions to be written after the JMP rel32 instruction.
 */
inline void write_jmp_rel32(void* write_to, void *jmp_to, int nops)
{
    *(uint8_t*)write_to = JMP_REL32_INSTR_OPCODE; // Write opcode byte
    *(uint32_t*)((uint8_t*)write_to + 1) = (int32_t)calculate_jmp_offset(write_to, jmp_to, JMP_REL32_INSTR_LENGTH); // Write operand bytes

    // Erase trailing garbage bytes from overwritten instruction(s) at write address:
    memset((void*)((uint8_t*)write_to + JMP_REL32_INSTR_LENGTH), NOP_INSTR_OPCODE, nops);
}



/* write_jmp_rel32
 *  Writes a JMP rel32 instruction at the specified write_to address using the given offset.
 *
 *  @param write_to  The address where the JMP rel32 instruction will be written.
 *  @param offset   The offset that the JMP rel32 instruction will jump to. After the JMP rel32
 *                  instruction executes, %eip = write_to+offset+5.
 *                  NOTE: offset must be in the range [write_to-2³¹,write_to+2³¹-1]
 *  @param nops The number of NOP instructions to be written after the JMP rel32 instruction.
 */
inline void write_jmp_rel32(void* write_to, int32_t offset, int nops)
{
    *(uint8_t*)write_to = JMP_REL32_INSTR_OPCODE; // Write opcode byte
    *(int32_t*)((uint8_t*)write_to + 1) = offset; // Write operand bytes

    // Erase trailing garbage bytes from overwritten instruction(s) at write address:
    memset((void*)((uint8_t*)write_to + JMP_REL32_INSTR_LENGTH), NOP_INSTR_OPCODE, nops);
}



/* write_call_rel32
 *  Writes a CALL rel32 instruction at the specified write_to address.
 *
 *  @param write_to      The address where the CALL rel32 instruction will be written.
 *  @param procedure    The address of the called procedure. After the CALL rel32
 *                      instruction executes, %eip = procedure.
 *                      NOTE: procedure must be in the range [write_to-2³¹,write_to+2³¹-1]
 *  @param nops     The number of NOP instructions to be written after the CALL rel32 instruction.
 */
inline void write_call_rel32(void* write_to, void *procedure, int nops)
{
    *(uint8_t*)write_to = CALL_REL32_INSTR_OPCODE; // Write opcode byte
    *(uint32_t*)((uint8_t*)write_to + 1) = (int32_t)calculate_jmp_offset(write_to, procedure, CALL_REL32_INSTR_LENGTH); // Write operand bytes

    // Erase trailing garbage bytes from overwritten instruction(s) at write address:
    memset((void*)((uint8_t*)write_to + CALL_REL32_INSTR_LENGTH), NOP_INSTR_OPCODE, nops);
}



/* write_call_rel32
 *  Writes a CALL rel32 instruction at the specified write_to address using the given offset.
 *
 *  @param write_to  The address where the CALL rel32 instruction will be written.
 *  @param offset   The offset of the called procedure. After the CALL rel32
 *                  instruction executes, %eip = write_to+offset+5.
 *                  NOTE: offset must be in the range [write_to-2³¹,write_to+2³¹-1]
 *  @param nops The number of NOP instructions to be written after the CALL rel32 instruction.
 */
inline void write_call_rel32(void* write_to, int32_t offset, int nops)
{
    *(uint8_t*)write_to = CALL_REL32_INSTR_OPCODE; // Write opcode byte
    *(int32_t*)((uint8_t*)write_to + 1) = offset; // Write operand bytes

    // Erase trailing garbage bytes from overwritten instruction(s) at write address:
    memset((void*)((uint8_t*)write_to + CALL_REL32_INSTR_LENGTH), NOP_INSTR_OPCODE, nops);
}



/* write_ret
 *  Writes a "near return" (RET) instruction at the specified write_to address.
 *
 *  @param write_to  The address where the RET instruction will be written.
 *  @param nops The number of NOP instructions to be written after the RET instruction.
 */
inline void write_ret(void* write_to, int nops)
{
    *(uint8_t*)write_to = RET_INSTR_OPCODE; // Write instruction

    // Erase trailing garbage bytes from overwritten instruction at write address:
    memset((void*)((uint8_t*)write_to + RET_INSTR_LENGTH), NOP_INSTR_OPCODE, nops);
}



/* write_ret_far
 *  Writes a "far return" (RET) instruction at the specified write_to address.
 *
 *  @param write_to  The address where the RET instruction will be written.
 *  @param nops The number of NOP instructions to be written after the RET instruction.
 */
inline void write_ret_far(void* write_to, int nops)
{
    *(uint8_t*)write_to = RET_FAR_INSTR_OPCODE; // Write instruction

    // Erase trailing garbage bytes from overwritten instruction at write address:
    memset((void*)((uint8_t*)write_to + RET_INSTR_LENGTH), NOP_INSTR_OPCODE, nops);
}



/* write_ret_imm16
 *  Writes a "near return, pop imm16 bytes from stack" (RET imm16) instruction at the
 *     specified write_to address.
 *
 *  @param write_to  The address where the RET imm16 instruction will be written.
 *  @param pop_bytes The number of bytes to pop from the stack before returning to the calling
 *                    procedure.
 *  @param nops The number of NOP instructions to be written after the RET imm16 instruction.
 */
inline void write_ret_imm16(void* write_to, uint16_t pop_bytes, int nops)
{
    *(uint8_t*)write_to = RET_IMM16_INSTR_OPCODE; // Write opcode byte
    *(uint16_t*)((uint8_t*)write_to + 1) = pop_bytes; // Write operand bytes

    // Erase trailing garbage bytes from overwritten instruction at write address:
    memset((void*)((uint8_t*)write_to + RET_IMM16_INSTR_LENGTH), NOP_INSTR_OPCODE, nops);
}



/* write_ret_far_imm16
 *  Writes a "far return, pop imm16 bytes from stack" (RET imm16) instruction at the
 *     specified write_to address.
 *
 *  @param write_to  The address where the RET imm16 instruction will be written.
 *  @param pop_bytes The number of bytes to pop from the stack before returning to the calling
 *                    procedure.
 *  @param nops The number of NOP instructions to be written after the RET imm16 instruction.
 */
inline void write_ret_far_imm16(void* write_to, uint16_t pop_bytes, int nops)
{
    *(uint8_t*)write_to = RET_FAR_IMM16_INSTR_OPCODE; // Write opcode byte
    *(uint16_t*)((uint8_t*)write_to + 1) = pop_bytes; // Write operand bytes

    // Erase trailing garbage bytes from overwritten instruction at write address:
    memset((void*)((uint8_t*)write_to + RET_IMM16_INSTR_LENGTH), NOP_INSTR_OPCODE, nops);
}




/* inject_jmp_5b
 *  Injects assembly code at the specified location in memory.
 *        NOTE:   Memory at the injection point must be writable before calling this function (use
 *                SET_MEM_PROTECTION).
 *        NOTE:   There must be AT LEAST 5 bytes of memory at the injection point that is either
 *                unused or that can be overwritten without disrupting program logic; this space
 *                is necessary to write the JMP instruction that directs execution to the injected
 *                assembly code.
 *
 *  @param inject_at      Location in memory where the assembly code will be injected. A
 *                          JMP instruction will be written at this location, containing the
 *                          address of the assembly function referenced by the asm_code parameter.
 *  @param returnJumpAddr Location in memory where the code cave should return to
 *                          after execution. returnJumpAddr should point to a uin32_t that
 *                          is referenced in a JMP instruction at the end of the assembly
 *                          function referenced by the asm_code parameter.
 *  @param nops           Number of NOP instructions to be written after the initial JMP
 *                          instruction. These NOPs will erase any remaining garbage bytes
 *                          that resulted from overwriting existing instructions at the injection
 *                          location.
 *  @param asm_code       Pointer to an in-line assembly function (to be used as a code cave).
 */
inline void inject_jmp_5b(uint8_t* inject_at, uint32_t* returnJumpAddr, int nops, void* asm_code)
{
    // Remove memory protections
#ifdef _WIN32
    sp::mem::set_protection(inject_at, 5+nops, MEM_PROTECT_RWX);
#else // UNIX
    sp::mem::set_protection(inject_at, 2, MEM_PROTECT_RWX);
#endif // _WIN32
#ifdef _MSC_VER
    // Using a Microsoft compiler; jump straight to the code cave:
    write_jmp_rel32(inject_at, asm_code, nops);
#else
    // Using non-MS compiler; GCC ASM starts +3 bytes from &asm_code:
    write_jmp_rel32(inject_at, (uint8_t*)asm_code + SP_ASM_FUNC_START_OFFSET_X86, nops);
#endif // _MSC_VER

    // Calculate the address of the next instruction after the injected JMP and write it to the in-line ASM function's return JMP:
    *returnJumpAddr = ((uint32_t)inject_at + JMP_REL32_INSTR_LENGTH);
}


} // namespace x86
} // namespace code
} // namespace mem
__SP_NAMESPACE_CLOSE // namespace sp


#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER

#endif // SP_UTILITY_ASSEMBLY_INJECTION_X86_H_
