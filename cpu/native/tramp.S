.text

.extern $_native_saved_eip
.extern $_native_isr_ctx
.extern $_native_cur_ctx
.extern $_native_in_isr

.globl _native_sig_leave_tramp

_native_sig_leave_tramp:
    pushl %eax
    pushf
    pushl %ebp
    pushl %esp

    movl  %esp, %ebp
    subl  $24, %esp
    movl  $_native_isr_ctx, 4(%esp)
    movl  $_native_cur_ctx, (%esp)
    call swapcontext

    addl  $24, %esp
    popl %esp
    popl %ebp
    popf 
    popl %eax
    movl $0x0, _native_in_isr;

    jmp *_native_saved_eip
