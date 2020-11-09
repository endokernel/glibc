#ifndef _SYSCALL_TRAP_H_
#define _SYSCALL_TRAP_H_

#ifdef __ASSEMBLER__
# ifdef SHARED
.weak syscall_trap
.type syscall_trap, @function

#  if defined(PSEUDO) && defined(SYSCALL_NAME) && defined(SYSCALL_SYMBOL)
#ifdef R_IV
#   define SYSCALL_TRAP                  \
    subq $128, %rsp;                    \
    callq *syscall_trap@GOTPCREL(%rip); \
    addq $128, %rsp
#else   // #ifdef R_IV
#   define SYSCALL_TRAP                  \
    subq $128, %rsp;                    \
    callq *syscall_trap@GOTPCREL(%rip);
#endif  // #ifdef R_IV
#  else
#ifdef R_IV
#   define SYSCALL_TRAP                  \
    callq *syscall_trap@GOTPCREL(%rip)
#else    // #ifdef R_IV
#   define SYSCALL_TRAP                  \
    subq $128, %rsp;                    \
    callq *syscall_trap@GOTPCREL(%rip)
#endif  // #ifdef R_IV
#  endif
# else /* !SHARED */
#   define SYSCALL_TRAP syscall
# endif

#else /* !__ASSEMBLER__ */
# ifdef SHARED
asm (
".weak syscall_trap\r\n"
".type syscall_trap, @function\r\n");

#ifdef R_IV
#  define SYSCALL_TRAP                           \
    "subq $128, %%rsp\n\t"                     \
    "callq *syscall_trap@GOTPCREL(%%rip)\n\t"  \
    "addq $128, %%rsp\n\t"
#else    // #ifdef R_IV
#  define SYSCALL_TRAP                           \
    "subq $128, %%rsp\n\t"                     \
    "callq *syscall_trap@GOTPCREL(%%rip)\n\t"
#endif    // #ifdef R_IV

#ifdef R_IV
#  define SYSCALL_TRAP_ASM                       \
    "callq *syscall_trap@GOTPCREL(%rip)\n\t"
#else    // #ifdef R_IV
#  define SYSCALL_TRAP_ASM                       \
    "subq $128, %rsp\n\t"                       \
    "callq *syscall_trap@GOTPCREL(%rip)\n\t"
#endif    // #ifdef R_IV

# else /* !SHARED */
#  define SYSCALL_TRAP     "syscall\n\t"
#  define SYSCALL_TRAP_ASM "syscall\n\t"
# endif

#endif /* Assembler */

#endif /* _SYSCALL_TRAP_H_ */
