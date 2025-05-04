# This file declare DEFAULT_MODULEs that MAY only have as dependecies modules
# with no dependencies themselfs. If they modules declared here have dependencies
# they must be delcared in defaultmodules_deps.inc.mk.
# These DEFAULT_MODULEs MAY be disabled during dependency resolution

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
DEFAULT_MODULE += periph_init_buttons
