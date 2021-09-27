APP = xorif-app

SRCS = xorif_app.c xorif_file.c xorif_socket.c xorif_command.c ecpri_proto.c ecpri.c
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
ifeq ($(BF),1)
CFLAGS += -I../libxobf -I=/usr/include/xobf
LDFLAGS += -L../libxobf
LDLIBS += -lxobf
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
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)

clean:
	rm -f $(APP) *.o *.d

%.o: %.c
	$(CC) $(CFLAGS) -c $< > $@
%.d: %.c
	$(CC) $(CFLAGS) -MM -MP $< > $@

-include $(DEPS)