#include <shell.h>

#ifdef MODULE_PS
#include <ps.h>
#endif

void shell_auto_init(shell_t *s) {
#ifdef MODULE_PS
shell_register_cmd(s, "ps", _ps_handler);
#endif
}
