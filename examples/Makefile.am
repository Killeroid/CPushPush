bin_PROGRAMS=pushgp_int 

EXTRA_DIST=test.config test.inputs inputs 

#CXXFLAGS=-g
AM_CXXFLAGS=-O2 -DNDEBUG -fpermissive#-fomit-frame-pointer 

INCLUDES = -I$(top_builddir)/src

pushgp_int_SOURCES=pushgp_int.cpp
pushgp_int_LDADD=$(top_builddir)/src/libpush.la #$(top_builddir)/src/libpush-ext.la

#pushpop_ramp_SOURCES=pushpop_ramp.cpp
#pushpop_ramp_LDADD=$(top_builddir)/src/libpush.la #$(top_builddir)/src/libpush-ext.la

