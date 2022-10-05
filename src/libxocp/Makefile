LIB = xocp

MAJOR = 1
MINOR = 1
VERSION = $(MAJOR).$(MINOR)

SRCS = xocp.c xocp_registers.c xocp_utils.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
CFLAGS += -I. -Werror -Wall -std=gnu99 -g -DDEBUG
CFLAGS += $(EXTRA_FLAGS)
LDFLAGS += -Wl,--version-script linker.script
LDLIBS += -lm

ifeq ($(NO_HW),1)
CFLAGS += -DNO_HW
else
LDLIBS += -lmetal
endif

ifeq ($(GCOV),1)
CFLAGS += -O0 -fprofile-arcs -ftest-coverage
LDFLAGS += --coverage
LDLIBS += -lgcov
endif

all: build

build: lib$(LIB).so lib$(LIB).a xocp_api_cffi.h

lib$(LIB).so: lib$(LIB).so.$(VERSION)
	rm -f lib$(LIB).so.$(MAJOR) lib$(LIB).so
	ln -s lib$(LIB).so.$(VERSION) lib$(LIB).so.$(MAJOR)
	ln -s lib$(LIB).so.$(MAJOR) lib$(LIB).so

lib$(LIB).so.$(VERSION): $(OBJS) linker.script
	$(CC) -o $@ $(LDFLAGS) -shared -Wl,-soname,lib$(LIB).so.$(MAJOR) $(OBJS) $(LDLIBS)

lib$(LIB).a: $(OBJS)
	ar -crs $@ $(OBJS)

xocp_api_cffi.h: xocp_api.h
	$(CC) $(CFLAGS) -E -DCFFI_CDEF_HDR -fdirectives-only -nostdinc xocp_api.h > $@
	sed -i '/# /d' $@
	sed -i '/#define\s*__/d' $@
	sed -i '/#define\s*_/d' $@
	sed -i '/#undef\s*__/d' $@
	sed -i '/#undef\s*_/d' $@
	sed -i '/#define\s*\w*\s*$$/d' $@
	sed -i '/#define\s*CFFI_CDEF_HDR/d' $@
	sed -i '/^$$/d' $@

linker.script:
	echo "{ global: xocp*; local: *; };" > $@

clean:
	rm -f *.o *.d *.so *.so.* *.a
	rm -f xocp_api_cffi.h
	rm -f linker.script
	rm -f *.gcov *.gcda *.gcno
	rm -f .coverage
	rm -rf htmlcov
	rm -rf coverage.css
	rm -f coverage.html
	rm -f coverage.*.html
	rm -f run-*.json

test:
	coverage run -a -m pytest -v test_*.py

dev-build:
	$(MAKE) build NO_HW=1 GCOV=1 EXTRA_FLAGS="-DEXTRA_DEBUG"

coverage:
	$(MAKE) -B build test NO_HW=1 GCOV=1 EXTRA_FLAGS="-DEXTRA_DEBUG"
	gcovr --json run-1.json
	gcovr -a "run-*.json" --txt --html-details coverage.html
	coverage report
	coverage html
	cppcheck --error-exitcode=1 --force *.c

%.o: %.c
	$(CC) $(CFLAGS) -c -fPIC $< > $@
%.d: %.c
	$(CC) $(CFLAGS) -MM -MP $< > $@

-include $(DEPS)
