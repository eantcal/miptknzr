SUBDIRS =
sbin_PROGRAMS = 

AM_CPPFLAGS = \
   -g \
   -I$(top_srcdir)/include

json_parser_test_CXXFLAGS = \
   -std=c++14 \
   -g 

json_parser_test_SOURCES = \
   json_parser.cc mip_json.cc

AM_CXXFLAGS = ${json_parser_test_CXXFLAGS}

json_parser_test_LDADD = \
   -L../lib/.libs/ -lmiptknzr

sbin_PROGRAMS += \
   json_parser_test 

