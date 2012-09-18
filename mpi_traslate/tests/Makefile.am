TESTS = check_traslate check_traslate_common
check_PROGRAMS = check_traslate check_traslate_common

check_traslate_SOURCES = check_traslate.c
check_traslate_CFLAGS = -fno-stack-protector -Wall -D'DEBUG' @CHECK_CFLAGS@
check_traslate_LDADD = @CHECK_LIBS@ -lm

check_traslate_common_SOURCES = check_traslate_common.c
check_traslate_common_CFLAGS = -fno-stack-protector -Wall -D'DEBUG' @CHECK_CFLAGS@
check_traslate_common_LDADD = @CHECK_LIBS@ -lm
