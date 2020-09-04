SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:

CXX      ?= g++
CPPFLAGS = -I ../include/vecmath

CXXFLAGS ?= -O3 -fPIC -Wall -Wextra -pedantic
LDFLAGS   = -shared

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

NAME    := $(shell basename $(CURDIR))
SNAME    = lib$(NAME).a
DNAME    = lib$(NAME).so

LIBDIR ?= ../lib/$(NAME)

all: $(SNAME) $(DNAME)

$(SNAME): $(OBJS)
	mkdir -p $(LIBDIR)
	ar rcs $(LIBDIR)/$@ $^

$(DNAME): $(OBJS)
	$(CXX) $(LDFLAGS) -Wl,-soname,$@ -o $(LIBDIR)/$@ $^

.PHONY: clean
clean:
	$(RM) $(OBJS)
	$(RM) -r $(LIBDIR)
