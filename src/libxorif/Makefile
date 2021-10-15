LIB = xorif

MAJOR = 1.0
MINOR = 1
VERSION = $(MAJOR).$(MINOR)

SRCS = xorif_common.c xorif_fh_func.c xorif_utils.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
CFLAGS += -I. -Werror -Wall -std=gnu99 -g -DDEBUG
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

ifeq ($(DEBUG),1)
CFLAGS += -DEXTRA_DEBUG
endif

all: build

build: lib$(LIB).so lib$(LIB).a xorif_api.txt

lib$(LIB).so: lib$(LIB).so.$(VERSION)
	rm -f lib$(LIB).so.$(MAJOR) lib$(LIB).so
	ln -s lib$(LIB).so.$(VERSION) lib$(LIB).so.$(MAJOR)
	ln -s lib$(LIB).so.$(MAJOR) lib$(LIB).so

lib$(LIB).so.$(VERSION): $(OBJS) linker.script
	$(CC) -o $@ $(LDFLAGS) -shared -Wl,-soname,lib$(LIB).so.$(MAJOR) $(OBJS) $(LDLIBS)

lib$(LIB).a: $(OBJS)
	ar -crs $@ $(OBJS)

xorif_api.txt: xorif_api.h
	$(CC) $(CFLAGS) -E -DCFFI_CDEF_HDR xorif_api.h > $@
	sed -i '/#/d' $@
	sed -i '/^$$/d' $@

linker.script:
	echo "{ global: xorif*; local: *; };" > $@

clean:
	rm -f *.o *.d *.so *.so.* *.a
	rm -f *.gcov *.gcda *.gcno
	rm -f xorif_api.txt
	rm -f linker.script

%.o: %.c
	$(CC) $(CFLAGS) -c -fPIC $< > $@
%.d: %.c
	$(CC) $(CFLAGS) -MM -MP $< > $@

-include $(DEPS)