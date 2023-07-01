# Add features provided by modules
#
# This is done *before* the regular dependency resolution in Makefile.dep.
# Hence, an application requiring to provide a feature via a module must do
# so early on

ifneq (,$(filter soft_spi_as_periph_spi,$(USEMODULE)))
  FEATURES_PROVIDED += periph_spi
endif
