AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = luogu3c
lib_LTLIBRARIES = libluogu3.la
nobase_include_HEADERS = luogu3/compile.hpp luogu3/diagnostic.hpp luogu3/program.hpp
libluogu3_la_SOURCES = luogu3/compile.cpp luogu3/diagnostic.cpp
luogu3c_SOURCES = argagg/argagg.hpp luogu3c.cpp
luogu3c_LDADD = libluogu3.la
AM_CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
