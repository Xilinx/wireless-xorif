APP = xorif-app

SRCS = xorif_app.c xorif_file.c xorif_socket.c xorif_command.c #ecpri_proto.c ecpri.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
CFLAGS += -I. -Werror -Wall -Wno-unused-function -std=gnu99 -g -DDEBUG

ifeq ($(BF),1)
CFLAGS += -DBF_INCLUDED
endif

ifeq ($(NO_HW),1)
CFLAGS += -DNO_HW
else
CFLAGS += -I../libxorif -I=/usr/include/xorif
LDFLAGS += -L../libxorif
LDLIBS += -lxorif
STATIC_LIBS += ../libxorif/libxorif.a
ifeq ($(BF),1)
CFLAGS += -I../libxobf -I=/usr/include/xobf
LDFLAGS += -L../libxobf
LDLIBS += -lxobf
STATIC_LIBS += ../libxobf/libxobf.a
endif
endif

ifeq ($(GCOV),1)
CFLAGS += -O0 -fprofile-arcs -ftest-coverage
LDFLAGS += --coverage
LDLIBS += -lgcov
endif

all: build

build: $(APP)

$(APP): $(OBJS)
ifeq ($(STATIC),1)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(STATIC_LIBS) -lm -lgcov
else
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)
endif

clean:
	rm -f $(APP) *.o *.d
	rm -f *.gcov *.gcda *.gcno

%.o: %.c
	$(CC) $(CFLAGS) -c $< > $@
%.d: %.c
	$(CC) $(CFLAGS) -MM -MP $< > $@

-include $(DEPS)