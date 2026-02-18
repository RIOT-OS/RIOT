/*
 * SPDX-FileCopyrightText: 2026 Francois Perrad
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "ficl.h"
#include "shell.h"

int main(void)
{
    char in[80] = { 0 };
    FICL_SYSTEM *pSys = ficlInitSystem(4096);
    FICL_VM *pVM = ficlNewVM(pSys);
    int ret = ficlEvaluate(pVM, ".ver 2 spaces .( " __DATE__ " ) cr");

    while (ret != VM_USEREXIT && shell_readline(in, sizeof(in)) >= 0) {
        ret = ficlExec(pVM, in);
    }

    ficlTermSystem(pSys);
    return 0;
}
