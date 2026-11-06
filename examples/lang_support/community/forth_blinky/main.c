/*
 * SPDX-FileCopyrightText: 2026 Francois Perrad
 * SPDX-License-Identifier: LGPL-2.1-only
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
