# Variables.
CXX=g++
CXXFLAGS = -D_LINUX -L../src/.libs -lckcore

# Targets.
all: clean file

clean:
	rm -f bin/file file.cc

file:
	cxxtestgen.pl --error-printer -o file.cc file.hh
	$(CXX) $(CXXFLAGS) file.cc -o bin/file

#g++ -D_LINUX -L../src/.libs -lckcore file.cc -o file

