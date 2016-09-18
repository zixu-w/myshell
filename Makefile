.PHONY : clean test run

BINDIR := bin
MYSHELL_SRC := $(wildcard src/*.c)
MYSHELL_OBJ := $(MYSHELL_SRC:.c=.o)
TESTDIR := test
TEST_SRC := $(TESTDIR/*.c)
TEST_OBJ := $(TEST_SRC:.c=.o)

MYSHELL := $(BINDIR)/myshell
TEST := $(TESTDIR)/test

myshell : $(MYSHELL)
test : $(TEST)

$(MYSHELL) : $(MYSHELL_OBJ) | $(BINDIR)
	$(CC) -o $@ $^

$(TEST) : $(TEST_OBJ) | $(BINDIR) $(TESTDIR)/test.in
	$(CC) -o $@ $^; ./$@ <$(TESTDIR)/test.in >$(TESTDIR)/test.out

$(BINDIR) :
	mkdir $@

%.o : %.c
	$(CC) -c -o $@ $^

run : $(MYSHELL)
	./$(MYSHELL)

clean :
	$(RM) $(MYSHELL_OBJ) $(TEST_OBJ)

fclean : clean
	$(RM) $(MYSHELL) $(TEST)
