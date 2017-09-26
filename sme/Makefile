CC = gcc
CFLAGS = -c -g -Wall -Werror -fno-stack-protector
CXXFLAGS = -o
SMESELECT = ./sme_select.c
SMESYS = ./sme_sys.c
OBJSEL = sme_select.o
OBJSYS = sme_sys.o
TARGETSME = libsme.a

all: $(TARGETSME)

$(OBJSYS): $(SMESYS)
	$(CC) $(CFLAGS) $(SMESYS) $(CXXFLAGS) $(OBJSYS)

$(OBJSEL): $(SMESELECT) $(SMESYS) sme.h sme_def.h sme_select.h list.h
	$(CC) $(CFLAGS) $(SMESELECT) $(CXXFLAGS) $(OBJSEL)

$(TARGETSME): $(OBJSEL) $(OBJSYS)
	ar -cvq $(TARGETSME) $(OBJSEL) $(OBJSYS)

.PHONY: clean
clean:
	rm $(TARGETSME) $(OBJSEL) $(OBJSYS)

