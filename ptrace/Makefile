OBJS = exp_countme exp_capstone exp_syscall exp_no_more_traps
LIBNAME = capstone

all: $(OBJS)

exp_capstone: exp_capstone.o 
	gcc $< -Wall -l$(LIBNAME) -o $@

%: %.o 
	gcc -o $@ $^

%.o: %.c
	gcc -c -g -o $@ $^

clean:
	rm -f *.o $(OBJS)
