###################################################################

.SUFFIXES: .h .o .c

CCMAIN = main.c
CC = gcc
CCOPTS = -g
LIBS = -lm
OBJS = main.o memwatch.o utilfunc.o
CCEXEC = lyrebird
MEM = -DMEMWATCH -DMEMWATCH_STDIO


all:	$(CCEXEC)

$(CCEXEC):	$(OBJS) makefile 
	@echo Linking $@ . . .
	$(CC) $(CCOPTS) $(OBJS) -o  $@ $(LIBS)

%.o:	%.c
	@echo Compiling $< . . .
	$(CC) -c -std=gnu99 $(MEM) $(CCOPTS) $<

run:	all
	./$(CCEXEC)

clean:
	rm -f $(OBJS)
	rm -f $(CCEXEC)
	rm -f memwatch.log
	rm -f core
