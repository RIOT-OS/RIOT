DEFAULT_MODULE += board cpu core core_init core_msg core_panic sys

DEFAULT_MODULE += auto_init

# Initialize all used peripherals by default
DEFAULT_MODULE += periph_init

# Handle inclusion of DEFAULT_MODULEs once
USEMODULE := $(USEMODULE) $(filter-out $(DISABLE_MODULE), $(DEFAULT_MODULE))
