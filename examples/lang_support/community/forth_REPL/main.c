/*
 * Copyright (C) 2026 Francois Perrad
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "ficl.h"
#include "shell.h"

int main(void)
{
    char in[80] = { 0 };
    FICL_SYSTEM *pSys = ficlInitSystem(4096);
    FICL_VM *pVM = ficlNewVM(pSys);
    int ret = ficlEvaluate(pVM, ".ver 2 spaces .( " __DATE__ " ) cr");

    while (ret != VM_USEREXIT && shell_readline(in, sizeof(in)) >= 0)
    {
        ret = ficlExec(pVM, in);
    }

    ficlTermSystem(pSys);
    return 0;
}
