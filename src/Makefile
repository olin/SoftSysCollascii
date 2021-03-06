CC=gcc
CFLAGS+=-Wall -Wpedantic -std=gnu99 -no-pie

version = $(shell git describe --always --tags --dirty)
CFLAGS+="-DVERSION=\"$(version)\""

# add debug annotations, turn off optimizations, and #define DEBUG
# use in bash with `DEBUG=1 make <foo>`
# use in fish with `env DEBUG=1 make <foo>`
ifdef DEBUG
CFLAGS+=-g -O0 -DDEBUG
endif

ifdef PRODUCTION
# set warnings as errors
CFLAGS+=-Werror
endif

## EXAMPLES

# link ncurses library
# foo: LDLIBS+=-lncurses

# require library canvas.c for foo
# foo.out: canvas.o

## FILE-SPECIFIC MODIFICATIONS

# the main executable
collascii: frontend.out
	mv frontend.out collascii

frontend.out: LDLIBS +=-lncurses -lm
frontend.out: cursor.o fe_modes.o canvas.o view.o network.o lib/argtable3.o

server.out: LDLIBS +=-lpthread
server.out: canvas.o

## PATTERNS

# Generate .out files for easier .gitignore. Based on the builtin make rule
# https://www.gnu.org/software/make/manual/html_node/Catalogue-of-Rules.html#Catalogue-of-Rules
%.out: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)

# **GNU Make only** run all test files (any file ending in _test.c)
# turns each `foo_test.c` into `.run-foo_test.c` with "Text Functions"
test: $(patsubst %.c, .run-%.c, $(wildcard *_test.c))

# run a single file
# given `.run-foo.c`, requires `foo.c` and runs `./foo`
.run-%.c: %
	./$<

# add flags for minunit libraries, works with `make test_foo` too
%_test: LDLIBS+=-lrt -lm
# require foo.c, foo_test, and minunit.h for foo_test
%_test: %.o %_test.c lib/minunit.h
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	-rm *.o *_test *.out collascii
