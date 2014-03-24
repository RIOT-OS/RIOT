CC = gcc
CFLAGS = -O
INCLUDES = ..
LIBS = ../lib
AR = ar
ARFLAGS = ru
RANLIB = ranlib
RM = rm
OUTPUT = ../lib/
TARGET = libtextui.a
OBJS = TextUIRunner.o XMLOutputter.o TextOutputter.o CompilerOutputter.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $(OUTPUT)$@ $(OBJS)
	$(RANLIB) $(OUTPUT)$@

.c.o:
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $<

TextUIRunner.o: TextUIRunner.h XMLOutputter.h TextOutputter.h CompilerOutputter.h Outputter.h
XMLOutputter.o: XMLOutputter.h Outputter.h
TextOutputter.o: TextOutputter.h Outputter.h
CompilerOutputter.o: CompilerOutputter.h Outputter.h

clean:
	-$(RM) $(TARGET) $(OBJS)

.PHONY: clean all
