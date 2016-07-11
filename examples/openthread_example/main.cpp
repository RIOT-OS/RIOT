
#ifdef MODULE_OPENTHREAD_CLI
#include "ot-cli.h"
#endif

int main(void)
{
#ifdef MODULE_OPENTHREAD_CLI
    openthread_cli_run();
#endif
	return 0;
}
