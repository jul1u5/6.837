SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:

TARGET    = a1

CPPFLAGS = -I ../vecmath

CXX      ?= clang++
CXXFLAGS ?= -O2 -Wall -pedantic -g

LDFLAGS = -L ../lib/vecmath
LDLIBS  = -lglut -lGL -lGLU

LDLIBS += -l:libvecmath.a

SRCS      = $(wildcard *.cpp)
OBJS      = $(SRCS:.cpp=.o)

all: $(SRCS) $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJS) $(TARGET)
