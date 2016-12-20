SUBDIRS =
sbin_PROGRAMS = 

AM_CPPFLAGS = \
   -g \
   -I$(top_srcdir)/include

json_parser_test_CXXFLAGS = \
   -std=c++11 \
   -g 

json_parser_test_SOURCES = \
   json_parser.cc

AM_CXXFLAGS = ${json_parser_test_CXXFLAGS}

json_parser_test_LDADD = \
   -L../lib/.libs/ -lmiptknzr

sbin_PROGRAMS += \
   json_parser_test 

