/* GCC ARM assembler */

  .text
  .code 32
  .align 4  /* 0 */

/*  .extern  sched_run*/

/* Public functions declared in this file */
  .global  atomic_set_return

.func
atomic_set_return:
    SWP r2,r1,[r0]
    MOV r0, r2
    mov pc, lr
.endfunc

