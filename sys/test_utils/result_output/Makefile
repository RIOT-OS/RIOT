MODULE = test_utils_result_output

ifneq (,$(filter test_utils_result_output_json,$(USEMODULE)))
  DIRS += json
endif

ifneq (,$(filter test_utils_result_output_txt,$(USEMODULE)))
  DIRS += txt
endif

ifneq (,$(filter test_utils_result_output_check,$(USEMODULE)))
  DIRS += check
endif

include $(RIOTBASE)/Makefile.base
