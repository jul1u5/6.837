SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:

CXX      ?= g++
CXXFLAGS ?= -O3 -Wall -Wextra -pedantic -fPIC

SRCS = Vector2f.cpp Vector3f.cpp Vector4f.cpp Matrix2f.cpp Matrix3f.cpp Matrix4f.cpp Quat4f.cpp
OBJS = $(SRCS:.cpp=.o)

INCDIR ?= ../include/vecmath
LIBDIR ?= ../lib/vecmath

MAJOR    = 1
MINOR    = 1
VERSION := $(MAJOR):$(MINOR)

all: headers static shared

static: $(OBJS)
	mkdir -p $(LIBDIR)
	ar rcs $(LIBDIR)/libvecmath.a $(OBJS)

shared: $(OBJS)
	$(CXX) -shared -Wl,-soname,libvecmath.so.$(MAJOR) -o $(LIBDIR)/libvecmath.so.$(VERSION) $(OBJS)
	cd $(LIBDIR)
	ln -sf libvecmath.so.$(VERSION) $(LIBDIR)/libvecmath.so
	ln -sf libvecmath.so.$(VERSION) $(LIBDIR)/libvecmath.so.$(MAJOR)

headers:
	mkdir -p $(INCDIR)
	cp *.h $(INCDIR)

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -rf $(INCDIR) $(LIBDIR)
