include ../common.mk

VPATH := ../jni

OBJFILES := $(addprefix obj/,$(patsubst %.c,%.o,$(SRCFILES)))

.PHONY: run-tests
run-tests: bin/test-stdio
	$<

.PHONY: clean
clean:
	rm -Rf test-stdio obj

bin:
	mkdir bin

obj:
	mkdir obj

bin/test-stdio: $(OBJFILES) | bin
	gcc -o $@ $^

obj/%.o: %.c | obj
	gcc -c -o $@ $^
