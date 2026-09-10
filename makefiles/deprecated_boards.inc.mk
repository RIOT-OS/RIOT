# Add deprecated modules here
# Keep this list ALPHABETICALLY SORTED!!!!
ifeq ($(MAKELEVEL),0)
  ifneq (,$(filter $(DEPRECATED_BOARDS),$(BOARD)))
    $(call echoinfo,($(call c_red,"Deprecated board: ")"\"$(BOARD)\""))
  endif
endif
