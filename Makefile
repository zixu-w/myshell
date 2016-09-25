.PHONY : clean test run testrun

BINDIR := bin
MYSHELL_SRC := $(wildcard src/*.c)
MYSHELL_OBJ := $(MYSHELL_SRC:.c=.o)
TESTDIR := test
TEST_SRC := $(wildcard $(TESTDIR)/*.c)
TEST_OUT := $(wildcard $(TESTDIR)/*.out)
TEST_OBJ := $(TEST_SRC:.c=.o)
CFLAG := -ggdb

MYSHELL := $(BINDIR)/myshell
TEST := $(BINDIR)/test

myshell : $(MYSHELL)
test : $(TEST)

$(MYSHELL) : $(MYSHELL_OBJ) | $(BINDIR)
	$(CC) -o $@ $^ $(CFLAG)

$(TEST) : $(TEST_OBJ) | $(BINDIR)
	$(CC) -o $@ $^ $(CFLAG)

$(BINDIR) :
	mkdir $@

%.o : %.c
	$(CC) -c -o $@ $^ $(CFLAG)

run : $(MYSHELL)
	./$(MYSHELL)

testrun: test | $(TESTDIR)/test.in
	./$(TEST)

clean :
	$(RM) $(MYSHELL_OBJ) $(TEST_OBJ) $(TEST_OUT) *.o

fclean : clean
	$(RM) $(MYSHELL) $(TEST)
