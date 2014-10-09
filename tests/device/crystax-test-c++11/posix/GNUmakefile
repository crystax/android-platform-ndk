include ../common.mk

CXX = g++

CXXFLAGS := -std=gnu++11 $(CFLAGS)

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
