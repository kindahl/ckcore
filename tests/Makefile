# Variables.
CXX=g++
CXXFLAGS=-D_UNIX -L../src/.libs -lckcore -I../include

# Targets.
all: clean test streambench smallclient filetester

clean:
	rm -f bin/test bin/streambench test.cc

test:
	cxxtestgen.pl --error-printer -o test.cc convert.hh directory.hh file.hh linereader.hh path.hh process.hh stream.hh string.hh
	$(CXX) $(CXXFLAGS) test.cc -o bin/test

streambench:
	$(CXX) $(CXXFLAGS) streambench.cc -o bin/streambench

smallclient:
	$(CXX) $(CXXFLAGS) smallclient.cc -o bin/smallclient

filetester:
	$(CXX) $(CXXFLAGS) filetester.cc -o bin/filetester
