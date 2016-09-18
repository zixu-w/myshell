.PHONY : clean test run

BINDIR := bin
SRCDIR := src
TESTDIR := test

MYSHELL := $(BINDIR)/myshell
TEST := $(BINDIR)/test

myshell : $(MYSHELL)
test : $(TEST)

$(MYSHELL) : $(SRCDIR)/myshell.c | $(BINDIR)
	$(CC) -o $@ $<

$(TEST) : $(SRCDIR)/test.c $(TESTDIR)/test.in | $(BINDIR)
	$(CC) -o $@ $<; ./$@ <$(TESTDIR)/test.in >$(TESTDIR)/test.out

$(BINDIR) :
	mkdir $@

run : $(MYSHELL)
	./$(MYSHELL)

clean :
	$(RM) $(BINDIR)/*
