lab6: main.o mem.o mem_debug.o
	gcc -g main.o mem.o mem_debug.c mem.h -o lab6
main.o: main.c
	gcc -c main.c
mem.o: mem.c
	gcc -c mem.c
mem_debug.o: mem_debug.c
	gcc -c mem_debug.c
clean:
	rm main.o mem.o mem_debug.o lab6 libmem.so
libmem.so:
	gcc -shared -fPIC -g -o libmem.so mem.c
ddfile:
	dd if=/dev/random of=ddfile bs=512 count=200000
gzip:
	LD_PRELOAD=./libmem.so gzip ddfile
	