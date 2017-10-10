# Filename : Makefile
# Description : To build the checkpoint and restore process libraries 
#               and executables
#
# Author : Nipesh Roy <roy.nip@husky.neu.edu>
# Date : Sept 11 2017

# Compiler flags
CC = gcc
CFLAGS = -c -g -fPIC -Wall -Werror -fno-stack-protector
C2FLAGS = -g -static -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000  -fno-stack-protector
CXXFLAGS = -o
LDFLAGS = -shared

#Source files
SRCSAMP = ./source/hello.c
SRCCKPT = ./source/checkpoint.c
SRCREST = ./source/myrestart.c
SRCEXEC = ./source/exec_cmd.c
SRCMEM = ./source/memory_map.c
SRCHELPER = ./source/helper_func.c

#Headers
HEADER1 = ./include/memory_map.h
HEADER2 = ./include/common_func.h
HEADER3 = ./include/helper_func.h

#Object files
OBJSAMP = hello.o
OBJCKPT = ckpt.o
OBJREST = myrestart.o
OBJHELPER = helper_func.o
OBJMEM = memory_map.o
OBJEXEC = exec_cmd.o

#Shared library
TARGET = libckpt.so

#Output checkpoint file
OUTFILE = myckpt

#Rules
all : $(TARGET) $(OBJSAMP) $(OBJREST)

$(OBJSAMP): $(SRCSAMP)
	$(CC) $(SRCSAMP) $(CXXFLAGS) $(OBJSAMP)

$(TARGET): $(OBJCKPT) $(OBJEXEC) $(OBJMEM) $(OBJHELPER)
	$(CC) $(LDFLAGS) $(CXXFLAGS) $(TARGET) $(OBJCKPT) $(OBJEXEC) $(OBJMEM) $(OBJHELPER)

$(OBJREST) : $(SRCREST) $(OBJEXEC) $(OBJMEM) $(OBJHELPER)	
	$(CC) $(C2FLAGS) $(SRCREST) $(CXXFLAGS) $(OBJREST) $(OBJEXEC) $(OBJMEM) $(OBJHELPER)

$(OBJCKPT): $(SRCCKPT) $(HEADER1) $(HEADER2) $(HEADER3)
	$(CC) $(CFLAGS) $(SRCCKPT) $(CXXFLAGS) $(OBJCKPT) 

$(OBJEXEC): $(SRCEXEC) $(HEADER2) $(OBJMEM) $(OBJHELPER)
	$(CC) $(CFLAGS) $(SRCEXEC) $(CXXFLAGS) $(OBJEXEC)

$(OBJMEM): $(SRCMEM) $(HEADER1) $(OBJHELPER)
	$(CC) $(CFLAGS) $(SRCMEM) $(CXXFLAGS) $(OBJMEM)

$(OBJHELPER): $(SRCHELPER) $(HEADER3)
	$(CC) $(CFLAGS) $(SRCHELPER) $(CXXFLAGS) $(OBJHELPER)

#Clean routine
.PHONY: clean
clean:
	rm $(TARGET) $(OBJCKPT) $(OBJREST)\
		$(OBJEXEC) $(OBJHELPER) $(OBJMEM) $(OBJSAMP) $(OUTFILE) 