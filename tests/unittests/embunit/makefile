all:
	-@mkdir lib
	-@cd ./embUnit		;$(MAKE)
	-@cd ./tests		;$(MAKE)
	-@cd ./samples		;$(MAKE)
	-@cd ./tools		;$(MAKE)

test:
	-@./tests/embUnitTest

samples:
	-@./samples/samples

clean:
	-@cd ./embUnit		;$(MAKE) clean
	-@cd ./tests		;$(MAKE) clean
	-@cd ./samples		;$(MAKE) clean
	-@cd ./tools		;$(MAKE) clean

.PHONY: clean samples test all
