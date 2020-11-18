LIB = xorif

MAJOR = 1.0
MINOR = 1
VERSION = $(MAJOR).$(MINOR)

SRCS = xorif_common.c xorif_fh_func.c xorif_utils.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
CFLAGS += -I. -Werror -Wall -g -DDEBUG -DENABLE_INTERRUPTS
LDFLAGS +=
LDLIBS += -lmetal

all: build

build: lib$(LIB).so xorif_api.txt

lib$(LIB).so: lib$(LIB).so.$(VERSION)
	rm -f lib$(LIB).so.$(MAJOR) lib$(LIB).so
	ln -s lib$(LIB).so.$(VERSION) lib$(LIB).so.$(MAJOR)
	ln -s lib$(LIB).so.$(MAJOR) lib$(LIB).so

lib$(LIB).so.$(VERSION): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) -shared -Wl,-soname,lib$(LIB).so.$(MAJOR) $(OBJS) $(LDLIBS)

xorif_api.txt: xorif_api.h
	$(CC) $(CFLAGS) -E -DCFFI_CDEF_HDR xorif_api.h > $@
	sed -i '/#/d' $@
	sed -i '/^$$/d' $@

clean:
	rm -f *.o *.d *.so *.so.*
	rm -f xorif_api.txt

%.o: %.c
	$(CC) $(CFLAGS) -c -fPIC $< > $@
%.d: %.c
	$(CC) $(CFLAGS) -MM -MP $< > $@

-include $(DEPS)