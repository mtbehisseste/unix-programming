EXE_NAME = hw1
OBJS = main.o handler.o

all: $(OBJS)
	gcc -o $(EXE_NAME) -Wall $^

%.o: %.c
	gcc -c -g -o $@ $<

# print the sample `netstat -nap` command for testing 
sample:
	netstat -nap -t -u
	
sample_tcp:
	netstat -nap -t
	
sample_udp:
	netstat -nap -u

clean:
	rm *.o $(EXE_NAME)
