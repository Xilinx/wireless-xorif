APP = xorif-app

SRCS = xorif_app.c xorif_file.c xorif_socket.c xorif_interactive.c xorif_parser.c xorif_command.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
CFLAGS += -I. -Werror -Wall -Wno-unused-function -std=gnu99 -g -DDEBUG

BF ?= 0
OCP ?= 0
OPRACH ?= 0

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

dev-build:
	make -B build
	make -B -C ../libxorif dev-build
	cp ../libxorif/*.so.1 .
	cp ../libxorif/xorif_api_cffi.h .
	cp ../libxorif/pylibxorif.py .
ifeq ($(BF),1)
	make -B -C ../libxobf dev-build NUM_BF=1
	cp ../libxobf/*.so.1 .
	cp ../libxobf/xobf_api_cffi.h .
endif
ifeq ($(OCP),1)
	cp ../libxorif/xocp_api_cffi.h .
	cp ../libxorif/pylibxocp.py .
endif
ifeq ($(OCP),2)
	make -B -C ../libxocp dev-build
	cp ../libxocp/*.so.1 .
	cp ../libxocp/xocp_api_cffi.h .
	cp ../libxocp/pylibxocp.py .
endif
ifeq ($(OPRACH),1)
	make -B -C ../libxoprach dev-build
	cp ../libxoprach/*.so.1 .
	cp ../libxoprach/xoprach_api_cffi.h .
	cp ../libxoprach/pylibxoprach.py .
endif

test:
	coverage run -a -m pytest -v test_app.py --app_type=0 --bf=$(BF) --ocp=$(OCP) --oprach=$(OPRACH)
ifneq ($(BF),1)
	coverage run -a -m pytest -v test_app.py --app_type=1 --bf=$(BF) --ocp=$(OCP) --oprach=$(OPRACH)
endif

coverage:
	$(MAKE) -B dev-build test GCOV=1 OCP=1 OPRACH=1
	gcovr --json run-1.json
	$(MAKE) -B dev-build test GCOV=1 BF=1
	gcovr --json run-2.json
	gcovr -a "run-*.json" --txt --html-details coverage.html
	coverage report
	coverage html
	cppcheck --error-exitcode=1 --force *.c

clean:
	rm -f xorif-app xnraas-app
	rm -f libxo.so*
	rm -f pylib*.py
	rm -f *_cffi.h
	rm -f $(APP) *.o *.d
	rm -f *.gcov *.gcda *.gcno
	rm -f .coverage
	rm -rf htmlcov
	rm -rf coverage.css
	rm -f coverage.html
	rm -f coverage.*.html
	rm -f run-*.json

%.o: %.c
	$(CC) $(CFLAGS) -c $< > $@
%.d: %.c
	$(CC) $(CFLAGS) -MM -MP $< > $@

-include $(DEPS)
