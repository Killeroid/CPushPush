check_PROGRAMS=testpush parse_hook test_name

EXTRA_DIST=Ackermann2.push fib.push run_tests.sh

INCLUDES = -I$(top_builddir)/src

TESTS=run_tests.sh parse_hook test_name

testpush_SOURCES=testpush.cpp
testpush_LDADD=$(top_builddir)/src/libpush.la 

parse_hook_SOURCES=parse_hook.cpp
parse_hook_LDADD=$(top_builddir)/src/libpush.la

test_name_SOURCES=test_name.cpp
test_name_LDADD=$(top_builddir)/src/libpush.la


