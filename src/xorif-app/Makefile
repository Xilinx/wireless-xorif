APP = xorif-app

SRCS = xorif_app.c xorif_file.c xorif_menu.c xorif_socket.c xorif_command.c ecpri_proto.c ecpri.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
CFLAGS += -I. -I../libxorif -Werror -Wall -Wno-unused-function -g -DDEBUG
ifeq ($(NO_HW),1)
CFLAGS += -DNO_HW
else
LDFLAGS += -L../libxorif
LDLIBS += -lxorif
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