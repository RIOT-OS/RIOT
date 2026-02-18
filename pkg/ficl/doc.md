<!--
SPDX-FileCopyrightText: 2026 Francois Perrad
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup pkg_ficl ficl
@ingroup  pkg
@brief    a lightweight, embeddable scripting language designed to be embedded.
@see      https://github.com/jwsadler58/ficl
@see      https://ficl.sourceforge.net/

# Forth programming language support

## Introduction

Ficl (Forth-inspired command language) is an ANS Forth
interpreter written in C. Unlike traditional Forths, this
interpreter is designed to be embedded into other systems
as a command/macro/development prototype language.

## Typical use

```c
    char in[80] = { 0 };
    FICL_SYSTEM *pSys = ficlInitSystem(4096);
    FICL_VM *pVM = ficlNewVM(pSys);
    int ret = ficlEvaluate(pVM, ".ver 2 spaces .( " __DATE__ " ) cr");

    while (ret != VM_USEREXIT && shell_readline(in, sizeof(in)) >= 0) {
        ret = ficlExec(pVM, in);
    }

    ficlTermSystem(pSys);
```
