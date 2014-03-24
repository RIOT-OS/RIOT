CC = gcc
CFLAGS = -O
INCLUDES = ..
LIBS = ../lib
RM = rm
TARGET = samples
OBJS = AllTests.o counter.o counterTest.o person.o personTest.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) -L$(LIBS) -lembUnit

.c.o:
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $<

clean:
	-$(RM) $(TARGET) $(OBJS)

.PHONY: clean all
