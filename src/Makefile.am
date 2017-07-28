bin_PROGRAMS = runpush
lib_LTLIBRARIES=libpush.la 

EXTRA_DIST=*.h

#AM_CXXFLAGS=-g 
AM_CXXFLAGS=-O2 -DNDEBUG -fomit-frame-pointer #-fpermissive

# set the include path found by configure
INCLUDES= $(all_includes)

libpush_la_SOURCES=Instructions.cpp Env.cpp Code.cpp CodeUtils.cpp CodeOps.cpp StaticInit.cpp Type.cpp Word.cpp ExecInstructions.cpp IntegerInstructions.cpp FloatInstructions.cpp BooleanInstructions.cpp CodeInstructions.cpp NameInstructions.cpp EnvInstructions.cpp  ../extension/pushC.cpp CompiledCode.cpp

runpush_SOURCES = runpush.cpp
runpush_LDFLAGS = -lpush
