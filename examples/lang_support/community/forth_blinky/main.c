/*
 * Copyright (C) 2026 Francois Perrad
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "ficl.h"

#include "blob/main.f.h"

int main(void)
{
    FICL_SYSTEM *pSys = ficlInitSystem(4096);
    FICL_VM *pVM = ficlNewVM(pSys);
    ficlEvaluate(pVM, (char *)main_f);
    return 0;
}
