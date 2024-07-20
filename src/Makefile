GCC = gcc
CFLAGS = -c #-Wall -Werror -Wextra -std=c11
LDFLAGS = -Wall -Werror -Wextra -std=c11
SOURCE = struct.c filereader.c
HEADER = struct.h filereader.h
OBJECTS = $(patsubst %.c,%.o, ${SOURCE})
GIFFLAGS = -I/Users/$(USER)/.brew/include -L/Users/$(USER)/.brew/lib -lpng -ljpeg -lgd
GTKFLAGS = `pkg-config --libs --cflags gtk+-3.0`
TESTS_SOURCE = test.c
TESTS_OBJECTS = $(patsubst %.c,%.o, ${TESTS_SOURCE})
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    OPEN_CMD = xdg-open
	ADD_LIB = -lcheck -lsubunit -lm -lrt -lpthread -D_GNU_SOURCE
	BREW = .
endif
ifeq ($(UNAME_S),Darwin)
	OPEN_CMD = open
	ADD_LIB =
	BREW := ${HOME}/homebrew
endif

PATH := $(BREW)/bin:$(PATH)

all: clean viewer_app

dvi:
	open dvi.html

dist:
	rm -rf viewer_1_0/
	mkdir viewer_1_0/
	mkdir viewer_1_0/src
	cp Makefile *.c *.h viewer_1_0/src/
	cp dvi.html viewer_1_0/src/	
	tar cvzf ../viewer_1_0.tgz viewer_1_0/
	rm -rf viewer_1_0/

install:
ifeq ($(OS), Linux)
	sudo apt update
	sudo apt-get install libgtk-3-dev
else
	brew install libgd	
	brew install gtk+3
endif

uninstall:
ifeq ($(OS), Linux)
	sudo apt-get purge libgtk-3-dev
else
	brew uninstall gtk+3
	brew uninstall libgd
endif

viewer.a: ${SOURCE}
	$(GCC) $(CFLAGS) $(ADD_LIB) $(SOURCE)
	ar rc $@ $(OBJECTS)
	ranlib $@
	cp $@ lib$@

viewer_app: viewer.a 
	$(GCC) draw.c draw.h viewer.a -o viewer.o $^ $(GTKFLAGS) $(GIFFLAGS)
	./viewer.o

tests: ${TESTS_SOURCE} viewer.a
	${GCC} $(LDFLAGS) -o test.o $^ -lcheck
	# CK_FORK=no leaks --atExit -- ./test.o
	./test.o

gcov_report: clean lcov ${SOURCE}
	$(GCC) --coverage ${SOURCE} test.c -o s21_test -lcheck
	./s21_test
	lcov -t "s21_test" -o s21_test.info -c -d .
	genhtml -o report s21_test.info
	$(OPEN_CMD) ./report/index.html

clean:
	-rm -rf *.o && rm -rf *.gcno
	-rm -rf *.a && rm -rf *.gcda
	-rm -rf *.gch && rm -rf *.out
	-rm -rf *.info && rm -rf *.gcov
	-rm -rf ./test && rm -rf ./gcov_report
	-rm -rf ./report/
	-rm -rf s21_test
	-rm -rf *.cfg && rm -rf *.py
	-rm -rf ../smart_calc_1_0.tgz

.PHONY: all clean lcov