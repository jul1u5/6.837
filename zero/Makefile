SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:

CXX      ?= g++
CXXFLAGS ?= -O2 -Wall -pedantic -g
SRCS      = main.cpp
OBJS      = $(SRCS:.cpp=.o)
TARGET    = a0

CPPFLAGS = -I ../include/vecmath

LDFLAGS = -L ../lib/vecmath
LDLIBS  = -lglut -lGL -lGLU

# Dynamic Linking
# LDFLAGS += -Wl,-rpath='$$ORIGIN/../lib/vecmath'
# LDLIBS  += -lvecmath

# Static Linking
LDLIBS += -l:libvecmath.a

all: $(SRCS) $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJS) $(TARGET)
