.PHONY : clean test run

BINDIR = bin
TESTDIR = test
compile = gcc -o $(BINDIR)/$@ $<

vpath %.c src
vpath %.h src
vpath %.in test

myshell : myshell.c
	@$(compile)

test : test.c test.in
	@$(compile); ./$(BINDIR)/$@ <$(TESTDIR)/test.in >$(TESTDIR)/test.out

run : myshell
	@./$(BINDIR)/myshell

clean :
	@rm -rf $(BINDIR)/*
