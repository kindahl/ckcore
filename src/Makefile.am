EXTRA_DIST = bufferedstream.hh convert.hh crcstream.hh directory.hh file.hh \
			 filestream.hh path stream.hh string.hh system.hh types.hh
SUBDIRS = linux

lib_LTLIBRARIES = libckcore.la

libckcore_la_SOURCES = linux/directory.cc linux/file.cc bufferedstream.cc \
					   convert.cc crcstream.cc filestream.cc path.cc \
					   stream.cc string.cc system.cc

library_includedir = $(includedir)/ckcore
library_include_HEADERS = bufferedstream.hh convert.hh crcstream.hh \
						  directory.hh file.hh filestream.hh path.hh \
						  stream.hh string.hh system.hh types.hh

