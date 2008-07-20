# Variables.
CXX=g++
CXXFLAGS = -D_LINUX -L../src/.libs -lckcore

# Targets.
all: clean test

clean:
	rm -f bin/file file.cc

test:
	cxxtestgen.pl --error-printer -o test.cc directory.hh file.hh path.hh
	$(CXX) $(CXXFLAGS) test.cc -o bin/test

