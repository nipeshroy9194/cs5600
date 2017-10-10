CC = gcc
CFLAGS = -g -Wall -Werror -fno-stack-protector
CXXFLAGS = -o
SMEECHO = ./echo.c
TARGET = ../sme/libsme.a
TARGET2 = ./echo
all:  $(TARGET2)

$(TARGET2): $(SMEECHO) $(TARGET)
	$(CC) $(CFLAGS) $(SMEECHO) $(CXXFLAGS) $(TARGET2) $(TARGET)

.PHONY: clean
clean:
	rm $(TARGET2)

