# Variables.
CXX=g++
CXXFLAGS = -D_LINUX -L../src/.libs -lckcore

# Targets.
all: clean test streambench

clean:
	rm -f bin/test bin/streambench test.cc

test:
	cxxtestgen.pl --error-printer -o test.cc convert.hh directory.hh file.hh path.hh stream.hh
	$(CXX) $(CXXFLAGS) test.cc -o bin/test

streambench:
	$(CXX) $(CXXFLAGS) streambench.cc -o bin/streambench

