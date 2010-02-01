CLIB_CFLAGS = -ccopt "$(shell pkg-config --cflags libudev)"
OCAMLC ?= ocamlc
OCAMLOPT ?= ocamlopt
OCAMLMKLIB ?= ocamlmklib

CLIB_LDFLAGS = -cclib "" $(shell pkg-config --libs libudev)

OCAMLOPTFLAGS =
OCAML_PKG_NAME = udev

OCAMLABI := $(shell $(OCAMLC) -version)
OCAMLLIBDIR := $(shell $(OCAMLC) -where)
OCAMLDESTDIR ?= $(OCAMLLIBDIR)

OCAML_TEST_INC = -I `ocamlfind query oUnit`
OCAML_TEST_LIB = `ocamlfind query oUnit`/oUnit.cmxa

CHECK_PKGS = libudev

INTERFACES = udev.cmi udev.mli
LIBS_NAT = udev.cmxa
LIBS_BYTE = udev.cma
LIBS = $(LIBS_BYTE) $(LIBS_NAT)
PROGRAMS = test

all: $(INTERFACES) $(LIBS_NAT) $(LIBS_BYTE)

all-opt: all

all-byte: $(INTERFACES) $(LIBS_BYTE)

bins: $(PROGRAMS)

libs: $(LIBS)

udev.cmxa: libudev_stubs.a udev_stubs.a udev.cmx
	$(OCAMLOPT) $(OCAMLOPTFLAGS) -a -cclib -ludev_stubs -cclib -ludev -o $@ udev.cmx

udev.cma: libudev_stubs.a udev.cmi udev.cmo
	$(OCAMLC) -a -dllib dlludev_stubs.so -cclib -ludev_stubs -cclib -ludev -o $@ udev.cmo

udev_stubs.a: libudev_stubs.a

libudev_stubs.a: udev_stubs.o
	$(OCAMLMKLIB) -o udev_stubs $(CLIB_LDFLAGS) $+

%.cmo: %.ml
	$(OCAMLC) -c -o $@ $<

%.cmi: %.mli
	$(OCAMLC) -c -o $@ $<

%.cmx: %.ml
	$(OCAMLOPT) $(OCAMLOPTFLAGS) -c -o $@ $<

%.o: %.c
	$(OCAMLC) $(CLIB_CFLAGS) -c -o $@ $<

.PHONY: check
check:
	$(foreach pkg, $(CHECK_PKGS), \
		@pkg-config --modversion $(pkg) > /dev/null 2>&1 || \
			(echo "$(pkg) package not found" > /dev/stderr && exit 1))

.PHONY: install
install: $(LIBS)
	ocamlfind install -destdir $(OCAMLDESTDIR) -ldconf ignore $(OCAML_PKG_NAME) META $(INTERFACES) $(LIBS) *.a *.so *.cmx

install-opt: install

install-byte: all-byte
	ocamlfind install -destdir $(OCAMLDESTDIR) -ldconf ignore $(OCAML_PKG_NAME) META $(INTERFACES) $(LIBS_BYTE) *.a *.so

uninstall:
	ocamlfind remove -destdir $(OCAMLDESTDIR) $(OCAML_PKG_NAME)

test: libudev.cmxa test.ml
	$(OCAMLOPT) -o $@ -cclib -L. unix.cmxa $+

clean:
	rm -f *.o *.so *.a *.cmo *.cmi *.cma *.cmx *.cmxa $(LIBS) $(PROGRAMS)
