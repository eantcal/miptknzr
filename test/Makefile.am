SUBDIRS =
sbin_PROGRAMS = 

AM_CPPFLAGS = \
   -g \
   -I$(top_srcdir)/src 

test_CXXFLAGS = \
   -std=c++11 \
   -g 

test_SOURCES = \
   main.cc

AM_CXXFLAGS = ${test_CXXFLAGS}

test_LDADD = \
   -L../src/.libs/ -lmiptknzr

sbin_PROGRAMS += \
   test 

