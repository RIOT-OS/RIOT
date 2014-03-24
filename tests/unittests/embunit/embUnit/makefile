CC = gcc
CFLAGS = -O
AR = ar
ARFLAGS = ru
RANLIB = ranlib
RM = rm
OUTPUT = ../lib/
TARGET = libembUnit.a
OBJS = AssertImpl.o RepeatedTest.o stdImpl.o TestCaller.o TestCase.o TestResult.o TestRunner.o TestSuite.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $(OUTPUT)$@ $(OBJS)
	$(RANLIB) $(OUTPUT)$@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

AssertImpl.o: AssertImpl.h stdImpl.h
RepeatedTest.o: RepeatedTest.h Test.h
stdImpl.o: stdImpl.h
TestCaller.o: TestCaller.h TestResult.h TestListener.h TestCase.h Test.h
TestCase.o: TestCase.h TestResult.h TestListener.h Test.h
TestResult.o: TestResult.h TestListener.h Test.h
TestRunner.o: TestRunner.h TestResult.h TestListener.h Test.h stdImpl.h config.h
TestSuite.o: TestSuite.h TestResult.h TestListener.h Test.h

clean:
	-$(RM) $(OBJS) $(TARGET)

.PHONY: clean all
