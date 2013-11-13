.text

#ifdef __MACH__
.globl __native_sig_leave_tramp
__native_sig_leave_tramp:
    pushf
    pusha

    pushl __native_isr_ctx
    pushl __native_cur_ctx
    call _swapcontext
    addl $8, %esp

    movl $0x0, __native_in_isr
    popa
    popf

    jmp *__native_saved_eip
#else
.extern $_native_saved_eip
.extern $_native_isr_ctx
.extern $_native_cur_ctx
.extern $_native_in_isr

.globl _native_sig_leave_tramp

_native_sig_leave_tramp:
    pushf
    pusha

    pushl _native_isr_ctx
    pushl _native_cur_ctx
    call swapcontext
    addl $8, %esp

    call eINT

    movl $0x0, _native_in_isr
    popa
    popf

    jmp *_native_saved_eip
#endif
