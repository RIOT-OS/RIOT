#ifndef GDBSTUB_CFG_H
#define GDBSTUB_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enable own stack for gdbstub
 *
 * Enable this to make the exception and debugging handlers switch to a
 * private stack. This will use GDBSTUB_STACK_SIZE of RAM, but may be useful
 * if you're debugging stack or stack pointer corruption problems. It's
 * normally disabled because not many situations need it. If for some reason
 * the GDB communication stops when you run into an error in your code, try
 * enabling this.
 */
#ifndef GDBSTUB_USE_OWN_STACK
#define GDBSTUB_USE_OWN_STACK (0)
#endif

/**
 * @brief   Size of gdbstub stack
 *
 * If own stack is enale for gdbstub (\ref GDBSTUB_USE_OWN_STACK),
 * GDBSTUB_STACK_SIZE defines the size of this stack.
 */
#ifndef GDBSTUB_STACK_SIZE
#define GDBSTUB_STACK_SIZE  (1024)
#endif

/**
 * @brief   Enable Ctrl-C handling
 *
 * If Ctrl-C is enabled, gdbstub interrupts the program when you press the
 * Ctrl-C key either in gdb or, if used, in the console window. Ctrl-C handling
 * is realized by hooking the UART interrupt.
 */
#ifndef GDBSTUB_CTRLC_BREAK
#define GDBSTUB_CTRLC_BREAK (1)
#endif

/**
 * @brief   Redirect console output to GDB
 *
 * Enabling this will redirect console output to GDB. This basically means
 * that printf/os_printf output will show up in your gdb session, which is
 * useful if you use gdb to do stuff. It also means that if you use a normal
 * terminal, you can't read the printfs anymore.
 */
#ifndef GDBSTUB_REDIRECT_CONSOLE_OUTPUT
#define GDBSTUB_REDIRECT_CONSOLE_OUTPUT (1)
#endif

/**
 * @brief   Break on init
 *
 * Enable this if you want the GDB stub to wait for you to attach GDB before
 * running. It does this by breaking in the init routine; use the gdb 'c'
 * command (continue) to start the program.
 */
#ifndef GDBSTUB_BREAK_ON_INIT
#define GDBSTUB_BREAK_ON_INIT (1)
#endif

/**
 * @brief   Function attributes for function types.
 *
 * gdbstub functions are placed in flash or IRAM using attributes, as defined
 * here. The gdbinit function (and related) can always be in flash, because
 * it's called in the normal code flow. The rest of the gdbstub functions can
 * be in flash too, but only if there's no chance of them being called when the
 * flash somehow is disabled (eg during SPI operations or flash write/erase
 * operations). If the routines are called when the flash is disabled (e.g.
 * due to a Ctrl-C at the wrong time), the ESP8266 will most likely crash.
 */
#define ATTR_GDBINIT    ICACHE_FLASH_ATTR
#ifndef ATTR_GDBFN
#define ATTR_GDBFN
#endif

#ifdef __cplusplus
}
#endif

#endif /* GDBSTUB_CFG_H */
