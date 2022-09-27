# Add deprecated modules here
# Keep this list ALPHABETICALLY SORTED!!!!
DEPRECATED_MODULES += event_thread_lowest
DEPRECATED_MODULES += gnrc_netdev_default
DEPRECATED_MODULES += gnrc_pktbuf_cmd # use shell_cmd_gnrc_pktbuf instead
DEPRECATED_MODULES += gnrc_udp_cmd # use shell_cmd_grnc_udp instead
DEPRECATED_MODULES += heap_cmd # use shell_cmd_heap instead
DEPRECATED_MODULES += i2c_scan # use shell_cmd_i2c_scan instead
DEPRECATED_MODULES += md5sum # use shell_cmd_md5sum instead
DEPRECATED_MODULES += nice # use shell_cmd_nice instead
DEPRECATED_MODULES += random_cmd # use shell_cmd_random instead
DEPRECATED_MODULES += sema_deprecated
DEPRECATED_MODULES += shell_commands # use shell_cmds_default instead
DEPRECATED_MODULES += sha1sum # use shell_cmd_sha1sum instead
DEPRECATED_MODULES += sha256sum # use shell_cmd_sha256sum instead
DEPRECATED_MODULES += ztimer_now64

# Warn about used deprecated modules
DEPRECATED_MODULES_USED := $(sort $(filter $(DEPRECATED_MODULES),$(USEMODULE)))
ifneq (,$(DEPRECATED_MODULES_USED))
  $(shell $(COLOR_ECHO) "$(COLOR_RED)Deprecated modules are in use:$(COLOR_RESET)"\
                        "$(DEPRECATED_MODULES_USED)" 1>&2)
endif
