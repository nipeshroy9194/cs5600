CC = gcc
CFLAGS = -c -g -Wall -Werror -fno-stack-protector
CXXFLAGS = -o
SMESELECT = ./sme_select.c
SMEEPOLL = ./sme_epoll.c
SMESYS = ./sme_sys.c
OBJSEL = sme_select.o
OBJEPOLL = sme_epoll.o
OBJSYS = sme_sys.o
TARGETSME = libsme.a

all: $(TARGETSME)

$(OBJSYS): $(SMESYS)
	$(CC) $(CFLAGS) $(SMESYS) $(CXXFLAGS) $(OBJSYS)

$(OBJSEL): $(SMESELECT) $(SMESYS) sme.h sme_def.h sme_select.h list.h
	$(CC) $(CFLAGS) $(SMESELECT) $(CXXFLAGS) $(OBJSEL)

$(OBJEPOLL): $(SMEEPOLL) $(SMESYS) sme.h sme_def.h sme_epoll.h list.h
	$(CC) $(CFLAGS) $(SMEEPOLL) $(CXXFLAGS) $(OBJEPOLL)

$(TARGETSME): $(OBJSEL) $(OBJSYS) $(OBJEPOLL)
	ar -cvq $(TARGETSME) $(OBJSEL) $(OBJSYS) $(OBJEPOLL)

.PHONY: clean
clean:
	rm $(TARGETSME) $(OBJSEL) $(OBJSYS) $(OBJEPOLL)

