# Add deprecated modules here
# Keep this list ALPHABETICALLY SORTED!!!!
ifeq ($(MAKELEVEL),0)
  ifneq (,$(filter $(DEPRECATED_CPUS),$(CPU)))
    $(call echoinfo,($(call c_red,"Deprecated CPU: ")"\"$(CPU)\""))
  endif
endif
