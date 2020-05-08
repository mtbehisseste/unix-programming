EXE_NAME = sandbox
SAMPLE = sample1 sample2
TARGETS = main.o
LIB = sandbox.so

all: exe lib

exe: $(TARGETS) 
	gcc -o $(EXE_NAME) $^

lib: $(LIB)

%.o: %.c
	gcc -o $@ -c -g -Wall $<

%.so: %.c
	gcc -o $@ --shared -fPIC $< -ldl

test: test_normal test_large 
	./$(EXE_NAME) ./sample1 || true 
	./$(EXE_NAME) ./sample2 || true

# regular version of sample
test_normal:
	gcc sample.c -o sample1

# enable large-file-support feature of sample
test_large:
	gcc sample.c -o sample2 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

clean:
	rm -f $(TARGETS) $(SAMPLE) $(EXE_NAME) $(LIB)
