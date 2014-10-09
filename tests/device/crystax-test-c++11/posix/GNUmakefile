include ../common.mk

CXX = g++

define is-cxx-clang
$(shell $(CXX) --version | grep -iq clang && echo yes)
endef

CXXFLAGS := -std=gnu++11 $(if $(call is-cxx-clang),,-pthread) $(CFLAGS)

VPATH := ../jni

OBJFILES := $(addprefix obj/,$(patsubst %.cpp,%.o,$(SRCFILES)))
TARGET := bin/test-c++11

.PHONY: run-tests
run-tests: $(TARGET)
	$<

.PHONY: clean
clean:
	rm -Rf bin obj

bin:
	mkdir -p bin

obj:
	mkdir -p obj

$(TARGET): $(OBJFILES) | bin
	$(CXX) $(CXXFLAGS) -o $@ $^

obj/%.o: %.cpp | obj
	$(CXX) $(CXXFLAGS) -c -o $@ $^
