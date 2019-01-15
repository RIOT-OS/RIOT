RFBASE ?= $(RIOTBASE)/dist/robotframework
RFPYPATH ?= $(APPDIR)/tests:$(RFBASE)/lib:$(RFBASE)/res:$(RIOTBASE)/dist/tests
RFOUTPATH ?= $(BUILD_DIR)/robot/$(BOARD)/$(APPLICATION)/

ROBOT_FILES ?= $(wildcard tests/*.robot)

robot-test:
	$(call check_cmd,robot,RobotFramework tool)
ifneq (,$(ROBOT_FILES))
	python3 -m robot.run \
				--name $(APPLICATION) \
				--noncritical warn-if-failed \
				--settag "APP_$(APPLICATION)" \
				--settag "BOARD_$(BOARD)" \
				--metadata RIOT-Version:$(RIOT_VERSION) \
				--metadata RIOT-Board:$(BOARD) \
				--metadata RIOT-Application:$(APPLICATION) \
				--xunit xunit.xml \
				-P "$(RFPYPATH)" \
				-d $(RFOUTPATH) \
				$(ROBOT_FILES)
else
	@echo No RobotFramework test scripts found, no worries.
endif
