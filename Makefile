GCC = gcc
CFLAGS = -c #-Wall -Werror -Wextra -std=c11
LDFLAGS = #-O2 -Wall -Werror -Wextra -std=c11
SOURCE = struct.c filereader.c
HEADER = struct.h filereader.h
OBJECTS = $(patsubst %.c,%.o, ${SOURCE})
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

all: clean smartcalc

dvi:
	open dvi.html

dist:
	rm -rf smart_calc_1_0/
	mkdir smart_calc_1_0/
	mkdir smart_calc_1_0/src
	cp Makefile *.c *.h smart_calc_1_0/src/
	# cp readme.tex readme.pdf smart_calc_1_0/src/	
	tar cvzf ../smart_calc_1_0.tgz smart_calc_1_0/
	rm -rf smart_calc_1_0/

install:
ifeq ($(OS), Linux)
	sudo apt update
	sudo apt-get install libgtk-3-dev
else
	brew install gtk+3
endif

uninstall:
ifeq ($(OS), Linux)
	sudo apt-get purge libgtk-3-dev
else
	brew uninstall gtk+3
	
endif

viewer.a: ${SOURCE}
	$(CC) $(CFLAGS) $(ADD_LIB) $(SOURCE)
	ar rc $@ $(OBJECTS)
	ranlib $@
	cp $@ lib$@

viewer_app: viewer.a 
	$(GCC) draw.c draw.h viewer.a -o viewer.o $^ `pkg-config --libs --cflags gtk+-3.0`
	./viewer.o

tests: ${TESTS_SOURCE} viewer.a
	${CC} $(LDFLAGS) -o test.o $^ -lcheck
	./test.o

gcov_report: clean lcov ${SOURCE}
	$(CC) --coverage ${SOURCE} test.c -o s21_test -lcheck
	./s21_test
	lcov -t "s21_test" -o s21_test.info -c -d .
	genhtml -o report s21_test.info
	$(OPEN_CMD) ./report/index.html

check:
	-rm -rf cpplint.py CPPLINT.cfg
	-ln -s ../materials/linters/cpplint.py
	-ln -s ../materials/linters/CPPLINT.cfg
	-python3 cpplint.py --extensions=c *.c *.h
	cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

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

.PHONY: all clean check lcov