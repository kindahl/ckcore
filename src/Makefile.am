EXTRA_DIST = ../include/ckcore/bufferedstream.hh ../include/ckcore/convert.hh \
			 ../include/ckcore/crcstream.hh ../include/ckcore/directory.hh \
             ../include/ckcore/dynlib.hh ../include/ckcore/file.hh \
			 ../include/ckcore/filestream.hh ../include/ckcore/log.hh \
			 ../include/ckcore/nullstream.hh ../include/ckcore/path.hh \
			 ../include/ckcore/process.hh ../include/ckcore/progress.hh \
			 ../include/ckcore/progresser.hh ../include/ckcore/stream.hh \
			 ../include/ckcore/string.hh ../include/ckcore/system.hh \
			 ../include/ckcore/thread.hh ../include/ckcore/types.hh
INCLUDES = -I../include
SUBDIRS = unix

lib_LTLIBRARIES = libckcore.la

libckcore_la_SOURCES = unix/directory.cc unix/file.cc unix/process.cc \
					   unix/thread.cc \
					   bufferedstream.cc convert.cc crcstream.cc dynlib.cc \
					   filestream.cc log.cc nullstream.cc path.cc \
					   progresser.cc stream.cc string.cc system.cc
libckcore_la_LDFLAGS = -version-info $(CKCORE_VERSION)

library_includedir = $(includedir)/ckcore
library_include_HEADERS = ../include/ckcore/bufferedstream.hh \
						  ../include/ckcore/convert.hh \
						  ../include/ckcore/crcstream.hh \
						  ../include/ckcore/directory.hh \
						  ../include/ckcore/dynlib.hh \
						  ../include/ckcore/file.hh \
						  ../include/ckcore/filestream.hh \
						  ../include/ckcore/linereader.hh \
						  ../include/ckcore/log.hh \
						  ../include/ckcore/nullstream.hh \
						  ../include/ckcore/path.hh \
						  ../include/ckcore/process.hh \
						  ../include/ckcore/progress.hh \
						  ../include/ckcore/progresser.hh \
						  ../include/ckcore/stream.hh \
						  ../include/ckcore/string.hh \
						  ../include/ckcore/system.hh \
						  ../include/ckcore/thread.hh \
						  ../include/ckcore/types.hh

