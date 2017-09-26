.PHONY: all
all:
	$(MAKE) -C sme
	$(MAKE) -C test
	$(MAKE) -C source

.PHONY: clean
clean:
	$(MAKE) -C sme clean
	$(MAKE) -C test clean
	$(MAKE) -C source clean
