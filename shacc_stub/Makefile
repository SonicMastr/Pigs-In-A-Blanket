LIBSGEN := $(SDKPREFIX)-libs-gen

all: out/libSceShaccCg_stub.a

out/libSceShaccCg_stub.a: out/Makefile
	$(MAKE) -C out

out/Makefile:
	$(LIBSGEN) nids.yml out

clean:
	$(RM) -r out

.PHONY: clean
