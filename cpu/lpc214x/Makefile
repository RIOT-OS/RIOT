SRC = blocklist.c kernel_init.c msg.c scheduler.c blocklist_malloc.c ktimer.c mutex.c thread.c clist.c queue.c
OBJ = $(SRC:%.c=../bin/%.o)

INCLUDES = -Iinclude/ -I../sys/include -I../sys/lib -I../sys/drivers/include -I../cpu/$(CPU)/include/

ifeq ($(CPU),lpc2387)
 INCLUDES += -I../cpu/arm_common/include/
 CPUINCLUDE = -includecpu-conf.h
endif
ifeq ($(CPU),lpc214x)
 INCLUDES += -I../cpu/arm_common/include/
endif

../bin/core.a: $(OBJ)
	$(AR) rcs ../bin/core.a $(OBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
../bin/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(CPUINCLUDE) -c $*.c -o ../bin/$*.o
	$(CC) $(CFLAGS) $(INCLUDES) $(CPUINCLUDE) -MM $*.c > ../bin/$*.d
	@printf "../bin/"|cat - ../bin/$*.d > /tmp/fw_out && mv /tmp/fw_out ../bin/$*.d

# remove compilation products
clean:
	rm -f ../bin/core.a ../bin/*.o ../bin/*.d




