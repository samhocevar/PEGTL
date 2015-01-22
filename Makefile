# Copyright (c) 2008 Dr. Colin Hirsch

CXX=g++-mp-4.3

CPPFLAGS=-std=gnu++0x -I. -D_REENTRANT

CXXFLAGS=-pipe -O3 -Wall -Wextra -Wimplicit -Wconversion -Wcast-align -Woverloaded-virtual -Wold-style-cast -Wformat=2 -Wswitch-enum -Wswitch-default -Wredundant-decls -fno-enforce-eh-specs -fno-strict-overflow -lpthread

.PHONY: t1 all test clean

t1:
	@echo
	@echo "PEGTL Makefile"
	@echo 
	@echo "  make all   # to build everything"
	@echo "  make test  # to run the unit tests"
	@echo "  make clean # to clean generated files"
	@echo
	@echo "Please customise Makefile before building!"
	@echo

SOURCES:=$(wildcard *.cc)
DEPENDS:=$(SOURCES:.cc=.d)
PROGRAMS:=$(SOURCES:.cc=)

all: $(PROGRAMS)

test: testcases
	./testcases 2> /dev/null

clean:
	rm -f $(DEPENDS) $(PROGRAMS)

.SECONDARY:

%.d: %.cc
	$(CXX) $(CPPFLAGS) -MM -MT $@ $< -o $@

%: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

ifeq ($(findstring $(MAKECMDGOALS),clean),)
ifneq ($(MAKECMDGOALS),)
-include $(DEPENDS)
endif
endif
