include ../Makefile.pkg_common

USEPKG += emlearn

BLOBS += digit

BUILDDEPS += model.h

include $(RIOTBASE)/Makefile.include

model.h: $(CURDIR)/model
	$(Q)$(CURDIR)/generate_model.py
	$(Q)echo "/* fix for no newline at eof */" >> model.h
