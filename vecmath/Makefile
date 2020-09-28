SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:

CXX      ?= clang++
CXXFLAGS ?= -O3 -fPIC -Wall -Wextra -pedantic

LDFLAGS   = -shared

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

LIBDIR ?= ../lib/$(NAME)

NAME    := $(shell basename $(CURDIR))
SNAME    = lib$(NAME).a
DNAME    = lib$(NAME).so

all: $(SNAME) $(DNAME)

$(SNAME): $(OBJS)
	mkdir -p $(LIBDIR)
	ar -rs $(LIBDIR)/$@ $^

$(DNAME): $(OBJS)
	$(CXX) $(LDFLAGS) -Wl,-soname,$@ -o $(LIBDIR)/$@ $^

.PHONY: clean
clean:
	$(RM) $(OBJS)
	$(RM) -r $(LIBDIR)
