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

depend:
	makedepend $(CPPFLAGS) -Y $(SRCS)

clean:
	rm -f $(OBJS) $(TARGET)
# DO NOT DELETE

main.o: ../include/vecmath/vecmath.h ../include/vecmath/Matrix2f.h
main.o: ../include/vecmath/Matrix3f.h ../include/vecmath/Matrix4f.h
main.o: ../include/vecmath/Quat4f.h ../include/vecmath/Vector2f.h
main.o: ../include/vecmath/Vector3f.h ../include/vecmath/Vector4f.h
