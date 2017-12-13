#include "mem.h"
int main(void) {
	heap_init(1);
	void *ptr0 = _malloc(1024);
	void *ptr1 = _malloc(1024);
	void *ptr2 = _malloc(1024);
	void *ptr3 = _malloc(1024);
	void *ptr4 = _malloc(1024);
	puts("malloc: ");
	memalloc_debug_heap(stdout, HEAP_START);
	_free(ptr1);
	_free(ptr0);
	_free(ptr4);
	_free(ptr3);
	_free(ptr2);
	puts("free: ");
	memalloc_debug_heap(stdout, HEAP_START);
	return 0;
}