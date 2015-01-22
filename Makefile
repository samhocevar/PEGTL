# Copyright (c) 2008 Dr. Colin Hirsch

CXX=g++-mp-4.3

CPPFLAGS=-Iinclude -std=gnu++0x -D_REENTRANT

CXXFLAGS=-O3 -g -Wall -Wextra -Wimplicit -Wconversion -Wcast-align -Woverloaded-virtual -Wold-style-cast -Wformat=2 -Wswitch-enum -Wswitch-default -Wredundant-decls -fno-enforce-eh-specs -fno-strict-overflow -lpthread

.PHONY: clean message

SOURCES:=$(wildcard */*.cc)
DEPENDS:=$(SOURCES:.cc=.d)
PROGRAMS:=$(SOURCES:.cc=)

all: message $(PROGRAMS)
	@echo "Executing PEGTL unit tests."
	@example/unittests 2> /dev/null

clean:
	rm -f $(DEPENDS) $(PROGRAMS)

message:
	@echo
	@echo "Please customise Makefile before building!"
	@echo
	@echo "Compiling PEGTL unit tests and examples."
	@echo

.SECONDARY:

%.d: %.cc
	$(CXX) $(CPPFLAGS) -MM -MT $@ $< -o $@

%: %.cc %.d
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(DEPENDS)
endif
