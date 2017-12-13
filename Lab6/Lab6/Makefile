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
	#gcc  -shared -fPIC -g -o  libmem.so mem.c
	gcc -shared -fPIC -ldl -O2 -o libmem.so mem.c /lib/x86_64-linux-gnu/libdl-2.23.so
ddfile:
	dd if=/dev/zero of=ddfile bs=512 count=200000
gzip:
	LD_PRELOAD=./libmem.so gzip -k ddfile	
gunzip:
	LD_PRELOAD=./libmem.so gunzip -k ddfile.gz
	