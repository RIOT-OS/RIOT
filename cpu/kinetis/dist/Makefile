wdog-disable.bin: wdog-disable.o
	arm-none-eabi-objcopy -O binary -j .text.wdog_disable -S -g $^ $@

wdog-disable.o: wdog-disable.s
	arm-none-eabi-as -mthumb -o $@ $^

.PHONY: clean
clean:
	@rm wdog-disable.o
