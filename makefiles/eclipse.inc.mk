# Generate an XML file containing all macro definitions and include paths for
# use in Eclipse CDT
.PHONY: eclipsesym eclipsesym.xml
eclipsesym: $(CURDIR)/eclipsesym.xml
eclipsesym.xml: eclipsesym

$(CURDIR)/eclipsesym.xml: FORCE
	$(Q)printf "%s\n" $(CC) $(CFLAGS_WITH_MACROS) $(INCLUDES) | \
		$(RIOTTOOLS)/eclipsesym/cmdline2xml.sh > $@
