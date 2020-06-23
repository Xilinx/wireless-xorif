LIB = xorif

MAJOR = 0
MINOR = 1
REV = 0
VERSION = $(MAJOR).$(MINOR).$(REV)

SRCS = xorif_common.c xorif_fh_func.c xorif_utils.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
CFLAGS += -I. -Werror -Wall -g -DDEBUG
LDFLAGS +=
LDLIBS += -lmetal

all: build

build: lib$(LIB).so

lib$(LIB).so: lib$(LIB).so.$(VERSION)
	rm -f lib$(LIB).so.$(MAJOR) lib$(LIB).so
	ln -s lib$(LIB).so.$(VERSION) lib$(LIB).so.$(MAJOR)
	ln -s lib$(LIB).so.$(MAJOR) lib$(LIB).so

lib$(LIB).so.$(VERSION): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) -shared -Wl,-soname,lib$(LIB).so.$(MAJOR) $(OBJS) $(LDLIBS)

clean:
	rm -f *.o *.d *.so *.so.*

%.o: %.c
	$(CC) $(CFLAGS) -c -fPIC $< > $@
%.d: %.c
	$(CC) $(CFLAGS) -MM -MP $< > $@

-include $(DEPS)