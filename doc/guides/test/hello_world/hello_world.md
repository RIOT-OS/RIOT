---
title: Test Code Block CI
description: Test Code Block CI
code_folder: examples/basic/hello-world/
---

# Test Code Block CI

```c
int main(void)
{
    puts("Hello World!");
```

# Skip CI

<!--skip ci-->
```c
int not_main(void)
{
    puts("Goodbye World!");
```

# Non C Code

```bash
cowsay "Hello World!"
```

# Non C Code with Skip CI

<!--skip ci-->
```bash
cowsay "Goodbye World!"
```

# Second C Code Block

```c
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s CPU.\n", RIOT_CPU);
```
