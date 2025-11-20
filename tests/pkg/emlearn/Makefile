include ../Makefile.pkg_common

USEPKG += emlearn

BLOBS += digit

include $(RIOTBASE)/Makefile.include

model_head.h:
	$(Q)echo "#ifndef MODEL_H" > model_head.h
	$(Q)echo "#define MODEL_H" >> model_head.h
	$(Q)echo "#ifdef __cplusplus" >> model_head.h
	$(Q)echo "extern \"C\" {" >> model_head.h
	$(Q)echo "#endif" >> model_head.h

model_tail.h:
	$(Q)echo "#ifdef __cplusplus" > model_tail.h
	$(Q)echo "}" >> model_tail.h
	$(Q)echo "#endif" >> model_tail.h
	$(Q)echo "#endif /* MODEL_H */" >> model_tail.h

model.h: $(CURDIR)/model model_head.h model_tail.h
	$(Q)$(CURDIR)/generate_model.py
	$(Q)sed -i model.h -e 's/[\t ]*$$//g'
	$(Q)cat model_head.h model.h model_tail.h > tmp.h
	$(Q)mv tmp.h model.h
