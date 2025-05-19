# This file declares DEFAULT_MODULEs, which may only have modules as
# dependencies that themselves have no dependencies. DEFAULT_MODULEs that have
# dependencies must be delcared in defaultmodules_deps.inc.mk.
# DEFAULT_MODULEs declared in this file MAY be disabled during dependency
# resolution.

DEFAULT_MODULE += auto_init periph_init

DEFAULT_MODULE += periph_init_leds
DEFAULT_MODULE += periph_init_led0
DEFAULT_MODULE += periph_init_led1
DEFAULT_MODULE += periph_init_led2
DEFAULT_MODULE += periph_init_led3
DEFAULT_MODULE += periph_init_led4
DEFAULT_MODULE += periph_init_led5
DEFAULT_MODULE += periph_init_led6
DEFAULT_MODULE += periph_init_led7
