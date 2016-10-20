.PHONY : fclean clean test run testrun

BINDIR := bin
MYSHELL_SRC := $(wildcard src/*.c)
MYSHELL_OBJ := $(MYSHELL_SRC:.c=.o)
TESTDIR := test
TEST_SRC := $(wildcard $(TESTDIR)/*.c)
TEST_OBJ := $(TEST_SRC:.c=.o)
CFLAG := -Wall -O3 -D NDEBUG

MYSHELL := $(BINDIR)/myshell
TEST := $(BINDIR)/test
LOOPF := $(BINDIR)/loopf
FOREVER := $(BINDIR)/forever
NESTED := $(BINDIR)/nested
INPROC := $(BINDIR)/inproc
OUTPROC := $(BINDIR)/outproc
AUX := $(LOOPF) $(FOREVER) $(NESTED) $(INPROC) $(OUTPROC)

myshell : $(MYSHELL)
test : $(TEST)
aux : $(AUX)

$(LOOPF) : $(TESTDIR)/test_aux/loopever.c
	$(CC) -o $@ $^ -D FOREVER=0
	chmod +x $@

$(FOREVER) : $(TESTDIR)/test_aux/loopever.c
	$(CC) -o $@ $^ -D FOREVER=1
	chmod +x $@

$(NESTED) : $(TESTDIR)/test_aux/nested.c
	$(CC) -o $@ $^
	chmod +x $@

$(INPROC) : $(TESTDIR)/test_aux/inProc.c
	$(CC) -o $@ $^
	chmod +x $@

$(OUTPROC) : $(TESTDIR)/test_aux/outProc.c
	$(CC) -o $@ $^
	chmod +x $@

$(MYSHELL) : $(MYSHELL_OBJ) | $(BINDIR)
	$(CC) -o $@ $^ $(CFLAG)
	chmod +x $@

$(TEST) : $(TEST_OBJ) | $(BINDIR)
	$(CC) -o $@ $^ $(CFLAG)
	chmod +x $@

$(BINDIR) :
	mkdir $@

%.o : %.c
	$(CC) -c -o $@ $^ $(CFLAG)

run : $(MYSHELL)
	./$(MYSHELL)

testrun: test aux myshell | $(TESTDIR)/test.in
	./$(TEST)

clean :
	$(RM) $(MYSHELL_OBJ) $(TEST_OBJ) $(AUX) $(TESTDIR)/test.out *.o

fclean : clean
	$(RM) $(MYSHELL) $(TEST)
