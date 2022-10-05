APP = xorif-app

SRCS = xorif_app.c xorif_file.c xorif_socket.c xorif_interactive.c xorif_parser.c xorif_command.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
CFLAGS += -I. -Werror -Wall -Wno-unused-function -std=gnu99 -g -DDEBUG

ifeq ($(BF),1)
CFLAGS += -DBF_INCLUDED
endif

ifeq ($(SRS),1)
CFLAGS += -DSRS_INCLUDED
endif

ifeq ($(EXAMPLE),1)
CFLAGS += -DEXAMPLE_INCLUDED
endif

ifeq ($(NO_HW),1)
CFLAGS += -DNO_HW
else
CFLAGS += -I../libxorif -I=/usr/include/xorif
LDFLAGS += -L../libxorif
LDLIBS += -lxorif
STATIC_LIBS += ../libxorif/libxorif.a
ifeq ($(BF),1)
CFLAGS += -I../libxobf -I=/usr/include/xorif
LDFLAGS += -L../libxobf
LDLIBS += -lxobf
STATIC_LIBS += ../libxobf/libxobf.a
endif
ifeq ($(SRS),1)
CFLAGS += -I../libxsrs -I=/usr/include/xsrs
LDFLAGS += -L../libxsrs
LDLIBS += -lxsrs
STATIC_LIBS += ../libxsrs/libxsrs.a
endif
endif

ifeq ($(GCOV),1)
CFLAGS += -O0 -fprofile-arcs -ftest-coverage
LDFLAGS += --coverage
LDLIBS += -lgcov
endif

ifeq ($(DEBUG),1)
CFLAGS += -DEXTRA_DEBUG
endif

ifeq ($(XNRAAS_APP),1)
APP = xnraas-app
CFLAGS += -DXNRAAS_APP
ifneq ($(NO_HW),1)
ifeq ($(EXAMPLE),1)
#SRCS += example_code.c
CFLAGS += -I../example -I=/usr/include/example
LDFLAGS += -L../example
LDLIBS += -lexample
STATIC_LIBS += ../libexample/libexample.a
endif
endif
endif

all: build

build: $(APP)

XORIF_APP:
	$(MAKE) -B APP=xorif-app

XNRAAS_APP:
	$(MAKE) -B APP=xnraas-app XNRAAS_APP=1 BF=1

$(APP): $(OBJS)
ifeq ($(STATIC),1)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(STATIC_LIBS) -lm -lgcov
else
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)
endif

clean:
	rm -f xorif-app xnraas-app
	rm -f $(APP) *.o *.d
	rm -f *.gcov *.gcda *.gcno

%.o: %.c
	$(CC) $(CFLAGS) -c $< > $@
%.d: %.c
	$(CC) $(CFLAGS) -MM -MP $< > $@

-include $(DEPS)
