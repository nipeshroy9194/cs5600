CC = gcc
CFLAGS = -g -Wall -Werror -fno-stack-protector
CXXFLAGS = -o
SRCWORKER = worker.c
SRCMASTER = master.c
TARGET = ../worker
TARGET1 = ../master
STATICSME = ../sme/libsme.a

all: $(TARGET) $(TARGET1)

$(TARGET): $(SRCWORKER)
	$(CC) $(CFLAGS) $(SRCWORKER) $(CXXFLAGS) $(TARGET)

$(TARGET1): $(SRCMASTER)
	$(CC) $(CFLAGS) $(SRCMASTER) $(CXXFLAGS) $(TARGET1) $(STATICSME)

.PHONY: clean
clean:
	rm $(TARGET) $(TARGET1)

