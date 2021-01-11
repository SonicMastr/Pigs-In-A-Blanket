LIBSGEN := $(SDKPREFIX)-libs-gen

all: out/liblibScePiglet.a

out/liblibScePiglet.a: out/Makefile
	$(MAKE) -C out

out/Makefile:
	$(LIBSGEN) nids.yml out

clean:
	$(RM) -r out

.PHONY: clean
